/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CMSIS_OS2_H_
#define CMSIS_OS2_H_

#include "fff.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define osMutexPrioInherit 0x00000002U
#define osWaitForever      0xFFFFFFFF

typedef void *osMutexId_t;
typedef struct {
    const char *name;
    uint32_t attr_bits;
    void *cb_mem;
    uint32_t cb_size;
} osMutexAttr_t;

DECLARE_FAKE_VALUE_FUNC(osMutexId_t, osMutexNew, const osMutexAttr_t *);
DECLARE_FAKE_VOID_FUNC(osMutexAcquire, osMutexId_t, unsigned int);
DECLARE_FAKE_VOID_FUNC(osMutexRelease, osMutexId_t);

#ifdef __cplusplus
}
#endif

#endif // CMSIS_OS2_H_
