// Code for the MCP4261 Digital Potentiometer
#include "TVC.h"

MCP4261 digiPot(IOexp_pin, 32, 11, 12, 13);

const uint32_t SPI_SPEED = 8000000;     //Set the SPI clock speed

void setupTVC() {
    digiPot.begin();
    digiPot.setSPIspeed(SPI_SPEED);

    // Setting the wipers to a default value (50%)
    digiPot.setValue(0, 128);
    digiPot.setValue(1, 128);
}

void setNonVolatileWiper(uint8_t wiper, uint8_t value) {
    digiPot.setValueNV(wiper, value);
}

void setWiper(uint8_t wiper, uint8_t value) {
    digiPot.setValue(wiper, value);
}

void WipertoNV(byte wiper) {
    // Read current volatile wiper value
    uint8_t value = digiPot.getValue(wiper);
    // Write to non-volatile register
    digiPot.setValueNV(wiper, value);
}

void NVtoWiper(byte wiper) {
    // Read value from non-volatile register
    uint8_t value = digiPot.getValueNV(wiper);
    // Write to volatile wiper
    digiPot.setValue(wiper, value);
}
