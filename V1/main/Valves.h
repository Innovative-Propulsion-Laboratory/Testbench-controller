#ifndef VALVES_H
#define VALVES_H

#include "MCP23S17.h" // Include MCP23S17 library
#include <SPI.h>
#include <Arduino.h>
#include "UDP.h"

#define NUM_VALVES 18  // Define before using it

extern long valvePositions;
extern const long valveTypes;
extern const int valvePins[NUM_VALVES];
extern MCP23S17 MCP;

void setupValves();
void setValve(int ID, bool command);

#endif
