#ifndef MPMT_MESSAGES_H
#define MPMT_MESSAGES_H

#include <BinaryStream.h>
#include <vector>
#include <zmq.hpp>
#include <mutex>



struct MPMTWaveformHeader{

  
  unsigned short GetHeader(){return ((data[0] & 0b11000000) >> 6 );}
  unsigned short GetFlags(){return ((data[0] & 0b00111100) >> 2 );}
  unsigned int GetCoarseCounter(){ return ((data[0] & 0b00000011) << 30 ) | (data[1] << 22) |  (data[2] << 14) | (data[3] << 6) | ((data[4] & 0b11111100) >> 2); }
  unsigned short GetChannel(){return  ((data[4] & 0b00000011) << 3) | ((data[5] & 0b11100000) >> 5 );}
  unsigned short GetNumSamples(){return (( data[5] & 0b00011111) << 11 ) | ( data[6] << 3) |  (( data[7] & 0b11100000) >> 5);}
  unsigned int GetLength(){ return ((data[7] & 0b00011111) << 12 ) | (data[8] << 4) |  ((data[9] & 0b11110000) >> 4); }
  unsigned short GetReserved(){return (data[9] & 0b00001111);}
  static unsigned int GetSize(){return sizeof(data);};
  unsigned char* GetData(){return data;}  
  
  void SetHeader(unsigned short in){ data[0] = (data[0] & 0b00111111) | ((in & 0b00000011) << 6);}
  void SetFlags(unsigned short in){ data[0] = (data[0] & 0b11000011) | ((in & 0b00001111) << 2);}
  void SetCoarseCounter(unsigned int in){
    data[0] = (data[0] & 0b11111100) | (in >> 30);
    data[1] = in >> 22;
    data[2] = in >> 14;
    data[3] = in >> 6;
    data[4] = (data[4] & 0b00000011) | ((in & 0b00111111) << 2);
  }
  void SetChannel(unsigned short in){
    data[4] = (data[4] & 0b11111100) | ((in & 0b00011000) >> 3);
    data[5] = (data[5] & 0b00011111) | ((in & 0b00000111) << 5);
    
  }
  void SetNumSamples(unsigned short in){
    data[5] = (data[5] & 0b11100000) | ((in >> 11) & 0b00011111 );
    data[6] = in >> 3;
    data[7] = (data[7] & 0b00011111) | ((in & 0b00000111) << 5);
  }
  void SetLength(unsigned int in){
    data[7] = (data[7] & 0b11100000) | ((in >> 12) & 0b00011111 );
    data[8] = in >> 4;
    data[9] = (data[9] & 0b00001111) | ((in & 0b00001111) << 4);
  }
  void SetReserved(unsigned short in){data[9] = (data[9] & 0b11110000) | (in & 0b00001111) ; }
  void Print(){
        printf("header = %u\n",GetHeader());
	printf("flags = %u\n",GetFlags());
	printf("coarse_counter = %u\n",GetCoarseCounter());
	printf("channel = %u\n",GetChannel());
	printf("num_samples = %u\n",GetNumSamples());
	printf("length = %u\n",GetLength());
	printf("reserved = %u\n",GetReserved());
    std::cout<<"header = "<<GetHeader()<<std::endl;
    std::cout<<"flags = "<<GetFlags()<<std::endl;
    std::cout<<"coarse_counter = "<<GetCoarseCounter()<<std::endl;
    std::cout<<"channel = "<<GetChannel()<<std::endl;
    std::cout<<"num_samples = "<<GetNumSamples()<<std::endl;
    std::cout<<"length = "<<GetLength()<<std::endl;
    std::cout<<"reserved = "<<GetReserved()<<std::endl;
    
  }
  
  
private:

  unsigned char data[10];
  
};

struct P_MPMTWaveformHeader {

  MPMTWaveformHeader* waveform_header;
  unsigned short card_id;
  P_MPMTWaveformHeader(MPMTWaveformHeader* in_waveform_header, short in_card_id){
    waveform_header=in_waveform_header;
    card_id=in_card_id;

  }

};



