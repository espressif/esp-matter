/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "iot_ntp_client.h"

#include "azure_c_shared_utility/xlogging.h"
#include "cmsis_os2.h"
#include "iot_socket.h"
#include "iotsdk/ip_network_api.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NTP_CLIENT_START_FLAG      0x00000001U
#define NTP_CLIENT_STOP_FLAG       0x00000002U
#define NTP_CLIENT_ERROR_FLAG      0x00000004U
#define NTP_CLIENT_TERMINATED_FLAG 0x00000008U
#define NTP_CLIENT_ALL_FLAGS \
    (NTP_CLIENT_START_FLAG | NTP_CLIENT_STOP_FLAG | NTP_CLIENT_ERROR_FLAG | NTP_CLIENT_TERMINATED_FLAG)

#define NTP_CLIENT_ACTION_TIMEOUT 1000
#define NTP_CLIENT_RECV_TIMEOUT   3000

/** NTP client event type */
typedef enum {
    NTP_EVENT_NONE = 0,  /** None event */
    NTP_EVENT_START,     /** Starting NTP client */
    NTP_EVENT_GET_TIME,  /** Get time from NTP server */
    NTP_EVENT_STOP,      /** Stoping NTP client */
    NTP_EVENT_TERMINATE, /** Terminate NTP client */
    NTP_EVENT_ERROR,     /** NTP client error occurs */
} ntp_event_t;

/** Unix time timestamp value */
static const time_t TIME1970 = (time_t)2208988800UL;

/** NTP client context */
static iotNtpClientContext_t ntp_context = {.state = IOT_NTP_CLIENT_UNINITIALISED};

/** This callback is called by NTP client timer.
 * It handles timer event and trigger getting time from NTP server.
 */
static void ntp_timer_callback(void *arg)
{
    ((void)arg);
    ntp_event_t event = NTP_EVENT_GET_TIME;
    osStatus_t status = osMessageQueuePut(ntp_context.event_queue, &event, 0, 0);
    if (status != osOK) {
        LogError("Failed to put event in NTP queue");
    }
}

/** Function converts the unsigned integer netlong from network byte order
 *  to host byte order
 */
static uint32_t ntohl(uint32_t x)
{
    uint32_t ret = (x & 0xff) << 24;
    ret |= (x & 0xff00) << 8;
    ret |= (x & 0xff0000UL) >> 8;
    ret |= (x & 0xff000000UL) >> 24;
    return ret;
}

/** Wait for specific NTP event and check error */
static iotNtpClientResult_t wait_for_ntp_event(uint32_t event)
{
    iotNtpClientResult_t res = IOT_NTP_CLIENT_OK;
    int ret =
        osEventFlagsWait(ntp_context.event_flags, NTP_CLIENT_ALL_FLAGS, osFlagsWaitAny, NTP_CLIENT_ACTION_TIMEOUT);
    if (ret < 0) {
        LogError("osEventFlagsWait failed %d", ret);
        return IOT_NTP_CLIENT_ERR_EVENT_FLAG;
    }

    if (!(ret & event)) {
        if (ret & NTP_CLIENT_ERROR_FLAG) {
            osStatus_t status = osMutexAcquire(ntp_context.mutex, NTP_CLIENT_ACTION_TIMEOUT);
            if (status != osOK) {
                LogError("osMutexAcquire failed %d", status);
                return IOT_NTP_CLIENT_ERR_MUTEX;
            }
            res = ntp_context.error;
            status = osMutexRelease(ntp_context.mutex);
            if (status != osOK) {
                LogError("osMutexRelease failed %d", status);
                return IOT_NTP_CLIENT_ERR_MUTEX;
            }
        } else {
            res = IOT_NTP_CLIENT_ERR_INTERNAL;
        }
    }

    ret = osEventFlagsClear(ntp_context.event_flags, NTP_CLIENT_ALL_FLAGS);
    if (ret < 0) {
        LogError("osEventFlagsClear failed %d", ret);
        return IOT_NTP_CLIENT_ERR_EVENT_FLAG;
    }

    return res;
}

