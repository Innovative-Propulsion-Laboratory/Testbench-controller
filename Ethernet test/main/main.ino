#include "Sensors.h"
// #include "SaveData.h"
// #include "UDP.h"
// main variable
uint32_t time_last_reading = 0;

unsigned long t_last_data_packet = 0, data_send_rate = 1000;

bool test_will_begin = false;

// Sequence
#define IGN_pin 33
#define IGN_check_pin 34

uint16_t T_confirm;
uint16_t Chilldown_finished;
uint16_t last_send;
uint16_t count_down_time;
uint16_t PS63_duration;
uint16_t PS63_seems_rise;
uint16_t Ign_duration;
uint16_t Ign_seems_on;
uint16_t T0;
uint16_t ETH_open;
uint16_t Bypass_duration;
uint16_t Main_seems_rise;
uint16_t Main_duration;
uint16_t Nominal_pressure_reached;
uint16_t T_burn;
uint16_t Chilldown_start;
uint16_t chill_temp_seems_ok;
uint16_t Chilldown_duration;
uint16_t Chilldown_verified_duration;

// sequence_data Sequence_data;

// SaveData
uint32_t frequence_save = 200;  // ms

void setup() {
  Serial.begin(9600);  //initialize Serial Port
  // SPI.begin();         //initialize SPI
  Serial.println("Start");
  pinMode(IGN_pin, OUTPUT);
  pinMode(IGN_check_pin, INPUT);

  pinMode(1, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(28, OUTPUT);
  pinMode(29, OUTPUT);
  pinMode(30, OUTPUT);
  pinMode(35, OUTPUT);
  pinMode(36, OUTPUT);
  pinMode(37, OUTPUT);

  digitalWrite(1, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(28, HIGH);
  digitalWrite(29, HIGH);
  digitalWrite(30, HIGH);
  digitalWrite(35, HIGH);
  digitalWrite(36, HIGH);
  digitalWrite(37, HIGH);

  Serial.println("pinmode");

  setupValves();
  Serial.println("setup valve");

  // Set_valve_position();
  // Serial.println("set valve");

  // setupSensors();
  // Serial.println("setup sensor");

  setupUDP();

  // setupSaveData();

  // setValve(SV21, 0);
  // setValve(SV22, 0);
  // setValve(SV24, 0);
  // setValve(SV31, 1);
  // setValve(SV32, 0);
  // setValve(SV33, 0);
  // setValve(SV34, 1);

  if (CrashReport) {
  Serial.println("CrashReport:");
  Serial.print(CrashReport);
}
}

void loop() {
  Serial.println(millis());
  delay(200);
  // // listen to commands
  // Packet p = receivePacket();

  // if (p.length >= 4 && p.data != nullptr) {
  // decode(p.data);
  // }
  // if (p.data != nullptr) {
  //   delete[] p.data;
  // }
  // if (millis() - time_last_reading >= 50){
  //   sensorsLoop();
  //   time_last_reading = millis();
  //   // serialSend();
  // }
  // if (test_will_begin){
  //   // vérifier que les valeurs sont bonne
  //   byte message[4] = {0xBB, 0xBB, 0xBB, 0xBB};
  //   reply(message,sizeof(message));
  // }
  // BBLoop();
}

uint16_t assembleUInt16(uint8_t lowByte, uint8_t highByte) {  // to assemble 2 byte
  return (static_cast<uint16_t>(highByte) << 8) | static_cast<uint16_t>(lowByte);
}

// void decode(byte* instructions) {

//   if (instructions[0] == 0xff && instructions[1] == 0xff && instructions[2] == 0xff && instructions[3] == 0xff) {  // Valve
//     if (instructions[5] == 0x00 || instructions[5] == 0x01) {
//       setValve(instructions[4], instructions[5]);  // Activer ou dÃ©sactiver la valve
//       byte message[8] = { 0xEE, 0xEE, 0xFF, 0xFF, 0xFF, 0xFF, instructions[4], instructions[5] };
//       reply(message, sizeof(message));
//     }
//   }
//   if (instructions[0] == 0xFF && instructions[1] == 0xFF && instructions[2] == 0xEE && instructions[3] == 0xEE) {  // set bang-bang pressurization
//     if (instructions[4] == 1) {                                                                                    // tank: 0 = LOX, 1 = ETH, 2 = WATER
//       uint16_t value = assembleUInt16(instructions[6], instructions[5]);
//       Serial.print("LOX pressure bangbang set : ");
//       Serial.println(value);
//       BB_param_set(1, value);
//     }
//     if (instructions[4] == 2) {
//       uint16_t value = assembleUInt16(instructions[6], instructions[5]);
//       Serial.println("ETH pressure bangbang set : ");
//       Serial.println(value);
//       BB_param_set(2, value);
//     }
//     if (instructions[4] == 6) {
//       uint16_t value = assembleUInt16(instructions[6], instructions[5]);
//       Serial.println("H20 pressure bangbang set : ");
//       Serial.println(value);
//       BB_param_set(6, value);
//     }
//   }
//   if (instructions[0] == 0xFF && instructions[1] == 0xFF && instructions[2] == 0xDD && instructions[3] == 0xDD) {  // bang-bang enable
//     if (instructions[4] == 1) {                                                                                    // tank: 1 = LOX, 2 = ETH, 6 = WATER
//       if (instructions[5] == 0X00) {
//         Serial.println("LOX bangbang desactivate");
//         BB_enable(1, 0);
//       } else if (instructions[5] == 0X01) {
//         Serial.println("LOX bangbang activate");
//         BB_enable(1, 1);
//       }
//     } else if (instructions[4] == 2) {
//       if (instructions[5] == 0X00) {
//         Serial.println("ETH bangbang desactivate");
//         BB_enable(2, 0);
//       }
//       if (instructions[5] == 0X01) {
//         Serial.println("ETH bangbang activate");
//         BB_enable(2, 1);
//       }
//     } else if (instructions[4] == 6) {
//       if (instructions[5] == 0X00) {
//         Serial.println("H2O bangbang desactivate");
//         BB_enable(6, 0);
//       }
//       if (instructions[5] == 0X01) {
//         Serial.println("H2O bangbang activate");
//         BB_enable(6, 1);
//       }
//     }
//   }
//   if (instructions[0] == 0xEE && instructions[1] == 0xEE && instructions[2] == 0xEE && instructions[3] == 0xEE) {  // Actuators
//     if (instructions[4] == 0) {
//       // the obcsur TVC name + instruction[5]
//     }
//     if (instructions[4] == 1) {
//       // the obcsur TVC name + instruction[5]
//     }
//   }
//   if (instructions[0] == 0xEE && instructions[1] == 0xEE && instructions[2] == 0xDD && instructions[3] == 0xDD) {  // TVC pattern
//     if (instructions[4] == 1) {
//       // the obcsur TVC name
//     }
//     if (instructions[4] == 2) {
//       // the obcsur TVC name
//     }
//     if (instructions[4] == 3) {
//       // the obcsur TVC name
//     }
//     if (instructions[4] == 4) {
//       // the obcsur TVC name
//     }
//     if (instructions[4] == 5) {
//       // the obcsur TVC name
//     }
//   }
//   if (instructions[0] == 0xAA && instructions[1] == 0xAA && instructions[2] == 0xAA && instructions[3] == 0xAA) {  // Start test
//     uint16_t value1 = assembleUInt16(instructions[5], instructions[4]);
//     Serial.print("LOX pressure bangbang set : ");
//     Serial.println(value1);
//     BB_param_set(1, value1);

//     uint16_t value = assembleUInt16(instructions[7], instructions[6]);
//     Serial.print("ETH pressure bangbang set : ");
//     Serial.println(value);
//     BB_param_set(2, value);

//     Sequence_data.cooling_enable = assembleUInt16(instructions[8], instructions[9]);
//     Serial.print("Cooling enable : ");
//     Serial.println(Sequence_data.cooling_enable);

//     uint16_t value2 = assembleUInt16(instructions[11], instructions[10]);
//     Serial.print("H2O pressure bangbang set : ");
//     Serial.println(value2);
//     BB_param_set(6, value2);

//     Sequence_data.tvc_pattern = assembleUInt16(instructions[13], instructions[12]);
//     Serial.print("TVC pattern : ");
//     Serial.println(Sequence_data.tvc_pattern);

//     Sequence_data.Confirm_to_purge_delay = assembleUInt16(instructions[15], instructions[14]);
//     Serial.print("Confirm to purge delay : ");
//     Serial.println(Sequence_data.Confirm_to_purge_delay);

//     Sequence_data.Purge_duration1 = assembleUInt16(instructions[17], instructions[16]);
//     Serial.print("Purge duration 1 : ");
//     Serial.println(Sequence_data.Purge_duration1);

//     Sequence_data.Chilldown_on_duration = assembleUInt16(instructions[19], instructions[18]);
//     Serial.print("Chilldown ON duration : ");
//     Serial.println(Sequence_data.Chilldown_on_duration);

//     Sequence_data.Chilldown_off_duration = assembleUInt16(instructions[21], instructions[20]);
//     Serial.print("Chilldown OFF duration : ");
//     Serial.println(Sequence_data.Chilldown_off_duration);

//     Sequence_data.chill_temp = assembleUInt16(instructions[23], instructions[22]) / 10;
//     Serial.print("Chill temperature (°C) : ");
//     Serial.println(Sequence_data.chill_temp);

//     Sequence_data.Max_chilldown = assembleUInt16(instructions[25], instructions[24]);
//     Serial.print("Max chilldown : ");
//     Serial.println(Sequence_data.Max_chilldown);

//     Sequence_data.Chilldown_to_cooling = assembleUInt16(instructions[27], instructions[26]);
//     Serial.print("Chilldown to cooling delay : ");
//     Serial.println(Sequence_data.Chilldown_to_cooling);

//     Sequence_data.cooling_pressure = assembleUInt16(instructions[29], instructions[28]);
//     Serial.print("Cooling pressure : ");
//     Serial.println(Sequence_data.cooling_pressure);

//     Sequence_data.PS63_check_duration = assembleUInt16(instructions[31], instructions[30]);
//     Serial.print("PS63 check duration : ");
//     Serial.println(Sequence_data.PS63_check_duration);

//     Sequence_data.PS63_verified_duration = assembleUInt16(instructions[33], instructions[32]);
//     Serial.print("PS63 verified duration : ");
//     Serial.println(Sequence_data.PS63_verified_duration);

//     Sequence_data.Ign_check_duration = assembleUInt16(instructions[35], instructions[34]);
//     Serial.print("Ignition check duration : ");
//     Serial.println(Sequence_data.Ign_check_duration);

//     Sequence_data.Ign_verified_duration = assembleUInt16(instructions[37], instructions[36]);
//     Serial.print("Ignition verified duration : ");
//     Serial.println(Sequence_data.Ign_verified_duration);

//     Sequence_data.burn_duration = assembleUInt16(instructions[39], instructions[38]);
//     Serial.print("Burn duration : ");
//     Serial.println(Sequence_data.burn_duration);

//     Sequence_data.ETH_to_LOX_bypass = assembleUInt16(instructions[41], instructions[40]);
//     Serial.print("ETH to LOX bypass : ");
//     Serial.println(Sequence_data.ETH_to_LOX_bypass);

//     Sequence_data.Bypass_check_duration = assembleUInt16(instructions[43], instructions[42]);
//     Serial.print("Bypass check duration : ");
//     Serial.println(Sequence_data.Bypass_check_duration);

//     Sequence_data.Bypass_pressure = assembleUInt16(instructions[45], instructions[44]);
//     Serial.print("Bypass pressure : ");
//     Serial.println(Sequence_data.Bypass_pressure);

//     Sequence_data.Bypass_verified_duration = assembleUInt16(instructions[47], instructions[46]);
//     Serial.print("Bypass verified duration : ");
//     Serial.println(Sequence_data.Bypass_verified_duration);

//     Sequence_data.ETH_to_LOX_main = assembleUInt16(instructions[49], instructions[48]);
//     Serial.print("ETH to LOX main : ");
//     Serial.println(Sequence_data.ETH_to_LOX_main);

//     Sequence_data.Main_check_duration = assembleUInt16(instructions[51], instructions[50]);
//     Serial.print("Main check duration : ");
//     Serial.println(Sequence_data.Main_check_duration);

//     Sequence_data.Main_pressure = assembleUInt16(instructions[53], instructions[52]);
//     Serial.print("Main pressure : ");
//     Serial.println(Sequence_data.Main_pressure);

//     Sequence_data.Main_verified_duration = assembleUInt16(instructions[55], instructions[54]);
//     Serial.print("Main verified duration : ");
//     Serial.println(Sequence_data.Main_verified_duration);

//     Sequence_data.TVC_pattern_duration = assembleUInt16(instructions[57], instructions[56]);
//     Serial.print("TVC pattern duration : ");
//     Serial.println(Sequence_data.TVC_pattern_duration);

//     Sequence_data.LOX_to_ETH_closing_delay = assembleUInt16(instructions[59], instructions[58]);
//     Serial.print("LOX to ETH closing delay : ");
//     Serial.println(Sequence_data.LOX_to_ETH_closing_delay);

//     Sequence_data.Purge_duration2 = assembleUInt16(instructions[61], instructions[60]);
//     Serial.print("Purge duration 2 : ");
//     Serial.println(Sequence_data.Purge_duration2);

//     Sequence_data.Cooling_duration_after_end_burn = assembleUInt16(instructions[63], instructions[62]);
//     Serial.print("Cooling duration after end of burn : ");
//     Serial.println(Sequence_data.Cooling_duration_after_end_burn);

//     BB_enable(2, 1);
//     BB_enable(6, 0);
//     BB_enable(6, 1);

//     test_will_begin = true;

//     byte message[6] = { 0xEE, 0xEE, 0xAA, 0xAA, 0xAA, 0xAA };
//     reply(message, sizeof(message));
//   }
//   if (instructions[0] == 0xBB && instructions[1] == 0xBB && instructions[2] == 0xBB && instructions[3] == 0xBB) {  // Confirm test
//     Data.state = 1;
//     Sequence();
//   }
//   if (instructions[0] == 0xCC && instructions[1] == 0xCC && instructions[2] == 0xCC && instructions[3] == 0xCC) {  // Abort test
//   }
// }

// void count_down() {
//   if (millis() - last_send >= 200) {
//     last_send = millis();
//     count_down_time += 200;
//     byte message[6] = { 0xAB, 0xAB, 0xAB, 0xAB, (byte)(count_down_time >> 8), (byte)(count_down_time & 0xFF) };
//     reply(message, sizeof(message));
//   }
// }

// void Sequence() {
//   pinMode(IGN_pin, OUTPUT);
//   pinMode(IGN_check_pin, INPUT);

//   T_confirm = millis();
//   Data.test_step = 1;
//   set_offset_pressure();

//   do {
//     sensorsLoop();
//     BBLoop();
//     Packet p = receivePacket();
//     if (p.length >= 4 && p.data != nullptr) { decode(p.data); }
//     if (p.data != nullptr) { delete[] p.data; }

//     switch (Data.test_step) {
//       ////// PURGE //////
//       case 1:
//         {
//           if (millis() >= (T_confirm + Sequence_data.Confirm_to_purge_delay)) {
//             Chilldown_start = 0;
//             setValve(SV36, 1);
//             Data.test_step++;
//           }
//           break;
//         }
//       case 2:
//         {
//           if (millis() >= static_cast<uint32_t>(T_confirm + Sequence_data.Confirm_to_purge_delay + Sequence_data.Purge_duration1)) {
//             setValve(SV36, 0);
//             ////// End of PURGE //////
//             ////// start chilldown //////
//             Chilldown_start++;
//             setValve(SV13, 1);
//             Data.test_step++;
//           }
//           break;
//         }
//       case 3:
//         {
//           if (millis() >= static_cast<uint32_t>(T_confirm + Sequence_data.Confirm_to_purge_delay + Sequence_data.Purge_duration1 + Sequence_data.Chilldown_on_duration)) {
//             setValve(SV13, 0);
//             Data.test_step++;
//           }
//           break;
//         }
//       case 4:
//         {
//           if (millis() <= static_cast<uint32_t>(T_confirm + Sequence_data.Confirm_to_purge_delay + Sequence_data.Purge_duration1 + Sequence_data.Chilldown_on_duration + Sequence_data.Chilldown_off_duration)) {
//             if (Data.TS12 >= Sequence_data.chill_temp) {
//               chill_temp_seems_ok = millis();
//               Data.test_step = 5;  // remplace l'étape 4.5
//             } else if (Chilldown_start >= Sequence_data.Max_chilldown) {
//               Data.state = 0;  // Erreur chilldown
//             }
//           } else {
//             T_confirm = millis();
//             Data.test_step = 2;
//             Chilldown_start++;
//           }
//           break;
//         }
//       case 5:
//         {
//           if ((Data.TS12 >= Sequence_data.chill_temp) && ((millis() - chill_temp_seems_ok) >= Chilldown_verified_duration)) {
//             Chilldown_finished = millis();
//             Chilldown_duration = Chilldown_finished - Chilldown_start;
//             Data.test_step++;
//             count_down_time = -10000;
//             byte message[6] = { 0xAB, 0xAB, 0xAB, 0xAB, (byte)(count_down_time >> 8), (byte)(count_down_time & 0xFF) };
//             reply(message, sizeof(message));
//           } else if (Data.TS12 < Sequence_data.chill_temp) {
//             Data.test_step = 4;
//           }
//           break;
//         }
//       ////// end chilldown //////
//       ////// Start cooling //////
//       case 6:
//         {
//           if (Sequence_data.cooling_enable) {
//             if (millis() >= (Chilldown_finished + Sequence_data.Chilldown_to_cooling)) {
//               setValve(SV63, 1);
//               PS63_duration = millis();
//               Data.test_step++;
//             }
//           } else if (millis() >= static_cast<uint32_t>(Chilldown_finished + 10000)) {
//             Data.test_step = 9;
//           }
//           count_down();
//           break;
//         }
//       ////// check cooling //////
//       case 7:
//         {
//           if (Data.PS63 >= Sequence_data.cooling_pressure) {
//             PS63_seems_rise = millis();
//             Data.test_step++;
//           } else if ((millis() - PS63_duration) >= Sequence_data.PS63_check_duration) {
//             Data.state = 0;  // erreur
//           }
//           count_down();
//           break;
//         }
//       case 8:
//         {
//           if ((Data.PS63 >= Sequence_data.cooling_pressure) && ((millis() - PS63_seems_rise) >= Sequence_data.PS63_verified_duration)) {
//             Data.test_step++;
//           } else if (Data.PS63 < Sequence_data.cooling_pressure) {
//             Data.test_step = 7;
//           }
//           count_down();
//           break;
//         }
//       ////// start ignite //////
//       case 9:
//         {
//           Ign_duration = millis();
//           digitalWrite(IGN_pin, HIGH);
//           Data.test_step++;
//           count_down();
//           break;
//         }
//       case 10:
//         {
//           ////// check ignite //////
//           if (digitalRead(IGN_check_pin) == HIGH) {
//             Ign_seems_on = millis();
//             Data.test_step++;
//           } else if ((millis() - Ign_duration) >= Sequence_data.Ign_check_duration) {
//             Data.state = 0;
//           }
//           count_down();
//           break;
//         }
//       case 11:
//         {
//           if ((digitalRead(IGN_check_pin) == HIGH) && ((millis() - Ign_seems_on) >= Sequence_data.Ign_verified_duration)) {
//             digitalWrite(IGN_pin, LOW);
//             T0 = millis();
//             ////// stop ignite //////
//             ////// start bypass //////
//             setValve(SV24, 1);
//             Data.test_step++;
//           } else {
//             Data.test_step = 10;
//           }
//           count_down();
//           break;
//         }
//       case 12:
//         {
//           if (millis() >= (T0 + Sequence_data.ETH_to_LOX_bypass)) {
//             Bypass_duration = millis();
//             setValve(SV13, 1);
//             Data.test_step++;
//           }
//           count_down();
//           break;
//         }
//       case 13:
//         {
//           ////// check bypass //////
//           if ((Data.PS41 >= Sequence_data.Bypass_pressure) && (Data.PS42 >= Sequence_data.Bypass_pressure)) {
//             Bypass_duration = millis();
//             Data.test_step++;
//           } else if ((millis() - Bypass_duration) >= Sequence_data.Bypass_check_duration) {
//             Data.state = 0;
//           }
//           count_down();
//           break;
//         }
//       case 14:
//         {
//           if ((Data.PS41 >= Sequence_data.Bypass_pressure) && (Data.PS42 >= Sequence_data.Bypass_pressure) && ((millis() - Bypass_duration) >= Sequence_data.Bypass_verified_duration)) {
//             ETH_open = millis();
//             ////// start main injection //////
//             setValve(SV22, 1);
//             Data.test_step++;
//           } else {
//             Data.test_step = 12;
//           }
//           count_down();
//           break;
//         }
//       case 15:
//         {
//           if (millis() >= (T0 + Sequence_data.ETH_to_LOX_main)) {
//             Main_duration = millis();
//             setValve(SV12, 1);
//             Data.test_step++;
//           }
//           count_down();
//           break;
//         }
//       case 16:
//         {
//           if ((Data.PS41 >= Sequence_data.Main_pressure) && (Data.PS42 >= Sequence_data.Main_pressure)) {
//             Main_seems_rise = millis();
//             Data.test_step++;
//           } else if ((millis() - Main_duration) >= Sequence_data.Main_check_duration) {
//             Data.state = 0;
//           }
//           count_down();
//           break;
//         }
//       case 17:
//         {
//           ////// check main injection //////
//           if ((Data.PS41 >= Sequence_data.Main_pressure) && (Data.PS42 >= Sequence_data.Main_pressure) && ((millis() - Main_seems_rise) >= Sequence_data.Main_verified_duration)) {
//             Data.test_step++;
//           } else {
//             Data.test_step = 15;
//           }
//           count_down();
//           break;
//         }
//       case 18:
//         {
//           ////// stop bypass //////
//           setValve(SV24, 0);
//           setValve(SV13, 0);
//           Nominal_pressure_reached = millis();
//           Data.test_step++;
//           count_down();
//           break;
//         }
//       case 19:
//         {
//           // TVC launch, not implemented
//           Data.test_step++;
//           count_down();
//           break;
//         }
//       case 20:
//         {
//           ////// stop main injection and purge //////
//           if (millis() >= (T0 + Sequence_data.burn_duration)) {
//             setValve(SV12, 0);
//             setValve(SV36, 1);
//             Data.test_step++;
//           }
//           count_down();
//           break;
//         }
//       case 21:
//         {
//           if (millis() >= static_cast<uint32_t>(T0 + Sequence_data.burn_duration + Sequence_data.LOX_to_ETH_closing_delay)) {
//             setValve(SV22, 0);
//             setValve(SV35, 1);
//             Data.test_step++;
//           }
//           count_down();
//           break;
//         }
//       case 22:
//         {
//           if (millis() >= static_cast<uint32_t>(T0 + Sequence_data.burn_duration + Sequence_data.LOX_to_ETH_closing_delay + Sequence_data.Purge_duration2)) {
//             setValve(SV36, 0);
//             setValve(SV35, 0);
//             Data.test_step++;
//           }
//           count_down();
//           break;
//         }
//       ////// stop cooling //////
//       case 23:
//         {
//           if (millis() >= static_cast<uint32_t>(T0 + Sequence_data.burn_duration + Sequence_data.LOX_to_ETH_closing_delay + Sequence_data.Purge_duration2 + Sequence_data.Cooling_duration_after_end_burn)) {
//             setValve(SV63, 0);
//             Data.state = 0;
//           }
//           count_down();
//           break;
//         }
//     }
//   } while (Data.state == 1);
// }

// float average(byte* L, int length) {
//   float sum = 0;
//   for (int i = 0; i < length; i++) {
//     sum += L[i];
//   }
//   return sum / length;
// }

// void set_offset_pressure() {  // set sensors at 0
//   const int N = 10;
//   byte average_PS12_data[N];
//   byte average_PS22_data[N];
//   byte average_PS41_data[N];
//   byte average_PS42_data[N];
//   byte average_PS63_data[N];
//   byte average_PS64_data[N];

//   for (int i = 0; i < N; i++) {
//     sensorsLoop();
//     // Packet p = receivePacket();
//     // if (p.length >= 4 && p.data != nullptr) {
//     //   decode(p.data);
//     // }
//     // if (p.data != nullptr) {
//     //   delete[] p.data;
//     // }
//     average_PS12_data[i] = Data.PS12;
//     average_PS22_data[i] = Data.PS22;
//     average_PS41_data[i] = Data.PS41;
//     average_PS42_data[i] = Data.PS42;
//     average_PS63_data[i] = Data.PS63;
//     average_PS64_data[i] = Data.PS64;
//   }

//   offset_PS12 = average(average_PS12_data, N);
//   offset_PS22 = average(average_PS22_data, N);
//   offset_PS41 = average(average_PS41_data, N);
//   offset_PS42 = average(average_PS42_data, N);
//   offset_PS63 = average(average_PS63_data, N);
//   offset_PS64 = average(average_PS64_data, N);
// }