struct MPMTHit{
  
  unsigned short GetHeader(){return (data[0] & 0b11000000) >> 6; }
  unsigned short GetEventType(){return (data[0] & 0b00111100) >> 2;}
  unsigned short GetChannel(){return ((data[0] & 0b00000011) << 3) | ((data[1] & 0b11100000) >> 5); }
  unsigned short GetFlags(){return (data[1] & 0b00011111);}
  unsigned int GetCoarseCounter(){ return (data[2] << 24) | (data[3] << 16) | (data[4] << 8 ) | (data[5]) ;}
  unsigned short GetFineTime(){return (data[6] << 8) | (data[7]);}
  unsigned short GetCharge(){return (data[8] << 8 ) | data[9] ;}
  unsigned short GetQualityFactor(){return data[10];}
  static unsigned int GetSize(){return sizeof(data);};
  unsigned char* GetData(){return data;}

  void SetHeader(unsigned short in){ data[0] = (data[0] & 0b00111111) | ((in & 0b00000011) << 6);}
  void SetEventType(unsigned short in){ data[0] = (data[0] & 0b11000011) | (( in & 0b00001111) << 2) ;}
  void SetChannel(unsigned short in){
    data[0] = (data[0] & 0b11111100) | ((in & 0b00011000) >> 3);
    data[1] = (data[1] & 0b00011111)  | ((in & 0b00000111) << 5);
   }
  void SetFlags(unsigned short in){ data[1] = (data[1] & 0b11100000) | (in & 0b00011111);}
  void SetCoarseCounter(unsigned int in){
    data[2] = in >> 24;
    data[3] = in >> 16;
    data[4] = in >> 8;
    data[5] = in;
  }
  void SetFineTime(unsigned short in){
    data[6] = in >> 8;
    data[7] = in;
  }
  void SetCharge(unsigned short in){
    data[8] = in >> 8;
    data[9] = in;
  }
  void SetQualityFactor(unsigned short in){ data[10] = in;}
  void Print(){
    std::cout<<" header = "<<GetHeader()<<std::endl;
    std::cout<<" event_type = "<<GetEventType()<<std::endl;
    std::cout<<" channel = "<<GetChannel()<<std::endl;
    std::cout<<" flags = "<<GetFlags()<<std::endl;
    std::cout<<" coarse_counter = "<<GetCoarseCounter()<<std::endl;
    std::cout<<" fine_time = "<<GetFineTime()<<std::endl;
    std::cout<<" charge = "<<GetCharge()<<std::endl;
    std::cout<<" quality_factor = "<<GetQualityFactor()<<std::endl;
  }

  unsigned char data[11];
  
};

struct P_MPMTHit {

  MPMTHit* hit;
  unsigned short card_id;
  P_MPMTHit(MPMTHit* in_hit, short in_card_id){
    hit=in_hit;
    card_id=in_card_id;

  }
  
};


struct MPMTLED{

