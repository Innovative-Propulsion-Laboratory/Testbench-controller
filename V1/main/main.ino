#include "Valves.h"
#include "Pressurization.h"
#include "Sensors.h"

unsigned long t_last_data_packet = 0, data_send_rate = 1000;


void setup() {
    Serial.begin(9600);       //initialize Serial Port
    SPI.begin();              //initialize SPI

    setupSensors();
    setupValves();
}

void loop() {
    // listen to commands
    if ((millis() - t_last_data_packet) >= data_send_rate){
        data();
    }
    BB_pressurization();
}
