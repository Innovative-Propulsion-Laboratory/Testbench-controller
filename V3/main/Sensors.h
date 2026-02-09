#ifndef SENSORS_H
#define SENSORS_H

#include "Pressurization.h"
#include <SdFat.h>
#include <Wire.h>
#include <TCA9548.h>
#include <Adafruit_MCP9600.h>
#include <MCP3208.h>
#include <INA219.h>

// ------------------------ PIN CONFIGURATION ----------------------------------

// Pressure sensors:
#define PS11_pin A9   //LOX tank pressure
#define PS12_pin A8   //LOX injector pressure
#define PS21_pin A7   //ETH tank pressure
#define PS22_pin A6   //ETH injector pressure
#define PS23_pin A3   //IG ETH injector pressure
#define PS31_pin A2   //GN2 tank pressure
#define PS41_pin A1   //Main Combustion Chamber pressure 1
#define PS42_pin A0   //Main Combustion Chamber pressure 2
#define PS51_pin A17   //Air tank pressure
#define PS61_pin A16   //Water tank pressure 1
#define PS62_pin A17   //Water tank pressure 2
#define PS63_pin 0  //Water pressure before regen
#define PS64_pin 1  //Water pressure after regen
#define PS71_pin 2  //Water pressure after regen
#define PS81_pin 3  //Water pressure after regen
// #define PSextra_pin A15
#define PSalim_pin A14      // Pressure sensors alimentation

// Igniter
#define GP_ignite_pin 33

// PS offset
extern int32_t offset_PS12;
extern int32_t offset_PS22;
extern int32_t offset_PS23;
extern int32_t offset_PS41;
extern int32_t offset_PS42;
extern int32_t offset_PS63;
extern int32_t offset_PS64;
extern int32_t offset_PS71;
extern int32_t offset_PS81;

extern int32_t avg_PS11, avg_PS21, avg_PS61, avg_PS62, avg_PS71;

// Load cell:
#define LC01_pin A10

// Flow sensors:
#define FM11_pin A11  //LOX injection flow
#define FM21_pin A12  //ETH injection flow
#define FM61_pin A13  //Cooling flow

// Thermocouples:
#define I2C_ADDRESS (0x67)
extern Adafruit_MCP9600 mcp_TS[6];
extern TCA9548 MP;
#define TS11_pin 0
#define TS12_pin 1
#define TS41_pin 2
#define TS42_pin 3
#define TS61_pin 4
#define TS62_pin 5

extern uint32_t time_last_reading;
extern bool bool_file;
extern bool state_test_spe;
extern unsigned long t_last_data_packet, data_send_rate, test_send_rate;
extern int print;


// Structure containing all the data sent from the Teensy to the computer
struct __attribute__((packed)) data {
  byte header[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
  uint32_t n = 0;  // packet ID
  uint32_t t = 0;  // Timestamp (ms)

  int16_t PS11, PS12, PS21, PS22, PS23;
  int32_t PS31;
  int16_t PS41, PS42;
  int32_t PS51;
  int16_t PS61, PS62, PS63, PS64, PS71, PS81;
  int16_t TS11, TS12, TS41, TS42, TS61, TS62;                                      // Thermocouples in °C
  uint16_t FM11, FM21, FM61;                                                       // Flow in mL/s
  int32_t LC;                                                                      // Load cell (N)
  uint16_t ref5V, glowplug_current;                                                // 5V reference (mV)

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
  uint16_t Purge_duration2;
  uint16_t Chilldown_to_cooling;
  float cooling_pressure;
  uint16_t PS63_check_duration;
  uint16_t PS63_verified_duration;
  uint16_t Ign_to_bypass;
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
  uint16_t Purge_duration3;
  uint16_t Cooling_duration_after_end_burn;

  // Igniter 
  uint16_t GP_current;
  uint16_t Glowplug_heat_before_duration;
  uint16_t Current_raising;
  uint16_t ETH_to_GOX;
  uint16_t Igniter_chamber_pressure;
  uint16_t Igniter_pressure_timemax;
  uint16_t Igniter_Highpressure_time;
  uint16_t Igniter_burn_duration;
  uint16_t GOX_to_ETH;
};

// External variable declarations
extern data Data;
extern sequence_data Sequence_data;

// Variables for the sequence
extern uint32_t T_confirm;
extern uint32_t T_midpurge;
extern uint32_t Chilldown_finished;
extern uint32_t last_send;
extern int32_t count_down_time;
extern uint32_t PS63_duration;
extern uint32_t PS63_seems_rise;
extern uint32_t Ign_duration;
extern uint32_t Ign_seems_on;
extern uint32_t T0;
extern uint32_t ETH_open;
extern uint32_t Bypass_duration;
extern uint32_t Bypass_duration_open;
extern uint32_t Main_seems_rise;
extern uint32_t Main_duration;
extern uint32_t Nominal_pressure_reached;
extern uint32_t T_burn;
extern uint32_t Chilldown_start;
extern uint32_t Chilldown_count;
extern uint32_t chill_temp_seems_ok;
extern uint32_t Chilldown_duration;
extern uint32_t Chilldown_verified_duration;
extern uint32_t Tchilldown;

extern uint32_t Heat_start;
extern uint32_t Igniter_burn_duration;
extern uint32_t Igniter_open_duration;

extern int32_t PS71_TLW , PS71_TUW;

// Functions:
void setupSensors();
void BBLoop();
void sensorsLoop();
void valuesCheck();
void updateData();
void serialSend();
void test_abort(int type);

// Sensor reading functions
int16_t PS_25bar_reading(int pin);
int16_t PS_25bar_ADCreading(int chan);
int32_t PS_70bar_reading(int pin);
int32_t PS_350bar_reading(int pin);
void reset_offset_pressure();
void read_atm_pressure();
uint16_t FM11_reading(int pin);
uint16_t FM21_reading(int pin);
uint16_t FM61_reading(int pin);
int32_t LC_reading(int pin);
uint16_t ref5V_reading(int pin);
int16_t read_TS(int TS_index);
void disableAllChannels(TCA9548 &MP);

// SD card functions
void setupSaveData();
void newFile();
void saveData();
void save_spe_Data();
void closeFile();
String generate_csv_line();

// INA reading current
extern INA219 ina; //pour l'utiliser dans serial_controller.h
void setup_current_reading();
uint16_t GP_current_reading();

void testcapteur();

#endif