/** NTP client task to  handle start, stop and get time actions. */
static void ntp_task(void *arg)
{
    ((void)arg);

    int res = 0;
    ntp_event_t event;
    int32_t socket = IOT_SOCKET_ESOCK;
    int buf[12];
    iotNtpClientResult_t error = IOT_NTP_CLIENT_OK;
    osStatus_t status;

    /** Initlise NTP client timer */
    osTimerId_t ntp_timer = osTimerNew(ntp_timer_callback, osTimerPeriodic, NULL, NULL);
    if (!ntp_timer) {
        LogError("Create NTP timer failed");
        return;
    }

    while (1) {
        if (osMessageQueueGet(ntp_context.event_queue, &event, NULL, osWaitForever) != osOK) {
            LogError("Get NTP event from queue failed");
            error = IOT_NTP_CLIENT_ERR_EVENT_QUEUE;
            event = NTP_EVENT_NONE;
        }

        switch (event) {
            case NTP_EVENT_START: {
                socket = iotSocketCreate(IOT_SOCKET_AF_INET, IOT_SOCKET_SOCK_DGRAM, IOT_SOCKET_IPPROTO_UDP);
                if (socket < 0) {
                    LogError("iotSocketCreate failed %d", socket);
                    error = IOT_NTP_CLIENT_ERR_NETWORK;
                    break;
                }
                // set a timeout to the socket in case the UDP message is not received
                unsigned int timeout = NTP_CLIENT_RECV_TIMEOUT;
                res = iotSocketSetOpt(socket, IOT_SOCKET_SO_RCVTIMEO, &timeout, sizeof(timeout));
                if (res) {
                    LogError("Set recv timeout failed %d", res);
                    error = IOT_NTP_CLIENT_ERR_NETWORK;
                    break;
                }
                iotNtpClientConfig_t ntp_config;
                status = osMutexAcquire(ntp_context.mutex, NTP_CLIENT_ACTION_TIMEOUT);
                if (status != osOK) {
                    LogError("osMutexAcquire failed %d", status);
                    error = IOT_NTP_CLIENT_ERR_MUTEX;
                    break;
                }
                memcpy(&ntp_config, &ntp_context.config, sizeof(ntp_config));
                status = osMutexRelease(ntp_context.mutex);
                if (status != osOK) {
                    LogError("osMutexRelease failed %d", status);
                    error = IOT_NTP_CLIENT_ERR_MUTEX;
                    break;
                }
                uint8_t ip[4];
                uint32_t ip_len = sizeof(ip);
                res = iotSocketGetHostByName(ntp_config.hostname, IOT_SOCKET_AF_INET, ip, &ip_len);
                if (res) {
                    LogError("iotSocketGetHostByName failed %d", res);
                    error = IOT_NTP_CLIENT_ERR_NETWORK;
                    break;
                }
                res = iotSocketConnect(socket, (const uint8_t *)&ip, ip_len, ntp_config.port);
                if (res) {
                    LogError("iotSocketConnect failed %d", res);
                    error = IOT_NTP_CLIENT_ERR_NETWORK;
                    break;
                }
                uint32_t ticks_interval = ((uint64_t)ntp_config.interval_ms * (uint64_t)osKernelGetTickFreq()) / 1000;
                res = osTimerStart(ntp_timer, ticks_interval);
                if (res) {
                    LogError("osTimerStart failed %d", res);
                    error = IOT_NTP_CLIENT_ERR_TIMER;
                    break;
                }

                res = osEventFlagsSet(ntp_context.event_flags, NTP_CLIENT_START_FLAG);
                if (res < 0) {
                    LogError("osEventFlagsSet failed %d", res);
                    error = IOT_NTP_CLIENT_ERR_EVENT_FLAG;
                    break;
                }
            }
            case NTP_EVENT_GET_TIME:
                memset(buf, 0, sizeof(buf));
                buf[0] = '\x1b';
                res = iotSocketSend(socket, buf, sizeof(buf));
                if (res < 0) {
                    // In case of timeout, restart a request immediately.
                    if (res == IOT_SOCKET_EAGAIN || res == IOT_SOCKET_ETIMEDOUT) {
                        ntp_timer_callback(NULL);
                    } else {
                        LogError("iotSocketSend failed %d", res);
                        error = IOT_NTP_CLIENT_ERR_NETWORK;
                    }
                    break;
                }

                res = iotSocketRecv(socket, buf, sizeof(buf));
                if (res < 0) {
                    // In some case, the server can timeout.
                    // Restart a request immediately.
                    if (res == IOT_SOCKET_EAGAIN || res == IOT_SOCKET_ETIMEDOUT) {
                        ntp_timer_callback(NULL);
                    } else {
                        LogError("iotSocketRecv failed %d", res);
                        error = IOT_NTP_CLIENT_ERR_NETWORK;
                    }
                    break;
                } else if (res > 10) {
                    time_t current_time = ntohl(buf[10]) - TIME1970;

                    LogInfo("NTP current time: %d", current_time);

                    status = osMutexAcquire(ntp_context.mutex, NTP_CLIENT_ACTION_TIMEOUT);
                    if (status != osOK) {
                        LogError("osMutexAcquire failed %d", status);
                        error = IOT_NTP_CLIENT_ERR_MUTEX;
                        break;
                    }
                    ntp_context.time = current_time;
                    iotNtpClientListener_t *listener = &ntp_context.listeners;
                    while (listener != NULL && listener->cb != NULL) {
                        listener->cb(ntp_context.time, listener->ctx);
                        listener = listener->next;
                    }
                    status = osMutexRelease(ntp_context.mutex);
                    if (status != osOK) {
                        LogError("osMutexRelease failed %d", status);
                        error = IOT_NTP_CLIENT_ERR_MUTEX;
                        break;
                    }
                }
                break;
            case NTP_EVENT_STOP:
                res = osTimerStop(ntp_timer);
                if (res) {
                    LogError("osTimerStop failed %d", res);
                    error = IOT_NTP_CLIENT_ERR_TIMER;
                    break;
                }
                res = iotSocketClose(socket);
                if (res) {
                    LogError("iotSocketClose failed %d", res);
                    error = IOT_NTP_CLIENT_ERR_NETWORK;
                    break;
                }
                socket = IOT_SOCKET_ESOCK;

                res = osEventFlagsSet(ntp_context.event_flags, NTP_CLIENT_STOP_FLAG);
                if (res < 0) {
                    LogError("osEventFlagsSet failed %d", res);
                    error = IOT_NTP_CLIENT_ERR_EVENT_FLAG;
                    break;
                }

                break;
            case NTP_EVENT_TERMINATE:
            default:
                break;
        }

        if (error != IOT_NTP_CLIENT_OK) {
            status = osMutexAcquire(ntp_context.mutex, NTP_CLIENT_ACTION_TIMEOUT);
            if (status != osOK) {
                LogError("osMutexAcquire failed %d", status);
                break;
            }
            ntp_context.error = error;
            status = osMutexRelease(ntp_context.mutex);
            if (status != osOK) {
                LogError("osMutexRelease failed %d", status);
                break;
            }
            res = osEventFlagsSet(ntp_context.event_flags, NTP_CLIENT_ERROR_FLAG);
            if (res < 0) {
                LogError("osEventFlagsSet failed %d", res);
                break;
            }
        }

        if (event == NTP_EVENT_TERMINATE) {
            break;
        }
    }

    if (socket != IOT_SOCKET_ESOCK) {
        res = iotSocketClose(socket);
        if (res) {
            LogError("iotSocketClose failed %d", res);
        }
    }

    if (ntp_timer) {
        res = osTimerDelete(ntp_timer);
        if (res != osOK) {
            LogError("Delete NTP fimer failed %d", res);
        }
    }

    res = osEventFlagsSet(ntp_context.event_flags, NTP_CLIENT_TERMINATED_FLAG);
    if (res < 0) {
        LogError("osEventFlagsSet failed %d", res);
    }
    LogInfo("NTP thread terminated");

    osThreadTerminate(osThreadGetId());
}

