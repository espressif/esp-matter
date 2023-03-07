/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iot_ntp_client.h"

#include <time.h>

/** time() function is required by mbedtls adapter and IoT Hub client
 * mbedtls adapter generate entropy with time seed - see tlsio_entropy_poll() in tlsio_mbedtls.c file
 * iothub_client uses time in many places e.g token generation
 */
time_t time(time_t *dst)
{
    time_t result = 0;

    // The time is only read if the ntp client is running otherwise, with traces
    // on, may issue a LogError which calls time() and this repeats until the
    // system runs out of stack.
    if (iotNtpClientGetState() == IOT_NTP_CLIENT_RUNNING) {
        result = iotNtpClientGetTime();
    }

    if (dst) {
        *dst = result;
    }

    return result;
}
