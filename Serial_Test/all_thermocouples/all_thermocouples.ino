#include <Arduino.h>
#include <Adafruit_MAX31856.h>  // https://github.com/adafruit/Adafruit_MAX31856

// Thermocouples:
#define TS11_pin 28         // LOX temperature
#define TS31_pin 29         // GN2 50bar tank temperature
#define TS41_pin 30         // Main Combustion Chamber temperature 1
#define TS42_pin 35         // Main Combustion Chamber temperature 2
#define TS61_pin 36         // Water initial temperature
#define TS62_pin 37         // Water final temperature

unsigned int t, n = 0;
float TS11, TS31, TS41, TS42, TS61, TS62;
bool TS11_waiting, TS31_waiting, TS41_waiting, TS42_waiting, TS61_waiting, TS62_waiting;


Adafruit_MAX31856 thermo31 = Adafruit_MAX31856(TS31_pin, 11, 12, 13);
Adafruit_MAX31856 thermo41 = Adafruit_MAX31856(TS41_pin, 11, 12, 13);
Adafruit_MAX31856 thermo42 = Adafruit_MAX31856(TS42_pin, 11, 12, 13);
Adafruit_MAX31856 thermo61 = Adafruit_MAX31856(TS61_pin, 11, 12, 13);
Adafruit_MAX31856 thermo62 = Adafruit_MAX31856(TS62_pin, 11, 12, 13);
Adafruit_MAX31856 thermo11 = Adafruit_MAX31856(TS11_pin, 11, 12, 13); //initialized last since it may return negative values


void setup() {
    thermo31.begin();
    thermo41.begin();
    thermo42.begin();
    thermo61.begin();
    thermo62.begin();
    thermo11.begin();

    thermo31.setThermocoupleType(MAX31856_TCTYPE_K);
    thermo31.setConversionMode(MAX31856_ONESHOT_NOWAIT);

    thermo41.setThermocoupleType(MAX31856_TCTYPE_K);
    thermo41.setConversionMode(MAX31856_ONESHOT_NOWAIT);

    thermo42.setThermocoupleType(MAX31856_TCTYPE_K);
    thermo42.setConversionMode(MAX31856_ONESHOT_NOWAIT);

    thermo61.setThermocoupleType(MAX31856_TCTYPE_K);
    thermo61.setConversionMode(MAX31856_ONESHOT_NOWAIT);

    thermo62.setThermocoupleType(MAX31856_TCTYPE_K);
    thermo62.setConversionMode(MAX31856_ONESHOT_NOWAIT);

    thermo11.setThermocoupleType(MAX31856_TCTYPE_K);
    thermo11.setConversionMode(MAX31856_ONESHOT_NOWAIT);
}

void loop() {
  trigger_TS();
  read_TS();
  print_TS();
  n++;
  delay(100);
}

void trigger_TS() {
     // requesting data from the thermocouples if not waiting for a conversion
     if (!TS41_waiting){thermo41.triggerOneShot(); TS41_waiting = 1;}
     if (!TS42_waiting){thermo42.triggerOneShot(); TS42_waiting = 1;}
     if (!TS61_waiting){thermo61.triggerOneShot(); TS61_waiting = 1;}
     if (!TS62_waiting){thermo62.triggerOneShot(); TS62_waiting = 1;}
     if (!TS11_waiting){thermo11.triggerOneShot(); TS11_waiting = 1;}
}

void read_TS(){
    // getting data from the thermocouples if ready
    t = millis();
    if (TS11_waiting && thermo11.conversionComplete()) {
        TS11 = thermo11.readThermocoupleTemperature();
        TS11_waiting = 0;
    }
    if (TS31_waiting && thermo31.conversionComplete()) {
        TS31 = thermo31.readThermocoupleTemperature();
        TS31_waiting = 0;
    }
    if (TS41_waiting && thermo41.conversionComplete()) {
        TS41 = thermo41.readThermocoupleTemperature();
        TS41_waiting = 0;
    }
    if (TS42_waiting && thermo42.conversionComplete()) {
        TS42 = thermo42.readThermocoupleTemperature();
        TS42_waiting = 0;
    }
    if (TS61_waiting && thermo61.conversionComplete()) {
        TS61 = thermo61.readThermocoupleTemperature();
        TS61_waiting = 0;
    }
    if (TS62_waiting && thermo62.conversionComplete()) {
        TS62 = thermo62.readThermocoupleTemperature();
        TS62_waiting = 0;
    }
}

void print_TS() {
    Serial.println("------ Sensor Data ------");

    Serial.print("Packet ID: "); Serial.println(n);
    Serial.print("Timestamp (ms): "); Serial.println(t);

    Serial.println("Thermocouples (°C):");
    Serial.print("TS11: "); Serial.print(TS11); Serial.print("\t");
    Serial.print("TS31: "); Serial.print(TS31); Serial.print("\t");
    Serial.print("TS41: "); Serial.print(TS41); Serial.print("\t");
    Serial.print("TS42: "); Serial.print(TS42); Serial.print("\t");
    Serial.print("TS61: "); Serial.print(TS61); Serial.print("\t");
    Serial.print("TS62: "); Serial.println(TS62);

    Serial.println("--------------------------");
    Serial.println();
}