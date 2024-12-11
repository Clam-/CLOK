#include <ArduinoBLE.h>

void AlarmSetup();

//void AlarmA(BLEDevice central, BLECharacteristic characteristic);
//void AlarmB(BLEDevice central, BLECharacteristic characteristic);

void Alarm_BLE_Connected();
void Alarm_BLE_Tick();
void Alarm_BLE_CleanUp();
void Alarm_BLE_Setup(BLEService clokService);