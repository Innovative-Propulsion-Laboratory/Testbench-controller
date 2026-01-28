#include "Sequence.h"
#include "Decoder.h"


// Sequence
#define DEBUG true  // Mettre sur false pour couper les logs

#define debug(msg) if (DEBUG) { Serial.println(msg); }

#define debugf(fmt, ...) if (DEBUG) { char buf[128]; snprintf(buf, sizeof(buf), fmt, __VA_ARGS__); Serial.println(buf); }

void count_down() {
  if (millis() - last_send >= 50) {
    last_send = millis();
    count_down_time += 50;
    byte message[8] = { 0xAB, 0xAB, 0xAB, 0xAB,(byte)(count_down_time >> 24),(byte)(count_down_time >> 16) ,(byte)(count_down_time >> 8), (byte)(count_down_time & 0xFF) };
    // Serial.println( assembleUInt16((byte)(count_down_time & 0xFF),(byte)(count_down_time >> 8)));
    reply(message, sizeof(message));
  }
}

void Sequence_allumeur() {
  newFile();
  T_confirm = millis();
  Serial.println(millis());
  Data.test_step = 1;
  Chilldown_count = 0;
  set_offset_pressure();
  debug("=== Debut de la sequence allumeur ===");
  do {

    BBLoop();
    sensorsLoop();

    Packet p = receivePacket();
    if (p.length >= 4 && p.data != nullptr) {decode(p.data);}
    if (p.data != nullptr) {delete[] p.data;}

    switch (Data.test_step) {
      case 1:
        debug("[1] Attente avant purge");
        if (millis() >= (T_confirm + Sequence_data.Confirm_to_purge_delay)) {
          Serial.println("Activation : ");
          Serial.println(millis());
          Serial.println("T_confirm : ");
          Serial.println(T_confirm);
          Serial.println("Confirm to purge : ");
          Serial.println(Sequence_data.Confirm_to_purge_delay);
          setValve(SV35, 1);
          debug("→ Ouverture SV35 (purge)");
          Data.test_step++;
        }
        break;

      case 2:
        debug("[2] Purge en cours");
        if (millis() >= (T_confirm + Sequence_data.Confirm_to_purge_delay + Sequence_data.Purge_duration1)) {
          setValve(SV35, 0);
          debug("→ Fermeture SV35 (fin purge)");
          Serial.println("Activation : ");
          Serial.println(millis());
          
          digitalWrite(GP_ignite_pin, HIGH);                // Commande allumage glowplug

          debug("→ Allumage glowplug");
          Serial.println("Activation : ");
          Serial.println(millis());
          Heat_start = millis();
          Data.test_step++;
        }
        break;

      case 3:
        debug("[3] Chauffage glowplug en cours");
        if ((Data.glowplug_current >= Sequence_data.GP_current) && ((millis() - Heat_start) >= (Sequence_data.Glowplug_heat_before_duration))) {
          setValve(SV25, 1);
          debug("→ Ouverture SV25 (ETH)");
          Serial.println("Activation : ");
          Serial.println(millis());
          Igniter_burn_duration = millis();
          Data.test_step++;
        } else if ((Data.glowplug_current < Sequence_data.GP_current) && ((millis() - Heat_start) >= Sequence_data.Current_raising))  {
          debug("✖ Erreur: Courant trop faible dans le GP");
          send_string("error: Current not detected", 1);
          test_abort();
        }
        break;

      case 4:
        debug("[4] Injection des ergols");
        if ((Data.glowplug_current >= Sequence_data.GP_current) && ((millis() - Igniter_burn_duration) >= Sequence_data.ETH_to_GOX)) {
          setValve(SV71, 1);
          debug("→ Ouverture SV71 (GOX)");
          Serial.println("Activation : ");
          Serial.println(millis());
          Igniter_open_duration = millis();
          Data.test_step++;
        }
        count_down();
        break;        

      case 5:
        debug("[5] Verification pression allumeur");
        if ((Data.PS81 >= Sequence_data.Igniter_chamber_pressure)) {
          debug("✓ Pressions allumeur OK");
          Data.test_step++;
        } else if ((millis() - Igniter_open_duration) >= Sequence_data.Igniter_pressure_time) {  // changer igniter_check_duration
          debug("✖ Erreur: Pression trop basse (allumeur)");
          send_string("error: Pressure too low in igniter pressure chamber", 1);
          test_abort();
        }
        count_down();
        break;

      case 6:
        debug("[6] Stabilisation pression allumeur");
        if ((Data.PS81 >= Sequence_data.Igniter_chamber_pressure) && ((millis() - Igniter_open_duration) >= Sequence_data.Igniter_verified_duration)) {
          
          digitalWrite(GP_ignite_pin, LOW);

          debug("→ Desactivation glowplug");
          Serial.println("Activation : ");
          Serial.println(millis());
          Data.test_step++;
        } else if (Data.PS81 <= Sequence_data.Igniter_chamber_pressure) {
          Data.test_step = 5;
        }
        count_down();
        break;

      case 7:
        debug("[7] Arrêt allumeur");
        if ((millis() - Igniter_burn_duration) >= Sequence_data.Igniter_burn_duration) {
          setValve(SV71, 0);
          debug("→ Fermeture SV71 (GOX)");
          Serial.println("Activation : ");
          Serial.println(millis());
          Data.test_step++;
        }
        break;

      case 8:
        debug("[8] Purge en cours");
        if ((millis() - Igniter_burn_duration) >= (Sequence_data.Igniter_burn_duration + Sequence_data.GOX_to_ETH)) {
          setValve(SV25, 0);
          debug("→ Fermeture SV25 (ETH)");
          Serial.println("Activation : ");
          Serial.println(millis());
          setValve(SV35, 1);
          debug("→ Ouverture SV35 (purge)");
          Serial.println("Activation : ");
          Serial.println(millis());
          Data.test_step++;
        }
        break;

      case 9:
        debug("[8] Fin de purge");
        if ((millis() - Igniter_burn_duration) >= static_cast<uint32_t>(Sequence_data.Igniter_burn_duration + Sequence_data.GOX_to_ETH + Sequence_data.Purge_after_duration)) {
          setValve(SV35, 0);          
          debug("→ Fermeture SV35 (purge)");
          Serial.println("Activation : ");
          Serial.println(millis());
          Data.state = 0;
        }
        count_down();
        break;
    }
  } while (Data.state == 1);
  debug("=== Fin de sequence allumeur ===");
  reset_offset_pressure();
  Data.test_cooling = 0;
  BB_enable(1, 0);
  BB_enable(2, 0);
  BB_enable(6, 0);
  byte message[4] = {0xAB, 0xCD, 0xAB, 0xCD};
  reply(message, sizeof(message));
  test_will_begin = false;
  Serial.println("sortie de boucle");
}

