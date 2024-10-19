#include "WindowBuilder.h"

WindowBuilder_args::WindowBuilder_args():Thread_args(){}

WindowBuilder_args::~WindowBuilder_args(){}


WindowBuilder::WindowBuilder():Tool(){}


bool WindowBuilder::Initialise(std::string configfile, DataModel &data){

  InitialiseTool(data);
  InitialiseConfiguration(configfile);
  //m_variables.Print();


  m_util=new Utilities();
  args=new WindowBuilder_args();
  LoadConfig();
  
  m_util->CreateThread("test", &Thread, args);

  ExportConfiguration();
  
  return true;
}


bool WindowBuilder::Execute(){

  if(m_data->change_config){
    LoadConfig();
    ExportConfiguration();
  }
  
  return true;
}


bool WindowBuilder::Finalise(){

  m_util->KillThread(args);

  delete args;
  args=0;

  delete m_util;
  m_util=0;

  return true;
}

void WindowBuilder::Thread(Thread_args* arg){

  WindowBuilder_args* args=reinterpret_cast<WindowBuilder_args*>(arg);

  if(args->data->triggered_data.count(0) && args->data->triggered_data.count(1)){



    //do stuff

    //   delete 0
  }

  std::vector<unsigned int> erase_list;
  
  for(std::map<unsigned int, MPMTData*>::iterator it=args->data->triggered_data.begin(); it!=args->data->triggered_data.end(); it++){


    
    if(args->data->triggered_data.count(it->first -1) && args->data->triggered_data.count(it->first +1)){


      //////////////////merging triggers//////////////////////////
      std::sort(it->second->unmerged_triggers.begin(), it->second->unmerged_triggers.end(), [](TriggerInfo a, TriggerInfo b)
      {
	return a.time < b.time;
      });      

      std::vector<std::vector< unsigned int> >merged_triggers;
      std::map<unsigned int, bool> trigger_veto;
      
      for(unsigned int i=0; i<it->second->unmerged_triggers.size(); i++){
	if(!trigger_veto.count(i)){	
	  std::vector< unsigned int> tmp;
	  tmp.push_back(i);
	  
	  for(unsigned int j=i; j<it->second->unmerged_triggers.size(); j++){
	    
	    if(it->second->unmerged_triggers.at(j).time > ( it->second->unmerged_triggers.at(i).time + args->offset_trigger[it->second->unmerged_triggers.at(i).type] - args->pre_trigger[it->second->unmerged_triggers.at(i).type] ) && it->second->unmerged_triggers.at(j).time < ( it->second->unmerged_triggers.at(i).time + args->offset_trigger[it->second->unmerged_triggers.at(i).type] + args->post_trigger[it->second->unmerged_triggers.at(i).type] ) ){
	      tmp.push_back(j);
	      trigger_veto[j]=true;
	      
	      
	    }     
	  }
	    merged_triggers.push_back(tmp);
	  
	}
      }

    
    //////////////////////////////////////////////////

    //////////////////////////// collecting data////////////////////////

    //if() // time is out of bounds
      for(unsigned int i=0; i<merged_triggers.size(); i++){

	unsigned long max_time=0;
	unsigned long min_time=-1;
	ReadoutWindow* tmp_readout = new ReadoutWindow;

	//////////////////// collecting trigger_info//////////////
	for(unsigned int j=0; j<merged_triggers.at(i).size(); j++){  
	  tmp_readout->triggers_info.push_back(it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)));
	  if( it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).time + args->offset_trigger[ it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).type] - args->pre_trigger[ it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).type] <min_time) min_time = it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).time + args->offset_trigger[ it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).type] + args->pre_trigger[ it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).type]; 
	  if( it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).time + args->offset_trigger[ it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).type] + args->post_trigger[ it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).type] > max_time) max_time = it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).time + args->offset_trigger[ it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).type] + args->post_trigger[ it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).type];
	  
	}
	
	tmp_readout->start_counter=min_time;
	tmp_readout->readout_num=args->data->readout_num;
	args->data->readout_num++;
	/////////////////////////////////////////////
	
	///////////////////////// collecting mpmt_hits//////////////////////
	std::vector<WCTEMPMTHit>::iterator start_hit;
	std::vector<WCTEMPMTHit>::iterator stop_hit;
	unsigned int size=0;
	
	for(std::vector<WCTEMPMTHit>::iterator it_hit=it->second->mpmt_hits.begin(); it_hit!=it->second->mpmt_hits.end(); it_hit++){
	  if(it_hit->GetCoarseCounter() > max_time) break;
	  if(it_hit->GetCoarseCounter() > min_time){
	    if(it_hit->GetCoarseCounter() < start_hit->GetCoarseCounter()) start_hit= it_hit;
	    if(it_hit->GetCoarseCounter() > stop_hit->GetCoarseCounter()) stop_hit= it_hit;
	    size++;
	  }
	}
	
	tmp_readout->mpmt_hits.resize(size);
	std::memcpy(tmp_readout->mpmt_hits.data(), it->second->mpmt_hits.data(), sizeof(it->second->mpmt_hits.data()));
	/////////////////////////////////////////////////////////////////////////////

	///////////////////////// collecting mpmt_waveforms//////////////////////
	std::vector<WCTEMPMTWaveform>::iterator start_waveform;
	std::vector<WCTEMPMTWaveform>::iterator stop_waveform;
	size=0;
	
	for(std::vector<WCTEMPMTWaveform>::iterator it_waveform=it->second->mpmt_waveforms.begin(); it_waveform!=it->second->mpmt_waveforms.end(); it_waveform++){
	  if(it_waveform->header.GetCoarseCounter() > max_time) break;
	  if(it_waveform->header.GetCoarseCounter() > min_time){
	    if(it_waveform->header.GetCoarseCounter() < start_waveform->header.GetCoarseCounter()) start_waveform= it_waveform;
	    if(it_waveform->header.GetCoarseCounter() > stop_waveform->header.GetCoarseCounter()) stop_waveform= it_waveform;
	    size++;
	  }
	}
	
	tmp_readout->mpmt_waveforms.resize(size);
	std::memcpy(tmp_readout->mpmt_waveforms.data(), it->second->mpmt_waveforms.data(), sizeof(it->second->mpmt_waveforms.data()));
	/////////////////////////////////////////////////////////////////////////////
	  
	//////////////////////  collecting qdc and tdc hits  /////////////////////////
	for(unsigned int j=0; j<merged_triggers.at(i).size(); j++){
	  if(it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).type==TriggerType::BEAM){
	    unsigned long vme_event_vec_num=it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).vme_event_num - args->data->vme_poped;
	    // oh dear can only pop the front one BEN!!!! is this garenteed to be sequential in which case you can get rid of evt num etc.
	    tmp_readout->tdc_hits = args->data->tdc_readout.getEvent();
	    tmp_readout->qdc_hits = args->data->qdc_readout.getEvent();
	     
	  }
	  
	}
	

	//////////////////////////////////////////////////////////////////////////

	//adding readout window
	args->data->readout_windows_mtx.lock();
	args->data->readout_windows->push_back(tmp_readout);
	args->data->readout_windows_mtx.unlock();
	
      }
    
    if(it->first !=0) erase_list.push_back(it->first - 1); ///BEN!!!! THIS NEEDS CHECKING 
    
    }
  }

  /// deleting data;

  for (std::vector<unsigned int>::iterator it=erase_list.begin(); it!=erase_list.end(); it++){

    delete args->data->triggered_data[*it];
    args->data->triggered_data[*it]=0;

    args->data->triggered_data.erase(*it);

  }
  
  erase_list.clear();
  
}

