#ifndef PRESSURIZATION_H
#define PRESSURIZATION_H

#include "Valves.h"
#include "UDP.h"

extern bool LOX_BB, ETH_BB, WATER_BB;
extern uint16_t PS11_BBLW, PS11_BBUW;
extern uint16_t PS21_BBLW, PS21_BBUW;
extern uint16_t WATER_BBLW, WATER_BBUW;

void BB_param_set(uint8_t tank, uint16_t pressure);
void BB_enable(uint8_t tank, bool command);
void BB_pressurization(uint16_t PS11, uint16_t PS21, uint16_t PS61, uint16_t PS62);

#endif