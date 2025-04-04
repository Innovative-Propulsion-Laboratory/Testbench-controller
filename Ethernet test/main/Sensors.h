#ifndef SENSORS_H
#define SENSORS_H

#include "Valves.h"
#include "Pressurization.h"
// #include "SaveData.h"
#include "UDP.h"
#include <Arduino.h>
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
struct __attribute__((packed)) data {
    byte header[4] = {0xFF,0xFF,0xFF,0xFF};
    uint32_t n = 0;                 // packet ID
    uint32_t t = 0;                 // Timestamp (ms)
    
    uint16_t PS11, PS12, PS21, PS22, PS31, PS41, PS42, PS51, PS61, PS62, PS63, PS64;    // Pressure in mbar
    uint32_t TS11, TS31, TS41, TS42, TS61, TS62;                                           // Thermocouples in °C
    uint16_t FM11, FM21, FM61;                                                          // Flow in mL/s
    uint32_t LC;                                                                           // Load cell (N)
    uint16_t ref5V;                                                                     // 5V reference (mV)

    uint32_t valvesState;
    uint8_t actLPos =0 , actRPos = 0;   // Actuator positions (0-255)
    uint8_t actLOK, actROK;     // Actuator OK flags (0 or 1)

    uint8_t state = 0;                  // System state ;  0 = active ; 1 = test ; 2 = emergy exit
    uint8_t test_step = 0;
    bool test_cooling = 1; // 0 = no cooling ; 1 = cooling
};

// External variable declarations
extern data Data;

// Functions:
void setupSensors();
void BBLoop();
void sensorsLoop();
void values_check();
void trigger_TS();
void updateData();
void serialSend();

uint16_t PS_25bar_reading(int pin);
uint16_t PS_70bar_reading(int pin);
uint16_t PS_350bar_reading(int pin);
uint16_t FM11_reading(int pin);
uint16_t FM21_reading(int pin);
uint16_t FM61_reading(int pin);
float LC_reading(int pin);
uint16_t ref5V_reading(int pin);


void sendDataFromSensor(data* d);




#endif