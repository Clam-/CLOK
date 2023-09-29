#include <HTTPClient.h>
#include <WiFiClientSecure.h>

WiFiMulti WiFiMulti;
bool WIFI_CONNECTED = false;

void webSetup() {
  WiFi.mode(WIFI_STA);
  
}

void addAP(const char* ssid, const char* key) {
  WiFiMulti.addAP(ssid, key);
}

bool getURL(const char* url, const char* writefile, void (*func)(String&, String&), String &eTag) {
  if (ROOTCA == NULL || url == NULL) { return false; }
  WiFiClientSecure *client = new WiFiClientSecure;
  bool returnstatus = false;
  if(client) {
    client -> setCACert(ROOTCA);
    {
      // Add a scoping block for HTTPClient http to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient http;
      if (eTag != NULL && eTag != "") { http.addHeader("If-None-Match", eTag); http.addHeader("ETag", eTag); }
      else { http.addHeader("ETag", ""); }

      Serial.print("[HTTP] begin...\n");
      if (http.begin(*client, url)) {  // HTTP
        Serial.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTP] GET... code: %d\n", httpCode);
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            // do callback
            if (writefile == NULL) {
              func(http.getString(), http.header("ETag"));
              returnstatus = true;
            } else {
              File file = LittleFS.open(writefile);
              int size = http.writeToStream(file);
              if (size > 0) {returnstatus = true; }
              func("", "") // pass stream
            }
          } else if (httpCode == HTTP_CODE_NOT_MODIFIED) { 
            Serial.printf("304 - Not Modified.\n");
            returnstatus = true;
          } else {}
        } else {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
      } else {
        Serial.printf("[HTTP] Unable to connect\n");
      }
      // End extra scoping block
    }
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
  return returnstatus;
}
