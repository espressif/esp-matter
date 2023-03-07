#include <string.h>
#include <stdio.h>
#include <FreeRTOS.h>

#include <vfs.h>
#include <aos/kernel.h>
#include <aos/yloop.h>
#include <event_device.h>

#include "wifi_prov_api.h"

/*limit ssid len NOT too aggressive*/
#define SSID_LEN_MAX        (128)
#define PASSWORD_LEN_MAX    (128)

int wifi_prov_api_event_trigger_connect(void)
{
    if (aos_post_event(EV_WIFI, CODE_WIFI_ON_PROV_CONNECT, 0) >= 0) {
        puts("[APP] [PROV] trigger CONNECT event OK\r\n");
    } else {
        puts("[APP] [PROV] trigger CONNECT event failed\r\n");
        return -1;
    }

    return 0;
}

int wifi_prov_api_event_trigger_disconnect(void)
{
    if (aos_post_event(EV_WIFI, CODE_WIFI_ON_PROV_CONNECT, 0) >= 0) {
        puts("[APP] [PROV] trigger DISCONNECT event OK\r\n");
    } else {
        puts("[APP] [PROV] trigger DISCONNECT event failed\r\n");
        return -1;
    }

    return 0;
}

int wifi_prov_api_event_trigger_ssid(const char *ssid, int len)
{
    int ret;
    int ssid_len;
    char *ssid_dup;

    if (len) {
        ssid_len = len;
    } else {
        ssid_len = strlen(ssid);
    }

    if (ssid_len > SSID_LEN_MAX) {
        return -1;
    }
    ssid_dup = pvPortMalloc(ssid_len + 1);
    if (NULL == ssid_dup) {
        return -1;
    }
    strncpy(ssid_dup, ssid, ssid_len);
    ssid_dup[ssid_len] = '\0';

    puts("[APP] [PROV] sending ssid trigger ");
    puts(ssid_dup);
    puts("\r\n");

    if ((ret = aos_post_event(EV_WIFI, CODE_WIFI_ON_PROV_SSID, (unsigned long)ssid_dup)) >= 0) {
        puts("[APP] [PROV] trigger SSID event OK\r\n");
    } else {
        printf("[APP] [PROV] trigger SSID event failed, ret %d\r\n", ret);
        vPortFree(ssid_dup);
    }

    return 0;
}

int wifi_prov_api_event_trigger_password(const char *password, int len)
{
    int password_len;
    char *password_dup;

    if (len) {
        password_len = len;
    } else {
        password_len = strlen(password);
    }

    if (password_len > PASSWORD_LEN_MAX) {
        return -1;
    }
    password_dup = pvPortMalloc(password_len + 1);
    if (NULL == password_dup) {
        return -1;
    }
    strncpy(password_dup, password, password_len);
    password_dup[password_len] = '\0';

    puts("[APP] [PROV] sending password trigger ");
    puts(password_dup);
    puts("\r\n");

    if (aos_post_event(EV_WIFI, CODE_WIFI_ON_PROV_PASSWD, (unsigned long)password_dup) >= 0) {
        puts("[APP] [PROV] trigger PASSWD event OK\r\n");
    } else {
        puts("[APP] [PROV] trigger PASSWD event failed\r\n");
        vPortFree(password_dup);
    }

    return 0;
}
