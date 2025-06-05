#include "BeamSWTrigger.h"

BeamSWTrigger_args::BeamSWTrigger_args():Thread_args(){}
BeamSWTrigger_args::~BeamSWTrigger_args(){}


BeamSWTrigger::BeamSWTrigger():Tool(){}

bool BeamSWTrigger::Initialise(std::string configfile, DataModel &data){
	
	m_configfile=configfile;
	InitialiseTool(data);
	
	// make a thread to handle sending the alerts
	m_util=new Utilities();
	thread_args=new BeamSWTrigger_args();
	thread_args->parent = this;
	thread_args->running=false;
	m_util->CreateThread("beamswtrigger", &Thread, thread_args);
	
	last_running=false;
	
	return true;
}

bool BeamSWTrigger::Execute(){
	
	if(!m_data->running){
		thread_args->running=false;
		last_running=false;
	} else if(!last_running && m_data->BeamSWTrigger_trigger){
		// ok so this fires if m_data->running is true and last_running is false
		// the m_data->BeamSWTrigger_trigger is an additional check if we're enabling BeamSWTrigger flashes in this run
		// i.e. on transition to running.
		// BUT, m_data->running goes true 1 min before the run actually starts, 
		// and the coarse_counter is not valid until this time
		// so we need one more check that the run has actually started
		if(boost::posix_time::microsec_clock::universal_time() > m_data->start_time){
			thread_args->running=true;
			last_running=true;
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


bool BeamSWTrigger::Finalise(){
	
	m_util->KillThread(thread_args);
	
	delete thread_args;
	thread_args=0;
	
	delete m_util;
	m_util=0;
	
	return true;
}

// will be repeatedly called by Utilities
void BeamSWTrigger::Thread(Thread_args* arg){
	
	BeamSWTrigger_args* m_args=reinterpret_cast<BeamSWTrigger_args*>(arg);
	
	// get reference to parent Tool so we can access its members
	BeamSWTrigger& p = *(m_args->parent);
	
	usleep(10);
	if(!m_args->running){
		m_args->last_running=false;
		sleep(1);
		return;
	}
	
	if(!m_args->last_running){
		std::cout<<"Starting BeamSWTrigger Tool thread"<<std::endl;
		// reset thread locals on transition to running
		m_args->last_beam_active = false;
		m_args->spills_since_last_alert=0;
	}
	m_args->last_running=true;
	
	if(p.m_data->beam_active && !m_args->last_beam_active){
		++m_args->spills_since_last_alert;
		p.Log("Noting spill "+std::to_string(m_args->spills_since_last_alert)+"/"
		              +std::to_string(p.spill_modulus),v_debug,p.m_verbose);
	}
	m_args->last_beam_active = p.m_data->beam_active;
	
	if(!p.m_data->beam_active) return;
	
	if(m_args->spills_since_last_alert<p.spill_modulus) return;
	
	// ok send alert
	std::string json = p.BuildAlertJson(m_args->last_beamspill_counts);
	p.Log("BeamSWTrigger sending SoftTrigger alert at coarse_counts "+std::to_string(p.m_data->current_coarse_counter)
	     +" with content '"+json+"'",v_message,p.m_verbose);
	bool ok = p.m_data->services->AlertSend("SoftTrigger", json);
	if(!ok){
		std::cerr<<"BeamSWTrigger::Thread AlertSend returned false! "<<std::endl;
		return; // this could flood with errors if we keep failing...
		// FIXME add a rate-limiting mechanism? move to next flash in sequence? abort sequence? depends on error...
	}
	
	m_args->spills_since_last_alert=0;
	
	return;
}

bool BeamSWTrigger::LoadConfig(){
	
	if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
	if(!m_variables.Get("SpillModulus",spill_modulus)) spill_modulus=1;
	if(spill_modulus==0){
		// assume this means every one
		Log("BeamSWTrigger::LoadConfig SpillModulus 0! Setting to 1",v_error,m_verbose);
		spill_modulus=1;
	}
	if(!m_variables.Get("BeamspillWarnNs",beam_prewarn_ns)){
		Log("LoadConfig error - SpillWarnAdvanceNs not provided!",v_error,m_verbose);
		return false;
	}
	BeamSWTrigger_variables.Get("BeamWarnCounts",beam_warn_notice);
	// tl;dr - We need to account for the difference between MPMT and DAQ clocks / coarse counters... so add an offset. :)
	if(!m_variables.Get("bens_offset",bens_offset)) bens_offset = 0;
	
	alertStore.Set("MPMTID",0);
	alertStore.Set("NumWindows",1);
	alertStore.Set("WindowInterval",1); // redundant with 1 window
	
	return true;
}

std::string BeamSWTrigger::BuildAlertJson(){
	
	// generate json for next alert
	// we just need to update the StartCounts
	// trigger card that receives the beam spill incoming signal will construct a Hit from it
	// get (full 48-bit) coarse counter from this Hit for best accuracy on beam spill time
	unsigned long StartCounts = m_data->beam_warn_coarse_counts + beam_warn_notice + bens_offset;
	alertStore.Set("StartTime", StartCounts);
	
	std::string json;
	alertStore>>json;
	return json;
}

