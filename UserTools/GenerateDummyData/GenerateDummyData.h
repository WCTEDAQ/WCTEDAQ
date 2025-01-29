#ifndef GenerateDummyData_H
#define GenerateDummyData_H

#include <string>
#include <iostream>
#include <chrono>

#include "Tool.h"
#include "DataModel.h"


/**
 * \class GenerateDummyData
 *
 * This is a balnk template for a Tool used by the script to generate a new custom tool. Please fill out the descripton and author information.
*
* $Author: B.Richards $
* $Date: 2019/05/28 10:44:00 $
*/

class GenerateDummyData: public Tool {
	enum class EventType{ Normal=0, Pedestal=1, LED=2, Cal=3, PPS=0x0F}; 
	
	public:
	
	GenerateDummyData(); ///< Simple constructor
	bool Initialise(std::string configfile,DataModel &data); ///< Initialise Function for setting up Tool resorces. @param configfile The path and name of the dynamic configuration file to read in. @param data A reference to the transient data class used to pass information between Tools.
	bool Execute(); ///< Executre function used to perform Tool perpose. 
	bool Finalise(); ///< Finalise funciton used to clean up resorces.
	
	WCTEMPMTHit MakeHit(unsigned int card_id, unsigned int channel_id, EventType type, unsigned int coarse_counter, bool is_waveform);
	bool SendNextMessage();
	
	private:
	zmq::socket_t* out_sock;
	int out_port;
	zmq::pollitem_t in_poll, out_poll;
	int poll_ms;
	std::chrono::time_point<std::chrono::high_resolution_clock> last_send; // ::system_clock
	int num_mpmts=0;
	int mpmt_num=0;
	//std::chrono::duration<std::chrono::milliseconds> send_period_ms;
	int send_period_ms;
	unsigned int coarse_counter=0; // for daq_header, represents upper 32 bits
	unsigned int msg_num=0;
	
	
};


#endif
