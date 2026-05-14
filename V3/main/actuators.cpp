#include "actuators.h"

const uint8_t POT_CS_PIN   = 1;
const uint8_t MCP_SHDN_PIN = 255;

extern const uint8_t SPI_CS_PINS[] = {29, ADC_CS};

const uint32_t POT_SPI_SPEED = 4000000;

int valeur = 0;

// Potentiometre associé au mcp
MCP4261 pot(POT_CS_PIN, MCP_SHDN_PIN, &SPI); 

void deselectSpiDevices()
{
  for (uint8_t i = 0; i < sizeof(SPI_CS_PINS) / sizeof(SPI_CS_PINS[0]); i++) {
    pinMode(SPI_CS_PINS[i], OUTPUT);
    digitalWrite(SPI_CS_PINS[i], HIGH);
  }
}

void ActSetup()
{
  Serial.println("debut init pot");
  pot.begin();

  // setActuators(0, 0);
  // pot.setValue(0, 0);
  // pot.setValue(1, 0);

  deselectSpiDevices();
  pot.begin();
  pot.setSPIspeed(POT_SPI_SPEED);
  Serial.println("MCP4261 init succeed (SPI) !");
  


  valeur = 128;
  applyValue();
}

bool writePotRegister(uint8_t reg, uint16_t value)
{
  if (reg > 0x0F || value > 0x01FF) return false;

  uint8_t command = reg << 4;
  if (value > 0xFF) command |= 0x01;
  uint8_t data = value & 0xFF;

  deselectSpiDevices();
  pinMode(SPI_SCK, OUTPUT);
  pinMode(SPI_MOSI, OUTPUT);

  digitalWrite(SPI_SCK, LOW);
  digitalWrite(POT_CS_PIN, LOW);
  shiftOut(SPI_MOSI, SPI_SCK, MSBFIRST, command);
  shiftOut(SPI_MOSI, SPI_SCK, MSBFIRST, data);
  digitalWrite(POT_CS_PIN, HIGH);

  return true;
}

bool writePotWiper(uint8_t wiper, uint16_t value)
{
  if (wiper > 1 || value > 255) return false;

  uint8_t reg = (wiper == 0) ? 0x00 : 0x01;
  return writePotRegister(reg, value);
}

void applyValue()
{
  if (valeur > 255) valeur = 255;
  if (valeur < 0)   valeur = 0;

  if (!writePotWiper(0, (uint8_t)valeur)) {
    Serial.println("MCP4261 wiper 0 write FAILED !");
    return;
  }

  delay(2);
  if (!writePotWiper(1, (uint8_t)valeur)) {
    Serial.println("MCP4261 wiper 1 write FAILED !");
    return;
  }

  Serial.print("valeur = ");
  Serial.println(valeur);
}

void normalActuators() 
{
    writePotWiper(0, 128);
    writePotWiper(1, 128);
}
    
// right
void setActR(uint8_t valeur) 
{
  if (valeur > 255) valeur = 255;
  if (valeur < 0)   valeur = 0;

  writePotWiper(0, (uint8_t)valeur);

  Serial.print("Right actuator value = ");
  Serial.println(valeur);
}

// Valeur sur actuator left
void setActL(uint8_t valeur) 
{
  if (valeur > 255) valeur = 255;
  if (valeur < 0)   valeur = 0;

  writePotWiper(1, (uint8_t)valeur);

  Serial.print("Left actuator value = ");
  Serial.println(valeur);
}

void setActuators(uint8_t valeurL, uint8_t valeurR) 
{
  if (valeurL > 255) valeurL = 255;
  if (valeurL < 0)   valeurL = 0;
  if (valeurR > 255) valeurR = 255;
  if (valeurR < 0)   valeurR = 0;

  writePotWiper(0, (uint8_t)valeurR);
  writePotWiper(1, (uint8_t)valeurL);

  Serial.print("Right actuator value = ");
  Serial.println(valeurR);

  Serial.print("Left actuator value = ");
  Serial.println(valeurL);
}