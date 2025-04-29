#include "Monitoring.h"

int parseLine(char* line){
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

Monitoring_args::Monitoring_args():Thread_args(){
  pps = new std::vector<WCTEMPMTPPS>{};
}

Monitoring_args::~Monitoring_args(){
  delete pps;
  delete sock;
  sock=0;
}


Monitoring::Monitoring():Tool(){}


bool Monitoring::Initialise(std::string configfile, DataModel &data){

  InitialiseTool(data);
  m_configfile=configfile;
  InitialiseConfiguration(configfile);
  //m_variables.Print();



  m_util=new Utilities();
  args=new Monitoring_args();
  args->last =  boost::posix_time::microsec_clock::universal_time();
  args->data = m_data;
  args->last2 =  boost::posix_time::microsec_clock::universal_time();
  args->period2 =  boost::posix_time::seconds(1);
  args->sock = new zmq::socket_t(*(m_data->context), ZMQ_PUB);
  args->sock->bind("tcp://*:2424");
  m_data->mon_pps = new std::vector<WCTEMPMTPPS>{};
  LoadConfig();
  
  m_util->CreateThread("test", &Thread, args);

  ExportConfiguration();

  mem=0.0;
  cpu=0.0;
  
  return true;
}


bool Monitoring::Execute(){

  if(m_data->change_config){
    InitialiseConfiguration(m_configfile);
    LoadConfig();
    ExportConfiguration();
  }
  if(m_data->run_start){
    
    // @ben what's the point of setting a bunch of unsigned ints to 0 only to clear the map anyway? FIXME
    for(std::map<std::string, unsigned int>::iterator it= args->data->hit_map.begin(); it!=args->data->hit_map.end(); it++){
      args->data->hit_map[it->first]=0; 
    }
    args->data->hit_map.clear();
  }
 
  std::stringstream tmp;
  std::string runinfo="";
  unsigned long part=0;
  unsigned long workers=0;
  unsigned long buffer_diff=0;  
  
  m_data->vars.Get("Runinfo",runinfo);
  m_data->vars.Get("part",part);
  m_data->monitoring_store.Get("pool_threads",workers);
  m_data->monitoring_store.Get("recevie_buffer_max_diff",buffer_diff);
  if(m_data->current_coarse_counter < cc) cc = m_data->current_coarse_counter;
  if(m_data->current_coarse_counter > cc + 625000000){
    rate=(m_data->preadout_windows->size()-out_size)/5.0;
    cc = m_data->current_coarse_counter;
    out_size=m_data->preadout_windows->size();
    //    printf("HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH rate=%f out_size= %u\n", rate, out_size);
        
    struct sysinfo memInfo;
    sysinfo (&memInfo);
    /*
    mem= (((float) memInfo.totalram - (float)memInfo.freeram)/((float)memInfo.totalram)) * 100;
    */



    //// Get mem
    FILE* file2 = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file2) != NULL){
        if (strncmp(line, "VmRSS:", 6) == 0){
            result = parseLine(line);
            break;
        }
    }
    fclose(file2); 

    mem= (((float)result * 1000)/ ((float) memInfo.totalram))*100;
    while(mem > 100.0) mem= mem/1000.0;


    // get cpu
    float percent;
    FILE* file;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;



    file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
	   &totalSys, &totalIdle);
    fclose(file);
    
    if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
	totalSys < lastTotalSys || totalIdle < lastTotalIdle){
      //Overflow detection. Just skip this value.
      cpu = -1.0;
    }
    else{
      total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
	(totalSys - lastTotalSys);
      cpu = total;
      total += (totalIdle - lastTotalIdle);
      cpu /= total;
      cpu *= 100;
    }
  
    lastTotalUser = totalUser;
    lastTotalUserLow = totalUserLow;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;
    
    m_data->monitoring_store_mtx.lock();
    m_data->monitoring_store.Set("cpu",cpu);
    m_data->monitoring_store.Set("mem",mem);
    m_data->monitoring_store_mtx.unlock();
    
  }
  
  //  tmp<< runinfo<<" buffers: unsorted| sorted| triggered| readout = "<<m_data->unsorted_data.size()<<"| "<<m_data->sorted_data.size()<<"| "<<m_data->triggered_data.size()<<"| "<<m_data->readout_windows->size()<<" (files="<<part<<") jobs:workers = "<<m_data->job_queue.size()<<":"<<workers<<" ["<<m_data->data_chunks.size()<<":"<<m_data->out_data_chunks->size()<<":"<<m_data->preadout_windows->size()<<"]"<<buffer_diff<<" ["<<m_data->mpmt_messages->size()<<"]";
  tmp<< runinfo<<" buffers: raw| chunked| collated| readout = "<<" ["<<m_data->mpmt_messages->size()<<":"<<m_data->data_chunks.size()<<":"<<m_data->out_data_chunks->size()<<":"<<m_data->preadout_windows->size()<<"]"<<"buffer_diff="<<buffer_diff<<" spill="<<m_data->spill_num<<" (files="<<part<<") jobs:workers = "<<m_data->job_queue.size()<<":"<<workers<<" rate ~ "<<rate<<"Hz "<<"mem="<<mem<<"% cpu="<<cpu<<"%";

  m_data->vars.Set("Status",tmp.str());
  
  return true;
}


