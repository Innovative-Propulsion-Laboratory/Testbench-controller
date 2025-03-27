#include "UDP.h"

uint32_t kDHCPTimeout = 15000;  //waiting time 15 seconds
uint16_t kPort = 5190;  // Chat port

int t;

void setupUDP() {
  Ethernet.begin();
  Ethernet.onLinkState([](bool state){});  // No debug output
  if (!Ethernet.waitForLocalIP(kDHCPTimeout)) {
      return;  // Exit if no IP address is assigned
  }

  udp.begin(kPort);
}

void reply (byte* message, uint16_t size){
  udp.send(senderIP, senderPort, message, size);
}

void send_data (data* Data, uint16_t size){
  udp.send(senderIP, senderPort, (const uint8_t*)Data, size)
}

void reply(int tracker, byte* index, int size) {
    else if(tracker == 1) { // data
      byte message[size+4]; 
      message[0] = 0xFF;
      message[1] = 0xFF;
      message[2] = 0xFF;
      message[3] = 0xFF;
      for (int j = 0; j < size; j++) {message[j+4] = index[j]; }  // copy data 
      udp.send(senderIP, senderPort, message, sizeof(message));}
    else if (tracker == 2) { // Start test
      byte message[size+4]; 
      message[0] = 0xDD;
      message[1] = 0xDD;
      message[2] = 0xDD;
      message[3] = 0xDD;
      for (int j = 0; j < size; j++) { // copy data
      message[j+4] = index[j]; } 
      udp.send(senderIP, senderPort, message, sizeof(message));}
    else if (tracker == 3) { // Confirm test
        Serial.println("Test confirmed");} 
    else if (tracker == 4) { // Abort test
      byte message[size+4]; 
      message[0] = 0xCC;
      message[1] = 0xCC;
      message[2] = 0xCC;
      message[3] = 0xCC;
      for (int j = 0; j < size; j++) { // copy data
      message[j+4] = index[j]; } 
      udp.send(senderIP, senderPort, message, sizeof(message));}
    else if (tracker == 5) { // Abort test
      byte message[size+4]; 
      message[0] = 0xBB;
      message[1] = 0xBB;
      message[2] = 0xBB;
      message[3] = 0xBB;
      for (int j = 0; j < size; j++) { // copy data
      message[j+4] = index[j]; } 
      udp.send(senderIP, senderPort, message, sizeof(message));}
}

byte receivePacket() {
  int size = udp.parsePacket();
  if (size < 0) { 
    return;
  }

  const uint8_t *data = udp.data();
  senderIP = udp.remoteIP(); // enlever et les définir une seule fois  fct first message 
  senderPort = udp.remotePort();
  printf("[%u.%u.%u.%u][%d] ", senderIP[0], senderIP[1], senderIP[2], senderIP[3], size);

  byte instructions[size];  
  for (int j = 0; j < size; j++) { // copy data
    instructions[j] = data[j]; 
    Serial.printf("%#04x ",instructions[j]); 
  }

  return instructions;
}

uint16_t assembleUInt16(uint8_t lowByte, uint8_t highByte) {
    return (static_cast<uint16_t>(highByte) << 8) | static_cast<uint16_t>(lowByte);
}