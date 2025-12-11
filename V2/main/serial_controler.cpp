#include "serial_controler.h"

/*
==================================================
📜 Commandes list 
==================================================

1. Open/Close valve 
    set valve <name valve> <0|1>;
    Ex: set valve SV11 1;  -> Open valve SV11
         set valve SV11 0;  -> Close valve SV11

2. Set BangBang pressure
    set pressure <numéro_tank> <valeur_en_mbar>;
    Ex: set pressure 1 1500;  -> Set tank 1 pressure to 1500 mbar

3. Enable/Disable BangBang
    set bangbang <numéro_tank> <0|1>;
    Ex: set bangbang 2 1;  -> Enable BangBang for tank 2
         set bangbang 1 0;  -> Disable BangBang for tank 1
4. Set sequence parameters
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
5. Start/Stop test sequence
    start sequence;
    stop sequence;
==================================================
*/

// ====== Buffer d'entrée ======
String command;


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
}

void processCommand(String command) {
  command.toLowerCase();
  Serial.print("Processing command: ");
  Serial.println(command);

  // Parse and execute commands here
  // Example: set valve 3 1;
  if (command.startsWith("set valve")) {
    int openParen  = command.indexOf('(');  // position de '('
    int comma      = command.indexOf(',');  // position de ','
    int closeParen = command.indexOf(')');  // position de ')'
    valve =  convertValve(command.substring(openParen + 1, comma)); // "SV11"
    int state = command.substring(comma + 1, closeParen).toInt(); 
    setValve(valve, state);
    Serial.print("Set valve ");
    Serial.print(valveNum);
    Serial.print(" to ");
    Serial.println(state);
  }
  // Add more command parsing as needed
}

uint8_t convertValve(String t) {
    switch(t){
        case "SV11" : return SV11;
        case "SV12" : return SV12;
        case "SV13" : return SV13;
        case "SV21" : return SV21;
        case "SV22" : return SV22;
        case "SV24" : return SV24;
        case "SV25" : return SV25;
        case "SV31" : return SV31;
        case "SV32" : return SV32;
        case "SV33" : return SV33;
        case "SV34" : return SV34;
        case "SV35" : return SV35;
        case "SV36" : return SV36;
        case "SV51" : return SV51;
        case "SV52" : return SV52;
        case "SV53" : return SV53;
        case "SV61" : return SV61;
        case "SV62" : return SV62;
        case "SV63" : return SV63;
        case "SV71" : return SV71;
        default:
            Serial.println("Invalid valve name");
            return 255; // Invalid valve
    }
}
