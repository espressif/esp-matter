/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PW_LOG_MDH_BACKEND_H_
#define PW_LOG_MDH_BACKEND_H_

#ifdef __cplusplus
extern "C" {
#endif

void pw_log_mdh_init(mdh_serial_t *serial);

void _pw_log_mdh(const char *message);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // PW_LOG_MDH_BACKEND_H_
