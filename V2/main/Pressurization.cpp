#include "Pressurization.h"
// Implementation of Bang-Bang pressurization for a specific tank and at a specific pressure

// --------------------- VARIABLES INITIALIZATION ------------------------------
bool LOX_BB = 0, ETH_BB = 0, WATER_BB = 0;

uint16_t PS11_BB_min = 15800, PS11_BB_max = 16200;
uint16_t PS11_BBLW = 15, PS11_BBUW = 17, PS11_BB_ulim = 22000;

uint16_t PS21_BB_min = 15800, PS21_BB_max = 16200;
uint16_t PS21_BBLW = 15000, PS21_BBUW = 17000, PS21_BB_ulim = 22000;

uint16_t WATER_BB_min = 9800, WATER_BB_max = 10200;
uint16_t WATER_BBLW = 9000, WATER_BBUW = 11000, WATER_BB_ulim = 12000;

void BB_param_set(uint8_t tank, uint16_t pressure) {  
  /* Set the pressure limits for the chosen tank
    !!! THIS FUNCTION DOES NOT ENABLE THE BANG-BANG PRESSURIZATION !!!
     BB_min/BB_max: range of the wanted pressure
     BB_LW/BB_UW: lower/upper warning limits
     BB_ulim: mawimum value to be set as target pressure
     tank: 1 = LOX, 2 = ETH, 6 = WATER */
  if (tank == 1) {
    if (pressure > PS11_BB_ulim) {
      return;
    } else {
      PS11_BB_min = pressure - 200;
      PS11_BBLW = pressure - 1000;
      PS11_BB_max = pressure + 200;
      PS11_BBUW = pressure + 1000;
    }
  }
  if (tank == 2) {
    if (pressure > PS21_BB_ulim) {
      return;
    } else {
      PS21_BB_min = pressure - 200;
      PS21_BBLW = pressure - 1000;
      PS21_BB_max = pressure + 200;
      PS21_BBUW = pressure + 1000;
      Serial.print(PS21_BB_min);
      Serial.print(PS21_BBLW);
      Serial.print(PS21_BB_max);
      Serial.print(PS21_BBUW);
    }
  }
  if (tank == 6) {
    if (pressure > WATER_BB_ulim) {
      return;
    } else {
      WATER_BB_min = pressure - 200;
      WATER_BBLW = pressure - 1000;
      WATER_BB_max = pressure + 200;
      WATER_BBUW = pressure + 1000;
    }
  }

  // Send confirmation to the computer
  byte message[9] = { 0xEE, 0xEE, 0xFF, 0xFF, 0xEE, 0xEE, tank, (byte)(pressure >> 8), (byte)(pressure & 0xFF) };  // encoding using the big-endian
  reply(message, sizeof(message));
}

void BB_enable(uint8_t tank, bool command) {
  /* Enable or disable the Bang-Bang pressurization for the chosen tank
     tank: 1 = LOX, 2 = ETH, 6 = WATER
     command: 1 = enable, 0 = disable */if (command == 0) {
    if (tank == 1) { LOX_BB = 0; }
    if (tank == 2) { ETH_BB = 0; }
    if (tank == 6) { WATER_BB = 0; }
  }
  if (command == 1) {
    if (tank == 1) {
      LOX_BB = 1;
      setValve(SV11, 0);
    }
    if (tank == 2) {
      ETH_BB = 1;
      setValve(SV21, 0);
      Serial.print("ETH_BB actif");
    }
    if (tank == 6) {
      WATER_BB = 1;
      setValve(SV61, 0);
      setValve(SV62, 0);
    }
  }
  byte message[8] = { 0xEE, 0xEE, 0xFF, 0xFF, 0xDD, 0xDD, tank, command };
  reply(message, sizeof(message));
}

void BB_pressurization(uint16_t PS11, uint16_t PS21, uint16_t PS61, uint16_t PS62) {
  /* Using the last read values for the PS11, PS21, PS61 & PS62,
     this function uses Bang-Bang to pressurize the tanks at the right pressure
     THIS FUNCTION NEEDS TO BE CALLED FREQUENTLY FOR THE BANG BANG TO WORK */
  if (LOX_BB) {
    if (PS11 < PS11_BB_min) { setValve(SV33, 1); }
    if (PS11 > PS11_BB_max) { setValve(SV33, 0); }
  }
  if (ETH_BB) {
    if (PS21 < PS21_BB_min) { setValve(SV34, 1); }
    if (PS21 > PS21_BB_max) { setValve(SV34, 0); }
  }
  if (WATER_BB) {
    if ((PS61 <= WATER_BB_min) || (PS62 <= WATER_BB_min)) {
      setValve(SV52, 1);
      setValve(SV53, 1);
    }
    if ((PS61 >= WATER_BB_max) || (PS62 >= WATER_BB_max)) {
      setValve(SV52, 0);
      setValve(SV53, 0);
    }
  }
}