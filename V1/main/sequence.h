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
#endif