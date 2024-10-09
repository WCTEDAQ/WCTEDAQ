0#ifndef Trigger_H
#define Trigger_H

#include <string>
#include <iostream>

#include "Tool.h"
#include "DataModel.h"

/**
 * \struct Trigger_args_args
 *
 * This is a struct to place data you want your thread to access or exchange with it. The idea is the datainside is only used by the threa\d and so will be thread safe
 *
 * $Author: B.Richards $
 * $Date: 2019/05/28 10:44:00 $
 */

struct Trigger_args:Thread_args{

  Trigger_args();
  ~Trigger_args();
  DataModel* data;
  std::vector< bool (*)(void*)> trigger_functions;
  std::vector<Trigger_algo_args*> trigger_algo_args;
  std::vector<std::string> triggers;
  MPMTData* sorted_data;
  unsigned int bin;
  std::map<TriggerType, unsigned long> pre_trigger;
  std::map<TriggerType, unsigned long> post_trigger;
  std::map<TriggerType, unsigned long> offset_trigger;
 
};

/**
 * \class Trigger
 *
 * This is a template for a Tool that produces a single thread that can be assigned a function seperate to the main thread. Please fill out the descripton and author information.
*
* $Author: B.Richards $
* $Date: 2019/05/28 10:44:00 $
*/

class Trigger: public Tool {


 public:

  Trigger(); ///< Simple constructor
  bool Initialise(std::string configfile,DataModel &data); ///< Initialise Function for setting up Tool resorces. @param configfile The path and name of the dynamic configuration file to read in. @param data A reference to the transient data class used to pass information between Tools.
  bool Execute(); ///< Executre function used to perform Tool perpose. 
  bool Finalise(); ///< Finalise funciton used to clean up resorces.


 private:

  
  static void Thread(Thread_args* arg); ///< Function to be run by the thread in a loop. Make sure not to block in it
  static bool TriggerJob(void* data);
  Utilities* m_util;  ///< Pointer to utilities class to help with threading
  Trigger_args* args; ///< thread args (also holds pointer to the thread)
  
};


#endif
