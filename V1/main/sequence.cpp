#include "sequences.h"
#include "Valves.h"
#include "Sensors.h"
#include "TVC.h"

uint16_t T_confirm = 1000;
uint16_t Purge_duration = 1000;
uint16_t Chilldown_on_duration = 1000;
uint32_t Chilldown_off_duration = 1000;
float chill_temp = 270;
uint16_t Chilldown_verified_duration = 100;
uint32_t Max_chilldown_duration = 10000;
uint32_t Chilldown_to_cooling = 1000;
float cooling_pressure = 3000;
uint32_t PS63_check_duration = 2000;
uint16_t PS63_verified_duration = 2000;
uint16_t Ign_check_duration = 2000;
uint16_t Ign_verified_duration = 2000;
uint16_t ETH_to_LOX_bypass = 200;
float Bypass_pressure = 200;
uint16_t Bypass_check_duration = 200;
uint16_t Bypass_verified_duration = 200;
float Main_pressure = 200;
uint16_t Main_check_duration = 2000;
uint16_t Main_verified_duration = 2000;
uint16_t burn_duration = 2000;
uint16_t LOX_to_ETH_closing_delay = 2000;
uint16_t Cooling_duration_after_end_burn = 2000;

uint16_t Confirm_to_purge_delay;

uint32_t Chilldown_finished;
uint32_t PS63_duration;
uint32_t PS63_seems_rise;
uint32_t Ign_duration;
uint32_t Ign_seems_on;
uint32_t T0;
uint32_t ETH_open;
uint32_t Bypass_duration;
uint32_t ETH_to_LOX_main;
uint32_t Main_seems_rise;
uint32_t Main_duration;
uint32_t Nominal_pressure_reached;
uint32_t TVC_pattern_duration;
uint32_t T_burn;
uint32_t Chilldown_start;
uint32_t chill_temp_seems_ok;
uint32_t Chilldown_duration = 0;

uint8_t test_step ;
String state = "active";
String active = "active";

