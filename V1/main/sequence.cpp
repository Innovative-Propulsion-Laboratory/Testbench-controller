#include "sequences.h"
#include "Valves.h"
#include "Sensors.h"

T_confirm = 1000;
Purge_duration = 1000;
Chilldown_on_duration = 1000;
uint32_t Chilldown_off_duration = 1000;
float chill_temp = 270; 
bool check_chill_temp = false;
uint32_t Maw_chilldown_duration = 10000;
uint32_t Chilldown_to_cooling = 1000;
uint32_t colling_pressure = 3000;
uint32_t PS63_max_duration = 2000;


uint16_t Confirm_to_purge_delay;

uint16_t Chilldown_end;
uint16_t Chilldown_to_cooling;
uint16_t colling_pressure;
uint16_t PS63_max_duration;
uint16_t  PS63_duration;
uint16_t Igniter_check_duration;
uint16_t Igniter_max_duration;
bool ignte_step;
uint16_t T0;
uint16_t ETH_to_LOX_bypass;
uint16_t ETH_open;
uint16_t ETH_to_LOX_main;
uint16_t Nominal_pressure_reached;
uint16_t TVC_pattern_duration;
uint16_t T_burn;
uint16_t LOX_to_ETH_closing_delay;
uint16_t Cooling_duration_after_end_burn;


void Sequence(){ // TODO: add all the arguments of the function
    
    uint32_t T_confirm = millis();
    uint32_t Chilldown_duration;
    uint32_t Chilldown_start;_step = "test";
    test_step = 1;
    do {
        sensorsLoop();
        Receives();
        switch test_step{
            case 1 { // beginning of GN2 purge
                if (millis() >= (T_confirm+Confirm_to_purge_delay)){
                    setValve(SV36,1);
                    tes_step++;
                }
            }
            case 2 { // end of GN2 purge and beginning of LOX chilldown
                if (millis() >= (T_confirm+Confirm_to_purge_delay+Purge_duration)){
                    setValve(SV36,0);
                    Chilldown_start = millis();
                    setValve(SV13,1);
                    tes_step++;
                }
            }
            case 3 {
                if (millis() >= (T_confirm+Confirm_to_purge_delay+Purge_duration+Chilldown_on_duration)){
                    setValve(SV13,0);
                    test_setp++;
                }
            }
            case 4 { // check of end chilldown
                if (millis() >= (T_confirm+Confirm_to_purge_delay+Purge_duration+Chilldown_on_duration+Chilldown_off_duration)) {
                    if (chill_temp >= Data.TS11 ){ 
                        check_chill_temp = true;
                        Chilldown_end = millis();
                        Chilldown_duration = Chilldown_end - Chilldown_start;
                        test = 1;
                        test_step++;
                        // maybe send a message chilldown okay
                    } 
                    else if ((millis() - Chilldown_start) >= Max_chilldown_duration){
                        // error: failed to chilldown on time
                        state = "active";
                        // return to the activ_step
                    }
                    else {
                        T_confirm = millis()+Confirm_to_purge_delay+Purge_duration;
                        test_step = 2;
                    }
                }
            }
            case 5{// open water coolling
                if ( millis() >= (Chilldown_finished+Chilldown_to_cooling)){
                    setValve(SV63,1);
                    PS63_duration = millis();
                    test_setp++;
                }
            }
            case 6{
                if (Data.PS63 >= colling_pressure) { 
                    Igniter_check_duration = millis();
                    test_step++;
                    // maybe send a message chilldown okay
                } 
                else if ((millis() - PS63_duration) >= PS63_max_duration){
                    // error: failed to chilldown on time
                    state = "active";
                    // return to the activ_step
                }
            }
            case 7 {
                //allumer allumeur
                test_step++; 
            }
            case 8 {
                if (/* allumeur alumer*/){
                    test_state++;
                }    
                else if ((millis() - Igniter_check_duration) >= Igniter_max_duration){
                    // error: failed to chilldown on time
                    state = "active";
                    // return to the activ_step
                }
            }
            case 9 {
                
            }
            // add more cases for the next steps
            // change the variabl_step to "active" and test_step back to 0 during the last step 
        }
    }while (test_step = "test")
    // j'ai changé les conditions du coup mais je te laisse ça là au cas où(check_chill_temp == false) or (Chilldown_duration == Max_chilldown_duration);

}