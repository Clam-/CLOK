#include "WiFi.hpp"
#include <WiFi.h>
#include <WiFiMulti.h>
#include "Prefs.hpp"

const unsigned int SSID_OPTION_LEN = 31;
const unsigned int KEY_OPTION_LEN = 63;

BLECharacteristic BLE_WiFi_ssid("00000011-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLEWrite, SSID_OPTION_LEN); // to get requested SSID
BLECharacteristic BLE_WiFi_wpakey("00000012-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLEWrite, KEY_OPTION_LEN); // to get requested WPA KEY
BLECharacteristic BLE_WiFi_ssids("00000013-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLENotify, SSID_OPTION_LEN+9); // to send data on SSIDs the clock sees
BLECharCharacteristic BLE_WiFi_scan("00000014-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLERead | BLEWrite | BLENotify);
BLECharacteristic BLE_WiFi_delete("00000015-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLEWrite, SSID_OPTION_LEN); // receive SSID to delete.
BLECharacteristic BLE_WiFi_known("00000016-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLENotify, SSID_OPTION_LEN); // to send data on SSIDs the clock *knows*, so the app can remove
BLECharCharacteristic BLE_WiFi_state("00000017-5AAD-BAAD-FFFF-5AD5ADBADC1C", BLENotify | BLERead); // WiFi state. 0 = connected. >0 number of times 'disconnected event' seen in a row.
// format of something like "SSID" or ":" for end of list. Sorry if your SSID is :.

// CAUTION: Most of the time strings are not null terminated, and must be used with len.
// maybe TODO: Replace rest of String usage with cstyle strings... but this is kind of tedious, and I don't think the String performance hit would really matter.

WiFiMulti WIFI_Multi;
const char* WIFI_SSID_PREFIX = "WIFI_SSID_";
const char* WIFI_KEY_PREFIX = "WIFI_KEY_";
unsigned char WIFI_AP_LIMIT = 10;

bool WIFI_CONNECTED = false;

int WiFicheckSSID(const char* ssid, int slen) {
  String pre = String(WIFI_SSID_PREFIX);
  char buf[SSID_OPTION_LEN];
  int s;
  for (unsigned char x=0;x<WIFI_AP_LIMIT;x++) {
    String ssidcheck = pre+x;
    if (preferences.isKey(ssidcheck.c_str())) { 
      s = preferences.getBytesLength(ssidcheck.c_str());
      if (s != slen) { continue; }
      preferences.getBytes(ssidcheck.c_str(), buf, s);
      if (memcmp(ssid, buf, s) == 0){ return x; }
    }
  }
  return -1;
}
int WiFigetUnusedSSIDSlot() {
  String pre = String(WIFI_SSID_PREFIX);
  for (unsigned char x=0;x<WIFI_AP_LIMIT;x++) {
    String ssidcheck = pre+x;
    if (!preferences.isKey(ssidcheck.c_str())) { return x; }
  }
  return -1;
}
void WiFiputSSID(const char* ssid, int slen, const char* key, int klen) {
  // also put key
  // TODO: Overwrite some arbitrary slot if full (either lowest number slot or highest??)
  int slot = WiFicheckSSID(ssid, slen);
  if (slot == -1) { slot = WiFigetUnusedSSIDSlot(); }
  if (slot == -1) { Serial.println("SSIDs full."); return; }
    String ssidslot = String(WIFI_SSID_PREFIX);
    String keyslot = String(WIFI_KEY_PREFIX);
    ssidslot += slot; keyslot += slot;
    Serial.print("Size: "); Serial.print(slen); Serial.print(" Key: ("); Serial.print(ssid); Serial.println(")");
    preferences.putBytes(ssidslot.c_str(), ssid, slen);
    preferences.putBytes(keyslot.c_str(), key, klen);
}
void WiFiremoveSSID(const char* ssid, int slen) {
  // also remove key
  String pre = String(WIFI_SSID_PREFIX);
  char buf[SSID_OPTION_LEN];
  for (int x=0;x<WIFI_AP_LIMIT;x++) {
    String ssidcheck = pre+x;
    if (preferences.isKey(ssidcheck.c_str())) {
      // check data in slot.
      int s = preferences.getBytesLength(ssidcheck.c_str());
      if (slen != s) { continue; }
      preferences.getBytes(ssidcheck.c_str(), buf, s);
      if (memcmp(ssid, buf, s) == 0) {
        Serial.println("Removed ssid.");
        preferences.remove(ssidcheck.c_str());
        String keyslot = String(WIFI_KEY_PREFIX)+x;
        preferences.remove(keyslot.c_str());
        Serial.println("Removed ssid.");
        return;
      }
    }
  }
}

char WIFI_DISCONNECTS_SENT = -2;
char WIFI_DISCONNECTS = -1;
void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: 
      if (WIFI_DISCONNECTS < 0) { WIFI_DISCONNECTS = 1; }
      else if (WIFI_DISCONNECTS < 10) { WIFI_DISCONNECTS++; }
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED: WIFI_DISCONNECTS = 0; break;
  }
}

void WiFiSetup() {
  // setup Wifi Event listener
  WiFi.onEvent(WiFiEvent);

  // add stored APs. Iterate over all stored preferences for WIFI_SSID_PREFIX prefix ones...
  String pre = String(WIFI_SSID_PREFIX);
  char ssidbuf[SSID_OPTION_LEN+1];
  char keybuf[KEY_OPTION_LEN+1];
  for (int x=0;x<WIFI_AP_LIMIT;x++) {
    String ssidslot = pre+x;
    if (preferences.isKey(ssidslot.c_str())) {
      int s = preferences.getBytesLength(ssidslot.c_str());
      preferences.getBytes(ssidslot.c_str(), ssidbuf, s);
      ssidbuf[s] = '\0';
      String keyslot = String(WIFI_KEY_PREFIX)+x;
      s = preferences.getBytesLength(keyslot.c_str());
      preferences.getBytes(keyslot.c_str(), keybuf, s);
      keybuf[s] = '\0';
      for (int i=0; i<strlen(keybuf); i++) {
        Serial.print("Char: ("); Serial.print(keybuf[i]); Serial.print(") Hex: "); Serial.println(keybuf[i], HEX);
      }
      WIFI_Multi.addAP(ssidbuf, keybuf);
    }
  }
  WiFi.mode(WIFI_STA);
  WIFI_Multi.run();
}

void WiFiDisconnectBLE() {
  BLEDevice central = BLE.central();
  if (central && central.connected()) { central.disconnect(); }
}

void WiFiwpakeywritten(BLEDevice central, BLECharacteristic characteristic) {
  WiFiputSSID((char*)BLE_WiFi_ssid.value(), BLE_WiFi_ssid.valueLength(), (char*)BLE_WiFi_wpakey.value(), BLE_WiFi_wpakey.valueLength());
  //reboot...
  Serial.println("RESTARTING...");
  // disconnect bluetooth first
  delay(1500);
  WiFiDisconnectBLE();
  delay(1000);
  ESP.restart();
}

void WiFiSSIDDelete(BLEDevice central, BLECharacteristic characteristic) {
  WiFiremoveSSID((char*)characteristic.value(), characteristic.valueLength());
  // reboot...
  Serial.println("RESTARTING...");
  // disconnect bluetooth first
  delay(1500);
  WiFiDisconnectBLE();
  delay(1000);
  ESP.restart();
}

// relating to output SSIDs...
char WIFI_Scan = 0;
int WIFI_ScanProgress = 0; // 0 = not scanning, 1 = scan requested and delivering results, 2 = scan results delivered.
int WIFI_ScanResults = 0;
int WIFI_ScanCurrent = 0;

void WiFiscanwritten(BLEDevice central, BLECharacteristic characteristic) {
  Serial.print("Recv scan: ");
  Serial.println(int(BLE_WiFi_scan.value()));
  WIFI_Scan = BLE_WiFi_scan.value();
}

unsigned char WIFI_Known_Index = WIFI_AP_LIMIT+1;

void WiFi_BLE_Connected() { }
void WiFi_BLE_Tick() {
  // send wifi status if changed
  if (WIFI_DISCONNECTS != WIFI_DISCONNECTS_SENT) {
    BLE_WiFi_state.writeValue(WIFI_DISCONNECTS);
    WIFI_DISCONNECTS_SENT = WIFI_DISCONNECTS;
    return;
  }
  // potentially send known SSIDs...
  int s;
  char buf[SSID_OPTION_LEN+1];
  if (WIFI_ScanProgress == 0 && WIFI_Known_Index <= WIFI_AP_LIMIT) {
    while (WIFI_Known_Index < WIFI_AP_LIMIT) {
      String pre = String(WIFI_SSID_PREFIX);
      String ssidcheck = pre+WIFI_Known_Index;
      WIFI_Known_Index++;
      Serial.print("Checking "); Serial.println(ssidcheck);
      if (preferences.isKey(ssidcheck.c_str())) { 
        s = preferences.getBytesLength(ssidcheck.c_str());
        preferences.getBytes(ssidcheck.c_str(), buf, s); // I could either null terminate this, or just send it over bluetooth with the length. The latter I guess is more efficient.
        BLE_WiFi_known.writeValue(buf, s, true);
        return;
      }
    }
    BLE_WiFi_known.writeValue(":"); // end of list...
    WIFI_Known_Index++;
    return;
  }
  WIFI_Known_Index++; // using this as a delay before sending next round of known SSIDs, to continuously send.
  if (WIFI_Known_Index > WIFI_AP_LIMIT+30) { WIFI_Known_Index = 0; }
  if (WIFI_ScanProgress > 0) {
    Serial.print("Scan Current: "); Serial.print(WIFI_ScanCurrent); Serial.print(" Results: "); Serial.println(WIFI_ScanResults);
    if (WIFI_ScanCurrent < WIFI_ScanResults) {
      String tstr = String(WiFi.RSSI(WIFI_ScanCurrent))+"|"+WiFi.SSID(WIFI_ScanCurrent);
      BLE_WiFi_ssids.writeValue(tstr.c_str()); // this is null terminated by default so it's fine.
      WIFI_ScanCurrent++;
    } else {
      WIFI_ScanProgress = 2;
      WIFI_Scan = 0;
      BLE_WiFi_scan.writeValue(WIFI_Scan);
    }
  }
}

void WiFi_BLE_CleanUp() {
  WIFI_Scan = 0;
  WIFI_ScanProgress = 0;
  WIFI_ScanResults = 0;
  WIFI_ScanCurrent = 0;
}

void WiFiRunScan() {
  WIFI_ScanResults = WiFi.scanNetworks();
  WIFI_ScanCurrent = 0;
  WIFI_ScanProgress = 1; 
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
    Serial.print("Scan: "); Serial.println(int(WIFI_Scan));
    if (WIFI_Scan == 1 && WIFI_ScanProgress == 0) {
      Serial.println("Running scan...");
      WiFi.scanDelete();
      WiFiRunScan();
    } else if (WIFI_ScanProgress == 2) {
        Serial.println("Cleaning up scan.");
        WiFi.scanDelete();
        WIFI_ScanProgress = 0;
    } else if (WIFI_Scan == 0 && WIFI_ScanProgress == 0) {
      Serial.println("Attempting to connect to WiFi...");
      
      int status = WIFI_Multi.run(); // 6 == disconnected, but if spammed, probably means incorrect password.

    }
    while (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected...");
      WIFI_CONNECTED = true;
      if (WIFI_Scan > 0) {
        Serial.println("Disconnecting to scan.");
        WiFi.disconnect();
        WIFI_CONNECTED = false;
      }
      delay(1000);
    }
    delay(500);
  }
}

void WiFi_BLE_Setup(BLEService clokService) {
  // hopefully the order I am registering these characteristics is the order that they are discovered in!!
  clokService.addCharacteristic(BLE_WiFi_ssid);
  clokService.addCharacteristic(BLE_WiFi_wpakey);
  clokService.addCharacteristic(BLE_WiFi_ssids);
  clokService.addCharacteristic(BLE_WiFi_scan);
  clokService.addCharacteristic(BLE_WiFi_delete);
  clokService.addCharacteristic(BLE_WiFi_known);
  clokService.addCharacteristic(BLE_WiFi_state);
  // init String values, I don't know if I actually need to do this.
  BLE_WiFi_ssids.writeValue("|");
  BLE_WiFi_ssid.writeValue("_");
  BLE_WiFi_wpakey.writeValue("_");
  BLE_WiFi_state.writeValue(WIFI_DISCONNECTS);
  BLE_WiFi_wpakey.setEventHandler(BLEWritten, WiFiwpakeywritten);
  BLE_WiFi_scan.setEventHandler(BLEWritten, WiFiscanwritten);
  BLE_WiFi_delete.setEventHandler(BLEWritten, WiFiSSIDDelete);
}