#include "Sorting2.h"

Sorting2_args::Sorting2_args():Thread_args(){}

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
     sleep(1);
     return;
   }

   args->last= boost::posix_time::microsec_clock::universal_time();

   //////////////////// selecting what to sort //////////////////////////
   
   std::vector<MPMTData*> data_to_sort;
   std::vector<unsigned int> items_to_remove;

   args->data->unsorted_data_mtx.lock();
  
   //////////// map key is in 33,553,920 ns ~ 33.5ms ////////////////
   for(std::unordered_map<unsigned int, MPMTData*>::iterator it= args->data->unsorted_data.begin(); it!= args->data->unsorted_data.end(); it++){
     if(it->first< ((args->data->current_coarse_counter >>22) - 60)){ //~2 seconds seconds away from current time (60 x ~33.5ms)    
       items_to_remove.push_back(it->first);
       data_to_sort.push_back(it->second);
       it->second=0;
     }

   }

   for(int i=0; i<items_to_remove.size(); i++){
     args->data->unsorted_data.erase(items_to_remove.at(i));
   }
   args->data->unsorted_data_mtx.unlock();

   for(int i=0 ; i<data_to_sort.size(); i++){
     Job* tmp_job= new Job("sorting");
     Sorting2_args* tmp_args = new Sorting2_args;
     tmp_args->data=args->data;
     tmp_args->unsorted_data=data_to_sort.at(i);
     tmp_job->func=SortData;
     tmp_job->data=tmp_args;
     //printf("sending job\n");
        args->data->job_queue.AddJob(tmp_job);
     //printf("added to queue\n");
   }
   
}

