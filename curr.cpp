#include "LED.h"

LED_args::LED_args():Thread_args(){}

LED_args::~LED_args(){}


LED::LED():Tool(){}


bool LED::Initialise(std::string configfile, DataModel &data){
	
	m_configfile=configfile;
	InitialiseTool(data);
	InitialiseConfiguration(configfile);
	//m_variables.Print();
	
	if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
	
	m_util=new Utilities();
	//args=new LED_args();
	
	//m_util->CreateThread("test", &Thread, args);
	
	LoadConfig();
	
	ExportConfiguration();
	
	// wait at least the delay_after_beam seconds before first flash, in case we just missed a beamspill end signal
	last_beamspill_end = boost::posix_time::microsec_clock::universal_time() + delay_after_beam*1E6;
	spills_since_last_flash = 0;
	
	return true;
}


bool LED::Execute(){
	
	if(m_data->change_config){
		InitialiseConfiguration(m_configfile);
		LoadConfig();
		ExportConfiguration();    
	}
	
	// just in case ben doesn't put a timestamp of when the beam spill ended, note it ourselves
	if(last_beam_active && !m_data->beam_active){
		last_beamspill_end  - boost::posix_time::microsec_clock::universal_time();
		++spills_since_last_flash;
	}
	last_beam_active = m_data->beam_active;
	// for when he does
	//last_beamspill_end = m_data->last_beamspill_end;
	
	// if only flashing every Nth beam spill, check if enough beamspills have passed
	if(spill_modulus!=0 && (spills_since_last_flash%spill_modulus)!=0)){
		continue;
	}
	
	lapse = period - (boost::posix_time::microsec_clock::universal_time() - last);
	
	if(!lapse.is_negative()) return true; // not yet time for next led alert
	
	// wait delay_after_beam coarse counter ticks after the end-of-beam-spill signal before resuming flash sequence
	if(((boost::posix_time::microsec_clock::universal_time() - last_beamspill_end)/(COARSE_COUNTER_MS*1E3)) > delay_after_beam){
		Log("Suppressing LED flash during beam",v_debug,m_verbose);
		return true;
	}
	
	std::string json = BuildMPMTJson();
	printf("Sending alert for LED flash (sequence %d) to MPMTs\n",sequence_num);
	m_data->services->AlertSend("LEDTrigger", json);
	
	sequence_num++;
	if(sequence_num == firing_sequence.size()) sequence_num=0;
	
	last = boost::posix_time::microsec_clock::universal_time();
	spills_since_last_flash = 0;
	
	return true;
}


bool LED::Finalise(){
	
	m_util->KillThread(args);
	
	delete args;
	args=0;
	
	delete m_util;
	m_util=0;
	
	return true;
}

void LED::Thread(Thread_args* arg){

	LED_args* args=reinterpret_cast<LED_args*>(arg);

}

