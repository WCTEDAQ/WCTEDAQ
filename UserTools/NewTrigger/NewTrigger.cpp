#include "NewTrigger.h"

NewTrigger_args::NewTrigger_args():Thread_args(){
  m_data=0;
  mpmt_collection=0;
}

NewTrigger_args::~NewTrigger_args(){
  m_data=0;
  mpmt_collection=0;
}


NewTrigger::NewTrigger():Tool(){}


bool NewTrigger::Initialise(std::string configfile, DataModel &data){

  InitialiseTool(data);
  m_configfile=configfile;
  InitialiseConfiguration(configfile);
  //m_variables.Print();

  LoadConfig();
  
  m_util=new Utilities();
  args=new NewTrigger_args();
  args->m_data=m_data; 
  m_util->CreateThread("test", &Thread, args);

  if(!m_data->out_data_chunks) m_data->out_data_chunks=new std::map<unsigned int, MPMTCollection*>;

  m_data->hardware_trigger = true;
  ExportConfiguration();
  
  return true;
}


bool NewTrigger::Execute(){

  if(m_data->change_config){
    InitialiseConfiguration(m_configfile);
    LoadConfig();
    ExportConfiguration();

  }
    
  return true;
}


bool NewTrigger::Finalise(){

  m_util->KillThread(args);

  delete args;
  args=0;

  delete m_util;
  m_util=0;

  return true;
}

void NewTrigger::Thread(Thread_args* arg){


  
  NewTrigger_args* args=reinterpret_cast<NewTrigger_args*>(arg);
  if(args->m_data->raw_readout) return;
  
  std::map<unsigned int, MPMTCollection*> local_data_chunks;
  std::vector<std::map<unsigned int, MPMTCollection*>::iterator > to_remove;

  ////////////////////////////// getting data ////////////////////////////
  
  args->m_data->data_chunks_mtx.lock();
  
  for(std::map<unsigned int, MPMTCollection*>::iterator it= args->m_data->data_chunks.begin(); it!= args->m_data->data_chunks.end(); it++){
    if(it->first < (args->m_data->current_coarse_counter >> 23) + 30){
      local_data_chunks[it->first] = it->second;
      it->second=0;
      to_remove.push_back(it);
    }
  }
  
  for(unsigned int i=0; i< to_remove.size(); i++){
    args->m_data->data_chunks.erase(to_remove.at(i));
  }

  if(args->m_data->data_chunks.size()){
    //printf("macus:%u, ben:%u\n", args->m_data->data_chunks.rbegin()->first,  (args->m_data->current_coarse_counter >>23 ));
    args->m_data->monitoring_store_mtx.lock();
    args->m_data->monitoring_store.Set("recevie_buffer_max_diff", args->m_data->data_chunks.rbegin()->first - (args->m_data->current_coarse_counter >>23 ));
    args->m_data->monitoring_store_mtx.unlock();
  }
  args->m_data->data_chunks_mtx.unlock();

  ///////////////////// finnished getting data //////////////////////
  
  if(local_data_chunks.size() == 0){
    usleep(100);
    return;
  }

  if(true){
    //  if(args->m_data->hardware_trigger){
    
    /// put into buffer
    args->m_data->out_data_chunks_mtx.lock();
    for(std::map<unsigned int, MPMTCollection*>::iterator it= local_data_chunks.begin(); it!= local_data_chunks.end(); it++){
      /*
      bool has_data =false;
      for(unsigned int i=0; i<  it->second->mpmt_output.size(); i++){
	if(it->second->mpmt_output.at(i)->hits.size() || it->second->mpmt_output.at(i)->waveforms.size() ) has_data=true;
	
      }
      
      if(it->second->triggers_info.size() || it->second->triggers.size() || has_data) printf("has data\n");
      else printf("no data!!!\n");
      */
      
      if(args->m_data->out_data_chunks->count(it->first)){
	//send log
	printf("there is late data\n");
	(*args->m_data->out_data_chunks)[it->first]->mpmt_output.insert((*args->m_data->out_data_chunks)[it->first]->mpmt_output.end(), it->second->mpmt_output.begin(), it->second->mpmt_output.end());
	(*args->m_data->out_data_chunks)[it->first]->triggers_info.insert((*args->m_data->out_data_chunks)[it->first]->triggers_info.end(), it->second->triggers_info.begin(), it->second->triggers_info.end());
	(*args->m_data->out_data_chunks)[it->first]->triggers.insert((*args->m_data->out_data_chunks)[it->first]->triggers.end(), it->second->triggers.begin(), it->second->triggers.end());
	
      }
      else{
	(*args->m_data->out_data_chunks)[it->first]=it->second;
	it->second=0;
      }
      /*
      delete (*args->m_data->out_data_chunks)[it->first];
      (*args->m_data->out_data_chunks)[it->first]=0;
      (*args->m_data->out_data_chunks).erase(it->first);
      */
    }
    
    args->m_data->out_data_chunks_mtx.unlock();
  }

  if(false){
    //  if(!args->m_data->hardware_trigger){
       
    //do nhits

    for(std::map<unsigned int, MPMTCollection*>::iterator it= local_data_chunks.begin(); it!= local_data_chunks.end(); it++){
      
      //printf("d4\n");
      Job* tmp_job = new Job("nhits");
      NewTrigger_args*  tmp_args= new NewTrigger_args;
      tmp_args->m_data = args->m_data;
      tmp_args->bin = it->first;
      tmp_args->mpmt_collection = it->second;
      tmp_args->window_size = 100;
      tmp_args->threashold = 100;

      it->second=0;
      //printf("d5 size=%d\n",args->triggers.size());
      
      tmp_job->func=NhitsJob;
      tmp_job->data=tmp_args;
      //printf("d6.2 pointerin=%p\n", tmp_args);
      args->m_data->job_queue.AddJob(tmp_job);
      
      
    }


  }
  
  
  /*
      
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
  /*
}
     
   }
  */
   //printf("d7\n");
}

