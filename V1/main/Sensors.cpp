#include "Sensors.h"

// --------------------- VARIABLES INITIALIZATION ------------------------------
float PS11, PS12, PS21, PS22, PS31, PS41, PS42, PS51, PS61, PS62, PS63, PS64, PSalim; //add PSextra if used
float LC01;
float FM11, FM21, FM61;

float TS11, TS31, TS41, TS42, TS61, TS62;
bool TS41_waiting = 0,TS42_waiting = 0,TS61_waiting= 0 ,TS62_waiting = 0,TS11_waiting = 0;

bool test = 0;

Adafruit_MAX31856 thermo_31 = Adafruit_MAX31856(TS31_pin);
Adafruit_MAX31856 thermo_41 = Adafruit_MAX31856(TS41_pin);
Adafruit_MAX31856 thermo_42 = Adafruit_MAX31856(TS42_pin);
Adafruit_MAX31856 thermo_61 = Adafruit_MAX31856(TS61_pin);
Adafruit_MAX31856 thermo_62 = Adafruit_MAX31856(TS62_pin);
Adafruit_MAX31856 thermo_11 = Adafruit_MAX31856(TS11_pin); //initialized last since it may return negative values

int n = 0; //Packet ID
unsigned long t;

// ------------------------- LIMITS DEFINITION ---------------------------------

int PS_oob_max_delay = 250;  // defines the maximum duration for which a pressure sensor value can stay out of bounds in ms
int TS_oob_max_delay = 1000; // defines the maximum duration for which a temperature sensor value can stay out of bounds in ms

// Limits are defined as: LL: Lower Limit, LW: Lower Warning, N: Nominal, UW: Upper Warning, UL: Upper Limit
// A "T" is added if the value is only checked during tests
// A "BB" is added if the value is only checked if the bang-bang pressurization of the concered tank is active -- in the "pressurization.ino" file
// If a warning is reached, a message is sent to the computer
// If a limit is reached, the microcontroller takes action to solve the problem or to put the testbench in a safe position

// Pressure sensors:
float PS11_UL = 25;
float PS12_TLW = 9,  PS12_TUW = 13;
float PS21_UL = 25;
float PS22_TLW = 9,  PS22_TUW = 13;
float PS31_LW = 20,  PS31_UW = 50,  PS31_UL = 55;
float PS41_TLL = 6,  PS41_TLW = 7,  PS41_TUW = 13, PS41_TUL = 14;
float PS42_TLL = 6,  PS42_TLW = 7,  PS42_TUW = 13, PS42_TUL = 14;
float PS51_TLL = 10, PS51_LW = 40,  PS51_UW = 210;
float PS61_TLL = 6,  PS61_UL = 14;
float PS62_TLL = 6,  PS62_UL = 14;

// Thermocouples:
float TS31_UW = 45;
float TS62_UW = 85, TS62_TUL = 95;


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

// ------------------------------ SETUP ----------------------------------------
void setupSensors(){
    delay(1500);              //delay

    // Thermocouples setup
    thermo_31.begin();
    thermo_41.begin();
    thermo_42.begin();
    thermo_61.begin();
    thermo_62.begin();
    thermo_11.begin();

    delay(1000);

    thermo_31.setThermocoupleType(MAX31856_TCTYPE_K);
    thermo_31.setConversionMode(MAX31856_ONESHOT_NOWAIT);

    thermo_41.setThermocoupleType(MAX31856_TCTYPE_K);
    thermo_41.setConversionMode(MAX31856_ONESHOT_NOWAIT);

    thermo_42.setThermocoupleType(MAX31856_TCTYPE_K);
    thermo_42.setConversionMode(MAX31856_ONESHOT_NOWAIT);

    thermo_61.setThermocoupleType(MAX31856_TCTYPE_K);
    thermo_61.setConversionMode(MAX31856_ONESHOT_NOWAIT);

    thermo_62.setThermocoupleType(MAX31856_TCTYPE_K);
    thermo_62.setConversionMode(MAX31856_ONESHOT_NOWAIT);

    thermo_11.setThermocoupleType(MAX31856_TCTYPE_K);
    thermo_11.setConversionMode(MAX31856_ONESHOT_NOWAIT);
}