void Sequence() {

    T_confirm = millis();
    test_step = 1;
    sate = "test"
    
    do {
        sensorsLoop();
        decode(receivePacket());

        switch (test_step) {
            case 1: {
                if (millis() >= (T_confirm + Confirm_to_purge_delay)) {
                    setValve(SV36, 1);
                    test_step++;
                }
                break;
            }
            case 2: {
                if (millis() >= (T_confirm + Confirm_to_purge_delay + Purge_duration)) {
                    setValve(SV36, 0);
                    Chilldown_start = millis();
                    setValve(SV13, 1);
                    test_step++;
                }
                break;
            }
            case 3: {
                if (millis() >= (T_confirm + Confirm_to_purge_delay + Purge_duration + Chilldown_on_duration)) {
                    setValve(SV13, 0);
                    test_step++;
                }
                break;
            }
            case 4: {
                if (millis() <= (T_confirm + Confirm_to_purge_delay + Purge_duration + Chilldown_on_duration + Chilldown_off_duration)) {
                    if (Data.TS12 >= chill_temp) {
                        chill_temp_seems_ok = millis();
                        test_step = 5;  // remplace l'étape 4.5
                    } else if ((millis() - Chilldown_start) >= Max_chilldown_duration) {
                        state = "active";  // Erreur chilldown
                    }
                } else {
                    T_confirm = millis();
                    test_step = 2;
                }
                break;
            }
            case 5: {
                if ((Data.TS12 >= chill_temp) && ((millis() - chill_temp_seems_ok) >= Chilldown_verified_duration)) {
                    Chilldown_finished = millis();
                    Chilldown_duration = Chilldown_finished - Chilldown_start;
                    test_step++;
                } else if (Data.TS12 < chill_temp) {
                    test_step = 4;
                }
                break;
            }
            case 6: {
                if (millis() >= (Chilldown_finished + Chilldown_to_cooling)) {
                    setValve(SV63, 1);
                    PS63_duration = millis();
                    test_step++;
                }
                break;
            }
            case 7: {
                if (Data.PS63 >= cooling_pressure) {
                    PS63_seems_rise = millis();
                    test_step++;
                } else if ((millis() - PS63_duration) >= PS63_check_duration) {
                    state = "active";  // erreur
                }
                break;
            }
            case 8: {
                if ((Data.PS63 >= cooling_pressure) && ((millis() - PS63_seems_rise) >= PS63_verified_duration)) {
                    test_step++;
                } else if (Data.PS63 < cooling_pressure) {
                    test_step = 7;
                }
                break;
            }
            case 9: {
                Ign_duration = millis();
                // allumer allumeur
                test_step++;
                break;
            }
            case 10: {
                if (/* allumeur allumé */) {
                    Ign_seems_on = millis();
                    test_step++;
                } else if ((millis() - Ign_duration) >= Ign_check_duration) {
                    state = "active";
                }
                break;
            }
            case 11: {
                if (/* allumeur allumé */ && ((millis() - Ign_seems_on) >= Ign_verified_duration)) {
                    // couper allumeur
                    T0 = millis();
                    setValve(SV24, 1);
                    test_step++;
                } else {
                    test_step = 10;
                }
                break;
            }
            case 12: {
                if (millis() >= (T0 + ETH_to_LOX_bypass)) {
                    Bypass_duration = millis();
                    setValve(SV13, 1);
                    test_step++;
                }
                break;
            }
            case 13: {
                if ((Data.PS41 >= Bypass_pressure) && (Data.PS42 >= Bypass_pressure)) {
                    Bypass_duration = millis();
                    test_step++;
                } else if ((millis() - Bypass_duration) >= Bypass_check_duration) {
                    state = "active";
                }
                break;
            }
            case 14: {
                if ((Data.PS41 >= Bypass_pressure) && (Data.PS42 >= Bypass_pressure) && ((millis() - Bypass_duration) >= Bypass_verified_duration)) {
                    ETH_open = millis();
                    setValve(SV22, 1);
                    test_step++;
                } else {
                    test_step = 12;
                }
                break;
            }
            case 15: {
                if (millis() >= (T0 + ETH_to_LOX_main)) {
                    Main_duration = millis();
                    setValve(SV12, 1);
                    test_step++;
                }
                break;
            }
            case 16: {
                if ((Data.PS41 >= Main_pressure) && (Data.PS42 >= Main_pressure)) {
                    Main_seems_rise = millis();
                    test_step++;
                } else if ((millis() - Main_duration) >= Main_check_duration) {
                    state = "active";
                }
                break;
            }
            case 17: {
                if ((Data.PS41 >= Main_pressure) && (Data.PS42 >= Main_pressure) && ((millis() - Main_seems_rise) >= Main_verified_duration)) {
                    test_step++;
                } else {
                    test_step = 15;
                }
                break;
            }
            case 18: {
                setValve(SV24, 0);
                setValve(SV13, 0);
                Nominal_pressure_reached = millis();
                test_step++;
                break;
            }
            case 19: {
                // TVC launch, not implemented
                test_step++;
                break;
            }
            case 20: {
                if (millis() >= (T0 + burn_duration)) {
                    setValve(SV12, 0);
                    setValve(SV36, 1);
                    test_step++;
                }
                break;
            }
            case 21: {
                if (millis() >= (T0 + burn_duration + LOX_to_ETH_closing_delay)) {
                    setValve(SV22, 0);
                    setValve(SV35, 1);
                    test_step++;
                }
                break;
            }
            case 22: {
                if (millis() >= (T0 + burn_duration + LOX_to_ETH_closing_delay + Purge_duration)) {
                    setValve(SV36, 0);
                    setValve(SV35, 0);
                    test_step++;
                }
                break;
            }
            case 23: {
                if (millis() >= (T0 + burn_duration + LOX_to_ETH_closing_delay + Purge_duration + Cooling_duration_after_end_burn)) {
                    setValve(SV63, 0);
                    state = "active";
                }
                break;
            }
        }

    } while (state == "test");
}

void set_offset_pressure() {
    const int N = 10;
    byte average_PS12_data[N];
    byte average_PS22_data[N];
    byte average_PS41_data[N];
    byte average_PS42_data[N];
    byte average_PS63_data[N];
    byte average_PS64_data[N];

    for (int i = 0; i < N; i++) {
        sensorsLoop();
        decode(receivePacket()); 
        average_PS12_data[i] = Data.PS12;
        average_PS22_data[i] = Data.PS22;
        average_PS41_data[i] = Data.PS41;
        average_PS42_data[i] = Data.PS42;
        average_PS63_data[i] = Data.PS63;
        average_PS64_data[i] = Data.PS64;  
    }

    offset_PS12 = average(average_PS12_data, N);
    offset_PS22 = average(average_PS22_data, N);
    offset_PS41 = average(average_PS41_data, N);
    offset_PS42 = average(average_PS42_data, N);
    offset_PS63 = average(average_PS63_data, N);
    offset_PS64 = average(average_PS64_data, N);      
}

float average(byte* L, int length) {
    float sum = 0;
    for (int i = 0; i < length; i++) {
        sum += L[i];
    }
    return sum / length;
}