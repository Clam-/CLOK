#include <ESP32-targz.h>

const char* TZ_FETCH_URL = "http://192.168.25.250:9999/zoneinfo.tar"

void tzFetchCallback(const char* bodystr) {
  
}

void tzVersionCheckCallback(const char* bodystr) {
  // check contents of web version to stored version data.
  if (!compareFileContents(bodystr, "/version")){
    // queue update
    queueGET(TZ_VERSION_URL, "/tz.tar", &tzFetchCallback)
  }
}

unsigned long TZ_NEXT_CHECK_TIME = 0;

const char* TZ_VERSION_URL = "http://192.168.25.250:9999/version"

void tzTick(unsigned long curtime) {
  if (curTime > TZ_NEXT_CHECK_TIME) {

    // check version
    queueGET(TZ_VERSION_URL, NULL, &tzVersionCheckCallback)
    
    // if version mismatch, get new tar and unpack.

    // settz something ??

    TZ_NEXT_CHECK_TIME + 21600000; // (6*60*60*1000) 6 hour checks.
  }
}
