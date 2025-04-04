#include "Valves.h"
#include "UDP.h"
#include "Sensors.h"
#include "sequence.h"

bool sendData = true;
uint32_t time_last_reading = 0;

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
  if (sendData == true && (millis() - time_last_reading >= 5)){
    sensorsLoop();
    time_last_reading = millis();
    // serialSend();
  }
  BBLoop();
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
      uint16_t value = assembleUInt16(instructions[6],instructions[5]);
      Serial.print("LOX pressure bangbang set : ");
      Serial.println(value);
      BB_param_set(1, value); 
    }
    if (instructions[4] == 2 ){
      uint16_t value = assembleUInt16(instructions[6],instructions[5]);
      Serial.println("ETH pressure bangbang set : ");
      Serial.println(value);
      BB_param_set(2, value);
    }
    if (instructions[4] == 6 ){
      uint16_t value = assembleUInt16(instructions[6],instructions[5]);
      Serial.println("H20 pressure bangbang set : ");
      Serial.println(value);
      BB_param_set(6, value); 
    }
  }
  if (instructions[0] == 0xFF  && instructions[1] == 0xFF && instructions[2] == 0xDD && instructions[3] == 0xDD){ // bang-bang enable
    if (instructions[4] == 1 ){// tank: 1 = LOX, 2 = ETH, 6 = WATER
      if (instructions[5]==0X00){
        Serial.println("LOX bangbang desactivate");
        byte message[8] = {0xEE, 0xEE, 0xFF, 0xFF, 0xDD, 0xDD, instructions[4], instructions[5]};
        reply(message,sizeof(message));
        BB_enable (1, 0);
      }
      else if (instructions[5]==0X01){
        Serial.println("LOX bangbang activate");
        byte message[8] = {0xEE, 0xEE, 0xFF, 0xFF, 0xDD, 0xDD, instructions[4], instructions[5]};
        reply(message,sizeof(message));
        BB_enable (1, 1);
      }
    }
    else if (instructions[4] == 2 ){
      if (instructions[5]==0X00){
        Serial.println("ETH bangbang desactivate");
        byte message[8] = {0xEE, 0xEE, 0xFF, 0xFF, 0xDD, 0xDD, instructions[4], instructions[5]};
        reply(message,sizeof(message));
        BB_enable (2, 0);
      }
      if (instructions[5]==0X01){
        Serial.println("ETH bangbang activate");
        byte message[8] = {0xEE, 0xEE, 0xFF, 0xFF, 0xDD, 0xDD, instructions[4], instructions[5]};
        reply(message,sizeof(message));
        BB_enable (2, 1);
      }
    }
    else if (instructions[4] == 6 ){
      if (instructions[5]==0X00){
        Serial.println("H2O bangbang desactivate");
        byte message[8] = {0xEE, 0xEE, 0xFF, 0xFF, 0xDD, 0xDD, instructions[4], instructions[5]};
        reply(message,sizeof(message));
        BB_enable (6, 0);
      }
      if (instructions[5]==0X01){
        Serial.println("H2O bangbang activate");
        byte message[8] = {0xEE, 0xEE, 0xFF, 0xFF, 0xDD, 0xDD, instructions[4], instructions[5]};
        reply(message,sizeof(message));
        BB_enable (6, 1);
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
    uint16_t value1 = assembleUInt16(instructions[5], instructions[4]);
    Serial.print("LOX pressure bangbang set : ");
    Serial.println(value1);
    // BB_param_set(0, value);
    
    uint16_t value = assembleUInt16(instructions[7], instructions[6]);
    Serial.print("ETH pressure bangbang set : ");
    Serial.println(value);
    // BB_param_set(1, value1);

    Sequence_data.cooling_enable = assembleUInt16(instructions[8], instructions[9]);
    Serial.print("Cooling enable : ");
    Serial.println(Sequence_data.cooling_enable);

    uint16_t value2 = assembleUInt16(instructions[11], instructions[10]);
    Serial.print("H2O pressure bangbang set : ");
    Serial.println(value2);
    // BB_param_set(2, value2);

    Sequence_data.tvc_pattern = assembleUInt16(instructions[13], instructions[12]);
    Serial.print("TVC pattern : ");
    Serial.println(Sequence_data.tvc_pattern);

    Sequence_data.Confirm_to_purge_delay = assembleUInt16(instructions[15], instructions[14]);
    Serial.print("Confirm to purge delay : ");
    Serial.println(Sequence_data.Confirm_to_purge_delay);

    Sequence_data.Purge_duration1 = assembleUInt16(instructions[17], instructions[16]);
    Serial.print("Purge duration 1 : ");
    Serial.println(Sequence_data.Purge_duration1);

    Sequence_data.Chilldown_on_duration = assembleUInt16(instructions[19], instructions[18]);
    Serial.print("Chilldown ON duration : ");
    Serial.println(Sequence_data.Chilldown_on_duration);

    Sequence_data.Chilldown_off_duration = assembleUInt16(instructions[21], instructions[20]);
    Serial.print("Chilldown OFF duration : ");
    Serial.println(Sequence_data.Chilldown_off_duration);

    Sequence_data.chill_temp = assembleUInt16(instructions[23], instructions[22]) / 10;
    Serial.print("Chill temperature (°C) : ");
    Serial.println(Sequence_data.chill_temp);

    Sequence_data.Max_chilldown = assembleUInt16(instructions[25], instructions[24]);
    Serial.print("Max chilldown : ");
    Serial.println(Sequence_data.Max_chilldown);

    Sequence_data.Chilldown_to_cooling = assembleUInt16(instructions[27], instructions[26]);
    Serial.print("Chilldown to cooling delay : ");
    Serial.println(Sequence_data.Chilldown_to_cooling);

    Sequence_data.cooling_pressure = assembleUInt16(instructions[29], instructions[28]);
    Serial.print("Cooling pressure : ");
    Serial.println(Sequence_data.cooling_pressure);

    Sequence_data.PS63_check_duration = assembleUInt16(instructions[31], instructions[30]);
    Serial.print("PS63 check duration : ");
    Serial.println(Sequence_data.PS63_check_duration);

    Sequence_data.PS63_verified_duration = assembleUInt16(instructions[33], instructions[32]);
    Serial.print("PS63 verified duration : ");
    Serial.println(Sequence_data.PS63_verified_duration);

    Sequence_data.Ign_check_duration = assembleUInt16(instructions[35], instructions[34]);
    Serial.print("Ignition check duration : ");
    Serial.println(Sequence_data.Ign_check_duration);

    Sequence_data.Ign_verified_duration = assembleUInt16(instructions[37], instructions[36]);
    Serial.print("Ignition verified duration : ");
    Serial.println(Sequence_data.Ign_verified_duration);

    Sequence_data.burn_duration = assembleUInt16(instructions[39], instructions[38]);
    Serial.print("Burn duration : ");
    Serial.println(Sequence_data.burn_duration);

    Sequence_data.ETH_to_LOX_bypass = assembleUInt16(instructions[41], instructions[40]);
    Serial.print("ETH to LOX bypass : ");
    Serial.println(Sequence_data.ETH_to_LOX_bypass);

    Sequence_data.Bypass_check_duration = assembleUInt16(instructions[43], instructions[42]);
    Serial.print("Bypass check duration : ");
    Serial.println(Sequence_data.Bypass_check_duration);

    Sequence_data.Bypass_pressure = assembleUInt16(instructions[45], instructions[44]);
    Serial.print("Bypass pressure : ");
    Serial.println(Sequence_data.Bypass_pressure);

    Sequence_data.Bypass_verified_duration = assembleUInt16(instructions[47], instructions[46]);
    Serial.print("Bypass verified duration : ");
    Serial.println(Sequence_data.Bypass_verified_duration);

    Sequence_data.ETH_to_LOX_main = assembleUInt16(instructions[49], instructions[48]);
    Serial.print("ETH to LOX main : ");
    Serial.println(Sequence_data.ETH_to_LOX_main);

    Sequence_data.Main_check_duration = assembleUInt16(instructions[51], instructions[50]);
    Serial.print("Main check duration : ");
    Serial.println(Sequence_data.Main_check_duration);

    Sequence_data.Main_pressure = assembleUInt16(instructions[53], instructions[52]);
    Serial.print("Main pressure : ");
    Serial.println(Sequence_data.Main_pressure);

    Sequence_data.Main_verified_duration = assembleUInt16(instructions[55], instructions[54]);
    Serial.print("Main verified duration : ");
    Serial.println(Sequence_data.Main_verified_duration);

    Sequence_data.TVC_pattern_duration = assembleUInt16(instructions[57], instructions[56]);
    Serial.print("TVC pattern duration : ");
    Serial.println(Sequence_data.TVC_pattern_duration);

    Sequence_data.LOX_to_ETH_closing_delay = assembleUInt16(instructions[59], instructions[58]);
    Serial.print("LOX to ETH closing delay : ");
    Serial.println(Sequence_data.LOX_to_ETH_closing_delay);

    Sequence_data.Purge_duration2 = assembleUInt16(instructions[61], instructions[60]);
    Serial.print("Purge duration 2 : ");
    Serial.println(Sequence_data.Purge_duration2);

    Sequence_data.Cooling_duration_after_end_burn = assembleUInt16(instructions[63], instructions[62]);
    Serial.print("Cooling duration after end of burn : ");
    Serial.println(Sequence_data.Cooling_duration_after_end_burn);
    byte message[8] = {0xBB, 0xBB, 0xBB, 0xBB};
    reply(message,sizeof(message));
  } 
  if (instructions[0] == 0xBB  && instructions[1] == 0xBB && instructions[2] == 0xBB && instructions[3] == 0xBB){ // Confirm test

  } 
  if (instructions[0] == 0xCC  && instructions[1] == 0xCC && instructions[2] == 0xCC && instructions[3] == 0xCC){ // Abort test

  } 
}

void test(){

}
