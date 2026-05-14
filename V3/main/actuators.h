#ifndef ACTUATORS_H
#define ACTUATORS_H

#include "MCP4261.h"
#include <SPI.h>
#include <Arduino.h>

#define ADC_CS    37
#define SPI_SCK   13
#define SPI_MOSI  11
#define SPI_MISO  12


extern MCP4261 pot;
extern int valeur;

void deselectSpiDevices();
void ActSetup();
bool writePotRegister(uint8_t reg, uint16_t value);
bool writePotWiper(uint8_t wiper, uint16_t value);
void applyValue();
void normalActuators();
void setActL(uint8_t valeur);
void setActR(uint8_t valeur);
void setActuators(uint8_t valeurL, uint8_t valeurR);

#endif