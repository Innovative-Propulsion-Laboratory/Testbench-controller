#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stdint.h>  

struct __attribute__((packed)) sequence_data {
    bool cooling_enable;
    uint16_t tvc_pattern;   
    uint16_t Confirm_to_purge_delay;
    uint16_t Purge_duration1;
    uint16_t Chilldown_on_duration;
    uint16_t Chilldown_off_duration;
    float chill_temp;
    uint16_t Max_chilldown;
    uint16_t Chilldown_to_cooling;
    float cooling_pressure;
    uint16_t PS63_check_duration;
    uint16_t PS63_verified_duration;
    uint16_t Ign_check_duration;
    uint16_t Ign_verified_duration;
    uint16_t ETH_to_LOX_bypass;
    uint16_t ETH_to_LOX_main;
    float Bypass_pressure;
    uint16_t Bypass_check_duration;
    uint16_t Bypass_verified_duration;
    float Main_pressure;
    uint16_t Main_check_duration;
    uint16_t Main_verified_duration;
    uint16_t burn_duration;
    uint16_t TVC_pattern_duration;
    uint16_t LOX_to_ETH_closing_delay;
    uint16_t Purge_duration2;
    uint16_t Cooling_duration_after_end_burn;
};

extern sequence_data Sequence_data;

void Sequence();
void set_offset_pressure();

#endif