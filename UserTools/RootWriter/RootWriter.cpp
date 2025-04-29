#include "RootWriter.h"
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include <set>

RootWriter_args::RootWriter_args():Thread_args(){
	m_data=0;
	file_name=0;
	part_number=0;
	file_writeout_threshold=0;
	readout_window_num=0;
	readout_windows=new std::vector<PReadoutWindow*>{};
	mpmt_messages=new std::vector<MPMTMessage*>{};
	pps=new std::vector<WCTEMPMTPPS>{};
}

RootWriter_args::~RootWriter_args(){
	m_data=0;
	file_name=0;
	part_number=0;
	file_writeout_threshold=0;
	readout_window_num=0;
	for(size_t i=0; i<readout_windows->size(); ++i){
		delete readout_windows->at(i);
		readout_windows->at(i)=nullptr;
	}
	delete readout_windows;
	readout_windows=0;
	for(size_t i=0; i<mpmt_messages->size(); ++i){
		delete mpmt_messages->at(i);
		mpmt_messages->at(i)=nullptr;
	}
	delete mpmt_messages;
	mpmt_messages=0;
	delete pps; // elements are not on the heap
	pps=0;
}


RootWriter::RootWriter():Tool(){}


bool RootWriter::Initialise(std::string configfile, DataModel &data){
	
	InitialiseTool(data);
	m_configfile=configfile;
	
	m_util=new Utilities();
	thread_args=new RootWriter_args();
	
	thread_args->m_data= m_data;
	thread_args->file_name= &m_file_name;
	thread_args->part_number= &m_part_number;
	thread_args->period= boost::posix_time::seconds(3000);
	thread_args->file_writeout_threshold = &m_file_writeout_threshold;
	
	m_util->CreateThread("test", &Thread, thread_args);
	
	ExportConfiguration();
	
	// number of files written is shown on Status and taken from part number
	// default to 0 so that it shows sane values before run starts
	m_part_number=0;
	
	return true;
}


bool RootWriter::Execute(){
	
	if(m_data->change_config){
		InitialiseConfiguration(m_configfile);
		//m_variables.Print();
		if(!LoadConfig()){
			Log("Error in LoadConfig!",v_error,m_verbose);
		}
		ExportConfiguration();
	}
	if(m_data->run_start){
		m_part_number=0;
		thread_args->last=m_data->start_time; //boost::posix_time::microsec_clock::universal_time();
	}
	if(m_data->run_stop) thread_args->period=boost::posix_time::seconds(10);
	
	m_data->vars.Set("part",m_part_number);
	
	return true;
}


bool RootWriter::Finalise(){
  
	m_util->KillThread(thread_args);
	
	delete thread_args;
	thread_args=0;
	
	delete m_util;
	m_util=0;

	m_data->preadout_windows_mtx.lock();
	for(unsigned int i=0; i< m_data->preadout_windows->size(); i++){
	  delete m_data->preadout_windows->at(i);
	  m_data->preadout_windows->at(i)=0;
	}
	delete m_data->preadout_windows;
	m_data->preadout_windows=0;
	m_data->preadout_windows_mtx.unlock();

	m_data->pps_mtx.lock();
	delete m_data->pps;
	m_data->pps=0;
	m_data->pps_mtx.unlock();
	
	
	return true;
}

