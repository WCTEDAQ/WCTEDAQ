#include "Sorting2.h"

Sorting2_args::Sorting2_args():Thread_args(){
  data=0;
  unsorted_data=0;
  tmp=0;
  arr=0;
}

Sorting2_args::~Sorting2_args(){}


Sorting2::Sorting2():Tool(){}


bool Sorting2::Initialise(std::string configfile, DataModel &data){

  InitialiseTool(data);
  m_configfile=configfile;
  InitialiseConfiguration(configfile);
  //m_variables.Print();

  m_util=new Utilities();
  args=new Sorting2_args();

  LoadConfig();
  
  args->data=m_data;
  args->last=boost::posix_time::microsec_clock::universal_time();
  m_util->CreateThread("test", &Thread, args);

  ExportConfiguration();
  
  return true;
}


bool Sorting2::Execute(){

  if(m_data->change_config){
    InitialiseConfiguration(m_configfile);
    LoadConfig();
    ExportConfiguration();
    
  }
  /*
  usleep(1000);
  m_data->monitoring_store_mtx.lock();
  m_data->monitoring_store.Set("unsorted_data_size",m_data->unsorted_data.size());
  m_data->monitoring_store.Set("sorted_data_size",m_data->sorted_data.size());
  m_data->monitoring_store_mtx.unlock();
  */
  
  return true;
}


bool Sorting2::Finalise(){

  m_util->KillThread(args);

  delete args;
  args=0;

  delete m_util;
  m_util=0;

  return true;
}

void Sorting2::Thread(Thread_args* arg){

  Sorting2_args* args=reinterpret_cast<Sorting2_args*>(arg);

   args->lapse = args->period -( boost::posix_time::microsec_clock::universal_time() - args->last);
   if(!args->lapse.is_negative()){
     //     sleep(1);
     usleep(100);
     return;
   }

   args->last= boost::posix_time::microsec_clock::universal_time();

   //////////////////// selecting what to sort //////////////////////////
   
   std::vector<MPMTData*> data_to_sort;
   std::vector<unsigned int> items_to_remove;

   args->data->unsorted_data_mtx.lock();

   //printf("unsorted size=%u\n",args->data->unsorted_data.size());
   //////////// map key is in 33,553,920 ns ~ 33.5ms ////////////////
   for(std::unordered_map<unsigned int, MPMTData*>::iterator it= args->data->unsorted_data.begin(); it!= args->data->unsorted_data.end(); it++){
     //printf("times %u:%u\n",((args->data->current_coarse_counter >>22) - 60), it->first);
     if( (args->data->current_coarse_counter >>22) >=60 && it->first< ((args->data->current_coarse_counter >>22) - 60)){ //~2 seconds seconds away from current time (60 x ~33.5ms)    
       items_to_remove.push_back(it->first);
       data_to_sort.push_back(it->second);
       it->second=0;
     }

   }

   for(unsigned int i=0; i<items_to_remove.size(); i++){
     args->data->unsorted_data.erase(items_to_remove.at(i));
   }
   args->data->unsorted_data_mtx.unlock();

   for(unsigned int i=0 ; i<data_to_sort.size(); i++){
     Job* tmp_job= new Job("sorting");
     Sorting2_args* tmp_args = new Sorting2_args;
     tmp_args->data=args->data;
     tmp_args->unsorted_data=data_to_sort.at(i);
     tmp_job->func=SortData;
     tmp_job->fail_func=FailSort;
     tmp_job->data=tmp_args;
     //printf("sending job\n");
     //   delete tmp_args;
     //delete tmp_job;
     //delete data_to_sort.at(i);
     if(!args->data->job_queue.AddJob(tmp_job)) printf("ERROR!!!\n");
     //printf("added to queue\n");
   }
   
}

