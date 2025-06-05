#include "SWTrigger.h"

SWTrigger_args::SWTrigger_args():Thread_args(){}
SWTrigger_args::~SWTrigger_args(){}


SWTrigger::SWTrigger():Tool(){}

bool SWTrigger::Initialise(std::string configfile, DataModel &data){
	
	m_configfile=configfile;
	InitialiseTool(data);
	
	// make a thread to handle sending the alerts
	m_util=new Utilities();
	thread_args=new SWTrigger_args();
	thread_args->parent = this;
	thread_args->running=false;
	m_util->CreateThread("test", &Thread, thread_args);
	
	last_running=false;
	
	return true;
}

void SWTrigger::ReinitSequence(){
	return;
}


bool SWTrigger::Execute(){
	
	if(!m_data->running){
		thread_args->running=false;
		/* termination alert not relevant for Software Trigger alerts
		if(last_running){
			// send trailing alert to prevent MPMTs flashing when their coarse counters loop
			std::string json = p.BuildMPMTJson(0,sequence_repetitions, 
			                   thread_args->last_beamspill_counts, thread_args->last_flash_counts);
			Log("Sending terminating LED alert to MPMTs with content '"+json+"'",v_debug,p.m_verbose);
			bool ok = p.m_data->services->AlertSend("LED", json);
			if(!ok){
				std::cerr<<"LED::Execute AlertSend returned false! "<<std::endl;
				return; // FIXME keep trying?
			}
		}
		*/
		last_running=false;
	} else if(!last_running && m_data->SWTrigger_trigger){
		// ok so this fires if m_data->running is true and last_running is false
		// the m_data->SWTrigger_trigger is an additional check if we're enabling SWTrigger flashes in this run
		// i.e. on transition to running. BUT, m_data->running goes true 1 min before
		// the run actually starts, and the coarse_counter is not valid until this time
		// so we need to check if the run has really started
		if(boost::posix_time::microsec_clock::universal_time() > m_data->start_time){
			last_running=true;
			thread_args->running=true;
		}
	}
	
	if(m_data->change_config){
		thread_args->running=false;
		InitialiseConfiguration(m_configfile);
		LoadConfig();
		ExportConfiguration();
	}
	
	return true;
}


bool SWTrigger::Finalise(){
	
	m_util->KillThread(thread_args);
	
	delete thread_args;
	thread_args=0;
	
	delete m_util;
	m_util=0;
	
	return true;
}

