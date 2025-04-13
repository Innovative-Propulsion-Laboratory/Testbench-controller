#include "Sensors.h"

void setup(){
    Serial.begin(9600);
    SPI.begin();

    //disable other SPI devices
    pinMode(1, OUTPUT);
    pinMode(10, OUTPUT);

    digitalWrite(1, HIGH);
    digitalWrite(10, HIGH);
    
    setupValves();
    Set_valve_position();
    setupSensors();
    delay(100);
}

void loop(){
    updateData();
    trigger_TS();
    serialSend();

    delay(200);
}