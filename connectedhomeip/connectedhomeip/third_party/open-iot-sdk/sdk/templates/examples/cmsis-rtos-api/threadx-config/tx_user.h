/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TX_USER_H
#define TX_USER_H

/* Armv8-M runs in secure mode only when TrustZone is not configured */
#define TX_SINGLE_MODE_SECURE

/* Prevent tx_kernel_enter() from scheduling threads so that the CMSIS layer
 * can create objects before the scheduler is started. */
#define TX_PORT_SPECIFIC_PRE_SCHEDULER_INITIALIZATION return;

/* Allow 64 priorities to allow space for all of the possible CMSIS-RTOSv2
 * priorities to be represented. */
#define TX_MAX_PRIORITIES 64

/* Add special thread extension. */
#define TX_THREAD_USER_EXTENSION VOID *tx_cmsis_extension;

#endif
