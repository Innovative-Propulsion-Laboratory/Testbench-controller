#include "Sensors.h"
// #include "UDP.h"

// main variable
bool test_will_begin = false;
uint32_t BB_check_time;
uint32_t BB_check_duration = 30000;

// Sequence
#define IGN_pin 33
#define IGN_check_pin 39
#define DEBUG true  // Mettre sur false pour couper les logs

#define debug(msg) if (DEBUG) { Serial.println(msg); }

#define debugf(fmt, ...) if (DEBUG) { char buf[128]; snprintf(buf, sizeof(buf), fmt, __VA_ARGS__); Serial.println(buf); }

uint16_t Tchilldown;



void setup() {
  Serial.begin(9600);  //initialize Serial Port
  SPI.begin();         //initialize SPI

  if (CrashReport) {
    Serial.println("CrashReport:");
    Serial.print(CrashReport);
  }
  pinMode(IGN_pin, OUTPUT);
  // pinMode(IGN_check_pin, INPUT);  // only on PCB V2

  // Disable all CS pins in the setup
  pinMode(1, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(28, OUTPUT);
  pinMode(29, OUTPUT);
  pinMode(30, OUTPUT);
  pinMode(35, OUTPUT);
  pinMode(36, OUTPUT);
  pinMode(37, OUTPUT);

  digitalWrite(2, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(28, HIGH);
  digitalWrite(29, HIGH);
  digitalWrite(30, HIGH);
  digitalWrite(35, HIGH);
  digitalWrite(36, HIGH);
  digitalWrite(37, HIGH);

  setupValves();

  delay(50);

  Set_valve_position();
  setupSensors();
  setupUDP();
  setupSaveData();

  Data.test_cooling = 0;
}

void loop() {
  // Listen to commands
  Packet p = receivePacket();

  if (p.length >= 4 && p.data != nullptr) {
    decode(p.data);
  }
  if (p.data != nullptr) {
    delete[] p.data;
  }

  // Send data at 20Hz
  if (millis() - time_last_reading >= 50) {
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
      if (millis() - time_last_reading >= 50) {
        sensorsLoop();
        time_last_reading = millis();
      }
    }

    if (check_BB_pressure() == true){
      byte message[4] = { 0xBB, 0xBB, 0xBB, 0xBB };
      reply(message, sizeof(message));      
    }
    else {
      send_string("error: BB target not reached",1);
      BB_enable(1, 0);
      BB_enable(2, 0);
      BB_enable(6, 0);
      test_will_begin = false;
    }
  }
  // Update BB pressurization more ofsten than sensorsLoop
  BBLoop();
}

uint16_t assembleUInt16(uint8_t lowByte, uint8_t highByte) {  // to assemble 2 byte
  return (static_cast<uint16_t>(highByte) << 8) | static_cast<uint16_t>(lowByte);
}

void decode(byte* instructions) {
  if (Data.state == 1){
    if (instructions[0] == 0xCC && instructions[1] == 0xCC && instructions[2] == 0xCC && instructions[3] == 0xCC) {
      test_abort();
    }
  }
  else {
    if (instructions[0] == 0xff && instructions[1] == 0xff && instructions[2] == 0xff && instructions[3] == 0xff) {  // Valve
      if (instructions[5] == 0x00 || instructions[5] == 0x01) {
        setValve(instructions[4], instructions[5]);  // Open or close the valve
        byte message[8] = { 0xEE, 0xEE, 0xFF, 0xFF, 0xFF, 0xFF, instructions[4], instructions[5] };
        reply(message, sizeof(message));
      }
    }
    if (instructions[0] == 0xFF && instructions[1] == 0xFF && instructions[2] == 0xEE && instructions[3] == 0xEE) {  // set bang-bang pressurization
      if (instructions[4] == 1) {                                                                                    // tank: 0 = LOX, 1 = ETH, 2 = WATER
        uint16_t value = assembleUInt16(instructions[6], instructions[5]);
        Serial.print("LOX pressure bangbang set : ");
        Serial.println(value);
        BB_param_set(1, value);
      }
      if (instructions[4] == 2) {
        uint16_t value = assembleUInt16(instructions[6], instructions[5]);
        Serial.println("ETH pressure bangbang set : ");
        Serial.println(value);
        BB_param_set(2, value);
      }
      if (instructions[4] == 6) {
        uint16_t value = assembleUInt16(instructions[6], instructions[5]);
        Serial.println("H20 pressure bangbang set : ");
        Serial.println(value);
        BB_param_set(6, value);
      }
    }
    if (instructions[0] == 0xFF && instructions[1] == 0xFF && instructions[2] == 0xDD && instructions[3] == 0xDD) {  // bang-bang enable
      if (instructions[4] == 1) {                                                                                    // tank: 1 = LOX, 2 = ETH, 6 = WATER
        if (instructions[5] == 0X00) {
          Serial.println("LOX bangbang disabled");
          BB_enable(1, 0);
        } else if (instructions[5] == 0X01) {
          Serial.println("LOX bangbang enabled");
          BB_enable(1, 1);
        }
      } else if (instructions[4] == 2) {
        if (instructions[5] == 0X00) {
          Serial.println("ETH bangbang disabled");
          BB_enable(2, 0);
        }
        if (instructions[5] == 0X01) {
          Serial.println("ETH bangbang enabled");
          BB_enable(2, 1);
        }
      } else if (instructions[4] == 6) {
        if (instructions[5] == 0X00) {
          Serial.println("H2O bangbang disabled");
          BB_enable(6, 0);
        }
        if (instructions[5] == 0X01) {
          Serial.println("H2O bangbang enabled");
          BB_enable(6, 1);
        }
      }
    }
    /* To be implemented when the digital potentiometers are ready
    if (instructions[0] == 0xEE && instructions[1] == 0xEE && instructions[2] == 0xEE && instructions[3] == 0xEE) {  // Actuators
      if (instructions[4] == 0) {
        // the obcsur TVC name + instruction[5]
      }
      if (instructions[4] == 1) {
        // the obcsur TVC name + instruction[5]
      }
    }
    if (instructions[0] == 0xEE && instructions[1] == 0xEE && instructions[2] == 0xDD && instructions[3] == 0xDD) {  // TVC pattern
      if (instructions[4] == 1) {
        // the obcsur TVC name
      }
      if (instructions[4] == 2) {
        // the obcsur TVC name
      }
      if (instructions[4] == 3) {
        // the obcsur TVC name
      }
      if (instructions[4] == 4) {
        // the obcsur TVC name
      }
      if (instructions[4] == 5) {
        // the obcsur TVC name
      }
    }
    */
    if (instructions[0] == 0xAA && instructions[1] == 0xAA && instructions[2] == 0xAA && instructions[3] == 0xAA) {  // Start test
      uint16_t value1 = assembleUInt16(instructions[5], instructions[4]);
      Serial.print("LOX pressure bangbang set : ");
      Serial.println(value1);
      BB_param_set(1, value1);
  
      uint16_t value = assembleUInt16(instructions[7], instructions[6]);
      Serial.print("ETH pressure bangbang set : ");
      Serial.println(value);
      BB_param_set(2, value);
  
      Data.test_cooling = assembleUInt16(instructions[8], instructions[9]);
      Serial.println("Cooling enable : ");
      Serial.println(Data.test_cooling);
  
      uint16_t value2 = assembleUInt16(instructions[11], instructions[10]);
      Serial.print("H2O pressure bangbang set : ");
      Serial.println(value2);
      if (Data.test_cooling == 1) {BB_param_set(6, value2);}
  
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
      
      BB_enable(1, 1);
      BB_enable(2, 1);
      if (Data.test_cooling == 1) {BB_enable(6, 1);}
  
      test_will_begin = true;
  
      byte message[6] = { 0xEE, 0xEE, 0xAA, 0xAA, 0xAA, 0xAA };
      reply(message, sizeof(message));
    }
    if (instructions[0] == 0xBB && instructions[1] == 0xBB && instructions[2] == 0xBB && instructions[3] == 0xBB) {  // Confirm test
      Data.state = 1;
      Sequence();
    }
    if (instructions[0] == 0xDC && instructions[1] == 0xBA && instructions[2] == 0xDC && instructions[3] == 0xBA) {  // Stop test 
      test_will_begin = false;
      BB_enable(1, 0);
      BB_enable(2, 0);
      BB_enable(6, 0);
      Serial.println("arreter le test");
    }
  }
}

void count_down() {
  if (millis() - last_send >= 200) {
    last_send = millis();
    count_down_time += 200;
    byte message[6] = { 0xAB, 0xAB, 0xAB, 0xAB, (byte)(count_down_time >> 8), (byte)(count_down_time & 0xFF) };
    reply(message, sizeof(message));
  }
}


void Sequence() {
  newFile(); // Create a new SD file at each test

  T_confirm = millis();
  Data.test_step = 1;
  Chilldown_count = 0;
  set_offset_pressure();
  debug("=== Debut de la sequence ===");

  do {
    BBLoop();

    if (millis() - time_last_reading >= data_send_rate) {
      sensorsLoop();
      time_last_reading = millis();
    }

    Packet p = receivePacket();
    if (p.length >= 4 && p.data != nullptr) { decode(p.data); }
    if (p.data != nullptr) { delete[] p.data; }

    switch (Data.test_step) {
      case 1:
        debug("[1] Attente avant purge");
        if (millis() >= (T_confirm + Sequence_data.Confirm_to_purge_delay)) {
          setValve(SV36, 1);
          debug("→ Ouverture SV36 (purge)");
          Data.test_step++;
        }
        break;

      case 2:
        debug("[2] Purge en cours");
        if (millis() >= static_cast<uint32_t>(T_confirm + Sequence_data.Confirm_to_purge_delay + Sequence_data.Purge_duration1)) {
          setValve(SV36, 0);
          debug("→ Fermeture SV36 (fin purge)");
        }
        break;

      case 3:  
        if (Chilldown_count == 0) { Chilldown_start = millis(); } else {Chilldown_count++; debug("→ Ouverture SV13 (chilldown)");}
        setValve(SV13, 1);
        Tchilldown = millis();
        Data.test_step++;
        break;

      case 4:
        debug("[3] Chilldown ON");
        if (millis() >= static_cast<uint32_t>(Tchilldown + Sequence_data.Chilldown_on_duration)) {
          setValve(SV13, 0);
          debug("→ Fermeture SV13 (chilldown OFF)");
          Data.test_step++;
        }
        break;

      case 5:
        debug("[4] Chilldown OFF / Verification");
        if (millis() <= static_cast<uint32_t>(T_confirm + Sequence_data.Confirm_to_purge_delay + Sequence_data.Purge_duration1 + Sequence_data.Chilldown_on_duration + Sequence_data.Chilldown_off_duration)) {
          if (Data.TS12 <= Sequence_data.chill_temp) {
            debug("✓ Temperature chilldown atteinte");
            chill_temp_seems_ok = millis();
            Data.test_step++;
          } else if (Chilldown_count >= Sequence_data.Max_chilldown) {
            debug("✖ Chilldown echoue");
            send_string("error: Chilldown failed", 1);
            test_abort();
          } else {
            debug("↻ Chilldown supplementaire necessaire");
            Tchilldown = millis();
            Sequence_data.Confirm_to_purge_delay = 0;
            Data.test_step = 3;
          }
        }
        break;

      case 6:
        debug("[5] Verif chilldown stable");
        if ((Data.TS12 <= Sequence_data.chill_temp) && ((millis() - chill_temp_seems_ok) >= Chilldown_verified_duration)) {
          Chilldown_finished = millis();
          Chilldown_duration = Chilldown_finished - Chilldown_start;
          debugf("✓ Chilldown termine en %d ms", Chilldown_duration);
          Data.test_step++;
          count_down_time = -10000;
          byte message[6] = { 0xAB, 0xAB, 0xAB, 0xAB, (byte)(count_down_time >> 8), (byte)(count_down_time & 0xFF)};
          reply(message, sizeof(message));
        } else if (Data.TS12 < Sequence_data.chill_temp) {
          Data.test_step = 4;
        }
        break;

      case 7:
        if (Data.test_cooling) {
          debug("[6] Demarrage refroidissement");
          if (millis() >= (Chilldown_finished + Sequence_data.Chilldown_to_cooling)) {
            setValve(SV63, 1);
            debug("→ Ouverture SV63 (refroidissement)");
            PS63_duration = millis();
            Data.test_step++;
          }
        } else if (millis() >= static_cast<uint32_t>(Chilldown_finished + 10000)) {
          Data.test_step = 9;
        }
        count_down();
        break;

      case 8:
        debug("[7] Verif pression refroidissement");
        if (Data.PS63 >= Sequence_data.cooling_pressure) {
          PS63_seems_rise = millis();
          debug("✓ Refroidissement detecte");
          Data.test_step++;
        } else if ((millis() - PS63_duration) >= Sequence_data.PS63_check_duration) {
          debug("✖ Erreur: Refroidissement non detecte");
          send_string("error: Cooling not detected", 1);
          test_abort();
        }
        count_down();
        break;

      case 9:
        debug("[8] Stabilisation refroidissement");
        if ((Data.PS63 >= Sequence_data.cooling_pressure) && ((millis() - PS63_seems_rise) >= Sequence_data.PS63_verified_duration)) {
          Data.test_step++;
        } else if (Data.PS63 < Sequence_data.cooling_pressure) {
          Data.test_step = 7;
        }
        count_down();
        break;

      case 10:
        debug("[9] Allumage");
        Ign_duration = millis();
        digitalWrite(IGN_pin, HIGH);
        Data.test_step++;
        count_down();
        break;

      case 11:
        debug("[10] Verif allumage");
        if (digitalRead(IGN_check_pin) == LOW) {
          Ign_seems_on = millis();
          debug("✓ Allumeur actif");
          Data.test_step++;
        } else if ((millis() - Ign_duration) >= Sequence_data.Ign_check_duration) {
          debug("✖ Erreur: Allumage rate");
          send_string("error: Ignition failed", 1);
          test_abort();
        }
        count_down();
        break;

      case 12:
        debug("[11] Confirmation allumage et bypass");
        if ((digitalRead(IGN_check_pin) == LOW) && ((millis() - Ign_seems_on) >= Sequence_data.Ign_verified_duration)) {
          digitalWrite(IGN_pin, LOW);
          T0 = millis();
          setValve(SV24, 1);
          debug("→ Ouverture SV24 (bypass)");
          Data.test_step++;
        } else {
          Data.test_step = 10;
        }
        count_down();
        break;

      case 13:
        debug("[12] Attente injection LOX");
        if (millis() >= (T0 + Sequence_data.ETH_to_LOX_bypass)) {
          Bypass_duration = millis();
          setValve(SV13, 1);
          debug("→ Ouverture SV13 (LOX)");
          Data.test_step++;
        }
        count_down();
        break;

      case 14:
        debug("[13] Verif bypass");
        if ((Data.PS41 >= Sequence_data.Bypass_pressure) && (Data.PS42 >= Sequence_data.Bypass_pressure)) {
          Bypass_duration = millis();
          debug("✓ Pressions bypass OK");
          Data.test_step++;
        } else if ((millis() - Bypass_duration) >= Sequence_data.Bypass_check_duration) {
          debug("✖ Erreur: Pression trop basse (bypass)");
          send_string("error: Pressure too low with bypass valves", 1);
          test_abort();
        }
        count_down();
        break;

      case 15:
        debug("[14] Stabilisation bypass");
        if ((Data.PS41 >= Sequence_data.Bypass_pressure) && (Data.PS42 >= Sequence_data.Bypass_pressure) && ((millis() - Bypass_duration) >= Sequence_data.Bypass_verified_duration)) {
          ETH_open = millis();
          setValve(SV22, 1);
          debug("→ Ouverture SV22 (ETH)");
          Data.test_step++;
        } else {
          Data.test_step = 12;
        }
        count_down();
        break;

      case 16:
        debug("[15] Injection LOX");
        if (millis() >= (T0 + Sequence_data.ETH_to_LOX_main)) {
          Main_duration = millis();
          setValve(SV12, 1);
          debug("→ Ouverture SV12 (LOX)");
          Data.test_step++;
        }
        count_down();
        break;

      case 17:
        debug("[16] Verif pression injection");
        if ((Data.PS41 >= Sequence_data.Main_pressure) && (Data.PS42 >= Sequence_data.Main_pressure)) {
          Main_seems_rise = millis();
          debug("✓ Pressions injection OK");
          Data.test_step++;
        } else if ((millis() - Main_duration) >= Sequence_data.Main_check_duration) {
          debug("✖ Erreur: Pression injection trop faible");
          send_string("error: Pressure too low with main valves", 1);
          test_abort();
        }
        count_down();
        break;

      case 18:
        debug("[17] Stabilisation injection");
        if ((Data.PS41 >= Sequence_data.Main_pressure) && (Data.PS42 >= Sequence_data.Main_pressure) && ((millis() - Main_seems_rise) >= Sequence_data.Main_verified_duration)) {
          Data.test_step++;
        } else {
          Data.test_step = 15;
        }
        count_down();
        break;

      case 19:
        debug("[18] Fermeture bypass");
        setValve(SV24, 0);
        setValve(SV13, 0);
        debug("→ Fermeture SV24 et SV13");
        Nominal_pressure_reached = millis();
        Data.test_step++;
        count_down();
        break;

      case 20:
        debug("[19] TVC (non implemente)");
        Data.test_step++;
        count_down();
        break;

      case 21:
        debug("[20] Fin de combustion, purge LOX");
        if (millis() >= (T0 + Sequence_data.burn_duration)) {
          setValve(SV12, 0);
          setValve(SV36, 1);
          debug("→ Fermeture SV12, Ouverture SV36");
          Data.test_step++;
        }
        count_down();
        break;

      case 22:
        debug("[21] Fermeture ETH, purge ETH");
        if (millis() >= static_cast<uint32_t>(T0 + Sequence_data.burn_duration + Sequence_data.LOX_to_ETH_closing_delay)) {
          setValve(SV22, 0);
          setValve(SV35, 1);
          debug("→ Fermeture SV22, Ouverture SV35");
          Data.test_step++;
        }
        count_down();
        break;

      case 23:
        debug("[22] Fin de purge");
        if (millis() >= static_cast<uint32_t>(T0 + Sequence_data.burn_duration + Sequence_data.LOX_to_ETH_closing_delay + Sequence_data.Purge_duration2)) {
          setValve(SV36, 0);
          setValve(SV35, 0);
          debug("→ Fermeture SV36 et SV35");
          Data.test_step++;
        }
        count_down();
        break;

      case 24:
        debug("[23] Fin refroidissement");
        if (millis() >= static_cast<uint32_t>(T0 + Sequence_data.burn_duration + Sequence_data.LOX_to_ETH_closing_delay + Sequence_data.Purge_duration2 + Sequence_data.Cooling_duration_after_end_burn)) {
          setValve(SV63, 0);
          debug("→ Fermeture SV63 (fin refroidissement)");
          Data.state = 0;
        }
        count_down();
        break;
    }
  } while ((Data.state == 1) && (test_will_begin));

  debug("=== Fin de sequence ===");
  reset_offset_pressure();
  Data.test_cooling = 1;
  BB_enable(1, 0);
  BB_enable(2, 0);
  BB_enable(6, 0);
  byte message[4] = {0xAB, 0xCD, 0xAB, 0xCD};
  reply(message, sizeof(message));
  test_will_begin = false;
}

float average(int32_t* L, int length) {
  float sum = 0;
  for (int i = 0; i < length; i++) {
    sum += L[i];
  }
  return sum / length;
}

void set_offset_pressure() {  // set sensors at 0
  const int N = 10;
  int32_t average_PS12_data[N];
  int32_t average_PS22_data[N];
  int32_t average_PS41_data[N];
  int32_t average_PS42_data[N];
  int32_t average_PS63_data[N];
  int32_t average_PS64_data[N];

  for (int i = 0; i < N; i++) {
    if (millis() - time_last_reading >= 50) {
    sensorsLoop();
    time_last_reading = millis();
    }
    BBLoop();
    read_atm_pressure();
    Packet p = receivePacket();
    if (p.length >= 4 && p.data != nullptr) {
      decode(p.data);
    }
    if (p.data != nullptr) {
      delete[] p.data;
    }
    average_PS12_data[i] = Data.PS12;
    average_PS22_data[i] = Data.PS22;
    average_PS41_data[i] = Data.PS41;
    average_PS42_data[i] = Data.PS42;
    average_PS63_data[i] = Data.PS63;
    average_PS64_data[i] = Data.PS64;
  }

  offset_PS12 = average(average_PS12_data, N);
  offset_PS22 = average(average_PS22_data, N);
  offset_PS41 = average(average_PS41_data, N);
  offset_PS42 = average(average_PS42_data, N);
  offset_PS63 = average(average_PS63_data, N);
  offset_PS64 = average(average_PS64_data, N);
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
    if (avg_PS11 > (PS11_BB_min - 200) && avg_PS11 < (PS11_BB_max + 200)
    && avg_PS21 > (PS21_BB_min - 200) && avg_PS21 < (PS21_BB_max + 200)
    && avg_PS61 > (WATER_BB_min - 200) && avg_PS61 < (WATER_BB_min + 200)
    && avg_PS62 > (WATER_BB_min - 200) && avg_PS62 < (WATER_BB_min + 200)){
      // Serial.println("pressure reached");
      return true;
  }
  }
  else if (avg_PS11 > (PS11_BB_min - 200) && avg_PS11 < (PS11_BB_max + 200)
  && avg_PS21 > (PS21_BB_min - 200) && avg_PS21 < (PS21_BB_max + 200)) {
    // Serial.println("pressure reached");
    return true;
  }
  return false;
}