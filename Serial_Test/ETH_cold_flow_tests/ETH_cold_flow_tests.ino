#include "Valves.h"

uint16_t t_open = 5000;
uint16_t t_after_setup, t_after_open, t_after_closed;
uint8_t state = 0;
unsigned int t, FM11;
int PS31, PS11, PS12, PS11_bb, PS21_bb;

#define PS11_TARGET 2000
#define PS21_TARGET 2000
#define PS11_UPPER_LIM PS11_TARGET+100
#define PS11_LOWER_LIM PS11_TARGET-100
#define PS21_UPPER_LIM PS21_TARGET+100
#define PS21_LOWER_LIM PS21_TARGET-100

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

    setValve(SV11, 0);
    setValve(SV21, 0);
    setValve(SV31, 1);
    setValve(SV32, 0);
    setValve(SV33, 1);
    setValve(SV34, 0);
    setValve(SV35, 0);
    setValve(SV36, 0);
    // setValve(SV24, 1);
    // delay(20000);
    // setValve(SV24, 0);

    t_after_setup = millis();
    state = 1;

}

void loop(){
  //read_sensors();                   //only read sensors
  SV_open_close();                  //read sensors & open main valve SV22 or bypass SV24
}


void SV_open_close() {
  switch (state) {
    case 1:
      if ((millis() - t_after_setup) > 10000){
        setValve(SV12, 1);          //change valve here
        //setValve(SV22, 1);
        t_after_open = millis();
        state++;
      }
      break;

    case 2:
      if ((millis() - t_after_open) > t_open){
        setValve(SV12, 0);          //change valve here
        //setValve(SV22, 0);
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
      break;
  }

  PS11_bb = (int)(31250.0 * ((float)analogRead(A9) / 1023.0 - 0.1));
  PS21_bb = (int)(31250.0 * ((float)analogRead(A7) / 1023.0 - 0.1));

  if (PS11_bb > PS11_UPPER_LIM){
    setValve(SV33, 0);
    }
  if (PS11_bb < PS11_LOWER_LIM){
    setValve(SV33, 1);
    }
    

  if (PS21_bb > PS21_UPPER_LIM){
    setValve(SV34, 0);
    }
  if (PS21_bb < PS21_LOWER_LIM){
    setValve(SV34, 1);
    }
    

  read_sensors();
  delay(5);
}



void read_sensors(){
  t = millis();
  //PS31 = analogRead(A5);
  PS31 = (int)(87500.0 * ((float)analogRead(A5) / 1023.0 - 0.1));
  PS11 = (int)(31250.0 * ((float)analogRead(A9) / 1023.0 - 0.1));
  PS12 = (int)(31250.0 * ((float)analogRead(A8) / 1023.0 - 0.1));
  FM11 = analogRead(A12);

  Serial.print(t);
  Serial.print(",");
  Serial.print(PS31);
  Serial.print(",");
  Serial.print(PS11);
  Serial.print(",");
  Serial.print(PS12);
  Serial.print(",");
  Serial.println(FM11);
}