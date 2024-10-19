#include "WindowBuilder2.h"

WindowBuilder2_args::WindowBuilder2_args():Thread_args(){}

WindowBuilder2_args::~WindowBuilder2_args(){}


WindowBuilder2::WindowBuilder2():Tool(){}


bool WindowBuilder2::Initialise(std::string configfile, DataModel &data){

  InitialiseTool(data);
  m_configfile=configfile;
  InitialiseConfiguration(configfile);
  //m_variables.Print();


  m_util=new Utilities();
  args=new WindowBuilder2_args();
  LoadConfig();
  args->data=m_data;
  
  m_util->CreateThread("test", &Thread, args);

  ExportConfiguration();
  
  return true;
}


bool WindowBuilder2::Execute(){

  if(m_data->change_config){
    InitialiseConfiguration(m_configfile);
    LoadConfig();
    ExportConfiguration();
  }

  // print somthing with it  args->readout_num;
  
  return true;
}


bool WindowBuilder2::Finalise(){

  m_util->KillThread(args);

  delete args;
  args=0;

  delete m_util;
  m_util=0;

  return true;
}

void WindowBuilder2::Thread(Thread_args* arg){

  WindowBuilder2_args* args=reinterpret_cast<WindowBuilder2_args*>(arg);

  std::vector<unsigned int> erase_list;
  
  for(std::map<unsigned int, MPMTData*>::iterator it=args->data->triggered_data.begin(); it!=args->data->triggered_data.end(); it++){
    
    
    
    if( args->data->triggered_data.count(it->first +1)  && (args->data->triggered_data.count(it->first -1) || it->first ==0)  ){
      
      
      //////////////////merging triggers//////////////////////////
      std::sort(it->second->unmerged_triggers.begin(), it->second->unmerged_triggers.end(), [](TriggerInfo a, TriggerInfo b){
	  return a.time < b.time;
	});
      
      
      std::vector<std::vector<TriggerInfo*> >merged_triggers;
      std::map<unsigned int, bool> trigger_veto;
      
      ////////////// finding MAIN and LED primaries ////////////////
      for(unsigned int i=0; i<it->second->unmerged_triggers.size(); i++){
	
	if((it->second->unmerged_triggers.at(i).type==TriggerType::MAIN && args->offset_trigger.count(TriggerType::MAIN))|| (it->second->unmerged_triggers.at(i).type==TriggerType::LED && args->offset_trigger.count(TriggerType::LED))){
	  std::vector<TriggerInfo*> tmp;
	  tmp.push_back(&(it->second->unmerged_triggers.at(i)));
	  trigger_veto[i]=true;
	  merged_triggers.push_back(tmp);
	}
	
      }

      ///////////////////////////// assosiating others with primaries //////////////
      for(unsigned int i=0; i<it->second->unmerged_triggers.size(); i++){
	if(trigger_veto.count(i)) continue;
	for(unsigned int j=0; j<merged_triggers.size(); j++){
	  long offset=args->offset_trigger[merged_triggers.at(j).at(0)->type];
	  unsigned long pre=args->pre_trigger[merged_triggers.at(j).at(0)->type];;
	  unsigned long post=args->post_trigger[merged_triggers.at(j).at(0)->type];;
	  if(it->second->unmerged_triggers.at(i).time + args->offset_trigger[it->second->unmerged_triggers.at(i).type] > merged_triggers.at(j).at(0)->time + offset - pre 
	     &&
	     it->second->unmerged_triggers.at(i).time + args->offset_trigger[it->second->unmerged_triggers.at(i).type] < merged_triggers.at(j).at(0)->time + offset + post){
	    merged_triggers.at(j).push_back(&(it->second->unmerged_triggers.at(i)));
	    trigger_veto[i]=true;
	  }	  
	}
      }
      //////////////////////////////////////////////////////////////////////////////
      
      ////////////////////////////// adding nhits and straglers (shouldnt be any other than nhits)
      
      if(args->offset_trigger.count(TriggerType::NHITS)){
	for(unsigned int i=0; i<it->second->unmerged_triggers.size(); i++){
	  if(trigger_veto.count(i)) continue;
	  std::vector<TriggerInfo*> tmp;
	  tmp.push_back(&(it->second->unmerged_triggers.at(i)));
	  trigger_veto[i]=true;

	  for(unsigned int j=0; j<it->second->unmerged_triggers.size(); j++){
	    long offset=args->offset_trigger[merged_triggers.at(j).at(0)->type];
	    unsigned long pre=args->pre_trigger[merged_triggers.at(j).at(0)->type];;
	    unsigned long post=args->post_trigger[merged_triggers.at(j).at(0)->type];;
	    if(it->second->unmerged_triggers.at(i).time + args->offset_trigger[it->second->unmerged_triggers.at(i).type] > merged_triggers.at(j).at(0)->time + offset - pre 
	       &&
	       it->second->unmerged_triggers.at(i).time + args->offset_trigger[it->second->unmerged_triggers.at(i).type] < merged_triggers.at(j).at(0)->time + offset + post){
	      merged_triggers.at(j).push_back(&(it->second->unmerged_triggers.at(i)));
	      trigger_veto[i]=true;
	    }	  
	    
	  }
	  
	  merged_triggers.push_back(tmp);
	}
      }
      
 ////////////////////////////////////////////////////////////////////////
 
      
      /*      

	      
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
      */
      
      //////////////////////////////////////////////////
      
      //////////////////////////// collecting data////////////////////////
      
      for(unsigned int i=0; i<merged_triggers.size(); i++){
	
	unsigned long max_time = merged_triggers.at(i).at(0)->time + args->offset_trigger[merged_triggers.at(i).at(0)->type] + args->post_trigger[merged_triggers.at(i).at(0)->type];
	unsigned long min_time = merged_triggers.at(i).at(0)->time + args->offset_trigger[merged_triggers.at(i).at(0)->type] - args->pre_trigger[merged_triggers.at(i).at(0)->type];;
	
	ReadoutWindow* tmp_readout = new ReadoutWindow;
	tmp_readout->start_counter=min_time;
	tmp_readout->readout_num=args->data->readout_num;
	args->data->readout_num++;
	
	for(unsigned int bin_count=0; bin_count<3 ; bin_count++){
	  if(it->first==0 && bin_count==0) bin_count = 1;
	  unsigned int bin=it->first -1 + bin_count;
	  /*
//////////////////// collecting trigger_info//////////////
for(unsigned int j=0; j<merged_triggers.at(i).size(); j++){  
tmp_readout->triggers_info.push_back(it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)));
if( it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).time + args->offset_trigger[ it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).type] - args->pre_trigger[ it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).type] <min_time) min_time = it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).time + args->offset_trigger[ it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).type] + args->pre_trigger[ it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).type]; 
if( it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).time + args->offset_trigger[ it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).type] + args->post_trigger[ it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).type] > max_time) max_time = it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).time + args->offset_trigger[ it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).type] + args->post_trigger[ it->second->unmerged_triggers.at(merged_triggers.at(i).at(j)).type];

}
	  */
	  /////////////////////////////////////////////
	  
	  ///////////////////////// collecting mpmt_hits//////////////////////
	  std::vector<WCTEMPMTHit>::iterator start_hit;
	  std::vector<WCTEMPMTHit>::iterator stop_hit;
	  
	  for(std::vector<WCTEMPMTHit>::iterator it_hit=args->data->triggered_data[bin]->mpmt_hits.begin(); it_hit!=args->data->triggered_data[bin]->mpmt_hits.end(); it_hit++){
	    if(it_hit->GetCoarseCounter() > max_time) break;
	    if(it_hit->GetCoarseCounter() > min_time){
	      if(it_hit->GetCoarseCounter() < start_hit->GetCoarseCounter()) start_hit= it_hit;
	      if(it_hit->GetCoarseCounter() > stop_hit->GetCoarseCounter()) stop_hit= it_hit;
	    }
	  }
	  
	  //	tmp_readout->mpmt_hits.resize(size);
	  //std::memcpy(tmp_readout->mpmt_hits.data(), it->second->mpmt_hits.data(), sizeof(it->second->mpmt_hits.data()));
	  tmp_readout->mpmt_hits.insert(tmp_readout->mpmt_hits.end(), start_hit, stop_hit);
	  /////////////////////////////////////////////////////////////////////////////
	  
	  ///////////////////////// collecting mpmt_waveforms//////////////////////
	  std::vector<WCTEMPMTWaveform>::iterator start_waveform;
	  std::vector<WCTEMPMTWaveform>::iterator stop_waveform;
	  
	  for(std::vector<WCTEMPMTWaveform>::iterator it_waveform=args->data->triggered_data[bin]->mpmt_waveforms.begin(); it_waveform!=args->data->triggered_data[bin]->mpmt_waveforms.end(); it_waveform++){
	    if(it_waveform->header.GetCoarseCounter() > max_time) break;
	    if(it_waveform->header.GetCoarseCounter() > min_time){
	      if(it_waveform->header.GetCoarseCounter() < start_waveform->header.GetCoarseCounter()) start_waveform= it_waveform;
	      if(it_waveform->header.GetCoarseCounter() > stop_waveform->header.GetCoarseCounter()) stop_waveform= it_waveform;
	      
	    }
	  }
	  
	  //tmp_readout->mpmt_waveforms.resize(size);
	  //std::memcpy(tmp_readout->mpmt_waveforms.data(), it->second->mpmt_waveforms.data(), sizeof(it->second->mpmt_waveforms.data()));
	  tmp_readout->mpmt_waveforms.insert(tmp_readout->mpmt_waveforms.end(), start_waveform, stop_waveform);
	  /////////////////////////////////////////////////////////////////////////////
	  
	  
	  ///////////////////////// collecting extra_hits//////////////////////
	  //std::vector<WCTEMPMTHit>::iterator start_hit;
	  //std::vector<WCTEMPMTHit>::iterator stop_hit;
	  
	  for(std::vector<WCTEMPMTHit>::iterator it_hit=args->data->triggered_data[bin]->extra_hits.begin(); it_hit!=args->data->triggered_data[bin]->extra_hits.end(); it_hit++){
	    if(it_hit->GetCoarseCounter() > max_time) break;
	    if(it_hit->GetCoarseCounter() > min_time){
	      if(it_hit->GetCoarseCounter() < start_hit->GetCoarseCounter()) start_hit= it_hit;
	      if(it_hit->GetCoarseCounter() > stop_hit->GetCoarseCounter()) stop_hit= it_hit;
	    }
	  }
	  
	  //	tmp_readout->mpmt_hits.resize(size);
	  //std::memcpy(tmp_readout->mpmt_hits.data(), it->second->mpmt_hits.data(), sizeof(it->second->mpmt_hits.data()));
	  tmp_readout->extra_hits.insert(tmp_readout->extra_hits.end(), start_hit, stop_hit);
	  /////////////////////////////////////////////////////////////////////////////
	  
	  ///////////////////////// collecting extra_waveforms//////////////////////
	  //std::vector<WCTEMPMTWaveform>::iterator start_waveform;
	  //std::vector<WCTEMPMTWaveform>::iterator stop_waveform;
	  
	  for(std::vector<WCTEMPMTWaveform>::iterator it_waveform=args->data->triggered_data[bin]->extra_waveforms.begin(); it_waveform!=args->data->triggered_data[bin]->extra_waveforms.end(); it_waveform++){
	    if(it_waveform->header.GetCoarseCounter() > max_time) break;
	    if(it_waveform->header.GetCoarseCounter() > min_time){
	      if(it_waveform->header.GetCoarseCounter() < start_waveform->header.GetCoarseCounter()) start_waveform= it_waveform;
	      if(it_waveform->header.GetCoarseCounter() > stop_waveform->header.GetCoarseCounter()) stop_waveform= it_waveform;
	      
	    }
	  }
	  
	  //tmp_readout->mpmt_waveforms.resize(size);
	  //std::memcpy(tmp_readout->mpmt_waveforms.data(), it->second->mpmt_waveforms.data(), sizeof(it->second->mpmt_waveforms.data()));
	  tmp_readout->extra_waveforms.insert(tmp_readout->extra_waveforms.end(), start_waveform, stop_waveform);
	  /////////////////////////////////////////////////////////////////////////////
	  
	}
      
      
	
      //////////////////////  collecting qdc and tdc hits  /////////////////////////	  
	if(merged_triggers.at(i).at(0)->type==TriggerType::MAIN){
	  
	  
	  //  unsigned long vme_event_vec_num=merged_triggers.at(i).at(0)).vme_event_num - args->data->vme_poped;
	  tmp_readout->tdc_hits = args->data->tdc_readout.getEvent();
	  tmp_readout->qdc_hits = args->data->qdc_readout.getEvent();
	}	     
	
	/// ///////////// finished collecting data /////////////////////////////
	
	
	
	
	//////////////////////////////////////////////////////////////////////////
	
	//adding readout window
	args->data->readout_windows_mtx.lock();
	args->data->readout_windows->push_back(tmp_readout);
	args->data->readout_windows_mtx.unlock();
	
	
	
	if(it->first !=0) erase_list.push_back(it->first - 1); ///BEN!!!! THIS NEEDS CHECKING 
	
      }
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

void WindowBuilder2::LoadConfig(){

    if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

    args->pre_trigger.clear();
    args->post_trigger.clear();
    args->offset_trigger.clear();
    /*    
    bool beamelec=false;
    unsigned long beamelec_pre_trigger=0;
    unsigned long beamelec_post_trigger=0;
    long beamelec_offset_trigger=0;
    m_variables.Get("beamelec", beamelec);
    m_variables.Get("beamelec_pre_trigger", beamelec_pre_trigger);
    m_variables.Get("beamelec_post_trigger", beamelec_post_trigger);
    m_variables.Get("beamelec_offset_trigger", beamelec_offset_trigger);
    if(beamelec){
      args->pre_trigger[TriggerType::EBEAM]=beamelec_pre_trigger;
      args->post_trigger[TriggerType::EBEAM]=beamelec_post_trigger;
      args->offset_trigger[TriggerType::EBEAM]=beamelec_offset_trigger;
    }

    
    bool beammu=false;
    unsigned long beammu_pre_trigger=0;
    unsigned long beammu_post_trigger=0;
    long beammu_offset_trigger=0;
    m_variables.Get("beamelec", beammu);
    m_variables.Get("beammu_pre_trigger", beammu_pre_trigger);
    m_variables.Get("beammu_post_trigger", beammu_post_trigger);
    m_variables.Get("beammu_offset_trigger", beammu_offset_trigger);
    if(beammu){
      args->pre_trigger[TriggerType::MBEAM]=beammu_pre_trigger;
      args->post_trigger[TriggerType::MBEAM]=beammu_post_trigger;
      args->offset_trigger[TriggerType::MBEAM]=beammu_offset_trigger;
    }
    */
    
    bool led=false;
    unsigned long led_pre_trigger=0;
    unsigned long led_post_trigger=0;
    long led_offset_trigger=0;
    m_variables.Get("led", led);
    m_variables.Get("led_pre_trigger", led_pre_trigger);
    m_variables.Get("led_post_trigger", led_post_trigger);
    m_variables.Get("led_offset_trigger", led_offset_trigger);
    if(led){
      args->pre_trigger[TriggerType::LED]=led_pre_trigger;
      args->post_trigger[TriggerType::LED]=led_post_trigger;
      args->offset_trigger[TriggerType::LED]=led_offset_trigger;
    }
    /*
      bool laser=false;
      unsigned long laser_pre_trigger=0;
      unsigned long laser_post_trigger=0;
      long laser_offset_trigger=0;
      m_variables.Get("laser", laser);
      m_variables.Get("laser_pre_trigger", laser_pre_trigger);
      m_variables.Get("laser_post_trigger", laser_post_trigger);
      m_variables.Get("laser_offset_trigger", laser_offset_trigger);
      if(laser){
      args->pre_trigger[TriggerType::LASER]=laser_pre_trigger;
      args->post_trigger[TriggerType::LASER]=laser_post_trigger;
      args->offset_trigger[TriggerType::LASER]=laser_offset_trigger;
      }
      
      bool none=false;
      unsigned long none_pre_trigger=0;
      unsigned long none_post_trigger=0;
      long none_offset_trigger=0;
      m_variables.Get("none", none);
      m_variables.Get("none_pre_trigger", none_pre_trigger);
      m_variables.Get("none_post_trigger", none_post_trigger);
      m_variables.Get("none_offset_trigger", none_offset_trigger);
      if(none){
      args->pre_trigger[TriggerType::NONE]=none_pre_trigger;
      args->post_trigger[TriggerType::NONE]=none_post_trigger;
      args->offset_trigger[TriggerType::NONE]=none_offset_trigger;
      }
    */
      
    bool nhits=false;
    unsigned long nhits_pre_trigger=0;
    unsigned long nhits_post_trigger=0;
    long nhits_offset_trigger=0;
    m_variables.Get("nhits", nhits);
    m_variables.Get("nhits_pre_trigger", nhits_pre_trigger);
    m_variables.Get("nhits_post_trigger", nhits_post_trigger);
    m_variables.Get("nhits_offset_trigger", nhits_offset_trigger);
    if(nhits){
      args->pre_trigger[TriggerType::NHITS]=nhits_pre_trigger;
      args->post_trigger[TriggerType::NHITS]=nhits_post_trigger;
      args->offset_trigger[TriggerType::NHITS]=nhits_offset_trigger;
    }
    /*
      bool hard6=false;
      unsigned long hard6_pre_trigger=0;
      unsigned long hard6_post_trigger=0;
      long hard6_offset_trigger=0;
      m_variables.Get("hard6", hard6);
      m_variables.Get("hard6_pre_trigger", hard6_pre_trigger);
      m_variables.Get("hard6_post_trigger", hard6_post_trigger);
      m_variables.Get("hard6_offset_trigger", hard6_offset_trigger);
      if(hard6){
      args->pre_trigger[TriggerType::HARD6]=hard6_pre_trigger;
      args->post_trigger[TriggerType::HARD6]=hard6_post_trigger;
      args->offset_trigger[TriggerType::HARD6]=hard6_offset_trigger;
      }
    */
    
    bool main=false;
    unsigned long main_pre_trigger=0;
    unsigned long main_post_trigger=0;
    long main_offset_trigger=0;
    m_variables.Get("main", main);
    m_variables.Get("main_pre_trigger", main_pre_trigger);
    m_variables.Get("main_post_trigger", main_post_trigger);
    m_variables.Get("main_offset_trigger", main_offset_trigger);
    if(main){
      args->pre_trigger[TriggerType::MAIN]=main_pre_trigger;
      args->post_trigger[TriggerType::MAIN]=main_post_trigger;
      args->offset_trigger[TriggerType::MAIN]=main_offset_trigger;
    }
    
    
}
