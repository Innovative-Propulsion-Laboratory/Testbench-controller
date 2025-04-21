#ifndef SENSORS_H
#define SENSORS_H

#include "Pressurization.h"
#include <SdFat.h>
#include <Arduino.h>
#include <Adafruit_MAX31856.h>  // https://github.com/adafruit/Adafruit_MAX31856


// ------------------------ PIN CONFIGURATION ----------------------------------

// Pressure sensors:
#define PS11_pin A9   //LOX tank pressure
#define PS12_pin A8   //LOX injector pressure
#define PS21_pin A7   //ETH tank pressure
#define PS22_pin A6   //ETH injector pressure
#define PS31_pin A5   //GN2 tank pressure
#define PS41_pin A4   //Main Combustion Chamber pressure 1
#define PS42_pin A3   //Main Combustion Chamber pressure 2
#define PS51_pin A2   //Air tank pressure
#define PS61_pin A1   //Water tank pressure 1
#define PS62_pin A0   //Water tank pressure 2
#define PS63_pin A17  //Water pressure before regen
#define PS64_pin A16  //Water pressure after regen
// #define PSextra_pin A15
#define PSalim_pin A14      // Pressure sensors alimentation

// PS offset
extern int32_t offset_PS12;
extern int32_t offset_PS22;
extern int32_t offset_PS41;
extern int32_t offset_PS42;
extern int32_t offset_PS63;
extern int32_t offset_PS64;

extern int32_t avg_PS11, avg_PS21, avg_PS61, avg_PS62;

// Load cell:
#define LC01_pin A10

// Flow sensors:
#define FM11_pin A11  //LOX injection flow
#define FM21_pin A12  //ETH injection flow
#define FM61_pin A13  //Cooling flow

// Thermocouples:
#define TS11_pin 28  // LOX temperature
#define TS12_pin 29  // GN2 50bar tank temperature
#define TS41_pin 30  // Main Combustion Chamber temperature 1
#define TS42_pin 35  // Main Combustion Chamber temperature 2
#define TS61_pin 36  // Water initial temperature
#define TS62_pin 37  // Water final temperature

extern uint32_t time_last_reading;
extern unsigned long t_last_data_packet, data_send_rate;


// Structure containing all the data sent from the Teensy to the computer
struct __attribute__((packed)) data {
  byte header[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
  uint32_t n = 0;  // packet ID
  uint32_t t = 0;  // Timestamp (ms)

  int16_t PS11, PS12, PS21, PS22;
  int32_t PS31;
  int16_t PS41, PS42;
  int32_t PS51;
  int16_t PS61, PS62, PS63, PS64;
  int16_t TS11, TS12, TS41, TS42, TS61, TS62;                                      // Thermocouples in °C
  uint16_t FM11, FM21, FM61;                                                        // Flow in mL/s
  int32_t LC;                                                                      // Load cell (N)
  uint16_t ref5V;                                                                   // 5V reference (mV)

  uint32_t valvesState;
  uint8_t actLPos = 0, actRPos = 0;  // Actuator positions (0-255)
  uint8_t actLOK, actROK;            // Actuator OK flags (0 or 1)

  uint8_t state = 0;  // System state ;  0 = active ; 1 = test ; 2 = emergy exit
  uint8_t test_step = 0;
  bool test_cooling = 1;  // 0 = no cooling ; 1 = cooling
};

// Structure containing all the parameters for the sequence
struct __attribute__((packed)) sequence_data {
  uint16_t tvc_pattern;
  uint16_t Confirm_to_purge_delay;
  uint16_t Purge_duration1;
  uint16_t Chilldown_on_duration;
  uint16_t Chilldown_off_duration;
  float chill_temp;
  uint16_t Max_chilldown;
  uint16_t Chilldown_to_cooling;
  float cooling_pressure;
  uint16_t PS63_check_duration;
  uint16_t PS63_verified_duration;
  uint16_t Ign_check_duration;
  uint16_t Ign_verified_duration;
  uint16_t ETH_to_LOX_bypass;
  uint16_t ETH_to_LOX_main;
  float Bypass_pressure;
  uint16_t Bypass_check_duration;
  uint16_t Bypass_verified_duration;
  float Main_pressure;
  uint16_t Main_check_duration;
  uint16_t Main_verified_duration;
  uint16_t burn_duration;
  uint16_t TVC_pattern_duration;
  uint16_t LOX_to_ETH_closing_delay;
  uint16_t Purge_duration2;
  uint16_t Cooling_duration_after_end_burn;
};

// External variable declarations
extern data Data;
extern sequence_data Sequence_data;

// Variables for the sequence
extern uint16_t T_confirm;
extern uint16_t Chilldown_finished;
extern uint16_t last_send;
extern uint16_t count_down_time;
extern uint16_t PS63_duration;
extern uint16_t PS63_seems_rise;
extern uint16_t Ign_duration;
extern uint16_t Ign_seems_on;
extern uint16_t T0;
extern uint16_t ETH_open;
extern uint16_t Bypass_duration;
extern uint16_t Main_seems_rise;
extern uint16_t Main_duration;
extern uint16_t Nominal_pressure_reached;
extern uint16_t T_burn;
extern uint16_t Chilldown_start;
extern uint16_t Chilldown_count;
extern uint16_t chill_temp_seems_ok;
extern uint16_t Chilldown_duration;
extern uint16_t Chilldown_verified_duration;
extern uint16_t Tchilldown;

// Functions:
void setupSensors();
void BBLoop();
void sensorsLoop();
void valuesCheck();
void trigger_TS();
void updateData();
void serialSend();
void test_abort();

// Sensor reading functions
int16_t PS_25bar_reading(int pin);
int32_t PS_70bar_reading(int pin);
int32_t PS_350bar_reading(int pin);
void reset_offset_pressure();
void read_atm_pressure();
uint16_t FM11_reading(int pin);
uint16_t FM21_reading(int pin);
uint16_t FM61_reading(int pin);
int32_t LC_reading(int pin);
uint16_t ref5V_reading(int pin);

// SD card functions
void setupSaveData();
void newFile();
void saveData();
void closeFile();
String generate_csv_line();

#endif