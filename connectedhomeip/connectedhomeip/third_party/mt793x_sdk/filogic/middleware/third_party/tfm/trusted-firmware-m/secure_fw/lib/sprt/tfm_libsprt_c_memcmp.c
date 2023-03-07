/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>

int tfm_sprt_c_memcmp(const void *s1, const void *s2, size_t n)
{
    int result = 0;
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;
    while (n--) {
        if ((*p1 != *p2) && (result == 0)) {
            result = *p1 - *p2;
        } else {
            p1++;
            p2++;
        }
    }
    return result;
}
