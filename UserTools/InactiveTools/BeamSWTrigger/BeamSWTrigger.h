#ifndef BeamSWTrigger_H
#define BeamSWTrigger_H

#include <string>
#include <iostream>

#include "Tool.h"
#include "DataModel.h"

/**
 * \struct BeamSWTrigger_args_args
 *
 * A struct to encapsulate persistent variables and parameters for the background thread
 *
 * $Author: M. O'Flaherty $
 * $Date: 2025/03/12 $
 */

class BeamSWTrigger;

struct BeamSWTrigger_args:Thread_args{
  
  BeamSWTrigger_args();
  ~BeamSWTrigger_args();
  
  BeamSWTrigger* parent;   // access to parent members (configuration varibles, functions)
  bool running;  // set by parent, read by thread
  
  bool last_running; // was thread active on last Thread call
  bool last_beam_active;
  
};

/**
* \class BeamSWTrigger
*
* Send a software trigger alert each time the beam spill warn flag in the datamodel goes true
*
* $Author: M. O'Flaherty $
* $Date: 2025/03/12 $
*/

class BeamSWTrigger: public Tool {


 public:
 friend class BeamSWTrigger_args;

  BeamSWTrigger();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();

 private:

  static void Thread(Thread_args* arg);
  bool LoadConfig();
  std::string m_configfile;

  unsigned int spill_modulus;
  unsigned int beam_warn_notice;
  unsigned int waveform_presamples;
  long int bens_offset;
  Store alertStore;
  
  bool last_running; // was run started on last Execute call
  
  Utilities* m_util;         ///< Pointer to utilities class to help with threading
  BeamSWTrigger_args* thread_args;     ///< thread args (also holds pointer to the thread)
  
};


#endif