  unsigned short GetHeader(){return (data[0] & 0b11000000) >> 6; }
  unsigned short GetEventType(){return (data[0] & 0b00111100) >> 2;}
  unsigned short GetLED(){return ((data[0] & 0b00000011) << 1) | ((data[1] & 0b10000000) >> 7); }
  bool GetGain(){return ((data[1] & 0b01000000) >> 6);}
  unsigned short GetDACSetting(){return ((data[1] & 0b00111111)  << 4 ) | ((data[2] & 0b11110000) >>4 ) ;}
  unsigned short GetType(){return ((data[2] & 0b00001100)  >> 2 );}
  unsigned short GetSequenceNumber(){return ((data[2] & 0b00000011)  << 12 ) | (data[3] << 4 ) | ((data[4] & 0b11110000 ) >> 4 ) ;}
  unsigned int GetCoarseCounter(){ return ((data[4] & 0b00001111) << 28) | (data[5] << 20) | (data[6] << 12 ) | (data[7] << 4) | ((data[8] & 0b11110000 ) >> 4 ) ;}
  unsigned short GetReserved(){return (data[8] & 0b00001111);}
  static unsigned int GetSize(){return sizeof(data);};
  unsigned char* GetData(){return data;}  


  
  void SetHeader(unsigned short in){ data[0] = (data[0] & 0b00111111) | ((in & 0b00000011) << 6);}
  void SetEventType(unsigned short in){ data[0] = (data[0] & 0b11000011) | (( in & 0b00001111) << 2) ;}
  void SetLED(unsigned short in){
    data[0] = (data[0] & 0b11111100) | ((in & 0b00000110) >> 1);
    data[1] = (data[1] & 0b01111111)  | ((in & 0b00000001) << 7);
   }
  void SetGain(bool in){ data[1] = (data[1] & 0b10111111) | ((in & 0b00000001) << 6) ;}
  void SetDACSetting(unsigned short in){
    data[1] = (data[1] & 0b11000000) | ((in >> 4 ) & 0b00111111) ;
    data[2] = (data[2] & 0b00001111)  | ((in & 0b00001111) << 4 );
  }
  void SetType(unsigned short in){ data[2] = (data[2] & 0b11110011) | ((in & 0b00000011) << 2) ;}
  void SetSequenceNumber(unsigned short in){
    data[2] = (data[2] & 0b11111100) | ((in >> 12 ) & 0b00000011) ;
    data[3] = (in >> 4);
    data[4] = (data[4] & 0b00001111)  | ((in & 0b00001111) << 4 );
  }
  void SetCoarseCounter(unsigned int in){
    data[4] = (data[4] & 0b11110000)  | ((in >> 28 ) & 0b00001111);
    data[5] = in >> 20;
    data[6] = in >> 12;
    data[7] = in >> 4;
    data[8] = (data[8] & 0b00001111)  | ((in & 0b00001111) << 4 );
  }
  void SetReserved(unsigned short in){ data[8] = (data[8] & 0b11110000) | (in & 0b00001111) ;}

  void Print(){
    std::cout<<" header = "<<GetHeader()<<std::endl;
    std::cout<<" event_type = "<<GetEventType()<<std::endl;
    std::cout<<" led = "<<GetLED()<<std::endl;
    std::cout<<" gain = "<<GetGain()<<std::endl;
    std::cout<<" dac_setting = "<<GetDACSetting()<<std::endl;
    std::cout<<" type = "<<GetType()<<std::endl;
    std::cout<<" sequence_number = "<<GetSequenceNumber()<<std::endl;
    std::cout<<" coarse_counter = "<<GetCoarseCounter()<<std::endl;
    std::cout<<" reserved = "<<GetReserved()<<std::endl;
  }


  unsigned char data[9];

};







 
class MPMTMessage : public SerialisableObject{

 public:
  
  zmq::message_t daq_header;
  zmq::message_t mpmt_data;
  std::vector<P_MPMTHit> hits;
  std::vector<P_MPMTWaveformHeader> waveforms;
  
  bool Print(){ return true;}
  std::string GetVersion(){return "1.0";}
  bool Serialise(BinaryStream &bs){

    unsigned int tmp = daq_header.size();
    bs & tmp;
    bs.Bwrite(daq_header.data(), daq_header.size());
    tmp = mpmt_data.size();
    bs & tmp;
    bs.Bwrite(mpmt_data.data(),  mpmt_data.size());

    return true;
  }



};

class MPMTCollection : public SerialisableObject{

 public:
  std::vector<MPMTMessage*> mpmt_output;
  
  std::vector<TriggerInfo*> triggers_info;
  std::vector<MPMTHit*> triggers;
  std::vector<MPMTLED*> leds;
  std::mutex mtx;

  bool Print(){ return true;}
  std::string GetVersion(){return "1.0";}
  bool Serialise(BinaryStream &bs){

    unsigned int tmp = mpmt_output.size();
    bs & tmp;
    for(int i=0; i<mpmt_output.size(); i++){
      bs & (*mpmt_output.at(i));
    }

    return true;
  }
  
};



#endif
