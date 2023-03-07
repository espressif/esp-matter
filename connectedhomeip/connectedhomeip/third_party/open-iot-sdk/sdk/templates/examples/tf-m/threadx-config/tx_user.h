/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TX_USER_H
#define TX_USER_H

/* Armv8-M runs in non-secure mode only when TrustZone is configured */
#define TX_SINGLE_MODE_NON_SECURE

/* Prevent tx_kernel_enter() from scheduling threads so that the CMSIS layer
 * can create objects before the scheduler is started. */
#define TX_PORT_SPECIFIC_PRE_SCHEDULER_INITIALIZATION return;

/* Allow 64 priorities to allow space for all of the possible CMSIS-RTOSv2
 * priorities to be represented. */
#define TX_MAX_PRIORITIES 64

/* Add special thread extension. */
#define TX_THREAD_USER_EXTENSION VOID *tx_cmsis_extension;

/* Increase default thread stack size to 1024 bytes to prevent stack
 * overflow. */
#define OS_STACK_SIZE 1024

#endif
