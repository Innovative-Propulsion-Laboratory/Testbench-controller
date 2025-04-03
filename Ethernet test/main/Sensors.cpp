#include "Sensors.h"

// --------------------- VARIABLES INITIALIZATION ------------------------------

uint32_t n = 0; //Packet ID

data Data;

bool TS31_waiting = 0,TS41_waiting = 0,TS42_waiting = 0,TS61_waiting= 0 ,TS62_waiting = 0,TS11_waiting = 0;

Adafruit_MAX31856 thermo31 = Adafruit_MAX31856(TS31_pin, 11, 12, 13);
Adafruit_MAX31856 thermo41 = Adafruit_MAX31856(TS41_pin, 11, 12, 13);
Adafruit_MAX31856 thermo42 = Adafruit_MAX31856(TS42_pin, 11, 12, 13);
Adafruit_MAX31856 thermo61 = Adafruit_MAX31856(TS61_pin, 11, 12, 13);
Adafruit_MAX31856 thermo62 = Adafruit_MAX31856(TS62_pin, 11, 12, 13);
Adafruit_MAX31856 thermo11 = Adafruit_MAX31856(TS11_pin, 11, 12, 13); //initialized last since it may return negative values

// ------------------------- LIMITS DEFINITION ---------------------------------

uint16_t PS_oob_max_delay = 250;  // defines the maximum duration for which a pressure sensor value can stay out of bounds in ms
uint16_t TS_oob_max_delay = 1000; // defines the maximum duration for which a temperature sensor value can stay out of bounds in ms

// Limits are defined as: LL: Lower Limit, LW: Lower Warning, N: Nominal, UW: Upper Warning, UL: Upper Limit
// A "T" is added if the value is only checked during tests
// A "BB" is added if the value is only checked if the bang-bang pressurization of the concered tank is active -- in the "pressurization.ino" file
// If a warning is reached, a message is sent to the computer
// If a limit is reached, the microcontroller takes action to solve the problem or to put the testbench in a safe position

// Pressure sensors:
uint16_t PS11_UL = 25000;
uint16_t PS12_TLW = 9000,  PS12_TUW = 13000;
uint16_t PS21_UL = 25000;
uint16_t PS22_TLW = 9000,  PS22_TUW = 13000;
uint16_t PS31_LW = 20000,  PS31_UW = 50000,  PS31_UL = 55000;
uint16_t PS41_TLL = 6000,  PS41_TLW = 7000,  PS41_TUW = 13000, PS41_TUL = 14000;
uint16_t PS42_TLL = 6000,  PS42_TLW = 7000,  PS42_TUW = 13000, PS42_TUL = 14000;
uint16_t PS51_TLL = 10000, PS51_LW = 40000; uint32_t PS51_UW = 210000;
uint16_t PS_WATER_TLL = 6000,  PS_WATER_UL = 14000;

// Thermocouples:
uint16_t TS31_UW = 45;
uint16_t TS62_UW = 85, TS62_TUL = 95;

// Pressure sensors:
bool PS11_UL_active = 0,  PS11_BBLW_active = 0, PS11_BBUW_active = 0;
bool PS12_TLW_active = 0, PS12_TUW_active = 0;
bool PS21_UL_active = 0, PS21_BBLW_active = 0, PS21_BBUW_active = 0;
bool PS22_TLW_active = 0, PS22_TUW_active = 0;
bool PS31_LW_active = 0,  PS31_UW_active = 0,  PS31_UL_active = 0;
bool PS41_TLL_active = 0, PS41_TLW_active = 0, PS41_TUW_active = 0, PS41_TUL_active = 0;
bool PS42_TLL_active = 0, PS42_TLW_active = 0, PS42_TUW_active = 0, PS42_TUL_active = 0;
bool PS51_TLL_active = 0, PS51_LW_active = 0,  PS51_UW_active = 0;
bool PS_WATER_TLL_active = 0, PS_WATER_BBLW_active = 0, PS_WATER_BBUW_active = 0, PS_WATER_UL_active = 0;

