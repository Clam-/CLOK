BLECharacteristic BLE_WiFi_ssids("00000011-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLENotify, 40); // to send data on SSIDs
BLECharacteristic BLE_WiFi_ssid("00000012-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLEWrite, 40); // to get requested SSID
BLECharacteristic BLE_WiFi_wpakey("00000013-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLEWrite, 65); // to get requested WPA KEY
BLEBoolCharacteristic BLE_WiFi_doscan("00000014-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite | BLENotify); 

WiFiMulti WIFI_Multi;
const char* WIFI_SSID_PREFIX = "WIFI_SSID_";
const char* WIFI_KEY_PREFIX = "WIFI_KEY_";

void WiFi_BLE_Setup() {
  clokService.addCharacteristic(BLE_WiFi_ssids);
  clokService.addCharacteristic(BLE_WiFi_ssid);
  clokService.addCharacteristic(BLE_WiFi_wpakey);
  clokService.addCharacteristic(BLE_WiFi_doscan);
  // init String values
  BLE_WiFi_ssids.writeValue("_");
  BLE_WiFi_ssid.writeValue("_");
  BLE_WiFi_wpakey.writeValue("_");
  BLE_WiFi_wpakey.setEventHandler(BLEWritten, WiFiwpakeywritten);
  BLE_WiFi_doscan.setEventHandler(BLEWritten, WiFidoscanwritten);
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
  WiFiputSSID((char*)BLE_WiFi_ssid.value(), (char*)BLE_WiFi_wpakey.value());
  // reboot...
  delay(3000);
  ESP.restart();
}

// functions relating to output SSIDs...
bool WIFI_DoScan = false;
bool WIFI_ScanReady = false;
int WIFI_ScanResults = 0;
int WIFI_ScanCurrent = 0;
void WiFi_BLE_Connected() {
  
}

void WiFidoscanwritten(BLEDevice central, BLECharacteristic characteristic) {
  Serial.print("Recv doscan: ");
  Serial.println(BLE_WiFi_doscan.value());
  if (BLE_WiFi_doscan.value()) {
    WIFI_DoScan = true;
  } else {
    WIFI_DoScan = false;
  }
}
void WiFi_BLE_Tick() {
  Serial.print(WIFI_ScanReady);Serial.print("Scan Current: "); Serial.print(WIFI_ScanCurrent); Serial.print("/"); Serial.println(WIFI_ScanResults);  
  if (WIFI_ScanReady) {
      if (WIFI_ScanCurrent < WIFI_ScanResults) {
      String tstr = String(WiFi.RSSI(WIFI_ScanCurrent))+"|"+WiFi.SSID(WIFI_ScanCurrent);
      Serial.print("Writing SSID: "); Serial.println(tstr);
      BLE_WiFi_ssids.writeValue(tstr.c_str());
      WIFI_ScanCurrent++;
    } else {
      WIFI_ScanReady = false;
    }
  }
}

void WiFi_BLE_CleanUp() {
  WIFI_DoScan = false;
}

void WiFiRunScan() {
  WIFI_ScanResults = WiFi.scanNetworks();
  WIFI_ScanCurrent = 0;
  WIFI_ScanReady = true; 
  return;
}

void WiFiTask(void *pvParameters) {
  (void) pvParameters;
  // stuff.
  WiFiSetup();
  for (;;){
    // WiFi mainloop
    // If scanRequested, turn off Wifi and initiate scan...
    WIFI_CONNECTED = false;
    Serial.print("DoScan: "); Serial.println(WIFI_DoScan);
    if (WIFI_DoScan) {
      if (WIFI_ScanReady == false) {
        WiFi.scanDelete();
        WiFiRunScan();
      }
    } else {
      WiFi.scanDelete();
      WIFI_Multi.run();
    }
    while (WiFi.status() == WL_CONNECTED) {
      WIFI_CONNECTED = true;
      if (WIFI_DoScan) {
        WiFi.disconnect();
        WIFI_CONNECTED = false;
      }
      delay(10);
    }
    delay(2000);
  }
}