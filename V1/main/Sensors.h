#ifndef SENSORS_H
#define SENSORS_H

#include "Valves.h"
#include "Pressurization.h"
#include <Adafruit_MAX31856.h>  // https://github.com/adafruit/Adafruit_MAX31856

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

extern float PS11, PS12, PS21, PS22, PS31, PS41, PS42, PS51, PS61, PS62, PS63, PS64, PSalim; //add PSextra if used
extern float LC01;
extern float FM11, FM21, FM61;

extern float TS11, TS31, TS41, TS42, TS61, TS62;
extern bool TS41_waiting,TS42_waiting,TS61_waiting,TS62_waiting,TS11_waiting;

extern bool test;

extern Adafruit_MAX31856 thermo_31;
extern Adafruit_MAX31856 thermo_41;
extern Adafruit_MAX31856 thermo_42;
extern Adafruit_MAX31856 thermo_61;
extern Adafruit_MAX31856 thermo_62;
extern Adafruit_MAX31856 thermo_11; //initialized last since it may return negative values

extern int n; //Packet ID
extern unsigned long t;

// ------------------------- LIMITS DEFINITION ---------------------------------

extern int PS_oob_max_delay;  // defines the maximum duration for which a pressure sensor value can stay out of bounds in ms
extern int TS_oob_max_delay; // defines the maximum duration for which a temperature sensor value can stay out of bounds in ms

// Limits are defined as: LL: Lower Limit, LW: Lower Warning, N: Nominal, UW: Upper Warning, UL: Upper Limit
// A "T" is added if the value is only checked during tests
// A "BB" is added if the value is only checked if the bang-bang pressurization of the concered tank is active -- in the "pressurization.ino" file
// If a warning is reached, a message is sent to the computer
// If a limit is reached, the microcontroller takes action to solve the problem or to put the testbench in a safe position

// Pressure sensors:
extern float PS11_UL;
extern float PS12_TLW, PS12_TUW;
extern float PS21_UL;
extern float PS22_TLW, PS22_TUW;
extern float PS31_LW,  PS31_UW,  PS31_UL;
extern float PS41_TLL, PS41_TLW, PS41_TUW, PS41_TUL;
extern float PS42_TLL, PS42_TLW, PS42_TUW, PS42_TUL;
extern float PS51_TLL, PS51_LW,  PS51_UW;
extern float PS61_TLL, PS61_UL;
extern float PS62_TLL, PS62_UL;

// Thermocouples:
extern float TS31_UW;
extern float TS62_UW, TS62_TUL;

// Pressure sensors:
extern bool PS11_UL_active,  PS11_BBLW_active, PS11_BBUW_active;
extern bool PS12_TLW_active, PS12_TUW_active;
extern bool PS21_UL_active, PS21_BBLW_active, PS21_BBUW_active;
extern bool PS22_TLW_active, PS22_TUW_active;
extern bool PS31_LW_active,  PS31_UW_active,  PS31_UL_active;
extern bool PS41_TLL_active, PS41_TLW_active, PS41_TUW_active, PS41_TUL_active;
extern bool PS42_TLL_active, PS42_TLW_active, PS42_TUW_active, PS42_TUL_active;
extern bool PS51_TLL_active, PS51_LW_active,  PS51_UW_active;
extern bool PS_WATER_TLL_active, PS_WATER_BBLW_active, PS_WATER_BBUW_active, PS_WATER_UL_active;

// Thermocouples:
extern bool TS31_UW_active;
extern bool TS62_UW_active, TS62_TUL_active;

// Functions:
void setupSensors();
void PS_for_BB();
void data();
void values_check();
void send_data();
void save_data();

float PS_25bar_reading(int pin);
float PS_70bar_reading(int pin);
float PS_350bar_reading(int pin);
float FM11_reading(int pin);
float FM21_reading(int pin);
float FM61_reading(int pin);
float LC_reading(int pin);

#endif