#ifndef SWTrigger_H
#define SWTrigger_H

#include <string>
#include <iostream>

#include "Tool.h"
#include "DataModel.h"

class SWTrigger;

// class to encapsulate arguments passed to the background Thread
struct SWTrigger_args:Thread_args{
  
  SWTrigger_args();
  ~SWTrigger_args();
  
  SWTrigger* parent;   // access to parent members (configuration varibles, functions)
  bool running;        // set by parent, read by thread
  bool last_running;   // was thread active on last Thread call
  
  bool last_beam_active;
  boost::posix_time::ptime last_trigger;
  unsigned long last_trigger_counts;
  boost::posix_time::ptime last_beamspill_end;
  unsigned long last_beamspill_counts;
  boost::posix_time::time_duration lapse;
  unsigned int spills_since_last_trigger;
  unsigned int sequence_num;
  unsigned int sequences_performed;
  
};

/**
 * \class SWTrigger
 *
 * This is a Tool that produces software trigger alerts
*
* $Author: M. O'Flaherty $
* $Date: 2025-03-10 $
*/

class SWTrigger: public Tool {

 public:
 friend class SWTrigger_args;

  SWTrigger();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();

 private:

  static void Thread(Thread_args* arg);
  bool LoadConfig();
  bool LoadSequence();
  void ReinitSequence();
  std::string BuildMPMTJson(unsigned long last_beamspill_counts, unsigned long& last_trigger_counts);
  
  std::string m_configfile;

  unsigned int spill_modulus;
  unsigned int delay_after_beam_counts;
  boost::posix_time::seconds delay_after_beam{1};
  
  unsigned int wait_to_start;
  unsigned int SWTrigger_advance_counts;
  Store software_trg;
  
  unsigned int sequence_period_counts;
  boost::posix_time::microseconds sequence_period{1};
  
  boost::posix_time::microseconds alert_prewarn{1};
  long int bens_offset;
  
  bool last_running; // was run started on last Execute call
  
  Utilities* m_util;         ///< Pointer to utilities class to help with threading
  SWTrigger_args* thread_args;     ///< thread args (also holds pointer to the thread)
  
};


#endif
