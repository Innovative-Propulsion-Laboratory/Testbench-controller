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
bool ignter_state;
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
    uint32_t Chilldown_start;
    state = "test";
    test_step = 1;
    do {
        sensorsLoop();
        Receives();
        switch test_step{
            case 1 { // beginning of GN2 purge
                if (millis() >= (T_confirm+Confirm_to_purge_delay)){
                    setValve(SV36,1);
                    test_state++;
                }
            }
            case 2 { // end of GN2 purge and beginning of LOX chilldown
                if (millis() >= (T_confirm+Confirm_to_purge_delay+Purge_duration)){
                    setValve(SV36,0);
                    Chilldown_start = millis();
                    setValve(SV13,1);
                    test_state++;
                }
            }
            case 3 {
                if (millis() >= (T_confirm+Confirm_to_purge_delay+Purge_duration+Chilldown_on_duration)){
                    setValve(SV13,0);
                    test_state++;
                }
            }
            case 4 { // check of end chilldown
                if (millis() >= (T_confirm+Confirm_to_purge_delay+Purge_duration+Chilldown_on_duration+Chilldown_off_duration)) {
                    if (chill_temp >= Data.TS11 ){ 
                        check_chill_temp = true;
                        Chilldown_end = millis();
                        Chilldown_duration = Chilldown_end - Chilldown_start;
                        test = 1;
                        test_state++;
                        // maybe send a message chilldown okay
                    } 
                    else if ((millis() - Chilldown_start) >= Max_chilldown_duration){
                        // error: failed to chilldown on time
                        state = "active";
                        // return to the active state
                    }
                    else {
                        T_confirm = millis()+Confirm_to_purge_delay+Purge_duration;
                        test_state = 2;
                    }
                }
            }
            // add more cases for the next steps
            // change the variable state to "active" and test_step back to 0 during the last step 
        }
    }while (state = "test")
    // j'ai changé les conditions du coup mais je te laisse ça là au cas où(check_chill_temp == false) or (Chilldown_duration == Max_chilldown_duration);

    if ((check_chill_temp == false) && (Chilldown_duration == Max_chilldown_duration)){
        abort(); 
        break;
    }
    else{
        PS63_duration = millis();
        do {
            sensorsLoop();
            Receives();
            if ((Chilldown_finished+Chilldown_to_cooling) >= millis()){
                setValve(SV63,1);
            }
        }while (Data.PS63 >= colling_pressure) or (PS63_duration >= PS63_max_duration);

        if (Data.PS63 <= colling_pressure) && (PS63_duration >= PS63_max_duration){
            abort();
            break;
        }
        else{
            test_state++;
            Igniter_check_duration = millis();
            ignter_state = false;
            do {
                sensorsLoop(); // comment check value abort un test ? 
                Receives(); 
                //allumer allumeur 
                if (/* allumeur alumer*/){
                    igneter_state = true
                    test_state++;
                }
                      
            }while (Igniter_check_duration >= Igniter_max_duration) or (/*allumeur allumé*/);

            if (Igniter_check_duration >= Igniter_max_duration) && (/*allumeur pas allumé*/){
                abort();
            }
            else{
                // Arreter d'allumer l'allumeur 
                T0 = millis();
                test_state++;
                do{ 
                    swicth test_state{
                        case 5 {
                            setValve(SV24,1);
                            test_state++;
                        }
                        case 4 {
                            if ((T0 + ) >= millis()) {
                                
                            }
                        }
                    }
                }while (Chilldown_finished == T) or (Chilldown_duration == Max_chilldown_duration);
            }
        }   
    }
}