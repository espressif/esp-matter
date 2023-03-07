/*
 * wpa_supplicant/hostapd / Internal implementation of OS specific functions
 * Copyright (c) 2005-2006, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 *
 * This file is an example of operating system specific  wrapper functions.
 * This version implements many of the functions internally, so it can be used
 * to fill in missing functions from the target system C libraries.
 *
 * Some of the functions are using standard C library calls in order to keep
 * this file in working condition to allow the functions to be tested on a
 * Linux target. Please note that OS_NO_C_LIB_DEFINES needs to be defined for
 * this file to work correctly. Note that these implementations are only
 * examples and are not optimized for speed.
 */

#include "os.h"
#include <stdlib.h>
/* #include <time.h> */
/* #include <unistd.h> */
/* #include <sys/time.h> */
/* #include "utils/common.h" */
#include <FreeRTOS.h>
#include <task.h>
#include <bl_sec.h>

int os_get_time(struct os_time *t)
{
#if 0
    struct timeval tv;
    int ret = gettimeofday(&tv, NULL);
    t->sec = (os_time_t) tv.tv_sec;
    t->usec = tv.tv_usec;
    return ret;
#endif
    return -1;
}

unsigned long os_random(void)
{
    return bl_rand();
}

int os_get_random(unsigned char *buf, size_t len)
{
    bl_rand_stream(buf, len);
    return 0;
}

void os_sleep(os_time_t sec, os_time_t usec)
{
    size_t time_ms = sec * 1000 + usec / 1000;
    time_ms = time_ms ? time_ms : 1;
    vTaskDelay(pdMS_TO_TICKS(time_ms));
}

void *wpa_supplicant_malloc(size_t size)
{
    return pvPortMalloc(size);
}

void *wpa_supplicant_realloc(void *ptr, size_t size)
{
    return pvPortRealloc(ptr, size);
}

void *wpa_supplicant_zalloc(size_t nmemb, size_t size)
{
    return pvPortCalloc(nmemb, size);
}

void wpa_supplicant_free(void *ptr)
{
    vPortFree(ptr);
}

void wpa_supplicant_bzero(void *s, size_t n)
{
    memset(s, 0, n);
}
