/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PW_LOG_MDH_UTIL_H_
#define PW_LOG_MDH_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

void _pw_log_init_lock();
void _pw_log_lock();
void _pw_log_unlock();

#ifdef __cplusplus
} // extern "C"
#endif

#endif // PW_LOG_MDH_UTIL_H_
