#ifndef LED_H
#define LED_H

#include <string>
#include <iostream>

#include "Tool.h"
#include "DataModel.h"

/**
 * \struct LED_args_args
 *
 * This is a struct to place data you want your thread to access or exchange with it. The idea is the datainside is only used by the threa\d and so will be thread safe
 *
 * $Author: B.Richards $
 * $Date: 2019/05/28 10:44:00 $
 */

class LED;

struct LED_args:Thread_args{
  
  LED_args();
  ~LED_args();
  
  LED* parent;   // access to parent members (configuration varibles, functions)
  bool running;  // set by parent, read by thread
  
  bool last_running; // was thread active on last Thread call
  bool last_beam_active;
  boost::posix_time::ptime last_flash;
  unsigned long last_flash_counts;
  boost::posix_time::ptime last_beamspill_end;
  unsigned long last_beamspill_counts;
  boost::posix_time::time_duration lapse;
  unsigned int spills_since_last_flash;
  unsigned int sequence_num;
  unsigned int sequences_performed;
  
};

/**
 * \class LED
 *
 * This is a template for a Tool that produces a single thread that can be assigned a function seperate to the main thread. Please fill out the descripton and author information.
*
* $Author: B.Richards $
* $Date: 2019/05/28 10:44:00 $
*/

class LED: public Tool {


 public:
 friend class LED_args;

  LED(); ///< Simple constructor
  bool Initialise(std::string configfile,DataModel &data); ///< Initialise Function for setting up Tool resorces. @param configfile The path and name of the dynamic configuration file to read in. @param data A reference to the transient data class used to pass information between Tools.
  bool Execute(); ///< Execute function used to perform Tool purpose. 
  bool Finalise(); ///< Finalise funciton used to clean up resorces.


 private:

  static void Thread(Thread_args* arg); ///< Function to be run by the thread in a loop. Make sure not to block in it
  bool LoadConfig();
  bool LoadSequence();
  void ReinitSequence();
  std::string BuildLedJson(int sequence_num, int sequences_performed, unsigned long last_beamspill_counts, unsigned long& last_flash_counts);
  std::string BuildSWTriggerJson(unsigned int MPMT_ID, unsigned long last_flash_counts);
  
  std::string m_configfile;

  unsigned int spill_modulus;
  unsigned int delay_after_beam_counts;
  boost::posix_time::seconds delay_after_beam{1};
  
  unsigned int wait_to_start;
  unsigned int led_advance_counts;
  unsigned int waveform_presamples;
  std::vector<Store> firing_sequence;
  int sequence_repetitions;
  Store SWTriggerAlertStore;
  
  unsigned int sequence_period_counts;
  boost::posix_time::microseconds sequence_period{1};
  
  boost::posix_time::microseconds alert_prewarn{1};
  long int bens_offset;
  
  bool last_running; // was run started on last Execute call
  
  Utilities* m_util;         ///< Pointer to utilities class to help with threading
  LED_args* thread_args;     ///< thread args (also holds pointer to the thread)
  
  
};


#endif
