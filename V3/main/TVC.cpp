#include "TVC.h"
#include "actuators.h"
#include <list>

// 1 = Cross
// 2 = Circle
// 3 = square
// 4 = triangle
// 5 = up-down
// 6 = left right

int L1_cross[] = {214, 45, 124, 214, 45, 128};
int L2_cross[] = {45, 214, 124, 214, 45, 128};

int L1_circle[] = {148, 172, 197,
  214, 194, 185, 177, 171, 165, 160, 155, 150, 145, 141, 137, 133, 129, 125, 121, 118, 114, 111, 108, 104, 101, 98, 95, 92, 89, 87, 84, 81, 78, 76, 73, 71, 68, 66, 64, 61, 59, 57, 55, 53, 50, 48, 46, 45, 43, 41, 39, 37, 35, 34, 32, 31, 29, 27, 26, 25, 23, 22, 21, 20, 18, 17, 16, 15, 14, 14, 13, 12, 11, 11, 10, 10, 10, 9, 9, 9, 10, 10, 11, 11, 12, 13, 15, 17, 20, 23, 28, 45, 45, 64, 74, 81, 88, 93, 99, 104, 109, 113, 118, 122, 126, 130, 133, 137, 141, 144, 148, 151, 154, 157, 160, 163, 166, 169, 172, 175, 177, 180, 183, 185, 188, 190, 193, 195, 197, 199, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220, 221, 223, 225, 226, 228, 230, 231, 233, 234, 235, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 247, 248, 248, 249, 249, 249, 249, 249, 249, 249, 248, 247, 246, 245, 244, 242, 239, 235, 230, 214,
  197, 172, 148, 128};
int L2_circle[] = {148, 172, 197, 
  214, 230, 235, 239, 242, 244, 245, 246, 247, 248, 249, 249, 249, 249, 249, 249, 249, 248, 248, 247, 247, 246, 245, 244, 243, 242, 241, 240, 239, 238, 237, 235, 234, 233, 231, 230, 228, 226, 225, 223, 221, 220, 218, 216, 214, 212, 210, 208, 206, 204, 202, 199, 197, 195, 193, 190, 188, 185, 183, 180, 177, 175, 172, 169, 166, 163, 160, 157, 154, 151, 148, 144, 141, 137, 133, 130, 126, 122, 118, 113, 109, 104, 99, 93, 88, 81, 74, 64, 45, 45, 28, 23, 20, 17, 15, 13, 12, 11, 11, 10, 10, 9, 9, 9, 10, 10, 10, 11, 11, 12, 13, 14, 14, 15, 16, 17, 18, 20, 21, 22, 23, 25, 26, 27, 29, 31, 32, 34, 35, 37, 39, 41, 43, 45, 46, 48, 50, 53, 55, 57, 59, 61, 64, 66, 68, 71, 73, 76, 78, 81, 84, 87, 89, 92, 95, 98, 101, 104, 108, 111, 114, 118, 121, 125, 129, 133, 137, 141, 145, 150, 155, 160, 165, 171, 177, 185, 194, 214, 
  197, 172, 148, 128};

int L1_square[] = {148, 172, 187, 
  249, 129, 9, 129, 187, 
  172, 148, 128};
int L2_square[] = {148, 172, 187, 
  129, 9, 129, 249, 187,
  172, 148, 128};

int L1_triangle[] = {148, 172, 187, 109, 11, 187, 172, 148, 128};
int L2_triangle[] = {148, 172, 187, 11, 109, 187, 172, 148, 128};

int L1_updown[] = {214, 45};
int L2_updown[] = {45, 214};

int L1_leftright[] = {214, 45, 128};
int L2_leftright[] = {214, 45, 128};

int* L1 = nullptr;
int* L2 = nullptr;
int taille = 0;

bool state_tvc = false;
unsigned long last_time = 0;
int i = 0;
uint32_t in_between_time = 100;

// si burn fini -> finir tvc (break loop)
void loopTVC() {
  if (state_tvc) {
    if (millis() - last_time >= in_between_time) {
      last_time = millis();
      i++;
      if (i >= taille) {
        state_tvc = false;
        i=0;
        return;
      }
      Serial.print("L1 value :");
      Serial.println(L1[i]);
      Serial.print("L2 value :");
      Serial.println(L2[i]);
      Serial.println(i);

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
      in_between_time = 1000;
      break;
    case 2:
      L1 = L1_circle;
      L2 = L2_circle;
      taille = sizeof(L1_circle) / sizeof(L1_circle[0]);
      in_between_time = 20;
      break;
    case 3:
      L1 = L1_square;
      L2 = L2_square;
      taille = sizeof(L1_square) / sizeof(L1_square[0]);
      in_between_time = 1000;
      break;
    case 4:
      L1 = L1_triangle;
      L2 = L2_triangle;
      taille = sizeof(L1_triangle) / sizeof(L1_triangle[0]);
      in_between_time = 1000;
      break;
    case 5:
      L1 = L1_updown;
      L2 = L2_updown;
      taille = sizeof(L1_updown) / sizeof(L1_updown[0]);
      in_between_time = 1000;
      break;
    case 6:
      L1 = L1_leftright;
      L2 = L2_leftright;
      taille = sizeof(L1_leftright) / sizeof(L1_leftright[0]);
      in_between_time = 1000;
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