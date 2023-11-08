
char *TZ_ENV = (char*)malloc(sizeof(char)*100);

const char* TZ_ZONEINFO_URL = "http://192.168.25.250:9999/zoneinfo.tar";
unsigned long TZ_CHECK_TIME = 1209600000; // (2*7*24*60*60*1000) 2weekly checks.
// The following offset means it'll first attempt to check 2mins post boot. (or until NTP syncs current time...)
unsigned long TZ_PREV_TIME = -TZ_CHECK_TIME + (60*1000*2); 

BLECharacteristic BLE_TZ_zoneinfoURL("00000051-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite | BLENotify, 256);
BLECharacteristic BLE_TZ_timezone("00000052-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite, 256);
BLECharacteristic BLE_TZ_regions("00000053-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLENotify, 100);
BLECharacteristic BLE_TZ_region("00000054-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLEWrite | BLERead, 100);
BLECharacteristic BLE_TZ_timezones("00000055-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLENotify, 100);
BLECharacteristic BLE_TZ_ntp1("00000056-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite | BLENotify, 100); 
BLECharacteristic BLE_TZ_ntp2("00000057-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite | BLENotify, 100);
const char *TZ_default = "";

void TZ_BLE_Setup() {
  clokService.addCharacteristic(BLE_TZ_zoneinfoURL);
  clokService.addCharacteristic(BLE_TZ_timezone);
  clokService.addCharacteristic(BLE_TZ_regions);
  clokService.addCharacteristic(BLE_TZ_region);
  clokService.addCharacteristic(BLE_TZ_timezones);
  clokService.addCharacteristic(BLE_TZ_ntp1);
  clokService.addCharacteristic(BLE_TZ_ntp2);
  // init string values...
  BLE_TZ_zoneinfoURL.writeValue("");
  BLE_TZ_timezone.writeValue("");
  BLE_TZ_regions.writeValue("");
  BLE_TZ_region.writeValue("");
  BLE_TZ_timezones.writeValue("");
  BLE_TZ_ntp1.writeValue("");
  BLE_TZ_ntp2.writeValue("");

  BLE_TZ_region.setEventHandler(BLEWritten, BLE_TZ_regionwritten);
  BLE_TZ_timezone.setEventHandler(BLEWritten, BLE_TZ_timezonewritten);
}

void TZSetup() {
  // get/set TZ string
  String tzstr = preferences.getString("TZ-ZoneStr");
  String ntp1 = preferences.getString("TZ-ntp1");
  String ntp2 = preferences.getString("TZ-ntp2");
  CLOKconfigTzTime(TZ_ENV, tzstr.c_str(), ntp1.c_str(), ntp2.c_str());
  // get/set NTP servers from prefs...
  sntp_set_time_sync_notification_cb(TZ_timeavailable);
  sntp_servermode_dhcp(1); // allow DHCP NTP server setting (not sure if this overrides specified ones...)
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
}


void BLE_TZ_timezonewritten(BLEDevice central, BLECharacteristic characteristic) {
  // now read timezone, load timezone file and use last line as TZ env
  String tz = String((char*)BLE_TZ_timezone.value());
  preferences.putString("TZ-TimeZone", tz);
  if (tz != "") {
    fs::File f = LittleFS.open(tz);
    String tail = FStail(f);
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
  uint8_t *buffer = new uint8_t[8];
  f.read(buffer, 8); // does this put \0 at the end ???
  preferences.putString("TZ-ETag", etag);
  preferences.putString("TZ-Version", reinterpret_cast<const char*>(buffer));
  delete []buffer;
}

void tzCheck(unsigned long &curtime) {
  Serial.print("Curtime: "); Serial.println(curtime);
  if (curtime - TZ_PREV_TIME > TZ_CHECK_TIME) {
    String tzurl = preferences.getString("TZ-URL");
    String etag = preferences.getString("TZ-ETag");
    if (getURL(tzurl.c_str(), NULL, processNewZoneFile, etag)) {
      TZ_PREV_TIME = curtime;
    } 
  }
}
