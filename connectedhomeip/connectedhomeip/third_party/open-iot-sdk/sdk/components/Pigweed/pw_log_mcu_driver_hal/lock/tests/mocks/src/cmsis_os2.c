/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cmsis_os2.h"

DEFINE_FAKE_VALUE_FUNC(osMutexId_t, osMutexNew, const osMutexAttr_t *);
DEFINE_FAKE_VOID_FUNC(osMutexAcquire, osMutexId_t, unsigned int);
DEFINE_FAKE_VOID_FUNC(osMutexRelease, osMutexId_t);
