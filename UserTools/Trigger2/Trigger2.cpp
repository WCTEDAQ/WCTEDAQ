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

  //  printf("d1\n");
   args->data->sorted_data_mtx.lock();
   //				    printf("d1.1\n");
  if(!args->data->sorted_data.size()){
    //printf("d1.2\n");
     args->data->sorted_data_mtx.unlock();
     //printf("d1.3\n");
     usleep(100);
     //printf("d1.4\n");
     return;
  }
   //printf("d1.5\n");
  //printf("d2\n");
   std::map<unsigned int, MPMTData*> sorted_data;
   
   std::swap(sorted_data , args->data->sorted_data);
   args->data->sorted_data_mtx.unlock();
   //printf("d3\n");

   /*
  for(std::map<unsigned int, MPMTData*>::iterator it= sorted_data.begin(); it!=sorted_data.end(); it++){
    
    delete it->second;
    it->second=0;
  }
  sorted_data.clear();
   */
   
  
   if(args->triggers.size()==0){
       ////////////////////

     args->triggers.push_back("nhits");
     args->triggers.push_back("main");
     args->triggers.push_back("led");
       //////////
   }

   
   if(args->triggers.size()>0){
     
     for(std::map<unsigned int, MPMTData*>::iterator it= sorted_data.begin(); it!= sorted_data.end(); it++){
       //printf("d4\n");
       Job* tmp_job = new Job("triggering");
       Trigger2_args*  tmp_args= new Trigger2_args;
       tmp_args->data = args->data;
       tmp_args->bin = it->first;
       tmp_args->sorted_data = it->second;
       //printf("d5 size=%d\n",args->triggers.size());

	for(unsigned int j=0; j<args->triggers.size(); j++){
	  //printf("d5.1 trigger=%s\n",args->triggers.at(j).c_str());
	  //printf("d5.2 count=%d\n",args->data->trigger_functions.count(args->triggers.at(j).c_str()));
	  if(args->data->trigger_functions.count(args->triggers.at(j).c_str())>0){
	    tmp_args->trigger_functions.push_back(args->data->trigger_functions[args->triggers.at(j)]);
	    Trigger_algo_args* tmp_algo_args = new Trigger_algo_args;
	    tmp_algo_args->m_data = args->data;
	    tmp_algo_args->sorted_data = it->second;
	    tmp_algo_args->trigger_vars = args->data->trigger_vars[args->triggers.at(j)];
	    tmp_args->trigger_algo_args.push_back(tmp_algo_args);
	  }
	}
       //printf("d6\n");
       tmp_job->func=TriggerJob;
       tmp_job->data=tmp_args;
       //printf("d6.2 pointerin=%p\n", tmp_args);
       args->data->job_queue.AddJob(tmp_job);

       /*
	 printf("d6.3\n");
       for(unsigned int k=0; k<tmp_args->trigger_algo_args.size(); k++){
	 //delete tmp_args->trigger_algo_args.at(k).sorted_data;
	 printf("d6.3a\n");
	 delete	tmp_args->trigger_algo_args.at(k);
	 printf("d6.3b\n");
	 tmp_args->trigger_algo_args.at(k)=0;
	 printf("d6.3c\n");
       }
       printf("d6.4\n");
       delete tmp_args->sorted_data;
       printf("d6.5\n");
       delete tmp_args;
       printf("d6.6\n");
       delete tmp_job;
       printf("d6.7\n");
       */
     }
     
   }
  
   //printf("d7\n");
}

bool Trigger2::TriggerJob(void* data){
  //printf("r1 pointer =%p\n",data);
  Trigger2_args* args=reinterpret_cast<Trigger2_args*>(data);
  //printf("r2\n");
  //printf("r2.1 size=%d\n",args->trigger_functions.size());
  for(unsigned int i=0; i<args->trigger_functions.size(); i++){
    //printf("r3\n");
    args->trigger_functions.at(i)(args->trigger_algo_args.at(i));
    //printf("r4\n");
 delete args->trigger_algo_args.at(i);
  }
  //printf("r5\n");
  args->data->triggered_data_mtx.lock();
  //printf("r6\n");
  //delete  args->sorted_data;
   args->data->triggered_data[args->bin] = args->sorted_data;
  //printf("r7\n");
  args->data->triggered_data_mtx.unlock();
  //printf("r8\n");
  delete args;
  
  return true;
  
}

bool Trigger2::MainTrigger(void* data){
  //printf("m1\n");
  Trigger_algo_args* args=reinterpret_cast<Trigger_algo_args*>(data);
  //printf("m2\n");
  for(unsigned int i=0; i<args->sorted_data->mpmt_triggers.size(); i++){
    //printf("m3\n");
    TriggerInfo tmp_trigger;
    //printf("m4\n");
    
    tmp_trigger.time = ((((unsigned long) args->sorted_data->coarse_counter) & 4294901760U) << 16)  |  ((unsigned long) args->sorted_data->mpmt_triggers.at(i).GetCoarseCounter()); 
    /*    if(( args->sorted_data->coarse_counter & 65535U) > (args->sorted_data->mpmt_triggers.at(i).GetCoarseCounter() >>16)){
      tmp_trigger.time +=4294967296U;
    }
    */
    
    switch(args->sorted_data->mpmt_triggers.at(i).GetChannel()){
           
    case 0U: //main trigger
      tmp_trigger.type = TriggerType::MAIN;
      
      tmp_trigger.spill_num = args->m_data->spill_num;
      tmp_trigger.vme_event_num = args->m_data->vme_event_num;       
      args->m_data->vme_event_num++; //might need mutex probably definitly need
      args->sorted_data->unmerged_triggers.push_back(tmp_trigger);
      break;
      
    case 1U: //beam monitor - electron
      tmp_trigger.type = TriggerType::EBEAM;
      args->sorted_data->unmerged_triggers.push_back(tmp_trigger);
      break;
      
    case 2U: //beam monitor - muon
      tmp_trigger.type = TriggerType::MBEAM;
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
      args->sorted_data->unmerged_triggers.push_back(tmp_trigger);    
      break;
      
    case 7U: //hardware trigger
      tmp_trigger.type = TriggerType::HARD6;
      args->sorted_data->unmerged_triggers.push_back(tmp_trigger); 
      break;
      
    }  
    //printf("m1\n");
    
  }
  return true;
  
}

bool Trigger2::LedTrigger(void* data){
  
  Trigger_algo_args* args=reinterpret_cast<Trigger_algo_args*>(data);
  
  for(unsigned int i=0; i<args->sorted_data->mpmt_leds.size(); i++){
    TriggerInfo tmp_trigger;
    tmp_trigger.type = TriggerType::LED;
    tmp_trigger.time = ((((unsigned long) args->sorted_data->coarse_counter) & 4294901760U) << 16)  |  ((unsigned long) args->sorted_data->mpmt_triggers.at(i).GetCoarseCounter()); 
    /*  if(( args->sorted_data->coarse_counter & 65535U) > (args->sorted_data->mpmt_triggers.at(i).GetCoarseCounter() >>16)){
      tmp_trigger.time +=4294967296U;
    }
    */
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
    m_data->trigger_functions["main"]=MainTrigger;
    m_data->trigger_vars["main"]=&main_vars;
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
