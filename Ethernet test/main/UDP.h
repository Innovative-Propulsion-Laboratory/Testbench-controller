#ifndef UDP_H
#define UDP_H

#include <stdint.h>  
#include <QNEthernet.h>
#include <Arduino.h>
using namespace qindesign::network;

extern EthernetUDP udp;
extern IPAddress senderIP;
extern uint16_t senderPort;

struct Packet {
  byte* data;
  int length;
};

void setupUDP();
Packet receivePacket();
void reply (byte* message, uint16_t size);
// void send_data (data* Data, uint16_t size);
// uint16_t assembleUInt16(uint8_t lowByte, uint8_t highByte);

#endif