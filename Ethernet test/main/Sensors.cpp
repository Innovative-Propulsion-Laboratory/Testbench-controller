#include "Sensors.h"

// --------------------- VARIABLES INITIALIZATION ------------------------------

uint32_t n = 0;  //Packet ID
uint8_t cr0, fault11, fault12, fault41, fault42, fault61, fault62;

data Data;

bool TS12_waiting = 0, TS41_waiting = 0, TS42_waiting = 0, TS61_waiting = 0, TS62_waiting = 0, TS11_waiting = 0;

Adafruit_MAX31856 thermo12 = Adafruit_MAX31856(TS12_pin, 11, 12, 13);
Adafruit_MAX31856 thermo41 = Adafruit_MAX31856(TS41_pin, 11, 12, 13);
Adafruit_MAX31856 thermo42 = Adafruit_MAX31856(TS42_pin, 11, 12, 13);
Adafruit_MAX31856 thermo61 = Adafruit_MAX31856(TS61_pin, 11, 12, 13);
Adafruit_MAX31856 thermo62 = Adafruit_MAX31856(TS62_pin, 11, 12, 13);
Adafruit_MAX31856 thermo11 = Adafruit_MAX31856(TS11_pin, 11, 12, 13);  //initialized last since it may return negative values


//Offset
float offset_PS12 = 0;
float offset_PS22 = 0;
float offset_PS41 = 0;
float offset_PS42 = 0;
float offset_PS63 = 0;
float offset_PS64 = 0;

// ------------------------- LIMITS DEFINITION ---------------------------------

uint16_t PS_oob_max_delay = 250;   // defines the maximum duration for which a pressure sensor value can stay out of bounds in ms
uint16_t TS_oob_max_delay = 1000;  // defines the maximum duration for which a temperature sensor value can stay out of bounds in ms

// Limits are defined as: LL: Lower Limit, LW: Lower Warning, N: Nominal, UW: Upper Warning, UL: Upper Limit
// A "T" is added if the value is only checked during tests
// A "BB" is added if the value is only checked if the bang-bang pressurization of the concered tank is active -- in the "pressurization.ino" file
// If a warning is reached, a message is sent to the computer
// If a limit is reached, the microcontroller takes action to solve the problem or to put the testbench in a safe position

sequence_data Sequence_data;

// Pressure sensors:
uint16_t PS11_UL = 25000;
uint16_t PS12_TLW = 9000, PS12_TUW = 13000;
uint16_t PS21_UL = 25000;
uint16_t PS22_TLW = 9000, PS22_TUW = 13000;
uint16_t PS31_LW = 20000, PS31_UW = 50000, PS31_UL = 55000;
uint16_t PS41_TLL = 6000, PS41_TLW = 7000, PS41_TUW = 13000, PS41_TUL = 14000;
uint16_t PS42_TLL = 6000, PS42_TLW = 7000, PS42_TUW = 13000, PS42_TUL = 14000;
uint16_t PS51_TLL = 10000, PS51_LW = 40000;
int32_t PS51_UW = 210000;
uint16_t PS_WATER_TLL = 6000, PS_WATER_UL = 14000;

// Thermocouples:
uint16_t TS62_UW = 85, TS62_TUL = 95;

// Pressure sensors:
bool PS11_UL_active = 0, PS11_BBLW_active = 0, PS11_BBUW_active = 0;
bool PS12_TLW_active = 0, PS12_TUW_active = 0;
bool PS21_UL_active = 0, PS21_BBLW_active = 0, PS21_BBUW_active = 0;
bool PS22_TLW_active = 0, PS22_TUW_active = 0;
bool PS31_LW_active = 0, PS31_UW_active = 0, PS31_UL_active = 0;
bool PS41_TLL_active = 0, PS41_TLW_active = 0, PS41_TUW_active = 0, PS41_TUL_active = 0;
bool PS42_TLL_active = 0, PS42_TLW_active = 0, PS42_TUW_active = 0, PS42_TUL_active = 0;
bool PS51_TLL_active = 0, PS51_LW_active = 0, PS51_UW_active = 0;
bool PS_WATER_TLL_active = 0, PS_WATER_BBLW_active = 0, PS_WATER_BBUW_active = 0, PS_WATER_UL_active = 0;

// Thermocouples:
bool TS62_UW_active = 0, TS62_TUL_active = 0;

// Pressure sensors:
uint32_t PS11_UL_time = 0, PS11_BBLW_time = 0, PS11_BBUW_time = 0;
uint32_t PS12_TLW_time = 0, PS12_TUW_time = 0;
uint32_t PS21_UL_time = 0, PS21_BBLW_time = 0, PS21_BBUW_time = 0;
uint32_t PS22_TLW_time = 0, PS22_TUW_time = 0;
uint32_t PS31_LW_time = 0, PS31_UW_time = 0, PS31_UL_time = 0;
uint32_t PS41_TLL_time = 0, PS41_TLW_time = 0, PS41_TUW_time = 0, PS41_TUL_time = 0;
uint32_t PS42_TLL_time = 0, PS42_TLW_time = 0, PS42_TUW_time = 0, PS42_TUL_time = 0;
uint32_t PS51_TLL_time = 0, PS51_LW_time = 0, PS51_UW_time = 0;
uint32_t PS_WATER_TLL_time = 0, PS_WATER_BBLW_time = 0, PS_WATER_BBUW_time = 0, PS_WATER_UL_time = 0;