void PS_for_BB(){
    // Reading the pressure sensors used for the Bang-Bang pressurization
    PS11 = PS_25bar_reading(PS11_pin);
    PS21 = PS_25bar_reading(PS21_pin);
    PS61 = PS_25bar_reading(PS61_pin);
    PS62 = PS_25bar_reading(PS62_pin);
}

void data(){

    // requesting data from the thermocouples if not waiting for a conversion
    if (!TS41_waiting){thermo_41.triggerOneShot(); TS41_waiting = 1;}
    if (!TS42_waiting){thermo_42.triggerOneShot(); TS42_waiting = 1;}
    if (!TS61_waiting){thermo_61.triggerOneShot(); TS61_waiting = 1;}
    if (!TS62_waiting){thermo_62.triggerOneShot(); TS62_waiting = 1;}
    if (!TS11_waiting){thermo_11.triggerOneShot(); TS11_waiting = 1;}

    t = millis();
    // reading sensors from ADC
    PS11 = PS_25bar_reading(PS11_pin);
    PS12 = PS_25bar_reading(PS12_pin);
    PS21 = PS_25bar_reading(PS21_pin);
    PS22 = PS_25bar_reading(PS22_pin);
    PS31 = PS_70bar_reading(PS31_pin);
    PS41 = PS_25bar_reading(PS41_pin);
    PS42 = PS_25bar_reading(PS42_pin);
    PS51 = PS_350bar_reading(PS51_pin);
    PS61 = PS_25bar_reading(PS61_pin);
    PS62 = PS_25bar_reading(PS62_pin);
    PS63 = PS_25bar_reading(PS63_pin);
    PS64 = PS_25bar_reading(PS64_pin);
    PSalim = PSalim_reading(PSalim_pin);

    LC01 = LC_reading(LC01_pin);

    FM11 = FM11_reading(FM11_pin);
    FM21 = FM21_reading(FM21_pin);
    FM61 = FM61_reading(FM61_pin);

    // getting data from the thermocouples if ready
    if (TS31_waiting && thermo_31.conversionComplete()) {
        TS31 = thermo_31.readThermocoupleTemperature();
        TS31_waiting = 0;
    }
    if (TS41_waiting && thermo_41.conversionComplete()) {
        TS41 = thermo_41.readThermocoupleTemperature();
        TS41_waiting = 0;
    }
    if (TS42_waiting && thermo_42.conversionComplete()) {
        TS42 = thermo_42.readThermocoupleTemperature();
        TS42_waiting = 0;
    }
    if (TS61_waiting && thermo_61.conversionComplete()) {
        TS61 = thermo_61.readThermocoupleTemperature();
        TS61_waiting = 0;
    }
    if (TS62_waiting && thermo_62.conversionComplete()) {
        TS62 = thermo_62.readThermocoupleTemperature();
        TS62_waiting = 0;
    }
    if (TS11_waiting && thermo_11.conversionComplete()) {
        TS11 = thermo_11.readThermocoupleTemperature();
        TS11_waiting = 0;
    }

    values_check();                             //check if values are within limits
    BB_pressurization();                        //bang-bang pressurization of the tanks if enabled
    send_data();                                //send data to the ground station
    save_data();                                //save data to the SD card
    n++;                                        //increment the packet ID
}

float PS_25bar_reading(int pin){                    //for all pressure sensors except PS31 and PS51
    return 31.25*(analogRead(pin)/1023.0 - 0.1);
}

float PS_70bar_reading(int pin){                    // for PS31
    return 87.5(analogRead(pin)/1023.0 - 0.1); 
}

float PS_350bar_reading(int pin){                   // for PS51
    return 437.5*(analogRead(pin)/1023.0 - 0.1) 
}

float FM11_reading(int pin){
    return 50*analogRead(pin)/1023.0;
}

float FM21_reading(int pin){
    return 40*analogRead(pin)/1023.0;
}

float FM61_reading(int pin){
    return 150*analogRead(pin)/1023.0;
}

float LC_reading(int pin){
    return 2943*analogRead(pin)/1023.0; 
}

float PSalim_reading(int pin){
    return 
}

