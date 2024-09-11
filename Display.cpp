#include <ArduinoBLE.h>

// this in reality is 0-15 based on MAX7221 chip 'intensity' register
BLECharacteristic BLE_CLOK_brightness("00000074-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite, 1);