#include "GenerateDummyData.h"
#include <zmq.hpp>

GenerateDummyData::GenerateDummyData():Tool(){}


bool GenerateDummyData::Initialise(std::string configfile, DataModel &data){
	
	InitialiseTool(data);
	InitialiseConfiguration(configfile);
	//m_variables.Print();
	
	//your code here
	m_variables.Get("verbosity",m_verbose);
	Log("GenerateDummyData initialising",v_debug,m_verbose);
	if(!m_variables.Get("output_port",out_port)) out_port = 4444;
	if(!m_variables.Get("num_MPMTs",num_mpmts)) num_mpmts = 10;
	double send_rate=1;
	if(!m_variables.Get("send_rate",send_rate)) send_period_ms = 100;
	else send_period_ms = 1000./send_rate;
	if(!m_variables.Get("poll_ms",poll_ms)) poll_ms = 500;
	Log("GenerateDummyData will simulate message from "+std::to_string(num_mpmts)
	         +" MPMTs, sending messages at "+std::to_string(send_rate)+"Hz",v_debug,m_verbose);
	
	Log("GenerateDummyData making zmq socket",v_debug,m_verbose);
	out_sock = new zmq::socket_t(*(m_data->context), ZMQ_DEALER);
	out_sock->setsockopt(ZMQ_LINGER,10);
	out_sock->setsockopt(ZMQ_RCVTIMEO,500);
	out_sock->setsockopt(ZMQ_SNDTIMEO,500);
	//out_sock->setsockopt(ZMQ_MANDATORY,1);
	
	Log("GenerateDummyData binding zmq socket",v_debug,m_verbose);
	std::string socket_url = "tcp://*:"+std::to_string(out_port);
	out_sock->bind(socket_url);
	out_poll = zmq::pollitem_t{*out_sock,0,ZMQ_POLLOUT,0};
	in_poll = zmq::pollitem_t{*out_sock,0,ZMQ_POLLIN,0};
	
	last_send = std::chrono::high_resolution_clock::now();
	coarse_counter = rand()%100;
	
	ExportConfiguration();
	
	return true;
}


bool GenerateDummyData::Execute(){
	
	auto ms_since_last_send = std::chrono::duration_cast<std::chrono::milliseconds>(last_send - std::chrono::high_resolution_clock::now());
	if(ms_since_last_send.count() > send_period_ms){
		SendNextMessage();
		last_send = std::chrono::high_resolution_clock::now();
	}
	
	return true;
}

