#ifndef PRESSURIZATION_H
#define PRESSURIZATION_H

#include "Valves.h"
// "UDP.h" is included in "Valves.h"

extern bool LOX_BB, ETH_BB, WATER_BB;
extern uint16_t PS11_BBLW, PS11_BBUW;
extern uint16_t PS11_BB_min, PS11_BB_max; 
extern uint16_t PS21_BBLW, PS21_BBUW;
extern uint16_t PS21_BB_min, PS21_BB_max; 
extern uint16_t WATER_BBLW, WATER_BBUW;
extern uint16_t WATER_BB_min, WATER_BB_max; 

void BB_param_set(uint8_t tank, uint16_t pressure);
void BB_enable(uint8_t tank, bool command);
void BB_pressurization(int16_t PS11, int16_t PS21, int16_t PS61, int16_t PS62);

#endif