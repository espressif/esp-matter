/*
 * Copyright (c) 2020 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
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
