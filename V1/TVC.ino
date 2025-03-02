// Code for the TPL0202 Digital Potentiometer

#include <SPI.h>

#define IOexp_pin 10  // Chip Select pin for TPL0202

void setup() {
    Serial.begin(115200);
    SPI.begin();

    pinMode(IOexp_pin, OUTPUT);
    digitalWrite(IOexp_pin, HIGH);  //Disable TPL0202 by default
}

void setNonVolatileWiper(byte wiper, float percentage = 50) { // precentage from 0 to 100

    byte value = round(percentage * 2.55);

    digitalWrite(IOexp_pin, LOW);
    
    if (wiper == 0)
        SPI.transfer(0b00010001);  // Write-NV command for Wiper A - left Actuator - 0
    else if (wiper == 1)
        SPI.transfer(0b00010010);  // Write-NV command for Wiper B - right Actuator - 1
    SPI.transfer(value);

    digitalWrite(IOexp_pin, HIGH);
}

void setWiper(byte wiper, float percentage = 50) { // precentage from 0 to 100

    byte value = round(percentage * 2.55);

    digitalWrite(IOexp_pin, LOW);
    
    if (wiper == 0)
        SPI.transfer(0b00000001);  // Write command for Wiper A - left Actuator - 0
    else if (wiper == 1)
        SPI.transfer(0b00000010);  // Write command for Wiper B - right Actuator - 1
    SPI.transfer(value);

    digitalWrite(IOexp_pin, HIGH);
}

void WipertoNV(byte wiper) { 

    digitalWrite(IOexp_pin, LOW);
    
    if (wiper == 0)
        SPI.transfer(0b00100001);  // Transfer Wiper A to NV memory - left Actuator - 0
    else if (wiper == 1)
        SPI.transfer(0b0010001);  // Transfer Wiper B to NV memory - right Actuator - 1

    digitalWrite(IOexp_pin, HIGH);
}

void NVtoWiper(byte wiper) {

    digitalWrite(IOexp_pin, LOW);
    
    if (wiper == 0)
        SPI.transfer(0b00110001);  // Transfer NV memory to Wiper A - left Actuator - 0   
    else if (wiper == 1)
        SPI.transfer(0b00110010);  // Transfer NV memory to Wiper B - right Actuator - 1

    digitalWrite(IOexp_pin, HIGH);
}
