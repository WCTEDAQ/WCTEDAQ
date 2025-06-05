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

  
  m_util=new Utilities();
  args=new NewTrigger_args();
  args->m_data=m_data; 
  LoadConfig();

  m_util->CreateThread("test", &Thread, args);

  
  if(!m_data->out_data_chunks){
    m_data->out_data_chunks_mtx.lock();
    m_data->out_data_chunks=new std::map<unsigned int, MPMTCollection*>;
    m_data->out_data_chunks_mtx.unlock();
  }

  if(m_data->preadout_windows==0){
    m_data->preadout_windows_mtx.lock();
    m_data->preadout_windows=new std::vector<PReadoutWindow*>;
    m_data->preadout_windows_mtx.unlock();
  }
  
  m_data->hardware_trigger = false;
  ExportConfiguration();
  
  return true;
}


bool NewTrigger::Execute(){

  if(m_data->change_config){
    InitialiseConfiguration(m_configfile);
    LoadConfig();
    ExportConfiguration();
  }

  if(!m_data->running){

    m_data->out_data_chunks_mtx.lock();
    if(m_data->out_data_chunks->size()){
      for( std::map<unsigned int, MPMTCollection*>::iterator it=m_data->out_data_chunks->begin(); it!=m_data->out_data_chunks->end(); it++){
	delete it->second;
	it->second=0;
      }
      m_data->out_data_chunks->clear();
    }
    m_data->out_data_chunks_mtx.unlock();
    

  }
    
  return true;
}


bool NewTrigger::Finalise(){

  m_util->KillThread(args);

  
  delete args;
  args=0;

  

  delete m_util;
  m_util=0;
  
  m_data->data_chunks_mtx.lock();
  
  for(std::map<unsigned int, MPMTCollection*>::iterator it=m_data->data_chunks.begin(); it != m_data->data_chunks.end(); it++){
    delete it->second;
    it->second=0;
  }
  m_data->data_chunks.clear();
  m_data->data_chunks_mtx.unlock();

  m_data->mpmt_messages_mtx.lock();
  for(unsigned int i=0; i<m_data->mpmt_messages->size(); i++){
    delete m_data->mpmt_messages->at(i);
    m_data->mpmt_messages->at(i)=0;
  }

  delete m_data->mpmt_messages;
  m_data->mpmt_messages =0;
  m_data->mpmt_messages_mtx.unlock();
  
  return true;
}