bool LED::LoadSequence(int version){

	firing_sequence.clear();
	
	// m_data->services->GetDeviceConfig(config_json, -1, "MCCAssignments");
	
	std::string test_sequence = "{\"sequence\":[{\"MPMTID\":77,\"LED\":1,\"DACSetting\":750},{\"MPMTID\":20,\"LED\":2,\"DACSetting\":750}, {\"MPMTID\":26,\"LED\":1,\"DACSetting\":750},{\"MPMTID\":104,\"LED\":2,\"DACSetting\":750}, {\"MPMTID\":13,\"LED\":2,\"DACSetting\":750},{\"MPMTID\":85,\"LED\":1,\"DACSetting\":750}, {\"MPMTID\":46,\"LED\":1,\"DACSetting\":750},{\"MPMTID\":41,\"LED\":1,\"DACSetting\":750},{\"MPMTID\":84,\"LED\":1,\"DACSetting\":750}, {\"MPMTID\":3,\"LED\":1,\"DACSetting\":750},{\"MPMTID\":45,\"LED\":1,\"DACSetting\":750}, {\"MPMTID\":14,\"LED\":2,\"DACSetting\":750},{\"MPMTID\":34,\"LED\":2,\"DACSetting\":750}, {\"MPMTID\":52,\"LED\":1,\"DACSetting\":750}]}";
	
	Store sequence_store;
	sequence_store.JsonParser(test_sequence);
	std::vector<std::string> triggers;
	sequence_store.Get("sequence", triggers);
	
	Store led_trigger;
	
	for(int i=0; i<triggers.size(); i++){
		
		led_trigger.JsonParser(triggers.at(i));
		firing_sequence.push_back(led_trigger);
		
	}
	
	/*
	unsigned int mpmt_ID = 45;
	unsigned int gain= 1;
	unsigned int dac_setting=750;
	unsigned int type = 0;
	unsigned int num_flashes = 10000;
	unsigned int flash_interval = 10000;
	unsigned int start_time = 62500;
	unsigned int led;
	
	led_trigger.Set("MPMTID", mpmt_ID);
	led_trigger.Set("Gain", gain);
	led_trigger.Set("DACSetting", dac_setting);
	led_trigger.Set("Type", type);
	led_trigger.Set("NumFlashes", num_flashes);
	led_trigger.Set("FashInterval", flash_interval);
	led_trigger.Set("StartTime", start_time);
	led_trigger.Set("LED", led);
	
	firing_sequence.push_back(led_trigger);
	*/
	
	return true;
}

void LED::LoadConfig(){

	if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
	
	led_variables.Get("StartDelay", delay_after_beam);
	led_variables.Get("SpillModulus", spill_modulus);
	led_variables.Get("SequenceRepetitions",sequence_repetitions);
	led_variables.Get("SamplingStart",waveform_presamples);
	led_variables.Get("WaveformLength",waveform_samples);
	led_variables.Get("LedAdvance",led_advance);
	led_variables.Get("LedSettings",sequence_settings);
	
	if(!m_variables.Get("alert_prewarn",alert_prewarn)) alert_prewarn = 15000000000U;
	// check that we are not being configured to send out LED flash requests so far in advance
	// that we may receive a beam spill notification after we have already told the MPMTs to flash
	// this is technically 0.2s. to allow for network delays in that message being sent and propagated
	// to the datamodel, limit the maximum pre-warn time to 1/10th of this: 20ms.
	if(alert_prewarn>m_data->trigger_prewarn){
		std::string msg = "LED::LoadConfig Error! Time between MPMT alert and requested flash (alert_prewarn: "+std::to_string(alert_prewarn)
			+") cannot be greater than beam spill notice ("+std::to_string(m_data->trigger_prewarn)+")!";
		Log(msg,v_error,m_verbose);
		return;
	}
	period=boost::posix_time::seconds(300);
	last= boost::posix_time::microsec_clock::universal_time();
	
	LoadSequence();
	sequence_num=0;
	
	

std::string LED::BuildMPMTJson(){
	
	firing_sequence.at(sequence_num).Set("start_time", m_data->current_coarse_counter + alert_prewarn );
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
		... presumably the MPMT will work through each LED in the LED bitmask in sequence.****
		These should be sent to the MPMTs along with other settings in LedSequence
	
	**** make sure that time between sequence steps is > sequence duration
	
	// the following defines what gets sent to the MPMTs in the alert:
	MPMTID: which MPMT will flash
	LED: 0-7 (3-bit bitmask of LEDs on the MPMT)
	Gain: 0 or 1 (1 is a significant bump to LED intensity)
	DACSetting: 0-1023 (fine control of LED intensity)
	StartTime: coarse counter of when to start flashing*
	NumFlashes: 1-65536
	FlashInterval: 1-65536 (in clock ticks)
	
	* due to a firmware bug at present, the LEDs will actually flash ~2us after the requested StartTime
	this needs to be corrected as follows:
	1. subtract the LedAdvance from the actually desired flash coarse counter, and send that value as StartTime to the MPMTs
	2. the MPMT will report the coarse counter on which it believed it flashed (should be close to StartTime)
	3. add LedAdvance to this time to find the actual time of the LED flash. Extract data in windows accordingly.
	
	
}


