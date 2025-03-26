#ifndef UDP_H
#define UDP_H

#include <stdint.h>  
#include <QNEthernet.h>

void setupUDP();
void receivePacket();
void reply(int tracker, byte* index, int size);
uint16_t assembleUInt16(uint8_t lowByte, uint8_t highByte);

#endif