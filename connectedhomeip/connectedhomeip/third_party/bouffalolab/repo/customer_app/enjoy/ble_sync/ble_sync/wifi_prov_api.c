#include "wifi_prov_api.h"

#include <string.h>
#include <stdio.h>
#include <FreeRTOS.h>

#include <vfs.h>
#include <aos/kernel.h>
#include <aos/yloop.h>
#include <event_device.h>

int wifi_prov_api_event_trigger_connect(struct wifi_conn *info)
{
    if (aos_post_event(EV_WIFI, CODE_WIFI_ON_PROV_CONNECT, (unsigned long)info) >= 0) {
        puts("[APP] [PROV] trigger CONNECT event OK\r\n");
    } else {
        puts("[APP] [PROV] trigger CONNECT event failed\r\n");
        return -1;
    }

    return 0;
}

int wifi_prov_api_event_trigger_disconnect(void)
{
    if (aos_post_event(EV_WIFI, CODE_WIFI_ON_PROV_DISCONNECT, 0) >= 0) {
        puts("[APP] [PROV] trigger DISCONNECT event OK\r\n");
    } else {
        puts("[APP] [PROV] trigger DISCONNECT event failed\r\n");
        return -1;
    }

    return 0;
}

int wifi_prov_api_event_trigger_scan(void(*complete)(void *))
{
    if (aos_post_event(EV_WIFI, CODE_WIFI_ON_PROV_SCAN_START, (unsigned long)complete) >= 0) {
        puts("[APP] [PROV] trigger scan event OK\r\n");
    } else {
        puts("[APP] [PROV] trigger scan event failed\r\n");
        return -1;
    }

    return 0;
}

int wifi_prov_api_event_state_get(void(*state_get)(void *))
{
    if (aos_post_event(EV_WIFI, CODE_WIFI_ON_PROV_STATE_GET, (unsigned long)state_get) >= 0) {
        puts("[APP] [PROV] trigger scan event OK\r\n");
    } else {
        puts("[APP] [PROV] trigger scan event failed\r\n");
        return -1;
    }

    return 0;
}