// will be repeatedly called by Utilities
void SWTrigger::Thread(Thread_args* arg){
	
	SWTrigger_args* m_args=reinterpret_cast<SWTrigger_args*>(arg);
	
	// get reference to parent Tool so we can access its members
	// just be careful not to access members simultaneously
	// from both thread and parent (use mutexes when necessary)
	// should be fine as our parent tool does nothing after spinning up
	// this thread other than stop it.
	SWTrigger& p = *(m_args->parent);
	
	// printout debuggers
	static bool wait_for_beam=true;
	static bool wait_beamdelay = true;
	static bool wait_flashdelay=true;
	
	usleep(10);
	if(!m_args->running){
		m_args->last_running=false;
		sleep(1);
		return;
	}
	
	if(!m_args->last_running){
		// reset thread locals on transition to running
		
		m_args->sequence_num = 0;
		m_args->sequences_performed = 0;
		// reset sequence to start from the beginning
		
		m_args->last_beam_active = false;
		m_args->last_beamspill_end = boost::posix_time::microsec_clock::universal_time();
		m_args->last_beamspill_counts = p.m_data->current_coarse_counter;
		m_args->spills_since_last_flash=0;
		// variables used in tracking beam when running flashing between spills
		
		m_args->last_flash = boost::posix_time::microsec_clock::universal_time() - p.alert_prewarn;
		// UTC of last flash. Flash Alerts will be sent out at the sequence period,
		// with this effectively defining the time of the first alert in the sequence.
		
		m_args->last_flash_counts = p.m_data->current_coarse_counter 
		                          + (double(p.alert_prewarn.total_microseconds())*8E-3)
		                          - p.SWTrigger_advance_counts
		                          + p.bens_offset;
		// coarse counts of last flash. Within Alerts flash times are specified in coarse counts.
		// Alert flash times are set to one sequence period in coarse counts since the last.
		// This ensures each alert contains a StartTime exactly one sequence period counts after the last,
		// so network latency & thread loop delays won't introduce jitter into the sequence timing.
		// if we had just sent the last alert now, the coarse counts of last flashing would have been: now + alert_prewarn in counts...
		// And then we have SWTrigger_advance_counts to account for MPMT firmware issues, added to all Alert StartTime values
		// And then we have ben's offset to account for differences between DAQ and MPMT coarse counters (" " ")
		
		std::cout<<"Starting SWTrigger Tool thread"<<std::endl;
	}
	m_args->last_running=true;
	
	// if interleaving with beam
	if(p.spill_modulus!=0){
		
		// just in case ben doesn't put a timestamp of when the beam spill ended in the datamodel, track it ourselves
		if(m_args->last_beam_active && !p.m_data->beam_active){
			m_args->last_beamspill_end  = boost::posix_time::microsec_clock::universal_time();
			m_args->last_beamspill_counts = p.m_data->current_coarse_counter;
			++m_args->spills_since_last_flash;
			//p.Log("Noting beam spill",v_debug+10,p.m_verbose);
			std::cout<<"SWTrigger noting beamspill"<<std::endl;
		}
		m_args->last_beam_active = p.m_data->beam_active;
		// for when he does
		//m_args->last_beamspill_end = m_args->m_data->last_beamspill_end;
		
		// if only flashing every Nth beam spill, check if enough beamspills have passed since last sequence
		if(m_args->spills_since_last_flash<p.spill_modulus){
			//p.Log("Waiting for next beam spill...",v_debug+10,p.m_verbose);
			if(wait_for_beam){
				std::cout<<"SWTrigger waiting for next beam spill"<<std::endl;
				wait_for_beam=false;
			}
			return;
		}
		
		// sanity check
		if(m_args->spills_since_last_flash>p.spill_modulus){
			//p.Log("ERROR! Spills since last flash ("+std::to_string(spills_since_last_flash)
			//   +") > spill modulus ("+std::to_string(p.spill_modulus)+")!"
			//   " Possible spill during flash sequence!",v_error,p.m_verbose);
		}
		
		// calculate time since last beamspill end signal
		m_args->lapse = boost::posix_time::microsec_clock::universal_time() - m_args->last_beamspill_end;
		if(m_args->lapse < (p.delay_after_beam - p.alert_prewarn)){
			//p.Log("Waiting DelayToStart after beam",v_debug+10,p.m_verbose);
			if(wait_beamdelay){
				std::cout<<"SWTrigger waiting for delay after beam"<<std::endl;
				wait_beamdelay=false;
			}
			return;
		}
	}
	
	// calculate time since last flash
	m_args->lapse =  boost::posix_time::microsec_clock::universal_time() - m_args->last_flash;
	if(m_args->lapse < p.sequence_period){
		//p.Log("Waiting sequence period after last flash",v_debug+10,m_verbose);
		if(wait_flashdelay){
			//std::cout<<"SWTrigger waiting for time to next flash"<<std::endl;
			wait_flashdelay=false;
		}
		return;
	}
	
	// ok send next alert
	std::string json = p.BuildMPMTJson(m_args->sequence_num, m_args->sequences_performed, m_args->last_beamspill_counts, m_args->last_flash_counts);
	//std::cout<<"SWTrigger sending alert at coarse_counts "<<p.m_data->current_coarse_counter<<" with content '"<<json<<"'"<<std::endl;
	//p.Log("Sending alert for SWTrigger flash repetition "+std::to_string(sequences_performed)
	//   +", sequence "+std::to_string(sequence_num)+" to MPMTs",v_debug,p.m_verbose);
	bool ok = p.m_data->services->AlertSend("SoftTrigger", json);
	if(!ok){
		std::cerr<<"SWTrigger::Thread AlertSend returned false! "<<std::endl;
		return; // this could flood with errors if we keep failing...
		// FIXME add a rate-limiting mechanism? move to next flash in sequence? abort sequence? depends on error...
	}
	
	// update time of last flash
	m_args->last_flash = boost::posix_time::microsec_clock::universal_time();
	
	// move to next flash in sequence
	++m_args->sequence_num;
	
	// if we've reached the end of the sequence, go back to the start
	if(m_args->sequence_num == p.firing_sequence.size()){
		m_args->sequence_num=0;
		++m_args->sequences_performed;
	}
	
	// if we're done all the sequence repetitions, go back to the beginning
	if(m_args->sequences_performed==p.sequence_repetitions){
		//m_args->running = false; // debug: uncomment to only do a single flash sequence
		m_args->sequence_num=0;
		m_args->sequences_performed=0;
		// and reset number of spills since last flash to start waiting for next beam
		m_args->spills_since_last_flash=0;
	}
	
	wait_for_beam=true;
	wait_beamdelay=true;
	wait_flashdelay=true;
	//std::cout<<"SWTrigger end loop"<<std::endl;
	
	return;
}

