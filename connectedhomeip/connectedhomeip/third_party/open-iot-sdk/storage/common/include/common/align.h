/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef COMMON_ALIGN_H
#define COMMON_ALIGN_H

#include <stdint.h>

static inline size_t align_up(size_t val, size_t size)
{
    return size ? size * (((val - 1) / size) + 1) : 0;
}

static inline uint64_t align_down(uint64_t val, uint64_t size)
{
    return (val / size) * size;
}

#endif // COMMON_ALIGN_H
