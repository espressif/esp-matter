/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/tlsio_mbedtls.h"
#include "azure_c_shared_utility/xlogging.h"
#include "cmsis_os2.h"
#include "iot_socket.h"
#include "iotsdk/ip_network_api.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#ifdef IOTSDK_AZURE_SDK_NTP_TIME
#include "iot_ntp_client.h"
#endif // IOTSDK_AZURE_SDK_NTP_TIME

#define AZURE_NETWORK_CHANGE_FLAG 0x00000001U
#define AZURE_TIME_READY_FLAG     0x00000002U

#define AZURE_EVENT_TIMEOUT 15000

static osEventFlagsId_t event_flags_id = NULL;
static osMutexId_t network_state_lock = NULL;
static bool network_up = false;

/** Wait for specific platform event and check error */
static int wait_for_event(uint32_t event)
{
    int ret = osEventFlagsWait(event_flags_id, event, osFlagsWaitAny, AZURE_EVENT_TIMEOUT);
    if (ret < 0) {
        LogError("osEventFlagsWait failed %d", ret);
        return MU_FAILURE;
    }

    return 0;
}

/** This callback is called by the ip network task. It translates from a network event code
 * to Azure event and sends it.
 *
 * @param event network up or down event.
 */
static void network_state_callback(network_state_callback_event_t event)
{
    int res = osMutexAcquire(network_state_lock, osWaitForever);
    if (res != osOK) {
        LogError("osMutexAcquire failed %d", res);
        return;
    }

    bool latest_network_up = (event == NETWORK_UP);
    if (latest_network_up != network_up) {
        LogInfo("network state changed");
        network_up = latest_network_up;
        res = osEventFlagsSet(event_flags_id, AZURE_NETWORK_CHANGE_FLAG);
        if (res < 0) {
            LogError("osEventFlagsSet failed %d", res);
        }
    }

    res = osMutexRelease(network_state_lock);
    if (res != osOK) {
        LogError("osMutexRelease failed %d", res);
        return;
    }
}

static bool is_network_up()
{
    int result = false;

    int res = osMutexAcquire(network_state_lock, osWaitForever);
    if (res != osOK) {
        LogError("osMutexAcquire failed %d", res);
        goto exit;
    }

    result = network_up;

    res = osMutexRelease(network_state_lock);
    if (res != osOK) {
        LogError("osMutexRelease failed %d", res);
    }

exit:
    return result;
}

#ifdef IOTSDK_AZURE_SDK_NTP_TIME
/** This callback is called by the NTP client. It inform about time update.
 *
 * @param time current time value.
 * @param ctx callback context
 */
static void ntp_listener_callback(time_t time, void *ctx)
{
    int res = osEventFlagsSet(event_flags_id, AZURE_TIME_READY_FLAG);
    if (res < 0) {
        LogError("osEventFlagsSet failed %d", res);
    }
}
#endif // IOTSDK_AZURE_SDK_NTP_TIME

int platform_init(void)
{
    int res = 0;

    event_flags_id = osEventFlagsNew(NULL);
    if (event_flags_id == NULL) {
        LogError("Create event flags failed");
        return MU_FAILURE;
    }

    // The state of the network should be updated by the network task started below.
    network_up = false;
    network_state_lock = osMutexNew(NULL);
    if (network_state_lock == NULL) {
        LogError("Create mutex failed");
        return MU_FAILURE;
    }

#ifdef IOTSDK_AZURE_SDK_NTP_TIME
    res = iotNtpClientInit(NULL);
    if (res != IOT_NTP_CLIENT_OK) {
        LogError("iotNtpClientInit failed %d", res);
        goto cleanup;
    }
#endif // IOTSDK_AZURE_SDK_NTP_TIME

    res = start_network_task(network_state_callback, 0);
    if (res != osOK) {
        LogError("start_network_task failed %d", res);
        goto cleanup;
    }

    if (!is_network_up()) {
        res = wait_for_event(AZURE_NETWORK_CHANGE_FLAG);
        if (res != 0 || !is_network_up()) {
            LogError("Network not up");
            goto cleanup;
        }
    }

#ifdef IOTSDK_AZURE_SDK_NTP_TIME
    res = iotNtpClientAddListener(ntp_listener_callback, NULL);
    if (res != IOT_NTP_CLIENT_OK) {
        LogError("iotNtpClientAddListener failed %d", res);
        goto cleanup;
    }

    res = iotNtpClientStart();
    if (res != IOT_NTP_CLIENT_OK) {
        LogError("iotNtpClientStart failed %d", res);
        goto cleanup;
    }

    res = wait_for_event(AZURE_TIME_READY_FLAG);
    if (res != 0) {
        LogError("NTP time not ready");
        goto cleanup;
    }

    res = iotNtpClientClearListeners();
    if (res != IOT_NTP_CLIENT_OK) {
        LogError("iotNtpClientClearListeners failed %d", res);
        goto cleanup;
    }
#endif // IOTSDK_AZURE_SDK_NTP_TIME

    LogInfo("Platform initialization success");
    return res;

cleanup:
    platform_deinit();
    return res;
}

void platform_deinit(void)
{
    int res;

#ifdef IOTSDK_AZURE_SDK_NTP_TIME
    res = iotNtpClientDeinit();
    if (res != IOT_NTP_CLIENT_OK) {
        LogError("iotNtpClientDeinit failed %d", res);
    }
#endif // IOTSDK_AZURE_SDK_NTP_TIME

    res = osMutexDelete(network_state_lock);
    if (res != osOK) {
        LogError("Delete event flags failed %d", res);
    }

    res = osEventFlagsDelete(event_flags_id);
    if (res != osOK) {
        LogError("Delete event flags failed %d", res);
    }
}

const IO_INTERFACE_DESCRIPTION *platform_get_default_tlsio(void)
{
    return tlsio_mbedtls_get_interface_description();
}

STRING_HANDLE platform_get_platform_info(PLATFORM_INFO_OPTION options)
{
    ((void)options);

    // Expected format: "(<runtime name>; <operating system name>; <platform>)"
    return STRING_construct("(native; OpenIoTSDK; undefined)");
}
