#include <ArduinoBLE.h>

// hmm how to represent hour and minute... I think probably an hour characteristic and a minute characteristic would be simplest
// could double stack and put 2 numbers in one byte, like x & 0xFFFF, x >> 16 & 0xFFFF
BLECharacteristic BLE_CLOK_alarmhour("00000081-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite, 1);
BLECharacteristic BLE_CLOK_alarmminute("00000082-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite, 1);
// I wonder how I should try and make different sounds...
BLECharacteristic BLE_CLOK_alarmsound("00000083-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite, 1);


