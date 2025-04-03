#include "Sensors.h"

uint32_t time_last_reading = 0;

void setup(){
    Serial.begin(9600);
    SPI.begin();

    //disable other SPI devices
    pinMode(2, OUTPUT);
    pinMode(10, OUTPUT);

    digitalWrite(2, HIGH);
    digitalWrite(10, HIGH);
    
    setupSensors();
    setupValves();
    setupValvesPosition();

    setValve(SV22, 0);
    setValve(SV24, 0);
    setValve(SV31, 1);
    setValve(SV32, 0);

    BB_param_set(2, 4000);
    BB_enable (2, 1);
}

void loop(){
    if (millis() - time_last_reading >= 1000){
      sensorsLoop();
      time_last_reading = millis();
    }
    BBLoop(); 
}