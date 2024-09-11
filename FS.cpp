#include "FS.hpp"

void FSSetup(){
  // apparently don't need this because console prints out that it's already mounted (????)
  if(!LittleFS.begin(true)){
    Serial.println("LittleFS Mount Failed");
    ESP.deepSleep(9999999999);
    ESP.restart();
  }
}

String FStail(fs::File &f) {
  String line;
  while (f.available()) {
    line = f.readStringUntil('\n');
  }
  return line;
}




