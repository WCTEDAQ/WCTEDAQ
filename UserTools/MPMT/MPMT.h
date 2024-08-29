#ifndef MPMT_H
#define MPMT_H

#include <string>
#include <iostream>

#include "Tool.h"
#include "DataModel.h"


struct MPMTMessages{

  MPMTMessages(){;}
  ~MPMTMessages(){
    delete daq_header;
    daq_header=0;
    delete mpmt_data;
    mpmt_data=0;
    m_data=0;
  }
  zmq::message_t* daq_header;
  zmq::message_t* mpmt_data;
  DataModel* m_data;

};

/**
 * \struct MPMT_args
 *
 * This is a struct to place data you want your thread to acess or exchange with it. The idea is the datainside is only used by the threa\
d and so will be thread safe
*
* $Author: B.Richards $
* $Date: 2019/05/28 10:44:00 $
*/


struct MPMT_args:Thread_args{

  MPMT_args();
  ~MPMT_args();
  zmq::socket_t* data_sock;
  DAQUtilities* utils;
  std::map<std::string,Store*> connections;
  zmq::pollitem_t items[1];
  boost::posix_time::time_duration period;
  boost::posix_time::ptime last;
  std::string data_port;
  unsigned int message_size;
  bool no_data;
  boost::posix_time::time_duration lapse;
  JobQueue* job_queue;
  DataModel* m_data;

};

/**
 * \class MPMT
 *
 * This is a template for a Tool that dynamically more or less threads, such that there is always 1 available thread.This can therefore be used to scale to your worklaod, however be carefull when using more than one of these tools and to apply upperlimits if necessary both locally within this tool and globally so that more threads than is practical are created causing massive inefficency. Please fill out the descripton and author information.
 *
 * $Author: B.Richards $
 * $Date: 2019/05/28 10:44:00 $
 */

class MPMT: public Tool {


 public:

  MPMT(); ///< Simple constructor
  bool Initialise(std::string configfile,DataModel &data); ///< Initialise Function for setting up Tool resorces. @param configfile The path and name of the dynamic configuration file to read in. @param data A reference to the transient data class used to pass information between Tools.
  bool Execute(); ///< Executre function used to perform Tool perpose. 
  bool Finalise(); ///< Finalise funciton used to clean up resorces.


 private:

  void CreateThread(); ///< Function to Create Thread
  void DeleteThread(unsigned int pos); ///< Function to delete thread @param pos is the position in the args vector below

  static void Thread(Thread_args* arg); ///< Function to be run by the thread in a loop. Make sure not to block in it
  Utilities* m_util; ///< Pointer to utilities class to help with threading
  std::vector<MPMT_args*> args; ///< Vector of thread args (also holds pointers to the threads)

  unsigned int m_freethreads; ///< Keeps track of free threads
  unsigned long m_threadnum; ///< Counter for unique naming of threads

  static bool ProcessData(void* data);
  //static WCTEMPMTHit ProcessMPMTHit(char* data, unsinged long& start);
  //static WCTEMPMTWaveform ProcessMPMTWaveForm(void* data);
  //static WCTEMPMTLED ProcessMPMTLED(void* data);
  //static WCTEMPMTLED ProcessMPMTLED(void* data);
  
};


#endif