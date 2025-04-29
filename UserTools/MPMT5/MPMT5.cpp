#include "MPMT5.h"

MPMT5Messages::MPMT5Messages(){
  //  daq_header=0;
  //mpmt_data=0;
  mpmt_message=0;
  m_data=0;
  time_corrections=0;
  hit_rates=0;
  hitcount=0;
  
}

MPMT5Messages::~MPMT5Messages(){
  // delete daq_header;
  //daq_header=0;
  //delete mpmt_data;
  // mpmt_data=0;
  mpmt_message=0;
  m_data=0;
  time_corrections=0;
  hit_rates=0;
  hitcount=0;
}

MPMT5_args::MPMT5_args():Thread_args(){
  data_sock=0;
  utils=0;
  job_queue=0;
  m_data=0;
  hit_rates=0;

}

MPMT5_args::~MPMT5_args(){
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
  hit_rates=0;
  time_corrections=0;
  
}


MPMT5::MPMT5():Tool(){}


bool MPMT5::Initialise(std::string configfile, DataModel &data){

  InitialiseTool(data);
  InitialiseConfiguration(configfile);
  m_configfile=configfile;
  //m_variables.Print();

  LoadConfig();
  
  m_util=new Utilities();

  m_beam_stopping=false;
  //laod port number maybe
  //load rounding of bins masybe
  m_data->hardware_trigger = false;
  m_data->raw_readout=false;
  m_data->nhits_trigger=false;
  m_data->pps_mtx.lock();
  m_data->pps= new std::vector<WCTEMPMTPPS>;
  m_data->pps_mtx.unlock();
  m_data->mpmt_messages_mtx.lock();
  m_data->mpmt_messages= new std::vector<MPMTMessage*>;
  m_data->mpmt_messages_mtx.unlock();
  
  m_threadnum=0;
  CreateThread();
  m_freethreads=1;

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

  
  ExportConfiguration();

  
  
  
 return true;
}


