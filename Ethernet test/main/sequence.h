#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stdint.h>  
#include "Sensors.h"
#include <Arduino.h>

#define IGN_pin 33
#define IGN_check_pin 34

void Sequence();
void set_offset_pressure();

#endif