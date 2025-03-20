#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stdint.h>  

uint32_t T_confirm;
uint16_t Confirm_to_purge_delay;
uint16_t Purge_duration;
uint16_t Chilldown_on_duration;
uint16_t Chilldown_off_duration;
uint16_t Max_chilldown_duration;
uint16_t Chilldown_duration;
int chill_temp;
bool check_chill_temp;

uint16_t Chilldown_finished;
uint16_t Chilldown_to_cooling;
uint16_t colling_pressure;
uint16_t PS63_max_duration;
uint16_t  PS63_duration;
uint16_t Igniter_check_duration;
uint16_t Igniter_max_duration;
bool ignter_state;
bool Test_on;
int test_state;
uint16_t T0;
uint16_t ETH_to_LOX_bypass;
uint16_t ETH_open;
uint16_t ETH_to_LOX_main;
uint16_t Nominal_pressure_reached;
uint16_t TVC_pattern_duration;
uint16_t T_burn;
uint16_t LOX_to_ETH_closing_delay;
uint16_t Cooling_duration_after_end_burn;

#endif