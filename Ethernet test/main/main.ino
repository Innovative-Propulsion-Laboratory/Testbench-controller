#include "Valves.h"
#include "UDP.h"
#include "Sensors.h"

bool sendData = true;

unsigned long t_last_data_packet = 0, data_send_rate = 1000;

void setup() {
  Serial.begin(9600);       //initialize Serial Port
  SPI.begin();              //initialize SPI
  setupSensors();
  setupValves();
  setupUDP();
  Set_valve_position();
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

  pinMode(2, OUTPUT);
  pinMode(28, OUTPUT);
  pinMode(29, OUTPUT);
  pinMode(30, OUTPUT);
  pinMode(35, OUTPUT);
  pinMode(36, OUTPUT);
  pinMode(37, OUTPUT);

  digitalWrite(2, HIGH);
  digitalWrite(28, HIGH);
  digitalWrite(29, HIGH);
  digitalWrite(30, HIGH);
  digitalWrite(35, HIGH);
  digitalWrite(36, HIGH);
  digitalWrite(37, HIGH);

  setValve(SV21, 0);
  setValve(SV22, 0);
  setValve(SV24, 0);
  setValve(SV31, 1);
  setValve(SV32, 0);
  setValve(SV33, 0);
  setValve(SV34, 1);
}

void loop() {
  // listen to commands
  Packet p = receivePacket();

  if (p.length >= 4 && p.data != nullptr) {
  decode(p.data);
  }
  if (p.data != nullptr) {
    delete[] p.data;
  }
  if (sendData == true){
    sensorsLoop();
    // serialSend();
  }
  // BBLoop();
}

uint16_t assembleUInt16(uint8_t lowByte, uint8_t highByte) { // to assemble 2 byte
  return (static_cast<uint16_t>(highByte) << 8) | static_cast<uint16_t>(lowByte);
}

void decode(byte* instructions){

  if (instructions[0] == 0xff  && instructions[1] == 0xff && instructions[2] == 0xff && instructions[3] == 0xff){ // Valve
    if (instructions[5] == 0x00 || instructions[5] == 0x01) {
      setValve(instructions[4], instructions[5]);  // Activer ou dÃ©sactiver la valve
    }
  }
  if (instructions[0] == 0xFF  && instructions[1] == 0xFF && instructions[2] == 0xEE && instructions[3] == 0xEE){ // set bang-bang pressurization 
    if (instructions[4] == 1 ){// tank: 0 = LOX, 1 = ETH, 2 = WATER
      uint16_t value = assembleUInt16(instructions[5],instructions[6]);
      Serial.print("ETH pressure bangbang set : ");
      Serial.println(value);
      // BB_param_set(0, float pressure); 
    }
    if (instructions[4] == 2 ){
      uint16_t value = assembleUInt16(instructions[5],instructions[6]);
      Serial.println("LOX pressure bangbang set : ");
      Serial.println(value);
      // BB_param_set(1, value);
    }
    if (instructions[4] == 6 ){
      uint16_t value = assembleUInt16(instructions[5],instructions[6]);
      Serial.println("H20 pressure bangbang set : ");
      Serial.println(value);
      // BB_param_set(2, value); 
    }
  }
  if (instructions[0] == 0xFF  && instructions[1] == 0xFF && instructions[2] == 0xDD && instructions[3] == 0xDD){ // bang-bang enable
    if (instructions[4] == 1 ){// tank: 0 = LOX, 1 = ETH, 2 = WATER
      if (instructions[5]==0X00){
        Serial.println("ETH bangbang activate");
        // BB_enable (0, 0);
      }
      else if (instructions[5]==0X01){
        Serial.println("ETH bangbang desactivate");
        // BB_enable (0, 1);
      }
    }
    else if (instructions[4] == 2 ){
      if (instructions[5]==0X00){
        Serial.println("ETH bangbang activate");
        // BB_enable (1, 0);
      }
      if (instructions[5]==0X01){
        Serial.println("ETH bangbang desactivate");
        // BB_enable (1, 1);
      }
    }
    else if (instructions[4] == 6 ){
      if (instructions[5]==0X00){
        Serial.println("ETH bangbang activate");
        // BB_enable (2, 0);
      }
      if (instructions[5]==0X01){
        Serial.println("ETH bangbang desactivate");
        // BB_enable (2, 1);
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
  if (instructions[0] == 0xAA  && instructions[1] == 0xAA && instructions[2] == 0xAA && instructions[3] == 0xAA){ // Start test

  } 
  if (instructions[0] == 0xBB  && instructions[1] == 0xBB && instructions[2] == 0xBB && instructions[3] == 0xBB){ // Confirm test

  } 
  if (instructions[0] == 0xCC  && instructions[1] == 0xCC && instructions[2] == 0xCC && instructions[3] == 0xCC){ // Abort test

  } 
}

void test(){

}