bool Monitoring::Finalise(){

  m_util->KillThread(args);

  delete args;
  args=0;

  delete m_util;
  m_util=0;

  return true;
}

void Monitoring::Thread(Thread_args* arg){

  Monitoring_args* args=reinterpret_cast<Monitoring_args*>(arg);


  args->lapse2 = args->period2 -( boost::posix_time::microsec_clock::universal_time() - args->last2);
  
  if(args->lapse.is_negative() ){
    unsigned int tmp=0;
   
    for(std::map<std::string, unsigned int>::iterator it= args->data->hit_map.begin(); it!=args->data->hit_map.end(); it++){
      args->hit_rates.Get(it->first,tmp);
      tmp = it->second - tmp;
      args->hit_rates.Set(it->first,tmp);
      //      args->hit_rates.Print();
    }
    std::string json="";
    args->hit_rates>>json;

    args->data->services->SendMonitoringData(json,"message_rates");
    
    // hit rate monitoring
    std::map<unsigned int, unsigned int[132]> local_hitrates;
    args->data->hitrates_mtx.lock();
    std::swap(local_hitrates, args->data->hitrates);
    args->data->hitrates_mtx.unlock();
    
    // translate it into JSON. Gonna raw dog it cos i don't trust any of our JSON stuff at this point.
    // original version: send rates in each time bin. This produces monitoring messages that are too large :(
    json="{";
    boost::posix_time::ptime m_bin_time;
    // loop over time bins
    for(std::map<unsigned int,unsigned int[132]>::iterator it=local_hitrates.begin(); it!=local_hitrates.end(); ++it){
      if(it!=local_hitrates.begin()){
        json += ", ";
      }
      // it->first is bin number, which is DAQ header coarse counter shifted down by 7 bits.
      // DAQ header coarse counter is bits 16+ (bits 0-15 are in hit)
      // so shifting down by 7 bits means LSB is 2^(16+7) = 2^23 = 8388608 CC or 8388608*8 = 67108864 ns ~67ms
      m_bin_time = args->data->start_time + boost::posix_time::milliseconds(it->first*67.108864);
      //uint64_t msecs_since_epoch= boost::posix_time::time_duration(m_bin_time - boost::posix_time::time_from_string("1970-01-01 00:00:00.000")).total_milliseconds();
      std::string tmps = boost::posix_time::to_iso_extended_string(m_bin_time);
      tmps[19]='.'; // for some reason to_iso_string output is 'YYYY-MM-DDTHH:MM:SS,fffffffff' - why a comma, not a decimal point?
      json += "\"" + tmps +"\": {";
      bool addcomma=false;
      // for each time bin, send number of hits on each MPMT in that time bin.
      for(size_t i=0; i<132; ++i){
        // skip MPMTs that had no hits
        if(it->second[i]!=0){
          if(addcomma) json += ", ";
          json += "\""+std::to_string(i)+"\": "+std::to_string(it->second[i]);
          addcomma=true;
        }
      }
      json += " }";
      break; // only send first bin. :( messages tooo looooong
    }
    json += "}";
    
    /*
    // stripped down version - merge time bins.
    static unsigned long sum_rates[132];
    for(std::map<unsigned int,unsigned int[132]>::iterator it=local_hitrates.begin(); it!=local_hitrates.end(); ++it){
      for(size_t i=0; i<132; ++i) sum_rates[i] += (it->second[i]);
    }
    m_bin_time = args->data->start_time + boost::posix_time::milliseconds(local_hitrates.begin()->first*67.108864);
    std::string tmps = boost::posix_time::to_iso_extended_string(m_bin_time);
    tmps[19]='.';
    json = "{\"" + tmps +"\": {";
    bool addcomma=false;
    for(size_t i=0; i<132; ++i){
      if(sum_rates[i]!=0){
        if(addcomma) json += ", ";
        json += "\""+std::to_string(i)+"\": "+std::to_string(sum_rates[i]);
        addcomma=true;
      }
    }
    json +="} }";
    */
    
    args->data->services->SendMonitoringData(json,"hit_rates");
    
    args->last2 = boost::posix_time::microsec_clock::universal_time();
  
  }
  
  
  args->lapse = args->period -( boost::posix_time::microsec_clock::universal_time() - args->last);
  //std::cout<< m_lapse<<std::endl;
  
  if(!args->lapse.is_negative() ){
    usleep(100);
    return;
  }
    //printf("in runstart lapse\n");
    
    std::string json="";
    args->data->monitoring_store_mtx.lock();
    args->data->monitoring_store.Set("running",args->data->running);
    args->data->monitoring_store>>json;
    args->data->monitoring_store_mtx.unlock();
    args->data->services->SendMonitoringData(json);
    
    /*
    args->data->monitoring_stream_mtx.lock();
    args->data->monitoring_stream>>json;
    args->data->monitoring_stream.Delete();
    args->data->monitoring_stream_mtx.unlock();
    args->data->services->SendMonitoringData(json);
    */

    // grab sample of readout windows
    std::vector<PReadoutWindow*> windows;
    args->data->mon_preadout_windows_mtx.lock();
    std::swap(args->data->mon_preadout_windows,windows);
    args->data->mon_preadout_windows_mtx.unlock();


    for( unsigned int i=0 ; i<windows.size(); i++){
      if(!args->data->hardware_trigger) windows.at(i)->Send(args->sock);

      delete windows.at(i);
      windows.at(i)=0;
    }

    windows.clear();
    
    // grab the PPS frames
    args->data->mon_pps_mtx.lock();
    std::swap(args->pps, args->data->mon_pps);
    args->data->mon_pps_mtx.unlock();

    if(args->pps->size()!=0){

      std::string identifier="PPS";
      zmq::message_t identifier_msg(identifier.length()+1);
      memcpy(identifier_msg.data(), identifier.data(), identifier.length()+1);
      args->sock->send(identifier_msg, ZMQ_SNDMORE);
      
      unsigned int tmp_size = args->pps->size();
      zmq::message_t size(tmp_size);
      memcpy(size.data(), &tmp_size, sizeof tmp_size);
      args->sock->send(size, ZMQ_SNDMORE);
      
      for(unsigned int i=0; i< args->pps->size()-1; i++){
	
	args->pps->at(i).Send(args->sock, ZMQ_SNDMORE);
	
      }
      
      args->pps->at(args->pps->size()-1).Send(args->sock);
      
      
    }
    
    
    // FIXME add code to do something with these
    
    args->pps->resize(0);
    args->pps->shrink_to_fit();
    
    args->last = boost::posix_time::microsec_clock::universal_time();
    
    
}

bool Monitoring::LoadConfig(){

  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
  unsigned int period_sec=0;
  if(!m_variables.Get("period_sec",period_sec)) period_sec=120;
  args->period = boost::posix_time::seconds(period_sec);
  
  // request RootWriter to put every Nth PReadoutWindow into the datamodel for monitoring to analyse
  unsigned int readout_ratio=0; // every N'th, none by default
  m_variables.Get("readout_ratio",readout_ratio);
  m_data->mon_preadout_ratio = readout_ratio;
  
  out_size=0;
  cc=0;
  rate=0;
  
  return true;
  
}
