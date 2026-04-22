#include <Arduino.h>
#include <SPI.h>
#include "MCP4261.h"
#include <MCP3208.h>
#include <Adafruit_MCP23X17.h>

#define NUM_VALVES 20

#define MCP_RESET 10
#define MCP_CS    29
#define ADC_CS    37

const uint8_t MCP_CS_PIN   = 1;
const uint8_t MCP_SHDN_PIN = 255;
const uint16_t POT_RESET_VALUE = 128;
const uint16_t POT_TCON_ALL_CONNECTED = 0x01FF;
const uint8_t SPI_CS_PINS[] = {MCP_CS_PIN, MCP_RESET, 28, MCP_CS, 30, 35, 36, ADC_CS};

MCP4261 pot(MCP_CS_PIN, MCP_SHDN_PIN, &SPI);
MCP3208 adc;
Adafruit_MCP23X17 mcp;



int valeur = 0;
bool testWiper0High = false;
bool testWiper1High = false;

void deselectSpiDevices()
{
  for (uint8_t i = 0; i < sizeof(SPI_CS_PINS) / sizeof(SPI_CS_PINS[0]); i++) {
    pinMode(SPI_CS_PINS[i], OUTPUT);
    digitalWrite(SPI_CS_PINS[i], HIGH);
  }
}

void resetPotRegisters()
{
  Serial.println("Reset registres MCP4261...");

  deselectSpiDevices();
  pot.setTCONMask(POT_TCON_ALL_CONNECTED);
  Serial.println("TCON MCP4261 reconnecte");

  delay(2);
  deselectSpiDevices();
  pot.reset(POT_RESET_VALUE);
  Serial.print("Wipers MCP4261 reset a ");
  Serial.println(POT_RESET_VALUE);

  valeur = POT_RESET_VALUE;
}

void printPotState()
{
  deselectSpiDevices();
  Serial.println("Etat MCP4261:");

  Serial.print("  cache wiper 0 = ");
  Serial.println(pot.getValue(0));
  Serial.print("  cache wiper 1 = ");
  Serial.println(pot.getValue(1));

  deselectSpiDevices();
  Serial.print("  device wiper 0 = ");
  Serial.println(pot.getValueDevice(0));
  deselectSpiDevices();
  Serial.print("  device wiper 1 = ");
  Serial.println(pot.getValueDevice(1));

  deselectSpiDevices();
  Serial.print("  TCON = 0x");
  Serial.println(pot.getTCONMask(), HEX);

  deselectSpiDevices();
  uint16_t status = pot.getStatusMask();
  Serial.print("  STATUS = 0x");
  Serial.println(status, HEX);
  Serial.print("  WP=");
  Serial.print(status & 0x01);
  Serial.print(" SHDN=");
  Serial.print((status >> 1) & 0x01);
  Serial.print(" WiperLock0=");
  Serial.print((status >> 2) & 0x01);
  Serial.print(" WiperLock1=");
  Serial.print((status >> 3) & 0x01);
  Serial.print(" EEPROM busy=");
  Serial.println((status >> 4) & 0x01);

  deselectSpiDevices();
  Serial.print("  NV wiper 0 = ");
  Serial.println(pot.getValueNV(0));
  deselectSpiDevices();
  Serial.print("  NV wiper 1 = ");
  Serial.println(pot.getValueNV(1));
}

void printWiper1DeviceValue()
{
  deselectSpiDevices();
  Serial.print("device wiper 1 = ");
  Serial.println(pot.getValueDevice(1));
}

