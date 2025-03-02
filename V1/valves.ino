#include "MCP23S17.h" // https://github.com/RobTillaart/MCP23S17
#include <SPI.h>

#define NUM_VALVES 18

long valvePositions = 0b011001000010001001;
const long valveTypes = 0b011001000010001001;  // 1 = NO, 0 = NC

const int valvePins[NUM_VALVES] = {7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}; //add 15 if SVXX is used
// -------------------------- Teensy GPIOs|| MCP23S17 GPIOs ---------------------------------
MCP23S17 MCP(10, 11, 12, 13);  // SPI: CS=10, MOSI=11, MISO=12, SCK=13

void setup() {
    Serial.begin(9600);

    SPI.begin();

    if (MCP.begin()) {
        Serial.println("MCP23S17 Initialized Successfully");
    } else {
        Serial.println("MCP23S17 Initialization Failed");
    }
    delay(100);

    // setting all pins used for valves as outputs
    for (int i = 0; i < 3; i++) {pinMode(valvePins[i], OUTPUT);}
    MCP.pinMode8(0, 0x00);  //GPA
    MCP.pinMode8(1, 0x00);  //GPB
}

void setValve(int ID, bool command) {   // ID: 0-17, command: 0 = close, 1 = open
    if (ID < 0 || ID >= NUM_VALVES) return;

    bool state = ((valveTypes >> ID) & 1) ? !command : command; //toggle command if valve is NO

    if (ID < 3){
        digitalWrite(valvePins[ID], state);
    } else {
        MCP.write1(valvePins[ID], state);
    }

    if (command)
        valvePositions |= (1L << ID);  // Set bit to 1 (open)
    else
        valvePositions &= ~(1L << ID); // Set bit to 0 (closed)
}