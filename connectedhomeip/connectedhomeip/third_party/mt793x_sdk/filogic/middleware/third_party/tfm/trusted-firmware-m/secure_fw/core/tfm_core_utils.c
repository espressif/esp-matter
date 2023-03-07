/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "tfm_utils.h"
#include "tfm_core_utils.h"

union tfm_core_addr_t {
    uintptr_t uint_addr;
    uint8_t *p_byte;
    uint32_t *p_word;
};

void *tfm_core_util_memcpy(void *dest, const void *src, size_t n)
{
    union tfm_core_addr_t p_dest;
    union tfm_core_addr_t p_src;

    TFM_CORE_ASSERT(dest != src);

    p_dest.p_byte = (uint8_t *)dest;
    p_src.p_byte = (uint8_t *)src;

    /*
     * Check src and dest address value to see if word-copy is applicable.
     * If applicable, use byte-copy for the first several unaligned bytes,
     * and then, word-copy for aligned memory.
     */
    if (!((p_dest.uint_addr ^ p_src.uint_addr) & (sizeof(uint32_t) - 1))) {
        while (n && (p_dest.uint_addr & (sizeof(uint32_t) - 1))) {
            *p_dest.p_byte++ = *p_src.p_byte++;
            n--;
        }

        while (n >= sizeof(uint32_t)) {
            *p_dest.p_word++ = *p_src.p_word++;
            n -= sizeof(uint32_t);
        }
    }

    /*
     * Word-copy is not applicable, use byte-copy for the remaining
     * unaligned memory.
     */
    while (n--) {
        *p_dest.p_byte++ = *p_src.p_byte++;
    }

    return dest;
}

void *tfm_core_util_memset(void *s, int c, size_t n)
{
    union tfm_core_addr_t p_mem;
    uint32_t quad_pattern;

    p_mem.p_byte = (uint8_t *)s;
    quad_pattern = (((uint8_t)c) << 24) | (((uint8_t)c) << 16) |
                   (((uint8_t)c) << 8) | ((uint8_t)c);

    while (n && (p_mem.uint_addr & (sizeof(uint32_t) - 1))) {
        *p_mem.p_byte++ = (uint8_t)c;
        n--;
    }

    while (n >= sizeof(uint32_t)) {
        *p_mem.p_word++ = quad_pattern;
        n -= sizeof(uint32_t);
    }

    while (n--) {
        *p_mem.p_byte++ = (uint8_t)c;
    }

    return s;
}
