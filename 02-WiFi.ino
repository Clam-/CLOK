#include <WiFi.h>
#include <WiFiMulti.h>

BLEStringCharacteristic BLE_WiFi_ssids("BAAD0000-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | Notify); // to send data on SSIDs
BLEStringCharacteristic BLE_WiFi_ssid("BAAD0000-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | BLEWrite); // to send data on SSIDs
BLEStringCharacteristic BLE_WiFi_wpakey("BAAD0001-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | BLEWrite);


BLE_WiFi_wpakey.setEventHandler(BLEWritten, WiFiwpakeywritten);

WiFiMulti WIFI_Multi;
bool WIFI_CONNECTED = false;

void WiFiSetup() {
  // add stored APs. Iterate over all stored preferences for AP_ prefix ones...
  nvs_iterator_t it = NULL;
  esp_err_t res = nvs_entry_find(NULL, "CLOK", NVS_TYPE_STR, &it);
  while(res == ESP_OK) {
      nvs_entry_info_t info;
      nvs_entry_info(it, &info); // Can omit error check if parameters are guaranteed to be non-NULL
      if (prefix("AP_", info.key)) {
        String wpakey = preferences.getString(info.key);
        char *ssid = info.key;
        ssid += 3;
        Serial.printf("Adding AP: '%s' \n", ssid);
        WIFI_Multi.addAP(ssid, wpakey);
      }
      res = nvs_entry_next(&it);
  }
  nvs_release_iterator(it);
  WiFi.mode(WIFI_STA);
  WIFI_Multi.run();
}

void WiFiwpakeywritten(BLEDevice central, BLECharacteristic characteristic) {
  // check if already in pref store...
  String key = "AP_"+BLE_WiFi_ssid.value();
  preferences.putString(key.c_str(), characteristic.value());
  // reboot...
  delay(3000);
  ESP.restart();
}

// function to output SSIDs...
bool WIFI_DoScan = false;
bool WIFI_ScanComplete = false;
int WIFI_ScanResults = 0;
int WIFI_ScanCurrent = 0;
void WiFi_BLE_Connected() {
  WIFI_DoScan = true;
}
void WiFi_BLE_Tick() {
  if (WIFI_ScanComplete && WIFI_ScanCurrent < WIFI_ScanResults) {
    BLE_WiFi_ssids.writeValue(String(WiFi.RSSI(i))+"|"+WiFi.SSID(WIFI_ScanCurrent));
    WIFI_ScanCurrent++;
  }
}

void WiFi_BLE_CleanUp() {
  WiFi.scanDelete();
  BLE_WiFi_ssids.writeValue("");
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
    if (WIFI_DoScan) {
      WiFiRunScan();
    } else {
      WIFI_Multi.run();
    }
    while (WiFi.status() == WL_CONNECTED) {
      if (WIFI_DoScan) {
        WiFi.disconnect();
      }
      delay(100);
    }
    delay(10);
  }
}