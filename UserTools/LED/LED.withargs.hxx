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

struct LED_args:Thread_args{

  LED_args();
  ~LED_args();
  
  boost::posix_time::microseconds sequence_period{1};
  boost::posix_time::milliseconds alert_prewarn{1};
  unsigned int spill_modulus;
  boost::posix_time::seconds delay_after_beam{1};
  unsigned int wait_to_start;
  DataModel* m_data;
  
  bool last_beam_active;
  boost::posix_time::ptime last_flash;
  unsigned long last_flash_counts;
  boost::posix_time::ptime last_beamspill_end;
  unsigned long last_beamspill_counts;
  boost::posix_time::time_duration lapse;
  unsigned int spills_since_last_flash;
  unsigned int sequence_num;
  unsigned int sequences_performed;
  int last_running;
  
  //int sequence_repetitions;
  //std::vector<Store> firing_sequence;

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

  LED(); ///< Simple constructor
  bool Initialise(std::string configfile,DataModel &data); ///< Initialise Function for setting up Tool resorces. @param configfile The path and name of the dynamic configuration file to read in. @param data A reference to the transient data class used to pass information between Tools.
  bool Execute(); ///< Executre function used to perform Tool perpose. 
  bool Finalise(); ///< Finalise funciton used to clean up resorces.


 private:

  static void Thread(Thread_args* arg); ///< Function to be run by the thread in a loop. Make sure not to block in it
  bool LoadConfig();
  bool LoadSequence();
  void ReinitSequence();
  static std::string BuildMPMTJson(int sequence_num, int sequences_performed, int last_beamspill_counts, long unsigned int& last_start_counts);
  

  unsigned int spill_modulus;
  boost::posix_time::seconds delay_after_beam{1};
  unsigned int wait_to_start;
  boost::posix_time::microseconds sequence_period{1};
  unsigned int last_start_counts;
  
  static int sequence_repetitions;
  static unsigned int led_advance_counts;
  static unsigned int sequence_period_counts;
  
  static std::vector<Store> firing_sequence;
  static unsigned int delay_after_beam_counts;
  
  std::string m_configfile;
  unsigned int alert_prewarn_counts;
  boost::posix_time::milliseconds alert_prewarn{1};
  
  // for threaded version
  Utilities* m_util;  ///< Pointer to utilities class to help with threading
  LED_args* args;     ///< thread args (also holds pointer to the thread)
  
  // for unthreaded version
  /*
  bool last_beam_active;
  boost::posix_time::ptime last;
  unsigned long last_start_counts;
  boost::posix_time::time_duration lapse;
  boost::posix_time::ptime last_beamspill_end;
  unsigned long last_beamspill_counts;
  unsigned int spills_since_last_flash;
  unsigned int sequence_num;
  unsigned int sequences_performed;
  */


};


#endif
