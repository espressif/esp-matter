/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IP_NETWORK_API_H_
#define IP_NETWORK_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"

typedef enum network_state_callback_event_t {
    NETWORK_UP,  /* The network is configured. */
    NETWORK_DOWN /* The network connection has been lost. */
} network_state_callback_event_t;

/* user provided hook for network state change */
typedef void (*network_state_callback_t)(network_state_callback_event_t event);

/** Initialise and run the network stack.
 *
 * @param network_state_callback pointer to network_state_callback_t that will be called when network changes state.
 * @param stack_size stack size for the thread handling data, uses default stack size if set to 0.
 *
 * @return osOK if successful, osError if it couldn't be started.
 */
osStatus_t start_network_task(network_state_callback_t network_state_callback, uint32_t stack_size);

#ifdef __cplusplus
}
#endif

#endif // IP_NETWORK_API_H_