void RootWriter::Thread(Thread_args* arg){
	
	RootWriter_args* m_args=reinterpret_cast<RootWriter_args*>(arg);
	
	// calculate time since last file write
	m_args->lapse =  boost::posix_time::microsec_clock::universal_time() - m_args->last;
	
	// debug: check how long it took to write the last file
	static bool dothecheck=false;
	if(dothecheck){
		printf("RootWriter took %d seconds to write file vs writeout period %d\n",
		       m_args->lapse.total_seconds(), m_args->period.total_seconds());
		dothecheck=false;
		if(m_args->lapse.total_seconds() > m_args->period.total_seconds()){
			printf("Warning! RootWriter not keeping up with data taking rate!\n"); // TODO send warning alert
			if(m_args->m_data->running) m_args->m_data->services->SendAlarm("Warning RootWriter : RootWriter Not keeping up with data taking rate!'");
		}
	}
	
	// get number of readout windows waiting to be written
	size_t readouts_waiting = 0;
	if(m_args->m_data->raw_readout){
		m_args->m_data->mpmt_messages_mtx.lock();
		readouts_waiting = m_args->m_data->mpmt_messages->size();
		m_args->m_data->mpmt_messages_mtx.unlock();
	} else {
		m_args->m_data->preadout_windows_mtx.lock();
		readouts_waiting = m_args->m_data->preadout_windows->size();
		m_args->m_data->preadout_windows_mtx.unlock();
	}
	
	// if not yet time to write out a new file and number of waiting windows is below threshold...
	if( m_args->lapse < m_args->period && (*m_args->file_writeout_threshold<0 || readouts_waiting < *m_args->file_writeout_threshold )){
		// not time to write yet
		usleep(100);
		return;
	}
	
	// time to write!... probably.
	m_args->last = boost::posix_time::microsec_clock::universal_time();
	
	// but first, check if we have data to write
	bool makefile=false;
	
	// raw readout - pointers to the zmq messages received from the MPMTs
	if(m_args->m_data->raw_readout){
		
		m_args->m_data->mpmt_messages_mtx.lock();
		if(m_args->m_data->mpmt_messages->size()>0){
			std::swap(m_args->mpmt_messages, m_args->m_data->mpmt_messages);
			makefile=true;
		}
		m_args->m_data->mpmt_messages_mtx.unlock();
		
	} else {
		
		// normal readout mode - separate readout and PPS vectors
		
		// preadout_windows - readout windows formatted into vectors of hits, triggers, waveforms etc
		m_args->m_data->preadout_windows_mtx.lock();
		if(m_args->m_data->preadout_windows->size()>0){
			// swap readout_windows vector from the datamodel with our one
			std::swap(m_args->readout_windows, m_args->m_data->preadout_windows);
			makefile=true;
		}
		m_args->m_data->preadout_windows_mtx.unlock();
		 
		// PPS frames
		m_args->m_data->pps_mtx.lock();
		if(m_args->m_data->pps->size()>0){
			std::swap(m_args->pps, m_args->m_data->pps);
			makefile=true;
		}
		m_args->m_data->pps_mtx.unlock();
		
	}
	
	// if we triggered on time since last file but had nothing to write out, just return
	if(!makefile){
		return;
	}
	
	// data to write!
	printf("RootWriter writing out data\n");
	dothecheck=true;
	
	printf("RootWriter got %u readout windows to write\n",m_args->readout_windows->size());
	printf("RootWriter got %u mpmt_messags to write\n",m_args->mpmt_messages->size());
	printf("RootWriter got %u PPS frames to write\n",m_args->pps->size());
	 
	// format next filename
	std::stringstream filename;
	filename<<(*m_args->file_name)<<"R"<<m_args->m_data->run_number<<"S"<<m_args->m_data->sub_run_number<<"P"<<(*m_args->part_number)<<".root";
	
	// make output ROOT file
	TFile fout(filename.str().c_str(),"CREATE"); // not RECREATE - don't clobber existing files
	fout.SetCompressionLevel(0);
	// XXX possibly put on heap if it's DUMMY THICC?
	if(fout.IsZombie()){
		std::cerr<<"RootWriter Error! Unable to create output file "<<filename.str()<<std::endl;
		return;
	}
	
	/*
	TFile::TFile ( const char* fname1, Option_t* option = "", const char* ftitle = "", Int_t compress = 1)
	// for compression integer can use helper function to make value:
	ROOT::CompressionSettings(ROOT::kLZMA, 1)  // defined in "Compression.h"
	*/
	
	// DAQInto Tree (one entry):
	//     DAQInfo daq_info; // just contains run and subrun num, get from datamodel
	//
	// Data Tree (one entry per PReadoutWindow):
	//     vector<P_MPMTHit*> mpmt_hits;
	//     vector<P_MPMTHit*>* trigger_hits;
	//     vector<TriggerInfo*>* triggers_info;
	//     vector<WCTEMPMTWaveformHeader*> mpmt_waveforms;
	//     vector<HKMPMTHit*> hk_mpmt_hits;
	//     unsigned long readout_num;
	//     unsigned long start_counter;
	// 
	// PPS Tree (one entry per WCTEMPMTPPS):
	//     WCTEMPMTPPS pps;
	//
	// MPMTMessages (one entry per MPMTMessage):
	//    MPMTMessage message;
	//     
	// as the quickest route forward, ROOT can handle just leaving everything as pointers!
	
	// tree for header info
	TTree* t_daq_info = new TTree("daq_info","Run Information");
	DAQInfo daq_info;
	t_daq_info->Branch("daq_info", &daq_info);
	daq_info.run_number = m_args->m_data->run_number;
	daq_info.sub_run_number = m_args->m_data->sub_run_number;
	t_daq_info->Fill();
	
	if(m_args->m_data->raw_readout){
		
		// raw readout mode; just write received ZMQ message contents
		TTree* t_raw_msgs = new TTree("raw_data","Raw MPMT zmq message contents");
		// convert to MPMTMessage to MPMTMessageData, which encapsulate the zmq::message_t payloads without zmq dependency
		MPMTMessageData zdaq_hdr_tmp;
		MPMTMessageData* zdaq_hdr_tmp_p=&zdaq_hdr_tmp;
		MPMTMessageData zmpmt_msg_tmp;
		MPMTMessageData* zmpmt_msg_tmp_p=&zmpmt_msg_tmp;
		t_raw_msgs->Branch("daq_header", &zdaq_hdr_tmp);
		t_raw_msgs->Branch("mpmt_data", &zmpmt_msg_tmp);
		std::vector<MPMTMessage*>& msgs_ref = *(m_args->mpmt_messages);
		for(size_t i=0; i<m_args->mpmt_messages->size(); ++i){
			zdaq_hdr_tmp.nbytes = msgs_ref[i]->daq_header.size();
			zdaq_hdr_tmp.bytes = (unsigned char*)msgs_ref[i]->daq_header.data();
			zmpmt_msg_tmp.nbytes = msgs_ref[i]->mpmt_data.size();
			zmpmt_msg_tmp.bytes = (unsigned char*)msgs_ref[i]->mpmt_data.data();
			t_raw_msgs->Fill();
		}
		t_raw_msgs->ResetBranchAddresses();
		
		// cleanup
		for(size_t i=0; i<m_args->mpmt_messages->size(); ++i){
			delete m_args->mpmt_messages->at(i);
			m_args->mpmt_messages->at(i)=nullptr;
		}
		
		m_args->mpmt_messages->resize(0);
		// FIXME this will release the container's reserved capacity, but it would be more performant not to do so
		m_args->mpmt_messages->shrink_to_fit();
		
	} else {
		
		// normal readout mode; write decoded readouts
		// (applies for any trigger mode)
		
		// first PPS frames
		TTree* t_pps = new TTree("pps","PPS Frames");
		WCTEMPMTPPS pps_tmp;
		WCTEMPMTPPS* pps_tmp_p=&pps_tmp;
		TBranch* b_pps = t_pps->Branch("pps", &pps_tmp_p);
		std::vector<WCTEMPMTPPS>& ppsref = *(m_args->pps);
		for(size_t i=0; i<ppsref.size(); ++i){
			pps_tmp_p = &ppsref[i];
			t_pps->Fill();
		}
		t_pps->ResetBranchAddresses();
		
		// cleanup. 
		m_args->m_data->mon_pps_mtx.lock();
		if(m_args->m_data->mon_pps!=nullptr){
			// if we have a monitoring pps vector, put the PPS frames in there
			if(m_args->m_data->mon_pps->size()){
				m_args->m_data->mon_pps->insert(m_args->m_data->mon_pps->end(), m_args->pps->begin(), m_args->pps->end());
				m_args->pps->resize(0);
				m_args->pps->shrink_to_fit();
			} else {
				std::swap(m_args->m_data->mon_pps,m_args->pps);
			}
		} else {
			// otherwise just clear them
			// (don't need to delete the elements, they're not pointers)
			m_args->pps->resize(0);
			m_args->pps->shrink_to_fit();
		}
		m_args->m_data->mon_pps_mtx.unlock();
		
		// next readout windows
		// a PReadoutWindow will map to the data TTree, with one branch for each member vector
		// (TODO we could use MakeClass to turn this into a ReadoutWindow object during reading)
		// MakeDataTree helper function makes the branch structure by cloning a reference tree
		TTree* t_data = MakeDataTree("data", "Readout Windows");
		
		//std::set<PReadoutWindow*> rwps; // used in debug
		
		// rather than deleting, we may send a subset of readout windows to Monitoring Tool after writing them to file
		std::vector<PReadoutWindow*> preadouts_for_monitoring;
		
		// loop over readout windows
		printf("readout windows=%u\n", m_args->readout_windows->size());
		for(size_t i=0; i<m_args->readout_windows->size(); ++i){
			
			PReadoutWindow* prw = m_args->readout_windows->at(i);
			
			// debug check
			/*
			if(!rwps.insert(prw).second){
				std::cerr<<"READOUT WINDOW REPEATED IN ROOTWRITER!"<<std::endl;
				continue;
			}
			*/
			
			// triggers_info and trigger_hits in the PReadoutWindow are already pointers
			// to vectors so we can pass their addresses to SetBranchAddress directly.
			// the rest are vectors, so we need pointers to those vectors 
			// whose address we can pass to SetBranchAddress
			std::vector<P_MPMTHit*>* hits_p = &prw->mpmt_hits;
			std::vector<P_MPMTWaveformHeader*>* wfheaders_p = &prw->mpmt_waveforms;
			std::vector<HKMPMTHit*>* hk_mpmt_hits_p = &prw->hk_mpmt_hits;
			std::vector<TDCHit*>* tdc_hits_p = &prw->tdc_hits;
			std::vector<QDCHit*>* qdc_hits_p = &prw->qdc_hits;
			
			// we have a couple of simple variables too
			unsigned long spill_num = prw->spill_num;
			unsigned long readout_num = prw->readout_num;
			unsigned long start_counter = prw->start_counter;
			
			//printf("DEBUG PRINT OF READOUT WINDOW %lu!!!\n",i);
			//prw->Print();
			/*
			std::cout<<"readout window "<<i<<" has "<<hits_p->size()<<" hits, "
			         <<prw->trigger_hits->size()<<" trigger hits, "
			         <<prw->triggers_info->size()<<" triggers and "
			         <<wfheaders_p->size()<<" waveform headers"<<std::endl;
			*/
			
			// sanity checks if know what data to expect (e.g. if configured so in GenerateFakeData)
			/*
			if(prw->triggers_info->size()==0){
				printf("Warning! RootWriter found readout_window element with no triggers!\n");
			}
			if(hits_p->size()==0){
				std::cerr<<"no hits in event!"<<std::endl;
			}
			if(wfheaders_p->size()==0){
				std::cerr<<"no waveform headers in readout!"<<std::endl;
			}
			if(hits_p->size()!=wfheaders_p->size()){
				std::cerr<<"MISMATCHED NUM HITS AND WAVEFORMS!!!"<<std::endl;
				continue;
			}
			*/
			
			// waveform samples are immediately after each MPMTWaveformHeader in memory,
			// and the number of samples is in the header. But for ROOT to write these out
			// we need to group the pointer and num samples into a class with a streamer
			// (i.e. the MPMTWaveformSamples class)
			std::vector<MPMTWaveformSamples> waveform_samples(wfheaders_p->size());
			std::vector<MPMTWaveformSamples>* waveform_samples_p = &waveform_samples;
			
			// for debug checks
			//std::set<P_MPMTWaveformHeader*> whps;
			//std::set<unsigned char*> wsps;
			
			for(size_t j=0; j<wfheaders_p->size(); ++j){
				
				P_MPMTWaveformHeader* next_hdr = (*wfheaders_p)[j];
				
				if(next_hdr==nullptr || next_hdr->waveform_header==nullptr){
					std::cerr<<"RootWriter ERROR! null waveform header!"<<std::endl;
					assert(false);
					continue;
				}
				//next_hdr->Print();
				//std::cout<<"waveform header at "<<next_hdr->waveform_header
				//         <<", card "<<next_hdr->card_id<<std::endl;
				
				// debug check
				/*
				if(!whps.insert(next_hdr).second){
					std::cerr<<"WAVEFORM HEADER REPEATED IN ROOTWRITER!"<<std::endl;
					continue;
				}
				*/
				
				if(next_hdr->waveform_header->GetNumSamples()==0){
					std::cerr<<"RootWriter: ERROR! EMPTY WAVEFORMS (SAMPLES)!"<<std::endl;
					continue;
				}
				if(next_hdr->waveform_header->GetLength()==0){
					std::cerr<<"RootWriter: ERROR! EMPTY WAVEFORMS (BYTES)!"<<std::endl;
					continue;
				}
				
				// debug check, only valid if not using compressed waveforms
				/*
				int expected_bytes = std::ceil(double(next_hdr->waveform_header->GetNumSamples())*(12./8.));
				if(next_hdr->waveform_header->GetLength()!=expected_bytes){
					std::cerr<<"RootWriter mismatch between #samples ("
					         <<next_hdr->waveform_header->GetNumSamples()
					         <<" and #bytes ("<<next_hdr->waveform_header->GetLength()
					         <<", expect "<<expected_bytes<<")"<<std::endl;
					continue;
				}
				*/
				
				if(next_hdr->bytes==nullptr){
					std::cerr<<"RootWriter: ERROR! NULL WAVEFORM SAMPLES!"<<std::endl;
					unsigned char* header_addr=(unsigned char*)(next_hdr->waveform_header->GetData());
					unsigned char* bytes_addr=header_addr+MPMTWaveformHeader::GetSize();
					printf("header at %p has bytes member %p ,should be %p which contains: [",
					       header_addr,next_hdr->bytes,bytes_addr);
					for(int k=0; k<next_hdr->waveform_header->GetLength(); ++k){
						if(k>0) printf(",");
						printf("0x%x ",bytes_addr[k]);
					}
					printf("]\n");
					continue;
				}
				
				// debug check
				/*
				if(!wsps.insert(next_hdr->bytes).second){
					std::cerr<<"WAVEFORM SAMPLES REPEATED IN ROOTWRITER!"<<std::endl;
					continue;
				}
				*/
				
				// could switch to push_back if ROOT doesn't like to serialise with nullptr. Seems to be fine though.
				waveform_samples[j].bytes = next_hdr->bytes;
				waveform_samples[j].nbytes = next_hdr->waveform_header->GetLength();
				waveform_samples[j].nsamples = next_hdr->waveform_header->GetNumSamples();
				
			}
			
			// TODO any sanity checks on hk_mpmt_hits, tdc_hits, qdc_hits...?
			
			// update branches to read from these objects
			t_data->SetBranchAddress("mpmt_hits",&hits_p);
			t_data->SetBranchAddress("trigger_hits",&prw->trigger_hits);
			t_data->SetBranchAddress("trigger_infos",&prw->triggers_info);
			t_data->SetBranchAddress("waveform_headers",&wfheaders_p);
			t_data->SetBranchAddress("waveform_samples",&waveform_samples_p);
			t_data->SetBranchAddress("hk_mpmt_hits",&hk_mpmt_hits_p);
			t_data->SetBranchAddress("tdc_hits",&tdc_hits_p);
			t_data->SetBranchAddress("qdc_hits",&qdc_hits_p);
			t_data->SetBranchAddress("readout_num",&readout_num);
			t_data->SetBranchAddress("spill_num",&spill_num);
			t_data->SetBranchAddress("start_counter",&start_counter);
			
			// add data to tree
			t_data->Fill();
			
			// just to be sure ROOT doesn't do anything funny
			t_data->ResetBranchAddresses();
		
			// if sending this readout window to monitoring, just grab its pointer
			m_args->m_data->mon_preadout_ratio=1;
			if((m_args->m_data->mon_preadout_ratio>0) && i==m_args->readout_windows->size() -1){
			  //(m_args->readout_window_num%m_args->m_data->mon_preadout_ratio)==0){

			  preadouts_for_monitoring.push_back(prw);

			} else {
			  // otherwise delete it
			  delete prw;
			}
			m_args->readout_window_num++;
		}
		
		t_data->ResetBranchAddresses();
		
		// debug check if we skipped writing any readouts
		if(t_data->GetEntries()!=m_args->readout_windows->size()){
			std::cerr<<"MISMATCH BETWEEN READOUTS TO WRITE OUT "<<m_args->readout_windows->size()
			         <<" AND O<UTPUT TREE SIZE "<<t_data->GetEntries()<<"!!!\n";
		}
		
		// clear vectors for next file
		m_args->readout_windows->resize(0);
		// FIXME this will release the container's reserved capacity, but it would be more performant not to do so
		m_args->readout_windows->shrink_to_fit();
		
		printf("output file data tree will contain %lu entries\n",t_data->GetEntries());
		
		// send the new monitoring readout windows
		if(preadouts_for_monitoring.size()){
			m_args->m_data->mon_preadout_windows_mtx.lock();
			m_args->m_data->mon_preadout_windows.insert(m_args->m_data->mon_preadout_windows.end(),
			                                            preadouts_for_monitoring.begin(),preadouts_for_monitoring.end());
			m_args->m_data->mon_preadout_windows_mtx.unlock();
			// FIXME could add a check that this vector does not get too long, indicating something is wrong
			preadouts_for_monitoring.clear(); // redundant tbh as it'll go out of scope anyway
		}
		
	}
	
	// write file and close
	fout.Write();
	fout.Close();
	printf("File %s Written!\n",filename.str().c_str());
	//Log("File "+filename.str()+" Written",v_message,m_verbose); // can't log in thread, send alart instead
	(*m_args->part_number)++;
	
	//m_data->Toolchain->verbose=10;
	
	return;
}

