/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cmsis_os2.h"

DEFINE_FAKE_VALUE_FUNC(uint32_t, osKernelGetTickCount);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osKernelGetTickFreq);