// Thermocouples:
uint32_t TS62_UW_time = 0, TS62_TUL_time = 0;

uint16_t message_delay = 2000;
// Pressure sensors:
uint32_t last_PS11_UL_msg, last_PS11_BBUW_msg, last_PS11_BBLW_msg;
uint32_t last_PS12_TUW_msg, last_PS12_TLW_msg;
uint32_t last_PS21_UL_msg, last_PS21_BBUW_msg, last_PS21_BBLW_msg;
uint32_t last_PS22_TUW_msg, last_PS22_TLW_msg;
uint32_t last_PS31_UL_msg, last_PS31_UW_msg, last_PS31_LW_msg;
uint32_t last_PS41_TUL_msg, last_PS41_TUW_msg, last_PS41_TLW_msg, last_PS41_TLL_msg;
uint32_t last_PS42_TUL_msg, last_PS42_TUW_msg, last_PS42_TLW_msg, last_PS42_TLL_msg;
uint32_t last_PS51_UW_msg, last_PS51_LW_msg, last_PS51_TLL_msg;
uint32_t last_PSWATER_UL_msg, last_PSWATER_BBUW_msg, last_PSWATER_BBLW_msg, last_PSWATER_TLL_msg;

// Thermocouples:
uint32_t last_TS62_UW_msg, last_TS62_TUL_msg;

// Save data 
bool state_file = false;
uint32_t time_since_save;
SdExFat sd;
ExFile fp;
uint32_t number = 1;

// ------------------------------ SETUP ----------------------------------------
void setupSensors() {


  Data.valvesState = valvePositions;

  // Thermocouples setup
  thermo12.begin();
  thermo41.begin();
  thermo42.begin();
  thermo61.begin();
  thermo62.begin();
  thermo11.begin();

  thermo12.setThermocoupleType(MAX31856_TCTYPE_K);
  thermo12.setConversionMode(MAX31856_ONESHOT_NOWAIT);

  thermo41.setThermocoupleType(MAX31856_TCTYPE_K);
  thermo41.setConversionMode(MAX31856_ONESHOT_NOWAIT);

  thermo42.setThermocoupleType(MAX31856_TCTYPE_K);
  thermo42.setConversionMode(MAX31856_ONESHOT_NOWAIT);

  thermo61.setThermocoupleType(MAX31856_TCTYPE_K);
  thermo61.setConversionMode(MAX31856_ONESHOT_NOWAIT);

  thermo62.setThermocoupleType(MAX31856_TCTYPE_K);
  thermo62.setConversionMode(MAX31856_ONESHOT_NOWAIT);

  thermo11.setThermocoupleType(MAX31856_TCTYPE_K);
  thermo11.setConversionMode(MAX31856_ONESHOT_NOWAIT);
}

void sendDataFromSensor(data* d) {
  //send_data((void*)d, sizeof(data));
}

void BBLoop() {
  // Reading the pressure sensors used for the Bang-Bang pressurization
  Data.PS11 = PS_25bar_reading(PS11_pin);
  Data.PS21 = PS_25bar_reading(PS21_pin);
  Data.PS61 = PS_25bar_reading(PS61_pin);
  Data.PS62 = PS_25bar_reading(PS62_pin);

  BB_pressurization(Data.PS11, Data.PS21, Data.PS61, Data.PS62); //bang-bang pressurization of the tanks if enabled
}

void sensorsLoop() {
  updateData();  //read the sensors
  // values_check();  //check if values are within limits
  BB_pressurization(Data.PS11, Data.PS21, Data.PS61, Data.PS62);  //bang-bang pressurization of the tanks if enabled
  Data.valvesState = valvePositions;
  serialSend();
  send_data(&Data,sizeof(data));   //send data to the ground station
  //if (Data.state == 1){save_data();}                                                    //save data to the SD card
  trigger_TS();  //requesting data from the thermocouples if not waiting for a conversion
}