// Thermocouples:
bool TS31_UW_active = 0;
bool TS62_UW_active = 0, TS62_TUL_active = 0;

// Pressure sensors:
uint32_t PS11_UL_time = 0,  PS11_BBLW_time = 0, PS11_BBUW_time = 0;
uint32_t PS12_TLW_time = 0, PS12_TUW_time = 0;
uint32_t PS21_UL_time = 0, PS21_BBLW_time = 0, PS21_BBUW_time = 0;
uint32_t PS22_TLW_time = 0, PS22_TUW_time = 0;
uint32_t PS31_LW_time = 0,  PS31_UW_time = 0,  PS31_UL_time = 0;
uint32_t PS41_TLL_time = 0, PS41_TLW_time = 0, PS41_TUW_time = 0, PS41_TUL_time = 0;
uint32_t PS42_TLL_time = 0, PS42_TLW_time = 0, PS42_TUW_time = 0, PS42_TUL_time = 0;
uint32_t PS51_TLL_time = 0, PS51_LW_time = 0,  PS51_UW_time = 0;
uint32_t PS_WATER_TLL_time = 0, PS_WATER_BBLW_time = 0, PS_WATER_BBUW_time = 0, PS_WATER_UL_time = 0;

// Thermocouples:
uint32_t TS31_UW_time = 0;
uint32_t TS62_UW_time = 0, TS62_TUL_time = 0;

// ------------------------------ SETUP ----------------------------------------
void setupSensors(){


    // Data.valveState = &valvePositions;

    // Thermocouples setup
    thermo31.begin();
    thermo41.begin();
    thermo42.begin();
    thermo61.begin();
    thermo62.begin();
    thermo11.begin();

    thermo31.setThermocoupleType(MAX31856_TCTYPE_K);
    thermo31.setConversionMode(MAX31856_ONESHOT_NOWAIT);

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
    send_data((void*)d, sizeof(data));
}

void BBLoop(){
    // Reading the pressure sensors used for the Bang-Bang pressurization
    Data.PS11 = PS_25bar_reading(PS11_pin);
    Data.PS21 = PS_25bar_reading(PS21_pin);
    Data.PS61 = PS_25bar_reading(PS61_pin);
    Data.PS62 = PS_25bar_reading(PS62_pin);

    // BB_pressurization(Data.PS11, Data.PS21, Data.PS61, Data.PS62); //bang-bang pressurization of the tanks if enabled
}

void sensorsLoop(){
  delay(1000);
    updateData();                                                   //read the sensors
    // values_check();                                                 //check if values are within limits
    // BB_pressurization(Data.PS11, Data.PS21, Data.PS61, Data.PS62);  //bang-bang pressurization of the tanks if enabled
    Data.valvesState = valvePositions;
    // Serial.println(Data.valvesState);
    send_data(&Data, sizeof(data));                                  //send data to the ground station
    // save_data();                                                    //save data to the SD card
    trigger_TS();                                                   //requesting data from the thermocouples if not waiting for a conversion
}

void trigger_TS() {
     // requesting data from the thermocouples if not waiting for a conversion
     if (!TS41_waiting){thermo41.triggerOneShot(); TS41_waiting = 1;}
     if (!TS42_waiting){thermo42.triggerOneShot(); TS42_waiting = 1;}
     if (!TS61_waiting){thermo61.triggerOneShot(); TS61_waiting = 1;}
     if (!TS62_waiting){thermo62.triggerOneShot(); TS62_waiting = 1;}
     if (!TS11_waiting){thermo11.triggerOneShot(); TS11_waiting = 1;}
}

