#define DEST_FS_USES_LITTLEFS

#include "FS.h"
#include <LittleFS.h>

#include <Preferences.h>
Preferences preferences;
  
#include <ArduinoBLE.h>
#include "date/tz.h"

#include <WiFi.h>
#include <WiFiMulti.h>

void WebTask(void *pvParameters);
void BLETask(void *pvParameters);

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
