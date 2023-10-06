// include root let's encrypt
// create some update method... to update once a year... (or two?)

// BLE options
BLEStringCharacteristic BLE_RootCA_URL("BAAD0007-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | BLEWrite, 256);

const char* RootCA_URL_default = "https://letsencrypt.org/certs/isrgrootx1.pem";
const char* RootCA_default = \
"-----BEGIN CERTIFICATE-----\
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\
-----END CERTIFICATE-----\n";

unsigned long ROOTCA_PREV_TIME = 1000000; // 1000 seconds start (16~mins)
unsigned long ROOTCA_CHECK_TIME = 1209600000; // (2*7*24*60*60*1000) 2weekly checks.
char* ROOTCA = NULL;
size_t ROOTCA_LEN = 0;

void rootCA_BLE_Setup() {
  // BLE setup
  clokService.addCharacteristic(BLE_RootCA_URL);
  BLE_RootCA_URL.setEventHandler(BLEWritten, rootCAURLwritten);
}

void rootCAsetup() {
  // check if certURL exists:
  String url = preferences.getString("ROOTCA-URL");
  // if doesn't exist, put the default one in
  if (url == "") { preferences.putString("ROOTCA-URL", RootCA_URL_default); }

  // check if cert exists in preferences
  size_t size = preferences.getUInt("ROOTCA-len", 0);
  // if doesn't exist, put the default one in, along with length
  if (size == 0) { setRootCA(RootCA_default, ""); }
  loadRootCA();
}

const char* loadRootCA() {
  size_t size = preferences.getUInt("ROOTCA-len", 0);
  if (size > ROOTCA_LEN) {
    if (ROOTCA != NULL) { delete[] ROOTCA; }
    ROOTCA_LEN = size;
    char* buffer = new char[ROOTCA_LEN];
    ROOTCA = buffer;
  }
  preferences.getString("ROOTCA", ROOTCA, size);
}

void setRootCA(const char* cert, const char* ETag) {
  // check and nuke all ROOTCA if not null
  if (ROOTCA != NULL) { delete[] ROOTCA; }
  size_t size = preferences.putString("ROOTCA-ETag", ETag);
  //preferences.putUInt("ROOTCA-ETag-len", size);
  size = preferences.putString("ROOTCA", cert);
  preferences.putUInt("ROOTCA-len", size);
  loadRootCA();
}

void processNewCA(String &body, String &etag) {
  setRootCA(body.c_str(), etag.c_str());
}

void rootCACheck(unsigned long &now) {
  if (now - ROOTCA_PREV_TIME > ROOTCA_CHECK_TIME) {
    String url = preferences.getString("ROOTCA-URL");
    String etag = preferences.getString("ROOTCA-ETag");
    if (getURL(url.c_str(), NULL, processNewCA, etag)) {
      ROOTCA_PREV_TIME = now;
    } 
  }
}

void rootCAURLwritten(BLEDevice central, BLECharacteristic characteristic) {
  preferences.putString("ROOTCA-URL", BLE_RootCA_URL.value());
}
