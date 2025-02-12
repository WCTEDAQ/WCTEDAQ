#include "LED.h"

LED_args::LED_args():Thread_args(){}
LED_args::~LED_args(){}


LED::LED():Tool(){}

bool LED::Initialise(std::string configfile, DataModel &data){
	
	m_configfile=configfile;
	InitialiseTool(data);
	
	// make a thread to handle sending the alerts
	m_util=new Utilities();
	thread_args=new LED_args();
	thread_args->parent = this;
	thread_args->running=false;
	m_util->CreateThread("test", &Thread, thread_args);
	
	last_running=false;
	
	return true;
}

void LED::ReinitSequence(){
	return;
}


bool LED::Execute(){
	
	if(!m_data->running){
		thread_args->running=false;
	} else if(!last_running){
		// ok so this would fire if m_data->running is true and last_running is false
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
		sleep(1);
		InitialiseConfiguration(m_configfile);
		LoadConfig();
		ExportConfiguration();
	}
	
	return true;
}


bool LED::Finalise(){
	
	m_util->KillThread(thread_args);
	
	delete thread_args;
	thread_args=0;
	
	delete m_util;
	m_util=0;
	
	return true;
}

// will be repeatedly called by Utilities
void LED::Thread(Thread_args* arg){
	
	LED_args* m_args=reinterpret_cast<LED_args*>(arg);
	
	// get reference to parent Tool so we can access its members
	// just be careful not to access members simultaneously
	// from both thread and parent (use mutexes when necessary)
	// should be fine as our parent tool does nothing after spinning up
	// this thread other than stop it.
	LED& p = *(m_args->parent);
	
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
		// reset counters on transition to running
		m_args->last_beam_active = false;
		m_args->last_flash = boost::posix_time::microsec_clock::universal_time() - (p.sequence_period/2.) - p.alert_prewarn;
		m_args->last_flash_counts = p.m_data->current_coarse_counter - p.led_advance_counts;
		m_args->last_beamspill_end = boost::posix_time::microsec_clock::universal_time();
		m_args->last_beamspill_counts = p.m_data->current_coarse_counter;
		m_args->spills_since_last_flash=0;
		m_args->sequence_num = 0;
		m_args->sequences_performed = 0;
		
		std::clog<<"Starting LED Tool thread"<<std::endl;
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
			std::clog<<"LED noting beamspill"<<std::endl;
		}
		m_args->last_beam_active = p.m_data->beam_active;
		// for when he does
		//m_args->last_beamspill_end = m_args->m_data->last_beamspill_end;
		
		// if only flashing every Nth beam spill, check if enough beamspills have passed since last sequence
		if(m_args->spills_since_last_flash<p.spill_modulus){
			//p.Log("Waiting for next beam spill...",v_debug+10,p.m_verbose);
			if(wait_for_beam){
				std::clog<<"LED waiting for next beam spill"<<std::endl;
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
				std::clog<<"LED waiting for delay after beam"<<std::endl;
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
			std::clog<<"LED waiting for time to next flash"<<std::endl;
			wait_flashdelay=false;
		}
		return;
	}
	
	// ok send next alert
	std::string json = p.BuildMPMTJson(m_args->sequence_num, m_args->sequences_performed, m_args->last_beamspill_counts, m_args->last_flash_counts);
	std::clog<<"LED sending alert at coarse_counts "<<p.m_data->current_coarse_counter<<" with content '"<<json<<"'"<<std::endl;
	//p.Log("Sending alert for LED flash repetition "+std::to_string(sequences_performed)
	//   +", sequence "+std::to_string(sequence_num)+" to MPMTs",v_debug,p.m_verbose);
	bool ok = p.m_data->services->AlertSend("LEDTrigger", json);
	if(!ok){
		std::cerr<<"LED::Thread AlertSend returned false! "<<std::endl;
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
	
	// after completing the sequence, we send one final alert to reset the MPMT LED registers
	// so we only have one element in this trailing pseudo-sequence
	if(m_args->sequences_performed==(p.sequence_repetitions+1)){  // XXX XXX UNCOMMENT TO ENABLE TERMINATION FLASH // TODO move termination flashes to run stop
	//if(m_args->sequences_performed==p.sequence_repetitions){
		//m_args->running = false; // XXX XXX XXX XXX XXX DEBUG XXX XXX XXX XXX ONLY DO SEQUENCE ONCE!
		m_args->sequence_num=0;
		m_args->sequences_performed=0;
		// and reset number of spills since last flash to start waiting for next beam
		m_args->spills_since_last_flash=0;
	}
	
	wait_for_beam=true;
	wait_beamdelay=true;
	wait_flashdelay=true;
	std::clog<<"LED end loop"<<std::endl;
	
	return;
}

