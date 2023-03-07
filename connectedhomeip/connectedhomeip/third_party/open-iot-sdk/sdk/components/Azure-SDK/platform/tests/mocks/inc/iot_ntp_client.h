/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IOTSDK_NTP_CLIENT_H
#define IOTSDK_NTP_CLIENT_H

#include "fff.h"

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    IOT_NTP_CLIENT_UNINITIALISED = 0,
    IOT_NTP_CLIENT_STOPPED,
    IOT_NTP_CLIENT_RUNNING,
} iotNtpClientState_t;

DECLARE_FAKE_VALUE_FUNC(time_t, iotNtpClientGetTime);
DECLARE_FAKE_VALUE_FUNC(iotNtpClientState_t, iotNtpClientGetState);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // IOTSDK_NTP_CLIENT_H