bool Sorting2::SortData(void* data){
  //printf("in sortdata\n");
  Sorting2_args* args=reinterpret_cast<Sorting2_args*>(data);
  //printf("h1\n");
  MPMTData* tmp= new MPMTData;
  //printf("h2\n");
  tmp->coarse_counter=args->unsorted_data->coarse_counter;
  //printf("h3\n");
  //  args->unsorted_data->Print();
  
  //  unsigned int* arr =new unsigned int[8388607U];
  //  memset(&arr[0], 0, sizeof(*arr)*8388607);
   //printf("h4\n");
  //////////sort mpmt hits ///////////////
  unsigned int bin=0;
  /*
  
  //printf("d5 hits size=%d\n", args->unsorted_data->mpmt_hits.size());
  if(args->unsorted_data->mpmt_hits.size() > 0){
    for(std::vector<WCTEMPMTHit>::iterator it=args->unsorted_data->mpmt_hits.begin(); it!= args->unsorted_data->mpmt_hits.end(); it++){
      arr[(it->GetCoarseCounter() & 8388607U)]++;
    }
    //printf("d5.5\n");
    
    for( unsigned int i=1; i<8388607U; i++){
      arr[i]+=arr[i-1];
      //    tmp->cumulative_sum[i]=arr[i];
    }
    //printf("d6\n");
    
    //  std::copy(std::begin(arr), std::end(arr), std::begin(tmp->cumulative_sum));
    memcpy(&tmp->cumulative_sum[0], &arr[0], sizeof(*arr)*8388607);
    //std::copy(&arr[0], &arr[194302]), std::begin(tmp->cumulative_sum));
    //printf("d7\n");
    //printf("d7.1 size=%d\n",args->unsorted_data->mpmt_hits.size());
    //printf("d7.2 size=%d\n",tmp->mpmt_hits.size());
    tmp->mpmt_hits.resize(args->unsorted_data->mpmt_hits.size());
    //printf("d8\n");
    
    //printf("d8.1\n");
    for(unsigned int i=0; i< args->unsorted_data->mpmt_hits.size(); i++){
      bin=(args->unsorted_data->mpmt_hits.at(i).GetCoarseCounter() & 8388607U);
      tmp->mpmt_hits.at(arr[bin]-1)=args->unsorted_data->mpmt_hits.at(i);
      arr[bin]--;
      
    }
    //printf("d9\n");
    memset(&arr[0], 0, sizeof(*arr)*8388607);
    //printf("d10\n");
  }
  /////////////////////////////
  /* bool proof=false;
  for(unsigned int i=1; i <tmp->mpmt_hits.size(); i++){
    //   printf(" %u:%u\n",i,tmp->mpmt_hits.at(i).GetCoarseCounter());
    if(tmp->mpmt_hits.at(i).GetCoarseCounter() <tmp->mpmt_hits.at(i-1).GetCoarseCounter()){
      //printf(" badsort %u,%u\n",i, tmp->mpmt_hits.size());
      //printf(" %u:%u\n",tmp->mpmt_hits.at(i-1).GetCoarseCounter(), tmp->mpmt_hits.at(i).GetCoarseCounter());
      //printf(" lower bits %u:%u\n",(tmp->mpmt_hits.at(i-1).GetCoarseCounter() & 8388607U), (tmp->mpmt_hits.at(i).GetCoarseCounter()& 8388607U));
      proof=true;
    }
  }
  if(proof){
 for(unsigned int i=1; i <tmp->mpmt_hits.size(); i++){
   //printf(" proof %u, %u, %u: %u, %u\n",i, tmp->mpmt_hits.at(i).GetCoarseCounter(), (tmp->mpmt_hits.at(i).GetCoarseCounter() & 8388607U),(tmp->coarse_counter & 65535U), (tmp->mpmt_hits.at(i).GetCoarseCounter() >> 16));
 }
  
}
*/
  //printf("end\n\n");
  ///////// sorting LEDS ////////
  /*
  //printf("d11 leds-%d\n",args->unsorted_data->mpmt_leds.size());
  if(args->unsorted_data->mpmt_leds.size() > 0){
    for(std::vector<WCTEMPMTLED>::iterator it=args->unsorted_data->mpmt_leds.begin(); it!= args->unsorted_data->mpmt_leds.end(); it++){
      arr[(it->GetCoarseCounter() & 8388607U)]++;
    }
    for( unsigned int i=1; i<8388607U; i++){
      arr[i]+=arr[i-1];
    }
    
    tmp->mpmt_leds.resize(args->unsorted_data->mpmt_leds.size());
    
    bin=0;
    for(unsigned int i=0; i< args->unsorted_data->mpmt_leds.size(); i++){
      bin=(args->unsorted_data->mpmt_leds.at(i).GetCoarseCounter() & 8388607U);
      tmp->mpmt_leds.at(arr[bin]-1)=args->unsorted_data->mpmt_leds.at(i);
      arr[bin]--;
    }
    
    memset(&arr[0], 0, sizeof(*arr)*8388607);
 
    //printf("d12\n");
  }
  ///////////////////////////////////////////


  ///////// sorting Waveforms ////////
  
  //printf("d13 waveforms=%d\n",args->unsorted_data->mpmt_waveforms.size());
  if(args->unsorted_data->mpmt_waveforms.size() > 0){
    for(std::vector<WCTEMPMTWaveform>::iterator it=args->unsorted_data->mpmt_waveforms.begin(); it!= args->unsorted_data->mpmt_waveforms.end(); it++){
      arr[(it->header.GetCoarseCounter() & 8388607U)]++;
    }
    for( unsigned int i=1; i<8388607U; i++){
      arr[i]+=arr[i-1];
    }
    
    tmp->mpmt_waveforms.resize(args->unsorted_data->mpmt_waveforms.size());
    
    bin=0;
    for(unsigned int i=0; i< args->unsorted_data->mpmt_waveforms.size(); i++){
      bin=(args->unsorted_data->mpmt_waveforms.at(i).header.GetCoarseCounter() & 8388607U);
      tmp->mpmt_waveforms.at(arr[bin]-1)=args->unsorted_data->mpmt_waveforms.at(i);
      arr[bin]--;
    }

     memset(&arr[0], 0, sizeof(*arr)*8388607);
    //printf("d14\n");
  }
  ///////////////////////////////////////////


  ///////// sorting PPS ////////
  
  /*  
  for(std::vector<WCTEMPMTPPS>::iterator it=args->unsorted_data->mpmt_pps.begin(); it!= args->unsorted_data->mpmt_pps.end(); it++){
    arr[(it->GetCoarseCounter() & 4194303U)]++;
  }
  for( unsigned int i=1; i<4194303U; i++){
    arr[i]+=arr[i-1];
  }

  tmp->mpmt_pps.resize(args->unsorted_data->mpmt_pps.size());

  bin=0;
  for(unsigned int i=0; i< args->unsorted_data->mpmt_pps.size(); i++){
    bin=(args->unsorted_data->mpmt_pps.at(i).GetCoarseCounter() & 4194303U);
    tmp->mpmt_pps.at(arr[bin]-1)=args->unsorted_data->mpmt_pps.at(i);
    arr[bin]--;
  }

  memset(arr, 0, sizeof(arr));
  */
  ///////////////////////////////////////////
  /*
  ///////// sorting Triggers ////////
  //printf("d15 triggers=%d\n",args->unsorted_data->mpmt_triggers.size());
  if(args->unsorted_data->mpmt_triggers.size() > 0){
    for(std::vector<WCTEMPMTHit>::iterator it=args->unsorted_data->mpmt_triggers.begin(); it!= args->unsorted_data->mpmt_triggers.end(); it++){
      arr[(it->GetCoarseCounter() & 8388607U)]++;
    }
    for( unsigned int i=1; i<8388607U; i++){
      arr[i]+=arr[i-1];
    }
    
    tmp->mpmt_triggers.resize(args->unsorted_data->mpmt_triggers.size());
    
    bin=0;
    for(unsigned int i=0; i< args->unsorted_data->mpmt_triggers.size(); i++){
      bin=(args->unsorted_data->mpmt_triggers.at(i).GetCoarseCounter() & 8388607U);
      tmp->mpmt_triggers.at(arr[bin]-1)=args->unsorted_data->mpmt_triggers.at(i);
      arr[bin]--; 
    }

     memset(&arr[0], 0, sizeof(*arr)*8388607);
    //printf("d16\n");
  }
    ///////////////////////////////////////////
    //////////sort extra hits ///////////////
    
  //  ((coarsecounter & 4194303U)<<3) | (fine >> 13)
  //printf("d17 extrahits=%d\n",args->unsorted_data->extra_hits.size());
  if(args->unsorted_data->extra_hits.size() > 0){
    for(std::vector<WCTEMPMTHit>::iterator it=args->unsorted_data->extra_hits.begin(); it!= args->unsorted_data->extra_hits.end(); it++){
      arr[(it->GetCoarseCounter() & 8388607U)]++;
    }
    for( unsigned int i=1; i<8388607U; i++){
      arr[i]+=arr[i-1];
    }
    
    tmp->extra_hits.resize(args->unsorted_data->extra_hits.size());
    
    bin=0;
    for(unsigned int i=0; i< args->unsorted_data->extra_hits.size(); i++){
      bin=(args->unsorted_data->extra_hits.at(i).GetCoarseCounter() & 8388607U);
      tmp->extra_hits.at(arr[bin]-1)=args->unsorted_data->extra_hits.at(i);
      arr[bin]--;
      
    }

     memset(&arr[0], 0, sizeof(*arr)*8388607);
    //printf("d18\n");
  }
  /////////////////////////////

  ///////// sorting extra Waveforms ////////
  //printf("d19 extra waveforms=%d\n",args->unsorted_data->extra_waveforms.size());
  if(args->unsorted_data->extra_waveforms.size() > 0){
    for(std::vector<WCTEMPMTWaveform>::iterator it=args->unsorted_data->extra_waveforms.begin(); it!= args->unsorted_data->extra_waveforms.end(); it++){
      arr[(it->header.GetCoarseCounter() & 8388607U)]++;
    }
    for( unsigned int i=1; i<8388607U; i++){
      arr[i]+=arr[i-1];
    }
    
    tmp->extra_waveforms.resize(args->unsorted_data->extra_waveforms.size());
    
    bin=0;
    for(unsigned int i=0; i< args->unsorted_data->extra_waveforms.size(); i++){
      bin=(args->unsorted_data->extra_waveforms.at(i).header.GetCoarseCounter() & 8388607U);
      tmp->extra_waveforms.at(arr[bin]-1)=args->unsorted_data->extra_waveforms.at(i);
      arr[bin]--;
    }
    
 memset(&arr[0], 0, sizeof(*arr)*8388607);
  }
  ///////////////////////////////////////////
  */
    //printf("d20\n");
  //printf("before after sort\n");
  //args->unsorted_data->Print();
  //tmp->Print();
  delete args->unsorted_data;
  args->unsorted_data=0;
  /*
  args->data->sorted_data_mtx.lock();
  args->data->sorted_data[tmp->coarse_counter >> 6]=tmp;
  args->data->sorted_data_mtx.unlock();
  */
  //printf("d21\n");
  args->data=0;
  delete args;
  args=0;
  //  delete[] arr;
  //arr=0;
  //printf("d22\n");
  
 return true;
 
}

void Sorting2::LoadConfig(){
  unsigned int pre_sort_wait=0;
  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
  if(!m_variables.Get("pre_sort_wait_sec",pre_sort_wait)) pre_sort_wait=2; 
  args->period=boost::posix_time::seconds(pre_sort_wait);

}
