#include "MPMT.h"

MPMTMessages::MPMTMessages(){
  daq_header=0;
  mpmt_data=0;
  m_data=0;
}

MPMTMessages::~MPMTMessages(){
  delete daq_header;
  daq_header=0;
  delete mpmt_data;
  mpmt_data=0;
  m_data=0;
  
}

MPMT_args::MPMT_args():Thread_args(){
  data_sock=0;
  utils=0;
  job_queue=0;
  m_data=0;

}

MPMT_args::~MPMT_args(){
  delete data_sock;
  data_sock=0;
  delete utils;
  utils=0;

  for(std::map<std::string,Store*>::iterator it=connections.begin(); it!= connections.end(); it++){
    delete it->second;
    it->second=0;
  }

  connections.clear();
  job_queue=0;
  m_data=0;
  
}


MPMT::MPMT():Tool(){}


bool MPMT::Initialise(std::string configfile, DataModel &data){

  InitialiseTool(data);
  InitialiseConfiguration(configfile);
  //m_variables.Print();
  
  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
  if(!m_variables.Get("MPMT_port",m_mpmt_port)) m_mpmt_port="4444";
  if(!m_variables.Get("MPMT_search_period_sec",m_mpmt_search_period_sec)) m_mpmt_search_period_sec=10;
 
  m_util=new Utilities();

  //laod port number maybe
  //load rounding of bins masybe
  
  m_threadnum=0;
  CreateThread();
  m_freethreads=1;
  
  ExportConfiguration();    
  
  return true;
}


bool MPMT::Execute(){

  for(std::map<std::string, unsigned int>::iterator it=m_data->hit_map.begin(); it!=m_data->hit_map.end(); it++){

    m_data->monitoring_store.Set(it->first, it->second);

  }
  
  /*
  for(unsigned int i=0; i<args.size(); i++){
    if(args.at(i)->busy==0){
      *m_log<<"reply="<<args.at(i)->message<<std::endl;
      args.at(i)->message="Hi";
      args.at(i)->busy=1;
      break;
    }

  }

  m_freethreads=0;
  unsigned int lastfree=0;
  for(unsigned int i=0; i<args.size(); i++){
    if(args.at(i)->busy==0){
      m_freethreads++;
      lastfree=i; 
    }
  }

  if(m_freethreads<1) CreateThread();
  if(m_freethreads>1) DeleteThread(lastfree);
  
  *m_log<<ML(1)<<"free threads="<<m_freethreads<<":"<<args.size()<<std::endl;
  MLC();
  
  // sleep(1);  for single tool testing
  */
  return true;
}


bool MPMT::Finalise(){

  for(unsigned int i=0;i<args.size();i++) DeleteThread(0);
  
  args.clear();
  
  delete m_util;
  m_util=0;

  return true;
}

void MPMT::CreateThread(){

  MPMT_args* tmparg=new MPMT_args();
 
  tmparg->data_sock=new zmq::socket_t(*(m_data->context), ZMQ_ROUTER);
  tmparg->data_port=m_mpmt_port;
  tmparg->utils= new DAQUtilities(m_data->context);
  
  tmparg->items[0].socket=*tmparg->data_sock;
  tmparg->items[0].fd=0;
  tmparg->items[0].events=ZMQ_POLLIN;
  tmparg->items[0].revents=0;
  
  tmparg->period=boost::posix_time::seconds(m_mpmt_search_period_sec);
  
  tmparg->last=boost::posix_time::microsec_clock::universal_time();
  tmparg->m_data=m_data;
  tmparg->job_queue=&(m_data->job_queue);
  
  args.push_back(tmparg);
  std::stringstream tmp;
  tmp<<"T"<<m_threadnum;
  m_util->CreateThread(tmp.str(), &Thread, args.at(args.size()-1));
  m_threadnum++;
  m_data->thread_num++;

}

 void MPMT::DeleteThread(unsigned int pos){

   m_util->KillThread(args.at(pos));

   delete args.at(pos)->data_sock;
   args.at(pos)->data_sock=0;

   delete args.at(pos)->utils;
   args.at(pos)->utils=0;
   
   delete args.at(pos);
   args.at(pos)=0;
   args.erase(args.begin()+(pos));
   

 }

