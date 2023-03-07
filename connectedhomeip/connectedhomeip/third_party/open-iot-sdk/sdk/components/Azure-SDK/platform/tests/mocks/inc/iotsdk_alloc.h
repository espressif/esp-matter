/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IOTSDK_ALLOC_H_
#define IOTSDK_ALLOC_H_

#include "fff.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_FAKE_VALUE_FUNC(void *, iotsdk_alloc_malloc, size_t);
DECLARE_FAKE_VOID_FUNC(iotsdk_alloc_free, void *);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* IOTSDK_ALLOC_H_ */
