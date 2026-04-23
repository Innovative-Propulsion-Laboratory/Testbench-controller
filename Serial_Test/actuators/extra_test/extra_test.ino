#include <Arduino.h>
#include <SPI.h>
#include "MCP4261.h"
#include <MCP3208.h>
#include <Adafruit_MCP23X17.h>

#define NUM_VALVES 20

#define MCP_RESET 10
#define MCP_CS    29
#define ADC_CS    37
#define SPI_SCK   13
#define SPI_MOSI  11
#define SPI_MISO  12

const uint8_t MCP_CS_PIN   = 1;
const uint8_t MCP_SHDN_PIN = 255;
const uint32_t POT_SPI_SPEED = 4000000;
const uint8_t SPI_CS_PINS[] = {MCP_CS_PIN, MCP_RESET, 28, MCP_CS, 30, 35, 36, ADC_CS};

MCP4261 pot(MCP_CS_PIN, MCP_SHDN_PIN, &SPI);
MCP3208 adc;
Adafruit_MCP23X17 mcp;

int valeur = 0;

void deselectSpiDevices()
{
  for (uint8_t i = 0; i < sizeof(SPI_CS_PINS) / sizeof(SPI_CS_PINS[0]); i++) {
    pinMode(SPI_CS_PINS[i], OUTPUT);
    digitalWrite(SPI_CS_PINS[i], HIGH);
  }
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
  digitalWrite(MCP_CS_PIN, LOW);
  shiftOut(SPI_MOSI, SPI_SCK, MSBFIRST, command);
  shiftOut(SPI_MOSI, SPI_SCK, MSBFIRST, data);
  digitalWrite(MCP_CS_PIN, HIGH);

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

void setup()
{
  Serial.begin(115200);
  delay(1000);

  deselectSpiDevices();
  SPI.begin();
  deselectSpiDevices();

  pot.begin();
  pot.setSPIspeed(POT_SPI_SPEED);
  Serial.println("MCP4261 init succeed (SPI) !");

  if (!mcp.begin_SPI(MCP_CS, SPI_SCK, SPI_MISO, SPI_MOSI, 0)) {
    Serial.println("MCP23X17 init FAILED (SPI) !");
  }
  else {
    Serial.println("MCP23X17 init succeed (SPI) !");
  }

  for (int i = 0; i < 15; i++) {
    mcp.pinMode(i, OUTPUT);
  }

  deselectSpiDevices();
  if (!adc.begin(SPI_SCK, SPI_MOSI, SPI_MISO, ADC_CS)) {
    Serial.println("MCP3208 init FAILED (SPI) !");
  }
  else {
    Serial.println("MCP3208 init succeed (SPI) !");
  }

  valeur = 128;
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
    if (c == '\r' || c == '\n') return;

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