void MPMT::Thread(Thread_args* arg){

  MPMT_args* args=reinterpret_cast<MPMT_args*>(arg);

  args->lapse = args->period -( boost::posix_time::microsec_clock::universal_time() - args->last);
  
  if( args->lapse.is_negative()){
    //printf("in lapse \n");
    unsigned short num_connections = args->connections.size();
    if(args->utils->UpdateConnections("MPMT", args->data_sock, args->connections, args->data_port) > num_connections) args->m_data->services->SendLog("Info: New MPMT connected",v_message); //add pmt id
    args->m_data->monitoring_store.Set("connected_MPMTs",num_connections);
    args->last= boost::posix_time::microsec_clock::universal_time();
    printf("conenctions=%d\n",args->connections.size());
  }
  
  zmq::poll(&(args->items[0]), 1, 100);
  
  if(args->items[0].revents & ZMQ_POLLIN){
    printf("received data\n");
    
    zmq::message_t identity;
    zmq::message_t* daq_header = new zmq::message_t;
    zmq::message_t* mpmt_data = new zmq::message_t;

    args->message_received=false;
    args->no_data=false;
    
    args->message_received=args->data_sock->recv(&identity);     

    if(!identity.more() || !args->message_received || identity.size() == 0){
      args->m_data->services->SendLog("Warning: MPMT thread identity has no size or only message",3);
      delete mpmt_data;
      mpmt_data=0;
      delete daq_header;
      daq_header=0;
      return;
    }
    
    args->message_received=args->data_sock->recv(daq_header);
    
    if(!args->message_received || daq_header->size()!=DAQHeader::GetSize() ){
      args->m_data->services->SendLog("Warning: MPMT thread daq header has no or incorrect size",3);
      delete mpmt_data;
      mpmt_data=0;
      delete daq_header;
      daq_header=0;
      return;
    }
    if(!daq_header->more()) args->no_data=true;
    else{
      args->message_received=args->data_sock->recv(mpmt_data);
      if(mpmt_data->more() || !args->message_received || mpmt_data->size() == 0){
	args->m_data->services->SendLog("ERROR: MPMT thread too many message parts or no data, throwing away data",2); //add mpmtid
	zmq::message_t throwaway;
	if(mpmt_data->more()){
	  args->data_sock->recv(&throwaway);
	  while(throwaway.more()) args->data_sock->recv(&throwaway);
	}
	delete mpmt_data;
	mpmt_data=0;
	delete daq_header;
	daq_header=0;
	return;
      }
    }
    //printf("data fine\n");
	
    zmq::message_t reply(4);
    memcpy(reply.data(), daq_header->data(), sizeof(reply));
    //printf("sending reply\n");
    args->data_sock->send(identity, ZMQ_SNDMORE); /// need to add checking probablly a poll incase sender dies
    args->data_sock->send(reply);
    //printf("sent reply\n");
    
    if(args->no_data){
      delete mpmt_data;
      mpmt_data=0;
      delete daq_header;
      daq_header=0;
    }
    else{
      //printf("creating job\n");
      Job* tmp_job= new Job("MPMT");
      MPMTMessages* tmp_msgs= new MPMTMessages;
      tmp_msgs->daq_header=daq_header;
      tmp_msgs->mpmt_data=mpmt_data;
      tmp_msgs->m_data=args->m_data;
      tmp_job->data= tmp_msgs;
      //printf("d1\n");
      tmp_job->func=ProcessData;
      //printf("d2\n");
      args->job_queue->AddJob(tmp_job);
      printf("job submitted %d\n",args->job_queue->size());
    }
  }
  
} //job deletion needed



