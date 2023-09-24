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

const char* URL1 = NULL;
const char* URL1writefile = NULL;
void (*URL1Callback)(void);
const char* URL2 = NULL;
const char* URL2writefile = NULL;
void (*URL2Callback)(void);
const char* URL3 = NULL;
const char* URL3writefile = NULL;
void (*URL3Callback)(void);

void queueGET(const char* url, const char* writefile, void (*func)(const char*)) {
  if (URL1 == NULL) {
    URL1 = url;
    URL1writefile = writefile;
    URL1Callback = func;
  } else if (URL2 == NULL) {
    URL2 = url;
    URL2writefile = writefile;
    URL2Callback = func;
  } else if (URL3 == NULL) {
    URL2 = url;
    URL3writefile = writefile;
    URL3Callback = func;
  } else {
    Serial.write("Too busy to GET: %s\n", url);
  }
}

// finish this.
const char* writeFromStream(HTTPClient *http) {
  WiFiClient *stream = http.getStreamPtr();
  int len = http.getSize();
  // create buffer for read
  uint8_t buff[128] = { 0 };
  File file = LittleFS.open(URL1writefile);
  // read all data from server
  while(http.connected() && (len > 0 || len == -1)) {
    // get available data size
    size_t size = stream->available();

    if(size) {
        // read up to 128 byte
        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
        // write it to File
        file.write(buff, c);

        if(len > 0) {
            len -= c;
        }
    }
    delay(1);
  }
  file.close()
  return "";
}

void popQueue() {
  if (URL2 != NULL) {
    URL1 = URL2;
    URL1writefile = URL2writefile;
    URL1Callback = URL2Callback;
    if (URL3 != NULL) {
      URL2 = URL3;
      URL2writefile = URL3writefile;
      URL2Callback = URL3Callback;
      URL3 = NULL;
      URL3writefile = NULL;
      URL3Callback = NULL;
    } else {
      URL2 = NULL;
      URL2writefile = NULL;
      URL2Callback = NULL;
    }
  } else {
    URL1 = NULL;
    URL1writefile = NULL;
    URL1Callback = NULL;
  }
}

void processURL() {
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client -> setCACert(rootCACertificate);
    {
      // Add a scoping block for HTTPClient http to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient http;
  
      Serial.print("[HTTP] begin...\n");
      if (http.begin(*client, URL1)) {  // HTTP
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
            if (URL1writefile == NULL) {
              URL1Callback(http.getString().c_str());
            } else {
              URL1Callback(writeFromStream(http.getStreamPtr())) // pass stream
            }
          }
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
  // move URL queue up
  popQueue();
}
