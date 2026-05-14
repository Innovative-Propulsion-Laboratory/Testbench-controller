#include "TVC.h"
#include "actuators.h"
#include <list>

// 1 = Cross
// 2 = Circle
// 3 = square
// 4 = triangle
// 5 = up-down
// 6 = left right

int L1_cross[] = {214, 45, 124, 214, 45};
int L2_cross[] = {45, 214, 124, 214, 45};

int L1_circle[] = {45, 17, 11, 9, 10, 12, 
  15, 20, 25, 31, 37, 45, 53, 61, 71, 81, 
  92, 104, 118, 133, 150, 171, 214, 214, 242, 247, 
  249, 249, 247, 243, 239, 234, 228, 221, 214, 206, 
  197, 188, 177, 166, 154, 141, 126, 109, 88, 45};

int L2_circle[] = {45, 88, 109, 126, 141, 154, 
  166, 177, 188, 197, 206, 214, 221, 228, 234, 239, 
  243, 247, 249, 249, 247, 242, 214, 214, 171, 150, 
  133, 118, 104, 92, 81, 71, 61, 53, 45, 37, 
  31, 25, 20, 15, 12, 10, 9, 11, 17, 45};

int L1_square[] = {187, 249, 129, 9, 129, 187};
int L2_square[] = {187, 129, 9, 129, 249, 187};

int L1_triangle[] = {187, 109, 11, 187};
int L2_triangle[] = {187, 11, 109, 187};

int L1_updown[] = {214, 45};
int L2_updown[] = {45, 214};

int L1_leftright[] = {214, 45};
int L2_leftright[] = {214, 45};

int* L1 = nullptr;
int* L2 = nullptr;
int taille = 0;

bool state_tvc = false;
unsigned long last_time = 0;
int i = 0;
uint32_t in_between_time = 100;

// si burn fini -> finir tvc (break loop)
void loopTVC(int shape) {
  if (state_tvc) {
    if (millis() - last_time >= in_between_time) {
      last_time = millis();
      i++;
      if (i >= taille) {
        state_tvc = false;
        normalActuators();
      }
      writePotWiper(0, (uint8_t)L1[i]);
      writePotWiper(1, (uint8_t)L2[i]);
    }
  }
}

void shapeTVC(int shape) {
  switch (shape) {
    case 1:
      L1 = L1_cross;
      L2 = L2_cross;
      taille = sizeof(L1_cross) / sizeof(L1_cross[0]);
      in_between_time = 100;
      break;
    case 2:
      L1 = L1_circle;
      L2 = L2_circle;
      taille = sizeof(L1_circle) / sizeof(L1_circle[0]);
      in_between_time = 50;
      break;
    case 3:
      L1 = L1_square;
      L2 = L2_square;
      taille = sizeof(L1_square) / sizeof(L1_square[0]);
      in_between_time = 100;
      break;
    case 4:
      L1 = L1_triangle;
      L2 = L2_triangle;
      taille = sizeof(L1_triangle) / sizeof(L1_triangle[0]);
      in_between_time = 100;
      break;
    case 5:
      L1 = L1_updown;
      L2 = L2_updown;
      taille = sizeof(L1_updown) / sizeof(L1_updown[0]);
      in_between_time = 100;
      break;
    case 6:
      L1 = L1_leftright;
      L2 = L2_leftright;
      taille = sizeof(L1_leftright) / sizeof(L1_leftright[0]);
      in_between_time = 100;
      break;
    default:
      state_tvc = false;
      normalActuators();
  }
}

void confirmTVCstate(bool state) {
  if (state){
    state_tvc = true;
  } else {
    state_tvc = false;
  }
}

// =============================================================
// =============================================================


void shape(int shape) {
  switch (shape) {
    case 1:
      Cross();
      break;
    case 2:
      Circle();
      break;
    case 3:
      Square();
      break;
    case 4:
      Triangle();
      break;
    case 5:
      UpandDown();
      break;
    case 6:
      LeftandRight();
      break;
    default:
      normalActuators();
  }
}

