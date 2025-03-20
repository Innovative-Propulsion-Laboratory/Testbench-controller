#include "Valves.h"
#include "Pressurization.h"
#include "Sensors.h"
#include "UDP.h"

unsigned long t_last_data_packet = 0, data_send_rate = 1000;


void setup() {
    Serial.begin(9600);       //initialize Serial Port
    SPI.begin();              //initialize SPI

    setupSensors();
    setupValves();
    setupUDP();
    setupSaveData()
}

void loop() {
    // listen to commands
    decode(receivePacket());

    if ((millis() - t_last_data_packet) >= data_send_rate){
        data();
    }
    BB_pressurization();
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

void test(){

}