void trigger_TS() {
  // requesting data from the thermocouples if not waiting for a conversion
  // fault11 = thermo11.readFault();
  // fault12 = thermo12.readFault();
  // fault41 = thermo41.readFault();
  // fault42 = thermo42.readFault();
  // fault61 = thermo61.readFault();
  // fault62 = thermo62.readFault();
  // if (fault11 & MAX31856_FAULT_OVUV) {
  //   cr0 = thermo11.readRegister8(MAX31856_CR0_REG);
  //   cr0 |= (1 << 1);  // Set bit 1: FAULTCLR
  //   thermo11.writeRegister8(MAX31856_CR0_REG, cr0);
  // }
  // if (fault12 & MAX31856_FAULT_OVUV) {
  //   cr0 = thermo12.readRegister8(MAX31856_CR0_REG);
  //   cr0 |= (1 << 1);  // Set bit 1: FAULTCLR
  //   thermo12.writeRegister8(MAX31856_CR0_REG, cr0);
  // }
  // if (fault41 & MAX31856_FAULT_OVUV) {
  //   cr0 = thermo41.readRegister8(MAX31856_CR0_REG);
  //   cr0 |= (1 << 1);  // Set bit 1: FAULTCLR
  //   thermo41.writeRegister8(MAX31856_CR0_REG, cr0);
  // }
  // if (fault42 & MAX31856_FAULT_OVUV) {
  //   cr0 = thermo42.readRegister8(MAX31856_CR0_REG);
  //   cr0 |= (1 << 1);  // Set bit 1: FAULTCLR
  //   thermo42.writeRegister8(MAX31856_CR0_REG, cr0);
  // }
  // if (fault61 & MAX31856_FAULT_OVUV) {
  //   cr0 = thermo61.readRegister8(MAX31856_CR0_REG);
  //   cr0 |= (1 << 1);  // Set bit 1: FAULTCLR
  //   thermo61.writeRegister8(MAX31856_CR0_REG, cr0);
  // }
  // if (fault62 & MAX31856_FAULT_OVUV) {
  //   cr0 = thermo62.readRegister8(MAX31856_CR0_REG);
  //   cr0 |= (1 << 1);  // Set bit 1: FAULTCLR
  //   thermo62.writeRegister8(MAX31856_CR0_REG, cr0);
  // }

  if (!TS12_waiting) {
    thermo12.triggerOneShot();
    TS12_waiting = 1;
  }
  if (!TS41_waiting) {
    thermo41.triggerOneShot();
    TS41_waiting = 1;
  }
  if (!TS42_waiting) {
    thermo42.triggerOneShot();
    TS42_waiting = 1;
  }
  if (!TS61_waiting) {
    thermo61.triggerOneShot();
    TS61_waiting = 1;
  }
  if (!TS62_waiting) {
    thermo62.triggerOneShot();
    TS62_waiting = 1;
  }
  if (!TS11_waiting) {
    thermo11.triggerOneShot();
    TS11_waiting = 1;
  }
}

void updateData() {
  Data.t = millis();
  //increment the packet ID
  Data.n++;

  // Read pressures and convert to mbar
  Data.PS11 = PS_25bar_reading(PS11_pin);
  Data.PS12 = PS_25bar_reading(PS12_pin);
  Data.PS21 = PS_25bar_reading(PS21_pin);
  Data.PS22 = PS_25bar_reading(PS22_pin);
  Data.PS31 = PS_70bar_reading(PS31_pin);
  Data.PS41 = PS_25bar_reading(PS41_pin);
  Data.PS42 = PS_25bar_reading(PS42_pin);
  Data.PS51 = PS_350bar_reading(PS51_pin);
  Data.PS61 = PS_25bar_reading(PS61_pin);
  Data.PS62 = PS_25bar_reading(PS62_pin);
  Data.PS63 = PS_25bar_reading(PS63_pin);
  Data.PS64 = PS_25bar_reading(PS64_pin);

  // Read 5V reference
  Data.ref5V = ref5V_reading(PSalim_pin);

  // Read load cell
  Data.LC = LC_reading(LC01_pin);

  // Read flow meters
  Data.FM11 = FM11_reading(FM11_pin);
  Data.FM21 = FM21_reading(FM21_pin);
  Data.FM61 = FM61_reading(FM61_pin);

  // getting data from the thermocouples if ready
  if (TS11_waiting && thermo11.conversionComplete()) {
    Data.TS11 = thermo11.readThermocoupleTemperature();
    TS11_waiting = 0;
  }
  if (TS12_waiting && thermo12.conversionComplete()) {
    Data.TS12 = thermo12.readThermocoupleTemperature();
    TS12_waiting = 0;
  }
  if (TS41_waiting && thermo41.conversionComplete()) {
    Data.TS41 = thermo41.readThermocoupleTemperature();
    TS41_waiting = 0;
  }
  if (TS42_waiting && thermo42.conversionComplete()) {
    Data.TS42 = thermo42.readThermocoupleTemperature();
    TS42_waiting = 0;
  }
  if (TS61_waiting && thermo61.conversionComplete()) {
    Data.TS61 = thermo61.readThermocoupleTemperature();
    TS61_waiting = 0;
  }
  if (TS62_waiting && thermo62.conversionComplete()) {
    Data.TS62 = thermo62.readThermocoupleTemperature();
    TS62_waiting = 0;
  }
}

void printFault(uint8_t fault) {
  if (fault) {
    if (fault & MAX31856_FAULT_CJRANGE) Serial.print("Cold Junction Range Fault");
    if (fault & MAX31856_FAULT_TCRANGE) Serial.print("Thermocouple Range Fault");
    if (fault & MAX31856_FAULT_CJHIGH) Serial.print("Cold Junction High Fault");
    if (fault & MAX31856_FAULT_CJLOW) Serial.print("Cold Junction Low Fault");
    if (fault & MAX31856_FAULT_TCHIGH) Serial.print("Thermocouple High Fault");
    if (fault & MAX31856_FAULT_TCLOW) Serial.print("Thermocouple Low Fault");
    if (fault & MAX31856_FAULT_OVUV) Serial.print("Over/Under Voltage Fault");
    if (fault & MAX31856_FAULT_OPEN) Serial.print("Thermocouple Open Fault");
  } else {
    Serial.print(fault);
  }
}

