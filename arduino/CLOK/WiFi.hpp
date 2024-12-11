#include <ArduinoBLE.h>

extern bool WIFI_CONNECTED;

int WiFicheckSSID(const char* ssid, int slen);
int WiFigetUnusedSSIDSlot();
void WiFiputSSID(const char* ssid, int slen, const char* key, int klen);
void WiFiremoveSSID(const char* ssid, int slen);

void WiFiSetup();

void WiFiwpakeywritten(BLEDevice central, BLECharacteristic characteristic);
void WiFiscanwritten(BLEDevice central, BLECharacteristic characteristic);

void WiFi_BLE_Connected();
void WiFi_BLE_Tick();
void WiFi_BLE_CleanUp();
void WiFi_BLE_Setup(BLEService clokService);

void WiFiRunScan();

void WiFiTask(void *pvParameters);