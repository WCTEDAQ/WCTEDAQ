#ifndef WCTE_RAW_DATA_H
#define WCTE_RAW_DATA_H

#include <vector>
#include <ReadoutWindow.h>
#include <DAQInfo.h>
#include <string>

#include <SerialisableObject.h>
#include <BinaryStream.h>


class WCTERawData : public SerialisableObject{

public:
  
  DAQInfo daq_info;
  std::vector<PReadoutWindow> readout_windows;
  std::vector<WCTEMPMTPPS> pps;

  bool Print(){
    daq_info.Print();
    for(unsigned int i=0; i<readout_windows.size(); i++){
      std::cout<<"///////////////////////////////// "<<std::endl;
      std::cout<<"ReadoutWindow "<<i<<"/"<<readout_windows.size()<<std::endl;
      std::cout<<"///////////////////////////////// "<<std::endl;
      readout_windows.at(i).Print();
      std::cout<<"///////////////////////////////// "<<std::endl<<std::endl;
    }
    
    return true;};
  std::string GetVersion(){return "1.0";}

  bool Serialise(BinaryStream &bs){ 
    
    bs & daq_info;
    std::cout<<"readout windows size="<<readout_windows.size()<<std::endl;
    
    bs & readout_windows;

    bs & pps;
    
    return true;  
  }
  
};


#endif
