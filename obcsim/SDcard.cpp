#include "Arduino.h"
#include "SDcard.hpp"
#include <SPI.h>
#include <SD.h>
static unsigned char conf[300];

void initSD(void){
    SD.begin(4);
    SD.remove("Datafil.txt");
}
void readSD(unsigned char* target, char *location){
  File confFile = SD.open(location, FILE_READ);
  if(confFile){
    for(int i=0; confFile.available(); i++){
      target[i] = confFile.read();
    }
    Serial.println(F("SD-card Read"));
  }
  else
    Serial.println(F("SD-card read failed")); 
  confFile.close();
}
void sendSD(unsigned char *data, unsigned long len){
    File dataFile = SD.open("Datafil.txt", FILE_WRITE);
    if(dataFile){
      int written = dataFile.write(data, len);
      dataFile.close();
      Serial.print(F("SD-card write success, "));
      Serial.print(written);
      Serial.println(F(" bytes written"));

    }
    else
      Serial.println(F("SD-card write failed"));
}
