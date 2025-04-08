#ifndef SAVEDATA_H
#define SAVEDATA_H

#include <SdFat.h>
#include <Arduino.h>

extern SdExFat sd;
extern ExFile fp;
extern ExFile fkp;

extern bool state_file;
extern uint32_t frequence_save;

void setupSaveData();
void save_data(byte* data, int size);

#endif