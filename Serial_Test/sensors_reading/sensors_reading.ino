#include "Sensors.h"

void setup(){
    Serial.begin(9600);
    SPI.begin();
    setupSensors();
}

void loop(){
    updateData();
    trigger_TS();
    serialSend();
}