bool LED::LoadConfig(){
	
	if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
	
	// LED flash settings
	// ==================
	if(!LoadSequence()) return false;
	
	int alert_prewarn_ms = 20;
	if(!m_variables.Get("alert_prewarn_ms",alert_prewarn_ms));
	
	// optional sanity check
	// ---------------------
	// we get notice of beam spills 200 ms in advance of spill start.
	// in order to ensure we don't flash during beam, we need to ensure that
	// the time from alert to flashing plus duration of flashing is not greater than
	// the beam start notice. To allow for delays in propagating beam spill notice
	// to this tool, we actually limit to 1/10th of this: 20ms
	// we perform the entire sequence SequenceRepetitions times BEFORE the next beam spill.
	int NumFlashes, FlashInterval, WaitToStart;
	firing_sequence.front().Get("NumFlashes",NumFlashes);
	firing_sequence.front().Get("FlashInterval",FlashInterval);
	firing_sequence.front().Get("WaitToStart",WaitToStart);
	int TotalSequenceDurationTicks = sequence_repetitions*firing_sequence.size()*(WaitToStart+(NumFlashes*FlashInterval));
	
	double min_prewarn_ms = (TotalSequenceDurationTicks * 8E-6);
	
	if((spill_modulus!=0) && (min_prewarn_ms > alert_prewarn_ms)){
		std::string msg = "LED::LoadConfig Warning! Time between MPMT alert and requested flash (alert_prewarn_ms: "
		                +  std::to_string(alert_prewarn_ms) + ") is too short!"; //" Coercing to "+std::to_string(min_prewarn_ms);
		Log(msg,v_warning,m_verbose);
		// TODO check this works with beam and enable once validated....
		//alert_prewarn_ms=min_prewarn_ms;
	}
	// ------------------------
	
	alert_prewarn = boost::posix_time::milliseconds(alert_prewarn_ms);
	
	std::clog<<"LED Config Loaded"<<std::endl;
	
	return true;
	
}



bool LED::LoadSequence(){
	
	std::string led_variables_json;
	if(!m_data->services->GetRunDeviceConfig(led_variables_json, m_data->run_configuration, "LED")){
		Log("LoadSequence Error! Failed to get LED config for run configuration "+std::to_string(m_data->run_configuration),v_error,m_verbose);
		return false;
	}
	
	Store led_variables;
	led_variables.JsonParser(led_variables_json);
	
	std::string led_run_type;
	unsigned int waveform_presamples;
	unsigned int waveform_samples;
	std::string sequence_json;
	std::string settings_json;
	int delay_after_beam_s;
	
	// LED flasher settings
	// ====================
	led_variables.Get("RunType",led_run_type);
	if(led_run_type=="beam"){
		led_variables.Get("StartDelay", delay_after_beam_s);
		led_variables.Get("SpillModulus", spill_modulus);
	} else {
		delay_after_beam_s=0;
		spill_modulus=0;
	}
	delay_after_beam=boost::posix_time::seconds(delay_after_beam_s);
	delay_after_beam_counts = delay_after_beam_s*125E6;
	led_variables.Get("SequenceRepetitions",sequence_repetitions);
	++sequence_repetitions; // 1 means run it twice.....
	led_variables.Get("SamplingStart",waveform_presamples); // XXX put in DataModel
	led_variables.Get("WaveformLength",waveform_samples);   // XXX put in DataModel
	led_variables.Get("LedAdvance",led_advance_counts);     // XXX put in DataModel
	led_variables.Get("LedSettings",settings_json);
	led_variables.Get("LedSequence",sequence_json);
	
	Store sequence_store;
	sequence_store.JsonParser(sequence_json);
	
	// XXX does Store allow you to get a JSON array into a vector now?
	std::vector<std::string> sequence_vector;
	bool ok = sequence_store.Get("sequence", sequence_vector);
	if(!ok){
		std::cerr<<"you can't get json arrays into a vector, silly!"<<std::endl;
		size_t next_char=0;
		// trim leading whitespace
		while((sequence_json.length()>next_char) && std::isspace(sequence_json.at(next_char))) ++next_char;
		// FIXME rest is brittle parsing
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
	
	// everything in LedSettings is common to all flashes in the sequence
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
		Log("Error! LED Sequence period "+std::to_string(sequence_period_counts)+" is too short to be timed with us clock!",v_error,m_verbose);
		return false;
	}
	// XXX is this strict enough of a check to ensure we send out alerts in time for the StartCount?
	sequence_period = boost::posix_time::microseconds(double(sequence_period_counts)*8E-3);
	std::cout<<"sequence period is "<<sequence_period<<std::endl;
	
	// LedSequence contains the specific settings for each flash in the sequence.
	// These need to be merged with the common settings to form the alert for each flash
	// alert_store.Erase("WaitToStart"); // this does not get propagated to the MPMTs FIXME when Store has Erase/Remove method
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
		/*
		MPMTID: which MPMT will flash, from LedSequence
		LED: 0-7 (3-bit bitmask of LEDs on the MPMT), from LedSequence
		Gain: 0 or 1 (1 is a significant bump to LED intensity), from LedSequence
		DACSetting: 0-1023 (fine control of LED intensity), from LedSequence
		StartTime: coarse counter of when to start flashing*, added later
		NumFlashes: 1-65536, from LedSettings
		FlashInterval: 1-65536 (in clock ticks), from LedSettings
		Type: 0=software trigger, 1=ext trigger, from LedSettings // FIXME missing from pdf
		*/
		
	}
	
	std::clog<<"LED Sequence loaded"<<std::endl;
	
	return true;
}

