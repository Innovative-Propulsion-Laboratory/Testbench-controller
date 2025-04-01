#include "Valves.h"

uint32_t t_after_setup, t_after_open, t_after_closed;
uint8_t state = 0;
unsigned int t, FM61;
int PS61, PS62, PS63, PS64;


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

    setValve(SV51, 0);
    setValve(SV61, 0);
    setValve(SV62, 0);
    setValve(SV52, 1);
    setValve(SV53, 1);
    setValve(SV63, 1);
    t_after_setup = millis();
    state = 1;

}

void loop(){
  read_sensors();
  //SV63_open_close();
}


void SV63_open_close() {
  switch (state) {
    case 1:
      if ((millis() - t_after_setup) > 5000){
        setValve(SV63, 1);
        t_after_open = millis();
        state++;
      }
      break;

    case 2:
      if ((millis() - t_after_open) > 8000){
        setValve(SV63, 0);
        t_after_closed = millis();
        state++;
      }
      break;

    case 3:
      if ((millis() - t_after_closed) > 2000){
        setValve(SV63, 0);
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
  delay(10);
}



void read_sensors(){
    t = millis();
  PS61 = (int)(31250.0 * ((float)analogRead(A1) / 1023.0 - 0.1));
  PS62 = (int)(31250.0 * ((float)analogRead(A0) / 1023.0 - 0.1));
  PS63 = (int)(31250.0 * ((float)analogRead(A17) / 1023.0 - 0.1));
  PS64 = (int)(31250.0 * ((float)analogRead(A16) / 1023.0 - 0.1));
  FM61 = analogRead(A13);

  Serial.print(t);
  Serial.print(",");
  Serial.print(PS61);
  Serial.print(",");
  Serial.print(PS62);
  Serial.print(",");
  Serial.print(PS63);
  Serial.print(",");
  Serial.print(PS64);
  Serial.print(",");
  Serial.println(FM61);
}