bool GenerateDummyData::SendNextMessage(){
	
	Log("GenerateDummyData Sending next message",v_debug,m_verbose);
	
	// messages have 2 parts
	
	// part 1 is DAQHeader
	DAQHeader hdr;
	hdr.SetMessageNumber(++msg_num);
	hdr.SetCoarseCounter(coarse_counter);  // 1 tick = 520us
	hdr.SetCardType(((rand()%10)>8) ? 3 : 0);
	hdr.SetCardID(mpmt_num++);
	if(mpmt_num >= num_mpmts) mpmt_num=0;  // rollover 
	
	Log("GenerateDummyData next message header:",v_debug,m_verbose);
	if(m_verbose>v_debug) hdr.Print();
	
	zmq::message_t msg_hdr(sizeof(DAQHeader));
	memcpy(&hdr,msg_hdr.data(),sizeof(DAQHeader));
	
	// part 2 is a binary concatenation of the 'data' members
	// from an arbitrary number of WCTEMPMTHit, WCTEMPMTLED, WCTEMPMTPPS,
	// and WCTEMPMTWaveform objects
	int num_hits = rand()%10;
	int num_led = rand()%3;
	int num_pps = rand()%3;
	int num_wf = 0; //rand()%3;
	Log("GenerateDummyData next message body will contain "+std::to_string(num_hits)
	   +" hits, "+std::to_string(num_led)+" LEDs, "+std::to_string(num_pps)+" PPS, and "
	   +std::to_string(num_wf)+" waveforms",v_debug,m_verbose);
	size_t hits_bytes = sizeof(WCTEMPMTHit)*num_hits;
	size_t led_bytes = sizeof(WCTEMPMTLED)*num_led;
	size_t pps_bytes = sizeof(WCTEMPMTPPS)*num_pps;
	size_t wf_bytes = sizeof(WCTEMPMTWaveform)*num_wf;
	int total_bytes = hits_bytes +
	                  led_bytes +
	                  pps_bytes +
	                  wf_bytes;
	zmq::message_t msg_data(total_bytes);
	
	size_t bytes_offset=0;
	// add hits
	for(int i=0; i<num_hits; ++i){
		WCTEMPMTHit tmp_hit = MakeHit(rand()%128,          // Card ID
		                              rand()%24,           // Channel ID
		                              EventType::Normal,   // Event Type (Normal, Pedestal, LED, Cal, PPS)
		                              rand()%1000,           // coarse counter (units 8ns)
		                              false);              // is waveform?
		memcpy(&tmp_hit,(unsigned char*)msg_data.data()+bytes_offset,sizeof(WCTEMPMTHit));
		bytes_offset += sizeof(WCTEMPMTHit);
	}
	
	// add led flashes
	for(int i=0; i<num_led; ++i){
		WCTEMPMTHit tmp_hit = MakeHit(rand()%128,          // Card ID
		                              rand()%24,           // Channel ID
		                              EventType::LED,      // Event Type (Normal, Pedestal, LED, Cal, PPS)
		                              rand()%1000,           // coarse counter (units 8ns)
		                              false);              // is waveform?
		memcpy(&tmp_hit,(unsigned char*)msg_data.data()+bytes_offset,sizeof(WCTEMPMTHit));
		bytes_offset += sizeof(WCTEMPMTHit);
	}
	
	// add PPS events
	for(int i=0; i<num_led; ++i){
		WCTEMPMTHit tmp_hit = MakeHit(rand()%128,          // Card ID
		                              rand()%24,           // Channel ID
		                              EventType::PPS,      // Event Type (Normal, Pedestal, LED, Cal, PPS)
		                              rand()%1000,           // coarse counter (units 8ns)
		                              false);              // is waveform?
		memcpy(&tmp_hit,(unsigned char*)msg_data.data()+bytes_offset,sizeof(WCTEMPMTHit));
		bytes_offset += sizeof(WCTEMPMTHit);
	}
	
	assert(bytes_offset==total_bytes && "total bytes doesn't match size of message!?");
	
	// add waveform events
	// TODO
	
	int poll_ok = zmq::poll(&out_poll,1,poll_ms);
	if(poll_ok){
		std::cerr<<"GenerateDummyData polling error "<<poll_ok<<std::endl;
		return false;
	}
	if(!(out_poll.revents & ZMQ_POLLOUT)){
		std::cerr<<"GenerateDummyData no poll listener"<<std::endl;
		return false;
	}
	int send_ok = out_sock->send(msg_hdr, ZMQ_MORE);
	if(!send_ok){
		std::cerr<<"GenerateDummyData: Error "<<zmq_strerror(errno)<<" sending header!"<<std::endl;
		return false;
	}
	send_ok = out_sock->send(msg_data);
	if(!send_ok){
		std::cerr<<"GenerateDummyData: Error "<<zmq_strerror(errno)<<" sending data!"<<std::endl;
		return false;
	}
	
	// between each send, increment the header coarse counter by ~10ms
	// since each tick is 0.52ms, this means adding ~20 counts
	coarse_counter += 18+rand()%5;
	
	return true;
}

bool GenerateDummyData::Finalise(){
	delete out_sock;
	out_sock = nullptr;
	
	return true;
}

WCTEMPMTHit GenerateDummyData::MakeHit(unsigned int card_id, unsigned int channel_id, EventType type, unsigned int coarse_counter, bool is_waveform){
	WCTEMPMTHit tmp_hit;
	tmp_hit.SetCardID(card_id);
	tmp_hit.SetChannel(channel_id);
	tmp_hit.SetHeader((is_waveform) ? 0x10 : 0x01); // 0x10 for waveforms, 0x01 otherwise
	tmp_hit.SetEventType(static_cast<int>(type)); // 0: normal, 1: pedestal, 2: led, 3: cal, 0x0F: PPS
	tmp_hit.SetCoarseCounter(coarse_counter);
	return tmp_hit;
}










