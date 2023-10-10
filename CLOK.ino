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

// FreeRTOS
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

//Preferences
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_partition.h"
#include <Preferences.h>
Preferences preferences;
  
void WiFiTask(void *pvParameters); // handling wifi loop
void BLETask(void *pvParameters); // handling BLE loop
void BackgroundTasks(void *pvParameters);  //webfetch, update, etc...
bool WIFI_CONNECTED = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  preferences.begin("clok", false);
  // start background tasks:
  Serial.println("Creating WiFiTask...");
  xTaskCreate(WiFiTask, "WiFiTask",  4096, NULL, 17, NULL);
  Serial.println("Creating BLETask...");
  // Needs to be pinned to Core 1, else possible boot loop crash.
  xTaskCreatePinnedToCore(BLETask, "BLETask", 4096, NULL, 17, NULL, 1);
  Serial.println("Creating BackgroundTasks...");
  xTaskCreate(BackgroundTasks, "BackgroundTasks",  8192, NULL, 17, NULL);
  clokSetup();
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
  (void) pvParameters;
  // background setup
  webSetup();
  FSSetup();
  tarSetup();
  TZSetup();
  for (;;){
    unsigned long now = millis();
    rootCACheck(now);
    tzCheck(now);
    delay(5000);
  }
}
