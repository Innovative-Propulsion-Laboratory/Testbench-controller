#ifndef SAVEDATA_H
#define SAVEDATA_H

#include <SdFat.h>

extern SdExFat sd;
extern ExFile fp;
extern ExFile fkp;

extern bool state_file;
int frequence_save;
unsigned long time_since_save;

void setupSaveData();
void save_data(byte* data, int size);

#endif