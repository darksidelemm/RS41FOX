#include "util.h"

uint16_t string_CRC16_checksum(char *string) {
  uint16_t crc = 0xffff;
  char i;
  while (*(string) != 0) {
    crc = crc ^ (*(string++) << 8);
    for (i = 0; i < 8; i++) {
      if (crc & 0x8000)
        crc = (uint16_t) ((crc << 1) ^ 0x1021);
      else
        crc <<= 1;
    }
  }
  return crc;
}

uint16_t array_CRC16_checksum(char *string, int len) {
  uint16_t crc = 0xffff;
  char i;
  int ptr = 0;
  while (ptr < len) {
    ptr++;
    crc = crc ^ (*(string++) << 8);
    for (i = 0; i < 8; i++) {
      if (crc & 0x8000)
        crc = (uint16_t) ((crc << 1) ^ 0x1021);
      else
        crc <<= 1;
    }
  }
  return crc;
}

void print_hex(char *data, uint8_t length, char *tmp) // prints 8-bit data in hex
{
 uint8_t first ;
 int j=0;
 for (uint8_t i=0; i<length; i++) 
 {
   first = ((uint8_t)data[i] >> 4) | 48;
   if (first > 57) tmp[j] = first + (uint8_t)39;
   else tmp[j] = first ;
   j++;

   first = ((uint8_t)data[i] & 0x0F) | 48;
   if (first > 57) tmp[j] = first + (uint8_t)39; 
   else tmp[j] = first;
   j++;
 }
 tmp[length*2] = '\n';
 tmp[length*2+1] = 0;
 // Serial.println(tmp);
}