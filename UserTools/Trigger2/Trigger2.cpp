#include "Trigger2.h"

Trigger2_args::Trigger2_args():Thread_args(){
  data=0;
}

Trigger2_args::~Trigger2_args(){
  data=0;
}


Trigger2::Trigger2():Tool(){}


bool Trigger2::Initialise(std::string configfile, DataModel &data){

  InitialiseTool(data);
  m_configfile=configfile;
  InitialiseConfiguration(configfile);
  //m_variables.Print();

  LoadConfig();
  
  m_util=new Utilities();
  args=new Trigger2_args();
  args->data=m_data; 
  m_util->CreateThread("test", &Thread, args);

  ExportConfiguration();
  
  return true;
}


bool Trigger2::Execute(){

  if(m_data->change_config){
    InitialiseConfiguration(m_configfile);
    LoadConfig();
    ExportConfiguration();

  }
    
  return true;
}


bool Trigger2::Finalise(){

  m_util->KillThread(args);

  delete args;
  args=0;

  delete m_util;
  m_util=0;

  return true;
}

void Trigger2::Thread(Thread_args* arg){

  Trigger2_args* args=reinterpret_cast<Trigger2_args*>(arg);

  if(!args->data->sorted_data.size()){
    usleep(100);
    return;
  }
  std::map<unsigned int, MPMTData*> sorted_data;
  args->data->sorted_data_mtx.lock();
  std::swap(sorted_data , args->data->sorted_data);
  args->data->sorted_data_mtx.unlock();
   
  for(std::map<unsigned int, MPMTData*>::iterator it= sorted_data.begin(); it!= sorted_data.end(); it++){
    
    Job* tmp_job = new Job("triggering");
    Trigger2_args*  tmp_args= new Trigger2_args;
    tmp_args->data = args->data;
    tmp_args->bin = it->first;
    tmp_args->sorted_data = it->second;
    
    for(int j=0; j<args->triggers.size(); j++){
      tmp_args->trigger_functions.push_back(args->data->trigger_functions[args->triggers.at(j)]);
      Trigger_algo_args* tmp_algo_args = new Trigger_algo_args;
      tmp_algo_args->m_data = args->data;
      tmp_algo_args->sorted_data = it->second;
      tmp_algo_args->trigger_vars = args->data->trigger_vars[args->triggers.at(j)];
      args->trigger_algo_args.push_back(tmp_algo_args);
    }
    
    tmp_job->func=TriggerJob;
    args->data->job_queue.AddJob(tmp_job);

  }
  
}

bool Trigger2::TriggerJob(void* data){

Trigger2_args* args=reinterpret_cast<Trigger2_args*>(args);

 for(int i=0; i<args->trigger_functions.size(); i++){

   args->trigger_functions.at(i)(args->trigger_algo_args.at(i));

 }

 args->data->triggered_data_mtx.lock();
 args->data->triggered_data[args->bin] = args->sorted_data;
 args->data->triggered_data_mtx.unlock();

 
 return true;

}

bool Trigger2::MainTrigger(void* data){

Trigger_algo_args* args=reinterpret_cast<Trigger_algo_args*>(data);
 
 for(unsigned int i=0; i<args->sorted_data->mpmt_triggers.size(); i++){
   TriggerInfo tmp_trigger;

   switch(args->sorted_data->mpmt_triggers.at(i).GetChannel()){

   case 0U: //main trigger
     tmp_trigger.type = TriggerType::MAIN;
     tmp_trigger.time = ((unsigned long) args->sorted_data->coarse_counter << 16) |  ((unsigned long) args->sorted_data->mpmt_triggers.at(i).GetCoarseCounter());
     tmp_trigger.spill_num = args->m_data->spill_num;
     tmp_trigger.vme_event_num = args->m_data->vme_event_num;       
     args->m_data->vme_event_num++;
     args->sorted_data->unmerged_triggers.push_back(tmp_trigger);
     break;
     
   case 1U: //beam monitor - electron
     tmp_trigger.type = TriggerType::EBEAM;
     tmp_trigger.time = ((unsigned long) args->sorted_data->coarse_counter << 16) |  ((unsigned long) args->sorted_data->mpmt_triggers.at(i).GetCoarseCounter());
     args->sorted_data->unmerged_triggers.push_back(tmp_trigger);
     break;
     
   case 2U: //beam monitor - muon
     tmp_trigger.type = TriggerType::MBEAM;
     tmp_trigger.time = ((unsigned long) args->sorted_data->coarse_counter << 16) |  ((unsigned long) args->sorted_data->mpmt_triggers.at(i).GetCoarseCounter());
     args->sorted_data->unmerged_triggers.push_back(tmp_trigger);     
     break;
     
   case 3U: //pre spill trigger
     args->m_data->spill_num++;
     break;
     
   case 4U: //beam end
     break;
     
     break;
   case 5U: //CDS laser
     tmp_trigger.type = TriggerType::LASER;
     tmp_trigger.time = ((unsigned long) args->sorted_data->coarse_counter << 16) |  ((unsigned long) args->sorted_data->mpmt_triggers.at(i).GetCoarseCounter());
     args->sorted_data->unmerged_triggers.push_back(tmp_trigger);    
     break;
     
   case 6U: //hardware trigger
     tmp_trigger.type = TriggerType::HARD6;
     tmp_trigger.time = ((unsigned long) args->sorted_data->coarse_counter << 16) |  ((unsigned long) args->sorted_data->mpmt_triggers.at(i).GetCoarseCounter());
     args->sorted_data->unmerged_triggers.push_back(tmp_trigger); 
     break;

   }  
   
   
 }
 return true;
 
}

bool Trigger2::LedTrigger(void* data){
  
  Trigger_algo_args* args=reinterpret_cast<Trigger_algo_args*>(data);
  
  for(unsigned int i=0; i<args->sorted_data->mpmt_leds.size(); i++){
    TriggerInfo tmp_trigger;
    tmp_trigger.type = TriggerType::LED;
    tmp_trigger.time = ((unsigned long) args->sorted_data->coarse_counter << 16) |  ((unsigned long) args->sorted_data->mpmt_leds.at(i).GetCoarseCounter());
    args->sorted_data->unmerged_triggers.push_back(tmp_trigger);
    
  }
  
  return true;
}

/*
  bool Trigger2::NoneTrigger(void* data){
  
  Trigger_algo_args* args=reinterpret_cast<Trigger_algo_args*>(data);
  
  
  return true;
  }
*/

void Trigger2::LoadConfig(){
  
  //m_data->trigger_functions.clear();
  //m_data->trigger_vars.clear();
  bool main=false;
  bool led=false;
  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
  m_variables.Get("main", main);
  m_variables.Get("led", led);
  //bool Laser=false;
  //bool none=false;
  //bool Beam=false;
  //bool Beam=false;
  //bool Beam=false;
  //bool Beam=false;
  
  if(main){
    m_data->trigger_functions["beam"]=MainTrigger;
    m_data->trigger_vars["beam"]=&main_vars;
  }
  if(led){
    m_data->trigger_functions["led"]=LedTrigger;
    m_data->trigger_vars["led"]=&led_vars;
  }
  /*  if(none){
    m_data->trigger_functions["none"]=LedTrigger;
    m_data->trigger_vars["none"]=&led_vars;
  }
  */

}
