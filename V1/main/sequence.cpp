#include "sequences.h"
#include "Valves.h"

Confirm_to_purge_delay = 1000;
Purge_duration = 1000;
Chilldown_on_duration = 1000;
Chilldown_off_duration = 1000;
chill_temp = 270; 
check_chill_temp = false;
Maw_chilldown_duration = 10000;


void Sequence(){
    T_confirm = micros();
    Chilldown_duration = millis();
    while (check_chill_temp == false) or (Chilldown_duration == Max_chilldown_duration){
        if (T_confirm+Confirm_to_purge_delay == micros()){
            setValve(18,1);
        }
        elif (T_confirm+Confirm_to_purge_delay+Purge_duration == micros()){
            setValve(18,0);
            setValve(2,1);
        }
        elif (T_confirm+Confirm_to_purge_delay+Purge_duration+Chilldown_on_duration == micros()){
            setValve(2,0);
        }
        elif (T_confirm+Confirm_to_purge_delay+Purge_duration+Chilldown_on_duration+Chilldown_off_duration == micros()) {
            if (chill_temp == 0){ // set the read of the thermo couple 
                check_chill_temp = true
                // maybe send a message chilldown okay
            } 
            else
            {
                T_confirm = micros();
            }
            
        }
    }
}