bool Sorting2::SortData(void* data){
  //printf("in sortdata\n");
  Sorting2_args* args=reinterpret_cast<Sorting2_args*>(data);
  //printf("h1\n");
  
  args->tmp= new MPMTData;
  //printf("h2\n");
  args->tmp->coarse_counter=args->unsorted_data->coarse_counter;
  //printf("h3\n");
  //  args->unsorted_data->Print();
  
  args->arr = new unsigned int[4194304U];
  memset(&args->arr[0], 0, sizeof(*args->arr)*4194304U);
  //   for( unsigned int i=0; i<4194304U; i++){
  //  printf("%u:%u\n",i, args->arr[i]);
  // }
  //printf("h4\n");//////////sort mpmt hits ///////////////
  unsigned int bin=0;
  
  
  //printf("d5 hits size=%d\n", args->unsorted_data->mpmt_hits.size());
  if(args->unsorted_data->mpmt_hits.size() > 0){
    for(std::vector<WCTEMPMTHit>::iterator it=args->unsorted_data->mpmt_hits.begin(); it!= args->unsorted_data->mpmt_hits.end(); it++){
      args->arr[(it->GetCoarseCounter() & 4194303U)]++;
    }
    //printf("d5.5\n");
    
    for( unsigned int i=1; i<4194304U; i++){
      args->arr[i]+=args->arr[i-1];
      //    args->tmp->cumulative_sum[i]=args->arr[i];
      if(args->arr[i]>args->unsorted_data->mpmt_hits.size()) printf("WFT!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    }
    //printf("d6\n");
    
    //  std::copy(std::begin(args->arr), std::end(args->arr), std::begin(args->tmp->cumulative_sum));
    memcpy(&(args->tmp->cumulative_sum[0]), &args->arr[0], sizeof(*args->arr)*4194304);
    //std::copy(&args->arr[0], &args->arr[194302]), std::begin(args->tmp->cumulative_sum));
    //printf("d7\n");
    //printf("d7.1 size=%d\n",args->unsorted_data->mpmt_hits.size());
    //printf("d7.2 size=%d\n",args->tmp->mpmt_hits.size());
    args->tmp->mpmt_hits.resize(args->unsorted_data->mpmt_hits.size());
    //printf("d8\n");
    
    //printf("d8.1\n");
    for(unsigned int i=0; i< args->unsorted_data->mpmt_hits.size(); i++){
      bin=(args->unsorted_data->mpmt_hits.at(i).GetCoarseCounter() & 4194303U);
      if((args->arr[bin] -1)>args->unsorted_data->mpmt_hits.size()) printf("WFT2!!!!!!!!!!! %u : %u\n",bin, args->arr[bin]);
      args->tmp->mpmt_hits.at(args->arr[bin]-1)=args->unsorted_data->mpmt_hits.at(i);
      args->arr[bin]--;
      
    }
    //printf("d9\n");
    memset(&args->arr[0], 0, sizeof(*args->arr)*4194304U);
    //printf("d10\n");
  }
  /////////////////////////////
  /* bool proof=false;
     for(unsigned int i=1; i <args->tmp->mpmt_hits.size(); i++){
     //   printf(" %u:%u\n",i,args->tmp->mpmt_hits.at(i).GetCoarseCounter());
     if(args->tmp->mpmt_hits.at(i).GetCoarseCounter() <args->tmp->mpmt_hits.at(i-1).GetCoarseCounter()){
     //printf(" badsort %u,%u\n",i, args->tmp->mpmt_hits.size());
     //printf(" %u:%u\n",args->tmp->mpmt_hits.at(i-1).GetCoarseCounter(), args->tmp->mpmt_hits.at(i).GetCoarseCounter());
     //printf(" lower bits %u:%u\n",(args->tmp->mpmt_hits.at(i-1).GetCoarseCounter() & 4194304U), (args->tmp->mpmt_hits.at(i).GetCoarseCounter()& 4194304U));
     proof=true;
     }
     }
  if(proof){
  for(unsigned int i=1; i <args->tmp->mpmt_hits.size(); i++){
  //printf(" proof %u, %u, %u: %u, %u\n",i, args->tmp->mpmt_hits.at(i).GetCoarseCounter(), (args->tmp->mpmt_hits.at(i).GetCoarseCounter() & 4194304U),(args->tmp->coarse_counter & 65535U), (args->tmp->mpmt_hits.at(i).GetCoarseCounter() >> 16));
  }
  
  }
  */
  //printf("end\n\n");
  ///////// sorting LEDS ////////
  
  //printf("d11 leds-%d\n",args->unsorted_data->mpmt_leds.size());
  if(args->unsorted_data->mpmt_leds.size() > 0){
  for(std::vector<WCTEMPMTLED>::iterator it=args->unsorted_data->mpmt_leds.begin(); it!= args->unsorted_data->mpmt_leds.end(); it++){
      args->arr[(it->GetCoarseCounter() & 4194303U)]++;
    }
    for( unsigned int i=1; i<4194304U; i++){
      args->arr[i]+=args->arr[i-1];
    }
    
    args->tmp->mpmt_leds.resize(args->unsorted_data->mpmt_leds.size());
    
    bin=0;
    for(unsigned int i=0; i< args->unsorted_data->mpmt_leds.size(); i++){
      bin=(args->unsorted_data->mpmt_leds.at(i).GetCoarseCounter() & 4194303U);
      args->tmp->mpmt_leds.at(args->arr[bin]-1)=args->unsorted_data->mpmt_leds.at(i);
      args->arr[bin]--;
    }
    
    memset(&args->arr[0], 0, sizeof(*args->arr)*4194304U);
 
    //printf("d12\n");
  }
  ///////////////////////////////////////////


  ///////// sorting Waveforms ////////
  
  //printf("d13 waveforms=%d\n",args->unsorted_data->mpmt_waveforms.size());
  if(args->unsorted_data->mpmt_waveforms.size() > 0){
    for(std::vector<WCTEMPMTWaveform>::iterator it=args->unsorted_data->mpmt_waveforms.begin(); it!= args->unsorted_data->mpmt_waveforms.end(); it++){
      args->arr[(it->header.GetCoarseCounter() & 4194303U)]++;
    }
    for( unsigned int i=1; i<4194304U; i++){
      args->arr[i]+=args->arr[i-1];
    }
    
    args->tmp->mpmt_waveforms.resize(args->unsorted_data->mpmt_waveforms.size());
    
    bin=0;
    for(unsigned int i=0; i< args->unsorted_data->mpmt_waveforms.size(); i++){
      bin=(args->unsorted_data->mpmt_waveforms.at(i).header.GetCoarseCounter() & 4194303U);
      args->tmp->mpmt_waveforms.at(args->arr[bin]-1)=args->unsorted_data->mpmt_waveforms.at(i);
      args->arr[bin]--;
    }

     memset(&args->arr[0], 0, sizeof(*args->arr)*4194304U);
    //printf("d14\n");
  }
  ///////////////////////////////////////////


  ///////// sorting PPS ////////
  
  /*  
  for(std::vector<WCTEMPMTPPS>::iterator it=args->unsorted_data->mpmt_pps.begin(); it!= args->unsorted_data->mpmt_pps.end(); it++){
    args->arr[(it->GetCoarseCounter() & 4194304U)]++;
  }
  for( unsigned int i=1; i<4194304U; i++){
    args->arr[i]+=args->arr[i-1];
  }

  args->tmp->mpmt_pps.resize(args->unsorted_data->mpmt_pps.size());

  bin=0;
  for(unsigned int i=0; i< args->unsorted_data->mpmt_pps.size(); i++){
    bin=(args->unsorted_data->mpmt_pps.at(i).GetCoarseCounter() & 4194304U);
    args->tmp->mpmt_pps.at(args->arr[bin]-1)=args->unsorted_data->mpmt_pps.at(i);
    args->arr[bin]--;
  }

  memset(args->arr, 0, sizeof(args->arr));
  */
  ///////////////////////////////////////////
  
  ///////// sorting Triggers ////////
  //printf("d15 triggers=%d\n",args->unsorted_data->mpmt_triggers.size());
  if(args->unsorted_data->mpmt_triggers.size() > 0){
    for(std::vector<WCTEMPMTHit>::iterator it=args->unsorted_data->mpmt_triggers.begin(); it!= args->unsorted_data->mpmt_triggers.end(); it++){
      args->arr[(it->GetCoarseCounter() & 4194303U)]++;
    }
    for( unsigned int i=1; i<4194304U; i++){
      args->arr[i]+=args->arr[i-1];
    }
    
    args->tmp->mpmt_triggers.resize(args->unsorted_data->mpmt_triggers.size());
    
    bin=0;
    for(unsigned int i=0; i< args->unsorted_data->mpmt_triggers.size(); i++){
      bin=(args->unsorted_data->mpmt_triggers.at(i).GetCoarseCounter() & 4194303U);
      args->tmp->mpmt_triggers.at(args->arr[bin]-1)=args->unsorted_data->mpmt_triggers.at(i);
      args->arr[bin]--; 
    }

     memset(&args->arr[0], 0, sizeof(*args->arr)*4194304U);
    //printf("d16\n");
  }
    ///////////////////////////////////////////
    //////////sort extra hits ///////////////
    
  //  ((coarsecounter & 4194304U)<<3) | (fine >> 13)
  //printf("d17 extrahits=%d\n",args->unsorted_data->extra_hits.size());
  if(args->unsorted_data->extra_hits.size() > 0){
    for(std::vector<WCTEMPMTHit>::iterator it=args->unsorted_data->extra_hits.begin(); it!= args->unsorted_data->extra_hits.end(); it++){
      args->arr[(it->GetCoarseCounter() & 4194303U)]++;
    }
    for( unsigned int i=1; i<4194304U; i++){
      args->arr[i]+=args->arr[i-1];
    }
    
    args->tmp->extra_hits.resize(args->unsorted_data->extra_hits.size());
    
    bin=0;
    for(unsigned int i=0; i< args->unsorted_data->extra_hits.size(); i++){
      bin=(args->unsorted_data->extra_hits.at(i).GetCoarseCounter() & 4194303U);
      args->tmp->extra_hits.at(args->arr[bin]-1)=args->unsorted_data->extra_hits.at(i);
      args->arr[bin]--;
      
    }

     memset(&args->arr[0], 0, sizeof(*args->arr)*4194304U);
    //printf("d18\n");
  }
  /////////////////////////////

  ///////// sorting extra Waveforms ////////
  //printf("d19 extra waveforms=%d\n",args->unsorted_data->extra_waveforms.size());
  if(args->unsorted_data->extra_waveforms.size() > 0){
    for(std::vector<WCTEMPMTWaveform>::iterator it=args->unsorted_data->extra_waveforms.begin(); it!= args->unsorted_data->extra_waveforms.end(); it++){
      args->arr[(it->header.GetCoarseCounter() & 4194303U)]++;
    }
    for( unsigned int i=1; i<4194304U; i++){
      args->arr[i]+=args->arr[i-1];
    }
    
    args->tmp->extra_waveforms.resize(args->unsorted_data->extra_waveforms.size());
    
    bin=0;
    for(unsigned int i=0; i< args->unsorted_data->extra_waveforms.size(); i++){
      bin=(args->unsorted_data->extra_waveforms.at(i).header.GetCoarseCounter() & 4194303U);
      args->tmp->extra_waveforms.at(args->arr[bin]-1)=args->unsorted_data->extra_waveforms.at(i);
      args->arr[bin]--;
    }
    
 memset(&args->arr[0], 0, sizeof(*args->arr)*4194304U);
  }
  ///////////////////////////////////////////
  
    //printf("d20\n");
  //printf("before after sort\n");
  //args->unsorted_data->Print();
  //args->tmp->Print();
  //printf("d11\n");
  delete args->unsorted_data;
//printf("d12\n");
  args->unsorted_data=0;
//printf("d13\n");
//  delete args->tmp;
   //printf("d14\n");
  //   args->tmp=0;
//printf("d15\n");

  args->data->sorted_data_mtx.lock();
  //  unsigned int a=args->tmp->coarse_counter >> 6;
  if(args->data->sorted_data.count(args->tmp->coarse_counter >> 6)) printf("DAAMMMMMMM!!\n");
  args->data->sorted_data[args->tmp->coarse_counter >> 6]=args->tmp;
  //delete args->data->sorted_data[args->tmp->coarse_counter >> 6];
  //args->data->sorted_data[a]=0;
  //args->data->sorted_data.erase(a);
  args->data->sorted_data_mtx.unlock();
  args->tmp=0;
  
  //printf("d21\n");
//printf("d16\n");
 args->data=0;
//printf("d17\n");
 delete[] args->arr;
//printf("d18\n");
 args->arr=0;
//printf("d19\n");
 delete args;
//printf("d20\n");
 args=0;
//printf("d21\n");
 delete args;
//printf("d22\n");
 args=0;
//printf("d23\n");

//printf("d22\n");
  
 return true;
 
}

void Sorting2::LoadConfig(){
  unsigned int pre_sort_wait=0;
  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
  if(!m_variables.Get("pre_sort_wait_sec",pre_sort_wait)) pre_sort_wait=2; 
  args->period=boost::posix_time::seconds(pre_sort_wait);

}

bool Sorting2::FailSort(void* data){

  //printf("w1\n");
  Sorting2_args* args=reinterpret_cast<Sorting2_args*>(data);
  //printf("w2\n");
  args->data=0;
  //printf("w3\n");
  delete args->unsorted_data;
  //printf("w4\n");
  args->unsorted_data=0;
  //printf("w5\n");
  delete args->tmp;
  //printf("w6\n");
  args->tmp=0;
  //printf("w7\n");
  delete[] args->arr;
  //printf("w8\n");
  args->arr=0;
  //printf("w9\n");
  delete args;
  args=0;
  //printf("w10\n");
   return true;
  
}