iotNtpClientResult_t iotNtpClientInit(const iotNtpClientConfig_t *config)
{
    if (ntp_context.state == IOT_NTP_CLIENT_UNINITIALISED) {
        memset(&ntp_context, 0, sizeof(iotNtpClientContext_t));

        if (config != NULL) {
            memcpy(&ntp_context.config, config, sizeof(iotNtpClientConfig_t));
        }

        if (!ntp_context.config.hostname) {
            LogInfo("Time server hostname not set, use the default");
            ntp_context.config.hostname = NTP_DEFAULT_NIST_SERVER_HOSTNAME;
        }

        if (!ntp_context.config.port) {
            LogInfo("Time server port not set, use the default");
            ntp_context.config.port = NTP_DEFAULT_NIST_SERVER_PORT;
        }

        if (!ntp_context.config.interval_ms) {
            LogInfo("Update interval not set, use the default");
            ntp_context.config.interval_ms = NTP_DEFAULT_INTERVAL_MS;
        }

        ntp_context.mutex = osMutexNew(NULL);
        if (!ntp_context.mutex) {
            LogError("Create NTP mutex failed");
            iotNtpClientDeinit();
            return IOT_NTP_CLIENT_ERR_MUTEX;
        }

        ntp_context.event_queue = osMessageQueueNew(5, sizeof(ntp_event_t), NULL);
        if (!ntp_context.event_queue) {
            LogError("Create NTP event queue failed");
            iotNtpClientDeinit();
            return IOT_NTP_CLIENT_ERR_EVENT_QUEUE;
        }

        ntp_context.event_flags = osEventFlagsNew(NULL);
        if (!ntp_context.event_flags) {
            LogError("Create NTP event flags failed");
            iotNtpClientDeinit();
            return IOT_NTP_CLIENT_ERR_EVENT_FLAG;
        }

        ntp_context.thread = osThreadNew(ntp_task, NULL, NULL);
        if (!ntp_context.thread) {
            LogError("Create NTP thread failed");
            iotNtpClientDeinit();
            return IOT_NTP_CLIENT_ERR_THREAD;
        }

        ntp_context.state = IOT_NTP_CLIENT_STOPPED;
    }

    return IOT_NTP_CLIENT_OK;
}

