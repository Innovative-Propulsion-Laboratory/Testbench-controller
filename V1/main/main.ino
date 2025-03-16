#include "Valves.h"
#include "Pressurization.h"
#include "Sensors.h"

void setup() {
    Serial.begin(9600);       //initialize Serial Port
    SPI.begin();              //initialize SPI

    setupSensors();
    setupValves();
}

void loop() {
    setValve(4, 1);  // Example usage
    delay(1000);
    setValve(4, 0);
    delay(1000);
}
