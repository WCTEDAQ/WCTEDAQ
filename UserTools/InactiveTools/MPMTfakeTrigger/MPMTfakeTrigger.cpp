#include "MPMTfakeTrigger.h"

MPMTfakeTrigger_args::MPMTfakeTrigger_args():Thread_args(){
  data=0;
}

MPMTfakeTrigger_args::~MPMTfakeTrigger_args(){
  data=0;
}


MPMTfakeTrigger::MPMTfakeTrigger():Tool(){}


bool MPMTfakeTrigger::Initialise(std::string configfile, DataModel &data){

  InitialiseTool(data);
  InitialiseConfiguration(configfile);
  //m_variables.Print();

  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

  m_data->readout_windows= new std::deque<ReadoutWindow*>;
  
  m_util=new Utilities();
  args=new MPMTfakeTrigger_args();
  args->data=m_data;
  args->last=boost::posix_time::microsec_clock::universal_time();
  args->period=boost::posix_time::seconds(1);
  
  m_util->CreateThread("test", &Thread, args);


  
  ExportConfiguration();
  
  return true;
}


bool MPMTfakeTrigger::Execute(){

  return true;
}


bool MPMTfakeTrigger::Finalise(){

  m_util->KillThread(args);

  delete args;
  args=0;

  delete m_util;
  m_util=0;

  delete m_data->readout_windows;
  m_data->readout_windows=0;
  
  return true;
}

void MPMTfakeTrigger::Thread(Thread_args* arg){

  MPMTfakeTrigger_args* args=reinterpret_cast<MPMTfakeTrigger_args*>(arg);

  args->lapse = args->period -( boost::posix_time::microsec_clock::universal_time() - args->last);
  if(!args->lapse.is_negative()){
    usleep(100);
    return;
  }

  args->last= boost::posix_time::microsec_clock::universal_time();
  
  std::unordered_map<unsigned int, MPMTData*> m_unsorted_data;
  
  args->data->unsorted_data_mtx.lock();
  for(std::unordered_map<unsigned int,MPMTData*>::iterator it=args->data->unsorted_data.begin(); it!=args->data->unsorted_data.end(); it++){
    
    if(it->first <= args->data->current_coarse_counter - 125000000U){
      m_unsorted_data[it->first]=it->second;
      it->second=0;
    }
  }
  
  for(std::unordered_map<unsigned int,MPMTData*>::iterator it=m_unsorted_data.begin(); it!=m_unsorted_data.end(); it++){
    args->data->unsorted_data.erase(it->first);
  }
  args->data->unsorted_data_mtx.unlock();
  
  for(std::unordered_map<unsigned int,MPMTData*>::iterator it=m_unsorted_data.begin(); it!=m_unsorted_data.end(); it++){
    ReadoutWindow* tmp=new ReadoutWindow;
    tmp->mpmt_hits=it->second->mpmt_hits;
    tmp->mpmt_waveforms=it->second->mpmt_waveforms;
    TriggerInfo tmp_trigger;
    tmp_trigger.type=TriggerType::NONE;
    tmp_trigger.time=it->first;
    tmp_trigger.mpmt_LEDs=it->second->mpmt_leds;
    tmp->triggers_info.push_back(tmp_trigger);
    args->data->readout_windows_mtx.lock();
    args->data->readout_windows->push_back(tmp);
    args->data->readout_windows_mtx.unlock();
    delete it->second;
    it->second=0;
  }
  m_unsorted_data.clear();
  
  
}
