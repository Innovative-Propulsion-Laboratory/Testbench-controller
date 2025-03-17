#ifndef TVC_H
#define TVC_H

#include <SPI.h>

#define IOexp_pin 2  // Chip Select pin for TPL0202

void setupTVC();
void setNonVolatileWiper(byte wiper, float percentage = 50);
void setWiper(byte wiper, float percentage = 50);
void WipertoNV(byte wiper);
void NVtoWiper(byte wiper);
