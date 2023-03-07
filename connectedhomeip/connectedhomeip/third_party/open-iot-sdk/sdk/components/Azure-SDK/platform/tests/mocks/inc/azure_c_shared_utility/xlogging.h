/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef XLOGGING_H
#define XLOGGING_H

#include "fff.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_FAKE_VOID_FUNC(LogError, char *);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // XLOGGING_H
