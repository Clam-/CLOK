#include <Arduino.h>
#include "FS.h"
#include <LittleFS.h>

#include <Preferences.h>
Preferences preferences;
  
#include <ArduinoBLE.h>

void WiFiTask(void *pvParameters); // handling wifi loop
void BLETask(void *pvParameters); // handling BLE loop
void BackgroundTasks(void *pvParameters);  //webfetch, update, etc...
void ClockTask(void *pvParameters);  //webfetch, update, etc...


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  FSSetup();
  preferences.begin("clok", false);
  BLESetup();
  TZSetup();
  webSetup();
  Serial.println("Startup complete.");
}

void loop() {
  // Tick/Process Clock

  // Tick/Process Alarm

  // meebee tick timezone rule update (?????)
  
  delay(100);
}

void BackgroundTasks(void *pvParameters){
  for (;;){
    unsigned long now = millis();
    rootCACheck(now);

    delay(10);
  }
}
