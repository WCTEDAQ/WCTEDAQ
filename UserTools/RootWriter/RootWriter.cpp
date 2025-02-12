#include "RootWriter.h"
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include <set>

RootWriter_args::RootWriter_args():Thread_args(){
	data=0;
	file_name=0;
	part_number=0;
	file_writeout_period=0;
	readout_windows=new std::vector<PReadoutWindow*>{};
}

RootWriter_args::~RootWriter_args(){
	data=0;
	file_name=0;
	part_number=0;
	file_writeout_period=0;
	delete readout_windows; // do not delete elements, we do not own them
	readout_windows=0;
}


RootWriter::RootWriter():Tool(){}


bool RootWriter::Initialise(std::string configfile, DataModel &data){
	
	InitialiseTool(data);
	m_configfile=configfile;
	
	m_util=new Utilities();
	thread_args=new RootWriter_args();
	
	thread_args->data= m_data;
	thread_args->file_name= &m_file_name;
	thread_args->part_number= &m_part_number;
	thread_args->file_writeout_period= & m_file_writeout_period;
	
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
		thread_args->last=boost::posix_time::microsec_clock::universal_time();
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
	
	return true;
}

void RootWriter::Thread(Thread_args* arg){
	
	RootWriter_args* m_args=reinterpret_cast<RootWriter_args*>(arg);
	
	// check if time to write out a new file
	m_args->lapse =  boost::posix_time::microsec_clock::universal_time() - m_args->last;
	if(m_args->lapse < m_args->period){
		// not yet
		usleep(100);
		return;
	}
	// time to write!
	m_args->last= boost::posix_time::microsec_clock::universal_time();
	
	// check if we have data to write
	m_args->data->preadout_windows_mtx.lock();
	if(m_args->data->preadout_windows->size()==0){
		// nothing to write
		m_args->data->preadout_windows_mtx.unlock();
		return;
	}
	// data to write!
	printf("RootWriter writing out data\n");
	
	// switch out the readout_windows vector from the datamodel with our one
	std::swap(m_args->readout_windows, m_args->data->preadout_windows);
	// TODO perhaps if cpu is a bottleneck we could put this file writing into a Job and parallelise writing
	// multiple file at once. If so each file writer job must have its own readout_windows vector instance
	m_args->data->preadout_windows_mtx.unlock();
	printf("RootWriter got %u readout windows to write\n",m_args->readout_windows->size());
	 
	// format next filename XXX would need synchronization for parallel writers
	std::stringstream filename;
	filename<<(*m_args->file_name)<<"R"<<m_args->data->run_number<<"S"<<m_args->data->sub_run_number<<"P"<<(*m_args->part_number)<<".root";
	
	// make output ROOT file
	TFile fout(filename.str().c_str(),"CREATE"); // not RECREATE - don't clobber existing files
	// XXX possibly put on heap if it's DUMMY THICC?
	if(fout.IsZombie()){
		std::cerr<<"RootWriter Error! Unable to create output file "<<filename.str()<<std::endl;
		return;
	}
	
	// WCTERawData:
	//     DAQInfo daq_info;
	//     vector<PReadoutWindow> readout_windows;
	// PReadoutWindow:
	//     vector<P_MPMTHit*> mpmt_hits;
	//     vector<P_MPMTHit*>* trigger_hits;
	//     vector<TriggerInfo*>* triggers_info;
	//     vector<WCTEMPMTWaveformHeader*> mpmt_waveforms;
	//     TODO output file will need another variable to store actual samples
	//     these are in memory immediately after the corresponding header
	//     and the header contains their size
	// as the quickest route forward, ROOT can handle just leaving everything as pointers!
	
	// tree for header info
	TTree* t_daq_info = new TTree("daq_info","Run Information");
	DAQInfo daq_info;
	t_daq_info->Branch("daq_info", &daq_info);
	daq_info.run_number = m_args->data->run_number;
	daq_info.sub_run_number = m_args->data->sub_run_number;
	t_daq_info->Fill();
	
	// a PReadoutWindow will map to the data TTree
	// with one branch for each member vector
	// we can use MakeClass to turn this into a ReadoutWindow object during reading
	// while preserving the option to draw directly from or disable some branches etc
	// helper function makes the branch structure by cloning a reference tree
	TTree* t_data = MakeDataTree("data", "Readout Windows");
	
	// loop over readout windows
	printf("readout windows=%u\n", m_args->readout_windows->size());
	
	std::set<PReadoutWindow*> rwps;
	for(size_t i=0; i<m_args->readout_windows->size(); ++i){
		
		PReadoutWindow* prw = m_args->readout_windows->at(i);
		
		// FIXME FIXME FIXME stupid debug check remove 
		if(!rwps.insert(prw).second){
			std::cerr<<"READOUT WINDOW REPEATED IN ROOTWRITER!"<<std::endl;
			continue;
		}
		
		// triggers_info and trigger_hits in the PReadoutWindow are already pointers
		// to vectors so we can pass their addresses to SetBranchAddress directly
		// mpmt_hits and mpmt_waveforms are vectors, so we need to make pointers
		// to those vectors whose address we can pass to SetBranchAddress
		std::vector<P_MPMTHit*>* hits = &prw->mpmt_hits;
		std::vector<P_MPMTWaveformHeader*>* waveformheaders = &prw->mpmt_waveforms;
		
		//printf("DEBUG PRINT OF READOUT WINDOW %lu!!!\n",i);
		//prw->Print();
		std::cout<<"readout window "<<i<<" has "<<hits->size()<<" hits, "
		         <<prw->trigger_hits->size()<<" trigger hits, "
		         <<prw->triggers_info->size()<<" triggers and "
		         <<waveformheaders->size()<<" waveform headers"<<std::endl;
		
		// XXX XXX DEBUG XXX XXX getting corrupt data but this may not be the case
		/*
		if(hits->size()!=waveformheaders->size()){
			std::cerr<<"MISMATCHED NUM HITS AND WAVEFORMS!!!"<<std::endl;
			continue;
		}
		*/
		
		// sanity checks
		/*
		if(prw->triggers_info->size()==0){
			printf("Warning! RootWriter found readout_window element with no triggers!\n");
		}
		if(hits->size()==0){
			std::cerr<<"no hits in event!"<<std::endl;
		}
		if(waveformheaders->size()==0){
			std::cerr<<"no waveform headers in readout!"<<std::endl;
		}
		
		for(size_t j=0; j<hits->size(); ++j){
			if(hits->at(j)->)
		}
		*/
		
		// waveform samples are immediately after each MPMTWaveformHeader in memory,
		// and the number of samples is in the header. But for ROOT to write these out
		// we need to group the pointer and num samples into a class with a streamer
		// (i.e. the MPMTWaveformSamples class)
		std::vector<MPMTWaveformSamples> waveform_samples(waveformheaders->size());
		//std::vector<MPMTWaveformSamples> waveform_samples; // can't pre-allocate if we use 'continue'
		std::vector<MPMTWaveformSamples>* waveform_samples_p = &waveform_samples;
		
		// FIXME FIXME FIXME stupid debug checks remove
		std::set<P_MPMTWaveformHeader*> whps;
		std::set<unsigned char*> wsps;
		
		for(size_t i=0; i<waveformheaders->size(); ++i){
			
			P_MPMTWaveformHeader* next_hdr = (*waveformheaders)[i];
			
			if(next_hdr==nullptr || next_hdr->waveform_header==nullptr){
				std::cerr<<"RootWriter ERROR! null waveform header!"<<std::endl;
				assert(false);
				//waveform_samples[i].nbytes=0;
				//waveform_samples[i].bytes=0;
				continue;
			}
			
			// FIXME FIXME FIXME stupid debug checks remove
			if(!whps.insert(next_hdr).second){
				std::cerr<<"WAVEFORM HEADER REPEATED IN ROOTWRITER!"<<std::endl;
				//waveform_samples[i].nbytes=0;
				//waveform_samples[i].bytes=0;
				continue;
			}
			
			//next_hdr->Print();
			//std::cout<<"waveform header at "<<next_hdr->waveform_header
			//         <<", card "<<next_hdr->card_id<<", spill "<<next_hdr->spill_num<<std::endl;
			
			if(next_hdr->waveform_header->GetNumSamples()==0){
				std::cerr<<"RootWriter: ERROR! EMPTY WAVEFORMS (SAMPLES)!"<<std::endl;
				//waveform_samples[i].nbytes=0;
				//waveform_samples[i].bytes=0;
				continue;
			}
			if(next_hdr->waveform_header->GetLength()==0){
				std::cerr<<"RootWriter: ERROR! EMPTY WAVEFORMS (BYTES)!"<<std::endl;
				//waveform_samples[i].nbytes=0;
				//waveform_samples[i].bytes=0;
				continue;
			}
			
			// FIXME expensive debug check! remove (think it's moving upstream)
			int expected_bytes = std::ceil(double(next_hdr->waveform_header->GetNumSamples())*(12./8.));
			if(next_hdr->waveform_header->GetLength()!=expected_bytes){
				std::cerr<<"RootWriter mismatch between #samples ("
				         <<next_hdr->waveform_header->GetNumSamples()
				         <<" and #bytes ("<<next_hdr->waveform_header->GetLength()
				         <<", expect "<<expected_bytes<<")"<<std::endl;
				//waveform_samples[i].nbytes=0;
				//waveform_samples[i].bytes=0;
				continue;
			}
			
			if(next_hdr->bytes==nullptr){
				std::cerr<<"RootWriter: ERROR! NULL WAVEFORM SAMPLES!"<<std::endl;
				unsigned char* header_addr=(unsigned char*)(next_hdr->waveform_header->GetData());
				unsigned char* bytes_addr=header_addr+MPMTWaveformHeader::GetSize();
				printf("header at %p has bytes member %p ,should be %p which contains: [",
				       header_addr,next_hdr->bytes,bytes_addr);
				for(int j=0; j<next_hdr->waveform_header->GetLength(); ++j){
					if(j>0) printf(", ");
					printf("%f",bytes_addr[j]);
				}
				printf("]\n");
				
				//waveform_samples[i].nbytes=0;
				//waveform_samples[i].bytes=0;
				continue;
			}
			
			// FIXME FIXME FIXME stupid debug checks remove
			if(!wsps.insert(next_hdr->bytes).second){
				std::cerr<<"WAVEFORM SAMPLES REPEATED IN ROOTWRITER!"<<std::endl;
				//waveform_samples[i].nbytes=0;
				//waveform_samples[i].bytes=0;
				continue;
			}
			
			//waveform_samples[i].bytes = (unsigned char*)((next_hdr->waveform_header->GetData())+MPMTWaveformHeader::GetSize());
			// FIXME switch to push_back if ROOT doesn't like to serialise with nullptr
			waveform_samples[i].bytes = next_hdr->bytes;
			waveform_samples[i].nbytes = next_hdr->waveform_header->GetLength();
			waveform_samples[i].nsamples = next_hdr->waveform_header->GetNumSamples();
			
		}
		
		// update branches to read from these objects
		t_data->SetBranchAddress("mpmt_hits",&hits);
		t_data->SetBranchAddress("trigger_hits",&prw->trigger_hits);
		t_data->SetBranchAddress("trigger_infos",&prw->triggers_info);
		t_data->SetBranchAddress("waveform_headers",&waveformheaders);
		t_data->SetBranchAddress("waveform_samples",&waveform_samples_p);
		
		// add data to tree
		t_data->Fill();
		
		// just to be sure ROOT doesn't do anything funny
		t_data->ResetBranchAddresses();
		
		// free memory of P_ReadoutWindow (but not the members)
		delete prw;
	}
	
	// write file and close
	t_data->ResetBranchAddresses();
	printf("output file data tree will contain %ul entries\n",t_data->GetEntries());
	if(t_data->GetEntries()!=m_args->readout_windows->size()){
		std::cerr<<"MISMATCH BETWEEN READOUTS TO WRITE OUT "<<m_args->readout_windows->size()
		        <<" AND OUTPUT TREE SIZE "<<t_data->GetEntries()<<"!!!\n"<<std::endl;
	}
	fout.Write();
	fout.Close();
	printf("File %s Written!\n",filename.str().c_str());
	(*m_args->part_number)++;
	
	// clear
	m_args->readout_windows->resize(0);
	m_args->readout_windows->shrink_to_fit();
	
	return;
}

bool RootWriter::LoadConfig(){
	
	if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
	if(!m_variables.Get("file_path",m_file_name)) m_file_name="./data"; // including path, R**S**P**.root will be appended
	if(!m_variables.Get("file_writeout_period",m_file_writeout_period)) m_file_writeout_period=60; // 300
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
		
		int bufsize=32000; // ROOT default
		int splitlevel=0;  // don't split class members into different branches, it slows file writing (and is really annoying!)
		t_ref->Branch("mpmt_hits",&mpmt_hits,bufsize,splitlevel);
		t_ref->Branch("trigger_hits",&trigger_hits,bufsize,splitlevel);
		t_ref->Branch("trigger_infos",&triggers_info,bufsize,splitlevel);
		t_ref->Branch("waveform_headers",&mpmt_waveforms,bufsize,splitlevel);
		t_ref->Branch("waveform_samples",&waveform_samples,bufsize,splitlevel);
		t_ref->ResetBranchAddresses();
		prevdir->cd();
	}
	// make a clone
	TTree* t = (TTree*)t_ref->Clone(name.c_str());
	t->SetTitle(title.c_str());
	return t;
}

