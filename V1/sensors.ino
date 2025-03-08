// ----------------------------- IMPORTS ---------------------------------------
#include <Adafruit_MAX31856.h>  // https://github.com/adafruit/Adafruit_MAX31856
// 
// ------------------------ PIN CONFIGURATION ----------------------------------

// Pressure sensors:
#define PS11_pin A9         //LOX tank pressure
#define PS12_pin A8         //LOX injector pressure
#define PS21_pin A7         //ETH tank pressure
#define PS22_pin A6         //ETH injector pressure
#define PS31_pin A5         //GN2 tank pressure
#define PS41_pin A4         //Main Combustion Chamber pressure 1
#define PS42_pin A3         //Main Combustion Chamber pressure 2
#define PS51_pin A2         //Air tank pressure
#define PS61_pin A1         //Water tank pressure 1
#define PS62_pin A0         //Water tank pressure 2
#define PS63_pin A17        //Water pressure before regen
#define PS64_pin A16        //Water pressure after regen
// #define PSextra_pin A15
#define PSalim_pin A14      // Pressure sensors alimentation

// Load cell:
#define LC01_pin A10

// Flow sensors:
#define FM11_pin A11        //LOX injection flow
#define FM21_pin A12        //ETH injection flow
#define FM61_pin A13        //Cooling flow

// Thermocouples:
#define TS11_pin 28         // LOX temperature
#define TS31_pin 29         // GN2 50bar tank temperature
#define TS41_pin 30         // Main Combustion Chamber temperature 1
#define TS42_pin 35         // Main Combustion Chamber temperature 2
#define TS61_pin 36         // Water initial temperature
#define TS62_pin 37         // Water final temperature


// --------------------- VARIABLES INITIALIZATION ------------------------------


float PS11, PS12, PS21, PS22, PS31, PS41, PS42, PS51, PS61, PS62, PS63, PS64, PSalim; //add PSextra if used
float LC01;
float FM11, FM21, FM61;

float TS11, TS31, TS41, TS42, TS61, TS62;
bool TS41_waiting=0,TS42_waiting=0,TS61_waiting=0,TS62_waiting=0,TS11_waiting=0;

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
// A "BB" is added if the value is only checked if the bang-bang pressurization of the concered tank is active
// If a warning is reached, a message is sent to the computer
// If a limit is reached, the microcontroller takes action to solve the problem or to put the testbench in a safe position

// Pressure sensors:
float PS11_BBLW = 15, PS11_N = 16,  PS11_BBUW = 17, PS11_UL = 25;
float PS12_TLW = 9,  PS12_N = 10,  PS12_TUW = 13;
float PS21_BBLW = 15, PS21_N = 16,  PS21_BBUW = 17, PS21_UL = 25;
float PS22_TLW = 9,  PS22_N = 10,  PS22_TUW = 13;
float PS31_LW = 20, PS31_N = 45,  PS31_UW = 50, PS31_UL = 55;
float PS41_TLL = 6,  PS41_TLW = 7,  PS41_N = 10,  PS41_TUW = 13, PS41_TUL = 14;
float PS42_TLL = 6,  PS42_TLW = 7,  PS42_N = 10,  PS42_TUW = 13, PS42_TUL = 14;
float PS51_TLL = 10, PS51_LW = 40, PS51_N = 200, PS51_UW = 210;
float PS61_TLL = 6,  PS61_BBLW = 8,  PS61_N = 10,  PS61_BBUW = 11, PS61_UL = 14;
float PS62_TLL = 6,  PS62_BBLW = 8,  PS62_N = 10,  PS62_BBUW = 11, PS62_UL = 14;

// Thermocouples:
float TS31_N = 25, TS31_UW = 45;
float TS62_N = 70, TS62_UW = 85, TS62_TUL = 95;

