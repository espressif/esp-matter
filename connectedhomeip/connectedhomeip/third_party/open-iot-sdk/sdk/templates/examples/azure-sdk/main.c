/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

/* This example shows device-to-cloud messaging with Azure.
 */

#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/xlogging.h"
#include "cmsis_os2.h"
#include "hal/serial_api.h"
#include "iothub.h"
#include "iothub_client_options.h"
#include "iothub_config.h"
#include "iothub_device_client_ll.h"
#include "iothub_message.h"
#include "iothubtransportmqtt.h"
#include "mbedtls/platform.h"
#include "mbedtls/threading.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdlib.h>

#define PW_LOG_MODULE_NAME "main"
#include "pw_log/log.h"

/* IoT Hub message settings */
#define MESSAGE_NUMBER 1
#define MESSAGE_STRING "Azure SDK Test Message"

typedef enum {
    APP_EVENT_IOT_HUB_CONNECTION_UP,
    APP_EVENT_IOT_HUB_CONNECTION_DOWN,
    APP_EVENT_SEND_MSG_OK,
    APP_EVENT_SEND_MSG_ERROR,
    APP_EVENT_NONE
} app_event_t;

typedef struct {
    app_event_t event;
    int32_t return_code;
} app_msg_t;

static osMessageQueueId_t app_msg_queue = NULL;

/** This example use NV seed as entropy generator.
 * Set dummy data as NV seed.
 */
static int mbedtls_platform_example_nv_seed_read(unsigned char *buf, size_t buf_len)
{
    if (buf == NULL) {
        return (-1);
    }
    memset(buf, 0xA5, buf_len);
    return 0;
}

static int mbedtls_platform_example_nv_seed_write(unsigned char *buf, size_t buf_len)
{
    (void)buf;
    (void)buf_len;
    return 0;
}

extern mdh_serial_t *get_example_serial();

#ifdef IOTSDK_AZURE_SDK_LOGGING
static void iothub_logging(
    LOG_CATEGORY log_category, const char *file, const char *func, int line, unsigned int options, const char *fmt, ...)
{
    const char *p, *basename;

    /* Extract basename from file */
    for (p = basename = file; *p != '\0'; p++) {
        if (*p == '/' || *p == '\\')
            basename = p + 1;
    }

    // determine required buffer size
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (len < 0)
        return;

    // format message
    char msg[len + 1];
    va_start(args, fmt);
    vsnprintf(msg, len + 1, fmt, args);
    va_end(args);

    switch (log_category) {
        case AZ_LOG_ERROR:
            PW_LOG_ERROR("%s:%d: %s", basename, line, msg);
            break;
        case AZ_LOG_INFO:
            PW_LOG_INFO("%s:%d: %s", basename, line, msg);
            break;
        case AZ_LOG_TRACE:
            PW_LOG_DEBUG("%s:%d: %s", basename, line, msg);
            break;
        default:
            break;
    }
}
#endif // IOTSDK_AZURE_SDK_LOGGING

/** This callback is called by the Azure IoT Hub client.
 * It handles IoT Hub connection status.
 */
static void connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS result,
                                       IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason,
                                       void *ignored)
{
    ((void)ignored);

    const app_msg_t msg = {.event = (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED)
                                        ? APP_EVENT_IOT_HUB_CONNECTION_UP
                                        : APP_EVENT_IOT_HUB_CONNECTION_DOWN,
                           .return_code = (result != IOTHUB_CLIENT_CONNECTION_AUTHENTICATED) ? (int32_t)reason : 0};
    if (osMessageQueuePut(app_msg_queue, &msg, 0, 0) != osOK) {
        PW_LOG_ERROR("Failed to send message to app_msg_queue");
    }
}

/** This callback is called by the Azure IoT Hub client.
 * It handles confirmation of sending messages to IoT Hub.
 */
static void send_confirm_callback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *ignored)
{
    ((void)ignored);

    const app_msg_t msg = {.event = (result == IOTHUB_CLIENT_CONFIRMATION_OK) ? APP_EVENT_SEND_MSG_OK
                                                                              : APP_EVENT_SEND_MSG_ERROR,
                           .return_code = (result != IOTHUB_CLIENT_CONFIRMATION_OK) ? (int32_t)result : 0};
    if (osMessageQueuePut(app_msg_queue, &msg, 0, 0) != osOK) {
        PW_LOG_ERROR("Failed to send message to app_msg_queue");
    }
}

