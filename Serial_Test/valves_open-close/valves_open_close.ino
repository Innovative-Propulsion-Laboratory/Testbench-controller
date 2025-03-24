#include "Valves.h"

void setup(){
    Serial.begin(9600);
    SPI.begin();

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