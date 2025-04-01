#ifndef UDP_H
#define UDP_H

#include <stdint.h>  
#include <QNEthernet.h>

extern IPAddress senderIP;
extern uint16_t senderPort;

void setupUDP();
void receivePacket();
void reply (byte* message, uint16_t size)
uint16_t assembleUInt16(uint8_t lowByte, uint8_t highByte);

#endif