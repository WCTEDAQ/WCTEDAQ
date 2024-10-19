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

   uint32_t GetFineTime() {
      return ((data[1] & 0b00000011) << 26) | 
         (data[2] << 18) | 
         ((data[3] & 0b11110000) << 14) | 
         ((data[4] & 0b00111111) << 8) |
         data[5];
   }

   uint8_t GetSubHitNum() {
      return (data[3] & 0b00001111);
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

   bool Dump() {
      printf("header: ");
      uint16_t *bufusint;
      bufusint = reinterpret_cast<uint16_t *>(data);
      for(int i=0; i<sizeof(data)/2; i++)
         printf("0x%X ", bufusint[i]);
      printf("\n");
      return true;
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

   bool Dump() {
      printf("subhit: ");
      uint16_t *bufusint;
      bufusint = reinterpret_cast<uint16_t *>(data);
      for(int i=0; i<sizeof(data)/2; i++)
         printf("0x%X ", bufusint[i]);
      printf("\n");
      return true;
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


   uint16_t GetUnixTime() {
      return ((data[0] & 0b00111111) << 8) | data[1];
   }

   uint16_t GetCharge() {
      return ((data[2] << 4) | ((data[3] & 0b11110000) >> 4));
   }

   uint8_t GetCRC() { 
      return (data[3] & 0b00001111);
   }

   bool Dump() {
      printf("footer: ");
      uint16_t *bufusint;
      bufusint = reinterpret_cast<uint16_t *>(data);
      for(int i=0; i<sizeof(data)/2; i++)
         printf("0x%X ", bufusint[i]);
      printf("\n");
      return true;
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
      printf(" header = %d\n", unsigned(header.GetHeader()));
      printf(" event_type = %d\n", unsigned(header.GetEventType()));
      printf(" channel = %d\n", unsigned(header.GetChannel()));
      printf(" channel state = %d\n", unsigned(header.GetChannelState()));
      printf(" number of subhits = %d\n", unsigned(header.GetSubHitNum()));
      printf(" fine_time = %ld\n", header.GetFineTime());
      printf(" tdc_start_time = %d\n", unsigned(header.GetTDCStartTime()));
      printf(" tdc_coarse_time = %d\n", unsigned(header.GetTDCCoarseTime()));
      printf(" tdc_stop_time = %d\n", unsigned(header.GetTDCStopTime()));

      for(int i=0; i<header.GetSubHitNum(); i++) {
         printf("- subhit #%d\n", i);
         printf("\tfine_time_delta = %d\n", unsigned(sub_hits[i].GetFineTimeDelta()));
         printf("\ttdc_start_time = %d\n", unsigned(sub_hits[i].GetTDCStartTime()));
         printf("\ttdc_coarse_time = %d\n", unsigned(sub_hits[i].GetTDCCoarseTime()));
         printf("\ttdc_stop_time = %d\n", unsigned(sub_hits[i].GetTDCStopTime()));
      }

      printf(" crc = %d\n", unsigned(footer.GetCRC()));
      printf(" unix_time = %d\n", footer.GetUnixTime());
      printf(" charge = %d\n", footer.GetCharge());
      return true;
   }

   bool Dump() {
      for(int i=0; i<header.GetSubHitNum(); i++) {
         header.Dump();
         sub_hits[i].Dump();
         footer.Dump();
      }
      return true;
   }

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