void values_check(){
    // check if values reached a warning or a limit
    // if so, start a timer (corresponding to the last time the value was in the bounds)
    // each time the sensors are read, check is the value is back in bounds (set the timer to zero) or is it is still out of bounds (let the timer run)
    // once the timer value is over the time limit: PS_oob_max_delay or TS_oob_max_delay
    // send a message, solve the problem (vent) or abort the test if necessary

    if (PS11 >= PS11_UL){
        if (PS11_UL_active == True && (millis()-PS11_UL_time) >= PS_oob_max_delay){
            setValve(0, 1);          // open SV11
            setValve(8, 0);          // close SV33

            // reply "error: PS11 over limit"
        }
        else if(PS11_UL_active == False) {
            PS11_UL_active = True; 
            PS11_UL_time = millis();
        }
    }
    else {PS11_UL_active = False;}
    if (LOX_BB == True && PS11 >= PS11_BBUW) {
        if (PS11_BBUW_active == True && (millis()-PS11_BBUW_time) >= PS_oob_max_delay){
            // reply "warning: PS11 too high in BB pressurization"
        }
        else if(PS11_BBUW_active == False) {
            PS11_BBUW_active = True; 
            PS11_BBUW_time = millis();
        }
    }
    else {PS11_BBUW_active = False;}
    if (LOX_BB == True && PS11 <= PS11_BBLW) {
        if (PS11_BBLW_active == True && (millis()-PS11_BBLW_time) >= PS_oob_max_delay){
            // reply "warning: PS11 too low in BB pressurization"
        }
        else if(PS11_BBLW_active == False) {
            PS11_BBLW_active = True; 
            PS11_BBLW_time = millis();
        }
    }
    else {PS11_BBLW_active = False;}

    if (test == True && PS12 >= PS12_TUW) {
        if (PS12_TUW_active == True && (millis()-PS12_TUW_time) >= PS_oob_max_delay){
            // reply "warning: PS12 too high"
        }
        else if(PS12_TUW_active == False) {
            PS12_TUW_active = True; 
            PS12_TUW_time = millis();
        }
    }
    else {PS12_TUW_active = False;}
    if (test == True && PS12 <= PS12_TLW) {
        if (PS12_TLW_active == True && (millis()-PS12_TLW_time) >= PS_oob_max_delay){
            // reply "warning: PS12 too low"
        }
        else if(PS12_TLW_active == False) {
            PS12_TLW_active = True; 
            PS12_TLW_time = millis();
        }
    }
    else {PS12_TLW_active = False;}

    if (PS21 >= PS21_UL){
        if (PS21_UL_active == True && (millis()-PS21_UL_time) >= PS_oob_max_delay){
            setValve(3, 1);          // open SV21
            setValve(9, 0);          // close SV34

            // reply "error: PS21 over limit"
        }
        else if(PS21_UL_active == False) {
            PS21_UL_active = True; 
            PS21_UL_time = millis();
        }
    }
    else {PS21_UL_active = False;}
    if (ETH_BB == True && PS21 >= PS21_BBUW) {
        if (PS21_BBUW_active == True && (millis()-PS21_BBUW_time) >= PS_oob_max_delay){
            // reply "warning: PS21 too high in BB pressurization"
        }
        else if(PS21_BBUW_active == False) {
            PS21_BBUW_active = True; 
            PS21_BBUW_time = millis();
        }
    }
    else {PS21_BBUW_active = False;}
    if (ETH_BB == True && PS21 <= PS21_BBLW) {
        if (PS21_BBLW_active == True && (millis()-PS21_BBLW_time) >= PS_oob_max_delay){
            // reply "warning: PS21 too low in BB pressurization"
        }
        else if(PS21_BBLW_active == False) {
            PS21_BBLW_active = True; 
            PS21_BBLW_time = millis();
        }
    }
    else {PS21_BBLW_active = False;}

    if (test == True && PS22 >= PS22_TUW) {
        if (PS22_TUW_active == True && (millis()-PS22_TUW_time) >= PS_oob_max_delay){
            // reply "warning: PS22 too high"
        }
        else if(PS22_TUW_active == False) {
            PS22_TUW_active = True; 
            PS22_TUW_time = millis();
        }
    }
    else {PS22_TUW_active = False;}
    if (test == True && PS22 <= PS22_TLW) {
        if (PS22_TLW_active == True && (millis()-PS22_TLW_time) >= PS_oob_max_delay){
            // reply "warning: PS12 too low"
        }
        else if(PS22_TLW_active == False) {
            PS22_TLW_active = True; 
            PS22_TLW_time = millis();
        }
    }
    else {PS22_TLW_active = False;}

    if (PS31 >= PS31_UL){
        if (PS31_UL_active == True && (millis()-PS31_UL_time) >= PS_oob_max_delay){
            setValve(7, 1)           //open SV32

            // reply "error: PS31 over limit"
        }
        else if(PS31_UL_active == False) {
            PS31_UL_active = True;
            PS31_UL_time = millis();
        }
    }
    else {PS31_UL_active = False;}
    if (PS31 >= PS31_UW){
        if (PS31_UW_active == True && (millis()-PS31_UW_time) >= PS_oob_max_delay){
            // reply "warning: PS31 too high"
        }
        else if(PS31_UW_active == False) {
            PS31_UW_active = True;
            PS31_UW_time = millis();
        }
    }
    else {PS31_UW_active = False;}
    if (PS31 <= PS31_LW){
        if (PS31_LM_active == True && (millis()-PS31_LW_time) >= PS_oob_max_delay){
            // reply "warning: PS31 too low"
        }
        else if(PS31_LW_active == False) {
            PS31_LW_active = True;
            PS31_LW_time = millis();
        }
    }
    else {PS31_LW_active = False;}

    if (test == True && PS41 >= PS41_TUL) {
        if (PS41_TUL_active == True && (millis()-PS41_TUL_time) >= PS_oob_max_delay){
            emergency_stop();       // stops the test and puts the testbench in a safe configuration

            // reply "error: PS41 over limit - test aborted"
        }
        else if(PS41_TUL_active == False) {
            PS41_TUL_active = True;
            PS41_TUL_time = millis();
        }    
    }
    else {PS41_TUL_active = False;}
    if (test == True && PS41 >= PS41_TUW){
        if (PS41_TUW_active == True && (millis()-PS41_TUW_time) >= PS_oob_max_delay){
            // reply "warning: PS41 too high"
        }
        else if(PS41_TUW_active == False) {
            PS41_TUW_active = True;
            PS41_TUW_time = millis();
        }
    }
    else {PS41_TUW_active = False;}
    if (test == True && PS41 <= PS41_TLW){
        if (PS41_TLW_active == True && (millis()-PS41_TLW_time) >= PS_oob_max_delay){
            // reply "warning: PS41 too low"
        }
        else if(PS41_TLW_active == False) {
            PS41_TLW_active = True;
            PS41_TLW_time = millis();
        }
    }
    else {PS41_TLW_active = False;}
    if (test == True && PS41 <= PS41_TLL) {
        if (PS41_TLL_active == True && (millis()-PS41_TLL_time) >= PS_oob_max_delay){
            emergency_stop();       // stops the test and puts the testbench in a safe configuration

            // reply "error: PS41 below limit - test aborted"
        }
        else if(PS41_TLL_active == False) {
            PS41_TLL_active = True;
            PS41_TLL_time = millis();
        }    
    }
    else {PS41_TLL_active = False;}

    if (test == True && PS42 >= PS42_TUL) {
        if (PS42_TUL_active == True && (millis()-PS42_TUL_time) >= PS_oob_max_delay){
            emergency_stop();       // stops the test and puts the testbench in a safe configuration

            // reply "error: PS42 over limit - test aborted"
        }
        else if(PS42_TUL_active == False) {
            PS42_TUL_active = True;
            PS42_TUL_time = millis();
        }    
    }
    else {PS42_TUL_active = False;}
    if (test == True && PS42 >= PS42_TUW){
        if (PS42_TUW_active == True && (millis()-PS42_TUW_time) >= PS_oob_max_delay){
            // reply "warning: PS42 too high"
        }
        else if(PS42_TUW_active == False) {
            PS42_TUW_active = True;
            PS42_TUW_time = millis();
        }
    }
    else {PS42_TUW_active = False;}
    if (test == True && PS42 <= PS42_TLW){
        if (PS42_TLW_active == True && (millis()-PS42_TLW_time) >= PS_oob_max_delay){
            // reply "warning: PS42 too low"
        }
        else if(PS42_TLW_active == False) {
            PS42_TLW_active = True;
            PS42_TLW_time = millis();
        }
    }
    else {PS42_TLW_active = False;}
    if (test == True && PS42 <= PS42_TLL) {
        if (PS42_TLL_active == True && (millis()-PS42_TLL_time) >= PS_oob_max_delay){
            emergency_stop();       // stops the test and puts the testbench in a safe configuration

            // reply "error: PS42 below limit - test aborted"
        }
        else if(PS42_TLL_active == False) {
            PS42_TLL_active = True;
            PS42_TLL_time = millis();
        }    
    }
    else {PS42_TLL_active = False;}

    if (PS51 >= PS51_UW){
        if (PS51_UW_active == True && (millis()-PS51_UW_time) >= PS_oob_max_delay){
            // reply "warning: PS51 too high"
        }
        else if(PS51_UW_active == False) {
            PS51_UW_active = True;
            PS51_UW_time = millis();
        }
    }
    else {PS51_UW_active = False;}
    if (PS51 <= PS51_LW){
        if (PS51_LM_active == True && (millis()-PS51_LW_time) >= PS_oob_max_delay){
            // reply "warning: PS51 too low"
        }
        else if(PS51_LW_active == False) {
            PS51_LW_active = True;
            PS51_LW_time = millis();
        }
    }
    else {PS51_LW_active = False;}
    if (test == True && PS51 <= PS51_TLL) {
        if (PS51_TLL_active == True && (millis()-PS51_TLL_time) >= PS_oob_max_delay){
            emergency_stop();       // stops the test and puts the testbench in a safe configuration

            // reply "error: PS51 below limit - test aborted"
        }
        else if(PS51_TLL_active == False) {
            PS51_TLL_active = True;
            PS51_TLL_time = millis();
        }    
    }
    else {PS51_TLL_active = False;}

    if ((PS61 >= PS_WATER_UL) || (PS62 >= PS_WATER_UL)){
        if (PS_WATER_UL_active == True && (millis()-PS_WATER_UL_time) >= PS_oob_max_delay){
            setValve(15, 1)           //open SV61
            setValve(16, 1)           //open SV62
            setValve(13, 0)           //close SV52
            setValve(14, 0)           //close SV53

            // reply "error: PS_WATER over limit"
        }
        else if(PS_WATER_UL_active == False) {
            PS_WATER_UL_active = True;
            PS_WATER_UL_time = millis();
        }
    }
    else {PS_WATER_UL_active = False;}
    if (WATER_BB == True &&((PS61 >= PS_WATER_BBUW) || (PS62 >= PS_WATER_BBUW))) {
        if (PS_WATER_BBUW_active == True && (millis()-PS_WATER_BBUW_time) >= PS_oob_max_delay){
            // reply "warning: PS_WATER too high in BB pressurization"
        }
        else if(PS_WATER_BBUW_active == False) {
            PS_WATER_BBUW_active = True; 
            PS_WATER_BBUW_time = millis();
        }
    }
    else {PS_WATER_BBUW_active = False;}
    if (WATER_BB == True && ((PS61 <= PS_WATER_BBLW) || (PS62 <= PS_WATER_BBLW))) {
        if (PS_WATER_BBLW_active == True && (millis()-PS_WATER_BBLW_time) >= PS_oob_max_delay){
            // reply "warning: PS_WATER too low in BB pressurization"
        }
        else if(PS_WATER_BBLW_active == False) {
            PS_WATER_BBLW_active = True; 
            PS_WATER_BBLW_time = millis();
        }
    }
    else {PS_WATER_BBLW_active = False;}
    if (test == True && ((PS61 <= PS_WATER_TLL) || (PS62 <= PS_WATER_TLL))) {
        if (PS_WATER_TLL_active == True && (millis()-PS_WATER_TLL_time) >= PS_oob_max_delay){
            emergency_stop();       // stops the test and puts the testbench in a safe configuration

            // reply "error: PS_WATER below limit - test aborted"
        }
        else if(PS_WATER_TLL_active == False) {
            PS_WATER_TLL_active = True;
            PS_WATER_TLL_time = millis();
        }    
    }
    else {PS_WATER_TLL_active = False;}

    if (TS31 >= TS31_UW){
        if (TS31_UW_active == True && (millis()-TS31_UW_time) >= TS_oob_max_delay){
            // reply "warning: TS31 too high"
        }
        else if(TS31_UW_active == False) {
            TS31_UW_active = True;
            TS31_UW_time = millis();
        }
    }
    else {TS31_UW_active = False;}

    if (TS62 >= TS62_UW){
        if (TS62_UW_active == True && (millis()-TS62_UW_time) >= TS_oob_max_delay){
            // reply "warning: TS62 too high"
        }
        else if(TS62_UW_active == False) {
            TS62_UW_active = True;
            TS62_UW_time = millis();
        }
    }
    else {TS62_UW_active = False;}
    if (test == True && TS62 >= TS62_TUL) {
        if (TS62_TUL_active == True && (millis()-TS62_TUL_time) >= TS_oob_max_delay){
            emergency_stop();       // stops the test and puts the testbench in a safe configuration

            // reply "error: TS62 over limit - test aborted"
        }
        else if(TS62_TUL_active == False) {
            TS62_TUL_active = True;
            TS62_TUL_time = millis();
        }    
    }
    else {TS62_TUL_active = False;}
}

