#include "Valves.h"

void setup(){
    Serial.begin(9600);
    SPI.begin();

    setupValves();
    Set_valve_position();

    //disable other SPI devices
    pinMode(1, OUTPUT);
    pinMode(28, OUTPUT);
    pinMode(29, OUTPUT);
    pinMode(30, OUTPUT);
    pinMode(35, OUTPUT);
    pinMode(36, OUTPUT);
    pinMode(37, OUTPUT);

    digitalWrite(1, HIGH);
    digitalWrite(28, HIGH);
    digitalWrite(29, HIGH);
    digitalWrite(30, HIGH);
    digitalWrite(35, HIGH);
    digitalWrite(36, HIGH);
    digitalWrite(37, HIGH);
}

void loop(){
  setValve (SV11, 0);
  delay(1000);
  setValve (SV11, 1);
  delay(1000);

  setValve (SV12, 1);
  delay(1000);
  setValve (SV12, 0);
  delay(1000);

  setValve (SV13, 1);
  delay(1000);
  setValve (SV13, 0);
  delay(1000);

  setValve (SV21, 0);
  delay(1000);
  setValve (SV21, 1);
  delay(1000);

  setValve (SV22, 1);
  delay(1000);
  setValve (SV22, 0);
  delay(1000);

  setValve (SV24, 1);
  delay(1000);
  setValve (SV24, 0);
  delay(1000);

  setValve (SV31, 1);
  delay(1000);
  setValve (SV31, 0);
  delay(1000);

  setValve (SV32, 0);
  delay(1000);
  setValve (SV32, 1);
  delay(1000);

  setValve (SV33, 1);
  delay(1000);
  setValve (SV33, 0);
  delay(1000);

  setValve (SV34, 1);
  delay(1000);
  setValve (SV34, 0);
  delay(1000);

  setValve (SV35, 1);
  delay(1000);
  setValve (SV35, 0);
  delay(1000);

  setValve (SV36, 1);
  delay(1000);
  setValve (SV36, 0);
  delay(1000);

  setValve (SV51, 0);
  delay(1000);
  setValve (SV51, 1);
  delay(1000);

  setValve (SV52, 1);
  delay(1000);
  setValve (SV52, 0);
  delay(1000);

  setValve (SV53, 1);
  delay(1000);
  setValve (SV53, 0);
  delay(1000);

  setValve (SV61, 0);
  delay(1000);
  setValve (SV61, 1);
  delay(1000);

  setValve (SV62, 0);
  delay(1000);
  setValve (SV62, 1);
  delay(1000);

  setValve (SV63, 1);
  delay(1000);
  setValve (SV63, 0);
  delay(1000);

}