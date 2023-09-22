#include <ESP32-targz.h>


void checkAndDoUpdate() {
  // check contents of web version to stored version data.
  if (!compareFileContents(preferences.getString("tzver").c_str(), "/version")){
    // do update

    // get https stream

    // pass stream to unpackdata...

  }
}

unsigned long TZ_NEXT_CHECK_TIME = 0;

void tzTick(unsigned long curtime) {
  if (curTime > TZ_NEXT_CHECK_TIME) {
    
    TZ_NEXT_CHECK_TIME + 21600000; // (6*60*60*1000) 6 hour checks.
  }
}
