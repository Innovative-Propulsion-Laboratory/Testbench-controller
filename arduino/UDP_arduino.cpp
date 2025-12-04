#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

//test Claire

// Configuration réseau
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
unsigned int localPort = 5190;  // Port d'écoute UDP
EthernetUDP Udp;

// Buffer de réception
char packetBuffer[255];
IPAddress senderIP;
uint16_t senderPort;

uint32_t kDHCPTimeout = 15000;

void setup() {
  Serial.begin(9600);
  setupUDP();
}

void setupUDP() {
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Échec DHCP, vérifiez le câble.");
    while (true);
  }

  Serial.print("IP assignée : ");
  Serial.println(Ethernet.localIP());

  Udp.begin(localPort);
}

void reply(byte* message, uint16_t size) {
  Udp.beginPacket(senderIP, senderPort);
  Udp.write(message, size);
  Udp.endPacket();
}

void send_data(void* Data, uint16_t size) {
  Udp.beginPacket(senderIP, senderPort);
  Udp.write((uint8_t*)Data, size);
  Udp.endPacket();
}

void reply(int tracker, byte* index, int size) {
  byte message[size + 4];

  byte header = 0;
  switch (tracker) {
    case 1: header = 0xFF; break;
    case 2: header = 0xDD; break;
    case 3:
      Serial.println("Test confirmé");
      return;
    case 4: header = 0xCC; break;
    case 5: header = 0xBB; break;
    default: return;
  }

  for (int i = 0; i < 4; i++) message[i] = header;
  for (int j = 0; j < size; j++) message[j + 4] = index[j];

  Udp.beginPacket(senderIP, senderPort);
  Udp.write(message, size + 4);
  Udp.endPacket();
}

bool receivePacket() {
  int packetSize = Udp.parsePacket();
  if (packetSize == 0) return false;

  Udp.read(packetBuffer, packetSize);
  senderIP = Udp.remoteIP();
  senderPort = Udp.remotePort();

  Serial.print("[");
  Serial.print(senderIP);
  Serial.print("][");
  Serial.print(packetSize);
  Serial.print("] ");

  for (int i = 0; i < packetSize; i++) {
    Serial.printf("%#04x ", (uint8_t)packetBuffer[i]);
  }
  Serial.println();

  return true;
}

uint16_t assembleUInt16(uint8_t lowByte, uint8_t highByte) {
  return ((uint16_t)highByte << 8) | lowByte;
}

void decode(byte* instrcutions){

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

void loop() {
    // listen to commands
    decode(receivePacket());
}