bool SWTrigger::LoadConfig(){
	
	if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
	
	// SWTrigger flash settings
	// ==================
	if(!LoadSequence()) return false;
	
	// 'alert_prewarn' is how far in advance to send an Alert before the flash is specified to happen.
	int alert_prewarn_us = 20;
	m_variables.Get("alert_prewarn_us",alert_prewarn_us);
	
	/*
	// There are some restrictions on this:
	// 1. The prewarn + entire flash sequence duration needs to be less than the notice we get of beam spills (0.2s)
	//    This prevents us sending a flash alert, only to find a beam spill will happen while we're flashing.
	//    (To allow for delays in propagating beam spill notice to this tool we limit to 1/10th of this: 20ms)
	// 2. It needs to be less than the 'WaitToStart', which represents the deadtime between subsequent StartTimes
	//    (total time between subsequent StartTime values is (NumFlashes*FlashInterval) + WaitToStart counts)
	//    This ensures the alert for the next flash sequence doesn't clobber a currently ongoing flash sequence.
	//  Let's check these in turn
	
	// 1.
	int NumFlashes, FlashInterval, WaitToStart;
	firing_sequence.front().Get("NumFlashes",NumFlashes);
	firing_sequence.front().Get("FlashInterval",FlashInterval);
	firing_sequence.front().Get("WaitToStart",WaitToStart);
	// Note that we perform the entire sequence SequenceRepetitions times before the next beam spill.
	// (although technically we could abort the sequence part-way if a spill notice came, avoiding such a situation is better)
	int TotalSequenceDurationTicks = sequence_repetitions*firing_sequence.size()*(WaitToStart+(NumFlashes*FlashInterval));
	double TotalSequenceDuration = double(TotalSequenceDurationTicks) * 8E-3;
	double max_prewarn_us_beam = 2E4 - TotalSequenceDuration; // *** see below for caveat
	
	// 2.
	double max_prewarn_us_seq = double(WaitToStart) * 8E-3;
	
	// since these are upper limits take lower of two
	double max_prewarn_us = std::min(max_prewarn_us_beam, max_prewarn_us_seq);
	
	std::cout<<"max prewarn us is "<<max_prewarn_us<<std::endl;
	
	if((spill_modulus!=0) && (alert_prewarn_us > max_prewarn_us)){
		std::string msg = "SWTrigger::LoadConfig Warning! Time between MPMT alert and requested flash (alert_prewarn_us: "
		                +  std::to_string(alert_prewarn_us) + ") is too long! Coercing to "+std::to_string(max_prewarn_us);
		Log(msg,v_warning,m_verbose);
		// TODO check this works....
		alert_prewarn_us=max_prewarn_us;
	}
	// ------------------------
	
	// the minimum prewarn time is effectively the sum of network latency and thread processing delay
	// i.e. if the thread loop runs every 1us and network delay is 1us, we need to send the alert at least 2us
	// before to guarantee the StartTime requested is still in the future when the Alert arrives *** caveat below
	double min_prewarn_us = 100; // ¯\_(ツ)_/¯
	
	if(alert_prewarn_us < min_prewarn_us){
		std::string msg = "SWTrigger::LoadConfig Warning! Time between MPMT alert and requested flash (alert_prewarn_us: "
		                +  std::to_string(alert_prewarn_us) + ") is too short! Coercing to "+std::to_string(min_prewarn_us);
		Log(msg,v_warning,m_verbose);
		alert_prewarn_us=min_prewarn_us;
	}
	
	*/
	
	alert_prewarn = boost::posix_time::microseconds(alert_prewarn_us);
	
	// another wrinkle: MPMT UTC clocks may differ to that of the DAQ by .. apparently a lot more than makes sense.
	// Since a run start is defined by a given UTC, the MPMTs may start the run (and reset their coarse counters)
	// at a different time to the DAQ. They will then have coarse counter values that are offset to that of the DAQ.
	// This offest is unknown and may vary from MPMT to MPMT...
	// 
	// If the MPMT clock is fast, it will start its run early and the coarse counter will be higher than ours,
	// and a requested coarse counter value that is in the future for us may already be in the past for the MPMT.
	// Conversely, if the MPMT clock is slow, our request to flash 1ms from now may be interpreted as a request to flash
	// 1001ms from now. This becomes a problem if our sequence period is such that the MPMT receives the next Alert
	// with a new StartTime overwriting the old value, such that it never reaches the designated time and flashes.
	
	// We have a few ways to address this
	// 1. Align our UTCs to sufficiently high precision before run start (is NTP that accurate? how accurate do we need*?)
	// 2. Offset the coarse counter values we send to the MPMTs, to account for the difference
	// 3. Offset when we send our alerts, so that alerts arrive at the appropriate time relative to the MPMT's coarse counter.
	//
	// How accurately do we need to do this?
	// Well, our requested coarse counts need to be in the future, for starters.
	// But we can't push the requested times arbitrarily far off or we'll clobber them with subsequent alerts.
	// Specifically the requested StartTime can't be more than (WaitToStart - alert_prewarn) counts in the future
	// (from the perspective of the MPMT), otherwise it'll still be flashing when the next alert comes in.
	// So the difference between alert arrival time and requested StartTime in the MPMT's coarse counter
	// must be > 0 and < (WaitToStart - alert_prewarn_counts).
	//
	// A +-1s UTC difference means an offset of anywhere between -125,000,000 to +125,000,000.
	// To account for a possible -1s offset we would need to add 125,000,000 counts to requested StartTime values.
	// But if the MPMT clock was actually running 1s slow, the requested StartTime would now be 250,000,000 in the future.
	// To ensure this is < (WaitToStart - alert_prewarn_counts), we must have (WaitToStart-alert_prewarn) > 250,000,000.
	// This means a minimum sequence period (time between alerts) of 2s+(NumFlashes*FlashInterval)!
	
	// tl;dr - We need to account for the difference between MPMT and DAQ clocks / coarse counters... so add an offset. :)
	bens_offset = 0;
	m_variables.Get("bens_offset",bens_offset);
	
	// Ultimately, alert_prewarn shifts when alerts are sent, while bens_offset shifts the contained coarse count values.
	// Both can address the issue, what matters is that at the alert arrival time in MPMT UTC, the contained StartTime value
	// aligns with the MPMT coarse counter to within ~[0 - WaitToStart] counts.
	
	// *** minimum/maximum prewarn limits assumed that the MPMT coarse counts aligned with the DAQ ones.
	// In reality these limits should also account for the maximum possible offset between DAQ and MPMT coarse counters.
	// e.g. to avoid overlap with beam: (time between alert send and MPMT flash sequence start) + flashing duration must be < 0.2s
	// The first term here is not just WaitToStart, but WaitToStart + max_positive_error_on_coarse_counter_estimate
	
	std::cout<<"SWTrigger Config Loaded"<<std::endl;
	
	return true;
	
}



