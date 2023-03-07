/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "assert.h"
#include "cmsis_os2.h"

// Note: the attribute osMutexPrioInherit is defined to prevent higher priority
// threads being blocked for too much time when a lower priority thread is
// logging.
// Note: The mutex is allocated dynamically as CMSIS-RTOS doesn't expose a
// common macro for the mutex size.

static osMutexId_t lock = NULL;
static const osMutexAttr_t lock_config = {"log_mutex",        // name
                                          osMutexPrioInherit, // attr_bits
                                          NULL,
                                          0};

void _pw_log_init_lock()
{
    lock = osMutexNew(&lock_config);
    assert(lock);
}

void _pw_log_lock()
{
    osMutexAcquire(lock, osWaitForever);
}

void _pw_log_unlock()
{
    osMutexRelease(lock);
}
