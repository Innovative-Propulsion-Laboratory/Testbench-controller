// Implementation of Bang-Bang pressurization for a specific tank and at a specific pressure

bool LOX_BB = 0, ETH_BB = 0, WATER_BB =0;

float LOX_BB_min, LOX_BB_max;
float ETH_BB_min, ETH_BB_max;
float WATER_BB_min, WATER_BB_max;

float LOX_BB_llim = 20, LOX_BB_ulim = 25;
float ETH_BB_llim = 20, ETH_BB_ulim = 25;
float WATER_BB_llim = 20, WATER_BB_ulim = 25;

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

void BB_param_set(int tank, float pressure){ // tank: 0 = LOX, 1 = ETH, 2 = WATER
    if (tank == 0){
        if (LOX_BB_llim < pressure || pressure < LOX_BB_ulim){reply('LOX pressure out of bounds'); return;}
        else{
            LOX_BB_min = pressure - 0.5;
            LOX_BB_max = pressure + 0.5;
        }
    }
    if (tank == 1){
        if (ETH_BB_llim < pressure || pressure < ETH_BB_ulim){reply('ETH pressure out of bounds'); return;}
        else{
            ETH_BB_min = pressure - 0.5;
            ETH_BB_max = pressure + 0.5;
        }
    }
    if (tank == 2){
        if (WATER_BB_llim < pressure || pressure < WATER_BB_ulim){reply('WATER pressure out of bounds'); return;}
        else{
            WATER_BB_min = pressure - 0.5;
            WATER_BB_max = pressure + 0.5;
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
        if (PS11 < LOX_BB_min) {setValve(SV33, 1); setValve(SV11, 0);}
        if (PS11 > LOX_BB_max) {setValve(SV33, 0); setValve(SV11, 1);}
    }
    if (ETH_BB){
        if (PS21 < ETH_BB_min) {setValve(SV32, 1); setValve(SV21, 0);}
        if (PS21 > ETH_BB_max) {setValve(SV32, 0); setValve(SV21, 1);}
    }
    if (WATER_BB){
        PS_WATER = (PS61 + PS62)/2;
        if (PS_WATER < WATER_BB_min) {setValve(SV52, 1); setValve(SV53, 1), setValve(SV61, 0), setValve(SV62, 0);}
        if (PS_WATER > WATER_BB_max) {setValve(SV52, 0); setValve(SV53, 0), setValve(SV61, 1), setValve(SV62, 1);}
    }
}