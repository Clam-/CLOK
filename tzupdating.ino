#include <ESP32-targz.h>


void checkAndDoUpdate() {
  // check contents of web version to stored version data.
  if (!compareFileContents(preferences.getString("tzver").c_str(), "/version")){
    // do update

    // get https stream

    // pass stream to unpackdata...

  }
}

