#include <ArduinoBLE.h>
#include "esp_sntp.h"

void TZ_BLE_Setup(BLEService clokService);

void TZSetup();

void TZ_timeavailable(struct timeval *t);

void BLE_TZ_regionwritten(BLEDevice central, BLECharacteristic characteristic);

void TZ_CleanUp();

void TZ_BLE_Tick();

void TZ_BLE_Connected();

void BLE_TZ_timezonewritten(BLEDevice central, BLECharacteristic characteristic);

void processNewZoneFile(String &body, String &etag);

void tzCheck(unsigned long &curtime);
