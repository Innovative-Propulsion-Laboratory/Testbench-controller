#include "Valves.h"

uint32_t valvePositions = 0b011001000010001001;    // read from right to left
const uint32_t valveTypes = 0b011001000010001001;  // read from right to left
const int valvePins[NUM_VALVES] = {7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
Adafruit_MCP23X17 mcp;

void setupValves() {
  /*
  Setup for the IO expander and the GPIOs conncected to the valves
  */
  // pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  // reset MCP23S17 during the setup
  // digitalWrite(9, LOW);
  // delay(100);
  // digitalWrite(9, HIGH);
  // delay(100);

  mcp.begin_SPI(10, 13, 12, 11);

  // configure pin for output
  for (int i = 0; i <= 15; i++) {
    mcp.pinMode(i, OUTPUT);
  }
}

void setValve(uint8_t ID, bool command) {
  /*
  Set the valve to the desired position
  ID: valve ID (0-17)
  command: 1 -> open
  command: 0 -> close
  */
  if (ID < 0 || ID >= NUM_VALVES) return; // Check if ID correct

  bool state = ((valveTypes >> ID) & 1) ? !command : command; // Check if the valve is NO or NC

  if (ID < 3) {
    digitalWrite(valvePins[ID], state); // Valves SV11, SV12, SV13 are connected to GPIOs
  } else {
    mcp.digitalWrite(valvePins[ID], state); // the remaining valves are connected to the IO expander
  }

  // Update valvesPositions
  if (command) {
    valvePositions |= (1 << ID);
  } else {
    valvePositions &= ~(1 << ID);
  }

  Serial.println(valvePositions);
  Serial.print("Valve ");
  Serial.print(ID);
  Serial.print(" set to ");
  Serial.println(command ? "open" : "closed");
}

void Set_valve_position() {
  /*
  Set all the valves to their default position: inactive
  for NO valves: open
  for NC valves: closed
  */

  setValve(SV11, 1);
  setValve(SV12, 0);
  setValve(SV13, 0);
  setValve(SV21, 1);
  setValve(SV22, 0);
  setValve(SV24, 0);
  setValve(SV31, 0);
  setValve(SV32, 1);
  setValve(SV33, 0);
  setValve(SV34, 0);
  setValve(SV35, 0);
  setValve(SV36, 0);
  setValve(SV51, 1);
  setValve(SV52, 0);
  setValve(SV53, 0);
  setValve(SV61, 1);
  setValve(SV62, 1);
  setValve(SV63, 0);
}