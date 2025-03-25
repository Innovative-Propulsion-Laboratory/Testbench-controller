#include "Valves.h"

void setup(){
    Serial.begin(9600);
    SPI.begin();

    //disable other SPI devices
    pinMode(2, OUTPUT);
    pinMode(28, OUTPUT);
    pinMode(29, OUTPUT);
    pinMode(30, OUTPUT);
    pinMode(35, OUTPUT);
    pinMode(36, OUTPUT);
    pinMode(37, OUTPUT);

    digitalWrite(2, HIGH);
    digitalWrite(28, HIGH);
    digitalWrite(29, HIGH);
    digitalWrite(30, HIGH);
    digitalWrite(35, HIGH);
    digitalWrite(36, HIGH);
    digitalWrite(37, HIGH);

    setupValves();
}

void loop(){
    for (int i = 0; i < NUM_VALVES; i++){
        setValve(i, true);
        delay(1000);
        setValve(i, false);
        delay(1000);
    }
}