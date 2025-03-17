#ifndef UDP_H
#define UDP_H

#include <stdint.h>  
#include <QNEthernet.h>

using namespace qindesign::network;
uint32_t kDHCPTimeout;  
uint16_t kPort;
uint16_t senderPort;
IPAddress senderIP;
EthernetUDP udp;
void setupUDP();
void receivePacket();
void reply(int tracker, byte* index, int size);

#endif  