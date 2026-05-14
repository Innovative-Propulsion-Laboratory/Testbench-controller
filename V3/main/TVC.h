#ifndef TVC_H
#define TVC_H

#include "MCP4261.h"
#include <SPI.h>
#include <Arduino.h>

void loopTVC(int shape);
void shapeTVC(int shape);
void shape(int shape);
void Triangle();
void Cross();
void Circle();
void Square();
void UpandDown();
void LeftandRight();

#endif