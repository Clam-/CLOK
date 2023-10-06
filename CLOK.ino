#include <Arduino.h>
// utils
#include "esp_netif.h"
#include "lwip/apps/sntp.h"
#include "esp32-hal.h"
//FS
#include "FS.h"
#include <LittleFS.h>
//Wifi
#include <WiFi.h>
#include <WiFiMulti.h>
//BLE
#include <ArduinoBLE.h>
//Web
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
//Tar
#define DEST_FS_USES_LITTLEFS
#include <ESP32-targz.h>
// TZ
#include "esp_sntp.h"

//Preferences
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_partition.h"
#include <Preferences.h>
Preferences preferences;
  
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
  clokTick();
  delay(1000);
}

void BackgroundTasks(void *pvParameters){
  // backgrou setup
  webSetup();
  FSSetup();
  tarSetup();
  TZSetup();
  for (;;){
    unsigned long now = millis();
    rootCACheck(now);

    delay(100);
  }
}
