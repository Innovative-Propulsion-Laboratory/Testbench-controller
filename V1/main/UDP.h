#ifndef UDP_H
#define UDP_H

#include <stdint.h>  
#include <QNEthernet.h>
#include "Pressurization.h" 
#include "Sensors.h"

extern using namespace qindesign::network;
extern uint32_t kDHCPTimeout;  
extern uint16_t kPort;
extern uint16_t senderPort;
extern IPAddress senderIP;
extern EthernetUDP udp;
void setupUDP();
void receivePacket();
void reply(int tracker, byte* index, int size);
uint16_t assembleUInt16(uint8_t lowByte, uint8_t highByte);

#endif  