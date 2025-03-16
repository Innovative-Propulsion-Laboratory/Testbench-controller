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
unsigned long t, t_last_data_packet;

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
    PSalim = PS_25bar_reading(PSalim_pin);

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

void values_check(){
    // check if values reached a warning or a limit
    // if so, start a timer (conrresponding to the last time the value was in the bounds)
    // each time the sensors are read, check is the value is back in bounds (set the timer to zero) or is it is still out of bounds (let the timer run)
    // once the timer value is over the time limit: PS_oob_max_delay or TS_oob_max_delay
    // send a message, solve the problem (vent) or abort the test if necessary
}

void send_data(){
    // send data to the ground station
}

void save_data(){
    // save data to the SD card
}