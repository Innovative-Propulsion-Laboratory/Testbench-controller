#ifndef VALVES_H
#define VALVES_H

#include <Adafruit_MCP23X17.h>  // Include MCP23S17 library
#include <SPI.h>
#include "UDP.h"

#define NUM_VALVES 18

#define SV11 0
#define SV12 1
#define SV13 2
#define SV21 3
#define SV22 4
#define SV24 5
#define SV25 6
#define SV31 7 
#define SV32 8 
#define SV33 9 
#define SV34 10 
#define SV35 11 
#define SV36 12 
#define SV51 13 
#define SV52 14 
#define SV53 15 
#define SV61 16 
#define SV62 17 
#define SV63 18 
#define SV71 19

extern uint32_t valvePositions;

void setupValves();
void setValve(uint8_t ID, bool command);
void Set_valve_position();

#endif