void Sequence() {
  newFile(); // Create a new SD file at each test
  T_confirm = millis();
  Serial.println(millis());
  Data.test_step = 1;
  Chilldown_count = 0;
  set_offset_pressure();
  debug("=== Debut de la sequence ===");

  do {
    BBLoop();
    // Send data at 200Hz
    sensorsLoop();

    Packet p = receivePacket();
    if (p.length >= 4 && p.data != nullptr) {decode(p.data);}
    if (p.data != nullptr) {delete[] p.data;}
    // Serial.print("Data.test_step: "); Serial.println(Data.test_step);

    switch (Data.test_step) {
      case 1:
        debug("[1] Attente avant purge");
        if (millis() >= (T_confirm + Sequence_data.Confirm_to_purge_delay)) {
          Serial.println("Activation : ");
          Serial.println(millis());
          Serial.println("T_confirm : ");
          Serial.println(T_confirm);
          Serial.println("Confirm to purge : ");
          Serial.println(Sequence_data.Confirm_to_purge_delay);
          setValve(SV36, 1);
          debug("→ Ouverture SV36 (purge)");
          Data.test_step++;
        }
        break;

      case 2:
        debug("[2] Purge en cours");
        if (millis() >= (T_confirm + Sequence_data.Confirm_to_purge_delay + Sequence_data.Purge_duration1)) {
          setValve(SV36, 0);
          debug("→ Fermeture SV36 (fin purge)");
          Serial.println("Activation : ");
          Serial.println(millis());
          Data.test_step++;
        }
        break;

      case 3:          
        if (Chilldown_count == 0) { 
          Chilldown_start = millis();
        } 
        Serial.println("→ Ouverture SV13 (chilldown) ON");
        setValve(SV13, 1);
        Tchilldown = millis();
        Data.test_step++;
        break;

      case 4:
        if (millis() >= (Tchilldown + Sequence_data.Chilldown_on_duration)) {
          setValve(SV13, 0);
          debug("→ Fermeture SV13 (chilldown OFF)");
          Data.test_step++;
        }
        break;

      case 5:
        if (millis() >= (Tchilldown + Sequence_data.Chilldown_on_duration + Sequence_data.Chilldown_off_duration)) {
          if (Data.TS12 <= (Sequence_data.chill_temp*10)) {
            debug("✓ Temperature chilldown atteinte");
            Serial.println("goal temp : ");
            Serial.println(Sequence_data.chill_temp*10);
            Serial.println("TS12 temp : ");
            Serial.println(Data.TS12);
            chill_temp_seems_ok = millis();
            Data.test_step++;
          } else if (Chilldown_count >= Sequence_data.Max_chilldown) {
            debug("✖ Chilldown echoue");
            send_string("error: Chilldown failed", 1);
            test_abort();
          } else {
            debug("↻ Chilldown supplementaire necessaire");
            Serial.println("Chilldown_off_duration : ");
            Serial.println(Sequence_data.Chilldown_off_duration);
            Serial.println("Chilldown count : ");
            Serial.println(Chilldown_count);
            Serial.println("TS12 temp : ");
            Serial.println(Data.TS12);
            Serial.println("goal temp : ");
            Serial.println(Sequence_data.chill_temp*10);
            Tchilldown = millis();
            Sequence_data.Confirm_to_purge_delay = 0;
            Chilldown_count++; 
            Data.test_step = 3;
          }
        }
        break;

      case 6:
        debug("[5] Verif chilldown stable");
        if ((Data.TS12 <= (Sequence_data.chill_temp*10)) && ((millis() - chill_temp_seems_ok) >= Chilldown_verified_duration)) {
          Chilldown_finished = millis();
          Chilldown_duration = Chilldown_finished - Chilldown_start;
          debugf("✓ Chilldown termine en %lu ms", Chilldown_duration);
          Data.test_step++;
          if (Data.test_cooling){count_down_time = -Sequence_data.Chilldown_to_cooling - Sequence_data.Ign_to_bypass - Sequence_data.PS63_verified_duration - 100;}
          else{count_down_time = - Sequence_data.Chilldown_to_cooling - Sequence_data.Ign_to_bypass;}
          byte message[6] = { 0xAB, 0xAB, 0xAB, 0xAB, (byte)(count_down_time >> 8), (byte)(count_down_time & 0xFF)};
          reply(message, sizeof(message));
          Serial.println("envoie countdown");
        } else if (Data.TS12 < (Sequence_data.chill_temp*10)) {
          Data.test_step = 4;
        }
        break;

      case 7:
        setValve(SV35, 1);
        T_midpurge = millis();
        debug("→ Ouverture SV35 (purge)");
        Data.test_step++;
        count_down();
        break;
      
      case 8:
        debug("[2] Purge en cours");
        if (millis() >= (T_midpurge + Sequence_data.Purge_duration2)) {
          setValve(SV35, 0);
          debug("→ Fermeture SV35 (fin purge)");
          Data.test_step++;
        }
        count_down();
        break;

      case 9:
        if (Data.test_cooling) {
          debug("[6] Demarrage refroidissement");
          if (millis() >= (Chilldown_finished + Sequence_data.Chilldown_to_cooling)) {
            setValve(SV63, 1);
            debug("→ Ouverture SV63 (refroidissement)");
            PS63_duration = millis();
            Data.test_step++;
          }
        } else if (millis() >= static_cast<uint32_t>(Chilldown_finished + Sequence_data.Chilldown_to_cooling)) {
          Data.test_step = 12;
          T0 = millis();
        }
        count_down();
        break;

      case 10:
        debug("[7] Verif pression refroidissement");
        if (Data.PS63 >= Sequence_data.cooling_pressure) {
          PS63_seems_rise = millis();
          debug("✓ Refroidissement detecte");
          Data.test_step++;
        } else if ((millis() - PS63_duration) >= Sequence_data.PS63_check_duration) {
          debug("✖ Erreur: Refroidissement non detecte");
          send_string("error: Cooling not detected", 1);
          test_abort();
        }
        count_down();
        break;

      case 11:
        debug("[8] Stabilisation refroidissement");
        if ((Data.PS63 >= Sequence_data.cooling_pressure) && ((millis() - PS63_seems_rise) >= Sequence_data.PS63_verified_duration)) {
          Data.test_step++;
          T0 = millis();
        } else if (Data.PS63 < Sequence_data.cooling_pressure) {
          Data.test_step = 10;
        }
        count_down();
        break;

      case 12:
        debug("[9] Allumage");
        digitalWrite(GP_ignite_pin, HIGH);
        if (millis() >= (T0 + Sequence_data.Ign_to_bypass)){
          setValve(SV24, 1);
          debug("→ Ouverture SV24 (bypass)");
          T0 = millis();
          Data.test_step++;
        }
        count_down();
        break;

      case 13:
        debug("[12] Attente injection LOX");
        if (millis() >= (T0 + Sequence_data.ETH_to_LOX_bypass)) {
          digitalWrite(GP_ignite_pin, LOW);
          Bypass_duration_open = millis();
          setValve(SV13, 1);
          debug("→ Ouverture SV13 (LOX)");
          Data.test_step++;
        }
        count_down();
        break;

      case 14: 
        debug("[13] Verif bypass");
        if ((Data.PS41 >= Sequence_data.Bypass_pressure) && (Data.PS42 >= Sequence_data.Bypass_pressure)) {
          Bypass_duration = millis();
          debug("✓ Pressions bypass OK");
          Data.test_step++;
        } else if ((millis() - Bypass_duration_open) >= Sequence_data.Bypass_check_duration) {
          debug("✖ Erreur: Pression trop basse (bypass)");
          send_string("error: Pressure too low with bypass valves", 1);
          test_abort();
        }
        count_down();
        break;

      case 15:
        debug("[14] Stabilisation bypass");
        if ((Data.PS41 >= Sequence_data.Bypass_pressure) && (Data.PS42 >= Sequence_data.Bypass_pressure) && ((millis() - Bypass_duration) >= Sequence_data.Bypass_verified_duration)) {
          ETH_open = millis();
          setValve(SV22, 1);
          debug("→ Ouverture SV22 (ETH)");
          Data.test_step++;
        } else if ((Data.PS41 <= Sequence_data.Bypass_pressure) || (Data.PS42 <= Sequence_data.Bypass_pressure)){
          Data.test_step = 14;
        }
        count_down();
        break;

      case 16:
        debug("[15] Injection LOX");
        if (millis() >= (T0 + Sequence_data.ETH_to_LOX_main)) {
          Main_duration = millis();
          setValve(SV12, 1);
          debug("→ Ouverture SV12 (LOX)");
          Data.test_step++;
        }
        count_down();
        break;

      case 17:
        debug("[16] Verif pression injection");
        if ((Data.PS41 >= Sequence_data.Main_pressure) && (Data.PS42 >= Sequence_data.Main_pressure)) {
          Main_seems_rise = millis();
          debug("✓ Pressions injection OK");
          Data.test_step++;
        } else if ((millis() - Main_duration) >= Sequence_data.Main_check_duration) {
          debug("✖ Erreur: Pression injection trop faible");
          send_string("error: Pressure too low with main valves", 1);
          test_abort();
        }
        count_down();
        break;

      case 18:
        debug("[17] Stabilisation injection");
        if ((Data.PS41 >= Sequence_data.Main_pressure) && (Data.PS42 >= Sequence_data.Main_pressure) && ((millis() - Main_seems_rise) >= Sequence_data.Main_verified_duration)) {
          Data.test_step++;
        } else if ((Data.PS41 <= Sequence_data.Main_pressure) || (Data.PS42 <= Sequence_data.Main_pressure)){
          Data.test_step = 16;
        }
        count_down();
        break;

      case 19:
        debug("[18] Fermeture bypass");
        setValve(SV24, 0);
        setValve(SV13, 0);
        debug("→ Fermeture SV24 et SV13");
        Nominal_pressure_reached = millis();
        Data.test_step++;
        count_down();
        break;

      case 20:
        debug("[19] TVC (non implemente)");
        Data.test_step++;
        count_down();
        break;

      case 21:
        debug("[20] Fin de combustion, purge LOX");
        if (millis() >= (T0 + Sequence_data.burn_duration)) {
          setValve(SV12, 0);
          setValve(SV36, 1);
          debug("→ Fermeture SV12, Ouverture SV36");
          Data.test_step++;
        }
        count_down();
        break;

      case 22:
        debug("[21] Fermeture ETH, purge ETH");
        if (millis() >= static_cast<uint32_t>(T0 + Sequence_data.burn_duration + Sequence_data.LOX_to_ETH_closing_delay)) {
          setValve(SV22, 0);
          setValve(SV35, 1);
          debug("→ Fermeture SV22, Ouverture SV35");
          Data.test_step++;
        }
        count_down();
        break;

      case 23:
        debug("[22] Fin de purge");
        if (millis() >= static_cast<uint32_t>(T0 + Sequence_data.burn_duration + Sequence_data.LOX_to_ETH_closing_delay + Sequence_data.Purge_duration3)) {
          setValve(SV36, 0);
          setValve(SV35, 0);
          debug("→ Fermeture SV36 et SV35");
          Data.test_step++;
        }
        count_down();
        break;

      case 24:
        debug("[23] Fin refroidissement");
        if (millis() >= static_cast<uint32_t>(T0 + Sequence_data.burn_duration + Sequence_data.LOX_to_ETH_closing_delay + Sequence_data.Purge_duration3 + Sequence_data.Cooling_duration_after_end_burn)) {
          setValve(SV63, 0);
          debug("→ Fermeture SV63 (fin refroidissement)");
          Data.state = 0;
        }
        count_down();
        break;
    }
  } while (Data.state == 1);

  debug("=== Fin de sequence ===");
  reset_offset_pressure();
  Data.test_cooling = 0;
  BB_enable(1, 0);
  BB_enable(2, 0);
  BB_enable(6, 0);
  byte message[4] = {0xAB, 0xCD, 0xAB, 0xCD};
  reply(message, sizeof(message));
  test_will_begin = false;
  Serial.println("sortie de boucle");
}

float average(int32_t* L, int length) {
  float sum = 0;
  for (int i = 0; i < length; i++) {
    sum += L[i];
  }
  return sum / length;
}

void set_offset_pressure() {  // set sensors at 0
  const int N = 10;
  int32_t average_PS12_data[N];
  int32_t average_PS22_data[N];
  int32_t average_PS41_data[N];
  int32_t average_PS42_data[N];
  int32_t average_PS63_data[N];
  int32_t average_PS64_data[N];

  for (int i = 0; i < N; i++) {
    if (millis() - time_last_reading >= 50) {
    sensorsLoop();
    time_last_reading = millis();
    }
    BBLoop();
    read_atm_pressure();
    Packet p = receivePacket();
    if (p.length >= 4 && p.data != nullptr) {
      decode(p.data);
    }
    if (p.data != nullptr) {
      delete[] p.data;
    }
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