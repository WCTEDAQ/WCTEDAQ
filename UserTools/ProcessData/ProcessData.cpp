#include "ProcessData.h"

ProcessData_args::ProcessData_args():Thread_args(){}

ProcessData_args::~ProcessData_args(){}


ProcessData::ProcessData():Tool(){}


bool ProcessData::Initialise(std::string configfile, DataModel &data){

  InitialiseTool(data);
  m_configfile=configfile;
  InitialiseConfiguration(configfile);
  //m_variables.Print();


  m_util=new Utilities();
  args=new ProcessData_args();
  LoadConfig();
  args->data=m_data;
  //  if(m_data->readout_windows==0) m_data->readout_windows=new std::deque<ReadoutWindow*>;
  m_util->CreateThread("test", &Thread, args);

  m_data->out_data_chunks= new std::map<unsigned int, MPMTCollection*>;  
  ExportConfiguration();
  
  return true;
}


bool ProcessData::Execute(){

  if(m_data->change_config){
    InitialiseConfiguration(m_configfile);
    LoadConfig();
    ExportConfiguration();
  }

  //printf("triggered_data::readout_windows = %d::%d\n", m_data->triggered_data.size(), m_data->readout_windows->size());
  // print somthing with it  args->readout_num;
  //usleep(100);
  return true;
}


bool ProcessData::Finalise(){

  m_util->KillThread(args);

  delete args;
  args=0;

  delete m_util;
  m_util=0;

  return true;
}

void ProcessData::Thread(Thread_args* arg){
  
  ProcessData_args* args=reinterpret_cast<ProcessData_args*>(arg);
  //printf("d1\n");
  // std::vector<unsigned int> erase_list;
  //printf("d2 size=%d\n",args->data->triggered_data.size());
  //std::map<unsigned int, MPMTData*> triggered_data;
  
  // some timer
  bool some=false;
  
  args->data->data_chunks_mtx.lock();
  /*
for(std::map<unsigned int, MPMTCollection*>::iterator it=args->data->data_chunks.begin(); it!=args->data->data_chunks.end(); it++){
      for(unsigned int i=0; i<it->second->mpmt_output.size(); i++){
        delete it->second->mpmt_output.at(i);
        it->second->mpmt_output.at(i)=0;
      }
      it->second->mpmt_output.clear();
      
      for(unsigned int i=0; i<it->second->triggers_info.size(); i++){
        delete it->second->triggers_info.at(i);
        it->second->triggers_info.at(i)=0;
      }
      it->second->triggers_info.clear();
      
      delete it->second;
      it->second-0;
 }
 args->data->data_chunks.clear();
  */
  std::map<unsigned int, MPMTCollection*>::iterator last=args->data->data_chunks.end();

  for(std::map<unsigned int, MPMTCollection*>::iterator it=args->data->data_chunks.begin(); it!=args->data->data_chunks.end(); it++){
    //     printf("%u < %u\n", it->first, ((args->data->current_coarse_counter >> 16 ) & 4294967295U) - 228881);
    //114440 228881
    if(((args->data->current_coarse_counter >> 16 ) & 4294967295U) > 114440 && it->first < ((args->data->current_coarse_counter >> 16 ) & 4294967295U) - 114440){
      last=it;
      last++;
      some=true;
    }
    else break;
  }
  if(some){
    args->local_chunks.insert(args->data->data_chunks.begin(), last);
    args->data->data_chunks.erase(args->data->data_chunks.begin(), last);
  }
  //std::swap(args->data->triggered_data.begin(),myints+7, myvector.begin());
   
 args->data->data_chunks_mtx.unlock();
  //


  
  

  //  printf("triggered size=%u\n", args->triggered_data.size());
  /*  
  for(std::map<unsigned int, MPMTData*>::iterator it=args->triggered_data.begin(); it!=args->triggered_data.end(); it++){
  delete it->second;
  }
  args->triggered_data.clear();
  */

 // if(args->local_chunks.size()) printf("size=%u\n", args->local_chunks.size());
  
  //unsigned int subset=0;
 for(std::map<unsigned int, MPMTCollection*>::iterator it=args->local_chunks.begin(); it!=args->local_chunks.end(); it++){
   
   //if(false){
   //   if(it->second->triggers.size() || it->second->leds.size()){
   if( it->second->triggers_info.size() || args->data->out_data_chunks->count(it->first)){
     
     args->data->out_data_chunks_mtx.lock();
     
     if(args->data->out_data_chunks->count(it->first)){
       
       (*args->data->out_data_chunks)[it->first]->mpmt_output.insert((*args->data->out_data_chunks)[it->first]->mpmt_output.end(), it->second->mpmt_output.begin(), it->second->mpmt_output.end());
       
       /*       (*args->data->out_data_chunks)[it->first]->triggers.insert((*args->data->out_data_chunks)[it->first]->triggers.end(), it->second->triggers.begin(), it->second->triggers.end());
		
		(*args->data->out_data_chunks)[it->first]->leds.insert((*args->data->out_data_chunks)[it->first]->leds.end(), it->second->leds.begin(), it->second->leds.end());
       */
       (*args->data->out_data_chunks)[it->first]->triggers_info.insert((*args->data->out_data_chunks)[it->first]->triggers_info.end(), it->second->triggers_info.begin(), it->second->triggers_info.end());
       
       delete it->second;
       it->second=0;
       //       printf("dam and blast!!!!%u\n", it->first);
     }
   else{
     (*args->data->out_data_chunks)[it->first]=it->second;
   }
 
   args->data->out_data_chunks_mtx.unlock();
 
   }
   else{
     for(unsigned int i=0; i<it->second->mpmt_output.size(); i++){
       delete it->second->mpmt_output.at(i);
       it->second->mpmt_output.at(i)=0;
     }
     for(unsigned int i=0; i<it->second->triggers_info.size(); i++){
       delete it->second->triggers_info.at(i);
       it->second->triggers_info.at(i)=0;
     }
     delete it->second;
   }
   it->second=0;
   
 }
 
 args->local_chunks.clear();
 
 //testing

 /*
for(std::map<unsigned int, MPMTCollection*>::iterator it=args->data->out_data_chunks->begin(); it!=args->data->out_data_chunks->end(); it++){

    for(unsigned int i=0; i<it->second->mpmt_output.size(); i++){
      delete it->second->mpmt_output.at(i);
      it->second->mpmt_output.at(i)=0;
    }
    for(unsigned int i=0; i<it->second->triggers_info.size(); i++){
      delete it->second->triggers_info.at(i);
      it->second->triggers_info.at(i)=0;
    }
    
    delete it->second;
    it->second=0;
  

 }

 args->data->out_data_chunks->clear();
 */
 
}

void ProcessData::LoadConfig(){

    if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
   
    
}
