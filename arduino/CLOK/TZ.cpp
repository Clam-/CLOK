#include "TZ.hpp"
#include "FS.hpp"
#include "utils.hpp"
#include "Tar.hpp"
#include "Web.hpp"

#include "Prefs.hpp"

// c strings in this module are all null terminated. Please ensure this is the case. EXCEPT TZ-version.

char *TZ_ENV = (char*)malloc(sizeof(char)*100);
// TZ Defaults...
const char* TZ_ZONEINFO_URL = "http://zoneinfo.nyanya.org/zoneinfo.tar";
const char* TZ_TIMEZONE = "UTC";
const char* TZ_NTP1 = "time.nist.gov"; // TODO: add handlers to actually allow changing of NTP... haha.
const char* TZ_NTP2 = "pool.ntp.org";

unsigned long TZ_CHECK_TIME = 1209600000; // (2*7*24*60*60*1000) 2weekly checks.
unsigned long TZ_TICK_DELAY = 6; // How many ticks should past between actually doing the check. This is to stop too many GETs if something goes wrong.
// The following offset means it'll first attempt to check 2mins post boot. (or until NTP syncs current time...)
unsigned long TZ_PREV_TIME = -TZ_CHECK_TIME + (60*1000*2);

BLECharacteristic BLE_TZ_zoneinfoURL("00000051-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite | BLENotify, 256);
BLECharacteristic BLE_TZ_region("00000052-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLEWrite | BLERead, 100);
BLECharacteristic BLE_TZ_timezones("00000053-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLENotify, 100);
BLECharacteristic BLE_TZ_timezone("00000054-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite, 256);
BLECharacteristic BLE_TZ_regions("00000055-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLENotify, 100);
BLECharacteristic BLE_TZ_ntp1("00000056-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite | BLENotify, 100); 
BLECharacteristic BLE_TZ_ntp2("00000057-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite | BLENotify, 100);
BLECharacteristic BLE_TZ_version("00000058-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLENotify, 10);

void TZ_BLE_Setup(BLEService clokService) {
  clokService.addCharacteristic(BLE_TZ_zoneinfoURL);
  clokService.addCharacteristic(BLE_TZ_timezone);
  clokService.addCharacteristic(BLE_TZ_region);
  clokService.addCharacteristic(BLE_TZ_timezones);
  clokService.addCharacteristic(BLE_TZ_regions);
  clokService.addCharacteristic(BLE_TZ_ntp1);
  clokService.addCharacteristic(BLE_TZ_ntp2);
  clokService.addCharacteristic(BLE_TZ_version);
  // init string values...
  BLE_TZ_regions.writeValue("_");
  BLE_TZ_region.writeValue("_");
  BLE_TZ_timezones.writeValue("_");

  BLE_TZ_region.setEventHandler(BLEWritten, BLE_TZ_regionwritten);
  BLE_TZ_timezone.setEventHandler(BLEWritten, BLE_TZ_timezonewritten);
}

void TZSetup() {
  // get/set pref strings and dump them in ble charas
  BLE_TZ_zoneinfoURL.writeValue(preferences.getString("TZ-URL", TZ_ZONEINFO_URL).c_str());
  String tzstr = preferences.getString("TZ-ZoneStr", TZ_TIMEZONE);
  BLE_TZ_timezone.writeValue(tzstr.c_str());
  String ntp1 = preferences.getString("TZ-ntp1", TZ_NTP1);
  BLE_TZ_ntp1.writeValue(ntp1.c_str());
  String ntp2 = preferences.getString("TZ-ntp2", TZ_NTP2);
  BLE_TZ_ntp2.writeValue(ntp2.c_str());
  CLOKconfigTzTime(TZ_ENV, tzstr.c_str(), ntp1.c_str(), ntp2.c_str(), true);
  sntp_set_time_sync_notification_cb(TZ_timeavailable);
  sntp_servermode_dhcp(0); // force use custom NTP servers, just in case wonky/bad DHCP setting
}

void TZ_timeavailable(struct timeval *t)
{
  Serial.println("Got time adjustment from NTP!");
}

bool TZ_DIR_REGION_OPEN = false;
bool TZ_DIR_LIST_FINISH = false;
File TZ_DIR = File();
// region set, open region dir and start iterating
void BLE_TZ_regionwritten(BLEDevice central, BLECharacteristic characteristic) {
  if (!TZ_DIR.available()) { Serial.println("TZ_Cleanup"); TZ_CleanUp(); }
  String s = String("/zoneinfo/") + (char*)BLE_TZ_region.value();
  TZ_DIR = LittleFS.open(s.c_str());
  TZ_DIR_REGION_OPEN = true;
  TZ_DIR_LIST_FINISH = false;
}

void TZ_CleanUp() {
  TZ_DIR = File();
  TZ_DIR_REGION_OPEN = false;
}

void TZ_BLE_Tick() {
  if (!TZ_DIR || TZ_DIR_LIST_FINISH) { return; }
  bool found = false;
  File tf = TZ_DIR.openNextFile();
  while (!found) {
    if (!tf) { return TZ_CleanUp(); }
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
  // write version to chara:
  updateTZversionChara();
}

void updateTZversionChara() {
  int s;
  s = preferences.getBytesLength("TZ-Version");
  char buf[s];
  preferences.getBytes("TZ-Version", buf, s);
  BLE_TZ_version.writeValue(buf, s, true);
}

void BLE_TZ_timezonewritten(BLEDevice central, BLECharacteristic characteristic) {
  // now read timezone, load timezone file and use last line as TZ env
  int s;
  s = characteristic.valueLength();
  char buf[s+1];
  memcpy(buf, BLE_TZ_timezone.value(), s);
  buf[s-1] = '\0';
  preferences.putString("TZ-TimeZone", buf);
  if (buf != "") {
    fs::File f = LittleFS.open(buf);
    String tail = FStail(f);
    // put in preference, then change tz
    // preferences.putString("TZ-ZoneStr", tail);
    CLOK_chtz(TZ_ENV, tail.c_str());
  } else {
    CLOK_chtz(TZ_ENV, "");
  }
}

void processNewZoneFile(String &body, String &etag) {
  // unpack .tar
  unpackTZdata(webGetClient());
  // read version string
  File f = LittleFS.open("/version");
  int s = f.size();
  uint8_t buf[s];
  f.read(buf, s); // not null terminated.
  preferences.putString("TZ-ETag", etag);
  preferences.putBytes("TZ-Version", buf, s);
  BLE_TZ_version.writeValue(buf, s, true);
}

int CUR_TICK = 0;
void tzCheck(unsigned long &curtime) {
  if (CUR_TICK > TZ_TICK_DELAY) {
    Serial.print("Curtime: "); Serial.println(curtime);
    if (curtime - TZ_PREV_TIME > TZ_CHECK_TIME) {
      String tzurl = preferences.getString("TZ-URL", TZ_ZONEINFO_URL);
      String etag = preferences.getString("TZ-ETag");
      if (getURL(tzurl.c_str(), "", processNewZoneFile, etag)) {
        TZ_PREV_TIME = curtime;
      } 
    }
    CUR_TICK = 0;
  }
  CUR_TICK++;
}
