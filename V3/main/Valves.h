#ifndef VALVES_H
#define VALVES_H

#include <Adafruit_MCP23X17.h>  // Include MCP23S17 library
#include <SPI.h>
#include "UDP.h"

#define NUM_VALVES 20

#define MCP_RESET 10
#define MCP_CS    28

#define SV11 0 // NO
#define SV12 1 // NC
#define SV13 2 // NC
#define SV21 5// NO
#define SV22 6 // NC
#define SV24 3 // NC
#define SV25 7 // NC
#define SV31 8 // NC
#define SV32 9 // NO
#define SV33 10 // NC
#define SV34 11 // NC
#define SV35 12 // NC
#define SV36 13 // NC
#define SV51 14 // NO
#define SV52 15 // NC
#define SV53 16 // NC
#define SV61 17 // NO
#define SV62 18 // NO
#define SV63 19 // NC
#define SV71 4 // NC

extern uint32_t valvePositions;
extern Adafruit_MCP23X17 mcp;

void setupValves();
void setValve(uint8_t ID, bool command);
void Set_valve_position();
void testValves();

#endif