std::string LED::BuildMPMTJson(int sequence_num, int sequences_performed, unsigned long last_beamspill_counts, long unsigned int& last_flash_counts){
	
	// generate json for next alert
	// most of it's already built during LoadSequence, we just need to update the StartCounts
	std::string json;
	unsigned long StartCounts;
	
	// if real flash
	if(sequences_performed<sequence_repetitions){
		
		if(sequence_num==0 && spill_modulus>0){
			// if interleaving with beam, start time of each sequence is determined from time since last beamspill
			StartCounts = last_beamspill_counts + delay_after_beam_counts - led_advance_counts;
		} else {
			// subsequent flash times, or all of them if not interleaving with beam,
			// are just determined from time since last flash
			// note that after each sequence there is a dead time equal to the sequence time
			StartCounts = last_flash_counts + sequence_period_counts;
		}
		
		last_flash_counts = StartCounts;
		//StartCounts += 125000000; // add 1s delay  FIXME FIXME FIXME debug only remove (could also add 1s to alert_prewarn_ms)
		
	} else {
		// since internal MPMT coarse counter will eventually loop, if we leave a valid
		// StartTime in the MPMT memory, it will eventually start flashing again.
		// Set it to a special value to prevent this happening.
		StartCounts = 0xFFFFFFFFFFFF;
		last_flash_counts = last_flash_counts + sequence_period_counts;
	}
	
	firing_sequence.at(sequence_num).Set("StartTime", StartCounts);
	firing_sequence.at(sequence_num)>>json;
	
	return json;
}

/*
// LED configuration JSON contents:
RunType: "calibration" or "beam" - redundant
StartDelay: how long to wait after end of beam spill before resuming flashing
SpillModulus: only flash every Nth beam spill
SequenceRepetitions: 0 = only perform the sequence once. 1 = do twice...etc
SamplingStart: waveform starts specified counts before LED flash
WaveformLength: number of samples in waveform
LedAdvance: debug variable required to determine time windows to extract from MPMT data*
LedSettings: an embedded JSON of...
	NumFlashes: number of times to flash the LED
	FlashInterval: time between flashes
	WaitToStart: delay in coarse counts before moving to next LED.
	These should be sent to the MPMTs along with other settings in LedSequence

**** make sure that cumulative time of NumFlashes*FlashInterval*number of set bits in LED > sequence duration

// the following defines what gets sent to the MPMTs in the alert:
MPMTID: which MPMT will flash
LED: 0-7 (3-bit bitmask of LEDs on the MPMT)
Gain: 0 or 1 (1 is a significant bump to LED intensity)
DACSetting: 0-1023 (fine control of LED intensity)
StartTime: coarse counter of when to start flashing*
NumFlashes: 1-65536
FlashInterval: 1-65536 (in clock ticks)
Type: 0=software trigger, 1=ext trigger

to disable flashing set future time to 0xFFFFFFFFFFFF (48bits of 1)

* due to a firmware bug at present, the LEDs will actually flash ~2us after the requested StartTime
this needs to be corrected as follows:
1. subtract the LedAdvance from the actually desired flash coarse counter, and send that value as StartTime to the MPMTs
2. the MPMT will report the coarse counter on which it believed it flashed (should be close to StartTime)
3. add LedAdvance to this time to find the actual time of the LED flash. Extract data in windows accordingly.


*/
