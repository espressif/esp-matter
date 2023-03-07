/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TICKCOUNTER_H
#define TICKCOUNTER_H

#include <stdint.h>

typedef uint64_t tickcounter_ms_t;
typedef struct TICK_COUNTER_INSTANCE_TAG *TICK_COUNTER_HANDLE;

TICK_COUNTER_HANDLE tickcounter_create(void);
void tickcounter_destroy(TICK_COUNTER_HANDLE tick_counter);
int tickcounter_get_current_ms(TICK_COUNTER_HANDLE tick_counter, tickcounter_ms_t *current_ms);

#endif // TICKCOUNTER_H
