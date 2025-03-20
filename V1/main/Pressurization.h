#ifndef PRESSURIZATION_H
#define PRESSURIZATION_H

#include "Valves.h"
#inclue "UDP.h"

extern bool LOX_BB, ETH_BB, WATER_BB;

extern uint16_t PS11_BB_min, PS11_BB_max;
extern uint16_t PS11_BBLW, PS11_BBUW, PS11_BB_ulim;

extern uint16_t PS21_BB_min, PS21_BB_max;
extern uint16_t PS21_BBLW, PS21_BBUW, PS21_BB_ulim;

extern uint16_t WATER_BB_min, WATER_BB_max;
extern uint16_t WATER_BBLW, WATER_BBUW, WATER_BB_ulim;

void BB_param_set(int tank, float pressure);
void BB_enable(int tank, bool command);
void BB_pressurization(uint16_t PS11, uint16_t PS21, uint16_t PS61, uint16_t PS62);

#endif