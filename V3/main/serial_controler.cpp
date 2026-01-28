#include "serial_controler.h"

 /*
 ==================================================
 📜 Commandes list 
 ==================================================

 1. Open/Close valve 
    set valve(<name valve>, <0|1>)
    Ex: set valve(SV11, 1)  -> Open valve SV11
        c  -> Close valve SV11

 2. Set BangBang pressure
    set BBpressure(<num_tank> <valeur_en_mbar>)
    Ex: set BBpressure(2,2000)  -> Set LOX tank pressure to 15bar

    Numero_tank:  1 -> LOX
                  2 -> ETH
                  6 -> H2O

 3. Enable/Disable BangBang
    set bangbang(<num_tank>,<0|1>)
    Ex: set bangbang(2,1)  -> Enable BangBang for tank LOX
        
    Numero_tank:  1 -> LOX
                  2 -> ETH
                  6 -> H2O

 4. Save data for a test 
    set savedata(<1|0>)

 5. launch discharge test 
    launch(<tank>,<pressure>,<time(ms)>)
    EX: launch(2,3000,10000) 


 . Set sequence parameters
     set sequence (
         LOX pressure,
         ETH pressure,
         cooling enable,
         H2O pressure,
         TVC pattern,
         Confirm to purge delay,
         Purge duration 1,
         Chilldown on duration,
         Chilldown off duration,
         Chill temperature,
         Max chilldown,
         Purge duration 2,
         Chilldown to cooling,
         PS63 verified duration,
         IGN to bypass,
         Burn duration, 
         ETH to LOX bypass,
         Bypass check duration,
         Bypass pressure,
         Bypass verified duration,
         ETH to LOX main,
         Main check duration,
         Main pressure,
         Main verified duration,
         TVC pattern duration,
         LOX to ETH closing delay,
         Purge duration 3,
         Cooling duration after end burn,
     )
 . Start/Stop test sequence
     start sequence;
     stop sequence;
 ==================================================
 */

// ====== Buffer d'entrée ======
 String command;

// Discharges test variable 

uint32_t duration_test;
uint8_t tank;
int pressure; 
uint32_t time_test_begin;
bool discharge_test = 0;





// ====== Boucle debug ======
void serial_loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      command.trim();
      if (command.length() > 0) {
        processCommand(command);
        command = "";
      }
    } else {
      command += c;
    }
  }
  if (discharge_test!=0){
    if ((millis()-time_test_begin) >= duration_test){
      setValve(SV71,0);
      BB_enable(2,0);
      discharge_test = 0;
      state_test_spe = 0;
    }
  }
}

