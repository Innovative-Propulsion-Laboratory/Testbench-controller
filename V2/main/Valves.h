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
#define SV31 6
#define SV32 7
#define SV33 8
#define SV34 9
#define SV35 10
#define SV36 11
#define SV51 12
#define SV52 13
#define SV53 14
#define SV61 15
#define SV62 16
#define SV63 17
#define SVXX 18

extern uint32_t valvePositions;

void setupValves();
void setValve(uint8_t ID, bool command);
void Set_valve_position();

#endif