int16_t PS_25bar_reading(int pin) {  // For all pressure sensors except PS31 and PS51
  return (int16_t)(31250.0 * ((float)analogRead(pin) / 1023.0 - 0.1));
}

int32_t PS_70bar_reading(int pin) {  // For PS31
  // Serial.print("PS31: ");
  // Serial.print(analogRead(pin));
  // Serial.print("\t");
  return (int32_t)(87500.0 * ((float)analogRead(pin) / 1023.0 - 0.1));
}

int32_t PS_350bar_reading(int pin) {  // For PS51
  // Serial.print("PS51: ");
  // Serial.println(analogRead(pin));
  return (int32_t)(437500.0 * ((float)analogRead(pin) / 1023.0 - 0.1));
}

uint16_t FM11_reading(int pin) {
  // Serial.print("FM11: ");
  // Serial.print(analogRead(pin));
  // Serial.print("\t");
  return (uint16_t)((50000.0 * (float)analogRead(pin)) / (1023.0 * 60.0));
}

uint16_t FM21_reading(int pin) {
  // Serial.print("FM21: ");
  // Serial.print(analogRead(pin));
  Serial.print("\t");
  return (uint16_t)((40000.0 * (float)analogRead(pin)) / (1023.0 * 60.0));
}

uint16_t FM61_reading(int pin) {
  // Serial.print("FM61: ");
  // Serial.println(analogRead(pin));
  return (uint16_t)((150000.0 * (float)analogRead(pin)) / (1023.0 * 60.0));
}

int32_t LC_reading(int pin) {
  return 2943 * analogRead(pin) / 1023.0;
}

uint16_t ref5V_reading(int pin) {
  return analogRead(pin);
}