bool SWTrigger::LoadSequence(){
	
	std::string SWTrigger_variables_json;
	if(!m_data->services->GetRunDeviceConfig(SWTrigger_variables_json, m_data->run_configuration, "SWTrigger")){
		Log("LoadSequence Error! Failed to get SWTrigger config for run configuration "+std::to_string(m_data->run_configuration),v_error,m_verbose);
		return false;
	}
	
	Store SWTrigger_variables;
	SWTrigger_variables.JsonParser(SWTrigger_variables_json);
	
	/////////////////////////////////////////////////////////////
	
	unsigned int waveform_presamples;
	unsigned int waveform_samples;
	std::string sequence_json;
	std::string settings_json;
	int delay_after_beam_s;
	
	
	
	// SWTrigger flasher settings
	// ==========================
	SWTrigger_run_type="normal";
	SWTrigger_variables.Get("RunType",SWTrigger_run_type);
	if(SWTrigger_run_type=="beam"){
		SWTrigger_variables.Get("StartDelay", delay_after_beam_s);
		SWTrigger_variables.Get("SpillModulus", spill_modulus);
	} else {
		delay_after_beam_s=0;
		spill_modulus=0;
	}
	delay_after_beam=boost::posix_time::seconds(delay_after_beam_s);
	delay_after_beam_counts = delay_after_beam_s*125E6;
	sequence_repetitions=0;
	SWTrigger_variables.Get("SequenceRepetitions",sequence_repetitions);
	++sequence_repetitions;
	SWTrigger_variables.Get("SamplingStart",waveform_presamples);             // XXX put in DataModel
	SWTrigger_variables.Get("WaveformLength",waveform_samples);               // XXX put in DataModel
	SWTrigger_variables.Get("SWTriggerAdvance",SWTrigger_advance_counts);     // XXX put in DataModel
	SWTrigger_variables.Get("SWTriggerSettings",settings_json);
	/*
	SWTrigger_variables.Get("SWTriggerSequence",sequence_json);
	
	Store sequence_store;
	sequence_store.JsonParser(sequence_json);
	
	std::vector<std::string> sequence_vector;
	bool ok = sequence_store.Get("sequence", sequence_vector);
	// TODO above should work but does not, uncomment the below once it's fixed
	if(!ok){
		std::cerr<<"you can't get json arrays into a vector, silly!"<<std::endl;
		size_t next_char=0;
		// trim leading whitespace
		while((sequence_json.length()>next_char) && std::isspace(sequence_json.at(next_char))) ++next_char;
		// skip over leading '['
		++next_char;
		// loop over internal objects
		size_t next_begin;
		for(; next_char<sequence_json.length(); ++next_char){
			if(sequence_json.at(next_char)=='{') next_begin = next_char;
			if(sequence_json.at(next_char)=='}') sequence_vector.push_back(sequence_json.substr(next_begin,next_char-next_begin+1));
		}
	}
	
	// firing_sequence is the vector of settings to be sent to the MPMTs
	// keep it as a Store so that we can update the StartTime before
	// converting to JSON to send out
	firing_sequence.resize(sequence_vector.size());
	
	// everything in SWTriggerSettings is common to all flashes in the sequence
	// and needs to go into the alert sent to the MPMTs
	Store alert_store;
	alert_store.JsonParser(settings_json);
	
	alert_store.Get("WaitToStart",wait_to_start);
	int num_flashes, flash_interval;
	alert_store.Get("NumFlashes",num_flashes);
	alert_store.Get("FlashInterval",flash_interval);
	sequence_period_counts = wait_to_start+(num_flashes*flash_interval);
	std::cout<<"doing "<<num_flashes<<" flashes separated by "<<flash_interval<<" ticks, after initial delay of "<<wait_to_start<<" ticks, giving total "<<sequence_period_counts<<" ticks or "<<(sequence_period_counts*8)<<"ns between flash sequences"<<std::endl;
	if((double(sequence_period_counts)*8)<1000){
		Log("Error! SWTrigger Sequence period "+std::to_string(sequence_period_counts)+" is too short to be timed with us clock!",v_error,m_verbose);
		return false;
	}
	// XXX is this strict enough of a check to ensure we send out alerts in time for the StartCount?
	sequence_period = boost::posix_time::microseconds(double(sequence_period_counts)*8E-3);
	std::cout<<"sequence period is "<<sequence_period<<std::endl;
	
	// SWTriggerSequence contains the specific settings for each flash in the sequence.
	// These need to be merged with the common settings to form the alert for each flash
	// alert_store.Remove("WaitToStart"); // WaitToStart does not get propagated to the MPMTs
	// FIXME Add Remove method to remove a key from Store. replace tmp_store with above once done.
	// for now make a new store and copy elements we need across
	Store tmp_store;
	tmp_store.Set("NumFlashes",num_flashes);
	tmp_store.Set("FlashInterval",flash_interval);
	
	for(int i=0; i<sequence_vector.size(); i++){
		
		// Store::JsonParser will add/override variables in the given JSON,
		// while retaining any others that already exist in the Store
		//alert_store.JsonParser(sequence_vector.at(i));
		tmp_store.JsonParser(sequence_vector.at(i));
		
		firing_sequence.at(i) = tmp_store; //alert_store;
		
		// the following defines what gets sent to the MPMTs in the alert:
		// MPMTID: which MPMT will flash, from SWTriggerSequence
		// SWTrigger: 0-7 (3-bit bitmask of SWTriggers on the MPMT), from SWTriggerSequence
		// Gain: 0 or 1 (1 is a significant bump to SWTrigger intensity), from SWTriggerSequence
		// DACSetting: 0-1023 (fine control of SWTrigger intensity), from SWTriggerSequence
		// StartTime: coarse counter of when to start flashing*, added later
		// NumFlashes: 1-65536, from SWTriggerSettings
		// FlashInterval: 1-65536 (in clock ticks), from SWTriggerSettings
		// Type: 0=software trigger, 1=ext trigger, from SWTriggerSettings // FIXME missing from documentation
		
	}
	
	*/
	
	// Alert should have variables:
	// MPMTID - as per LED
	// StartTime - as per LED, handled by BuildMPMTJson
	// NumWindows - equivalent to NumFlashes
	// WindowInterval - equivalent to FlashInterval
	// Type - 0? maybe not needed
	
	int num_flashes, flash_interval;
	SWTrigger_variables.Get("NumFlashes",num_flashes);
	SWTrigger_variables.Get("FlashInterval",flash_interval);
	sequence_period_counts = wait_to_start+(num_flashes*flash_interval);
	std::cout<<"doing "<<num_flashes<<" flashes separated by "<<flash_interval<<" ticks, after initial delay of "<<wait_to_start<<" ticks, giving total "<<sequence_period_counts<<" ticks or "<<(sequence_period_counts*8)<<"ns between flash sequences"<<std::endl;
	if((double(sequence_period_counts)*8)<1000){
		Log("Error! SWTrigger Sequence period "+std::to_string(sequence_period_counts)+" is too short to be timed with us clock!",v_error,m_verbose);
		return false;
	}
	// XXX is this strict enough of a check to ensure we send out alerts in time for the StartCount?
	sequence_period = boost::posix_time::microseconds(double(sequence_period_counts)*8E-3);
	std::cout<<"sequence period is "<<sequence_period<<std::endl;
	
	int num_flashes, flash_interval, mpmtID;
	mpmtID=0;
	Store alert_store;
	alert_store.Set("NumWindows",num_flashes);
	alert_store.Set("WindowInterval",flash_interval);
	alert_store.Set("MPMTID",mpmtID)
	firing_sequence.at(i) = alert_store;
	
	
	
	sequence_period_counts = // must set this as counts between StartTime values!
	sequence_period = // must be same as boost object
	Store tmpStore;
	// XXX 
	firing_sequence.push_back(tmpStore);
	
	
	
	
	
	
	
	

	
	
	////////////////////////////////////////////////////////////////
	/*
	std::string SWTrigger_run_type;
	unsigned int waveform_presamples;
	unsigned int waveform_samples;
	std::string sequence_json;
	std::string settings_json;
	int delay_after_beam_s;
	
	// SWTrigger flasher settings
	// ====================
	SWTrigger_variables.Get("RunType",SWTrigger_run_type);
	if(SWTrigger_run_type=="beam"){
		SWTrigger_variables.Get("StartDelay", delay_after_beam_s);
		SWTrigger_variables.Get("SpillModulus", spill_modulus);
	} else {
		delay_after_beam_s=0;
		spill_modulus=0;
	}
	delay_after_beam=boost::posix_time::seconds(delay_after_beam_s);
	delay_after_beam_counts = delay_after_beam_s*125E6;
	SWTrigger_variables.Get("SequenceRepetitions",sequence_repetitions);
	++sequence_repetitions; // 1 means run it twice.....
	SWTrigger_variables.Get("SamplingStart",waveform_presamples);             // XXX put in DataModel
	SWTrigger_variables.Get("WaveformLength",waveform_samples);               // XXX put in DataModel
	SWTrigger_variables.Get("SWTriggerAdvance",SWTrigger_advance_counts);     // XXX put in DataModel
	SWTrigger_variables.Get("SWTriggerSettings",settings_json);
	SWTrigger_variables.Get("SWTriggerSequence",sequence_json);
	
	Store sequence_store;
	sequence_store.JsonParser(sequence_json);
	
	std::vector<std::string> sequence_vector;
	bool ok = sequence_store.Get("sequence", sequence_vector);
	// TODO above should work but does not, uncomment the below once it's fixed
	if(!ok){
		std::cerr<<"you can't get json arrays into a vector, silly!"<<std::endl;
		size_t next_char=0;
		// trim leading whitespace
		while((sequence_json.length()>next_char) && std::isspace(sequence_json.at(next_char))) ++next_char;
		// skip over leading '['
		++next_char;
		// loop over internal objects
		size_t next_begin;
		for(; next_char<sequence_json.length(); ++next_char){
			if(sequence_json.at(next_char)=='{') next_begin = next_char;
			if(sequence_json.at(next_char)=='}') sequence_vector.push_back(sequence_json.substr(next_begin,next_char-next_begin+1));
		}
	}
	
	// firing_sequence is the vector of settings to be sent to the MPMTs
	// keep it as a Store so that we can update the StartTime before
	// converting to JSON to send out
	firing_sequence.resize(sequence_vector.size());
	
	// everything in SWTriggerSettings is common to all flashes in the sequence
	// and needs to go into the alert sent to the MPMTs
	Store alert_store;
	alert_store.JsonParser(settings_json);
	
	alert_store.Get("WaitToStart",wait_to_start);
	int num_flashes, flash_interval;
	alert_store.Get("NumFlashes",num_flashes);
	alert_store.Get("FlashInterval",flash_interval);
	sequence_period_counts = wait_to_start+(num_flashes*flash_interval);
	std::cout<<"doing "<<num_flashes<<" flashes separated by "<<flash_interval<<" ticks, after initial delay of "<<wait_to_start<<" ticks, giving total "<<sequence_period_counts<<" ticks or "<<(sequence_period_counts*8)<<"ns between flash sequences"<<std::endl;
	if((double(sequence_period_counts)*8)<1000){
		Log("Error! SWTrigger Sequence period "+std::to_string(sequence_period_counts)+" is too short to be timed with us clock!",v_error,m_verbose);
		return false;
	}
	// XXX is this strict enough of a check to ensure we send out alerts in time for the StartCount?
	sequence_period = boost::posix_time::microseconds(double(sequence_period_counts)*8E-3);
	std::cout<<"sequence period is "<<sequence_period<<std::endl;
	
	// SWTriggerSequence contains the specific settings for each flash in the sequence.
	// These need to be merged with the common settings to form the alert for each flash
	// alert_store.Remove("WaitToStart"); // WaitToStart does not get propagated to the MPMTs
	// FIXME Add Remove method to remove a key from Store. replace tmp_store with above once done.
	// for now make a new store and copy elements we need across
	Store tmp_store;
	tmp_store.Set("NumFlashes",num_flashes);
	tmp_store.Set("FlashInterval",flash_interval);
	
	for(int i=0; i<sequence_vector.size(); i++){
		
		// Store::JsonParser will add/override variables in the given JSON,
		// while retaining any others that already exist in the Store
		//alert_store.JsonParser(sequence_vector.at(i));
		tmp_store.JsonParser(sequence_vector.at(i));
		
		firing_sequence.at(i) = tmp_store; //alert_store;
		
		// the following defines what gets sent to the MPMTs in the alert:
		// MPMTID: which MPMT will flash, from SWTriggerSequence
		// SWTrigger: 0-7 (3-bit bitmask of SWTriggers on the MPMT), from SWTriggerSequence
		// Gain: 0 or 1 (1 is a significant bump to SWTrigger intensity), from SWTriggerSequence
		// DACSetting: 0-1023 (fine control of SWTrigger intensity), from SWTriggerSequence
		// StartTime: coarse counter of when to start flashing*, added later
		// NumFlashes: 1-65536, from SWTriggerSettings
		// FlashInterval: 1-65536 (in clock ticks), from SWTriggerSettings
		// Type: 0=software trigger, 1=ext trigger, from SWTriggerSettings // FIXME missing from documentation
		
	}
	*/
	
	
	std::cout<<"SWTrigger Sequence loaded"<<std::endl;
	
	return true;
}

