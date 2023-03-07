/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SECURE_UTILITIES_H__
#define __SECURE_UTILITIES_H__

#include <stdbool.h>
#include "tfm_hal_device_header.h"
#include "cmsis_compiler.h"

#define EXC_NUM_THREAD_MODE     (0)
#define EXC_NUM_SVCALL          (11)
#define EXC_NUM_PENDSV          (14)
#define EXC_NUM_SYSTICK         (15)

#define ERROR_MSG(msg)

bool tfm_is_one_bit_set(uint32_t n);

#endif /* __SECURE_UTILITIES_H__ */