void LoadConfig(){

    if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

    args->pre_trigger.clear();
    args->post_trigger.clear();
    args->offset_trigger.clear();
    
    bool beamelec=false;
    unsigned long beamelec_pre_trigger=0;
    unsigned long beamelec_post_trigger=0;
    unsigned long beamelec_offset_trigger=0;
    m_variables.Get("beamelec", beamelec);
    m_variables.Get("beamelec_pre_trigger", beamelec_pre_trigger);
    m_variables.Get("beamelec_post_trigger", beamelec_post_trigger);
    m_variables.Get("beamelec_offset_trigger", beamelec_offset_trigger);
    if(beamelec){
      args->pre_trigger[TriggerInfo::EBEAM]=beamelec_pre_trigger;
      args->post_trigger[TriggerInfo::EBEAM]=beamelec_post_trigger;
      args->offset_trigger[TriggerInfo::EBEAM]=beamelec_offset_trigger;
    }

    
    bool beammu=false;
    unsigned long beammu_pre_trigger=0;
    unsigned long beammu_post_trigger=0;
    unsigned long beammu_offset_trigger=0;
    m_variables.Get("beamelec", beammu);
    m_variables.Get("beammu_pre_trigger", beammu_pre_trigger);
    m_variables.Get("beammu_post_trigger", beammu_post_trigger);
    m_variables.Get("beammu_offset_trigger", beammu_offset_trigger);
    if(beammu){
      args->pre_trigger[TriggerInfo::MBEAM]=beammu_pre_trigger;
      args->post_trigger[TriggerInfo::MBEAM]=beammu_post_trigger;
      args->offset_trigger[TriggerInfo::MBEAM]=beammu_offset_trigger;
    }
    
    bool led=false;
    unsigned long led_pre_trigger=0;
    unsigned long led_post_trigger=0;
    unsigned long led_offset_trigger=0;
    m_variables.Get("led", led);
    m_variables.Get("led_pre_trigger", led_pre_trigger);
    m_variables.Get("led_post_trigger", led_post_trigger);
    m_variables.Get("led_offset_trigger", led_offset_trigger);
    if(led){
      args->pre_trigger[TriggerInfo::LED]=led_pre_trigger;
      args->post_trigger[TriggerInfo::LED]=led_post_trigger;
      args->offset_trigger[TriggerInfo::LED]=led_offset_trigger;
    }
    
    bool laser=false;
    unsigned long laser_pre_trigger=0;
    unsigned long laser_post_trigger=0;
    unsigned long laser_offset_trigger=0;
    m_variables.Get("laser", laser);
    m_variables.Get("laser_pre_trigger", laser_pre_trigger);
    m_variables.Get("laser_post_trigger", laser_post_trigger);
    m_variables.Get("laser_offset_trigger", laser_offset_trigger);
    if(laser){
      args->pre_trigger[TriggerInfo::LASER]=laser_pre_trigger;
      args->post_trigger[TriggerInfo::LASER]=laser_post_trigger;
      args->offset_trigger[TriggerInfo::LASER]=laser_offset_trigger;
    }
    
    bool none=false;
    unsigned long none_pre_trigger=0;
    unsigned long none_post_trigger=0;
    unsigned long none_offset_trigger=0;
    m_variables.Get("none", none);
    m_variables.Get("none_pre_trigger", none_pre_trigger);
    m_variables.Get("none_post_trigger", none_post_trigger);
    m_variables.Get("none_offset_trigger", none_offset_trigger);
    if(none){
      args->pre_trigger[TriggerInfo::NONE]=none_pre_trigger;
      args->post_trigger[TriggerInfo::NONE]=none_post_trigger;
      args->offset_trigger[TriggerInfo::NONE]=none_offset_trigger;
    }

    bool nhits=false;
    unsigned long nhits_pre_trigger=0;
    unsigned long nhits_post_trigger=0;
    unsigned long nhits_offset_trigger=0;
    m_variables.Get("nhits", nhits);
    m_variables.Get("nhits_pre_trigger", nhits_pre_trigger);
    m_variables.Get("nhits_post_trigger", nhits_post_trigger);
    m_variables.Get("nhits_offset_trigger", nhits_offset_trigger);
    if(nhits){
      args->pre_trigger[TriggerInfo::NHITS]=nhits_pre_trigger;
      args->post_trigger[TriggerInfo::NHITS]=nhits_post_trigger;
      args->offset_trigger[TriggerInfo::NHITS]=nhits_offset_trigger;
    }

    bool hard6=false;
    unsigned long hard6_pre_trigger=0;
    unsigned long hard6_post_trigger=0;
    unsigned long hard6_offset_trigger=0;
    m_variables.Get("hard6", hard6);
    m_variables.Get("hard6_pre_trigger", hard6_pre_trigger);
    m_variables.Get("hard6_post_trigger", hard6_post_trigger);
    m_variables.Get("hard6_offset_trigger", hard6_offset_trigger);
    if(hard6){
      args->pre_trigger[TriggerInfo::HARD6]=hard6_pre_trigger;
      args->post_trigger[TriggerInfo::HARD6]=hard6_post_trigger;
      args->offset_trigger[TriggerInfo::HARD6]=hard6_offset_trigger;
    }

    
}
