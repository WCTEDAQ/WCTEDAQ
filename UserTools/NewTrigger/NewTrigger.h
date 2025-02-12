#ifndef NewTrigger_H
#define NewTrigger_H

#include <string>
#include <iostream>

#include "Tool.h"
#include "DataModel.h"

/**
 * \struct NewTrigger_args_args
 *
 * This is a struct to place data you want your thread to access or exchange with it. The idea is the datainside is only used by the threa\d and so will be thread safe
 *
 * $Author: B.Richards $
 * $Date: 2019/05/28 10:44:00 $
 */

struct NewTrigger_args:Thread_args{

  NewTrigger_args();
  ~NewTrigger_args();
  DataModel* m_data;
  unsigned int bin;  
  MPMTCollection* mpmt_collection;
  unsigned short window_size;
  unsigned short threashold;
};

/**
 * \class NewTrigger
 *
 * This is a template for a Tool that produces a single thread that can be assigned a function seperate to the main thread. Please fill out the descripton and author information.
*
* $Author: B.Richards $
* $Date: 2019/05/28 10:44:00 $
*/

class NewTrigger: public Tool {


 public:

  NewTrigger(); ///< Simple constructor
  bool Initialise(std::string configfile,DataModel &data); ///< Initialise Function for setting up Tool resorces. @param configfile The path and name of the dynamic configuration file to read in. @param data A reference to the transient data class used to pass information between Tools.
  bool Execute(); ///< Executre function used to perform Tool perpose. 
  bool Finalise(); ///< Finalise funciton used to clean up resorces.


 private:

  void LoadConfig();  
  static void Thread(Thread_args* arg); ///< Function to be run by the thread in a loop. Make sure not to block in it
  static bool NhitsJob(void* data);
  Utilities* m_util;  ///< Pointer to utilities class to help with threading
  NewTrigger_args* args; ///< thread args (also holds pointer to the thread)
  Store main_vars;
  Store led_vars;
  //  Store none_vars;
  std::string m_configfile;
  
};


#endif
