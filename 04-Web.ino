#include <HTTPClient.h>
#include <WiFiClientSecure.h>

WiFiClientSecure *WEB_CLIENT;
HTTPClient HTTP_Client;

void webSetup() {
  rootCAsetup();
  WEB_CLIENT = new WiFiClientSecure;
  if (ROOTCA != NULL) { WEB_CLIENT->setCACert(ROOTCA); }
  HTTP_Client.setReuse(true);
}

void webUpdateCA() {
  WEB_CLIENT->setCACert(ROOTCA);
}


bool getURL(const char* url, const char* writefile, void (*func)(String&, String&), String &eTag) {
  if (ROOTCA == NULL || url == NULL) { return false; }
  
  bool returnstatus = false;
  if(HTTP_Client) {
    if (eTag != NULL && eTag != "") { HTTP_Client.addHeader("If-None-Match", eTag); HTTP_Client.addHeader("ETag", eTag); }
    else { HTTP_Client.addHeader("ETag", ""); }
    HTTP_Client.addHeader("Content-Length", "");

    Serial.print("[HTTP] begin...\n");
    if (HTTP_Client.begin(*WEB_CLIENT, url)) {  // HTTP
      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = HTTP_Client.GET();
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          // do callback
          if (writefile == NULL) {
            func(HTTP_Client.getString(), HTTP_Client.header("ETag"));
            returnstatus = true;
          } else {
            func(nullptr, HTTP_Client.header("ETag"))
            returnstatus = true;
          }
        } else if (httpCode == HTTP_CODE_NOT_MODIFIED) { 
          Serial.printf("304 - Not Modified.\n");
          returnstatus = true;
        } else {}
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", HTTP_Client.errorToString(httpCode).c_str());
      }
      HTTP_Client.end();
    } else {
      Serial.printf("[HTTP] Unable to connect\n");
    }
  } else {
    Serial.println("Unable to create client");
  }
  return returnstatus;
}
