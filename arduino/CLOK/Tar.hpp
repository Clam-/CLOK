#define DEST_FS_USES_LITTLEFS
#include <ESP32-targz.h>
#include <HTTPClient.h>

void tarSetup();

void unpackTZdata(HTTPClient &hclient);