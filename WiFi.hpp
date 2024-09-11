#include <ArduinoBLE.h>

extern bool WIFI_CONNECTED;

int WiFicheckSSID(const char* ssid);
int WiFigetUnusedSSIDSlot();
void WiFiputSSID(const char* ssid, const char* key);
void WiFiremoveSSID(const char* ssid);

void WiFiSetup();

void WiFiwpakeywritten(BLEDevice central, BLECharacteristic characteristic);

void WiFiscanwritten(BLEDevice central, BLECharacteristic characteristic);

void WiFiWantKnown(BLEDevice central, BLECharacteristic characteristic);

void WiFi_BLE_Connected();
void WiFi_BLE_Tick();

void WiFi_BLE_CleanUp();

void WiFiRunScan();

void WiFiTask(void *pvParameters);

void WiFi_BLE_Setup(BLEService clokService);