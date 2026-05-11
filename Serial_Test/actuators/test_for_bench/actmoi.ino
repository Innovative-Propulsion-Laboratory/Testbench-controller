#include <Arduino.h>
#include <SPI.h>
#include "MCP4261.h"


const uint8_t MCP_CS_PIN   = 1;
const uint8_t MCP_SHDN_PIN = 255;

// Potentiometre associé au mcp
MCP4261 pot(MCP_CS_PIN, MCP_SHDN_PIN, &SPI); 

void setup()
{
  Serial.begin(115200);
  delay(1000);

  SPI.begin();
  pot.begin();

  valeur = 0;
  applyValue();

  Serial.println("Controle MCP4261");
  Serial.println("z : +20");
  Serial.println("s : -20");
  Serial.println("------------------");
}

void loop()
{
  if (Serial.available())
  {
    char c = Serial.read();
    Serial.print("recu: ");
    Serial.println(c);

    if (c == 'z')
    {
      valeur += 20;
      applyValue();
    }
    else if (c == 's')
    {
      valeur -= 20;
      applyValue();
    }
  }
}

void setactuators(bite length) : 

// Valeur sur actuator left
void setActL(uint8_t valeur) 
{
  if (valeur > 255) valeur = 255;
  if (valeur < 0)   valeur = 0;

  pot.setValue(0, (uint8_t)valeur);

  Serial.print("Left actuator value = ");
  Serial.println(valeur);
}

// right
void setActR(uint8_t valeur) 
{
  if (valeur > 255) valeur = 255;
  if (valeur < 0)   valeur = 0;

  pot.setValue(1, (uint8_t)valeur);

  Serial.print("Right actuator value = ");
  Serial.println(valeur);
}