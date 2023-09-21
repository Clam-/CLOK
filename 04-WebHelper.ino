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

const char* URL1 = "";
void (*URLCallback1)(void);
const char* URL2 = "";
void (*URLCallback2)(void);
const char* URL3 = "";
void (*URLCallback3)(void);


const char* GETstring(const char* url, void (*func)(void)) {
  if (memcmp(URL1, "")) {
    URL1 = url;
    URLCallback1 = func;
  } else if (memcmp(URL2, "")) {
    URL2 = url;
    URLCallback2 = func;
  } else if (memcmp(URL2, "")) {
    URL2 = url;
    URLCallback2 = func;
  } else {
    Serial.write("Too busy to GET: %s\n", url);
  }
}

void webLoop() {
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client -> setCACert(rootCACertificate);

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https;
  
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, "https://jigsaw.w3.org/HTTP/connection.html")) {  // HTTPS
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
  
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.println(payload);
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }

      // End extra scoping block
    }
  
    delete client;
  } else {
    Serial.println("Unable to create client");
  }

  Serial.println();
  Serial.println("Waiting 10s before the next round...");
  delay(10000);
}