void applyValue()
{
  if (valeur > 255) valeur = 255;
  if (valeur < 0)   valeur = 0;

  deselectSpiDevices();
  pot.setValue(0, (uint8_t)valeur);
  delay(2);
  deselectSpiDevices();
  pot.setValue(1, (uint8_t)valeur);

  Serial.print("valeur = ");
  Serial.println(valeur);
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  deselectSpiDevices();
  SPI.begin();

  // if (!mcp.begin_SPI(MCP_CS, 13, 12, 11, 0)) {// bool begin_SPI(cs_pin,sck_pin,miso_pin,mosi_pin, uint8_t _hw_addr = 0x00);         
  // Serial.println("MCP23X17 init FAILED (SPI) !");
  // }
  // else{Serial.println("MCP23X17 init succeed (SPI) !");}

  // // configure TOUS les pins 0..15 en sortie
  // for (int i = 0; i < 15; i++) {
  //   mcp.pinMode(i, OUTPUT);
  // }

  // adc.begin(13, 11, 12, 37); //bool begin(sck, mosi, miso, cs);

  
  pot.begin();
  Serial.println("MCP4261 init succeed (SPI) !");

  resetPotRegisters();

  Serial.println("Controle MCP4261");
  Serial.println("z : +20");
  Serial.println("s : -20");
  Serial.println("r : reset registres volatiles");
  Serial.println("p : afficher registres MCP4261");
  Serial.println("0 : alterner wiper 0 entre 40 et 220");
  Serial.println("1 : alterner wiper 1 entre 40 et 220");
  Serial.println("a : wiper 1 = 40 sans lecture");
  Serial.println("b : wiper 1 = 128 sans lecture");
  Serial.println("c : wiper 1 = 220 sans lecture");
  Serial.println("m : mesure w0=40 / w1=220");
  Serial.println("n : mesure w0=220 / w1=40");
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
    else if (c == 's')   // ✅ correction ici
    {
      valeur -= 20;
      applyValue();
    }
    else if (c == 'r')
    {
      resetPotRegisters();
    }
    else if (c == 'p')
    {
      printPotState();
    }
    else if (c == '0')
    {
      uint8_t testValue = testWiper0High ? 40 : 220;
      testWiper0High = !testWiper0High;
      deselectSpiDevices();
      Serial.print("Test wiper 0 = ");
      Serial.println(testValue);
      Serial.flush();
      bool ok = pot.setValue(0, testValue);
      Serial.println(ok ? "wiper 0 OK" : "wiper 0 FAILED");
      Serial.flush();
      delay(100);
      printPotState();
    }
    else if (c == '1')
    {
      uint8_t testValue = testWiper1High ? 40 : 220;
      testWiper1High = !testWiper1High;
      deselectSpiDevices();
      Serial.print("Test wiper 1 = ");
      Serial.println(testValue);
      Serial.flush();
      bool ok = pot.setValue(1, testValue);
      Serial.println(ok ? "wiper 1 OK" : "wiper 1 FAILED");
      Serial.flush();
      delay(100);
      Serial.println("Lecture etat apres wiper 1...");
      Serial.flush();
      printPotState();
    }
    else if (c == 'a')
    {
      deselectSpiDevices();
      Serial.println("Test wiper 1 = 40");
      Serial.flush();
      bool ok = pot.setValue(1, 40);
      Serial.println(ok ? "wiper 1 OK" : "wiper 1 FAILED");
      printWiper1DeviceValue();
      Serial.println("Fin test wiper 1 = 40");
    }
    else if (c == 'b')
    {
      deselectSpiDevices();
      Serial.println("Test wiper 1 = 128");
      Serial.flush();
      bool ok = pot.setValue(1, 128);
      Serial.println(ok ? "wiper 1 OK" : "wiper 1 FAILED");
      printWiper1DeviceValue();
      Serial.println("Fin test wiper 1 = 128");
    }
    else if (c == 'c')
    {
      deselectSpiDevices();
      Serial.println("Test wiper 1 = 220");
      Serial.flush();
      bool ok = pot.setValue(1, 220);
      Serial.println(ok ? "wiper 1 OK" : "wiper 1 FAILED");
      printWiper1DeviceValue();
      Serial.println("Fin test wiper 1 = 220");
    }
    else if (c == 'm')
    {
      Serial.println("Mesure: w0=40 / w1=220");
      deselectSpiDevices();
      Serial.println(pot.setValue(0, 40) ? "wiper 0 OK" : "wiper 0 FAILED");
      delay(5);
      deselectSpiDevices();
      Serial.println(pot.setValue(1, 220) ? "wiper 1 OK" : "wiper 1 FAILED");
      printPotState();
    }
    else if (c == 'n')
    {
      Serial.println("Mesure: w0=220 / w1=40");
      deselectSpiDevices();
      Serial.println(pot.setValue(0, 220) ? "wiper 0 OK" : "wiper 0 FAILED");
      delay(5);
      deselectSpiDevices();
      Serial.println(pot.setValue(1, 40) ? "wiper 1 OK" : "wiper 1 FAILED");
      printPotState();
    }
  }
}
