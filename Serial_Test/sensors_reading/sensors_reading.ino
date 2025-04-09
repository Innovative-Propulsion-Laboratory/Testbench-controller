#include "Sensors.h"

void setup(){
    Serial.begin(9600);
    SPI.begin();

    //disable other SPI devices
    pinMode(2, OUTPUT);
    pinMode(10, OUTPUT);

    digitalWrite(2, HIGH);
    digitalWrite(10, HIGH);
    
    setupValves();
    Set_valve_position();
    setupSensors();
    delay(100);
    setValve(SV31, 1);
    delay(5000);
    //setValve(SV31, 0);
    setValve(SV32, 0);
}

void loop(){
    updateData();
    trigger_TS();
    serialSend();

    delay(200);
}