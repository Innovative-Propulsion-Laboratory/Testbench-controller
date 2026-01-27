#include "Decoder.h"
#include "Sequence.h"

// main variable

uint32_t BB_check_time;
uint32_t BB_check_duration = 30000;

#define PL_pin 36  // pilot light pin 



void setup() {
  Serial.begin(9600);  //initialize Serial Port
  SPI.begin();         //initialize SPI

  if (CrashReport) {
    Serial.println("CrashReport:");
    Serial.print(CrashReport);
  }
  pinMode(IGN_pin, OUTPUT);
  pinMode(PL_pin, OUTPUT);
  digitalWrite(IGN_pin, LOW);

  // Disable all CS pins in the setup
  pinMode(1, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(28, OUTPUT);
  pinMode(35, OUTPUT);
  pinMode(36, OUTPUT);
  pinMode(37, OUTPUT);

  digitalWrite(1, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(28, HIGH);
  digitalWrite(35, HIGH);
  digitalWrite(36, HIGH);
  digitalWrite(37, HIGH);

  setupValves();

  Set_valve_position();
  setValve(SV21,0);
  setValve(SV32,0);

  setupSensors();

  delay(50);

  setupUDP();
  setupSaveData();

  Data.test_cooling = 1;
}

void loop() {
  // Listen to commands
  Packet p = receivePacket();
  serial_loop();
  // if (p.length >= 4 && p.data != nullptr) {
  //   decode(p.data);
  // }
  // if (p.data != nullptr) {
  //   delete[] p.data;
  // }

  // Send data at 20Hz
  if (millis() - time_last_reading >= data_send_rate) {
    sensorsLoop();
    time_last_reading = millis();
    
  }
  if (test_will_begin) {
    BB_check_time = millis();
    // // Check if the Bang Bang Pressurization works
    while ((millis() - BB_check_time) < BB_check_duration && !check_BB_pressure() && test_will_begin){

      //listen to commands
      Packet p = receivePacket();
      if (p.length >= 4 && p.data != nullptr) {decode(p.data);}
      if (p.data != nullptr) {delete[] p.data;}

      // Send data at 20Hz
      if (millis() - time_last_reading >= test_send_rate) {
        sensorsLoop();
        time_last_reading = millis();
      }
    }

    if (check_BB_pressure() == true){
      byte message[4] = { 0xBB, 0xBB, 0xBB, 0xBB };
      reply(message, sizeof(message));     
      test_will_begin = false;
    }
    else {
      send_string("error: BB target not reached",1);
      BB_enable(1, 0);
      BB_enable(2, 0);
      BB_enable(6, 0);
      test_will_begin = false;
    }
  }
  // Update BB pressurization more often than sensorsLoop
  BBLoop();

  if ((!UDPactive)&&(millis() - (time_last_reading) >= data_send_rate)){ // nouveau 
    Ethernet.begin();
    Ethernet.onLinkState([](bool state) {});  // No debug output
    if (Ethernet.waitForLocalIP(kDHCPTimeout)) {
      udp.begin(kPort);
      fisrt_message = false;
      UDPactive = true;
    }
  }
}


bool check_BB_pressure() {
  const int N = 100;
  int32_t average_PS11_data[N];
  int32_t average_PS21_data[N];
  int32_t average_PS61_data[N];
  int32_t average_PS62_data[N];

  for (int i = 0; i < N; i++) {
    BBLoop();
    average_PS11_data[i] = Data.PS11;
    average_PS21_data[i] = Data.PS21;
    average_PS61_data[i] = Data.PS61;
    average_PS62_data[i] = Data.PS62;
  }
  
  avg_PS11 = average(average_PS11_data, N);
  avg_PS21 = average(average_PS21_data, N);
  avg_PS61 = average(average_PS61_data, N);
  avg_PS62 = average(average_PS62_data, N);
  // Serial.print(avg_PS11);
  // Serial.print(avg_PS21);
  // Serial.print(avg_PS61);
  // Serial.println(avg_PS62);

  if (Data.test_cooling == 1){
    if (avg_PS11 > (PS11_BB_min - 400) && avg_PS11 < (PS11_BB_max + 400)
    && avg_PS21 > (PS21_BB_min - 400) && avg_PS21 < (PS21_BB_max + 400)
    && avg_PS61 > (WATER_BB_min - 400) && avg_PS61 < (WATER_BB_min + 400)
    && avg_PS62 > (WATER_BB_min - 400) && avg_PS62 < (WATER_BB_min + 400)){
      // Serial.println("pressure reached");
      return true;
  }
  }
  else if (avg_PS11 > (PS11_BB_min - 400) && avg_PS11 < (PS11_BB_max + 400)
  && avg_PS21 > (PS21_BB_min - 400) && avg_PS21 < (PS21_BB_max + 400)) {
    // Serial.println("pressure reached");
    return true;
  }
  return false;
}

void set_pilot_light(bool state){
  digitalWrite(PL_pin, state);
}

void test(){
  // testValves();
  // set_pilot_light(false);
  // digitalWrite(IGN_pin, LOW);
  // testcapteur();
}