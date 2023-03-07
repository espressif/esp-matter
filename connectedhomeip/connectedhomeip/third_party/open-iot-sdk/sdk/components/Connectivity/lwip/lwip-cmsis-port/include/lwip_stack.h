/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LWIP_STACK_H_
#define LWIP_STACK_H_

#include "lwip_emac_netif.h"

/** Cause the stack to read the input buffer.
 *
 * @param context netif context
 */
void signal_receive(netif_context_t *context);

/** Run the LWIP stack. This does not return.
 *
 * @param pvParameters pointer to network_state_callback_t that will be called when network changes state.
 */
void lwip_task(void *network_state_callback);

#endif // LWIP_STACK_H_