void Cross() {
  // make sure we start at the center position
  normalActuators();
  // move to top right
  writePotWiper(0, 129);
  writePotWiper(1, 9);
  delay(500);
  // move to bottom left
  writePotWiper(0, 129);
  writePotWiper(1, 249);
  delay(500);
  // go back to center
  normalActuators();
  delay(500);
  // move to top left
  writePotWiper(0, 249);
  writePotWiper(1, 129);
  delay(500);
  // move to bottom right
  writePotWiper(0, 9);
  writePotWiper(1, 129);
  delay(500);
  // go back to center
  normalActuators();
  delay(500);
}

void Circle() {
  // strating : x = -7.8, y = 0, top center
  for (int i = 0; i < taille; i++) {
    writePotWiper(0, (uint8_t)L1_circle[i]);
    writePotWiper(1, (uint8_t)L2_circle[i]);
    delay(100);
  }
  normalActuators();
  delay(500);
}

// 7.8°
void Square() {
  // 1. au milieu en haut
  writePotWiper(0, 187);
  writePotWiper(1, 67); //six seveeeen
  delay(500);
  // 2. angle en haut a droite
  writePotWiper(0, 129);
  writePotWiper(1, 9);
  delay(500);
  // 3. angle en bas a droite (prob, peut etre a gauche)
  writePotWiper(0, 9);
  writePotWiper(1, 129);
  delay(500);
  // 4. angle en bas a gauche
  writePotWiper(0, 129);
  writePotWiper(1, 249);
  delay(500);
  // 5. angle en haut a gauche
  writePotWiper(0, 249);
  writePotWiper(1, 129);
  delay(500);
  // 6. re au milieu en haut
  writePotWiper(0, 187);
  writePotWiper(1, 67); //six seveeeen
  delay(500);
  // 7. au centre position de base
  normalActuators();
  delay(500);
}

void Triangle() {
  delay(1000);
  // Equilateral triangle 
  // top center max
  writePotWiper(0, 187);
  writePotWiper(1, 67); //six seveeeen
  delay(500);
  // bottom right point : x = 8.98, y = 6.35
  writePotWiper(0, 11);
  writePotWiper(1, 150);
  delay(500);
  // bottom left point : x = 8.98, y = -6.35
  writePotWiper(0, 109);
  writePotWiper(1, 247);
  delay(500);
  // top center max
  writePotWiper(0, 187);
  writePotWiper(1, 67); //six seveeeen
  delay(500);
  // back to top center
  normalActuators();
  delay(500);
}

void UpandDown() {
  //tout en haut
  writePotWiper(0, 214);
  writePotWiper(1, 45);
  delay(500);
  //tout en bas
  writePotWiper(0, 45);
  writePotWiper(1, 45); 
  delay(500);
  
  normalActuators();
  delay(500);

  // for (int i = 0; i <= 255; i += 20) {
  //   writePotWiper(0, (uint8_t)i);
  //   writePotWiper(1, (uint8_t)(255 - i));
  //   // Serial.print("Increasing left, decreasing right: ");
  //   // Serial.println(i);
  //   delay(500);
  // }
  // for (int i = 255; i >= 0; i -= 20) {
  //   writePotWiper(0, (uint8_t)i);
  //   writePotWiper(1, (uint8_t)(255 - i));
  //   // Serial.print("Decreasing left, increasing right: ");
  //   // Serial.println(i);
  //   delay(500);
  // }
}

void LeftandRight() {
  //tout en haut 
  writePotWiper(0, 214);
  writePotWiper(1, 45);
  delay(500);
  // tout en bas
  writePotWiper(0, 45);
  writePotWiper(1, 214);
  delay(500);

  normalActuators();
  
  // for (int i = 0; i <= 255; i += 20) {
  //   writePotWiper(0, (uint8_t)i);
  //   writePotWiper(1, (uint8_t)i);
  //   // Serial.print("Increasing value: ");
  //   // Serial.println(i);
  //   delay(500);
  // }
  // for (int i = 255; i >= 0; i -= 20) {
  //   writePotWiper(0, (uint8_t)i);
  //   writePotWiper(1, (uint8_t)i);
  //   // Serial.print("Decreasing value: ");
  //   // Serial.println(i);
  //   delay(500);
  // }
}