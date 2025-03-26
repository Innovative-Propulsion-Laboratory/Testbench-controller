#include "sequences.h"
#include "Valves.h"
#include "Sensors.h"
#include "TVC.h"

uint16_t T_confirm = 1000;
uint16_t Purge_duration = 1000;
uint16_t Chilldown_on_duration = 1000;
uint32_t Chilldown_off_duration = 1000;
float chill_temp = 270; 
uint16_t Chilldown_verifed_duration = 100;
uint32_t Maw_chilldown_duration = 10000;
uint32_t Chilldown_to_cooling = 1000;
float colling_pressure = 3000;
uint32_t PS63_check_duration = 2000;
uint16_t PS63_verified_duration = 2000;
uint16_t Ign_check_duration = 2000;
uint16_t Ign_verified_duration = 2000;
uint16_t ETH_to_LOX_bypass = 200;
float Bypass_pressure= 200;
uint16_t Bypass_check_duration = 200;
uint16_t Bypass_verifed_duration = 200;
float Main_pressure = 200;
uint16_t Main_check_duration = 2000;
uint16_t Main_verified_duration = 2000;
burn_duration = 2000;
uint16_t LOX_to_ETH_closing_delay = 2000;
uint16_t Cooling_duration_after_end_burn = 2000;

uint16_t Confirm_to_purge_delay;

uint16_t Chilldown_finished;

uint16_t PS63_duration;
uint16_t PS63_seems_rise;

uint16_t Ign_duration;
uint16_t Ign_seems_on;
uint16_t T0;
uint16_t ETH_open;
uint16_t Bypass_duration;
uint16_t ETH_to_LOX_main;

uint16_t Main_seems_rise ;
uint16_t Main_duration;

uint16_t Nominal_pressure_reached;
uint16_t TVC_pattern_duration;
uint16_t T_burn;




void Sequence(){ // TODO: add all the arguments of the function
    
    uint32_t T_confirm = millis();
    uint32_t Chilldown_duration;
    active = "test";
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
                if (millis() <= (T_confirm+Confirm_to_purge_delay+Purge_duration+Chilldown_on_duration+Chilldown_off_duration)) {
                    if (chill_temp <= Data.TS12 ){ 
                        chill_temp_seems_ok = millis();
                        test_step = test_step + 0.5;
                        // maybe send a message chilldown okay
                    } 
                    else if ((millis() - Chilldown_start) >= Max_chilldown_duration){
                        // error: failed to chilldown on time
                        state = "active";
                        // return to the activ_step
                    }
                    else {
                        T_confirm = millis();
                        test_step = 2;
                    }
                }
            }
            case 4.5{
                if (Data.TS12 >= chill_temp) && (chill_temp_seems_ok >= Chilldown_verifed_duration){
                    Chilldown_finished = millis();
                    Chilldown_duration = Chilldown_finished - Chilldown_start;
                    test = 1;
                    test_state = test_state + 0.5;
                }
                else if (Data.TS41 < chill_temp){
                    test_state = 4;
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
                    PS63_seems_rise = millis();
                    test_step++;
                    // maybe send a message chilldown okay
                } 
                else if ((millis() - PS63_duration) >= PS63_check_duration){
                    // error: failed to chilldown on time
                    state = "active";
                    // return to the activ_step
                }
            }
            case 7{
                if (Data.PS63 >= colling_pressure) && (PS63_seems_rise >= PS63_verified_duration){
                    test_step++;
                }
                else if (Data.PS63 < colling_pressure){
                    test_step = 6;
                }
                
            }
            case 8 {
                Ign_duration = millis();
                //allumer allumeur
                test_step++; 
            }
            case 9 { // Check igner and open Bypass ETH
                if (/* allumeur alumer*/){
                    Ign_seems_on = millis(); 
                }    
                else if ((millis() - Ign_duration) >= Ign_check_duration){
                    // error: failed to chilldown on time
                    state = "active";
                    // return to the activ_step
                }
            }
            case 10 {
                if (/*allumeur alumer*/) && (Ign_seems_on >= Ign_verified_duration){
                    // couper alimentation igneter 
                    test_state++;
                    T0 = millis();
                    setValve(SV24,1);
                }
                else if (/*allumeur eteins*/){
                    test_state = 9;
                }
            }
            case 11 { // open bypass LOX
                if (millis() >= (T0 + ETH_to_LOX_bypass)){
                    Bypass_duration = millis();
                    setValve(SV13,1);
                    test_state++;
                }
            }
            case 12 { // check if bypass rise the pressure
                if (Data.PS41 >= Bypass_pressure) && (Data.PS42 >Bypass_pressure){
                    Bypass_seems_rise = millis();
                    test_state++;

                }
                else if ((millis() - Bypass_duration) >= Bypass_check_duration){
                    // error: failed to rise pressure of bypass on time
                    state = "active";
                    // return to the activ_step
                }
            }
            case 13 { // check if ETH rise the pressure during as time
                if (Data.PS41 >= Bypass_pressure) && (Data.PS42 >= Bypass_pressure) && (Bypass_seems_rise >= Bypass_verifed_duration){
                    ETH_open = millis();
                    setValve(SV22,1);
                    test_state++;
                }
                else if (Data.PS41 < Bypass_pressure) or (Data.PS42 < Bypass_pressure){
                    test_state = 11;
                }
            }
            case 14 {
                if (millis()>= (T0+ETH_to_LOX_main)){
                    Main_duration = millis();
                    setValve(SV12,1);
                    test_state++;
                }
            }
            case 15 {
                if (Data.PS41 >= Main_pressure) && (Data.PS42 >Main_pressure){
                    Main_seems_rise = millis();
                    test_state++;

                }
                else if ((millis() - Main_duration) >= Main_check_duration){
                    // error: failed to rise pressure of bypass on time
                    state = "active";
                    // return to the activ_step
                }

            }
            case 16 {
                if (Data.PS41 >= Main_pressure) && (Data.PS42 >= Main_pressure) && (Main_seems_rise >= Main_verifed_duration){
                    test_state++;
                }
                else if (Data.PS41 < Main_pressure) or (Data.PS42 < Main_pressure){
                    test_state = 11;
                }
            }
            case 17 {
                setValve(SV24,0);
                setValve(SV13,0);
                Nominal_pressure_reached = millis();
                state_state++;
                
            }
            case 18 {
                /*Begin TVC*/
                /*TVC stop*/
                state_state++;
            }
            case 19{
                if (T0 + burn_duration){
                    setValve(SV12,0);
                    setValve(SV36,1);
                    test_state++;
                }
            }
            case 20{
                if (T0 + burn_duration + LOX_to_ETH_closing_delay){
                    setValve(SV22,0);
                    setValve(SV35,1);
                    test_state++;
                }
            }
            case 21{
                if (T0 + burn_duration + LOX_to_ETH_closing_delay + Purge_duration){
                    setValve(SV36,0);
                    setValve(SV35,0);
                    test_state++;
                }
            }
            case 22{
                if (T0 + burn_duration + LOX_to_ETH_closing_delay + Purge_duration + Cooling_duration_after_end_burn){
                    setValve(SV63,0);
                    state = "active";
                }
            }
        }
    }while (state = "test")

}