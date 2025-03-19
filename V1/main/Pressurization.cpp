#include "Pressurization.h"
// Implementation of Bang-Bang pressurization for a specific tank and at a specific pressure

bool LOX_BB = 0, ETH_BB = 0, WATER_BB =0;

uint16_t PS11_BB_min = 15800, PS11_BB_max = 16200;
uint16_t PS11_BBLW = 15, PS11_BBUW = 17, PS11_BB_ulim = 22000;

uint16_t PS21_BB_min = 15800, PS21_BB_max = 16200;
uint16_t PS21_BBLW = 15000, PS21_BBUW = 17000, PS21_BB_ulim = 22000;

uint16_t WATER_BB_min = 9800, WATER_BB_max = 10200;
uint16_t WATER_BBLW = 9000, WATER_BBUW = 11000, WATER_BB_ulim = 12000;

void BB_param_set(int tank, float pressure){ // tank: 0 = LOX, 1 = ETH, 2 = WATER
    if (tank == 0){
        if (pressure < PS11_BB_ulim){return;}
        else{
            PS11_BB_min = pressure - 200;
            PS11_BBLW = pressure - 1000;
            PS11_BB_max = pressure + 200;
            PS11_BBUW = pressure + 1000;
        }
    }
    if (tank == 1){
        if (pressure < PS21_BB_ulim){ return;}
        else{
            PS21_BB_min = pressure - 200;
            PS21_BBLW = pressure - 1000;
            PS21_BB_max = pressure + 200;
            PS21_BBUW = pressure + 1000;
        }
    }
    if (tank == 2){
        if (pressure < WATER_BB_ulim){return;}
        else{
            WATER_BB_min = pressure - 200;
            WATER_BBLW = pressure - 1000;
            WATER_BB_max = pressure + 200;
            WATER_BBUW = pressure + 1000;
        }
    }
}

void BB_enable (int tank, bool command){ // tank: 0 = LOX, 1 = ETH, 2 = WATER
    if (command == 0){
        if (tank == 0) LOX_BB = 0;
        if (tank == 1) ETH_BB = 0;
        if (tank == 2) WATER_BB = 0;
    }
    if (command == 1){
        if (tank == 0) LOX_BB = 1;
        if (tank == 1) ETH_BB = 1;
        if (tank == 2) WATER_BB = 1;
    }
}

void BB_pressurization(uint16_t PS11, uint16_t PS21, uint16_t PS61, uint16_t PS62){
    if (LOX_BB){
        if (PS11 < PS11_BB_min) {setValve(SV33, 1); setValve(SV11, 0);}
        if (PS11 > PS11_BB_max) {setValve(SV33, 0); setValve(SV11, 1);}
    }
    if (ETH_BB){
        if (PS21 < PS21_BB_min) {setValve(SV32, 1); setValve(SV21, 0);}
        if (PS21 > PS21_BB_max) {setValve(SV32, 0); setValve(SV21, 1);}
    }
    if (WATER_BB){
        if ((PS61 <= WATER_BB_min) || (PS62 <= WATER_BB_min)) {setValve(SV52, 1); setValve(SV53, 1), setValve(SV61, 0), setValve(SV62, 0);}
        if ((PS61 >= WATER_BB_max) || (PS62 >= WATER_BB_max)) {setValve(SV52, 0); setValve(SV53, 0), setValve(SV61, 1), setValve(SV62, 1);}
    }
}