#include "Valves.h"

long valvePositions = 0b011001000010001001;
const long valveTypes = 0b011001000010001001;
const int valvePins[NUM_VALVES] = {7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
MCP23S17 MCP(10, 11, 12, 13);  // SPI: CS=10, MOSI=11, MISO=12, SCK=13

void setupValves() {
    if (MCP.begin()) {
        Serial.println("MCP23S17 Initialized Successfully");
    } else {
        Serial.println("MCP23S17 Initialization Failed");
    }
    delay(100);

    for (int i = 0; i < 3; i++) {
        pinMode(valvePins[i], OUTPUT);
    }
    MCP.pinMode8(0, 0x00);  // GPA
    MCP.pinMode8(1, 0x00);  // GPB
}

void setValve(int ID, bool command) {
    if (ID < 0 || ID >= NUM_VALVES) return;

    bool state = ((valveTypes >> ID) & 1) ? !command : command;

    if (ID < 3) {
        digitalWrite(valvePins[ID], state);
    } else {
        MCP.write1(valvePins[ID], state);
    }

    if (command)
        valvePositions |= (1L << ID);
    else
        valvePositions &= ~(1L << ID);
    Serial.println("Setting Valve");

    byte instruction[2] = {ID, command};
    reply(0,instruction,sizeof(instruction));
}
