#include "Valves.h"

uint32_t valvePositions =   0b01100100001000100001;    // read from right to left
const uint32_t valveTypes = 0b01100100001010100001;  // read from right to left
const int valvePins[NUM_VALVES] = {6, 7, 8, 9, 29, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
Adafruit_MCP23X17 mcp;


void setupValves() {
  /*
  Setup for the IO expander and the GPIOs conncected to the valves
  */
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(29, OUTPUT);
  // reset MCP23S17 during the setup
  pinMode(MCP_RESET, OUTPUT);
  digitalWrite(MCP_RESET, HIGH);
  delay(10);
  digitalWrite(MCP_RESET, LOW);
  delay(1500);
  digitalWrite(MCP_RESET, HIGH);
  delay(10);

  pinMode(MCP_CS, OUTPUT);
  digitalWrite(MCP_CS, HIGH);

  // Init MCP (SPI logiciel). Si A0/A1/A2 = GND => hw_addr = 0
  if (!mcp.begin_SPI(MCP_CS, 13, 12, 11, 0)) {// bool begin_SPI(cs_pin,sck_pin,miso_pin,mosi_pin, uint8_t _hw_addr = 0x00);         
    Serial.println("MCP23X17 init FAILED (SPI) !");
    while (1) delay(10);
  }
  else{Serial.println("MCP23X17 init succeed (SPI) !");}

  // configure TOUS les pins 0..15 en sortie
  for (int i = 0; i < 15; i++) {
    mcp.pinMode(i, OUTPUT);
  }

  Serial.print("GPIOAB="); Serial.println(mcp.readGPIOAB(), HEX);
  mcp.writeGPIOAB(0xFFFF); delay(20);
  Serial.print("GPIOAB="); Serial.println(mcp.readGPIOAB(), HEX);
  mcp.writeGPIOAB(0x0000); delay(20);
  Serial.print("GPIOAB="); Serial.println(mcp.readGPIOAB(), HEX);
}

void setValve(uint8_t ID, bool command) {
  /*
  Set the valve to the desired position
  ID: valve ID (0-19)
  command: 1 -> open
  command: 0 -> close
  */
  Serial.printf("ID:%d\n",ID);
  Serial.printf("Pin:%d\n",valvePins[ID]);
  if (ID < 0){Serial.println("ID incorrrect");return;} // Check if ID correct

  bool state = ((valveTypes >> ID) & 1) ? !command : command; // Check if the valve is NO or NC


  if (ID < 5) {
    digitalWrite(valvePins[ID], state); // Valves SV11, SV12, SV13, SV25 are connected to GPIOs
  } else {
    mcp.digitalWrite(valvePins[ID], state); // the remaining valves are connected to the IO expander
  }

  // Update valvesPositions
  if (command) {
    valvePositions |= (1 << ID);
  } else {
    valvePositions &= ~(1 << ID);
  }

  // Serial.println(valvePositions);
  // Serial.print("Valve ");
  // Serial.print(ID);
  // Serial.print(" set to ");
  // Serial.println(command ? "open" : "closed");
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
  setValve(SV25, 0);
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
  setValve(SV71, 0);
}

void testValves() {
  /*
  Test all the valves by activating
  */
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(29, HIGH);
  for (int i = 0; i <= 14; i++) {
    mcp.digitalWrite(i, HIGH);
  }
  
}