#include <Arduino.h>

// FreeRTOS
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif


#include "Prefs.hpp"
#include "BLE.hpp"
#include "FS.hpp"
#include "Tar.hpp"
#include "WiFi.hpp"
#include "RootCA.hpp"
#include "Web.hpp"
#include "TZ.hpp"
#include "aClok.hpp"
#include "Alarm.hpp"

void WiFiTask(void *pvParameters); // handling wifi loop
void BLETask(void *pvParameters); // handling BLE loop
void BackgroundTasks(void *pvParameters);  //webfetch, update, etc...

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  prefsSetup();
  clokSetup();
  AlarmSetup();
  // start background tasks:
  Serial.println("Creating WiFiTask...");
  xTaskCreate(WiFiTask, "WiFiTask",  4096, NULL, 17, NULL);
  Serial.println("Creating BLETask...");
  // Needs to be pinned to Core 1, else possible boot loop crash.
  xTaskCreatePinnedToCore(BLETask, "BLETask", 4096, NULL, 17, NULL, 1);
  Serial.println("Creating BackgroundTasks...");
  xTaskCreate(BackgroundTasks, "BackgroundTasks",  8192, NULL, 17, NULL);
  Serial.println("Startup complete.");
}

// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/performance/speed.html#choosing-task-priorities-of-the-application
// maybe have dedicated ClockTask after all so I can give it good priority (>19 or =16 [less than the IP stack])
void loop() {
  // I guess clock related items are in the main loop task?
  clokTick();
  delay(2000);
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