bool MPMT5::Execute(){

  if(m_data->change_config){
    InitialiseConfiguration(m_configfile);
    LoadConfig();
    ExportConfiguration();
  }
  
  for(std::map<std::string, unsigned int>::iterator it=m_data->hit_map.begin(); it!=m_data->hit_map.end(); it++){
    m_data->monitoring_store_mtx.lock();
    m_data->monitoring_store.Set(it->first, it->second);
    m_data->monitoring_store_mtx.unlock();
  }

  if(m_data->beam_stopping){
    m_data->beam_stopping=false;
    m_beam_stopping=true;
    m_ref = boost::posix_time::microsec_clock::universal_time();
  }

  if(m_beam_stopping){
    
    m_lapse = m_period -( boost::posix_time::microsec_clock::universal_time() - m_ref);
    
    if(m_lapse.is_negative()){

      m_beam_stopping=false;
      m_data->beam_active=false;
      //      m_data->spill_num++;
    }
  }

  if(!m_data->running){
    
    m_data->mpmt_messages_mtx.lock();
    if(m_data->mpmt_messages->size()>0){
      for(unsigned int i=0 ; i< m_data->mpmt_messages->size(); i++){
	delete m_data->mpmt_messages->at(i);
	m_data->mpmt_messages->at(i)=0;
      }
      m_data->mpmt_messages->clear();
    }
      m_data->mpmt_messages_mtx.unlock();
    
    
    m_data->pps_mtx.lock();
    if(m_data->pps->size()) m_data->pps->clear();
    m_data->pps_mtx.unlock();
    

    m_data->data_chunks_mtx.lock();
    if(m_data->data_chunks.size()){
      for( std::map<unsigned int, MPMTCollection*>::iterator it=m_data->data_chunks.begin(); it!=m_data->data_chunks.end(); it++){
	delete it->second;
	it->second=0;
      }
      m_data->data_chunks.clear();
      std::map<unsigned int, MPMTCollection*> tmp;
      std::swap(m_data->data_chunks , tmp);
    }
    m_data->data_chunks_mtx.unlock();

  }

  

     /*
  usleep (1000);
  m_data->unsorted_data_mtx.lock();
  m_data->unsorted_data.clear();
  m_data->unsorted_data_mtx.unlock();
  */
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


bool MPMT5::Finalise(){

  for(unsigned int i=0;i<args.size();i++) DeleteThread(0);

  args.clear();
  
  delete m_util;
  m_util=0;
  

  

  return true;
}

void MPMT5::CreateThread(){

  MPMT5_args* tmparg=new MPMT5_args();
 
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
  tmparg->hit_rates=&m_data->hit_map;

  std::string config_json="";
  m_data->services->GetDeviceConfig(config_json, -1, "MCCAssignments");
  Store times;
  times.JsonParser(config_json);

  tmparg->time_corrections = &m_data->time_corrections[0];

  int correction =0;
  for(unsigned int i=0; i<200; i++){
    if(times.Get(std::to_string(i),correction)){
      tmparg->time_corrections[i]=correction*16;
      //tmparg->time_corrections[i]=0;
    }
    else tmparg->time_corrections[i]=0;
  }
  
  
  args.push_back(tmparg);
  std::stringstream tmp;
  tmp<<"T"<<m_threadnum;
  m_util->CreateThread(tmp.str(), &Thread, args.at(args.size()-1));
  m_threadnum++;
  m_data->thread_num++;

}

 void MPMT5::DeleteThread(unsigned int pos){

   m_util->KillThread(args.at(pos));

   delete args.at(pos)->data_sock;
   args.at(pos)->data_sock=0;

   delete args.at(pos)->utils;
   args.at(pos)->utils=0;
   
   delete args.at(pos);
   args.at(pos)=0;
   args.erase(args.begin()+(pos));
   

 }

void MPMT5::Thread(Thread_args* arg){

  MPMT5_args* args=reinterpret_cast<MPMT5_args*>(arg);

  args->lapse = args->period -( boost::posix_time::microsec_clock::universal_time() - args->last);
  
  if( args->lapse.is_negative()){
    //printf("in lapse \n");
    unsigned short num_connections = args->connections.size();
    //if(args->utils->UpdateConnections("TPMT", args->data_sock, args->connections, args->data_port) > num_connections) args->m_data->services->SendLog("Info: New MPMT connected",v_message); //add pmt id
    if(args->utils->UpdateConnections("MPMT", args->data_sock, args->connections, args->data_port) > num_connections) args->m_data->services->SendLog("Info: New MPMT connected",v_message); //add pmt id
    //if(args->utils->UpdateConnections("XMPMT", args->data_sock, args->connections, "4445") > num_connections) args->m_data->services->SendLog("Info: New MPMT connected",v_message); //add pmt id
    //if(args->utils->UpdateConnections("XMPMT", args->data_sock, args->connections, "4446") > num_connections) args->m_data->services->SendLog("Info: New MPMT connected",v_message); //add pmt id
    //if(args->utils->UpdateConnections("XMPMT", args->data_sock, args->connections, "4447") > num_connections) args->m_data->services->SendLog("Info: New MPMT connected",v_message); //add pmt id    
    args->m_data->monitoring_store_mtx.lock();
    args->m_data->monitoring_store.Set("connected_MPMTs",num_connections);
    args->m_data->monitoring_store_mtx.unlock();
    args->last= boost::posix_time::microsec_clock::universal_time();
    printf("conenctions=%d: %u\n",args->connections.size(), args->m_data->unsorted_data.size());
  }
  
  zmq::poll(&(args->items[0]), 1, 100);
  
  if(args->items[0].revents & ZMQ_POLLIN){
    //printf("received data\n");
    
    zmq::message_t identity;

    //zmq::message_t* daq_header = new zmq::message_t;
    //zmq::message_t* mpmt_data = new zmq::message_t;

    //args->message_received=false;
    args->no_data=false;
    
    args->message_received=args->data_sock->recv(&identity);     

    if(!identity.more() || !args->message_received || identity.size() == 0){
      args->m_data->services->SendLog("Warning: MPMT thread identity has no size or only message",3);
      //printf("d1\n");
      //   delete mpmt_data;
      //mpmt_data=0;
      //delete daq_header;
      //daq_header=0;
      return;
    }

    MPMTMessage* mpmt_message = new MPMTMessage;
    
    args->message_received=args->data_sock->recv(&mpmt_message->daq_header);
    
    if(!args->message_received || mpmt_message->daq_header.size()!=DAQHeader::GetSize() ){
      args->m_data->services->SendLog("Warning: MPMT thread daq header has no or incorrect size",3);
      //printf("d2\n");
      //std::cout<<"bad header size. Got="<<mpmt_message->daq_header.size()<<", expected="<<DAQHeader::GetSize()<<std::endl;
      delete mpmt_message;
      mpmt_message=0;
      // delete daq_header;
      // daq_header=0;
      return;
    }
    if(!mpmt_message->daq_header.more()) args->no_data=true;
    else{
      //printf("d3\n");
      args->message_received=args->data_sock->recv(&mpmt_message->mpmt_data);
      if(mpmt_message->mpmt_data.more() || !args->message_received || mpmt_message->mpmt_data.size() == 0){
	args->m_data->services->SendLog("ERROR: MPMT thread too many message parts or no data, throwing away data",2); //add mpmtid
	zmq::message_t throwaway;
	if(mpmt_message->mpmt_data.more()){
	  args->data_sock->recv(&throwaway);
	  while(throwaway.more()) args->data_sock->recv(&throwaway);
	}
	delete mpmt_message;
	mpmt_message=0;
	//delete daq_header;
	//daq_header=0;
	return;
      }
    }
    //printf("data fine\n");
	
    zmq::message_t reply(4);
    //printf("d1\n");
    memcpy(reply.data(), mpmt_message->daq_header.data(), reply.size());
    //printf("sending reply\n");
    args->data_sock->send(identity, ZMQ_SNDMORE); /// need to add checking probablly a poll incase sender dies
    args->data_sock->send(reply);
    //printf("sent reply\n");
    
    if(args->no_data){
      delete mpmt_message;
      mpmt_message=0;
      //      delete daq_header;
      //daq_header=0;
    }
    else{

      if(args->m_data->raw_readout){
	args->m_data->mpmt_messages_mtx.lock();
	args->m_data->mpmt_messages->push_back(mpmt_message);
	args->m_data->mpmt_messages_mtx.unlock();
      }

      else{
	//printf("creating job\n");
	Job* tmp_job= new Job("MPMT");
	MPMT5Messages* tmp_msgs= new MPMT5Messages;
	tmp_msgs->time_corrections=args->time_corrections;
	// tmp_msgs->daq_header=daq_header;
	//  tmp_msgs->mpmt_data=mpmt_data;
	tmp_msgs->mpmt_message=mpmt_message;
	tmp_msgs->m_data=args->m_data;
	tmp_msgs->hit_rates=args->hit_rates;
	tmp_job->data= tmp_msgs;
	//printf("d1\n");
	tmp_job->func=ProcessData;
	tmp_job->fail_func=ProcessDataFail;
	//printf("d2\n");
	if(!args->m_data->running){
	  delete tmp_msgs;
	  delete mpmt_message;
	  delete tmp_job;
	}
	//printf("d3\n");
	else args->job_queue->AddJob(tmp_job);
	//sleep(5);
	//printf("job submitted %d\n",args->job_queue->size());
      }
    }
  }
} //job deletion needed



bool MPMT5::ProcessData(void* data){

  //printf("in process data\n");
  MPMT5Messages* msgs=reinterpret_cast<MPMT5Messages*>(data);
  //printf("d1\n");  
  DAQHeader* daq_header=reinterpret_cast<DAQHeader*>(msgs->mpmt_message->daq_header.data());

  unsigned char card_id = (unsigned char) daq_header->GetCardID();
  
  unsigned int bin= (daq_header->GetCoarseCounter()) >>7; //+ msgs->time_corrections[card_id]) >> 7;
  //printf("coarse counts: mpmt=%lu, daq=%lu\n", bin, msgs->m_data->current_coarse_counter >>23);
  
  ///* comment out for fake data
  if((card_id > 132 || // invalid card id
      bin > ((msgs->m_data->current_coarse_counter >>23) +150) || // data too early
      ( bin < ((msgs->m_data->current_coarse_counter >>23) - 150) && ((msgs->m_data->current_coarse_counter >>23) >150) ) ) // data too late
     && (msgs->m_data->current_coarse_counter >>23) > 4      ){ // stop error at start of run
    msgs->m_data->services->SendLog("ERROR: throwing away bad data " + std::to_string(card_id),0);
    
    std::string reason = "too late";
    if(card_id>132) reason = "bad card";
    if(bin > ((msgs->m_data->current_coarse_counter >>23) +150)) reason="too early";
    //printf("bad data: %s, card_id %d, coarse counts: mpmt=%lu, daq=%lu\n", reason.c_str(), card_id, daq_header->GetCoarseCounter(), msgs->m_data->current_coarse_counter >>16);
    delete msgs->mpmt_message;
    msgs->mpmt_message=0;
    delete msgs;
    msgs=0;
    return true;
  }
  
  
  unsigned short card_type = daq_header->GetCardType();
  
  //printf("%u:%u\n", daq_header->GetCardID(), daq_header->GetCoarseCounter());
  
  std::vector<P_MPMTHit*> triggers;
  //  std::vector<P_MPMTLED*> leds;
  std::vector<TriggerInfo*> triggers_info;
  
  //    if(card_type==3U){
  // card_type=3U;
    //find triggers

  
  unsigned long bytes=msgs->mpmt_message->mpmt_data.size();
  unsigned long current_byte=0;
    //printf("%u:%u\n", daq_header->GetCardID(), daq_header->GetCoarseCounter());
    /*if((msgs->m_data->current_coarse_counter >>22) >2000){  
      if(bin< ((msgs->m_data->current_coarse_counter >>22) - 2000) || bin > ((msgs->m_data->current_coarse_counter >>22) + 2000)){ //~2 seconds seconds away from current time (60 x ~33.5ms)
	
	std::string tmp="ERROR: Data from MPMT" +  std::to_string(card_id)  +" is out of temporal threshold, maybe desynced (got:" + std::to_string(bin) +" expected: ~" + std::to_string(msgs->m_data->current_coarse_counter >>22);
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
    */
  //printf("d2 car_id=%d\n", card_id);
  //  daq_header->Print();
  //  std::map<unsigned int, MPMTData*> local_data;
  /*  std::vector<WCTEMPMTHit> vec_mpmt_hit;
  std::vector<WCTEMPMTLED> vec_mpmt_led;
  std::vector<WCTEMPMTPPS> vec_mpmt_pps;
  std::vector<WCTEMPMTWaveform> vec_mpmt_waveform;
  std::vector<WCTEMPMTHit> vec_triggers_hit;
  */
  // std::vector<P_MPMTHit> hits;
  //std::vector<P_MPMTWaveformHeader> waveforms;
  //printf("d3\n");
  //std::string pmt_unique_name="";

  unsigned char* mpmt_data= reinterpret_cast<unsigned char*>(msgs->mpmt_message->mpmt_data.data());
  //printf("data size %d\n",msgs->mpmt_data->size());
  //printf("d4\n");

  while(current_byte<bytes && (bytes-current_byte)>8){
    
    //printf("d5 curent:total= %d:%d\n", current_byte, bytes);
    //printf("cuurent byte %d : %d\n",mpmt_data[current_byte], (mpmt_data[current_byte] >> 6));
    //printf("(mpmt_data[current_byte] >> 6) == 0b1:%d\n", ((mpmt_data[current_byte] >> 6) == 0b1));
    //printf("testing current byte\n");
    //printf("current byte=%u\n",(mpmt_data[current_byte] >> 6));
    if((mpmt_data[current_byte] >> 6) == 0b1){ //its a hit or led or pps
      //printf("in hit, led or pps \n");
      //printf("type is %u\n", ((mpmt_data[current_byte] >> 2) & 0b00001111 ));
      if(((mpmt_data[current_byte] >> 2) & 0b00001111 ) <= 3U && bytes-current_byte >= WCTEMPMTHit::GetSize()){ // its normal mpmt hit
	//printf("in hit\n");
	MPMTHit* tmp= reinterpret_cast<MPMTHit*>(&mpmt_data[current_byte]);
	//	if((!tmp->GetChannel()<10) || card_type!=3U)	msgs->mpmt_message->hits.emplace_back(tmp, msgs->m_data->spill_num, card_id);
	if(card_type!=3U){
	  if(card_id != 130 && card_id != 132){
	    msgs->mpmt_message->hits.emplace_back(tmp, card_id);
	    //marcus added
	    ++msgs->hitcount;  // FINDME
	    //
	    
	  } else{
	    P_MPMTHit* tmp_hit= new P_MPMTHit(tmp, card_id);
	    triggers.push_back(tmp_hit);
	  }
	}
	//printf("hits.size()=%u\n",hits.size());
	//	  tmp(card_id, &mpmt_data[current_byte]);
	//tmp.SetCoarseCounter(tmp.GetCoarseCounter() + 12*msgs->time_corrections[card_id]);
	//	UWCTEMPMTHit* bob = reinterpret_cast<UWCTEMPMTHit*>(&tmp);
	
	else{
	  //  if(tmp->GetChannel()<10 && card_type==3U){
	  //printf("trigger card\n");
	  P_MPMTHit* tmp_hit= new P_MPMTHit(tmp, card_id);
	  
	  triggers.push_back(tmp_hit);
	  if(tmp->GetChannel()<10){
	    TriggerInfo* trigger_info = new TriggerInfo;
	    trigger_info->time= (((unsigned long)(daq_header->GetCoarseCounter() & 4294901760U ))<<16)  | (tmp->GetCoarseCounter());
	    if((daq_header->GetCoarseCounter() & 65535U) > (tmp->GetCoarseCounter() >>16)) trigger_info->time+=4294967296ull;// 65536U;
	    //trigger_info->time+=msgs->time_corrections[card_id];
	    trigger_info->card_id=card_id;
	    trigger_info->spill_num=msgs->m_data->spill_num;
	    
	    //triggers.push_back(tmp);
	    
	    switch(triggers.back()->hit->GetChannel()){
	      
	    case 0U: //main trigger
	      trigger_info->type = TriggerType::MAIN;
	      triggers_info.push_back(trigger_info);
	      break;
	      
	    case 1U: //beam monitor - electron
	      trigger_info->type = TriggerType::EBEAM;
	      triggers_info.push_back(trigger_info);
	      break;
	      
	    case 2U: //beam monitor - muon
	      trigger_info->type = TriggerType::MBEAM;
	      triggers_info.push_back(trigger_info);
	      break;
	      
	    case 3U: //beam spill warn
	      msgs->m_data->beam_active=true;
	      msgs->m_data->beam_stopping=false;
	      msgs->m_data->beam_warn_coarse_counts= trigger_info->time;
	      delete trigger_info;
              trigger_info=0;
	      msgs->m_data->services->AlertSend("SpillWarn", "");
	      //trigger_info->type = TriggerType::MBEAM;
	      //triggers_info.push_back(trigger_info);
	      break;
	      
	    case 4U: //beam spill end
	      msgs->m_data->beam_stopping=true;
	      msgs->m_data->beam_active=false;
	      delete trigger_info;
              trigger_info=0;
	      msgs->m_data->spill_update_coarse_counter=msgs->m_data->current_coarse_counter+2500000000;
	      msgs->m_data->services->AlertSend("SpillEnd", "");
	      msgs->m_data->spill_update_flag_mtx.lock();
	      msgs->m_data->spill_update_flag=true;
	      //    printf("mpmt spill_num before =%lu, b=%d, \n", msgs->m_data->spill_num, msgs->m_data->spill_update_flag);
	      msgs->m_data->spill_update_flag_mtx.unlock();
	      // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!spill end!!!!!!!!!!!!!!!!!!!!\n");
	      
	      //trigger_info->type = TriggerType::MBEAM;
	      //triggers_info.push_back(trigger_info);
	      break;
	      
	    case 5U: //CDS laser
	      trigger_info->type = TriggerType::LASER;
	      triggers_info.push_back(trigger_info);
	      break;
	      
	    case 7U:
	      trigger_info->type = TriggerType::HARD6;
	      triggers_info.push_back(trigger_info);
	      break;
	      
	    default:
	      delete trigger_info;
	      trigger_info=0;
	      break;
	    }
	  
	  }
	}
	
	current_byte+=WCTEMPMTHit::GetSize();
	//pmt_unique_name=std::to_string(card_id)+":"+std::to_string(tmp->GetChannel());
	//	if(!msgs->hit_rates->count(pmt_unique_name)) (*msgs->hit_rates)[pmt_unique_name]=0;
	//	(*msgs->hit_rates)[pmt_unique_name]++;

	
	
	/*	unsigned int tmp_bin = (daq_header->GetCoarseCounter() & 4294901760U ) | (tmp.GetCoarseCounter() >>16);
	if((daq_header->GetCoarseCounter() & 65535U) > (tmp.GetCoarseCounter() >>16)){
	   //printf("this should not print alot\n");
	   tmp_bin +=65536U;
	}
	tmp_bin = tmp_bin >> 6;
	if(!local_data.count(tmp_bin)){
	  local_data[tmp_bin] = new MPMTData();
	  local_data[tmp_bin]->coarse_counter=tmp_bin<<6;
	}
	//printf("cardtype=%u\n",card_type);
	// printf("cardchannel=%u\n",tmp.GetChannel());
	if(card_type==3U){
	  if(tmp.GetChannel() < 10){
	    //printf("its a trigger bin=%u\n",tmp_bin);
	    local_data[tmp_bin]->mpmt_triggers.push_back(tmp);
	  }
	  //else {
	    local_data[tmp_bin]->extra_hits.push_back(tmp);
	    //printf("its an extra trigger=hit\n");
	    //}
	}
	else{
	  local_data[tmp_bin]->mpmt_hits.push_back(tmp);
	  //printf("its a normal hit\n");
	}
	*/
      }
	
      //else if(((mpmt_data[current_byte] >> 2) & 0b00001111 ) == 1U ){
	//printf("in ped \n");
      //      }// its a pedistal (dont know) 
      
      else if(((mpmt_data[current_byte] >> 2) & 0b00001111 ) == 14U && bytes-current_byte >= WCTEMPMTLED::GetSize()){// its LED
	//	WCTEMPMTLED tmp(card_id, &mpmt_data[current_byte]);
	//tmp.SetCoarseCounter(tmp.GetCoarseCounter() + 12*msgs->time_corrections[card_id]);

	MPMTLED* tmp = reinterpret_cast<MPMTLED*>(&mpmt_data[current_byte]);
	P_MPMTLED* tmp_led = new P_MPMTLED(tmp, card_id);
	//leds.push_back(tmp_led);
	//	leds.push_back(tmp);
	//tmp->Print();

	TriggerInfo* trigger_info = new TriggerInfo;
	trigger_info->type= TriggerType::LED;

	trigger_info->time= (((unsigned long)(daq_header->GetCoarseCounter() & 4294901760U ))<<16)  | (tmp->GetCoarseCounter());
	if((daq_header->GetCoarseCounter() & 65535U) > (tmp->GetCoarseCounter() >>16)) trigger_info->time+=4294967296ull;//65536U;
	//printf("in led cardid=%u, daq coarse counter=%lu, LED coarse counter=%lu, trigger time=%lu\n",card_id, msgs->m_data->current_coarse_counter, tmp->GetCoarseCounter(), trigger_info->time);
	//	trigger_info->time+=msgs->time_corrections[card_id];
	trigger_info->card_id=card_id;
	trigger_info->spill_num=msgs->m_data->spill_num;
	trigger_info->mpmt_LEDs.push_back(tmp_led);
	triggers_info.push_back(trigger_info);
	
	/*
	char logmsg[256];
	snprintf(logmsg, 256, "LED frame from MPMT %u at initial coarse counter %lu, corrected to %lu, time bin %llu\n",card_id, tmp->GetCoarseCounter(), trigger_info->time, bin);
	std::string ledmsgs;
	m_data->monitoring_stream_mtx.lock();
	msgs->m_data->monitoring_stream.Get("LED_frames",ledmsgs);
	ledmsgs += logmsg;
	msgs->m_data->monitoring_stream.Set("LED_frames",ledmsgs);
	m_data->monitoring_stream_mtx.unlock();
	*/
	
	current_byte+=WCTEMPMTLED::GetSize();
	//unsigned int tmp_bin = (daq_header->GetCoarseCounter() & 4294901760U ) | (tmp.GetCoarseCounter() >>16);
        //if((daq_header->GetCoarseCounter() & 65535U) > (tmp.GetCoarseCounter() >>16)){
	  //printf("this should not print alot\n");
	  //tmp_bin +=65536U;
	//}
	//tmp_bin = tmp_bin >> 6;
	//if(!local_data.count(tmp_bin)){
	// local_data[tmp_bin] = new MPMTData();
	// local_data[tmp_bin]->coarse_counter=tmp_bin<<6;
	//}
	// local_data[tmp_bin]->mpmt_leds.push_back(tmp);
      }
      
      //      else if(((mpmt_data[current_byte] >> 2) & 0b00001111 ) == 3U){
	//printf("in calib \n");
      //	}// its calib??
      
      else if(((mpmt_data[current_byte] >> 2) & 0b00001111 ) == 15U && bytes-current_byte >= WCTEMPMTPPS::GetSize() ){// its PPS
	//printf("in pps\n");
	msgs->m_data->pps_mtx.lock();
	msgs->m_data->pps->emplace_back(card_id, &mpmt_data[current_byte]);
	if((msgs->m_data->pps->back().GetReserved() & 0b10) !=2 &&
	   ( msgs->m_data->pps->back().GetCurrentPPSCoarseCounter() - msgs->m_data->pps->back().GetPreviousPPSCoarseCounter() > 125000010) ||
	   (msgs->m_data->pps->back().GetCurrentPPSCoarseCounter() - msgs->m_data->pps->back().GetPreviousPPSCoarseCounter() < 124999990 )){
	  //	  msgs->m_data->services->SendAlarm("ERROR MPMT"+ std::to_string(card_id)+": PPS check fail current="+std::to_string(msgs->m_data->pps->back().GetCurrentPPSCoarseCounter())+", previous ="+std::to_string(msgs->m_data->pps->back().GetPreviousPPSCoarseCounter()));
	  msgs->m_data->services->SendLog("ERROR MPMT"+ std::to_string(card_id)+": PPS check fail current="+std::to_string(msgs->m_data->pps->back().GetCurrentPPSCoarseCounter())+", previous ="+std::to_string(msgs->m_data->pps->back().GetPreviousPPSCoarseCounter()),v_message);
	}
	
	msgs->m_data->pps_mtx.unlock();
	

	current_byte+=WCTEMPMTPPS::GetSize();
	/*	WCTEMPMTPPS tmp(card_id, &mpmt_data[current_byte]);
	current_byte+=WCTEMPMTPPS::GetSize();
	unsigned int tmp_bin = (daq_header->GetCoarseCounter() & 4294901760U ) | (tmp.GetCoarseCounter() >>16);
        if((daq_header->GetCoarseCounter() & 65535U) > (tmp.GetCoarseCounter() >>16)) tmp_bin +=65536U;
        tmp_bin = tmp_bin >> 6;
        local_data[tmp_bin].mpmt_pps.push_back(tmp);
	*/
      }
      else{
	 //printf("none of hit LED or pps despite saying it is\n");
	std::string tmp="ERROR: Data from MPMT" +  std::to_string(card_id)  +" is corrupt or of uknown structure. event type is" + std::to_string(((mpmt_data[current_byte] >> 2) & 0b00001111 ));
	//	for(std::map<unsigned int, MPMTData*>::iterator it=local_data.begin(); it!=local_data.end(); it++){
	//	  delete it->second;
	//it->second=0;
	//}
	//local_data.clear();
	 msgs->m_data->services->SendLog(tmp,0);
	 // delete msgs->daq_header;
	 //msgs->daq_header=0;
	 //delete msgs->mpmt_data;
	 //msgs->mpmt_data=0;
	 delete msgs->mpmt_message;
	 msgs->mpmt_message=0;
	 for(unsigned int i=0; i<triggers_info.size(); i++){
	   delete triggers_info.at(i);
	   triggers_info.at(i)=0;
	 }
	 for(unsigned int i=0; i<triggers.size(); i++){
	   delete triggers.at(i);
	   triggers.at(i)=0;
	 }

	 
	 delete msgs;
	 msgs=0;
	 //printf("d1\n");
	return true;

      }
    }
    else if ((mpmt_data[current_byte] >> 6) == 2U && bytes-current_byte >= WCTEMPMTWaveformHeader::GetSize()){ //its a waveform
      //printf("in Waveform\n");
       //printf("k0\n");
      //WCTEMPMTWaveform tmp(card_id, &mpmt_data[current_byte]);
      //tmp.header.SetCoarseCounter(tmp.header.GetCoarseCounter() + 12*msgs->time_corrections[card_id]);
      // printf("k1 channel=%u\n", tmp.header.GetChannel());

      //MPMTWaveformHeader* tmp = reinterpret_cast<MPMTWaveformHeader*>(&mpmt_data[current_byte]);
      msgs->mpmt_message->waveforms.emplace_back(reinterpret_cast<MPMTWaveformHeader*>(&mpmt_data[current_byte]), card_id);
      current_byte+=  WCTEMPMTWaveformHeader::GetSize() +  msgs->mpmt_message->waveforms.back().waveform_header->GetLength();
			     //      waveforms.push_back(tmp);

      if(current_byte-1 > bytes ){//|| ((msgs->mpmt_message->waveforms.back().waveform_header->GetLength()*8) /12) != msgs->mpmt_message->waveforms.back().waveform_header->GetNumSamples()){
	printf("MPMTS GARBAGE DATA  bytes %u = samples %u : cardid=%u \n", msgs->mpmt_message->waveforms.back().waveform_header->GetLength(), msgs->mpmt_message->waveforms.back().waveform_header->GetNumSamples(), card_id);
	std::string tmp="ERROR: Waveform samples not present card=" +  std::to_string(card_id)  +".";
	msgs->m_data->services->SendLog(tmp,0);
	delete msgs->mpmt_message;
	msgs->mpmt_message=0;
	for(unsigned int i=0; i<triggers_info.size(); i++){
	  delete triggers_info.at(i);
	  triggers_info.at(i)=0;
	}
	for(unsigned int i=0; i<triggers.size(); i++){
	  delete triggers.at(i);
	  triggers.at(i)=0;
	}

	delete msgs;
	msgs=0;
	//printf("d1\n");
	return true;
      
    

      }
 
      //printf("k2\n");
       /*
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
       */
       //	unsigned int tmp_bin = (daq_header->GetCoarseCounter() & 4294901760U ) | (tmp.header.GetCoarseCounter() >>16);
       //if((daq_header->GetCoarseCounter() & 65535U) > (tmp.header.GetCoarseCounter() >>16)) tmp_bin +=65536U;
       //	tmp_bin = tmp_bin >> 6;
	//printf("k1 channel=%u : %u\n", tmp.header.GetChannel(), tmp_bin);

	//if(!local_data.count(tmp_bin)){
       // local_data[tmp_bin] = new MPMTData();
       //  local_data[tmp_bin]->coarse_counter=tmp_bin<<6;
       //}
       //  if(card_type!=3U) local_data[tmp_bin]->mpmt_waveforms.push_back(tmp);
	  //	if(card_type==3U && tmp.header.GetChannel() >= 10) local_data[tmp_bin]->extra_waveforms.push_back(tmp);
       //if(card_type==3U) local_data[tmp_bin]->extra_waveforms.push_back(tmp);
    }
    else{
      //      printf("currentbyte=%u\n",current_byte);
      //printf("bytes=%u\n",bytes);
      //printf("expected header size=%u\n", WCTEMPMTWaveformHeader::GetSize());
      //printf("(mpmt_data[current_byte] >> 6)%u\n", (mpmt_data[current_byte] >> 6));
      // printf("(mpmt_data[current_byte] >> 6)%u\n", (mpmt_data[current_byte] >> 6));
      
      std::string tmp="ERROR: Waveform data from MPMT" +  std::to_string(card_id)  +" is corrupt or of uknown structure.";
      msgs->m_data->services->SendLog(tmp,0);
      // for(std::map<unsigned int, MPMTData*>::iterator it=local_data.begin(); it!=local_data.end(); it++){
      //	delete it->second;
      //	it->second=0;
      //}
      //local_data.clear();
      // delete msgs->daq_header;
      //msgs->daq_header=0;
      //delete msgs->mpmt_data;
      //msgs->mpmt_data=0;
      delete msgs->mpmt_message;
      msgs->mpmt_message=0;
      for(unsigned int i=0; i<triggers_info.size(); i++){
	delete triggers_info.at(i);
	triggers_info.at(i)=0;
      }
      for(unsigned int i=0; i<triggers.size(); i++){
	delete triggers.at(i);
	triggers.at(i)=0;
      }
      
      delete msgs;
      msgs=0;
      //printf("d1\n");
      return true;
      
    }
  }
  //printf("MPMT5 hits.size()=%u\n",msgs->mpmt_message->hits.size());
  //  }

  //printf("data processed \n");
  
  /////////////////////////////////////////////////
  ////adding data to datamodel
  ////////////////////////////////////////////////////
  //printf("d5\n");

  /*
  bool has_data =false;
    if(msgs->mpmt_message->hits.size() || msgs->mpmt_message->waveforms.size() ) has_data=true;
  
  if(triggers_info.size() || triggers.size() || has_data) printf("MPMT has data\n");
  else printf("MPMT no data!!!\n");
  */
  
    msgs->m_data->data_chunks_mtx.lock();
    if(msgs->m_data->data_chunks.count(bin)==0){
      msgs->m_data->data_chunks[bin]=new MPMTCollection;
    }
    //printf("NEW DATA bin=%u, current size=%u\n",bin, msgs->m_data->data_chunks[bin]->mpmt_output.size()); 

    msgs->m_data->data_chunks[bin]->mtx.lock();
    msgs->m_data->data_chunks[bin]->mpmt_output.push_back(msgs->mpmt_message);
    /*    if(triggers.size())   msgs->m_data->data_chunks[bin]->triggers.insert( msgs->m_data->data_chunks[bin]->triggers.end(), triggers.begin(), triggers.end());
    if(leds.size()) msgs->m_data->data_chunks[bin]->leds.insert( msgs->m_data->data_chunks[bin]->leds.end(), leds.begin(),leds.end());
    */    
    if(triggers_info.size()) msgs->m_data->data_chunks[bin]->triggers_info.insert( msgs->m_data->data_chunks[bin]->triggers_info.end(), triggers_info.begin(), triggers_info.end());
    if(triggers.size()) msgs->m_data->data_chunks[bin]->triggers.insert( msgs->m_data->data_chunks[bin]->triggers.end(), triggers.begin(), triggers.end());
    //if(leds.size()) msgs->m_data->data_chunks[bin]->leds.insert( msgs->m_data->data_chunks[bin]->leds.end(), leds.begin(), leds.end());
    
    // FINDME
    msgs->m_data->data_chunks[bin]->hitcounts[(short)card_id] += msgs->hitcount;
    
    msgs->m_data->data_chunks[bin]->mtx.unlock();
    /* 
    delete msgs->m_data->data_chunks[bin];
    msgs->m_data->data_chunks[bin] =0;
    msgs->m_data->data_chunks.erase(bin);
    */
    msgs->m_data->data_chunks_mtx.unlock();
	
  /*
  msgs->m_data->unsorted_data_mtx.lock();
  for(std::map<unsigned int, MPMTData*>::iterator it=local_data.begin(); it!=local_data.end(); it++){
    delete it->second;
    it->second=0;
    
    printf("d6\n");
    //printf("time %u:MPMT%u:current=%u\n", it->first, card_id, ((msgs->m_data->current_coarse_counter >>22) - 60));
    //    it->second.Print();
    //   msgs->m_data->unsorted_data_mtx.lock();
    if(msgs->m_data->unsorted_data.count(it->first)==0){
      msgs->m_data->unsorted_data[it->first]=it->second;
      it->second=0;
      
           
      delete msgs->m_data->unsorted_data[it->first];
      msgs->m_data->unsorted_data[it->first]=0;
      msgs->m_data->unsorted_data.erase(it->first);
      
      continue;
      //      msgs->m_data->unsorted_data[it->first]=new MPMTData();
      // msgs->m_data->unsorted_data[it->first]->coarse_counter=(it->first<<6);
    }
    //msgs->m_data->unsorted_data_mtx.unlock();
    
    if(it->second->mpmt_hits.size() > 0){
      msgs->m_data->unsorted_data[it->first]->mpmt_hits_mtx.lock();
      msgs->m_data->unsorted_data[it->first]->mpmt_hits.insert( msgs->m_data->unsorted_data[it->first]->mpmt_hits.end(), it->second->mpmt_hits.begin(), it->second->mpmt_hits.end());
       msgs->m_data->unsorted_data[it->first]->mpmt_hits_mtx.unlock();
    }
    if(it->second->mpmt_leds.size() > 0){
      msgs->m_data->unsorted_data[it->first]->mpmt_leds_mtx.lock();
      msgs->m_data->unsorted_data[it->first]->mpmt_leds.insert( msgs->m_data->unsorted_data[it->first]->mpmt_leds.end(), it->second->mpmt_leds.begin(), it->second->mpmt_leds.end());
      msgs->m_data->unsorted_data[it->first]->mpmt_leds_mtx.unlock();
    }
    if(it->second->mpmt_waveforms.size() > 0){
      msgs->m_data->unsorted_data[it->first]->mpmt_waveforms_mtx.lock();
      msgs->m_data->unsorted_data[it->first]->mpmt_waveforms.insert( msgs->m_data->unsorted_data[it->first]->mpmt_waveforms.end(), it->second->mpmt_waveforms.begin(), it->second->mpmt_waveforms.end());
      msgs->m_data->unsorted_data[it->first]->mpmt_waveforms_mtx.unlock();
    }
    
    if(it->second->mpmt_triggers.size() > 0){
      msgs->m_data->unsorted_data[it->first]->mpmt_triggers_mtx.lock();
      msgs->m_data->unsorted_data[it->first]->mpmt_triggers.insert( msgs->m_data->unsorted_data[it->first]->mpmt_triggers.end(), it->second->mpmt_triggers.begin(), it->second->mpmt_triggers.end());
      msgs->m_data->unsorted_data[it->first]->mpmt_triggers_mtx.unlock();
      ///////////////////// send alert to evgeni for beam spill ////////////////
      
      for(int i=0; i<it->second->mpmt_triggers.size(); i++){
	std::string tmp="{\"Spill\":"+ std::to_string(msgs->m_data->spill_num)  + "}";
	if(it->second->mpmt_triggers.at(i).GetChannel()==4U) msgs->m_data->services->AlertSend("SpillCount", tmp);
      }
      ///////////////////////////
    }

    //if(it->second->mpmt_pps.size() > 0) msgs->m_data->unsorted_data[it->first]->mpmt_pps.insert( msgs->m_data->unsorted_data[it->first]->mpmt_pps.end(), it->second->mpmt_pps.begin(), it->second->mpmt_pps.end());
    if(it->second->extra_hits.size() > 0){
      msgs->m_data->unsorted_data[it->first]->extra_hits_mtx.lock();
      msgs->m_data->unsorted_data[it->first]->extra_hits.insert( msgs->m_data->unsorted_data[it->first]->extra_hits.end(), it->second->extra_hits.begin(), it->second->extra_hits.end());
      msgs->m_data->unsorted_data[it->first]->extra_hits_mtx.unlock();
    }
    if(it->second->extra_waveforms.size() > 0){
      msgs->m_data->unsorted_data[it->first]->extra_waveforms_mtx.lock();
      msgs->m_data->unsorted_data[it->first]->extra_waveforms.insert( msgs->m_data->unsorted_data[it->first]->extra_waveforms.end(), it->second->extra_waveforms.begin(), it->second->extra_waveforms.end());
      msgs->m_data->unsorted_data[it->first]->extra_waveforms_mtx.unlock();
    }
  

    delete it->second;
    it->second=0;

    
    delete msgs->m_data->unsorted_data[it->first];
    msgs->m_data->unsorted_data[it->first]=0;
    msgs->m_data->unsorted_data.erase(it->first);
    
  
} 
  printf("d7\n");
  msgs->m_data->unsorted_data_mtx.unlock();

    


  }
  */
  //printf("d8\n");
    //printf("in send unsorted triggercard\n");
   
  //printf("d9\n");
  //  msgs->m_data->unsorted_data[bin]->Print();
  //printf("d10\n");
    std::stringstream tmp;
    tmp<<"MPMT:"<<((short)card_id);
    msgs->m_data->hit_map[tmp.str()]++;  // FIXME no locking?
    //printf("delete data\n");
    //delete msgs->daq_header;
    //msgs->daq_header=0;
    //delete msgs->mpmt_data;
    //msgs->mpmt_data=0;
    delete msgs;
    msgs=0;
    //printf("datadeleted all good\n");
    return true;
    
}

void MPMT5::LoadConfig(){

  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
  if(!m_variables.Get("MPMT_port",m_mpmt_port)) m_mpmt_port="4444";
  if(!m_variables.Get("MPMT_search_period_sec",m_mpmt_search_period_sec)) m_mpmt_search_period_sec=10;
  unsigned int period=0;
  if(!m_variables.Get("beam_end_delay_sec",period)) period=30;
  m_period=boost::posix_time::seconds(period);


}

bool MPMT5::ProcessDataFail(void* data){

  MPMT5Messages* msgs=reinterpret_cast<MPMT5Messages*>(data);

  /*  if(msgs!=0){
    delete msgs->mpmt_message;
    msgs->mpmt_message=0;
    
    
    msgs->m_data->services->SendLog("MPMT: New MPMT ProcessData fil",v_message);
    delete msgs;
    msgs=0;
    }*/
 printf("MPMT Process data fail\n");
   
  return true;
}