void values_check() {
  // check if values reached a warning or a limit
  // if so, start a timer (corresponding to the last time the value was in the bounds)
  // each time the sensors are read, check is the value is back in bounds (set the timer to zero) or is it is still out of bounds (let the timer run)
  // once the timer value is over the time limit: PS_oob_max_delay or TS_oob_max_delay
  // send a message, solve the problem (vent) or abort the test if necessary

  if (Data.PS11 >= PS11_UL) {
    if (PS11_UL_active == 1 && (millis() - PS11_UL_time) >= PS_oob_max_delay) {
      setValve(SV11, 1);  // open SV11
      setValve(SV33, 0);  // close SV33
      if ((millis() - last_PS11_UL_msg) >= message_delay) {
        send_string("error: PS11 over limit", 1);
        Serial.println("error: PS11 over limit");

        last_PS11_UL_msg = millis();
      }
    } else if (PS11_UL_active == 0) {
      PS11_UL_active = 1;
      PS11_UL_time = millis();
    }
  } else {
    PS11_UL_active = 0;
  }

  if (LOX_BB == 1 && Data.PS11 >= PS11_BBUW) {
    if (PS11_BBUW_active == 1 && (millis() - PS11_BBUW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS11_BBUW_msg) >= message_delay) {
        send_string("warning: PS11 too high in BB pressurization", 0);
        Serial.println("warning: PS11 too high in BB pressurization");

        last_PS11_BBUW_msg = millis();
      }
      
    } else if (PS11_BBUW_active == 0) {
      PS11_BBUW_active = 1;
      PS11_BBUW_time = millis();
    }
  } else {
    PS11_BBUW_active = 0;
  }

  if (LOX_BB == 1 && Data.PS11 <= PS11_BBLW) {
    if (PS11_BBLW_active == 1 && (millis() - PS11_BBLW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS11_BBLW_msg) >= message_delay) {
        send_string("warning: PS11 too low in BB pressurization", 0);
        Serial.println("warning: PS11 too low in BB pressurization");
        last_PS11_BBLW_msg = millis();
      }
    } else if (PS11_BBLW_active == 0) {
      PS11_BBLW_active = 1;
      PS11_BBLW_time = millis();
    }
  } else {
    PS11_BBLW_active = 0;
  }

  if (Data.state == 1 && Data.PS12 >= PS12_TUW) {
    if (PS12_TUW_active == 1 && (millis() - PS12_TUW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS12_TUW_msg) >= message_delay) {
        send_string("warning: PS12 too high", 0);
        Serial.println("warning: PS12 too high");
        last_PS12_TUW_msg = millis();
      }
    } else if (PS12_TUW_active == 0) {
      PS12_TUW_active = 1;
      PS12_TUW_time = millis();
    }
  } else {
    PS12_TUW_active = 0;
  }

  if (Data.state == 1 && Data.PS12 <= PS12_TLW) {
    if (PS12_TLW_active == 1 && (millis() - PS12_TLW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS12_TLW_msg) >= message_delay) {
        send_string("warning: PS12 too low", 0);
        Serial.println("warning: PS12 too low");
        last_PS12_TLW_msg = millis();
      }
    } else if (PS12_TLW_active == 0) {
      PS12_TLW_active = 1;
      PS12_TLW_time = millis();
    }
  } else {
    PS12_TLW_active = 0;
  }

  if (Data.PS21 >= PS21_UL) {
    if (PS21_UL_active == 1 && (millis() - PS21_UL_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS21_UL_msg) >= message_delay) {
        setValve(SV21, 1);
        setValve(SV34, 0);
        send_string("error: PS21 over limit", 1);
        Serial.println("error: PS21 over limit");
        last_PS21_UL_msg = millis();
      }
    } else if (PS21_UL_active == 0) {
      PS21_UL_active = 1;
      PS21_UL_time = millis();
    }
  } else {
    PS21_UL_active = 0;
  }

  if (ETH_BB == 1 && Data.PS21 >= PS21_BBUW) {
    if (PS21_BBUW_active == 1 && (millis() - PS21_BBUW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS21_BBUW_msg) >= message_delay) {
        send_string("warning: PS21 too high in BB pressurization", 0);
        Serial.println("warning: PS21 too high in BB pressurization");
        last_PS21_BBUW_msg = millis();
      }
    } else if (PS21_BBUW_active == 0) {
      PS21_BBUW_active = 1;
      PS21_BBUW_time = millis();
    }
  } else {
    PS21_BBUW_active = 0;
  }

  if (ETH_BB == 1 && Data.PS21 <= PS21_BBLW) {
    if (PS21_BBLW_active == 1 && (millis() - PS21_BBLW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS21_BBLW_msg) >= message_delay) {
        send_string("warning: PS21 too low in BB pressurization", 0);
        Serial.println("warning: PS21 too low in BB pressurization");
        last_PS21_BBLW_msg = millis();
      }
    } else if (PS21_BBLW_active == 0) {
      PS21_BBLW_active = 1;
      PS21_BBLW_time = millis();
    }
  } else {
    PS21_BBLW_active = 0;
  }

  if (Data.state == 1 && Data.PS22 >= PS22_TUW) {
    if (PS22_TUW_active == 1 && (millis() - PS22_TUW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS22_TUW_msg) >= message_delay) {
        send_string("warning: PS22 too high", 0);
        Serial.println("warning: PS22 too high");
        last_PS22_TUW_msg = millis();
      }
    } else if (PS22_TUW_active == 0) {
      PS22_TUW_active = 1;
      PS22_TUW_time = millis();
    }
  } else {
    PS22_TUW_active = 0;
  }

  if (Data.state == 1 && Data.PS22 <= PS22_TLW) {
    if (PS22_TLW_active == 1 && (millis() - PS22_TLW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS22_TLW_msg) >= message_delay) {
        send_string("warning: PS22 too low", 0);
        Serial.println("warning: PS22 too low");
        last_PS22_TLW_msg = millis();
      }
    } else if (PS22_TLW_active == 0) {
      PS22_TLW_active = 1;
      PS22_TLW_time = millis();
    }
  } else {
    PS22_TLW_active = 0;
  }

  if (Data.PS31 >= PS31_UL) {
    if (PS31_UL_active == 1 && (millis() - PS31_UL_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS31_UL_msg) >= message_delay) {
        setValve(SV32, 1);
        send_string("error: PS31 over limit", 0);
        Serial.println("error: PS31 over limit");
        last_PS31_UL_msg = millis();
      }
    } else if (PS31_UL_active == 0) {
      PS31_UL_active = 1;
      PS31_UL_time = millis();
    }
  } else {
    PS31_UL_active = 0;
  }

  if (Data.PS31 >= PS31_UW) {
    if (PS31_UW_active == 1 && (millis() - PS31_UW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS31_UW_msg) >= message_delay) {
        send_string("warning: PS31 too high", 0);
        Serial.println("warning: PS31 too high");
        last_PS31_UW_msg = millis();
      }
    } else if (PS31_UW_active == 0) {
      PS31_UW_active = 1;
      PS31_UW_time = millis();
    }
  } else {
    PS31_UW_active = 0;
  }

  if (Data.PS31 <= PS31_LW) {
    if (PS31_LW_active == 1 && (millis() - PS31_LW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS31_LW_msg) >= message_delay) {
        send_string("warning: PS31 too low", 0);
        Serial.println("warning: PS31 too low");
        last_PS31_LW_msg = millis();
      }
    } else if (PS31_LW_active == 0) {
      PS31_LW_active = 1;
      PS31_LW_time = millis();
    }
  } else {
    PS31_LW_active = 0;
  }

  if (Data.state == 1 && Data.PS41 >= PS41_TUL) {
    if (PS41_TUL_active == 1 && (millis() - PS41_TUL_time) >= PS_oob_max_delay) {
      // emergency_stop();
      if ((millis() - last_PS41_TUL_msg) >= message_delay) {
        send_string("error: PS41 over limit - test aborted", 1);
        Serial.println("error: PS41 over limit - test aborted");
        last_PS41_TUL_msg = millis();
      }
    } else if (PS41_TUL_active == 0) {
      PS41_TUL_active = 1;
      PS41_TUL_time = millis();
    }
  } else {
    PS41_TUL_active = 0;
  }
  
  if (Data.state == 1 && Data.PS41 >= PS41_TUW) {
    if (PS41_TUW_active == 1 && (millis() - PS41_TUW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS41_TUW_msg) >= message_delay) {
        send_string("warning: PS41 too high", 0);
        Serial.println("warning: PS41 too high");
        last_PS41_TUW_msg = millis();
      }
    } else if (PS41_TUW_active == 0) {
      PS41_TUW_active = 1;
      PS41_TUW_time = millis();
    }
  } else {
    PS41_TUW_active = 0;
  }
  
  if (Data.state == 1 && Data.PS41 <= PS41_TLW) {
    if (PS41_TLW_active == 1 && (millis() - PS41_TLW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS41_TLW_msg) >= message_delay) {
        send_string("warning: PS41 too low", 0);
        Serial.println("warning: PS41 too low");
        last_PS41_TLW_msg = millis();
      }
    } else if (PS41_TLW_active == 0) {
      PS41_TLW_active = 1;
      PS41_TLW_time = millis();
    }
  } else {
    PS41_TLW_active = 0;
  }
  
  if (Data.state == 1 && Data.PS41 <= PS41_TLL) {
    if (PS41_TLL_active == 1 && (millis() - PS41_TLL_time) >= PS_oob_max_delay) {
      // emergency_stop();
      if ((millis() - last_PS41_TLL_msg) >= message_delay) {
        send_string("error: PS41 below limit - test aborted", 1);
        Serial.println("error: PS41 below limit - test aborted");
        last_PS41_TLL_msg = millis();
      }
    } else if (PS41_TLL_active == 0) {
      PS41_TLL_active = 1;
      PS41_TLL_time = millis();
    }
  } else {
    PS41_TLL_active = 0;
  }
  
  if (Data.state == 1 && Data.PS42 >= PS42_TUL) {
    if (PS42_TUL_active == 1 && (millis() - PS42_TUL_time) >= PS_oob_max_delay) {
      // emergency_stop();
      if ((millis() - last_PS42_TUL_msg) >= message_delay) {
        send_string("error: PS42 over limit - test aborted", 1);
        Serial.println("error: PS42 over limit - test aborted");
        last_PS42_TUL_msg = millis();
      }
    } else if (PS42_TUL_active == 0) {
      PS42_TUL_active = 1;
      PS42_TUL_time = millis();
    }
  } else {
    PS42_TUL_active = 0;
  }
  
  if (Data.state == 1 && Data.PS42 >= PS42_TUW) {
    if (PS42_TUW_active == 1 && (millis() - PS42_TUW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS42_TUW_msg) >= message_delay) {
        send_string("warning: PS42 too high", 0);
        Serial.println("warning: PS42 too high");
        last_PS42_TUW_msg = millis();
      }
    } else if (PS42_TUW_active == 0) {
      PS42_TUW_active = 1;
      PS42_TUW_time = millis();
    }
  } else {
    PS42_TUW_active = 0;
  }
  
  if (Data.state == 1 && Data.PS42 <= PS42_TLW) {
    if (PS42_TLW_active == 1 && (millis() - PS42_TLW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS42_TLW_msg) >= message_delay) {
        send_string("warning: PS42 too low", 0);
        Serial.println("warning: PS42 too low");
        last_PS42_TLW_msg = millis();
      }
    } else if (PS42_TLW_active == 0) {
      PS42_TLW_active = 1;
      PS42_TLW_time = millis();
    }
  } else {
    PS42_TLW_active = 0;
  }
  
  if (Data.state == 1 && Data.PS42 <= PS42_TLL) {
    if (PS42_TLL_active == 1 && (millis() - PS42_TLL_time) >= PS_oob_max_delay) {
      // emergency_stop();
      if ((millis() - last_PS42_TLL_msg) >= message_delay) {
        send_string("error: PS42 below limit - test aborted", 1);
        Serial.println("error: PS42 below limit - test aborted");
        last_PS42_TLL_msg = millis();
      }
    } else if (PS42_TLL_active == 0) {
      PS42_TLL_active = 1;
      PS42_TLL_time = millis();
    }
  } else {
    PS42_TLL_active = 0;
  }
  
  if (Data.PS51 >= PS51_UW) {
    if (PS51_UW_active == 1 && (millis() - PS51_UW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS51_UW_msg) >= message_delay) {
        send_string("warning: PS51 too high", 0);
        Serial.println("warning: PS51 too high");
        last_PS51_UW_msg = millis();
      }
    } else if (PS51_UW_active == 0) {
      PS51_UW_active = 1;
      PS51_UW_time = millis();
    }
  } else {
    PS51_UW_active = 0;
  }
  
  if (Data.PS51 <= PS51_LW) {
    if (PS51_LW_active == 1 && (millis() - PS51_LW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PS51_LW_msg) >= message_delay) {
        send_string("warning: PS51 too low", 0);
        Serial.println("warning: PS51 too low");
        last_PS51_LW_msg = millis();
      }
    } else if (PS51_LW_active == 0) {
      PS51_LW_active = 1;
      PS51_LW_time = millis();
    }
  } else {
    PS51_LW_active = 0;
  }
  
  if (Data.state == 1 && Data.test_cooling == 1 && Data.PS51 <= PS51_TLL) {
    if (PS51_TLL_active == 1 && (millis() - PS51_TLL_time) >= PS_oob_max_delay) {
      // emergency_stop();
      if ((millis() - last_PS51_TLL_msg) >= message_delay) {
        send_string("error: PS51 below limit - test aborted", 1);
        Serial.println("error: PS51 below limit - test aborted");
        last_PS51_TLL_msg = millis();
      }
    } else if (PS51_TLL_active == 0) {
      PS51_TLL_active = 1;
      PS51_TLL_time = millis();
    }
  } else {
    PS51_TLL_active = 0;
  }
  
  if ((Data.PS61 >= PS_WATER_UL) || (Data.PS62 >= PS_WATER_UL)) {
    if (PS_WATER_UL_active == 1 && (millis() - PS_WATER_UL_time) >= PS_oob_max_delay) {
      setValve(SV61, 1);  //open SV61
      setValve(SV62, 1);  //open SV62
      setValve(SV52, 0);  //close SV52
      setValve(SV53, 0);  //close SV53
      if ((millis() - last_PSWATER_UL_msg) >= message_delay) {
        send_string("error: PS_WATER over limit", 1);
        Serial.println("error: PS_WATER over limit");
        last_PSWATER_UL_msg = millis();
      }
    } else if (PS_WATER_UL_active == 0) {
      PS_WATER_UL_active = 1;
      PS_WATER_UL_time = millis();
    }
  } else {
    PS_WATER_UL_active = 0;
  }

  if (WATER_BB == 1 && ((Data.PS61 >= WATER_BBUW) || (Data.PS62 >= WATER_BBUW))) {
    if (PS_WATER_BBUW_active == 1 && (millis() - PS_WATER_BBUW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PSWATER_BBUW_msg) >= message_delay) {
        send_string("warning: PS_WATER too high in BB pressurization", 0);
        Serial.println("warning: PS_WATER too high in BB pressurization");
        last_PSWATER_BBUW_msg = millis();
      }
      
    } else if (PS_WATER_BBUW_active == 0) {
      PS_WATER_BBUW_active = 1;
      PS_WATER_BBUW_time = millis();
    }
  } else {
    PS_WATER_BBUW_active = 0;
  }
  if (WATER_BB == 1 && ((Data.PS61 <= WATER_BBLW) || (Data.PS62 <= WATER_BBLW))) {
    if (PS_WATER_BBLW_active == 1 && (millis() - PS_WATER_BBLW_time) >= PS_oob_max_delay) {
      if ((millis() - last_PSWATER_BBLW_msg) >= message_delay) {
        send_string("warning: PS_WATER too low in BB pressurization", 0);
        Serial.println("warning: PS_WATER too low in BB pressurization");
        last_PSWATER_BBLW_msg = millis();
      }
      
    } else if (PS_WATER_BBLW_active == 0) {
      PS_WATER_BBLW_active = 1;
      PS_WATER_BBLW_time = millis();
    }
  } else {
    PS_WATER_BBLW_active = 0;
  }
  
  if (Data.state == 1 && Data.test_cooling == 1 && ((Data.PS61 <= PS_WATER_TLL) || (Data.PS62 <= PS_WATER_TLL))) {
    if (PS_WATER_TLL_active == 1 && (millis() - PS_WATER_TLL_time) >= PS_oob_max_delay) {
      setValve(SV52, 1);  //open SV52
      setValve(SV53, 1);  //open SV53
      setValve(SV61, 0);  //close SV61
      setValve(SV62, 0);  //close SV62
      if ((millis() - last_PSWATER_TLL_msg) >= message_delay) {
        send_string("error: PS_WATER below limit", 1);
        Serial.println("error: PS_WATER below limit");
        last_PSWATER_TLL_msg = millis();
      }
      
    } else if (PS_WATER_TLL_active == 0) {
      PS_WATER_TLL_active = 1;
      PS_WATER_TLL_time = millis();
    }
  } else {
    PS_WATER_TLL_active = 0;
  }
  
  if (Data.TS62*10 >= TS62_UW) {
    if (TS62_UW_active == 1 && (millis() - TS62_UW_time) >= PS_oob_max_delay) {
      if ((millis() - last_TS62_UW_msg) >= message_delay) {
        send_string("warning: TS62 too high", 0);
        Serial.println("warning: TS62 too high");
        last_TS62_UW_msg = millis();
      }
      
    } else if (TS62_UW_active == 0) {
      TS62_UW_active = 1;
      TS62_UW_time = millis();
    }
  } else {
    TS62_UW_active = 0;
  }

  if (Data.state == 1 && Data.test_cooling == 1 && Data.TS62*10 >= TS62_TUL) {
    if (TS62_TUL_active == 1 && (millis() - TS62_TUL_time) >= PS_oob_max_delay) {
      // emergency_stop();       // stops the test and puts the testbench in a safe configuration
      
      if ((millis() - last_TS62_TUL_msg) >= message_delay) {
        send_string("error: TS62 over limit - test aborted",1);
        Serial.println("error: TS62 over limit - test aborted");
        last_TS62_TUL_msg = millis();
      }
    } else if (TS62_TUL_active == 0) {
      TS62_TUL_active = 1;
      TS62_TUL_time = millis();
    }
  } else {
    TS62_TUL_active = 0;
  }
  
}

