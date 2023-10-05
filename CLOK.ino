#include <Arduino.h>
#include "FS.h"
#include <LittleFS.h>

#include <Preferences.h>
Preferences preferences;
  
#include <ArduinoBLE.h>

void WiFiTask(void *pvParameters); // handling wifi loop
void BLETask(void *pvParameters); // handling BLE loop
void BackgroundTasks(void *pvParameters);  //webfetch, update, etc...


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  preferences.begin("clok", false);
  
  Serial.println("Startup complete.");
}

// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/performance/speed.html#choosing-task-priorities-of-the-application
// maybe have dedicated ClockTask after all so I can give it good priority (>19 or =16 [less than the IP stack])
void loop() {
  // I guess clock related items are in the main loop task?
  alarmTick()
  delay(1000);
}

void BackgroundTasks(void *pvParameters){
  // backgrou setup
  FSSetup();
  tarSetup();
  TZSetup();
  webSetup();
  for (;;){
    unsigned long now = millis();
    rootCACheck(now);

    delay(100);
  }
}
