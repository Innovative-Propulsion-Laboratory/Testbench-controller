#include "Sensors.h"

void setup(){
    Serial.begin(9600);
    SPI.begin();

    //disable other SPI devices
    pinMode(2, OUTPUT);
    pinMode(10, OUTPUT);

    digitalWrite(2, HIGH);
    digitalWrite(10, HIGH);
    
    setupSensors();
}

void loop(){
    updateData();
    trigger_TS();
    serialSend();

    delay(200);
}