// void abort() {
//   if (Data.state == 1) {
//     if (Data.test_step < 9 && Sequence_data.cooling_enable == true){
//       setValve(SV63, 0);  //close SV63
//     }
//     if (Data.test_step >= 9){
      
//     }
//   }

// }

void serialSend() {
  Serial.println("------ Sensor Data ------");

  Serial.print("Packet ID: ");
  Serial.println(Data.n);
  Serial.print("Timestamp (ms): ");
  Serial.println(Data.t);

  Serial.println("Pressure Sensors (mbar):");
  Serial.print("PS11: ");
  Serial.print(Data.PS11);
  Serial.print("\t");
  Serial.print("PS12: ");
  Serial.print(Data.PS12);
  Serial.print("\t");
  Serial.print("PS21: ");
  Serial.print(Data.PS21);
  Serial.print("\t");
  Serial.print("PS22: ");
  Serial.print(Data.PS22);
  Serial.print("\t");
  Serial.print("PS31: ");
  Serial.print(Data.PS31);
  Serial.print("\t");
  Serial.print("PS41: ");
  Serial.print(Data.PS41);
  Serial.print("\t");
  Serial.print("PS42: ");
  Serial.print(Data.PS42);
  Serial.print("\t");
  Serial.print("PS51: ");
  Serial.print(Data.PS51);
  Serial.print("\t");
  Serial.print("PS61: ");
  Serial.print(Data.PS61);
  Serial.print("\t");
  Serial.print("PS62: ");
  Serial.print(Data.PS62);
  Serial.print("\t");
  Serial.print("PS63: ");
  Serial.print(Data.PS63);
  Serial.print("\t");
  Serial.print("PS64: ");
  Serial.println(Data.PS64);

  Serial.println("Thermocouples (°C):");
  Serial.print("TS11: ");
  Serial.print(Data.TS11);
  Serial.print("\t");
  Serial.print("TS12: ");
  Serial.print(Data.TS12);
  Serial.print("\t");
  Serial.print("TS41: ");
  Serial.print(Data.TS41);
  Serial.print("\t");
  Serial.print("TS42: ");
  Serial.print(Data.TS42);
  Serial.print("\t");
  Serial.print("TS61: ");
  Serial.print(Data.TS61);
  Serial.print("\t");
  Serial.print("TS62: ");
  Serial.println(Data.TS62);

  Serial.println("Thermocouples faults:");
  Serial.print("TS11: ");
  printFault(thermo11.readFault());
  Serial.print("\t");
  Serial.print("TS12: ");
  printFault(thermo12.readFault());
  Serial.print("\t");
  Serial.print("TS41: ");
  printFault(thermo41.readFault());
  Serial.print("\t");
  Serial.print("TS42: ");
  printFault(thermo42.readFault());
  Serial.print("\t");
  Serial.print("TS61: ");
  printFault(thermo61.readFault());
  Serial.print("\t");
  Serial.print("TS62: ");
  printFault(thermo62.readFault());
  Serial.println("");

  Serial.println("Flow Meters (mL/s):");
  Serial.print("FM11: ");
  Serial.print(Data.FM11);
  Serial.print("\t");
  Serial.print("FM21: ");
  Serial.print(Data.FM21);
  Serial.print("\t");
  Serial.print("FM61: ");
  Serial.println(Data.FM61);

  Serial.print("Load Cell (N): ");
  Serial.println(Data.LC);
  Serial.print("5V Reference (mV): ");
  Serial.println(Data.ref5V);

  Serial.println("--------------------------");
  Serial.println();
}


