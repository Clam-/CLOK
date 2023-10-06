
void FSSetup(){
  if(!LittleFS.begin(true)){
    Serial.println("LittleFS Mount Failed");
    ESP.deepSleep(9999999999);
    ESP.restart();
  }
}

String FStail(fs::File &f) {
  String line;
  while (f.available()) {
    line = f.readStringUntil('\n');
  }
  return line;
}




