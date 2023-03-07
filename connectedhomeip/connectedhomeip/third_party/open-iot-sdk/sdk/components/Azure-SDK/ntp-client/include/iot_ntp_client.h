/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef IOTSDK_NTP_CLIENT_H
#define IOTSDK_NTP_CLIENT_H

#include "iot_socket.h"
#include "iotsdk/ip_network_api.h"

#include <time.h>

#define NTP_DEFAULT_NIST_SERVER_HOSTNAME "pool.ntp.org"
#define NTP_DEFAULT_NIST_SERVER_PORT     123
#define NTP_DEFAULT_INTERVAL_MS          30000 // 30s

/**
 * Result of a call to the NTP client API.
 */
typedef enum {
    IOT_NTP_CLIENT_OK = 0,          /**< No error. */
    IOT_NTP_CLIENT_ERR_WRONG_STATE, /**< Wrong NTP state. */
    IOT_NTP_CLIENT_ERR_INVALID_ARG, /**< Error due to invalid arguments. */
    IOT_NTP_CLIENT_ERR_THREAD,      /**< Error thread. */
    IOT_NTP_CLIENT_ERR_MUTEX,       /**< Error thread. */
    IOT_NTP_CLIENT_ERR_EVENT_QUEUE, /**< Error event queue. */
    IOT_NTP_CLIENT_ERR_EVENT_FLAG,  /**< Error event flag. */
    IOT_NTP_CLIENT_ERR_NETWORK,     /**< Error network. */
    IOT_NTP_CLIENT_ERR_TIMER,       /**< Error timer. */
    IOT_NTP_CLIENT_ERR_INTERNAL     /**< Internal error (e.g mutex initalizaiton failed) */
} iotNtpClientResult_t;

/**
 * Execution state of the client.
 */
typedef enum {
    IOT_NTP_CLIENT_UNINITIALISED = 0, /** The initial state and the state after DeInit. */
    IOT_NTP_CLIENT_STOPPED,           /** The NTP client was initialised but hasn't started. */
    IOT_NTP_CLIENT_RUNNING,           /** The NTP client is actively updating. */
} iotNtpClientState_t;

/** Callback called when NTP time is updated successfully. */
typedef void (*iotNtpCb_t)(time_t time, void *ctx);

/**
 * Listener list.
 */
typedef struct iotNtpClientListener_s {
    /** NTP callback function */
    iotNtpCb_t cb;
    /** Listener context */
    void *ctx;
    /** Next listener on the list */
    struct iotNtpClientListener_s *next;
} iotNtpClientListener_t;

/**
 * Optional configuration.
 */
typedef struct {
    /** Time server hostname */
    char *hostname;
    /** Time server port */
    uint32_t port;
    /** Update interval. The time is updated immediately when the client starts and again every time this interval
     * elapses.
     */
    uint32_t interval_ms;
} iotNtpClientConfig_t;

/**
 * NTP client context.
 */
typedef struct {
    /** NTP client configuration */
    iotNtpClientConfig_t config;
    /** NTP client listeners list */
    iotNtpClientListener_t listeners;
    /** NTP client current state */
    iotNtpClientState_t state;
    /** NTP client error code */
    iotNtpClientResult_t error;
    /** NTP client thread */
    osThreadId_t thread;
    /** NTP client mutex */
    osMutexId_t mutex;
    /** NTP client event queue */
    osMessageQueueId_t event_queue;
    /** NTP client event flags */
    osEventFlagsId_t event_flags;
    /** NTP client current time */
    time_t time;
} iotNtpClientContext_t;

/**
 * @brief Initialise the NTP client. May only be called when state is UNINITIALISED.
 * @param config May be NULL to use defaults. 0 values are also replaced by defaults.
 * @return iotNtpClientResult_t
 */
iotNtpClientResult_t iotNtpClientInit(const iotNtpClientConfig_t *config);

/**
 * @brief Stop the NTP client if run and free all held resources.
 * @return iotNtpClientResult_t
 */
iotNtpClientResult_t iotNtpClientDeinit(void);

/**
 * @brief Start the client. May not be called when state is UNINITIALISED. This should be called when the network is
 * successfully brought up.
 * @return iotNtpClientResult_t
 */
iotNtpClientResult_t iotNtpClientStart(void);

/**
 * @brief Stop the client. May not be called when state is UNINITIALISED.
 * @return iotNtpClientResult_t
 */
iotNtpClientResult_t iotNtpClientStop(void);

/**
 * @brief Add listener to the NTP client. May not be called when state is UNINITIALISED.
 * @param callback Callback function.
 * @param context Context pass to NTP client
 * @return iotNtpClientResult_t
 */
iotNtpClientResult_t iotNtpClientAddListener(iotNtpCb_t callback, void *context);

/**
 * @brief Clear NTP client listeners list.
 * @return iotNtpClientResult_t
 */
iotNtpClientResult_t iotNtpClientClearListeners(void);

/**
 * @brief Get the current status of the client.
 * @return iotNtpClientState_t
 */
iotNtpClientState_t iotNtpClientGetState(void);

/**
 * @brief Get the last set time. If the client has not been started or has not had time to contact the server, the value
 * will be 0.
 * @return The last set time.
 */
time_t iotNtpClientGetTime(void);

#endif // ! IOTSDK_NTP_CLIENT_H
