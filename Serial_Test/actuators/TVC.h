#ifndef TVC_H
#define TVC_H

#include <SPI.h>
#include <Arduino.h>

#define IOexp_pin 2  // Chip Select pin for TPL0202

void setupTVC();
void setNonVolatileWiper(uint8_t wiper, uint8_t value = 128);
void setWiper(uint8_t wiper, uint8_t value = 128);
void WipertoNV(byte wiper);
void NVtoWiper(byte wiper);
#endif