bool RootWriter::LoadConfig(){
	
	if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
	if(!m_variables.Get("file_path",m_file_name)) m_file_name="./data"; // including path, R**S**P**.root will be appended
	if(!m_variables.Get("file_writeout_period",m_file_writeout_period)) m_file_writeout_period=60; // 300
	if(!m_variables.Get("file_writeout_threshold",m_file_writeout_threshold)) m_file_writeout_threshold=-1;
	thread_args->period=boost::posix_time::seconds(m_file_writeout_period);
	
	return true;
}

TTree* RootWriter::MakeDataTree(std::string name, std::string title){
	// simplest way to make a new TTree with a given structure
	// is to clone one with that structure.
	static TTree* t_ref = nullptr;
	// make the reference if we don't have it yet
	if(t_ref==nullptr){
		TDirectory* prevdir = gDirectory;
		gROOT->cd();
		t_ref = new TTree("ref","ref");
		std::vector<P_MPMTHit*> mpmt_hits;
		std::vector<P_MPMTHit*> trigger_hits;
		std::vector<TriggerInfo*> triggers_info;
		std::vector<P_MPMTWaveformHeader*> mpmt_waveforms;
		std::vector<MPMTWaveformSamples> waveform_samples;
		std::vector<HKMPMTHit*> hk_mpmt_hits;
		std::vector<TDCHit*> tdc_hits;
		std::vector<QDCHit*> qdc_hits;
		unsigned long spill_num;
		unsigned long readout_num;
		unsigned long start_counter;
		
		int bufsize=32000; // ROOT default
		int splitlevel=0;  // don't split class members into different branches, it slows file writing (and is really annoying!)
		t_ref->Branch("mpmt_hits",&mpmt_hits,bufsize,splitlevel);
		t_ref->Branch("trigger_hits",&trigger_hits,bufsize,splitlevel);
		t_ref->Branch("trigger_infos",&triggers_info,bufsize,splitlevel);
		t_ref->Branch("waveform_headers",&mpmt_waveforms,bufsize,splitlevel);
		t_ref->Branch("waveform_samples",&waveform_samples,bufsize,splitlevel);
		
		t_ref->Branch("hk_mpmt_hits",&hk_mpmt_hits,bufsize,splitlevel);
		t_ref->Branch("tdc_hits",&tdc_hits,bufsize,splitlevel);
		t_ref->Branch("qdc_hits",&qdc_hits,bufsize,splitlevel);
		t_ref->Branch("spill_num",&spill_num,"spill_num/g");
		t_ref->Branch("readout_num",&readout_num,"readout_num/g");
		t_ref->Branch("start_counter",&start_counter,"start_counter/g");
		
		t_ref->ResetBranchAddresses();
		prevdir->cd();
	}
	// make a clone
	TTree* t = (TTree*)t_ref->Clone(name.c_str());
	t->SetTitle(title.c_str());
	return t;
}