std::string SWTrigger::BuildMPMTJson(int sequence_num, int sequences_performed, unsigned long last_beamspill_counts, long unsigned int& last_flash_counts){
	
	// generate json for next alert
	// most of it's already built during LoadSequence, we just need to update the StartCounts
	std::string json;
	unsigned long StartCounts;
	
	// if real flash
	if(sequences_performed<sequence_repetitions){
		
		if(sequence_num==0 && spill_modulus>0){
			// if interleaving with beam, start time of each sequence is determined from time since last beamspill
			StartCounts = last_beamspill_counts + delay_after_beam_counts - SWTrigger_advance_counts;
		} else {
			// subsequent flash times, or all of them if not interleaving with beam,
			// are just determined from time since last flash
			// note that after each sequence there is a dead time equal to the sequence time
			StartCounts = last_flash_counts + sequence_period_counts;
		}
		
		last_flash_counts = StartCounts;
		
		firing_sequence.at(sequence_num).Set("StartTime", StartCounts);
		firing_sequence.at(sequence_num)>>json;
	} else {
		// since internal MPMT coarse counter will eventually loop, if we leave a valid
		// StartTime in the MPMT memory, it will eventually start flashing again.
		// We need to set it to a special value to prevent this happening.
		StartCounts = 0xFFFFFFFFFFFF;
		last_flash_counts = last_flash_counts + sequence_period_counts;
		
		static std::string termination_msg;
		if(termination_msg.empty()){
			// in case an alert is rejected by MPMTs if it does not contain
			// all the appropriate fields, copy the first flash alert as a base
			// (unused vars: SWTrigger, Gain, DACSetting, NumFlashes, FlashInterval, Type)
			Store termination_store = firing_sequence.at(0);
			termination_store.Set("StartTime", StartCounts);
			// Alerts sent to MPMTID 0 will be picked up by all MPMTs (robert on slack, 13/02/2025)
			termination_store.Set("MPMTID", 0);
			termination_store >> termination_msg;
		}
		json = termination_msg;
	}
	
	return json;
}

