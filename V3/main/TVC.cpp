#include "TVC.h"
#include "actuators.h"
#include <list>


int L1_circle[] = {45, 88, 109, 126, 141, 154, 
  166, 177, 188, 197, 206, 214, 221, 228, 234, 239, 
  243, 247, 249, 249, 247, 242, 214, 214, 171, 150, 
  133, 118, 104, 92, 81, 71, 61, 53, 45, 37, 
  31, 25, 20, 15, 12, 10, 9, 11, 17, 45};

int L2_circle[] = {214, 242, 247, 249, 249, 247, 
  243, 239, 234, 228, 221, 214, 206, 197, 188, 177, 
  166, 154, 141, 126, 109, 88, 45, 45, 17, 11, 
  9, 10, 12, 15, 20, 25, 31, 37, 45, 53, 
  61, 71, 81, 92, 104, 118, 133, 150, 171, 214};

int taille = sizeof(L1_circle) / sizeof(L1_circle[0]); 

// 1 = Cross
// 2 = Circle
// 3 = square
// 4 = up-down
// 5 = left right

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
      UpandDown();
      break;
    case 5:
      LeftandRight();
      break;
    default:
      normalActuators();
  }
}


void triangle() {
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
  // back to top center
  normalActuators();
  delay(500);
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
    delay(500);
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

void UpandDown() {
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

void LeftandRight() {
  //tout a gauche
  writePotWiper(0, 214);
  writePotWiper(1, 214);
  delay(500);
  //tout a droite
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