void setupSaveData() {
  Serial.println("Initialisation du stockage SD...");
  if (!sd.begin(SdioConfig(FIFO_SDIO))) {
    Serial.println("Erreur : Carte SD non détectée !");
  }
  Serial.println("Stockage SD Initialisé");
}

void save_data() {
  String line = String(Data.n) + "," +
              String(Data.t) + "," +
              String(Data.PS11) + "," + String(Data.PS12) + "," + String(Data.PS21) + "," +
              String(Data.PS22) + "," + String(Data.PS31) + "," + String(Data.PS41) + "," +
              String(Data.PS42) + "," + String(Data.PS51) + "," + String(Data.PS61) + "," +
              String(Data.PS62) + "," + String(Data.PS63) + "," + String(Data.PS64) + "," +

              String(Data.TS11) + "," + String(Data.TS12) + "," + String(Data.TS41) + "," +
              String(Data.TS42) + "," + String(Data.TS61) + "," + String(Data.TS62) + "," +

              String(Data.FM11) + "," + String(Data.FM21) + "," + String(Data.FM61) + "," +

              String(Data.LC) + "," +
              String(Data.ref5V) + "," +

              String(Data.valvesState) + "," +
              String(Data.actLPos) + "," + String(Data.actRPos) + "," +
              String(Data.actLOK) + "," + String(Data.actROK) + "," +

              String(Data.state) + "," + String(Data.test_step) + "," +
              String(Data.test_cooling ? 1 : 0);  // bool en int

  if (state_file == false) {
    state_file = true;
    if (Data.n >= (number + 1000) ){
      number = Data.n;
    }
    // check if have the good ID
    String fileID = String(number) + ".txt";
    fp = sd.open(fileID, FILE_WRITE);
    fp.println(line);
    time_since_save = millis();
  } else if (state_file == true) {
    if (static_cast<uint32_t>(millis() - time_since_save) >= frequence_save) {
      fp.println(line);
      fp.close();
      state_file = false;
    } else if (static_cast<uint32_t>(millis() - time_since_save) < frequence_save) {
      fp.println(line);
    }
  }
}

