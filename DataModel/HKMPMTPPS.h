#ifndef HK_MPMT_PPS_H
#define HK_MPMT_PPS_H

#include <vector>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <bitset>
#include <string>

class HKMPMTPPS {

public:

   HKMPMTPPS() { 
      std::fill(std::begin(data), std::end(data), 0);
   }

   HKMPMTPPS(unsigned short& in_card_id, char* in_data) { 
      card_id=in_card_id; 
      memcpy(&data[0], in_data, sizeof(data));
   }

   static unsigned int GetSize() { 
      return sizeof(data);
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

   uint32_t GetUnixTime() {
      return ((data[0] & 0b00000011) << 30) | 
         (data[1] << 22) | 
         (data[2] << 14) | 
         (data[3] << 6) |
         (data[4] & 0b00111111);
   }

   uint32_t GetRateMeter() {
      return (data[5] << 16) |
         (data[6] << 8) |
         data[7];
   }

   uint8_t GetState () {
      return (data[8] & 0b00111111);
   }

   uint16_t GetHumidity() {
      return (data[9] << 4) | ((data[10] & 0b11110000) >> 4);
   }

   uint16_t GetTemperature() {
      return ((data[10] & 0b00001111) << 8) | data[11];
   }

   uint8_t GetCRC() { 
      return (data[12] & 0b00111100) >> 2;
   }

   uint16_t GetDeadTime() {
      return (data[14] << 8) | data[15];
   }
   
   void SetCardID(uint16_t in) {
      card_id = in;
   }

   bool Print() {

      printf(" header = %d\n", unsigned(GetHeader()));
      printf(" event_type = %d\n", unsigned(GetEventType()));
      printf(" unix_time = %ld\n", GetUnixTime());
      printf(" rate_meter = %ld\n", GetRateMeter());
      printf(" state = %d\n", unsigned(GetState()));
      printf(" humidity = %d\n", GetHumidity());
      printf(" temperature = %d\n", GetTemperature());
      printf(" crc = %d\n", unsigned(GetCRC()));
      printf(" dead_time = %d\n", GetDeadTime());

      return true;
   }

   std::string GetVersion() {
      return "1.0";
   }

private:

   uint16_t card_id;
   uint8_t data[16];

};

#endif