void updateData(){
    Data.t = millis();
    //increment the packet ID
    Data.n ++;
    
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
    Data.LC = LC_reading(LC01_pin)*10;

    // Read flow meters
    Data.FM11 = FM11_reading(FM11_pin);
    Data.FM21 = FM21_reading(FM21_pin);
    Data.FM61 = FM61_reading(FM61_pin);

    // getting data from the thermocouples if ready
    if (TS11_waiting && thermo11.conversionComplete()) {
        Data.TS11 = (thermo11.readThermocoupleTemperature())*10;
        TS11_waiting = 0;
    }
    if (TS31_waiting && thermo31.conversionComplete()) {
        Data.TS31 = (thermo31.readThermocoupleTemperature())*10;
        TS31_waiting = 0;
    }
    if (TS41_waiting && thermo41.conversionComplete()) {
        Data.TS41 = (thermo41.readThermocoupleTemperature())*10;
        TS41_waiting = 0;
    }
    if (TS42_waiting && thermo42.conversionComplete()) {
        Data.TS42 = (thermo42.readThermocoupleTemperature())*10;
        TS42_waiting = 0;
    }
    if (TS61_waiting && thermo61.conversionComplete()) {
        Data.TS61 = (thermo61.readThermocoupleTemperature())*10;
        TS61_waiting = 0;
    }
    if (TS62_waiting && thermo62.conversionComplete()) {
        Data.TS62 = (thermo62.readThermocoupleTemperature())*10;
        TS62_waiting = 0;
    }
}

uint16_t PS_25bar_reading(int pin) {  // For all pressure sensors except PS31 and PS51
    return (uint16_t)(31250.0 * ((float)analogRead(pin) / 1023.0 - 0.1));
}

uint16_t PS_70bar_reading(int pin) {  // For PS31
    return (uint16_t)(87500.0 * ((float)analogRead(pin) / 1023.0 - 0.1)); 
}

uint16_t PS_350bar_reading(int pin) {  // For PS51
    return (uint16_t)(437500.0 * ((float)analogRead(pin) / 1023.0 - 0.1)); 
}

uint16_t FM11_reading(int pin) {
    return (uint16_t)((50000.0 * (float)analogRead(pin)) / (1023.0 * 60.0));
}

uint16_t FM21_reading(int pin) {
    return (uint16_t)((40000.0 * (float)analogRead(pin)) / (1023.0 * 60.0));
}

uint16_t FM61_reading(int pin) {
    return (uint16_t)((150000.0 * (float)analogRead(pin)) / (1023.0 * 60.0));
}

float LC_reading(int pin){
    return 2943*analogRead(pin)/1023.0; 
}

uint16_t ref5V_reading(int pin){
    return analogRead(pin);
}

