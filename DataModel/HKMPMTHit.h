#ifndef HK_MPMT_HIT_H
#define HK_MPMT_HIT_H

#include <vector>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <bitset>
#include <string>

#include <SerialisableObject.h>
#include <BinaryStream.h>

class HKMPMTHeader {

public:

   HKMPMTHeader() { 
      std::fill(std::begin(data), std::end(data), 0);
   }

   static unsigned int GetSize() {
      return sizeof(data);
   }

   uint8_t* GetData() {
      return data;
   }

   uint8_t GetCardID() {
      return card_id;
   }

   uint8_t GetHeader() {
      return (data[0] & 0b11000000) >> 6;
   }

   uint8_t GetEventType() {
      return (data[0] & 0b00111100) >> 2;
   }

   uint8_t GetChannel() {
      return ((data[0] & 0b00000011) << 3) | ((data[1] & 0b11100000) >> 5);
   }

   uint8_t GetChannelState() {
      return (data[1] & 0b00011100) >> 2;
   }

   uint8_t GetSubHitNum() {
      return ((data[1] & 0b00000011) << 2) | ((data[2] & 0b11000000) >> 6);
   }

   uint32_t GetFineTime() {
      return ((data[2] & 0b00111111) << 22) | 
         (data[3] << 14) | 
         ((data [4] & 0b00111111) << 8) | 
         data[5]; 
   }

   uint8_t GetTDCStartTime() {
      return data[6] >> 3;
   }

   uint8_t GetTDCCoarseTime() {
      return ((data[6] & 0b00000111) << 3) | ((data[7] & 0b11100000) >> 5);
   }
   
   uint8_t GetTDCStopTime() {
      return data[7] & 0b00011111;
   }

   void SetCardID(uint16_t in) {
      card_id = in;
   }

private:

   uint16_t card_id;
   uint8_t data[8];
};


class HKMPMTSubHit {

public:

   HKMPMTSubHit() {
      std::fill(std::begin(data), std::end(data), 0);
   }

   static unsigned int GetSize() {
      return sizeof(data);
   }

   uint8_t* GetData() {
      return data;
   }

   uint8_t GetHeader() {
      return (data[0] & 0b11000000) >> 6;
   }

   uint16_t GetFineTimeDelta() {
      return ((data[0] & 0b00111111) << 8) | data[1];
   }

   uint8_t GetTDCStartTime() {
      return data[2] >> 3;
   }

   uint8_t GetTDCCoarseTime() {
      return ((data[2] & 0b00000111) << 3) | ((data[3] & 0b11100000) >> 5);
   }
   
   uint8_t GetTDCStopTime() {
      return data[3] & 0b00011111;
   }

private:

   uint8_t data[4];
};

class HKMPMTFooter {

public:

   HKMPMTFooter() {
      std::fill(std::begin(data), std::end(data), 0);
   }

   static unsigned int GetSize() {
      return sizeof(data);
   }

   uint8_t* GetData() {
      return data;
   }

   uint8_t GetCRC() { 
      return (data[0] & 0b00111100) >> 2;
   }

   uint16_t GetUnixTime() {
      return ((data[0] & 0b00000011) << 12) | (data[1] << 4) | ((data[2] & 0b11110000) >> 4);
   }

   uint16_t GetCharge() {
      return ((data[2] & 0b00001111) << 8) | data[3];
   }

private:

   uint8_t data[4];

};


class HKMPMTHit : public ToolFramework::SerialisableObject {

public:
   HKMPMTHit() {
      sub_hits.clear();
   }

   HKMPMTHit(unsigned short& in_card_id, char* in_data) { 
      sub_hits.clear();
      header.SetCardID(in_card_id);
      memcpy(header.GetData(), in_data, HKMPMTHeader::GetSize());
      sub_hits.resize(header.GetSubHitNum());
      for(int i=0; i<header.GetSubHitNum(); i++) {
         memcpy(sub_hits[i].GetData(), in_data + HKMPMTHeader::GetSize() + (HKMPMTSubHit::GetSize() * i), 
               HKMPMTSubHit::GetSize());
      }
      memcpy(footer.GetData(), in_data + HKMPMTHeader::GetSize() + (HKMPMTSubHit::GetSize() * header.GetSubHitNum()), 
         HKMPMTFooter::GetSize());
   }


   unsigned int GetSize() {
      return (HKMPMTHeader::GetSize() + (HKMPMTSubHit::GetSize() * header.GetSubHitNum()) + HKMPMTFooter::GetSize()); 
   }
   
   bool Print() {
      std::cout << " header = " << unsigned(header.GetHeader()) << std::endl;
      std::cout << " event_type = " << unsigned(header.GetEventType()) << std::endl;
      std::cout << " channel = " << unsigned(header.GetChannel()) << std::endl;
      std::cout << " channel state = " << unsigned(header.GetChannelState()) << std::endl;
      std::cout << " number of subhits = " << unsigned(header.GetSubHitNum()) << std::endl;
      std::cout << " fine_time = " << header.GetFineTime() << std::endl;
      std::cout << " tdc_start_time = " << unsigned(header.GetTDCStartTime()) << std::endl;
      std::cout << " tdc_coarse_time = " << unsigned(header.GetTDCCoarseTime()) << std::endl;
      std::cout << " tdc_stop_time = " << unsigned(header.GetTDCStopTime()) << std::endl;

      for(int i=0; i<header.GetSubHitNum(); i++) {
         std::cout << "- subhit #" << i << std::endl;
         std::cout << "\tfine_time_delta = " << unsigned(sub_hits[i].GetFineTimeDelta()) << std::endl;
         std::cout << "\ttdc_start_time = " << unsigned(sub_hits[i].GetTDCStartTime()) << std::endl;
         std::cout << "\ttdc_coarse_time = " << unsigned(sub_hits[i].GetTDCCoarseTime()) << std::endl;
         std::cout << "\ttdc_stop_time = " << unsigned(sub_hits[i].GetTDCStopTime()) << std::endl;
      }

      std::cout << " crc = " << unsigned(footer.GetCRC()) << std::endl;
      std::cout << " unix_time = " << footer.GetUnixTime() << std::endl;
      std::cout << " charge = " << footer.GetCharge() << std::endl;
      return true;
   }

   /*
   bool Dump() {
      for(int i=0; i<sizeof(data); i++)
         printf("data[%d]: 0x%X\n", i, data[i]);

      return true;
   }
   */

   std::string GetVersion() {
      return "1.0";
   }

   HKMPMTHeader header;
   std::vector<HKMPMTSubHit> sub_hits;
   HKMPMTFooter footer;

   bool Serialise(ToolFramework::BinaryStream &bs) {
      bs & header;
      bs & sub_hits;
      bs & footer;

      return true;
   }

private:

};


/*
union UHKMPMTHit {

   UHKMPMTHit() {
      bits.reset();
   }

   HKMPMTHit hit;
   std::bitset<96> bits;
};
*/


#endif