bool MPMT::ProcessData(void* data){

  printf("in process data\n");
  MPMTMessages* msgs=reinterpret_cast<MPMTMessages*>(data);
 printf("d1\n");  
  DAQHeader* daq_header=reinterpret_cast<DAQHeader*>(msgs->daq_header->data());
  unsigned int bin= daq_header->GetCoarseCounter() >> 6; //might not be worth rounding
  unsigned short card_id = daq_header->GetCardID();
  unsigned short card_type = daq_header->GetCardType();
  unsigned long bytes=msgs->mpmt_data->size();
  unsigned long current_byte=0;
  printf("d2 car_id=%d\n", card_id);
  //  daq_header->Print();
  std::vector<WCTEMPMTHit> vec_mpmt_hit;
  std::vector<WCTEMPMTLED> vec_mpmt_led;
  std::vector<WCTEMPMTPPS> vec_mpmt_pps;
  std::vector<WCTEMPMTWaveform> vec_mpmt_waveform;
  std::vector<WCTEMPMTHit> vec_triggers_hit;
   printf("d3\n");
  unsigned char* mpmt_data= reinterpret_cast<unsigned char*>(msgs->mpmt_data->data());
  //printf("data size %d\n",msgs->mpmt_data->size());
   printf("d4\n");
  while(current_byte<bytes){
    printf("d5 curent:total= %d:%d\n", current_byte, bytes);
    //printf("cuurent byte %d : %d\n",mpmt_data[current_byte], (mpmt_data[current_byte] >> 6));
    //printf("(mpmt_data[current_byte] >> 6) == 0b1:%d\n", ((mpmt_data[current_byte] >> 6) == 0b1));
    printf("testing current byte\n");
    printf("current byte=%u\n",(mpmt_data[current_byte] >> 6));
    if((mpmt_data[current_byte] >> 6) == 0b1){ //its a hit or led or pps
      printf("in hit, led or pps \n");
      if(((mpmt_data[current_byte] >> 2) & 0b00001111 ) == 0U && bytes-current_byte >= WCTEMPMTHit::GetSize()){ // its normal mpmt hit
	printf("in hit\n");
	WCTEMPMTHit tmp(card_id, &mpmt_data[current_byte]);
	UWCTEMPMTHit* bob = reinterpret_cast<UWCTEMPMTHit*>(&tmp);
	
	current_byte+=WCTEMPMTHit::GetSize();
	if(card_type==3U && tmp.GetChannel() < 10) vec_triggers_hit.push_back(tmp);
	else vec_mpmt_hit.push_back(tmp);
      }
      
      //else if(((mpmt_data[current_byte] >> 2) & 0b00001111 ) == 1U ){
	//printf("in ped \n");
      //      }// its a pedistal (dont know) 
      
      else if(((mpmt_data[current_byte] >> 2) & 0b00001111 ) == 2U && bytes-current_byte >= WCTEMPMTLED::GetSize()){// its LED
	printf("in led \n");
	WCTEMPMTLED tmp(card_id, &mpmt_data[current_byte]);
	current_byte+=WCTEMPMTLED::GetSize();
	vec_mpmt_led.push_back(tmp);
      }
      
      //      else if(((mpmt_data[current_byte] >> 2) & 0b00001111 ) == 3U){
	//printf("in calib \n");
      //	}// its calib??
      
      else if(((mpmt_data[current_byte] >> 2) & 0b00001111 ) == 15U && bytes-current_byte >= WCTEMPMTPPS::GetSize() ){// its PPS
	printf("in pps\n");
	WCTEMPMTPPS tmp(card_id, &mpmt_data[current_byte]);
	current_byte+=WCTEMPMTPPS::GetSize();
	vec_mpmt_pps.push_back(tmp);
      }
      else{
	 printf("none of hit LED or pps despite saying it is\n");
	 std::string tmp="ERROR: Data from MPMT" +  std::to_string(card_id)  +" is courupt or of uknown structure";
	 msgs->m_data->services->SendLog(tmp,0);
	 delete msgs->daq_header;
	 msgs->daq_header=0;
	 delete msgs->mpmt_data;
	 msgs->mpmt_data=0;
	 delete msgs;
	 msgs=0;
	return false;

      }
    }
    else if ((mpmt_data[current_byte] >> 6) == 2U && bytes-current_byte >= WCTEMPMTWaveformHeader::GetSize()){ //its a waveform
       printf("in Waveform\n");
       printf("k0\n");
      WCTEMPMTWaveform tmp(card_id, &mpmt_data[current_byte]);
       printf("k1\n");
       current_byte+=  WCTEMPMTWaveformHeader::GetSize();
       printf("k2\n");
	   if(bytes-current_byte >= tmp.header.GetLength()){
	     printf("k3\n");
	     tmp.samples.resize(tmp.header.GetLength());
	     printf("k4\n");
	     memcpy(tmp.samples.data(), &mpmt_data[current_byte], tmp.header.GetLength());
	     printf("k5\n");
	     current_byte+=(tmp.header.GetLength());
	     printf("k6\n");
	     UWCTEMPMTWaveformHeader* tmp2 = reinterpret_cast<UWCTEMPMTWaveformHeader*>(&(tmp.header));
	     std::stringstream hello;
	     hello<<tmp2->bits;
	     printf("k6.5 %s\n",hello.str().c_str());
	     vec_mpmt_waveform.push_back(tmp);
	     printf("k7\n");
	   }
	   
    }
    else{
      printf("currentbyte=%u\n",current_byte);
      printf("bytes=%u\n",bytes);
      printf("(mpmt_data[current_byte] >> 6)%u\n", (mpmt_data[current_byte] >> 6));
      printf("(mpmt_data[current_byte] >> 6)%u\n", (mpmt_data[current_byte] >> 6));
      
      std::string tmp="ERROR: Waveform data from MPMT" +  std::to_string(card_id)  +" is courupt or of uknown structure.";
      msgs->m_data->services->SendLog(tmp,0);      
      delete msgs->daq_header;
      msgs->daq_header=0;
      delete msgs->mpmt_data;
      msgs->mpmt_data=0;
      delete msgs;
      msgs=0;
      return false;
      
    }
  }  
  printf("data processed \n");
  
  /////////////////////////////////////////////////
  ////adding data to datamodel
  ////////////////////////////////////////////////////
   printf("d5\n");
  msgs->m_data->unsorted_data_mtx.lock();
  if(msgs->m_data->unsorted_data.count(bin)==0){
    msgs->m_data->unsorted_data[bin]=new MPMTData();
    msgs->m_data->unsorted_data[bin]->coarse_counter=bin<<6;
  }
   printf("d6\n");
  if(card_type<2U){ //WCTEMPMT and buffered ADC
    printf("in send unsorted ADC\n");
    printf("data_hit_size=%d\n", msgs->m_data->unsorted_data[bin]->mpmt_hits.size());
    msgs->m_data->unsorted_data[bin]->mpmt_hits.insert( msgs->m_data->unsorted_data[bin]->mpmt_hits.end(), vec_mpmt_hit.begin(), vec_mpmt_hit.end());
    printf("data_led_size=%d\n", msgs->m_data->unsorted_data[bin]->mpmt_leds.size());
    msgs->m_data->unsorted_data[bin]->mpmt_leds.insert( msgs->m_data->unsorted_data[bin]->mpmt_leds.end(), vec_mpmt_led.begin(), vec_mpmt_led.end());
    printf("data_pps_size=%d\n", msgs->m_data->unsorted_data[bin]->mpmt_pps.size());
    msgs->m_data->unsorted_data[bin]->mpmt_pps.insert( msgs->m_data->unsorted_data[bin]->mpmt_pps.end(), vec_mpmt_pps.begin(), vec_mpmt_pps.end());
    printf("data_waveform_size=%d\n", msgs->m_data->unsorted_data[bin]->mpmt_waveforms.size());
    msgs->m_data->unsorted_data[bin]->mpmt_waveforms.insert( msgs->m_data->unsorted_data[bin]->mpmt_waveforms.end(), vec_mpmt_waveform.begin(), vec_mpmt_waveform.end());
   printf("unsorted ADC sent\n");
  }
  else if(card_type==3U){ //trigger card
    printf("in send unsorted triggercard\n");
    ///////////////////// send alert to evgeni for beam spill ////////////////
    for(int i=0; i<vec_triggers_hit.size(); i++){
      std::string tmp="{\"Spill\":"+ std::to_string(msgs->m_data->spill_num)  + "}";
      if(vec_triggers_hit.at(i).GetChannel()==4U) msgs->m_data->services->AlertSend("SpillCount", tmp);
    }
    ////////////////////////////////////////////
    msgs->m_data->unsorted_data[bin]->mpmt_triggers.insert( msgs->m_data->unsorted_data[bin]->mpmt_triggers.end(), vec_triggers_hit.begin(), vec_triggers_hit.end());
    msgs->m_data->unsorted_data[bin]->mpmt_pps.insert( msgs->m_data->unsorted_data[bin]->mpmt_pps.end(), vec_mpmt_pps.begin(), vec_mpmt_pps.end());
    msgs->m_data->unsorted_data[bin]->extra_hits.insert( msgs->m_data->unsorted_data[bin]->extra_hits.end(), vec_mpmt_hit.begin(), vec_mpmt_hit.end());
    msgs->m_data->unsorted_data[bin]->extra_waveforms.insert(msgs->m_data->unsorted_data[bin]->extra_waveforms.end(), vec_mpmt_waveform.begin(), vec_mpmt_waveform.end());
    
    printf("unsorted triggercard sent\n");
  }
  printf("d9\n");
  msgs->m_data->unsorted_data_mtx.unlock();
  printf("d10\n");
  std::stringstream tmp;
  tmp<<"MPMT:"<<card_id;
  msgs->m_data->hit_map[tmp.str()]++;
  printf("delete data\n");
  delete msgs->daq_header;
  msgs->daq_header=0;
  delete msgs->mpmt_data;
  msgs->mpmt_data=0;
  delete msgs;
  msgs=0;
  printf("datadeleted all good\n");
  return true;
  
}
