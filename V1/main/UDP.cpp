#include "UDP.h"

uint32_t kDHCPTimeout = 15'000;  //waiting time 15 seconds
uint16_t kPort = 5190;  // Chat port

int t;

void setupUDP() {
  // Serial.begin(11500);
  /////////////////////////////////////////////////////
  // Initilisation and setup //
  Ethernet.begin();
  Ethernet.onLinkState([](bool state) {
    printf("[Ethernet] Link %s\r\n", state ? "ON" : "OFF");
  });
  if (!Ethernet.waitForLocalIP(kDHCPTimeout)) {
    printf("Failed to get IP address\r\n");
    return;
  }
  IPAddress ip = Ethernet.localIP();
  printf("    Local IP     = %u.%u.%u.%u\r\n", ip[0], ip[1], ip[2], ip[3]);
  udp.begin(kPort);
  /////////////////////////////////////////////////////

}

void reply(int tracker, byte* index, int size) {
    if (tracker == 0){ // valve
      byte message[size+4];
      message[0] = 0xEE;
      message[1] = 0xEE;
      message[2] = 0xEE;
      message[3] = 0xEE;
      for (int j = 0; j < size; j++) {
      message[j+4] = index[j]; } 
      udp.send(senderIP, senderPort, message, sizeof(message));}
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

void receivePacket() {
  int size = udp.parsePacket();
  if (size < 0) { 
    return;
  }

  const uint8_t *data = udp.data();
  senderIP = udp.remoteIP();
  senderPort = udp.remotePort();
  printf("[%u.%u.%u.%u][%d] ", senderIP[0], senderIP[1], senderIP[2], senderIP[3], size);

  byte instructions[size];  
  for (int j = 0; j < size; j++) { // copy data
    instructions[j] = data[j]; 
    Serial.printf("%#04x ",instructions[j]); 
  }

  if (instructions[0] == 0xFF  && instructions[1] == 0xFF && instructions[2] == 0xFF && instructions[3] == 0xFF){ // Valve
    if (instructions[6] == 0x00 || instructions[6] == 0x01) {
      setValve(instruction[5], instruction[6]);  // Activer ou dÃ©sactiver la valve
    }
  }
  if (instructions[0] == 0xFF  && instructions[1] == 0xFF && instructions[2] == 0xEE && instructions[3] == 0xEE){ // set bang-bang pressurization 
    if (instructions[4] == 1 ){
      BB_param_set(0, float pressure); // tank: 0 = LOX, 1 = ETH, 2 = WATER
    }
    if (instructions[4] == 2 ){
      uint16_t value = assembleUInt16(instructions[5],instructions[6]);
      BB_param_set(1, value);
    }
    if (instructions[4] == 6 ){
      uint16_t value = assembleUInt16(instructions[5],instructions[6]);
      BB_param_set(2, value); 
    }
  }
  if (instructions[0] == 0xFF  && instructions[1] == 0xFF && instructions[2] == 0xDD && instructions[3] == 0xDD){ // bang-bang enable
    if (instructions[4] == 1 ){
      if (instructions[5]==0X00){
        BB_enable (0, 0);
      }
      elif (instructions[5]==0X01){
        BB_enable (0, 1);
      }
    }
    elif (instructions[4] == 2 ){
      if (instructions[5]==0X00){
        BB_enable (1, 0);
      }
      if (instructions[5]==0X01){
        BB_enable (1, 1);
      }
    }
    elif (instructions[4] == 6 ){
      if (instructions[5]==0X00){
        BB_enable (2, 0);
      }
      if (instructions[5]==0X01){
        BB_enable (2, 1);
      }
    }
  }
  if (instructions[0] == 0xEE  && instructions[1] == 0xEE && instructions[2] == 0xEE && instructions[3] == 0xEE){ // Actuators
    if (instructions[4] == 0 ){
      // the obcsur TVC name + instruction[5]
    }
    if (instructions[4] == 1 ){
      // the obcsur TVC name + instruction[5]
    }
  }
  if (instructions[0] == 0xEE  && instructions[1] == 0xEE && instructions[2] == 0xDD && instructions[3] == 0xDD){ // TVC pattern
    if (instructions[4] == 1 ){
      // the obcsur TVC name
    }
    if (instructions[4] == 2 ){
      // the obcsur TVC name
    }
    if (instructions[4] == 3 ){
      // the obcsur TVC name
    }
    if (instructions[4] == 4 ){
      // the obcsur TVC name
    }
    if (instructions[4] == 5){
      // the obcsur TVC name
    }
  }
  if (instructions[0] == 0xDD  && instructions[1] == 0xDD && instructions[2] == 0xDD && instructions[3] == 0xDD){ // IHM on/off

  }
  if (instructions[0] == 0xAA  && instructions[1] == 0xAA && instructions[2] == 0xAA && instructions[3] == 0xAA){ // Start test

  } 
  if (instructions[0] == 0xBB  && instructions[1] == 0xBB && instructions[2] == 0xBB && instructions[3] == 0xBB){ // Confirm test

  } 
  if (instructions[0] == 0xCC  && instructions[1] == 0xCC && instructions[2] == 0xCC && instructions[3] == 0xCC){ // Abort test

  } 
  
}

uint16_t assembleUInt16(uint8_t lowByte, uint8_t highByte) {
    return (static_cast<uint16_t>(highByte) << 8) | static_cast<uint16_t>(lowByte);
}