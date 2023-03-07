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

DECLARE_FAKE_VALUE_FUNC(uint32_t, osKernelGetTickCount);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osKernelGetTickFreq);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CMSIS_OS2_H_
