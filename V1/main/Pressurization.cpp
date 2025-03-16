#include "Pressurization.h"
// Implementation of Bang-Bang pressurization for a specific tank and at a specific pressure

bool LOX_BB = 0, ETH_BB = 0, WATER_BB =0;

float PS11_BB_min = 15.8, PS11_BB_max = 16.2;
float PS11_BBLW = 15, PS11_BBUW = 17, PS11_BB_ulim = 22;

float PS21_BB_min = 15.8, PS21_BB_max = 16.2;
float PS21_BBLW = 15, PS21_BBUW = 17, PS21_BB_ulim = 22;

float WATER_BB_min = 9.8, WATER_BB_max = 10.2;
float WATER_BBLW = 9, WATER_BBUW = 11, WATER_BB_ulim = 12;
float PS_WATER;

void BB_param_set(int tank, float pressure){ // tank: 0 = LOX, 1 = ETH, 2 = WATER
    if (tank == 0){
        if (pressure < PS11_BB_ulim){return;}
        else{
            PS11_BB_min = pressure - 0.2;
            PS11_BBLW = pressure - 1;
            PS11_BB_max = pressure + 0.2;
            PS11_BBUW = pressure + 1;
        }
    }
    if (tank == 1){
        if (pressure < PS21_BB_ulim){ return;}
        else{
            PS21_BB_min = pressure - 0.2;
            PS21_BBLW = pressure - 1;
            PS21_BB_max = pressure + 0.2;
            PS21_BBUW = pressure + 1;
        }
    }
    if (tank == 2){
        if (pressure < WATER_BB_ulim){return;}
        else{
            WATER_BB_min = pressure - 0.2;
            WATER_BBLW = pressure - 1;
            WATER_BB_max = pressure + 0.2;
            WATER_BBUW = pressure + 1;
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

void BB_pressurization(){
    if (LOX_BB){
        if (PS11 < PS11_BB_min) {setValve(SV33, 1); setValve(SV11, 0);}
        if (PS11 > PS11_BB_max) {setValve(SV33, 0); setValve(SV11, 1);}
    }
    if (ETH_BB){
        if (PS21 < PS21_BB_min) {setValve(SV32, 1); setValve(SV21, 0);}
        if (PS21 > PS21_BB_max) {setValve(SV32, 0); setValve(SV21, 1);}
    }
    if (WATER_BB){
        PS_WATER = (PS61 + PS62)/2;
        if (PS_WATER < WATER_BB_min) {setValve(SV52, 1); setValve(SV53, 1), setValve(SV61, 0), setValve(SV62, 0);}
        if (PS_WATER > WATER_BB_max) {setValve(SV52, 0); setValve(SV53, 0), setValve(SV61, 1), setValve(SV62, 1);}
    }
}