static void app_task(void *arg)
{
    (void)arg;

    int res = 0;
    IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle = NULL;
    IOTHUB_MESSAGE_HANDLE message_handle = NULL;
    app_msg_t msg;
    uint32_t msg_counter = 0;

    PW_LOG_INFO("Initialising IoT Hub");

#ifdef IOTSDK_AZURE_SDK_LOGGING
    xlogging_set_log_function(iothub_logging);
#endif // IOTSDK_AZURE_SDK_LOGGING

    res = IoTHub_Init();
    if (res) {
        PW_LOG_ERROR("IoTHub_Init failed: %d", res);
        goto exit;
    }

    PW_LOG_INFO("IoT Hub initialization success");

    PW_LOG_INFO("IoT hub connection setup");
    iotHubClientHandle = IoTHubDeviceClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);
    if (!iotHubClientHandle) {
        PW_LOG_ERROR("IoTHubDeviceClient_LL_CreateFromConnectionString failed");
        goto exit;
    }

    IoTHubDeviceClient_LL_SetOption(iotHubClientHandle, OPTION_TRUSTED_CERT, certificates);

    bool traceOn = true;
    IoTHubDeviceClient_LL_SetOption(iotHubClientHandle, OPTION_LOG_TRACE, &traceOn);

    bool urlEncodeOn = true;
    IoTHubDeviceClient_LL_SetOption(iotHubClientHandle, OPTION_AUTO_URL_ENCODE_DECODE, &urlEncodeOn);

    IoTHubDeviceClient_LL_SetConnectionStatusCallback(iotHubClientHandle, connection_status_callback, NULL);

    PW_LOG_INFO("IoT Hub connecting...");

    // The function IoTHubDeviceClient_LL_DoWork must be executed every 100ms to work properly.
    // Compute the time we are allowed to wait for a message
    uint32_t msg_timeout_ticks = (100 * osKernelGetTickFreq()) / 1000U;
    msg_timeout_ticks += ((100 * osKernelGetTickFreq()) % 1000U) ? 1 : 0;

    while (1) {
        if (osMessageQueueGet(app_msg_queue, &msg, NULL, msg_timeout_ticks) != osOK) {
            msg.event = APP_EVENT_NONE;
        }

        switch (msg.event) {
            case APP_EVENT_IOT_HUB_CONNECTION_UP:
                PW_LOG_INFO("IoT Hub connection success");
                message_handle = IoTHubMessage_CreateFromString(MESSAGE_STRING);
                if (!message_handle) {
                    PW_LOG_ERROR("IoTHubMessage_CreateFromString failed");
                    res = EXIT_FAILURE;
                    break;
                }
                PW_LOG_INFO("IoT Hub message created");
                PW_LOG_INFO("Start message sending...");
                IoTHubDeviceClient_LL_SendEventAsync(iotHubClientHandle, message_handle, send_confirm_callback, NULL);
                break;
            case APP_EVENT_SEND_MSG_OK:
                msg_counter++;
                PW_LOG_INFO("Send message %d of %d", msg_counter, MESSAGE_NUMBER);
                if (msg_counter < MESSAGE_NUMBER) {
                    IoTHubDeviceClient_LL_SendEventAsync(
                        iotHubClientHandle, message_handle, send_confirm_callback, NULL);
                }
                break;
            case APP_EVENT_IOT_HUB_CONNECTION_DOWN:
                PW_LOG_ERROR("IoT Hub connection failed %d; reconnecting", msg.return_code);
                // Connection is down, the middleware will attempt to reconnect
                break;
            case APP_EVENT_SEND_MSG_ERROR:
                PW_LOG_ERROR("Send message failed %d", msg.return_code);
                res = EXIT_FAILURE;
                break;
            default:
                break;
        }

        IoTHubDeviceClient_LL_DoWork(iotHubClientHandle);

        if (res || msg_counter == MESSAGE_NUMBER) {
            if (message_handle) {
                IoTHubMessage_Destroy(message_handle);
                message_handle = NULL;
            }
            break;
        }
    }

exit:
    PW_LOG_INFO("Demo stopped");

    if (iotHubClientHandle) {
        IoTHubDeviceClient_LL_Destroy(iotHubClientHandle);
    }
    IoTHub_Deinit();
    while (1)
        ;
}

int main(void)
{
    int res = 0;

    pw_log_mdh_init(get_example_serial());

    mbedtls_threading_set_cmsis_rtos();
    mbedtls_platform_set_nv_seed(mbedtls_platform_example_nv_seed_read, mbedtls_platform_example_nv_seed_write);

    PW_LOG_INFO("Initialising kernel");
    res = osKernelInitialize();
    if (res != osOK) {
        PW_LOG_ERROR("osKernelInitialize failed: %d", res);
        return EXIT_FAILURE;
    }

    app_msg_queue = osMessageQueueNew(5, sizeof(app_msg_t), NULL);
    if (!app_msg_queue) {
        PW_LOG_ERROR("Failed to create a app msg queue");
        return EXIT_FAILURE;
    }

    osThreadId_t demo_thread = osThreadNew(app_task, NULL, NULL);
    if (!demo_thread) {
        PW_LOG_ERROR("Failed to create thread\r\n");
        return EXIT_FAILURE;
    }

    osKernelState_t state = osKernelGetState();
    if (state != osKernelReady) {
        PW_LOG_ERROR("Kernel not ready");
        return EXIT_FAILURE;
    }

    PW_LOG_INFO("Starting kernel");
    res = osKernelStart();
    if (res != osOK) {
        PW_LOG_ERROR("osKernelStart failed: %d", res);
        return EXIT_FAILURE;
    }

    return 0;
}
