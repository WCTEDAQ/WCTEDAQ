#ifndef READOUT_WINDOW_H
#define READOUT_WINDOW_H

#include <vector>
#include <string>
#include <SerialisableObject.h>
#include <BinaryStream.h>

#include <WCTEMPMTHit.h>
#include <WCTEMPMTWaveform.h>
#include <WCTEMPMTLED.h>
#include <HKMPMTHit.h>
#include <TriggerType.h>
#include <TDCHit.h>
#include <QDCHit.h>

class ReadoutWindow : SerialisableObject{

public:
  
  std::vector<TriggerInfo> triggers_info;
  std::vector<WCTEMPMTHit> mpmt_hits;  
  std::vector<WCTEMPMTWaveform> mpmt_waveforms;
  std::vector<HKMPMTHit> hk_mpmt_hits;
   std::vector<WCTEMPMTHit> extra_hits;
  std::vector<WCTEMPMTWaveform> extra_waveforms;
  std::vector<TDCHit> tdc_hits;
  std::vector<QDCHit> qdc_hits;
  unsigned long start_counter;
  unsigned long readout_num;

  bool Print(){

    std::cout<<"/////////////////////////trigger info//////////////////"<<std::endl;
    for(int i=0; i<triggers_info.size(); i++){ std::cout<<"///["<<i<<"]///"<<std::endl; triggers_info.at(i).Print();}
    std::cout<<"/////////////////////////mpmt hits//////////////////"<<std::endl;
    for(int i=0; i<mpmt_hits.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; mpmt_hits.at(i).Print();}
    std::cout<<"/////////////////////////mpmt waveforms//////////////////"<<std::endl;
    for(int i=0; i<mpmt_waveforms.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; mpmt_waveforms.at(i).Print();}
    std::cout<<"/////////////////////////hk mpmt hits//////////////////"<<std::endl;
    for(int i=0; i<hk_mpmt_hits.size(); i++){ std::cout<<"///["<<i<<"]///"<<std::endl; hk_mpmt_hits.at(i).Print();}
    std::cout<<"/////////////////////////extra hits//////////////////"<<std::endl;
    for(int i=0; i<extra_hits.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; extra_hits.at(i).Print();}
    std::cout<<"/////////////////////////extra waveforms//////////////////"<<std::endl;
    for(int i=0; i<extra_waveforms.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; extra_waveforms.at(i).Print();}
    std::cout<<"/////////////////////////tdc data//////////////////"<<std::endl;
    for(int i=0; i<tdc_hits.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; tdc_hits.at(i).Print();}
    std::cout<<"/////////////////////////qdc data//////////////////"<<std::endl;
    for(int i=0; i<qdc_hits.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; qdc_hits.at(i).Print();}   
    std::cout<<"///////////////////////////start counter ="<<start_counter<<"////////////////"<<std::endl;
   std::cout<<"///////////////////////////readout numebr ="<<readout_num<<"////////////////"<<std::endl;
   std::cout<<"///////////////////////////////////////////"<<std::endl<<std::endl;
    return true;

  }
  std::string GetVersion(){return "1.0";};
  bool Serialise(BinaryStream &bs){
    
    bs & triggers_info;
    bs & mpmt_hits;
    bs & mpmt_waveforms;
    bs & hk_mpmt_hits;
    bs & extra_hits;
    bs & extra_waveforms;
    bs & tdc_hits;
    bs & qdc_hits;

    return true;
  } 
  
};


#endif
