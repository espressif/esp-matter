/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iotsdk_alloc.h"

DEFINE_FAKE_VALUE_FUNC(void *, iotsdk_alloc_malloc, size_t);
DEFINE_FAKE_VOID_FUNC(iotsdk_alloc_free, void *);
