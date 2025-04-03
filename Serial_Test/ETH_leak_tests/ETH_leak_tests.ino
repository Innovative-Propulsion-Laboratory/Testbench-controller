#include "Valves.h"

unsigned int t;
int PS31, PS12;


void setup(){
    Serial.begin(115200);
    SPI.begin();

    setupValves();

    //disable other SPI devices
    pinMode(2, OUTPUT);
    pinMode(28, OUTPUT);
    pinMode(29, OUTPUT);
    pinMode(30, OUTPUT);
    pinMode(35, OUTPUT);
    pinMode(36, OUTPUT);
    pinMode(37, OUTPUT);

    digitalWrite(2, HIGH);
    digitalWrite(28, HIGH);
    digitalWrite(29, HIGH);
    digitalWrite(30, HIGH);
    digitalWrite(35, HIGH);
    digitalWrite(36, HIGH);
    digitalWrite(37, HIGH);

    setValve(SV21, 0);
    setValve(SV22, 0);
    setValve(SV24, 0);
    setValve(SV31, 1);
    setValve(SV32, 0);
    setValve(SV33, 0);
    setValve(SV34, 1);
}

void loop(){
  read_sensors();
}



void read_sensors(){
  t = millis();
  PS31 = (int)(87500.0 * ((float)analogRead(A5) / 1023.0 - 0.1));
  PS12 = (int)(31250.0 * ((float)analogRead(A8) / 1023.0 - 0.1));

  Serial.print(t);
  Serial.print(",");
  Serial.print(PS31);
  Serial.print(",");
  Serial.println(PS12);
}