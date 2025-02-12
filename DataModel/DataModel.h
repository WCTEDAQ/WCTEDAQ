#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <map>
#include <string>
#include <vector>
#include <unordered_map>

#include <zmq.hpp>

#include <caen++/v1290.hpp>
#include <caen++/v792.hpp>

//#include "TTree.h"o

#include "Store.h"
#include "BoostStore.h"
//#include "DAQLogging.h"
#include "DAQUtilities.h"
#include "SlowControlCollection.h"
#include "DAQDataModelBase.h"
#include "ThreadLoop.h"
#include "WCTERawData.h"
#include "DAQHeader.h"
#include "WCTEMPMTPPS.h"
#include "MPMTData.h"
#include "VMEReadout.h"
#include <unordered_map>

#include "TDCHit.h"
#include "QDCHit.h"

#include <MPMTMessages.h>

using namespace ToolFramework;



/*o*
 * \class DataModel
 *
 * This class Is a transient data model class for your Tools within the ToolChain. If Tools need to comunicate they pass all data objects through the data model. There fore inter tool data objects should be deffined in this class. 
 *
 *
 * $Author: B.Richards $ 
 * $Date: 2019/05/26 18:34:00 $
 * Contact: b.richards@qmul.ac.uk
 *
 */

class DataModel : public DAQDataModelBase {
  
  
public:
  
  DataModel(); ///< Simple constructor 

  //TTree* GetTTree(std::string name);
  //void AddTTree(std::string name,TTree *tree);
  //void DeleteTTree(std::string name,TTree *tree);

  ThreadLoop vme_readout_loop;
  VMEReadout<QDCHit> qdc_readout;
  VMEReadout<TDCHit> tdc_readout;
  unsigned long vme_poped;
  
  bool load_config;
  bool change_config;
  bool run_start;
  bool run_stop;
  bool sub_run;
  boost::posix_time::ptime start_time;
  unsigned long current_coarse_counter;

  unsigned long run_number;
  unsigned long sub_run_number;
  unsigned int run_configuration;
  bool running;
  
  
  JobQueue job_queue;
  unsigned int thread_cap;
  unsigned int thread_num;

  std::mutex unsorted_data_mtx;
  std::unordered_map<unsigned int, MPMTData*> unsorted_data;
  
  std::mutex sorted_data_mtx;
  std::map<unsigned int, MPMTData*> sorted_data;

  std::mutex triggered_data_mtx;
  std::map<unsigned int, MPMTData*> triggered_data;
  
  std::mutex readout_windows_mtx;
  std::deque<ReadoutWindow*>* readout_windows;

  std::mutex monitoring_store_mtx;
  Store monitoring_store;
  std::map<std::string, unsigned int> hit_map;
  unsigned int qtc_transfer_num;
  unsigned int adc_transfer_num;

  std::map<std::string, bool (*)(void*)> trigger_functions;
  std::map<std::string, Store*> trigger_vars;

  std::mutex trigger_mtx;
  bool beam_active;
  bool beam_stopping;
  unsigned long spill_num;
  unsigned long vme_event_num;
  unsigned long readout_num;

  bool raw_readout;
  bool hardware_trigger;
  bool nhits_trigger;

  std::map<unsigned int, MPMTCollection*> data_chunks;
  std::mutex data_chunks_mtx;

  std::map<unsigned int, MPMTCollection*>* out_data_chunks;
  std::mutex out_data_chunks_mtx;

  std::mutex preadout_windows_mtx;
  std::vector<PReadoutWindow*>* preadout_windows;
  
  unsigned int time_corrections[200];
  
  std::mutex pps_mtx;
  std::vector<WCTEMPMTPPS>* pps;

  std::mutex mpmt_messages_mtx;
  std::vector<MPMTMessage*>* mpmt_messages;

  void* AlertSubscribe(const std::string& alert, ToolFramework::AlertFunction);
  void AlertUnsubscribe(const std::string& alert, void* handle);
  
private:

  // We need to subscribe V1290 and V792 to the beam spill alert, but ToolDAQ
  // does not support multiple receivers on an alert.
  std::unordered_map<std::string, std::list<ToolFramework::AlertFunction>> alerts;
  
  
  //std::map<std::string,TTree*> m_trees; 
  
  
  
};



#endif
