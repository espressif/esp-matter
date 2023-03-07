/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_LOG_H__
#define __TFM_LOG_H__

#include "tfm_log_raw.h"

/* Functions and macros in this file is for 'thread mode' usage. */

#if defined(TFM_NORMAL_BUILD) || defined(TFM_DEBUG_BUILD)
#define LOG_MSG(...) printf(__VA_ARGS__)
#else
#define LOG_MSG(...)
#endif /* ENABLE_TFM_TESTCASE */

#endif /* __TFM_LOG_H__ */
