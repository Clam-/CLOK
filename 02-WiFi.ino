BLEStringCharacteristic BLE_WiFi_ssids("BAAD0000-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | BLENotify, 256); // to send data on SSIDs
BLEStringCharacteristic BLE_WiFi_ssid("BAAD0000-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | BLEWrite, 256); // to send data on SSIDs
BLEStringCharacteristic BLE_WiFi_wpakey("BAAD0001-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | BLEWrite, 256);

WiFiMulti WIFI_Multi;
bool WIFI_CONNECTED = false;
const char* WIFI_SSID_PREFIX = "WIFI_SSID_";
const char* WIFI_KEY_PREFIX = "WIFI_KEY_";

void WiFi_BLE_Setup() {
  clokService.addCharacteristic(BLE_WiFi_ssids);
  clokService.addCharacteristic(BLE_WiFi_ssid);
  clokService.addCharacteristic(BLE_WiFi_wpakey);
  BLE_WiFi_wpakey.setEventHandler(BLEWritten, WiFiwpakeywritten);
}
int WIFI_AP_LIMIT = 10;
int WiFicheckSSID(const char* ssid) {
  String pre = String(WIFI_SSID_PREFIX);
  for (int x=0;x<WIFI_AP_LIMIT;x++) {
    String ssidcheck = pre+x;
    if (preferences.isKey(ssidcheck.c_str())) { 
      if (preferences.getString(ssidcheck.c_str()) == ssid){ return x; }
    }
  }
  return -1;
}
int WiFigetUnusedSSIDSlot() {
  String pre = String(WIFI_SSID_PREFIX);
  for (int x=0;x<WIFI_AP_LIMIT;x++) {
    String ssidcheck = pre+x;
    if (!preferences.isKey(ssidcheck.c_str())) { return x; }
  }
  return -1;
}
void WiFiputSSID(const char* ssid, const char* key) {
  // also put key
  if (WiFicheckSSID(ssid) == -1) {
    int slot = WiFigetUnusedSSIDSlot();
    if (slot == -1) { Serial.println("SSIDs full."); return; }
      String ssidslot = String(WIFI_SSID_PREFIX);
      String keyslot = String(WIFI_KEY_PREFIX);
      ssidslot += slot; keyslot += slot;
      preferences.putString(ssidslot.c_str(), ssid);
      preferences.putString(keyslot.c_str(), key);
  }
}
void WiFiremoveSSID(const char* ssid) {
  // also remove key
  String pre = String(WIFI_SSID_PREFIX);
  for (int x=0;x<WIFI_AP_LIMIT;x++) {
    String ssidcheck = pre+x;
    if (preferences.isKey(ssidcheck.c_str())) {
      preferences.remove(ssidcheck.c_str());
      String keyslot = String(WIFI_KEY_PREFIX)+x;
      preferences.remove(keyslot.c_str());
      return;
    }
  }
}

void WiFiSetup() {
  // add stored APs. Iterate over all stored preferences for WIFI_SSID_PREFIX prefix ones...
  String pre = String(WIFI_SSID_PREFIX);
  for (int x=0;x<WIFI_AP_LIMIT;x++) {
    String ssidslot = pre+x;
    if (preferences.isKey(ssidslot.c_str())) {
      String ssid = preferences.getString(ssidslot.c_str());
      String keyslot = String(WIFI_KEY_PREFIX)+x;
      String wpakey = preferences.getString(keyslot.c_str());
      WIFI_Multi.addAP(ssid.c_str(), wpakey.c_str());
    }
  }
  WiFi.mode(WIFI_STA);
  WIFI_Multi.run();
}

void WiFiwpakeywritten(BLEDevice central, BLECharacteristic characteristic) {
  WiFiputSSID(BLE_WiFi_ssid.value().c_str(), BLE_WiFi_wpakey.value().c_str());
  // reboot...
  delay(3000);
  ESP.restart();
}

// functions relating to output SSIDs...
bool WIFI_DoScan = false;
bool WIFI_ScanComplete = false;
int WIFI_ScanResults = 0;
int WIFI_ScanCurrent = 0;
int WIFI_ScanCount = 0;
void WiFi_BLE_Connected() {
  WIFI_DoScan = true;
  WIFI_ScanCount = 0;
}
void WiFi_BLE_Tick() {
  if (WIFI_ScanComplete && WIFI_ScanCurrent < WIFI_ScanResults) {
    BLE_WiFi_ssids.writeValue(String(WiFi.RSSI(WIFI_ScanCurrent))+"|"+WiFi.SSID(WIFI_ScanCurrent));
    WIFI_ScanCurrent++;
  }
  WIFI_ScanComplete = false;
}

void WiFi_BLE_CleanUp() {
  WiFi.scanDelete();
  BLE_WiFi_ssids.writeValue("");
  WIFI_DoScan = false;
}

void WiFiRunScan() {
  WIFI_ScanResults = WiFi.scanNetworks();
  WIFI_ScanComplete = true;
  return;
}

void WiFiTask() {
  // stuff.
  WiFiSetup();
  for (;;){
    // WiFi mainloop
    // If scanRequested, turn off Wifi and initiate scan...
    //
    if (WIFI_DoScan && WIFI_ScanCount < 4) {
      if (WIFI_ScanComplete == false) {
        WiFiRunScan(); WIFI_ScanCount++;
      }
    } else {
      WIFI_Multi.run();
    }
    while (WiFi.status() == WL_CONNECTED) {
      if (WIFI_DoScan) {
        WiFi.disconnect();
      }
      delay(10);
    }
    delay(100);
  }
}