void NewTrigger::LoadConfig(){
  
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
  /*
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

bool NewTrigger::NhitsJob(void* data){

 NewTrigger_args* args=reinterpret_cast<NewTrigger_args*>(data);

 //printf("d1\n");
 // unsigned short* hit_sum = new unsigned short[8388608];
 unsigned short* hit_sum = new unsigned short[131072];
 *hit_sum = { 0 };
 //printf("d2\n");
 
 for(unsigned int i=0; i< args->mpmt_collection->mpmt_output.size(); i++){
   for(unsigned int j=0; j< args->mpmt_collection->mpmt_output.at(i)->hits.size(); j++){

     //     hit_sum[args->mpmt_collection->mpmt_output.at(i)->hits.at(j).hit->GetCoarseCounter() - (args->bin <<23)]++;

     hit_sum[args->mpmt_collection->mpmt_output.at(i)->hits.at(j).hit->GetCoarseCounter() >>15 ];
   }
 }
 //printf("d3\n");
 for (unsigned int i= 1 ; i <131071; i++){
   hit_sum[i]+= hit_sum[i-1];
 }
  //printf("d4\n");
 for (unsigned int i= args->window_size; i<131071; i++){
   if(hit_sum[i]-hit_sum[i-args->window_size] > args->threashold){
     TriggerInfo* tmp = new TriggerInfo;
     tmp->type= TriggerType::NHITS;
     tmp->time= i + (args->bin <<7);
     args->mpmt_collection->triggers_info.push_back(tmp);
     i+=args->window_size;
   }

 }
 //printf("d5\n");
 //put into buffer
 
 args->m_data->out_data_chunks_mtx.lock();
 if(args->m_data->out_data_chunks->count(args->bin)){
   //send log
   printf("there is late data from nhits\n");
   /* (*args->m_data->out_data_chunks)[args->bin]->mpmt_output.insert((*args->m_data->out_data_chunks)[args->bin]->mpmt_output.end(), it->second->mpmt_output.begin(), it->second->mpmt_output.end());
   (*args->m_data->out_data_chunks)[args->bin]->triggers_info.insert((*args->m_data->out_data_chunks)[args->bin]->triggers_info.end(), it->second->triggers_info.begin(), it->second->triggers_info.end());
   (*args->m_data->out_data_chunks)[args->bin]->triggers.insert((*args->m_data->out_data_chunks)[args->bin]->triggers.end(), it->second->triggers.begin(), it->second->triggers.end());
   */
   delete args->mpmt_collection;
   args->mpmt_collection=0;
 }

 else{
    //printf("d6\n");
   (*args->m_data->out_data_chunks)[args->bin]=args->mpmt_collection;
   args->mpmt_collection=0;
 }

 args->m_data->out_data_chunks_mtx.unlock();
 //printf("d7\n");
 
 delete[] hit_sum;
 
 return true;
 
}