void NewTrigger::Thread(Thread_args* arg){


  
  NewTrigger_args* args=reinterpret_cast<NewTrigger_args*>(arg);
  if(args->m_data->raw_readout || !args->m_data->running) return;
  
  std::map<unsigned int, MPMTCollection*> local_data_chunks;
  std::vector<std::map<unsigned int, MPMTCollection*>::iterator > to_remove;

  ////////////////////////////// getting data ////////////////////////////
  
  args->m_data->data_chunks_mtx.lock();
  
  for(std::map<unsigned int, MPMTCollection*>::iterator it= args->m_data->data_chunks.begin(); it!= args->m_data->data_chunks.end(); it++){
    //printf(" first=%u, counter=%u\n",  it->first, ((args->m_data->current_coarse_counter >> 23) + 300));
    if((args->m_data->current_coarse_counter >> 23) > 170 && it->first < (args->m_data->current_coarse_counter >> 23) - 170){
      //printf(" processing bin=%u\n", it->first);
      local_data_chunks[it->first] = it->second;
      it->second=0;
      to_remove.push_back(it);
      
      if(args->check_time.count(it->first)){
	args->m_data->services->SendLog("ERROR Accumulation: there is late data to the accumulator" , v_error);
	args->m_data->services->SendAlarm("ERROR Accumulation: there is late data to the accumulator" , v_error);
      }
      args->check_time[it->first]=true;
      if(args->check_time.size() > 200) args->check_time.clear();
    }
  }
  
  for(unsigned int i=0; i< to_remove.size(); i++){
    args->m_data->data_chunks.erase(to_remove.at(i));
  }

  if(args->m_data->data_chunks.size()){
    //printf("marcus:%u, ben:%u\n", args->m_data->data_chunks.rbegin()->first,  (args->m_data->current_coarse_counter >>23 ));
    args->m_data->monitoring_store_mtx.lock();
    args->m_data->monitoring_store.Set("receive_buffer_max_diff", args->m_data->data_chunks.rbegin()->first - (args->m_data->current_coarse_counter >>23 ));
    args->m_data->monitoring_store_mtx.unlock();
  }
  args->m_data->data_chunks_mtx.unlock();

  ///////////////////// finished getting data //////////////////////
  
  if(local_data_chunks.size() == 0){
    //printf("d1\n");
    usleep(100);
    return;
  }
  
  if(!args->m_data->nhits_trigger){

    //      printf("d2\n");
    /// put into buffer
    args->m_data->out_data_chunks_mtx.lock();
    for(std::map<unsigned int, MPMTCollection*>::iterator it= local_data_chunks.begin(); it!= local_data_chunks.end(); it++){
      //  printf("d3\n");
      /*
      bool has_data =false;
      for(unsigned int i=0; i<  it->second->mpmt_output.size(); i++){
	if(it->second->mpmt_output.at(i)->hits.size() || it->second->mpmt_output.at(i)->waveforms.size() ) has_data=true;
	
      }
      
      if(it->second->triggers_info.size() || it->second->triggers.size() || has_data) printf("has data\n");
      else printf("no data!!!\n");
      */
      
      // FINDME
      args->m_data->hitrates_mtx.lock();
      // Ben commented out for zmq bug    memcpy(&(args->m_data->hitrates[it->first][0]), &(it->second->hitcounts[0]), (sizeof(unsigned int)*132));
      args->m_data->hitrates_mtx.unlock();
      
      if(args->m_data->out_data_chunks->count(it->first)){
	//send log
	//	args->m_data->services->SendLog("warning Acumulation: there is late data to the acumulator" , v_error);
	(*args->m_data->out_data_chunks)[it->first]->mpmt_output.insert((*args->m_data->out_data_chunks)[it->first]->mpmt_output.end(), it->second->mpmt_output.begin(), it->second->mpmt_output.end());
	(*args->m_data->out_data_chunks)[it->first]->triggers_info.insert((*args->m_data->out_data_chunks)[it->first]->triggers_info.end(), it->second->triggers_info.begin(), it->second->triggers_info.end());
	(*args->m_data->out_data_chunks)[it->first]->triggers.insert((*args->m_data->out_data_chunks)[it->first]->triggers.end(), it->second->triggers.begin(), it->second->triggers.end());

	it->second->mpmt_output.clear();
	it->second->triggers_info.clear();
	it->second->triggers.clear();
	delete it->second;
	it->second=0;
	
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

  if(args->m_data->nhits_trigger){
       
    //do nhits

    for(std::map<unsigned int, MPMTCollection*>::iterator it= local_data_chunks.begin(); it!= local_data_chunks.end(); it++){
      
      //printf("d4\n");
      Job* tmp_job = new Job("nhits");
      NewTrigger_args*  tmp_args= new NewTrigger_args;
      tmp_args->m_data = args->m_data;
      tmp_args->bin = it->first;
      tmp_args->mpmt_collection = it->second;
      tmp_args->nhits_window_size = args->nhits_window_size;
      tmp_args->nhits_threshold =  args->nhits_threshold;
      tmp_args->nhits_jump =  args->nhits_jump;
      tmp_args->nhits_bit_shift =  args->nhits_bit_shift;
      
      it->second=0;
      //printf("d5 size=%d\n",args->triggers.size());
      
      tmp_job->func=NhitsJob2;
      tmp_job->data=tmp_args;
      //printf("d6.2 pointerin=%p\n", tmp_args);
      if(args->m_data->running) args->m_data->job_queue.AddJob(tmp_job);
      else {
	delete tmp_args->mpmt_collection;
	tmp_args->mpmt_collection=0;
	
	delete tmp_args;
	tmp_args=0;

	delete tmp_job;
	tmp_job=0;
      }
      
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
  if(!m_variables.Get("led_trigger", m_data->led_trigger)) m_data->led_trigger=false;
  if(!m_variables.Get("nhits_trigger", m_data->nhits_trigger)) m_data->nhits_trigger=false;
  if(!m_variables.Get("raw_readout", m_data->raw_readout)) m_data->raw_readout=false;
  if(!m_variables.Get("hardware_trigger", m_data->hardware_trigger)) m_data->hardware_trigger=false;
  if(!m_variables.Get("software_trigger", m_data->software_trigger)) m_data->software_trigger=false;
  printf("hardware_trigger=%u\n", m_data->hardware_trigger);
  
  if(!m_variables.Get("nhits_threshold", args->nhits_threshold)) args->nhits_threshold=20;
  if(!m_variables.Get("nhits_jump", args->nhits_jump)) args->nhits_jump=25;
  if(!m_variables.Get("nhits_windows_size", args->nhits_window_size)) args->nhits_window_size=25;
  if(!m_variables.Get("nhits_bit_shift", args->nhits_bit_shift)) args->nhits_bit_shift=15;
  //bool Laser=false;
  //bool none=false;
 
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
 unsigned int bins = pow(2,32 - args->nhits_bit_shift) -1;
 unsigned short* hit_sum = new unsigned short[bins+1];
 *hit_sum = { 0 };
 //printf("d2\n");
 // unsigned int sum=0;
 
 for(unsigned int i=0; i< args->mpmt_collection->mpmt_output.size(); i++){
   for(unsigned int j=0; j< args->mpmt_collection->mpmt_output.at(i)->hits.size(); j++){

     //     hit_sum[args->mpmt_collection->mpmt_output.at(i)->hits.at(j).hit->GetCoarseCounter() - (args->bin <<23)]++;

     hit_sum[((args->m_data->time_corrections[args->mpmt_collection->mpmt_output.at(i)->hits.at(j).card_id]) + args->mpmt_collection->mpmt_output.at(i)->hits.at(j).hit->GetCoarseCounter()) >> args->nhits_bit_shift ]++;
     // sum++;
   }
 }

 
 
 // printf("d3 %u\n",sum);
 //if(sum!=0){
   for (unsigned int i= 1 ; i <bins+1; i++){
     hit_sum[i]+= hit_sum[i-1];
   }
   //printf("d4 %u\n",hit_sum[bins]);
   for (unsigned int i= args->nhits_window_size; i<bins; i++){
     if(hit_sum[i]-hit_sum[i-args->nhits_window_size] > args->nhits_threshold){
       TriggerInfo* tmp = new TriggerInfo;
       tmp->type= TriggerType::NHITS;
       tmp->time= i + (args->bin <<7);
       args->mpmt_collection->triggers_info.push_back(tmp);
       i+=args->nhits_jump;
     }
     
   }
   //}
 //printf("d5\n");
 //put into buffer
 
 args->m_data->out_data_chunks_mtx.lock();
 if(args->m_data->out_data_chunks->count(args->bin)){
   //send log
   printf("there is late data from nhits\n");
   /*   (*args->m_data->out_data_chunks)[args->bin]->mpmt_output.insert((*args->m_data->out_data_chunks)[args->bin]->mpmt_output.end(), it->second->mpmt_output.begin(), it->second->mpmt_output.end());
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

bool NewTrigger::NhitsJob2(void* data){

 NewTrigger_args* args=reinterpret_cast<NewTrigger_args*>(data);

 //printf("d1\n");
 // unsigned short* hit_sum = new unsigned short[8388608];
 unsigned int bins = pow(2,32 - args->nhits_bit_shift) -1;
 unsigned short* hit_sum = new unsigned short[bins+1]{0};
 // *hit_sum = { 0 };
 //printf("d2\n");
 std::unordered_map <unsigned long, bool> trigger_times;

 
 for(unsigned int i=0; i< args->mpmt_collection->mpmt_output.size(); i++){
   for(unsigned int j=0; j< args->mpmt_collection->mpmt_output.at(i)->hits.size(); j++){

     //     hit_sum[args->mpmt_collection->mpmt_output.at(i)->hits.at(j).hit->GetCoarseCounter() - (args->bin <<23)]++;
     unsigned int bin = ((args->m_data->time_corrections[args->mpmt_collection->mpmt_output.at(i)->hits.at(j).card_id]) + args->mpmt_collection->mpmt_output.at(i)->hits.at(j).hit->GetCoarseCounter()) >> args->nhits_bit_shift;

     hit_sum[bin]++;
     if(trigger_times.count(bin)) continue;
     if(bin==0) bin++;
     //     else if(bin==bins) bin--;
     if( (hit_sum[bin] + hit_sum[bin-1])>  args->nhits_threshold){
       for(size_t index=0; index< args->nhits_jump; index++){
	 trigger_times[bin+index]=true;
       }
       TriggerInfo* tmp = new TriggerInfo;
       tmp->type= TriggerType::NHITS;
       
       tmp->time= (((unsigned long)(args->bin & 33553920U))<< 23) | ((args->m_data->time_corrections[args->mpmt_collection->mpmt_output.at(i)->hits.at(j).card_id]) + args->mpmt_collection->mpmt_output.at(i)->hits.at(j).hit->GetCoarseCounter()); 
	 //   tmp->time= bin + (args->bin <<7);
       args->mpmt_collection->triggers_info.push_back(tmp);
       
     }
   }
 }
 
 
 args->m_data->out_data_chunks_mtx.lock();
 if(args->m_data->out_data_chunks->count(args->bin)){
   //send log
   //  printf("there is late data from nhits\n");
   /*   (*args->m_data->out_data_chunks)[args->bin]->mpmt_output.insert((*args->m_data->out_data_chunks)[args->bin]->mpmt_output.end(), it->second->mpmt_output.begin(), it->second->mpmt_output.end());
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
 delete args;
 args=0;
 
 return true;
 
}