void values_check(){
    // check if values reached a warning or a limit
    // if so, start a timer (corresponding to the last time the value was in the bounds)
    // each time the sensors are read, check is the value is back in bounds (set the timer to zero) or is it is still out of bounds (let the timer run)
    // once the timer value is over the time limit: PS_oob_max_delay or TS_oob_max_delay
    // send a message, solve the problem (vent) or abort the test if necessary

    if (Data.PS11 >= PS11_UL){
        if (PS11_UL_active == 1 && (millis()-PS11_UL_time) >= PS_oob_max_delay){
            setValve(SV11, 1);          // open SV11
            setValve(SV33, 0);          // close SV33

            // reply "error: PS11 over limit"
        }
        else if(PS11_UL_active == 0) {
            PS11_UL_active = 1; 
            PS11_UL_time = millis();
        }
    }
    else {PS11_UL_active = 0;}
//    if (LOX_BB == 1 && Data.PS11 >= PS11_BBUW) {
//        if (PS11_BBUW_active == 1 && (millis()-PS11_BBUW_time) >= PS_oob_max_delay){
//            // reply "warning: PS11 too high in BB pressurization"
//        }
//        else if(PS11_BBUW_active == 0) {
//            PS11_BBUW_active = 1; 
//            PS11_BBUW_time = millis();
//        }
//    }
//    else {PS11_BBUW_active = 0;}
//    if (LOX_BB == 1 && Data.PS11 <= PS11_BBLW) {
//        if (PS11_BBLW_active == 1 && (millis()-PS11_BBLW_time) >= PS_oob_max_delay){
//            // reply "warning: PS11 too low in BB pressurization"
//        }
//        else if(PS11_BBLW_active == 0) {
            PS11_BBLW_active = 1; 
            PS11_BBLW_time = millis();
//        }
//    }
//    else {PS11_BBLW_active = 0;}

    if (Data.state == 1 && Data.PS12 >= PS12_TUW) {
        if (PS12_TUW_active == 1 && (millis()-PS12_TUW_time) >= PS_oob_max_delay){
            // reply "warning: PS12 too high"
        }
        else if(PS12_TUW_active == 0) {
            PS12_TUW_active = 1; 
            PS12_TUW_time = millis();
        }
    }
    else {PS12_TUW_active = 0;}
    if (Data.state == 1 && Data.PS12 <= PS12_TLW) {
        if (PS12_TLW_active == 1 && (millis()-PS12_TLW_time) >= PS_oob_max_delay){
            // reply "warning: PS12 too low"
        }
        else if(PS12_TLW_active == 0) {
            PS12_TLW_active = 1; 
            PS12_TLW_time = millis();
        }
    }
    else {PS12_TLW_active = 0;}

    if (Data.PS21 >= PS21_UL){
        if (PS21_UL_active == 1 && (millis()-PS21_UL_time) >= PS_oob_max_delay){
            setValve(SV21, 1);          // open SV21
            setValve(SV34, 0);          // close SV34

            // reply "error: PS21 over limit"
        }
        else if(PS21_UL_active == 0) {
            PS21_UL_active = 1; 
            PS21_UL_time = millis();
        }
    }
    else {PS21_UL_active = 0;}
//    if (ETH_BB == 1 && Data.PS21 >= PS21_BBUW) {
//        if (PS21_BBUW_active == 1 && (millis()-PS21_BBUW_time) >= PS_oob_max_delay){
//            // reply "warning: PS21 too high in BB pressurization"
//        }
//        else if(PS21_BBUW_active == 0) {
//            PS21_BBUW_active = 1; 
//            PS21_BBUW_time = millis();
//        }
//    }
//    else {PS21_BBUW_active = 0;}
//    if (ETH_BB == 1 && Data.PS21 <= PS21_BBLW) {
//        if (PS21_BBLW_active == 1 && (millis()-PS21_BBLW_time) >= PS_oob_max_delay){
//            // reply "warning: PS21 too low in BB pressurization"
//        }
//        else if(PS21_BBLW_active == 0) {
//            PS21_BBLW_active = 1; 
//            PS21_BBLW_time = millis();
//        }
//    }
//    else {PS21_BBLW_active = 0;}

    if (Data.state == 1 && Data.PS22 >= PS22_TUW) {
        if (PS22_TUW_active == 1 && (millis()-PS22_TUW_time) >= PS_oob_max_delay){
            // reply "warning: PS22 too high"
        }
        else if(PS22_TUW_active == 0) {
            PS22_TUW_active = 1; 
            PS22_TUW_time = millis();
        }
    }
    else {PS22_TUW_active = 0;}
    if (Data.state == 1 && Data.PS22 <= PS22_TLW) {
        if (PS22_TLW_active == 1 && (millis()-PS22_TLW_time) >= PS_oob_max_delay){
            // reply "warning: PS12 too low"
        }
        else if(PS22_TLW_active == 0) {
            PS22_TLW_active = 1; 
            PS22_TLW_time = millis();
        }
    }
    else {PS22_TLW_active = 0;}

    if (Data.PS31 >= PS31_UL){
        if (PS31_UL_active == 1 && (millis()-PS31_UL_time) >= PS_oob_max_delay){
            setValve(SV32, 1);           //open SV32

            // reply "error: PS31 over limit"
        }
        else if(PS31_UL_active == 0) {
            PS31_UL_active = 1;
            PS31_UL_time = millis();
        }
    }
    else {PS31_UL_active = 0;}
    if (Data.PS31 >= PS31_UW){
        if (PS31_UW_active == 1 && (millis()-PS31_UW_time) >= PS_oob_max_delay){
            // reply "warning: PS31 too high"
        }
        else if(PS31_UW_active == 0) {
            PS31_UW_active = 1;
            PS31_UW_time = millis();
        }
    }
    else {PS31_UW_active = 0;}
    if (Data.PS31 <= PS31_LW){
        if (PS31_LW_active == 1 && (millis()-PS31_LW_time) >= PS_oob_max_delay){
            // reply "warning: PS31 too low"
        }
        else if(PS31_LW_active == 0) {
            PS31_LW_active = 1;
            PS31_LW_time = millis();
        }
    }
    else {PS31_LW_active = 0;}

    if (Data.state == 1 && Data.PS41 >= PS41_TUL) {
        if (PS41_TUL_active == 1 && (millis()-PS41_TUL_time) >= PS_oob_max_delay){
            // emergency_stop();       // stops the test and puts the testbench in a safe configuration

            // reply "error: PS41 over limit - test aborted"
        }
        else if(PS41_TUL_active == 0) {
            PS41_TUL_active = 1;
            PS41_TUL_time = millis();
        }    
    }
    else {PS41_TUL_active = 0;}
    if (Data.state == 1 && Data.PS41 >= PS41_TUW){
        if (PS41_TUW_active == 1 && (millis()-PS41_TUW_time) >= PS_oob_max_delay){
            // reply "warning: PS41 too high"
        }
        else if(PS41_TUW_active == 0) {
            PS41_TUW_active = 1;
            PS41_TUW_time = millis();
        }
    }
    else {PS41_TUW_active = 0;}
    if (Data.state == 1 && Data.PS41 <= PS41_TLW){
        if (PS41_TLW_active == 1 && (millis()-PS41_TLW_time) >= PS_oob_max_delay){
            // reply "warning: PS41 too low"
        }
        else if(PS41_TLW_active == 0) {
            PS41_TLW_active = 1;
            PS41_TLW_time = millis();
        }
    }
    else {PS41_TLW_active = 0;}
    if (Data.state == 1 && Data.PS41 <= PS41_TLL) {
        if (PS41_TLL_active == 1 && (millis()-PS41_TLL_time) >= PS_oob_max_delay){
            // emergency_stop();       // stops the test and puts the testbench in a safe configuration

            // reply "error: PS41 below limit - test aborted"
        }
        else if(PS41_TLL_active == 0) {
            PS41_TLL_active = 1;
            PS41_TLL_time = millis();
        }    
    }
    else {PS41_TLL_active = 0;}

    if (Data.state == 1 && Data.PS42 >= PS42_TUL) {
        if (PS42_TUL_active == 1 && (millis()-PS42_TUL_time) >= PS_oob_max_delay){
            // emergency_stop();       // stops the test and puts the testbench in a safe configuration

            // reply "error: PS42 over limit - test aborted"
        }
        else if(PS42_TUL_active == 0) {
            PS42_TUL_active = 1;
            PS42_TUL_time = millis();
        }    
    }
    else {PS42_TUL_active = 0;}
    if (Data.state == 1 && Data.PS42 >= PS42_TUW){
        if (PS42_TUW_active == 1 && (millis()-PS42_TUW_time) >= PS_oob_max_delay){
            // reply "warning: PS42 too high"
        }
        else if(PS42_TUW_active == 0) {
            PS42_TUW_active = 1;
            PS42_TUW_time = millis();
        }
    }
    else {PS42_TUW_active = 0;}
    if (Data.state == 1 && Data.PS42 <= PS42_TLW){
        if (PS42_TLW_active == 1 && (millis()-PS42_TLW_time) >= PS_oob_max_delay){
            // reply "warning: PS42 too low"
        }
        else if(PS42_TLW_active == 0) {
            PS42_TLW_active = 1;
            PS42_TLW_time = millis();
        }
    }
    else {PS42_TLW_active = 0;}
    if (Data.state == 1 && Data.PS42 <= PS42_TLL) {
        if (PS42_TLL_active == 1 && (millis()-PS42_TLL_time) >= PS_oob_max_delay){
            //emergency_stop();       // stops the test and puts the testbench in a safe configuration

            // reply "error: PS42 below limit - test aborted"
        }
        else if(PS42_TLL_active == 0) {
            PS42_TLL_active = 1;
            PS42_TLL_time = millis();
        }    
    }
    else {PS42_TLL_active = 0;}

    if (Data.PS51 >= PS51_UW){
        if (PS51_UW_active == 1 && (millis()-PS51_UW_time) >= PS_oob_max_delay){
            // reply "warning: PS51 too high"
        }
        else if(PS51_UW_active == 0) {
            PS51_UW_active = 1;
            PS51_UW_time = millis();
        }
    }
    else {PS51_UW_active = 0;}
    if (Data.PS51 <= PS51_LW){
        if (PS51_LW_active == 1 && (millis()-PS51_LW_time) >= PS_oob_max_delay){
            // reply "warning: PS51 too low"
        }
        else if(PS51_LW_active == 0) {
            PS51_LW_active = 1;
            PS51_LW_time = millis();
        }
    }
    else {PS51_LW_active = 0;}
    if (Data.state == 1 && Data.PS51 <= PS51_TLL) {
        if (PS51_TLL_active == 1 && (millis()-PS51_TLL_time) >= PS_oob_max_delay){
            // emergency_stop();       // stops the test and puts the testbench in a safe configuration

            // reply "error: PS51 below limit - test aborted"
        }
        else if(PS51_TLL_active == 0) {
            PS51_TLL_active = 1;
            PS51_TLL_time = millis();
        }    
    }
    else {PS51_TLL_active = 0;}

    if ((Data.PS61 >= PS_WATER_UL) || (Data.PS62 >= PS_WATER_UL)){
        if (PS_WATER_UL_active == 1 && (millis()-PS_WATER_UL_time) >= PS_oob_max_delay){
            setValve(SV61, 1);           //open SV61
            setValve(SV62, 1);           //open SV62
            setValve(SV52, 0);           //close SV52
            setValve(SV52, 0);           //close SV53

            // reply "error: PS_WATER over limit"
        }
        else if(PS_WATER_UL_active == 0) {
            PS_WATER_UL_active = 1;
            PS_WATER_UL_time = millis();
        }
    }
    else {PS_WATER_UL_active = 0;}
//    if (WATER_BB == 1 &&((Data.PS61 >= PS_WATER_BBUW) || (Data.PS62 >= PS_WATER_BBUW))) {
//        if (PS_WATER_BBUW_active == 1 && (millis()-PS_WATER_BBUW_time) >= PS_oob_max_delay){
//            // reply "warning: PS_WATER too high in BB pressurization"
//        }
//        else if(PS_WATER_BBUW_active == 0) {
//            PS_WATER_BBUW_active = 1; 
//            PS_WATER_BBUW_time = millis();
//        }
//    }
//    else {PS_WATER_BBUW_active = 0;}
//    if (WATER_BB == 1 && ((Data.PS61 <= PS_WATER_BBLW) || (Data.PS62 <= PS_WATER_BBLW))) {
//        if (PS_WATER_BBLW_active == 1 && (millis()-PS_WATER_BBLW_time) >= PS_oob_max_delay){
//            // reply "warning: PS_WATER too low in BB pressurization"
//        }
//        else if(PS_WATER_BBLW_active == 0) {
//            PS_WATER_BBLW_active = 1; 
//            PS_WATER_BBLW_time = millis();
//        }
//    }
//    else {PS_WATER_BBLW_active = 0;}
    if (Data.state == 1 && ((Data.PS61 <= PS_WATER_TLL) || (Data.PS62 <= PS_WATER_TLL))) {
        if (PS_WATER_TLL_active == 1 && (millis()-PS_WATER_TLL_time) >= PS_oob_max_delay){
            setValve(SV52, 1);           //open SV52
            setValve(SV53, 1);           //open SV53
            setValve(SV61, 0);           //close SV61
            setValve(SV62, 0);           //close SV62

            // reply "error: PS_WATER below limit - test aborted"
        }
        else if(PS_WATER_TLL_active == 0) {
            PS_WATER_TLL_active = 1;
            PS_WATER_TLL_time = millis();
        }    
    }
    else {PS_WATER_TLL_active = 0;}

    if (Data.TS31 >= TS31_UW){
        if (TS31_UW_active == 1 && (millis()-TS31_UW_time) >= TS_oob_max_delay){
            // reply "warning: TS31 too high"
        }
        else if(TS31_UW_active == 0) {
            TS31_UW_active = 1;
            TS31_UW_time = millis();
        }
    }
    else {TS31_UW_active = 0;}

    if (Data.TS62 >= TS62_UW){
        if (TS62_UW_active == 1 && (millis()-TS62_UW_time) >= TS_oob_max_delay){
            // reply "warning: TS62 too high"
        }
        else if(TS62_UW_active == 0) {
            TS62_UW_active = 1;
            TS62_UW_time = millis();
        }
    }
    else {TS62_UW_active = 0;}
    if (Data.state == 1 && Data.TS62 >= TS62_TUL) {
        if (TS62_TUL_active == 1 && (millis()-TS62_TUL_time) >= TS_oob_max_delay){
            // emergency_stop();       // stops the test and puts the testbench in a safe configuration

            // reply "error: TS62 over limit - test aborted"
        }
        else if(TS62_TUL_active == 0) {
            TS62_TUL_active = 1;
            TS62_TUL_time = millis();
        }    
    }
    else {TS62_TUL_active = 0;}
}

