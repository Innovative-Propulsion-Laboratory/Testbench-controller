#ifndef SENSORS_H
#define SENSORS_H

#include "Valves.h"
#include "Pressurization.h"
#include "SaveData.h"
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

// Structure containing all the data sent from the Teensy to the computer
struct data {
    uint32_t n;                 // packet ID
    uint32_t t;                 // Timestamp (ms)
    
    uint16_t PS11, PS12, PS21, PS22, PS31, PS41, PS42, PS51, PS61, PS62, PS63, PS64;    // Pressure in mbar
    float TS11, TS31, TS41, TS42, TS61, TS62;                                           // Thermocouples in °C
    uint16_t FM11, FM21, FM61;                                                          // Flow in mL/s
    float LC;                                                                           // Load cell (N)
    uint16_t ref5V;                                                                     // 5V reference (mV)

    uint32_t* valveState;       // Pointer to valvePositions: 18-bit valve state
    uint8_t actLPos, actRPos;   // Actuator positions (0-255)
    uint8_t actLOK, actROK;     // Actuator OK flags (0 or 1)

    int state;                  // System state
};

// External variable declarations
extern uint32_t n;
extern uint32_t t, t_since_save;
extern data Data;

extern bool TS41_waiting, TS42_waiting, TS61_waiting, TS62_waiting, TS11_waiting;
extern bool test;

extern Adafruit_MAX31856 thermo31;
extern Adafruit_MAX31856 thermo41;
extern Adafruit_MAX31856 thermo42;
extern Adafruit_MAX31856 thermo61;
extern Adafruit_MAX31856 thermo62;
extern Adafruit_MAX31856 thermo11;

// Limit Definitions
extern int PS_oob_max_delay;
extern int TS_oob_max_delay;

// Pressure sensor limits
extern uint16_t PS11_UL;
extern uint16_t PS12_TLW, PS12_TUW;
extern uint16_t PS21_UL;
extern uint16_t PS22_TLW, PS22_TUW;
extern uint16_t PS31_LW, PS31_UW, PS31_UL;
extern uint16_t PS41_TLL, PS41_TLW, PS41_TUW, PS41_TUL;
extern uint16_t PS42_TLL, PS42_TLW, PS42_TUW, PS42_TUL;
extern uint16_t PS51_TLL, PS51_LW, PS51_UW;
extern uint16_t PS61_TLL, PS61_UL;
extern uint16_t PS62_TLL, PS62_UL;

// Thermocouple limits
extern uint16_t TS31_UW;
extern uint16_t TS62_UW, TS62_TUL;

// Active flags for pressure sensors
extern bool PS11_UL_active, PS11_BBLW_active, PS11_BBUW_active;
extern bool PS12_TLW_active, PS12_TUW_active;
extern bool PS21_UL_active, PS21_BBLW_active, PS21_BBUW_active;
extern bool PS22_TLW_active, PS22_TUW_active;
extern bool PS31_LW_active, PS31_UW_active, PS31_UL_active;
extern bool PS41_TLL_active, PS41_TLW_active, PS41_TUW_active, PS41_TUL_active;
extern bool PS42_TLL_active, PS42_TLW_active, PS42_TUW_active, PS42_TUL_active;
extern bool PS51_TLL_active, PS51_LW_active, PS51_UW_active;
extern bool PS_WATER_TLL_active, PS_WATER_BBLW_active, PS_WATER_BBUW_active, PS_WATER_UL_active;

// Active flags for thermocouples
extern bool TS31_UW_active;
extern bool TS62_UW_active, TS62_TUL_active;

// Timestamps for pressure sensors
extern uint32_t PS11_UL_time, PS11_BBLW_time, PS11_BBUW_time;
extern uint32_t PS12_TLW_time, PS12_TUW_time;
extern uint32_t PS21_UL_time, PS21_BBLW_time, PS21_BBUW_time;
extern uint32_t PS22_TLW_time, PS22_TUW_time;
extern uint32_t PS31_LW_time, PS31_UW_time, PS31_UL_time;
extern uint32_t PS41_TLL_time, PS41_TLW_time, PS41_TUW_time, PS41_TUL_time;
extern uint32_t PS42_TLL_time, PS42_TLW_time, PS42_TUW_time, PS42_TUL_time;
extern uint32_t PS51_TLL_time, PS51_LW_time, PS51_UW_time;
extern uint32_t PS_WATER_TLL_time, PS_WATER_BBLW_time, PS_WATER_BBUW_time, PS_WATER_UL_time;

// Timestamps for thermocouples
extern uint32_t TS31_UW_time;
extern uint32_t TS62_UW_time, TS62_TUL_time;

// Functions:
void setupSensors();
void PS_for_BB();
void sensorsLoop();
void trigger_TS();
void updateData();
void values_check();
void send_data();
void save_data();

uint16_t PS_25bar_reading(int pin);
uint16_t PS_70bar_reading(int pin);
uint16_t PS_350bar_reading(int pin);
uint16_t FM11_reading(int pin);
uint16_t FM21_reading(int pin);
uint16_t FM61_reading(int pin);
float LC_reading(int pin);
uint16_t ref5V_reading(int pin);

#endif