iotNtpClientResult_t iotNtpClientDeinit()
{
    int res;

    if (ntp_context.state == IOT_NTP_CLIENT_RUNNING) {
        res = iotNtpClientStop();
        if (res != IOT_NTP_CLIENT_OK) {
            LogError("Stop NTP client failed %d", res);
            return res;
        }
    }

    if (ntp_context.thread) {
        ntp_event_t event = NTP_EVENT_TERMINATE;
        res = osMessageQueuePut(ntp_context.event_queue, &event, 0, 0);
        if (res != osOK) {
            LogError("Failed to put event in NTP queue %d", res);
            return IOT_NTP_CLIENT_ERR_EVENT_QUEUE;
        }

        if ((res = wait_for_ntp_event(NTP_CLIENT_TERMINATED_FLAG)) != IOT_NTP_CLIENT_OK) {
            return res;
        }

        ntp_context.thread = NULL;
    }

    if (ntp_context.event_flags) {
        res = osEventFlagsDelete(ntp_context.event_flags);
        if (res != osOK) {
            LogError("Delete NTP event flags failed %d", res);
            return IOT_NTP_CLIENT_ERR_EVENT_FLAG;
        }
    }

    if (ntp_context.event_queue) {
        res = osMessageQueueDelete(ntp_context.event_queue);
        if (res != osOK) {
            LogError("Delete NTP event queue failed %d", res);
            return IOT_NTP_CLIENT_ERR_EVENT_QUEUE;
        }

        ntp_context.event_queue = NULL;
    }

    if (ntp_context.mutex) {
        res = osMutexDelete(ntp_context.mutex);
        if (res != osOK) {
            LogError("Delete NTP mutex failed %d", res);
            return IOT_NTP_CLIENT_ERR_MUTEX;
        }
    }

    memset(&ntp_context, 0, sizeof(iotNtpClientContext_t));
    ntp_context.state = IOT_NTP_CLIENT_UNINITIALISED;

    return IOT_NTP_CLIENT_OK;
}

iotNtpClientResult_t iotNtpClientStart(void)
{
    iotNtpClientResult_t res;

    if (ntp_context.state == IOT_NTP_CLIENT_UNINITIALISED) {
        LogError("NTP client not initialised");
        return IOT_NTP_CLIENT_ERR_WRONG_STATE;
    }

    if (ntp_context.state == IOT_NTP_CLIENT_STOPPED) {
        ntp_event_t event = NTP_EVENT_START;
        osStatus_t status = osMessageQueuePut(ntp_context.event_queue, &event, 0, 0);
        if (status != osOK) {
            LogError("Failed to put event in NTP queue");
            return IOT_NTP_CLIENT_ERR_EVENT_QUEUE;
        }
    }

    if ((res = wait_for_ntp_event(NTP_CLIENT_START_FLAG)) == IOT_NTP_CLIENT_OK) {
        ntp_context.state = IOT_NTP_CLIENT_RUNNING;
    }

    return res;
}

