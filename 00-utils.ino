bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

// shinanigans: https://sourceware.org/legacy-ml/newlib/2018/msg00309.html
void CLOKconfigTzTime(char* tz_env, const char* tz_str, const char* server1, const char* server2)
{
    //tcpip_adapter_init();  // Should not hurt anything if already inited
    esp_netif_init();
    if(sntp_enabled()){
        sntp_stop();
    }
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, (char*)server1);
    sntp_setservername(1, (char*)server2);
    sntp_init();
    strcpy(tz_env, (char*)tz_str);
    putenv(tz_env);
    tzset();
}

void CLOK_chtz(char* tz_env, const char* tz_str) {
    strcpy(tz_env, tz_str);
    tzset();
}