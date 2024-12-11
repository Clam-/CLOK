#include <ArduinoBLE.h>
#include "Prefs.hpp"

// hmm how to represent hour and minute... I think probably an hour characteristic and a minute characteristic would be simplest
// could double stack and put 2 numbers in one byte, like x & 0xFFFF, x >> 16 & 0xFFFF
BLECharCharacteristic BLE_alarm_hour("00000081-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite | BLENotify);
BLECharCharacteristic BLE_alarm_minute("00000082-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite | BLENotify);
// The following is  7 bit flags: Sun, Mon, Tue, Wed, Thur, Fri, Sat - e.g. everyday 01111111, mon-fri 00111110
BLECharCharacteristic BLE_alarm_schedule("00000083-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite | BLENotify);
// I wonder how I should try and make different sounds...
BLECharCharacteristic BLE_alarm_sound("00000084-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite | BLENotify);
BLECharCharacteristic BLE_alarm_snoozelength("00000085-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite | BLENotify);

int ALARM_HOUR = 7;
int ALARM_MIN = 0;
int ALARM_SCHEDULE = 0;
int ALARM_SOUND = 0;
int ALARM_SNOOZELEN = 10;

void AlarmSetup() {
  // load preference stored values
  ALARM_HOUR = preferences.getChar("ALARM_HOUR", ALARM_HOUR);
  ALARM_MIN = preferences.getChar("ALARM_MIN", ALARM_MIN);
  ALARM_SCHEDULE = preferences.getChar("ALARM_SCHEDULE", ALARM_SCHEDULE);
  ALARM_SOUND = preferences.getChar("ALARM_SOUND", ALARM_SOUND);
  ALARM_SNOOZELEN = preferences.getChar("ALARM_SNOOZELEN", ALARM_SNOOZELEN);
}

void Alarm_BLE_Connected() { }
void Alarm_BLE_Tick() {

}

void Alarm_BLE_CleanUp() {

}

void AlarmTimeWritten(BLEDevice central, BLECharacteristic characteristic) {
  //WiFiputSSID((char*)BLE_WiFi_ssid.value(), BLE_WiFi_ssid.valueLength(), (char*)BLE_WiFi_wpakey.value(), BLE_WiFi_wpakey.valueLength());
  
}

void AlarmScheduleWritten(BLEDevice central, BLECharacteristic characteristic) {

}
void AlarmSnoozeLenWritten(BLEDevice central, BLECharacteristic characteristic) {

}

void Alarm_BLE_Setup(BLEService clokService) {
  clokService.addCharacteristic(BLE_alarm_hour);
  clokService.addCharacteristic(BLE_alarm_minute);
  clokService.addCharacteristic(BLE_alarm_schedule);
  clokService.addCharacteristic(BLE_alarm_sound);
  clokService.addCharacteristic(BLE_alarm_snoozelength);
  // init values, I don't know if I actually need to do this.
  BLE_alarm_hour.writeValue(ALARM_HOUR);
  BLE_alarm_minute.writeValue(ALARM_MIN);
  BLE_alarm_schedule.writeValue(ALARM_SCHEDULE);
  BLE_alarm_sound.writeValue(ALARM_SOUND);
  BLE_alarm_snoozelength.writeValue(ALARM_SNOOZELEN);
  
  BLE_alarm_minute.setEventHandler(BLEWritten, AlarmTimeWritten);
  BLE_alarm_schedule.setEventHandler(BLEWritten, AlarmScheduleWritten);
  BLE_alarm_snoozelength.setEventHandler(BLEWritten, AlarmSnoozeLenWritten);
}