iotNtpClientResult_t iotNtpClientStop(void)
{
    iotNtpClientResult_t res;

    if (ntp_context.state == IOT_NTP_CLIENT_UNINITIALISED) {
        LogError("NTP client not initialised");
        return IOT_NTP_CLIENT_ERR_WRONG_STATE;
    }

    if (ntp_context.state == IOT_NTP_CLIENT_RUNNING) {
        ntp_event_t event = NTP_EVENT_STOP;
        osStatus_t status = osMessageQueuePut(ntp_context.event_queue, &event, 0, 0);
        if (status != osOK) {
            LogError("Failed to put event in NTP queue %d", status);
            return IOT_NTP_CLIENT_ERR_EVENT_QUEUE;
        }
    }

    if ((res = wait_for_ntp_event(NTP_CLIENT_STOP_FLAG)) == IOT_NTP_CLIENT_OK) {
        ntp_context.state = IOT_NTP_CLIENT_STOPPED;
    }

    return res;
}

iotNtpClientResult_t iotNtpClientAddListener(iotNtpCb_t callback, void *context)
{
    if (!callback) {
        LogError("Callback function empty");
        return IOT_NTP_CLIENT_ERR_INVALID_ARG;
    }

    if (ntp_context.state == IOT_NTP_CLIENT_UNINITIALISED) {
        LogError("NTP client not initialised");
        return IOT_NTP_CLIENT_ERR_WRONG_STATE;
    }

    osStatus_t status = osMutexAcquire(ntp_context.mutex, NTP_CLIENT_ACTION_TIMEOUT);
    if (status != osOK) {
        LogError("osMutexAcquire failed %d", status);
        return IOT_NTP_CLIENT_ERR_MUTEX;
    }

    iotNtpClientListener_t *listener = &ntp_context.listeners;
    while (listener->cb != NULL) {
        if (listener->next == NULL) {
            listener->next = (iotNtpClientListener_t *)malloc(sizeof(iotNtpClientListener_t));
            listener->next->next = NULL;
        }
        listener = listener->next;
    }

    listener->cb = callback;
    if (context) {
        listener->ctx = context;
    }

    status = osMutexRelease(ntp_context.mutex);
    if (status != osOK) {
        LogError("osMutexRelease failed %d", status);
        return IOT_NTP_CLIENT_ERR_MUTEX;
    }

    return IOT_NTP_CLIENT_OK;
}

iotNtpClientResult_t iotNtpClientClearListeners()
{
    if (ntp_context.state == IOT_NTP_CLIENT_UNINITIALISED) {
        LogError("NTP client not initialised");
        return IOT_NTP_CLIENT_ERR_WRONG_STATE;
    }

    osStatus_t status = osMutexAcquire(ntp_context.mutex, NTP_CLIENT_ACTION_TIMEOUT);
    if (status != osOK) {
        LogError("osMutexAcquire failed %d", status);
        return IOT_NTP_CLIENT_ERR_MUTEX;
    }

    iotNtpClientListener_t *listener = &ntp_context.listeners;
    iotNtpClientListener_t *next;

    if (listener->cb) {
        listener->cb = NULL;
    }

    if (listener->ctx) {
        listener->ctx = NULL;
    }

    while (listener->next != NULL) {
        next = listener->next;
        free(listener);
        listener = next;
    }

    status = osMutexRelease(ntp_context.mutex);
    if (status != osOK) {
        LogError("osMutexRelease failed %d", status);
        return IOT_NTP_CLIENT_ERR_MUTEX;
    }

    return IOT_NTP_CLIENT_OK;
}

time_t iotNtpClientGetTime(void)
{
    time_t time;

    if (ntp_context.state == IOT_NTP_CLIENT_UNINITIALISED) {
        LogError("NTP client not initialised");
        return IOT_NTP_CLIENT_ERR_WRONG_STATE;
    }

    osStatus_t status = osMutexAcquire(ntp_context.mutex, NTP_CLIENT_ACTION_TIMEOUT);
    if (status != osOK) {
        LogError("osMutexAcquire failed %d", status);
        return IOT_NTP_CLIENT_ERR_MUTEX;
    }
    time = ntp_context.time;
    status = osMutexRelease(ntp_context.mutex);
    if (status != osOK) {
        LogError("osMutexRelease failed %d", status);
        return IOT_NTP_CLIENT_ERR_MUTEX;
    }

    return time;
}

iotNtpClientState_t iotNtpClientGetState(void)
{
    return ntp_context.state;
}
