#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stdint.h>  

struct __attribute__((packed)) sequence_data {
    uint16_t Confirm_to_purge_delay;
    uint16_t Purge_duration;
    uint16_t Chilldown_on_duration;
    uint32_t Chilldown_off_duration;
    float chill_temp;
    uint16_t Chilldown_verified_duration;
    uint32_t Max_chilldown;
    uint32_t Chilldown_to_cooling;
    float cooling_pressure;
    uint32_t PS63_check_duration;
    uint16_t PS63_verified_duration;
    uint16_t Ign_check_duration;
    uint16_t Ign_verified_duration;
    uint16_t ETH_to_LOX_bypass;
    float Bypass_pressure;
    uint16_t Bypass_check_duration;
    uint16_t Bypass_verified_duration;
    float Main_pressure;
    uint16_t Main_check_duration;
    uint16_t Main_verified_duration;
    uint16_t burn_duration;
    uint16_t LOX_to_ETH_closing_delay;
    uint16_t Cooling_duration_after_end_burn;
};

void Sequence();
void set_offset_pressure();

#endif