/*
// SWTrigger configuration JSON contents:
RunType: "calibration" or "beam" - redundant
StartDelay: how long to wait after end of beam spill before resuming flashing
SpillModulus: only flash every Nth beam spill
SequenceRepetitions: 0 = only perform the sequence once. 1 = do twice...etc
SamplingStart: waveform starts specified counts before SWTrigger flash
WaveformLength: number of samples in waveform
SWTriggerAdvance: debug variable required to determine time windows to extract from MPMT data*
SWTriggerSettings: an embedded JSON of...
	NumFlashes: number of times to flash the SWTrigger
	FlashInterval: time between flashes
	WaitToStart: delay in coarse counts before moving to next SWTrigger.
	These should be sent to the MPMTs along with other settings in SWTriggerSequence

**** make sure that cumulative time of NumFlashes*FlashInterval*number of set bits in SWTrigger > sequence duration

// the following defines what gets sent to the MPMTs in the alert:
MPMTID: which MPMT will flash
SWTrigger: 0-7 (3-bit bitmask of SWTriggers on the MPMT)
Gain: 0 or 1 (1 is a significant bump to SWTrigger intensity)
DACSetting: 0-1023 (fine control of SWTrigger intensity)
StartTime: coarse counter of when to start flashing*
NumFlashes: 1-65536
FlashInterval: 1-65536 (in clock ticks)
Type: 0=software trigger, 1=ext trigger

to disable flashing set future time to 0xFFFFFFFFFFFF (48bits of 1)

* due to a firmware bug at present, the SWTriggers will actually flash ~2us after the requested StartTime
this needs to be corrected as follows:
1. subtract the LEDAdvance from the actually desired flash coarse counter, and send that value as StartTime to the MPMTs
2. the MPMT will report the coarse counter on which it believed it flashed (should be close to StartTime)
3. add LEDAdvance to this time to find the actual time of the SWTrigger flash. Extract data in windows accordingly.
*/
