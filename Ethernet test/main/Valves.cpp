#include "Valves.h"

uint32_t valvePositions = 0b011001000010001001; // read from right to left
const uint32_t valveTypes = 0b011001000010001001; // read from right to left
const int valvePins[NUM_VALVES] = {7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
Adafruit_MCP23X17 mcp;

void setupValves() {
    mcp.begin_SPI(10, 13, 12, 11);
  // configure pin for output
  for (int i=0;i<=15;i++)
  {
    mcp.pinMode(i, OUTPUT);
  }
}
  
void setValve(uint8_t ID, bool command) {
  if (ID < 0 || ID >= NUM_VALVES) return;

  bool state = ((valveTypes >> ID) & 1) ? !command : command;

  if (ID < 3) {
      digitalWrite(valvePins[ID], state);
  } else {
      mcp.digitalWrite(valvePins[ID], state);
  }

  if (command)
      {valvePositions |= (1 << ID);}
  else
     {valvePositions &= ~ (1 << ID); }
  
  Serial.println(valvePositions);
  Serial.print("Valve ");
  Serial.print(ID);
  Serial.print(" set to ");
  Serial.println(command ? "open" : "closed");

  byte message[8] = {0xEE, 0xEE, 0xFF, 0xFF, 0xFF, 0xFF, ID, (byte)command};
  reply(message,sizeof(message));
}

