/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MDH_SERIAL_API_H
#define MDH_SERIAL_API_H

#include "fff.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int mdh_serial_t;

DECLARE_FAKE_VOID_FUNC(mdh_serial_put_data, mdh_serial_t *, uint32_t);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MDH_SERIAL_API_H
