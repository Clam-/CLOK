#include <ArduinoBLE.h>
// all the alarm clock functionality: display, alarm, etc...

// TODO: potentially allow for manually updating the time(??... later.)
// wonder how I should send this...
BLECharacteristic BLE_CLOK_localdatetime("00000071-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLENotify, 32);

time_t CLOK_NOW;
struct tm CLOK_LOCAL_TIME;
size_t CLOK_TIME_STR_SIZE = 32;
char* CLOK_TIME_STR;

void clokSetup() {
  CLOK_TIME_STR = new char[CLOK_TIME_STR_SIZE];
}

void clok_BLE_Setup(BLEService clokService) {
  
}

void clokDebugPrintTime() {
  Serial.print("Time: ");
  // e.g. 20240606T180606Z+0606 (AEDT) TUE
  strftime(CLOK_TIME_STR, CLOK_TIME_STR_SIZE, "%Y%m%dT%H%M%SZ%z (%Z) %a", &CLOK_LOCAL_TIME);
  Serial.println(CLOK_TIME_STR);
  BLE_CLOK_localdatetime.writeValue(CLOK_TIME_STR);
}

void clokTick() {
  time(&CLOK_NOW);
  localtime_r(&CLOK_NOW, &CLOK_LOCAL_TIME);
  clokDebugPrintTime();
}