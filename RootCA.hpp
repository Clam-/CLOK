#include <ArduinoBLE.h>

void setRootCA(const char* cert, const char* ETag);

void rootCA_BLE_Setup(BLEService clokService);

void rootCAsetup();

void loadRootCA();

void processNewCA(String &body, String &etag);

void rootCACheck(unsigned long &now);

void rootCAURLwritten(BLEDevice central, BLECharacteristic characteristic);
