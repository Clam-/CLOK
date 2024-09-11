#include <HTTPClient.h>

void webSetup();

void webUpdateCA();

HTTPClient& webGetClient();

bool getURL(const char* url, const char* writefile, void (*func)(String&, String&), String &eTag);
