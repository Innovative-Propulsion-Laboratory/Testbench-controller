#include "Valves.h"

uint16_t t_open = 5000;
uint16_t t_after_setup, t_after_open, t_after_closed;
uint8_t state = 0;
unsigned int t, FM21;
uint16_t PS31, PS21, PS22;

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
    setValve(SV31, 1);
    setValve(SV32, 0);
    setValve(SV33, 0);
    setValve(SV34, 1);
    setValve(SV35, 0);
    setValve(SV36, 0);

    t_after_setup = millis();
    state = 1;

}

void loop(){
  read_sensors();                   //only read sensors
  SV_open_close();                  //read sensors & open main valve SV22 or bypass SV24
}


void SV_open_close() {
  switch (state) {
    case 1:
      if ((millis() - t_after_setup) > 5000){
        setValve(SV22, 1);          //change valve here
        t_after_open = millis();
        state++;
      }
      break;

    case 2:
      if ((millis() - t_after_open) > t_open){
        setValve(SV22, 0);          //change valve here
        t_after_closed = millis();
        state++;
      }
      break;

    case 3:
      if ((millis() - t_after_closed) > 2000){
        state++;
      }
      break;

    case 4:
      while(true) {
        delay(1000);
      }
      break;
  }
  read_sensors();
}



void read_sensors(){
  t = millis();
  PS31 = (int)(87500.0 * ((float)analogRead(A5) / 1023.0 - 0.1));
  PS21 = (int)(31250.0 * ((float)analogRead(A7) / 1023.0 - 0.1));
  PS22 = (int)(31250.0 * ((float)analogRead(A6) / 1023.0 - 0.1));
  FM21 = analogRead(A12);

  Serial.print(t);
  Serial.print(",");
  Serial.print(PS31);
  Serial.print(",");
  Serial.print(PS21);
  Serial.print(",");
  Serial.print(PS22);
  Serial.print(",");
  Serial.println(FM21);
}