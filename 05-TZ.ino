#include <ESP32-targz.h>

const char* TZ_ZONEINFO_URL = "http://192.168.25.250:9999/zoneinfo.tar"
BLEStringCharacteristic BLE_TZ_zoneinfoURL("BAAD0007-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | BLEWrite);
BLEStringCharacteristic BLE_TZ_timezone("BAAD0004-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | BLEWrite);
BLEStringCharacteristic BLE_TZ_regions("BAAD0004-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | Notify);
BLEStringCharacteristic BLE_TZ_region("BAAD0004-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLEWrite | BLERead);
BLEStringCharacteristic BLE_TZ_timezones("BAAD0004-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | Notify);
BLEStringCharacteristic BLE_TZ_ntp1("BAAD0002-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | BLEWrite); 
BLEStringCharacteristic BLE_TZ_ntp2("BAAD0003-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | BLEWrite);
const char *TZ_default = ""

BLE_TZ_region.setEventHandler(BLEWritten, BLE_TZ_regionwritten);

void TZSetup() {
  // get/set NTP servers from prefs...
  sntp_set_time_sync_notification_cb(timeavailable);
  sntp_servermode_dhcp(1); // allow DHCP NTP server setting (not sure if this overrides specified ones...)
  configTzTime();
}

void timeavailable(struct timeval *t)
{
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}

bool TZ_DIR_REGION_OPEN = false;
bool TZ_DIR_LIST_FINISH = false;
File TZ_DIR = nullptr;
// region set, open region dir and start iterating
void BLE_TZ_regionwritten(BLEDevice central, BLECharacteristic characteristic) {
  if (TZ_DIR != nullptr) { TZ_CleanUp(); }
  String s = String("/zoneinfo/") + characteristic.value();
  TZ_DIR = LittleFS.open(s.c_str());
  TZ_DIR_REGION_OPEN = true;
  TZ_DIR_LIST_FINISH = false;
}

void TZ_CleanUp() {
  TZ_DIR = nullptr;
  TZ_DIR_REGION_OPEN = false;
}

void TZ_BLE_Tick() {
  if (!TZ_DIR || TZ_DIR_LIST_FINISH) { return; }
  bool found = false;
  File tf = TZ_DIR.openNextFile();
  while (!found) {
    if (!tf) { return TZ_CleanUP(); }
    if (!TZ_DIR_REGION_OPEN && tf.isDirectory()) {
      // output item and return
      BLE_TZ_regions.writeValue(tf.name());
      return;
    } else if (TZ_DIR_REGION_OPEN && !tf.isDirectory()) {
      BLE_TZ_timezones.writeValue(tf.name());
      return;
    }
    tf = TZ_DIR.openNextFile();
  }
}

void TZ_BLE_Connected() {
  // open root of tz
  TZ_DIR = LittleFS.open("/zoneinfo");
  TZ_DIR_LIST_FINISH = false;
}

unsigned long TZ_NEXT_CHECK_TIME = 0;

void processNewZoneFile(String &body, String &etag) {
  // unpack .tar
  unpackTZdata();
  // read version string
  File f = LittleFS.open("/version");
  char *buffer = new char[6];
  f.read(buffer, 6);
  preferences.putString("TZ-ETag", etag);
  preferences.putString("TZ-Version", buffer);
  delete []buffer;
}
void tzTick(unsigned long curtime) {
  if (curTime > TZ_NEXT_CHECK_TIME) {
    if !(getURL(preferences.getString("TZ-URL").c_str(), NULL, processNewZoneFile, preferences.getString("TZ-ETag"))) {
      // retry...
    }
    TZ_NEXT_CHECK_TIME + 21600000; // (6*60*60*1000) 6 hour checks.
  }
}