void send_data(){
    // send data to the ground station
    byte frame[60] = {
        0xFF, 0xFF,                           //header
        0xFF, 0xFF,
        (t >> 24) & 0xFF, (t >> 16) & 0xFF,
        (t >> 8) & 0xFF, t & 0xFF,            //t
        (PS11 & 0xFF00) >> 8, PS11 & 0x00FF,  //PS11
        (PS12 & 0xFF00) >> 8, PS12 & 0x00FF,  //PS12
        (PS21 & 0xFF00) >> 8, PS21 & 0x00FF,  //PS21
        (PS22 & 0xFF00) >> 8, PS22 & 0x00FF,  //PS22
        (PS31 & 0xFF00) >> 8, PS31 & 0x00FF,  //PS31
        (PS41 & 0xFF00) >> 8, PS41 & 0x00FF,  //PS41
        (PS42 & 0xFF00) >> 8, PS42 & 0x00FF,  //PS42
        (PS61 & 0xFF00) >> 8, PS61 & 0x00FF,  //PS61
        (PS62 & 0xFF00) >> 8, PS62 & 0x00FF,  //PS62
        (PS63 & 0xFF00) >> 8, PS63 & 0x00FF,  //PS63
        
        (TS11 >> 24) & 0xFF, (TS11 >> 16) & 0xFF,
        (TS11 >> 8) & 0xFF, TS11 & 0xFF,            //TS11
        (TS41 >> 24) & 0xFF, (TS41 >> 16) & 0xFF,
        (TS41 >> 8) & 0xFF, TS41 & 0xFF,            //TS41
        (TS42 >> 24) & 0xFF, (TS42 >> 16) & 0xFF,
        (TS42 >> 8) & 0xFF, TS42 & 0xFF,            //TS42
        (TS61 >> 24) & 0xFF, (TS61 >> 16) & 0xFF,
        (TS61 >> 8) & 0xFF, TS61 & 0xFF,            //TS61
        (TS62 >> 24) & 0xFF, (TS62 >> 16) & 0xFF,
        (TS62 >> 8) & 0xFF, TS62 & 0xFF,            //TS62
        (FS01 & 0xFF00) >> 8, FS01 & 0x00FF,  //FS01
        (FM11 & 0xFF00) >> 8, FM11 & 0x00FF,  //FM11
        (FM21 & 0xFF00) >> 8, FM21 & 0x00FF,  //FM21
        (FM61 & 0xFF00) >> 8, FM61 & 0x00FF,  //FM61
        (n >> 24) & 0xFF, (n >> 16) & 0xFF,
        (n >> 8) & 0xFF, n & 0xFF,            //frame ID
    };

}

void save_data(){
    // save data to the SD card
}