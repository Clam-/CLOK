//Preferences
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_partition.h"
#include <Preferences.h>
Preferences preferences;

void prefsSetup() {
  preferences.begin("clok", false);
}