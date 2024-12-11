#include "esp_netif.h"
//#include "lwip/apps/esp_sntp.h"
#include "esp32-hal.h"

bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

// shinanigans: https://sourceware.org/legacy-ml/newlib/2018/msg00309.html
void CLOKconfigTzTime(char* tz_env, const char* tz_str, const char* server1, const char* server2, bool firsttime=false)
{
    esp_netif_init();// Should not hurt anything if already inited
    if(esp_sntp_enabled()){
        esp_sntp_stop();
    }
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, (char*)server1);
    sntp_setservername(1, (char*)server2);
    sntp_init();
    strcpy(tz_env, (char*)tz_str);
    if (firsttime) { putenv(tz_env); }
    tzset();
}

void CLOK_chtz(char* tz_env, const char* tz_str) {
    strcpy(tz_env, tz_str);
    tzset();
}