// ------------------------------ SETUP ----------------------------------------
void setup(){
    Serial.begin(9600);       //initialize Serial Port
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

void data{

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
    send_data();                                //send data to the ground station
    save_data();                                //save data to the SD card
    n++;                                        //increment the packet ID
}

float PS_25bar_reading(pin){                    //for all pressure sensors except PS31 and PS51
    return 31.25*(analogRead(pin)/1023;-0.1);
}

float PS_70bar_reading(pin){                    // for PS31
    return 87.5(analogRead(pin)/1023;-0.1); 
}

float PS_350bar_reading(pin){                   // for PS51
    return 437.5*(analogRead(pin)/1023;-0.1) 
}

float FM11_reading(pin){
    return 50*analogRead(pin)/1023;
}

float FM21_reading(pin){
    return 40*analogRead(pin)/1023;
}

float FM61_reading(pin){
    return 150*analogRead(pin)/1023;
}

float LC_reading(pin){
    return 2943*analogRead(pin)/1023; 
}

void values_check(){
    if (PS11 => PS11_UL){
      if (PS11_Limit_active == True && (millis(()-PS11_time_limit)) == PS_limit_time{
          // ouvrir 
          abort_test();
          // reply(PS11 a merder)
        }
      else if(PS11_Limit_active == False) {
        PS11_Limit_active == True; 
        PS11_time_limit = millis();}
    }
    if (bangbang_active == True && (PS11_UL > PS11 => PS11_BBUW)) {
      if PS11_Limit_active == True {PS11_Limit_active == False}
      reply("warning PS11 trop haut");
    }
    else if (bangbang_active == True && PS11 <= PS11_BBLW) {
      if PS11_Limit_active == True {PS11_Limit_active == False}
      reply("warning PS11 trop bas");
    }

    if (test_active == True && PS12 => PS12_TUW) {
      // veirf abort
      reply("warning PS12 trop haut");
    }
    else if (test_active == True && PS12 <= PS12_TLW) {
      // verif abort
      reply("warning PS12 trop bas");
    }

    if (PS21 => PS21_UL){
      if (PS21_Limit_active == True && (millis(()-PS21_time_limit)) == PS_limit_time{
          // ouvrir 
          abort_test();
          // reply(PS11 a merder)
        }
      else if(PS21_Limit_active == False) {
        PS21_Limit_active == True; 
        PS21_time_limit = millis();}
    }
    if (bangbang_active == True && PS21 => PS21_BBUW) {
      if PS21_Limit_active == True {PS21_Limit_active == False}
      reply("warning PS21 trop haut");
    }
    else if (bangbang_active == True && PS21 <= PS21_BBLW) {
      if PS21_Limit_active == True {PS21_Limit_active == False}
      reply("warning PS21 trop bas");
    }

    if (test_active == True && PS22 => PS22_BBUW) {
      reply("warning PS22 trop haut");
    }
    else if (test_active == True && PS22 <= PS22_BBLW) {
      reply("warning PS22 trop bas");
    }

    if (PS31 => PS31_UL){
      if (PS31_Limit_active == True && (millis(()-PS31_time_limit)) == PS_limit_time{
          // ouvrir 
          abort_test();
          // reply(PS11 a merder)
        }
      else if(PS31_Limit_active == False) {
        PS31_Limit_active == True; 
        PS31_time_limit = millis();}
    }
    else if (PS31 => PS31_UW){
      // warning
    }
    else if (PS31 <= PS31_LW){
      // warning
    }

    if (test_active == True && PS41 => PS41_UL){
      if (PS41_Limit_active == True && (millis(()-PS41_time_limit)) == PS_limit_time{
          // ouvrir 
          abort_test();
          // reply(PS11 a merder)
        }
      else if(PS41_Limit_active == False) {
        PS41_Limit_active == True; 
        PS41_time_limit = millis();}
    }
    if (test_active == True && PS41 => PS41_BBUW) {
      reply("warning PS41 trop haut");
    }
    else if (test == True && PS41 <= PS41_BBLW) {
      reply("warning PS41 trop bas");
    }
    else if (test_active == True && PS41 <= PS41_LL){
      if (PS41_Limit_active == True && (millis(()-PS41_time_limit)) == PS_limit_time{
          // ouvrir 
          abort_test();
          // reply(PS11 a merder)
        }
      else if(PS41_Limit_active == False) {
        PS41_Limit_active == True; 
        PS41_time_limit = millis();}
    }

    if (test_active == True && PS42 => PS42_UL){
      if (PS42_Limit_active == True && (millis(()-PS42_time_limit)) == PS_limit_time{
          // ouvrir 
          abort_test();
          // reply(PS11 a merder)
        }
      else if(PS41_Limit_active == False) {
        PS42_Limit_active == True; 
        PS42_time_limit = millis();}
    }
    if (test_active == True && PS42 => PS42_BBUW) {
      reply("warning PS42 trop haut");
    }
    else if (test_active == True && PS42 <= PS42_BBLW) {
      reply("warning PS42 trop bas");
    }
    else if (test_active == True && PS42 <= PS42_LL){
      if (PS42_Limit_active == True && (millis(()-PS42_time_limit)) == PS_limit_time{
          // ouvrir 
          abort_test();
          // reply(PS11 a merder)
        }
      else if(PS41_Limit_active == False) {
        PS42_Limit_active == True; 
        PS42_time_limit = millis();}
    }

    if (PS51 => PS51_UW) {
      // warning
    }
    else if (PS51 <= PS51_LW) {
      reply("warning PS51 trop bas");
    }
    else if (test_active == True && PS51 <= PS51_LL){
      if (PS51_Limit_active == True && (millis(()-PS51_time_limit)) == PS_limit_time{
          // ouvrir 
          abort_test();
          // reply(PS11 a merder)
        }
      else if(PS41_Limit_active == False) {
        PS51_Limit_active == True; 
        PS51_time_limit = millis();}
    }

    if (PS61 => PS61_UL){
      if (PS61_Limit_active == True && (millis(()-PS61_time_limit)) == PS_limit_time{
          // ouvrir 
          abort_test();
          // reply(PS11 a merder)
        }
      else if(PS41_Limit_active == False) {
        PS61_Limit_active == True; 
        PS61_time_limit = millis();}
    }
    if (test_active == True && PS61 => PS61_BBUW) {
      reply("warning PS61 trop haut");
    }
    else if (test_active == True && PS61 <= PS61_BBLW) {
      reply("warning PS51 trop bas");
    }
    if (test_active == True && PS61 <= PS61_LL){
      if (PS61_Limit_active == True && (millis(()-PS61_time_limit)) == PS_limit_time{
          // ouvrir 
          abort_test();
          // reply(PS11 a merder)
        }
      else if(PS41_Limit_active == False) {
        PS61_Limit_active == True; 
        PS61_time_limit = millis();}
    }
    if (PS62 => PS62_UL){
      if (PS62_Limit_active == True && (millis(()-PS62_time_limit)) == PS_limit_time{
          // ouvrir 
          abort_test();
          // reply(PS11 a merder)
        }
      else if(PS41_Limit_active == False) {
        PS62_Limit_active == True; 
        PS62_time_limit = millis();}
    }
    if (test_active == True && PS62 => PS62_BBUW) {
      reply("warning PS52 trop haut");
    }
    else if (test_active == True && PS62 <= PS62_BBLW) {
      reply("warning PS62 trop bas");
    }
    if (test_active == True && PS62 <= PS62_UL){
      if (PS62_Limit_active == True && (millis(()-PS62_time_limit)) == PS_limit_time{
          // ouvrir 
          abort_test();
          // reply(PS11 a merder)
        }
      else if(PS41_Limit_active == False) {
        PS62_Limit_active == True; 
        PS62_time_limit = millis();}
    }

    if (TS31 => TS31_UW) {
      reply("warning TS52 trop haut");
    }

    if (TS62_UL > TS62 => TS62_BBUW) {
      // warning
    }
    else if (test_active == True && TS62 >= TS62_UL) {
      if (PS62_Limit_active == True && (millis(()-PS62_time_limit)) == PS_limit_time{
          // ouvrir 
          abort_test();
          // reply(PS11 a merder)
        }
      else if(PS41_Limit_active == False) {
        PS62_Limit_active == True; 
        PS62_time_limit = millis();}
    }



    // check if values reached a warning or a limit
    // if so, start a timer (conrresponding to the last time the value was in the bounds)
    // each time the sensors are read, check is the value is back in bounds (set the timer to zero) or is it is still out of bounds (let the timer run)
    // once the timer value is over the time limit: PS_oob_max_delay or TS_oob_max_delay
    // send a message, solve the problem (vent) or abort the test if necessary
}

// float PS11_BBLW = 15, PS11_N = 16,  PS11_BBUW = 17, PS11_UL = 25;
// float PS12_TLW = 9,  PS12_N = 10,  PS12_TUW = 13;
// float PS21_BBLW = 15, PS21_N = 16,  PS21_BBUW = 17, PS21_UL = 25;
// float PS22_TLW = 9,  PS22_N = 10,  PS22_TUW = 13;
// float PS31_LW = 20, PS31_N = 45,  PS31_UW = 50, PS31_UL = 55;
// float PS41_TLL = 6,  PS41_TLW = 7,  PS41_N = 10,  PS41_TUW = 13, PS41_TUL = 14;
// float PS42_TLL = 6,  PS42_TLW = 7,  PS42_N = 10,  PS42_TUW = 13, PS42_TUL = 14;
// float PS51_TLL = 10, PS51_LW = 40, PS51_N = 200, PS51_UW = 210;
// float PS61_TLL = 6,  PS61_BBLW = 8,  PS61_N = 10,  PS61_BBUW = 11, PS61_UL = 14;
// float PS62_TLL = 6,  PS62_BBLW = 8,  PS62_N = 10,  PS62_BBUW = 11, PS62_UL = 14;