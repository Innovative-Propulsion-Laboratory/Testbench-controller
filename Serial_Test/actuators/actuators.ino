#include "TVC.h"
#include <SPI.h>

void setup(){
    Serial.begin(9600);       //initialize Serial Port
    SPI.begin();              //initialize SPI
    setupTVC();
}

void loop(){
    Serial.println("Setting left actuator to 50%");
    setWiper (0, 128);
    delay(5000);
    Serial.println("Setting left actuator to 95%");
    setWiper (0, 240);
    delay(5000);
    Serial.println("Setting left actuator to 5%");
    setWiper (0, 14);
    delay (5000);

    Serial.println("Setting right actuator to 50%");
    setWiper (1, 128);
    delay(5000);
    Serial.println("Setting right actuator to 95%");
    setWiper (1, 240);
    delay(5000);
    Serial.println("Setting right actuator to 5%");
    setWiper (1, 14);
    delay (5000);
}
