/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include "tfm_libsprt_c.h"

void *tfm_sprt_c_memcpy(void *dest, const void *src, size_t n)
{
    return tfm_sprt_c_memmove(dest, src, n);
}
