
void FSSetup(){
  if(!LittleFS.begin(true)){
    Serial.println("LittleFS Mount Failed");
    ESP.deepSleep(9999999999);
    ESP.restart();
  }
}

// I'm using ETag I don't need this after all...
bool compareFileContents(const char *s, const char * path){
    Serial.printf("Reading file: %s\r\n", path);
    int len = strlen(s);
    char buf[len];
    File file = LittleFS.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    int ret = file.read(buf, len);
    if (ret < 0 ) { Serial.println("Read failed."); }
    file.close();
    if (strcmp(buf, s)==0) {return true;}
    return false;
}



