#ifndef Trigger2_H
#define Trigger2_H

#include <string>
#include <iostream>

#include "Tool.h"
#include "DataModel.h"

/**
 * \struct Trigger2_args_args
 *
 * This is a struct to place data you want your thread to access or exchange with it. The idea is the datainside is only used by the threa\d and so will be thread safe
 *
 * $Author: B.Richards $
 * $Date: 2019/05/28 10:44:00 $
 */

struct Trigger2_args:Thread_args{

  Trigger2_args();
  ~Trigger2_args();
  DataModel* data;
  std::vector< bool (*)(void*)> trigger_functions;
  std::vector<Trigger_algo_args*> trigger_algo_args;
  std::vector<std::string> triggers;
  MPMTData* sorted_data;
  unsigned int bin;
  
  
};

/**
 * \class Trigger2
 *
 * This is a template for a Tool that produces a single thread that can be assigned a function seperate to the main thread. Please fill out the descripton and author information.
*
* $Author: B.Richards $
* $Date: 2019/05/28 10:44:00 $
*/

class Trigger2: public Tool {


 public:

  Trigger2(); ///< Simple constructor
  bool Initialise(std::string configfile,DataModel &data); ///< Initialise Function for setting up Tool resorces. @param configfile The path and name of the dynamic configuration file to read in. @param data A reference to the transient data class used to pass information between Tools.
  bool Execute(); ///< Executre function used to perform Tool perpose. 
  bool Finalise(); ///< Finalise funciton used to clean up resorces.


 private:

  void LoadConfig();  
  static void Thread(Thread_args* arg); ///< Function to be run by the thread in a loop. Make sure not to block in it
  static bool TriggerJob(void* data);
  static bool MainTrigger(void* data); 
  static bool LedTrigger(void* data);
  // static bool NoneTrigger(void* data);
  Utilities* m_util;  ///< Pointer to utilities class to help with threading
  Trigger2_args* args; ///< thread args (also holds pointer to the thread)
  Store main_vars;
  Store led_vars;
  //  Store none_vars;
  std::string m_configfile;
  
};


#endif
