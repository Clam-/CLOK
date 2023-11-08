// all the alarm clock functionality: display, alarm, etc...

// hmm how to represent hour and minute... I think probably an hour characteristic and a minute characteristic would be simplest
// could double stack and put 2 numbers in one byte, like x & 0xFFFF, x >> 16 & 0xFFFF
BLECharacteristic BLE_CLOK_alarmhour("00000071-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite, 1);
BLECharacteristic BLE_CLOK_alarmminute("00000072-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite, 1);
// I wonder how I should try and make different sounds...
BLECharacteristic BLE_CLOK_alarmsound("00000073-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite, 1);
// this in reality is 0-15 based on MAX7221 chip 'intensity' register
BLECharacteristic BLE_CLOK_brightness("00000074-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite, 1);
// wonder how I should send this...
BLECharacteristic BLE_CLOK_localdatetime("00000075-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLENotify, 1);

time_t ALARM_NOW;
struct tm ALARM_LOCAL_TIME;
size_t ALARM_STR_SIZE = 25;
char* CLOK_TIME_STR;

void clokSetup() {
  CLOK_TIME_STR = new char[ALARM_STR_SIZE];
}

void clok_BLE_Setup() {
  
}

void clokUdateDisplay() {
  Serial.print("Time: ");
  
  strftime(CLOK_TIME_STR, ALARM_STR_SIZE, "%Y%m%dT%H%M%SZ%z", &ALARM_LOCAL_TIME);
  Serial.println(CLOK_TIME_STR);
}

void clokTick() {
  time(&ALARM_NOW);
  localtime_r(&ALARM_NOW, &ALARM_LOCAL_TIME);
  clokUdateDisplay();
}