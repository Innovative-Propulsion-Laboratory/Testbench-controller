#ifndef PRESSURIZATION_H
#define PRESSURIZATION_H

#include "Valves.h"

// Valves to control LOX pressurization
#define SV11 0
#define SV33 8

// Valves to control ETH pressurization
#define SV21 3
#define SV32 7

// Valves to control WATER pressurization
#define SV61 15
#define SV62 16
#define SV52 13
#define SV53 14

extern bool LOX_BB, ETH_BB, WATER_BB;

extern float PS11_BB_min, PS11_BB_max;
extern float PS11_BBLW, PS11_BBUW, PS11_BB_ulim;

extern float PS21_BB_min, PS21_BB_max;
extern float PS21_BBLW, PS21_BBUW, PS21_BB_ulim;

extern float WATER_BB_min, WATER_BB_max;
extern float WATER_BBLW, WATER_BBUW, WATER_BB_ulim;

extern float PS11, PS21, PS61, PS62;
extern float PS_WATER;

void BB_param_set(int tank, float pressure);
void BB_enable(int tank, bool command);
void BB_pressurization();

#endif