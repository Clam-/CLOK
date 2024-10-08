#include "BLE.hpp"
#include "WiFi.hpp"
#include "RootCA.hpp"
#include "TZ.hpp"
#include "aClok.hpp"

#include <ArduinoBLE.h>
// characteristics are defined in the modules that are responsible for them.
BLEService clokService("00000000-5AAD-BAAD-FFFF-5AD5ADBADC1C"); // 

void BLE_Modules_Setup() {
  WiFi_BLE_Setup(clokService);
  rootCA_BLE_Setup(clokService);
  TZ_BLE_Setup(clokService);
  clok_BLE_Setup(clokService);
}

void BLESetup() {
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    delay(5000);
    ESP.restart();
  }
  BLE.setLocalName("CLOK");
  BLE.setDeviceName("CLOK");
  BLE.setAdvertisedService(clokService);
  BLE_Modules_Setup();
  BLE.addService(clokService);
  BLE.advertise();
}

void BLE_ConnectionSetup() {
  TZ_BLE_Connected();
  WiFi_BLE_Connected();
}
void BLE_ConnectionCleanUp() {
  TZ_CleanUp();
  WiFi_BLE_CleanUp();
}
int tick = 0;
void BLE_ConnectionTick() {
  // only process every 10 ticks
  if (tick >= 10){
    TZ_BLE_Tick();
    WiFi_BLE_Tick();
    tick = 0;
  } else {
    tick++;
  }
}

void BLETask(void *pvParameters) {
  (void) pvParameters;
  // setup.
  BLESetup();
  for (;;){
    BLEDevice central = BLE.central();

    // if a central is connected to peripheral:
    if (central) {
      Serial.print("Connected to central: ");
      // print the central's MAC address:
      Serial.println(central.address());
      // BLE Connection setup
      BLE_ConnectionSetup();
      // while the central is still connected to peripheral:
      while (central.connected()) {
        // if the remote device wrote to the characteristic,
        // tick BLE related functions
        BLE_ConnectionTick();
        delay(50);
      }
      // cleanup...
      BLE_ConnectionCleanUp();
      // when the central disconnects, print it out:
      Serial.print("Disconnected from central: ");
      Serial.println(central.address());
    }
    delay(100);
  }
}