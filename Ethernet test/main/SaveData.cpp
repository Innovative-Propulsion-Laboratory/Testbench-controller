#include "SaveData.h"

bool state_file = false;
uint32_t time_since_save;
SdExFat sd;

void setupSaveData(){
    Serial.println("Initialisation du stockage SD...");
    if (!sd.begin(SdioConfig(FIFO_SDIO))) 
    {
      Serial.println("Erreur : Carte SD non détectée !");
      while (1);
    }
}

void save_data(byte* data, int size){
    if (state_file == false){
        state_file = true;
        uint32_t number = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]; // check if have the good ID
        String fileID = String(number) + ".txt";
        fp = sd.open(fileID, FILE_WRITE);
        fp.write(data, sizeof(data)); 
        time_since_save = millis();
    }
    else if(state_file == true){
        if (static_cast<uint32_t>(millis()-time_since_save) >= frequence_save){
            fp.write(data, sizeof(data));
            fp.close();
            state_file = false;
        }
        else if (static_cast<uint32_t>(millis()-time_since_save) < frequence_save){
            fp.write(data, sizeof(data));
        }
    }
}