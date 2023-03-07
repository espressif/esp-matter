/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TX_USER_H
#define TX_USER_H

/* Armv8-M runs in secure mode only when TrustZone is not configured */
#define TX_SINGLE_MODE_SECURE

/* Special definitions for the FreeRTOS layer */

/* From the ThreadX FreeRTOS layer documentation:
 *
 * "To better emulate the FreeRTOS timer behaviour it is recommended,
 * but not necessary, to enable processing of ThreadX timers within a task
 * instead of within an ISR. To do so the TX_TIMER_PROCESS_IN_ISR preprocessor
 * definition should NOT be defined within tx_user.h or tx_port.h It is also
 * recommended, but not required to have the timer task priority set at
 * priority 0, which is the highest priority within ThreadX"
 *
 * To do this, ensure that TX_TIMER_PROCESS_IN_ISR is not defined, and define
 * TX_TIMER_THREAD_PRIORITY as 0.
 */
#define TX_TIMER_THREAD_PRIORITY 0

/*
 * The FreeRTOS compatibility layer uses a custom field in the TX_THREAD
 * struct to store a reference to some extra compatibility-layer-related
 * data.
 */
#define TX_THREAD_USER_EXTENSION VOID *txfr_thread_ptr;

/*
 * For auto-initialization of the compatibility layer so FreeRTOS
 * application doesn't need modifying:
 */
#define TX_PORT_SPECIFIC_PRE_SCHEDULER_INITIALIZATION return;
#define TX_FREERTOS_AUTO_INIT                         1

#endif