void processCommand(String command) {
  command.toLowerCase();
  Serial.print("Processing command: ");
  Serial.println(command);

  if (command.startsWith("set valve")) {
    int openParen  = command.indexOf('(');  // position de '('
    int comma      = command.indexOf(',');  // position de ','
    int closeParen = command.indexOf(')');  // position de ')'
    uint8_t valve =  convertValve(command.substring(openParen + 1, comma));
    int state = command.substring(comma + 1, closeParen).toInt(); 

    if (state!=0 && state !=1){Serial.println("Invalid valve state");return;}
    if (valve == 255){Serial.println("Invalid valve ID ");return;}

    setValve(valve, state);
    Serial.print("Set valve ");
    Serial.print(valve);
    Serial.print(" to ");
    Serial.println(state);

  }
  else if (command.startsWith("set bbpressure")) {
    int openParen  = command.indexOf('(');  // position de '('
    int comma      = command.indexOf(',');  // position de ','
    int closeParen = command.indexOf(')');  // position de ')'
    uint8_t tank = command.substring(openParen + 1, comma).toInt();
    int pressure = command.substring(comma + 1, closeParen).toInt(); 
    BB_param_set(tank, pressure);
    Serial.print("Set tank ");
    Serial.print(tank);
    Serial.print(" set pressure to");
    Serial.println(pressure);
  }
  else if (command.startsWith("set bangbang")) {
    int openParen  = command.indexOf('(');  // position de '('
    int comma      = command.indexOf(',');  // position de ','
    int closeParen = command.indexOf(')');  // position de ')'
    uint8_t tank = command.substring(openParen + 1, comma).toInt();
    int state = command.substring(comma + 1, closeParen).toInt(); 
    if(state!=0 && state !=1){
      Serial.println("Invalid bangbang state");
      return;
    }
    BB_enable(tank, state);
    Serial.print("Set tank ");
    Serial.print(tank);
    Serial.print(" bangbang to ");
    Serial.println(state);
  }
  else if (command.startsWith("set savedata")) {
    int openParen  = command.indexOf('(');  // position de '('
    int closeParen = command.indexOf(')');  // position de ')'
    int state = command.substring(openParen + 1, closeParen).toInt();
    if(state!=0 && state !=1){
      Serial.println("Invalid savedata state");
      return;
    }
    if (state==0){
      bool_file = 1;
    }else{
      bool_file = 0;
    }
    state_test_spe = state;
    Serial.print("Set save sate to ");
    Serial.println(state);
  }
  else if (command.startsWith("set glowplug")) {
    int openParen  = command.indexOf('(');  // position de '('
    int closeParen = command.indexOf(')');  // position de ')'
    int state = command.substring(openParen + 1, closeParen).toInt();
    if(state!=0 && state !=1){
      Serial.println("Invalid savedata state");
      return;
    }

    digitalWrite(GP_ignite_pin, state);

    Serial.print("Set glowplug to ");
    Serial.println(state);
  }
  else if (command.startsWith("launch")) { // launch(2,3000,10000)
    int openParen  = command.indexOf('(');
    int comma1     = command.indexOf(',', openParen + 1);
    int comma2     = command.indexOf(',', comma1 + 1);
    int closeParen = command.indexOf(')', comma2 + 1);

    if (openParen < 0 || comma1 < 0 || comma2 < 0 || closeParen < 0) {
      Serial.println("Bad launch format");
      return;
    }

    int tank = command.substring(openParen + 1, comma1).toInt();
    int pressure = command.substring(comma1 + 1, comma2).toInt();
    duration_test = command.substring(comma2 + 1, closeParen).toInt();

    if (tank < 1 || tank > 3) {
      Serial.println("Invalid tank");
      return;
    }
    if (pressure <= 0 || duration_test <= 0) {
      Serial.println("Invalid pressure/duration");
      return;
    }

    BB_param_set(tank, pressure);
    BB_enable(tank, 1);

    discharge_test = 1;           // ou state_test_spe, mais sois cohérent
    time_test_begin = millis();
    state_test_spe = 1;
    bool_file = 0;
    setValve(SV71, 1);
  }
  else if (command.startsWith("Ignitertest")) {
    int openParen  = command.indexOf('(');
    int comma1     = command.indexOf(',', openParen + 1);
    int comma2     = command.indexOf(',', comma1 + 1);
    int comma3     = command.indexOf(',', comma2 + 1);
    int comma4     = command.indexOf(',', comma3 + 1);
    int comma5     = command.indexOf(',', comma4 + 1);
    int comma6     = command.indexOf(',', comma5 + 1);
    int comma7     = command.indexOf(',', comma6 + 1);
    int comma8     = command.indexOf(',', comma7 + 1);
    int comma9     = command.indexOf(',', comma8 + 1);
    int closeParen = command.indexOf(')', comma9 + 1);

    if (openParen < 0 || comma1 < 0 || comma2 < 0 || comma3 < 0 || comma4 < 0 || comma5 < 0 || comma6 < 0 || comma7 < 0 || comma8 < 0 || comma9 < 0 || closeParen < 0) {
      Serial.println("Bad command format");
      return;
    }
  }
  // Add more command parsing as needed
}

uint8_t convertValve(const String& t) {
  for (size_t i = 0; i < sizeof(valveMap)/sizeof(valveMap[0]); ++i) {
    if (t.equals(valveMap[i].name)) return valveMap[i].id;
  }
  Serial.println("Invalid valve name: " + t);
  return 255;
}
