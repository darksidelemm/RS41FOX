#include <stdint.h>
uint16_t string_CRC16_checksum(char *string);
uint16_t array_CRC16_checksum(char *string, int len);
void print_hex(char *data, uint8_t length, char *tmp);