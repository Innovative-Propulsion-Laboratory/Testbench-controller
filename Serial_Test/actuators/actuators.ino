#include "TVC.h"
#include <SPI.h>

void setup(){
    Serial.begin(9600);       //initialize Serial Port
    SPI.begin();              //initialize SPI

    //disable other SPI devices
    pinMode(10, OUTPUT);
    pinMode(28, OUTPUT);
    pinMode(29, OUTPUT);
    pinMode(30, OUTPUT);
    pinMode(35, OUTPUT);
    pinMode(36, OUTPUT);
    pinMode(37, OUTPUT);

    digitalWrite(10, HIGH);
    digitalWrite(28, HIGH);
    digitalWrite(29, HIGH);
    digitalWrite(30, HIGH);
    digitalWrite(35, HIGH);
    digitalWrite(36, HIGH);
    digitalWrite(37, HIGH);

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
