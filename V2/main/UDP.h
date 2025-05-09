#ifndef UDP_H
#define UDP_H

#include <stdint.h>
#include <QNEthernet.h>
#include <Arduino.h>
using namespace qindesign::network;

extern EthernetUDP udp;
extern IPAddress senderIP;
extern uint16_t senderPort;
extern IPAddress ip;
extern bool UDPactive;
extern uint32_t kDHCPTimeout;
extern uint16_t kPort;
extern bool fisrt_message;

struct Packet {
  byte* data;
  int length;
};

void setupUDP();
Packet receivePacket();
void reply(byte* message, uint16_t size);
void convert(const char* message, uint16_t length, int type);
void send_string(const String& msg, int type);
void send_data(void* payload, uint16_t size);
uint16_t assembleUInt16(uint8_t lowByte, uint8_t highByte);

#endif