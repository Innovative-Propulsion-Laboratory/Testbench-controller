// This example demonstrates doing a one-shot measurement "manually".
// Separate calls are made to trigger the conversion and then check
// for conversion complete. While this typically only takes a couple
// 100 milliseconds, that times is made available by separating these
// two steps.

#include <Adafruit_MAX31856.h>

// Use software SPI: CS, DI, DO, CLK
//Adafruit_MAX31856 maxthermo = Adafruit_MAX31856(10, 11, 12, 13);
// use hardware SPI, just pass in the CS pin
Adafruit_MAX31856 maxthermo = Adafruit_MAX31856(36, 11, 12, 13);
// use hardware SPI, pass in the CS pin and using SPI1
//Adafruit_MAX31856 maxthermo = Adafruit_MAX31856(10, &SPI1);

unsigned long t0 = millis();

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("MAX31856 thermocouple test");

  if (!maxthermo.begin()) {
    Serial.println("Could not initialize thermocouple.");
    while (1) delay(10);
    }
    maxthermo.setThermocoupleType(MAX31856_TCTYPE_K);
    maxthermo.setConversionMode(MAX31856_ONESHOT_NOWAIT);
}

void loop() {
  // trigger a conversion, returns immediately

  maxthermo.triggerOneShot();
  //
  // here's where you can do other things
  delay(1000);
  //
  if (maxthermo.conversionComplete()) {
    Serial.print("N°10: ");
    Serial.print(maxthermo.readThermocoupleTemperature());
    Serial.print("°C   ");
    Serial.print(millis() - t0);
    Serial.print("ms \n");
    t0 = millis();
  }
}