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
    BB_param_set(6, 4000)
    
}

void loop(){
    sensorsLoop();

    delay(200);
}