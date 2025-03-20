#include "sequences.h"
#include "Valves.h"
#include "Sensors.h"

Confirm_to_purge_delay = 1000;
Purge_duration = 1000;
Chilldown_on_duration = 1000;
Chilldown_off_duration = 1000;
chill_temp = 270; 
check_chill_temp = false;
Maw_chilldown_duration = 10000;
Chilldown_to_cooling = 1000;
colling_pressure = 3000;
PS63_max_duration = 2000;


void Sequence(){
    T_confirm = millis();
    Chilldown_duration = millis();
    test_state = 1;
    do {
        sensorsLoop();
        Receives();
        swicth test_state{
            case 1 { 
                if ((T_confirm+Confirm_to_purge_delay) >= millis()){
                    setValve(SV36,1);
                    test_state++;
                }
            }
            case 2 {
                if (T_confirm+Confirm_to_purge_delay+Purge_duration >= millis()){
                    setValve(SV36,0);
                    setValve(SV13,1);
                    test_state++;
                }
            }
            case 3 {
                if ((T_confirm+Confirm_to_purge_delay+Purge_duration+Chilldown_on_duration) >= millis()){
                    setValve(SV13,0);
                    test_state++;
                }
            }
            case 4 {
                if ((T_confirm+Confirm_to_purge_delay+Purge_duration+Chilldown_on_duration+Chilldown_off_duration) >= millis()) {
                    if (chill_temp >= Data.TS11 ){ 
                        check_chill_temp = true;
                        Chilldown_finished = millis();
                        test = 1;
                        test_state++;
                        // maybe send a message chilldown okay
                    } 
                    else{
                        T_confirm = millis()+Confirm_to_purge_delay+Purge_duration;
                         test_state = 2;
                    }
                }
            }
        }
    }while (check_chill_temp == false) or (Chilldown_duration == Max_chilldown_duration);

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