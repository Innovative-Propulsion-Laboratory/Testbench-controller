#ifndef DECODER_H
#define DECODER_H

#include "Sensors.h"


extern bool test_will_begin;

uint16_t assembleUInt16(uint8_t lowByte, uint8_t highByte);
int16_t assembleInt16(uint8_t lowByte, uint8_t highByte);
void decode(byte* instructions);

#endif