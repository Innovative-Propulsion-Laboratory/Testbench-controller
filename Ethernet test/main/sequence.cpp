#include "sequence.h"
#include "Valves.h"
#include "Sensors.h"

uint16_t T_confirm;
uint16_t Chilldown_finished;
uint16_t PS63_duration;
uint16_t PS63_seems_rise;
uint16_t Ign_duration;
uint16_t Ign_seems_on;
uint16_t T0;
uint16_t ETH_open;
uint16_t Bypass_duration;
uint16_t Main_seems_rise;
uint16_t Main_duration;
uint16_t Nominal_pressure_reached;
uint16_t T_burn;
uint16_t Chilldown_start;
uint16_t chill_temp_seems_ok;
uint16_t Chilldown_duration;
uint16_t Chilldown_verified_duration;

sequence_data Sequence_data;

void Sequence() { 
    pinMode(IGN_pin, OUTPUT);
    pinMode(IGN_check_pin, INPUT);

    T_confirm = millis();
    Data.test_step = 1;
    Data.state = 1;
    // Data.test_cooling ...
    set_offset_pressure();

    do {
        sensorsLoop();
        // decode(receivePacket());
        
        switch ( Data.test_step) {
            ////// PURGE //////
            case 1: {
                if (millis() >= (T_confirm + Sequence_data.Confirm_to_purge_delay)) {
                    Chilldown_start = 0;
                    setValve(SV36, 1);
                    Data.test_step++;
                }
                break;
            }
            case 2: {
                if (millis() >= static_cast<uint32_t>(T_confirm + Sequence_data.Confirm_to_purge_delay + Sequence_data.Purge_duration1)) {
                    setValve(SV36, 0);
            ////// End of PURGE //////
            ////// start chilldown //////
                    Chilldown_start++;
                    setValve(SV13, 1);
                    Data.test_step++;
                }
                break;
            }
            case 3: {
                if (millis() >= static_cast<uint32_t>(T_confirm + Sequence_data.Confirm_to_purge_delay + Sequence_data.Purge_duration1 + Sequence_data.Chilldown_on_duration)) {
                    setValve(SV13, 0);
                    Data.test_step++;
                }
                break;
            }
            case 4: {
                if (millis() <= static_cast<uint32_t>(T_confirm + Sequence_data.Confirm_to_purge_delay + Sequence_data.Purge_duration1 + Sequence_data.Chilldown_on_duration + Sequence_data.Chilldown_off_duration)) {
                    if (Data.TS12 >= Sequence_data.chill_temp) {
                        chill_temp_seems_ok = millis();
                        Data.test_step = 5;  // remplace l'étape 4.5
                    } else if (Chilldown_start >= Sequence_data.Max_chilldown) {
                        Data.state = 0;  // Erreur chilldown
                    }
                } else {
                    T_confirm = millis();
                    Data.test_step = 2;
                    Chilldown_start++;
                }
                break;
            }
            case 5: {
                if ((Data.TS12 >= Sequence_data.chill_temp) && ((millis() - chill_temp_seems_ok) >= Chilldown_verified_duration)) {
                    Chilldown_finished = millis();
                    Chilldown_duration = Chilldown_finished - Chilldown_start;
                    Data.test_step++;
                } else if (Data.TS12 < Sequence_data.chill_temp) {
                    Data.test_step = 4;
                }
                break;
            }
            case 6: {
                if (millis() >= (Chilldown_finished + Sequence_data.Chilldown_to_cooling)) {
                    setValve(SV63, 1);
            ////// end chilldown //////
            ////// Start cooling //////
                    PS63_duration = millis();
                    Data.test_step++;
                }
                break;
            }
            ////// check cooling //////
            case 7: {
                if (Data.PS63 >= Sequence_data.cooling_pressure) {
                    PS63_seems_rise = millis();
                    Data.test_step++;
                } else if ((millis() - PS63_duration) >= Sequence_data.PS63_check_duration) {
                    Data.state = 0;  // erreur
                }
                break;
            }
            case 8: {
                if ((Data.PS63 >= Sequence_data.cooling_pressure) && ((millis() - PS63_seems_rise) >= Sequence_data.PS63_verified_duration)) {
                    Data.test_step++;
                } else if (Data.PS63 < Sequence_data.cooling_pressure) {
                    Data.test_step = 7;
                }
                break;
            }
            ////// start ignite //////
            case 9: {
            
                Ign_duration = millis();
                digitalWrite(IGN_pin, HIGH);
                Data.test_step++;
                break;
            }
            case 10: {
            ////// check ignite //////
                if (digitalRead (IGN_check_pin) == HIGH) {
                    Ign_seems_on = millis();
                    Data.test_step++;
                } else if ((millis() - Ign_duration) >= Sequence_data.Ign_check_duration) {
                    Data.state = 0;
                }
                break;
            }
            case 11: {
                if ((digitalRead (IGN_check_pin) == HIGH) && ((millis() - Ign_seems_on) >= Sequence_data.Ign_verified_duration)) {
                    digitalWrite(IGN_pin, LOW);
                    T0 = millis();
            ////// stop ignite //////
            ////// start bypass //////
                    setValve(SV24, 1);
                    Data.test_step++;
                } else {
                    Data.test_step = 10;
                }
                break;
            }
            case 12: {
                if (millis() >= (T0 + Sequence_data.ETH_to_LOX_bypass)) {
                    Bypass_duration = millis();
                    setValve(SV13, 1);
                    Data.test_step++;
                }
                break;
            }
            case 13: {
            ////// check bypass //////
                if ((Data.PS41 >= Sequence_data.Bypass_pressure) && (Data.PS42 >= Sequence_data.Bypass_pressure)) {
                    Bypass_duration = millis();
                    Data.test_step++;
                } else if ((millis() - Bypass_duration) >= Sequence_data.Bypass_check_duration) {
                    Data.state = 0;
                }
                break;
            }
            case 14: {
                if ((Data.PS41 >= Sequence_data.Bypass_pressure) && (Data.PS42 >= Sequence_data.Bypass_pressure) && ((millis() - Bypass_duration) >= Sequence_data.Bypass_verified_duration)) {
                    ETH_open = millis();
             ////// start main injection //////
                    setValve(SV22, 1);
                    Data.test_step++;
                } else {
                    Data.test_step = 12;
                }
                break;
            }
            case 15: {
                if (millis() >= (T0 + Sequence_data.ETH_to_LOX_main)) {
                    Main_duration = millis();
                    setValve(SV12, 1);
                    Data.test_step++;
                }
                break;
            }
            case 16: {
                if ((Data.PS41 >= Sequence_data.Main_pressure) && (Data.PS42 >= Sequence_data.Main_pressure)) {
                    Main_seems_rise = millis();
                    Data.test_step++;
                } else if ((millis() - Main_duration) >= Sequence_data.Main_check_duration) {
                    Data.state = 0;
                }
                break;
            }
            case 17: {
            ////// check main injection //////
                if ((Data.PS41 >= Sequence_data.Main_pressure) && (Data.PS42 >= Sequence_data.Main_pressure) && ((millis() - Main_seems_rise) >= Sequence_data.Main_verified_duration)) {
                    Data.test_step++;
                } else {
                    Data.test_step = 15;
                }
                break;
            }
            case 18: {
            ////// stop bypass //////
                setValve(SV24, 0);
                setValve(SV13, 0);
                Nominal_pressure_reached = millis();
                Data.test_step++;
                break;
            }
            case 19: {
                // TVC launch, not implemented
                Data.test_step++;
                break;
            }
            case 20: {
            ////// stop main injection and purge //////
                if (millis() >= (T0 + Sequence_data.burn_duration)) {
                    setValve(SV12, 0);
                    setValve(SV36, 1);
                    Data.test_step++;
                }
                break;
            }
            case 21: {
                if (millis() >= static_cast<uint32_t>(T0 + Sequence_data.burn_duration + Sequence_data.LOX_to_ETH_closing_delay)) {
                    setValve(SV22, 0);
                    setValve(SV35, 1);
                    Data.test_step++;
                }
                break;
            }
            case 22: {
                if (millis() >= static_cast<uint32_t>(T0 + Sequence_data.burn_duration + Sequence_data.LOX_to_ETH_closing_delay + Sequence_data.Purge_duration2)) {
                    setValve(SV36, 0);
                    setValve(SV35, 0);
                    Data.test_step++;
                }
                break;
            }
            ////// stop cooling //////
            case 23: {
                if (millis() >= static_cast<uint32_t>(T0 + Sequence_data.burn_duration + Sequence_data.LOX_to_ETH_closing_delay + Sequence_data.Purge_duration2 + Sequence_data.Cooling_duration_after_end_burn)) {
                    setValve(SV63, 0);
                    Data.state = 0;
                }
                break;
            }
        }

    } while (Data.state == 1);
}

float average(byte* L, int length) {
    float sum = 0;
    for (int i = 0; i < length; i++) {
        sum += L[i];
    }
    return sum / length;
}

void set_offset_pressure() { // set sensors at 0 
    const int N = 10;
    byte average_PS12_data[N];
    byte average_PS22_data[N];
    byte average_PS41_data[N];
    byte average_PS42_data[N];
    byte average_PS63_data[N];
    byte average_PS64_data[N];

    for (int i = 0; i < N; i++) {
        sensorsLoop();
        // decode(receivePacket()); 
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