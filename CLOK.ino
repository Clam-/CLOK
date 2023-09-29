#define DEST_FS_USES_LITTLEFS

#include "FS.h"
#include <LittleFS.h>

#include <Preferences.h>
Preferences preferences;
  
#include <ArduinoBLE.h>

#include <WiFi.h>
#include <WiFiMulti.h>

void WiFiTask(void *pvParameters); // handling wifi loop
void BLETask(void *pvParameters); // handling BLE loop
void BackgroundTasks(void *pvParameters);  //webfetch, update, etc...

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  FSSetup();
  preferences.begin("clok", false);
  BLESetup();
  webSetup();  
  Serial.println("Startup complete.");
}



void updateTZdata() {
  // poll db
  setenv("TZ", tz, 1);
}

void loop() {
  // Tick/Process Clock

  // Tick/Process Alarm

  // meebee tick timezone rule update (?????)
  
  delay(100);
}
