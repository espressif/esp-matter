/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PW_LOG_MDH_UTIL_H_
#define PW_LOG_MDH_UTIL_H_

#include "fff.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_FAKE_VOID_FUNC(_pw_log_init_lock);
DECLARE_FAKE_VOID_FUNC(_pw_log_lock);
DECLARE_FAKE_VOID_FUNC(_pw_log_unlock);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // PW_LOG_MDH_UTIL_H_
