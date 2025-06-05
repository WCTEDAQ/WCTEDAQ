#include <iostream>
#include <WCTEMPMTWaveform.h>

int main(){

  WCTEMPMTWaveform tmp;

  UWCTEMPMTWaveformHeader* bob = reinterpret_cast<UWCTEMPMTWaveformHeader*>(&tmp.header);
  bob->bits=0b00000000000000000000000000000000000000000000000000010111010001000110000110000010;  
  //bob->bits=0b00000000000000000000000000000000100111001011010011110011100000100000000010000011;

  for(int i=0; i<10 ; i++){
    //    std::cout<<"data["<<i<<"]= "<<((unsigned short)tmp.header.data[i])<<std::endl;
  }

  //00000000|00000000|00000000|00000000|10011100|10110100|11110011|10000010|00000000|10000011
  //10000011|00000000|10000010|11110011
  //10110100|10011100|00000000|00000000
  //00000000|00000000
  
  //10                                               2 head
  //0000                                             0 flags
  //11000000001000001011110011101101                 3,223,370,989 coarse counter
  //00100                                            4   channel
  //1110000000000000                                 57344 samples
  //00000000000000000                                0 length
  //0000                                             0 reserved

    
  tmp.header.Print();
  

  return 0;

}