void serialSend() {
    Serial.println("------ Sensor Data ------");

    Serial.print("Packet ID: "); Serial.println(Data.n);
    Serial.print("Timestamp (ms): "); Serial.println(Data.t);

    Serial.println("Pressure Sensors (mbar):");
    Serial.print("PS11: "); Serial.print(Data.PS11); Serial.print("\t");
    Serial.print("PS12: "); Serial.print(Data.PS12); Serial.print("\t");
    Serial.print("PS21: "); Serial.print(Data.PS21); Serial.print("\t");
    Serial.print("PS22: "); Serial.print(Data.PS22); Serial.print("\t");
    Serial.print("PS31: "); Serial.print(Data.PS31); Serial.print("\t");
    Serial.print("PS41: "); Serial.print(Data.PS41); Serial.print("\t");
    Serial.print("PS42: "); Serial.print(Data.PS42); Serial.print("\t");
    Serial.print("PS51: "); Serial.print(Data.PS51); Serial.print("\t");
    Serial.print("PS61: "); Serial.print(Data.PS61); Serial.print("\t");
    Serial.print("PS62: "); Serial.print(Data.PS62); Serial.print("\t");
    Serial.print("PS63: "); Serial.print(Data.PS63); Serial.print("\t");
    Serial.print("PS64: "); Serial.println(Data.PS64);

    Serial.println("Thermocouples (°C):");
    Serial.print("TS11: "); Serial.print(Data.TS11); Serial.print("\t");
    Serial.print("TS31: "); Serial.print(Data.TS31); Serial.print("\t");
    Serial.print("TS41: "); Serial.print(Data.TS41); Serial.print("\t");
    Serial.print("TS42: "); Serial.print(Data.TS42); Serial.print("\t");
    Serial.print("TS61: "); Serial.print(Data.TS61); Serial.print("\t");
    Serial.print("TS62: "); Serial.println(Data.TS62);

    Serial.println("Flow Meters (mL/s):");
    Serial.print("FM11: "); Serial.print(Data.FM11); Serial.print("\t");
    Serial.print("FM21: "); Serial.print(Data.FM21); Serial.print("\t");
    Serial.print("FM61: "); Serial.println(Data.FM61);

    Serial.print("Load Cell (N): "); Serial.println(Data.LC);
    Serial.print("5V Reference (mV): "); Serial.println(Data.ref5V);

    Serial.println("--------------------------");
    Serial.println();
}
