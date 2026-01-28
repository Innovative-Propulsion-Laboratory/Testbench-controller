#include "Decoder.h"
#include "Sequence.h"

bool test_will_begin = false;

uint16_t assembleUInt16(uint8_t lowByte, uint8_t highByte) {  // to assemble 2 byte
  return (static_cast<uint16_t>(highByte) << 8) | static_cast<uint16_t>(lowByte);
}

int16_t assembleInt16(uint8_t lowByte, uint8_t highByte) {
  return static_cast<int16_t>((highByte << 8) | lowByte);
}


void decode(byte* instructions) {
  if (Data.state == 1){
    if (instructions[0] == 0xCC && instructions[1] == 0xCC && instructions[2] == 0xCC && instructions[3] == 0xCC) {
      Data.state = 0;
      test_abort(1);
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
    if (instructions[0] == 0xFF && instructions[1] == 0xFF && instructions[2] == 0xAA && instructions[3] == 0xAA) { //open/close glow plug
      if (instructions[4] == 0) {
        digitalWrite(GP_ignite_pin, LOW); //turn off glow plug
        Serial.println("Glowplug OFF");
      }
      if (instructions[4] == 1) {
        digitalWrite(GP_ignite_pin, HIGH); //turn on glow plug
        Serial.println("Glowplug ON");
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
      
      Data.test_cooling = assembleUInt16(instructions[9], instructions[8]);
      Serial.println("Cooling enable : ");
      Serial.println(Data.test_cooling);
      
      uint16_t value2 = assembleUInt16(instructions[11], instructions[10]);
      Serial.print("H2O pressure bangbang set : ");
      Serial.println(value2);
      if (Data.test_cooling == 1) {
        BB_param_set(6, value2);
      }
      
      Sequence_data.tvc_pattern = assembleUInt16(instructions[13], instructions[12]);
      Serial.print("TVC pattern : ");
      Serial.println(Sequence_data.tvc_pattern);
      
      Sequence_data.Confirm_to_purge_delay = assembleUInt16(instructions[15], instructions[14]);
      Serial.print("Confirm to purge delay : ");
      Serial.println(Sequence_data.Confirm_to_purge_delay);
      
      Sequence_data.Purge_duration1 = assembleUInt16(instructions[17], instructions[16]);
      Serial.print("Purge duration 1 : ");
      Serial.println(Sequence_data.Purge_duration1);
      
      Sequence_data.GP_current = assembleUInt16(instructions[19], instructions[18]);
      Serial.print("GP's current : ");
      Serial.println(Sequence_data.GP_current);
      
      Sequence_data.Glowplug_heat_before_duration = assembleUInt16(instructions[21], instructions[20]);
      Serial.print("GP heat before duration : ");
      Serial.println(Sequence_data.Glowplug_heat_before_duration);

      Sequence_data.Current_raising = assembleUInt16(instructions[23], instructions[22]);
      Serial.print("Raising current : ");
      Serial.println(Sequence_data.Current_raising);
      
      Sequence_data.Chilldown_on_duration = assembleUInt16(instructions[25], instructions[24]);
      Serial.print("Chilldown ON duration : ");
      Serial.println(Sequence_data.Chilldown_on_duration);
      
      Sequence_data.Chilldown_off_duration = assembleUInt16(instructions[27], instructions[26]);
      Serial.print("Chilldown OFF duration : ");
      Serial.println(Sequence_data.Chilldown_off_duration);
      
      Sequence_data.chill_temp = assembleInt16(instructions[29], instructions[28]);
      Serial.print("Chill temperature (°C) : ");
      Serial.println(Sequence_data.chill_temp);
      
      Sequence_data.Max_chilldown = assembleUInt16(instructions[31], instructions[30]);
      Serial.print("Max chilldown : ");
      Serial.println(Sequence_data.Max_chilldown);
      
      Sequence_data.Purge_duration2 = assembleUInt16(instructions[33], instructions[32]);
      Serial.print("Purge duration 2 : ");
      Serial.println(Sequence_data.Purge_duration2);
      
      Sequence_data.Chilldown_to_cooling = assembleUInt16(instructions[35], instructions[34]);
      Serial.print("Chilldown to cooling delay : ");
      Serial.println(Sequence_data.Chilldown_to_cooling);
      
      Sequence_data.PS63_check_duration = assembleUInt16(instructions[37], instructions[36]);
      Serial.print("PS63 check duration : ");
      Serial.println(Sequence_data.PS63_check_duration);

      Sequence_data.cooling_pressure = assembleUInt16(instructions[39], instructions[38]);
      Serial.print("Cooling pressure : ");
      Serial.println(Sequence_data.cooling_pressure);
      
      Sequence_data.PS63_verified_duration = assembleUInt16(instructions[41], instructions[40]);
      Serial.print("PS63 verified duration : ");
      Serial.println(Sequence_data.PS63_verified_duration);

      Sequence_data.ETH_to_GOX = assembleUInt16(instructions[43], instructions[42]);
      Serial.print("ETH to GOX : ");
      Serial.println(Sequence_data.ETH_to_GOX);    

      Sequence_data.Igniter_chamber_pressure = assembleUInt16(instructions[45], instructions[44]);
      Serial.print("Igniter chamber pressure : ");
      Serial.println(Sequence_data.Igniter_chamber_pressure);     

      Sequence_data.Igniter_pressure_time = assembleUInt16(instructions[47], instructions[46]);
      Serial.print("Igniter pressure time : ");
      Serial.println(Sequence_data.Igniter_pressure_time);    

      Sequence_data.Igniter_Highpressure_time = assembleUInt16(instructions[49], instructions[48]);
      Serial.print("Igniter Highpressure time : ");
      Serial.println(Sequence_data.Igniter_Highpressure_time);   

      Sequence_data.Igniter_burn_duration = assembleUInt16(instructions[51], instructions[50]);
      Serial.print("Igniter burn duration : ");
      Serial.println(Sequence_data.Igniter_burn_duration);

      Sequence_data.GOX_to_ETH = assembleUInt16(instructions[53], instructions[52]);
      Serial.print("GOX to ETH : ");
      Serial.println(Sequence_data.GOX_to_ETH);

      Sequence_data.Purge_after_duration = assembleUInt16(instructions[55], instructions[54]);
      Serial.print("Purge after duration : ");
      Serial.println(Sequence_data.Purge_after_duration);

      /*   Séquence de l'ancien allumeur (à réintégrer si besoin dans une future version)

      Sequence_data.Ign_to_bypass = assembleUInt16(instructions[37], instructions[36]);
      Serial.print("Ign to bypass : ");
      Serial.println(Sequence_data.Ign_to_bypass);
      
      Sequence_data.burn_duration = assembleUInt16(instructions[39], instructions[38]);
      Serial.print("Burn duration : ");
      Serial.println(Sequence_data.burn_duration);
      */
      Sequence_data.ETH_to_LOX_bypass = assembleUInt16(instructions[57], instructions[56]);
      Serial.print("ETH to LOX bypass : ");
      Serial.println(Sequence_data.ETH_to_LOX_bypass);
      
      Sequence_data.Bypass_check_duration = assembleUInt16(instructions[59], instructions[58]);
      Serial.print("Bypass check duration : ");
      Serial.println(Sequence_data.Bypass_check_duration);
      
      Sequence_data.Bypass_pressure = assembleUInt16(instructions[61], instructions[60]);
      Serial.print("Bypass pressure : ");
      Serial.println(Sequence_data.Bypass_pressure);
      
      Sequence_data.Bypass_verified_duration = assembleUInt16(instructions[63], instructions[62]);
      Serial.print("Bypass verified duration : ");
      Serial.println(Sequence_data.Bypass_verified_duration);
      
      Sequence_data.ETH_to_LOX_main = assembleUInt16(instructions[65], instructions[64]);
      Serial.print("ETH to LOX main : ");
      Serial.println(Sequence_data.ETH_to_LOX_main);
      
      Sequence_data.Main_check_duration = assembleUInt16(instructions[67], instructions[66]);
      Serial.print("Main check duration : ");
      Serial.println(Sequence_data.Main_check_duration);
      
      Sequence_data.Main_pressure = assembleUInt16(instructions[69], instructions[68]);
      Serial.print("Main pressure : ");
      Serial.println(Sequence_data.Main_pressure);
      
      Sequence_data.Main_verified_duration = assembleUInt16(instructions[71], instructions[70]);
      Serial.print("Main verified duration : ");
      Serial.println(Sequence_data.Main_verified_duration);
      
      Sequence_data.TVC_pattern_duration = assembleUInt16(instructions[73], instructions[72]);
      Serial.print("TVC pattern duration : ");
      Serial.println(Sequence_data.TVC_pattern_duration);
      
      Sequence_data.LOX_to_ETH_closing_delay = assembleUInt16(instructions[75], instructions[74]);
      Serial.print("LOX to ETH closing delay : ");
      Serial.println(Sequence_data.LOX_to_ETH_closing_delay);
      
      Sequence_data.Purge_duration3 = assembleUInt16(instructions[77], instructions[76]);
      Serial.print("Purge duration 3 : ");
      Serial.println(Sequence_data.Purge_duration3);
      
      Sequence_data.Cooling_duration_after_end_burn = assembleUInt16(instructions[79], instructions[78]);
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

