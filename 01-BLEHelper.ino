
BLEService clokService("BAAD0000-5AAD-BAAD-81E1-5DE05CBADCLK"); // 

BLEByteCharacteristic ssid("BAAD0000-5AAD-BAAD-81E1-5DE05CBADCLK", BLERead | BLEWrite); // to send data on SSIDs
BLEByteCharacteristic wpakey("BAAD0001-5AAD-BAAD-81E1-5DE05CBADCLK", BLERead | BLEWrite);
BLEByteCharacteristic ntp1("BAAD0002-5AAD-BAAD-81E1-5DE05CBADCLK", BLERead | BLEWrite); 
BLEByteCharacteristic ntp2("BAAD0003-5AAD-BAAD-81E1-5DE05CBADCLK", BLERead | BLEWrite);
BLEByteCharacteristic timezone("BAAD0004-5AAD-BAAD-81E1-5DE05CBADCLK", BLERead | BLEWrite);
BLEByteCharacteristic alarmtime("BAAD0005-5AAD-BAAD-81E1-5DE05CBADCLK", BLERead | BLEWrite);
BLEByteCharacteristic alarmsound("BAAD0006-5AAD-BAAD-81E1-5DE05CBADCLK", BLERead | BLEWrite);
BLEByteCharacteristic ledlight("BAAD0007-5AAD-BAAD-81E1-5DE05CBADCLK", BLERead | BLEWrite);

void BLESetup() {
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    delay(5000);
    ESP.restart();
  }
  BLE.setLocalName("CLOK");
  BLE.setAdvertisedService(clokService);
  clokService.addCharacteristic(ssid);
  BLE.addService(clokService);
  ssids.writeValue("");
  BLE.advertise();
}

void BLETask() {
  // stuff.
  for (;;){
    BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (switchCharacteristic.written()) {
        if (switchCharacteristic.value()) {   // any value other than 0
          Serial.println("LED on");
          digitalWrite(ledPin, HIGH);         // will turn the LED on
        } else {                              // a 0 value
          Serial.println(F("LED off"));
          digitalWrite(ledPin, LOW);          // will turn the LED off
        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
    delay(100);
  }
}