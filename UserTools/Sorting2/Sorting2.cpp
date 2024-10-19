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
  
  m_data->monitoring_store.Set("unsorted_data_size",m_data->unsorted_data.size());
  m_data->monitoring_store.Set("sorted_data_size",m_data->sorted_data.size());
  
  
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
   for(std::map<unsigned int, MPMTData*>::iterator it= args->data->unsorted_data.begin(); it!= args->data->unsorted_data.end(); it++){
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
     args->data->job_queue.AddJob(tmp_job);
   }
   
}

bool Sorting2::SortData(void* data){
  
  Sorting2_args* args=reinterpret_cast<Sorting2_args*>(data);
  
  MPMTData* tmp= new MPMTData;
  tmp->coarse_counter=args->unsorted_data->coarse_counter;
  
  unsigned int arr[4194303U] = { 0 };
  
  //////////sort mpmt hits ///////////////

  
  for(std::vector<WCTEMPMTHit>::iterator it=args->unsorted_data->mpmt_hits.begin(); it!= args->unsorted_data->mpmt_hits.end(); it++){
    arr[(it->GetCoarseCounter() & 4194303U)]++;
  }
  for( unsigned int i=1; i<4194303U; i++){
    arr[i]+=arr[i-1];
    //    tmp->cumulative_sum[i]=arr[i];
  }

  std::copy(std::begin(arr), std::end(arr), std::begin(tmp->cumulative_sum));

  tmp->mpmt_hits.resize(args->unsorted_data->mpmt_hits.size());

  unsigned int bin=0;
  for(unsigned int i=0; i< args->unsorted_data->mpmt_hits.size(); i++){
    bin=(args->unsorted_data->mpmt_hits.at(i).GetCoarseCounter() & 4194303U);
    tmp->mpmt_hits.at(arr[bin]-1)=args->unsorted_data->mpmt_hits.at(i);
    arr[bin]--;
    
  }

  memset(arr, 0, sizeof(arr));

  /////////////////////////////

  ///////// sorting LEDS ////////
  
  
  for(std::vector<WCTEMPMTLED>::iterator it=args->unsorted_data->mpmt_leds.begin(); it!= args->unsorted_data->mpmt_leds.end(); it++){
    arr[(it->GetCoarseCounter() & 4194303U)]++;
  }
  for( unsigned int i=1; i<4194303U; i++){
    arr[i]+=arr[i-1];
  }

  tmp->mpmt_leds.resize(args->unsorted_data->mpmt_leds.size());

  bin=0;
  for(unsigned int i=0; i< args->unsorted_data->mpmt_leds.size(); i++){
    bin=(args->unsorted_data->mpmt_leds.at(i).GetCoarseCounter() & 4194303U);
    tmp->mpmt_leds.at(arr[bin]-1)=args->unsorted_data->mpmt_leds.at(i);
    arr[bin]--;
  }

  memset(arr, 0, sizeof(arr));

  ///////////////////////////////////////////


  ///////// sorting Waveforms ////////
  
  
  for(std::vector<WCTEMPMTWaveform>::iterator it=args->unsorted_data->mpmt_waveforms.begin(); it!= args->unsorted_data->mpmt_waveforms.end(); it++){
    arr[(it->header.GetCoarseCounter() & 4194303U)]++;
  }
  for( unsigned int i=1; i<4194303U; i++){
    arr[i]+=arr[i-1];
  }

  tmp->mpmt_waveforms.resize(args->unsorted_data->mpmt_waveforms.size());

  bin=0;
  for(unsigned int i=0; i< args->unsorted_data->mpmt_waveforms.size(); i++){
    bin=(args->unsorted_data->mpmt_waveforms.at(i).header.GetCoarseCounter() & 4194303U);
    tmp->mpmt_waveforms.at(arr[bin]-1)=args->unsorted_data->mpmt_waveforms.at(i);
    arr[bin]--;
  }

  memset(arr, 0, sizeof(arr));

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

  ///////// sorting Triggers ////////
  
  
  for(std::vector<WCTEMPMTHit>::iterator it=args->unsorted_data->mpmt_triggers.begin(); it!= args->unsorted_data->mpmt_triggers.end(); it++){
    arr[(it->GetCoarseCounter() & 4194303U)]++;
  }
  for( unsigned int i=1; i<4194303U; i++){
    arr[i]+=arr[i-1];
  }

  tmp->mpmt_triggers.resize(args->unsorted_data->mpmt_triggers.size());

  bin=0;
  for(unsigned int i=0; i< args->unsorted_data->mpmt_triggers.size(); i++){
    bin=(args->unsorted_data->mpmt_triggers.at(i).GetCoarseCounter() & 4194303U);
    tmp->mpmt_triggers.at(arr[bin]-1)=args->unsorted_data->mpmt_triggers.at(i);
    arr[bin]--; 
  }


  ///////////////////////////////////////////
    //////////sort extra hits ///////////////

  //  ((coarsecounter & 4194303U)<<3) | (fine >> 13)
  
  for(std::vector<WCTEMPMTHit>::iterator it=args->unsorted_data->extra_hits.begin(); it!= args->unsorted_data->extra_hits.end(); it++){
    arr[(it->GetCoarseCounter() & 4194303U)]++;
  }
  for( unsigned int i=1; i<4194303U; i++){
    arr[i]+=arr[i-1];
  }

  tmp->extra_hits.resize(args->unsorted_data->extra_hits.size());

  bin=0;
  for(unsigned int i=0; i< args->unsorted_data->extra_hits.size(); i++){
    bin=(args->unsorted_data->extra_hits.at(i).GetCoarseCounter() & 4194303U);
    tmp->extra_hits.at(arr[bin]-1)=args->unsorted_data->extra_hits.at(i);
    arr[bin]--;
    
  }
  
  memset(arr, 0, sizeof(arr));

  /////////////////////////////

  ///////// sorting extra Waveforms ////////
  
  
  for(std::vector<WCTEMPMTWaveform>::iterator it=args->unsorted_data->extra_waveforms.begin(); it!= args->unsorted_data->extra_waveforms.end(); it++){
    arr[(it->header.GetCoarseCounter() & 4194303U)]++;
  }
  for( unsigned int i=1; i<4194303U; i++){
    arr[i]+=arr[i-1];
  }

  tmp->extra_waveforms.resize(args->unsorted_data->extra_waveforms.size());

  bin=0;
  for(unsigned int i=0; i< args->unsorted_data->extra_waveforms.size(); i++){
    bin=(args->unsorted_data->extra_waveforms.at(i).header.GetCoarseCounter() & 4194303U);
    tmp->extra_waveforms.at(arr[bin]-1)=args->unsorted_data->extra_waveforms.at(i);
    arr[bin]--;
  }

  memset(arr, 0, sizeof(arr));

  ///////////////////////////////////////////

    
  
  delete args->unsorted_data;
  args->unsorted_data=0;
  args->data->sorted_data_mtx.lock();
  args->data->sorted_data[tmp->coarse_counter >> 6]=tmp;
  args->data->sorted_data_mtx.unlock();
  
  args->data=0;
  delete args;
  args=0;
  
  return true;
}

void Sorting2::LoadConfig(){
  unsigned int pre_sort_wait;
  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
  if(!m_variables.Get("pre_sort_wait_sec",pre_sort_wait)) pre_sort_wait=2; 
  args->period=boost::posix_time::seconds(pre_sort_wait);

}
