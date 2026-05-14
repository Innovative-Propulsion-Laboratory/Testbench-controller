#ifndef TVC_H
#define TVC_H

#include "MCP4261.h"
#include <SPI.h>
#include <Arduino.h>

void shape(int shape);
void Cross();
void Circle();
void Square();
void UpandDown();
void LeftandRight();
void normalActuators();

#endif