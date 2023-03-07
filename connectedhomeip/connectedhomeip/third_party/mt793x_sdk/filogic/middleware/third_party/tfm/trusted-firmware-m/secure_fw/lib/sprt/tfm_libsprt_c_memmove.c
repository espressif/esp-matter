/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <stddef.h>
#include "tfm_libsprt_c.h"

#define GET_MEM_ADDR_BIT0(x)        ((x) & 0x1)
#define GET_MEM_ADDR_BIT1(x)        ((x) & 0x2)

union tfm_mem_addr_t {
    uintptr_t uint_addr;
    uint8_t *p_byte;
    uint16_t *p_dbyte;
    uint32_t *p_qbyte;
};

/*
 * Consider 3 conditions.
 * 1) quad-byte copy (qbyte)
 * 2) double-byte copy (dbyte)
 * 3) byte copy
 *
 * And for overlapped memory area.
 * 1) overlapped: use backward memory move.
 * 2) non-overlapped: use forward memory move.
 */

static void *tfm_memmove_forward(void *dest, const void *src, size_t n)
{
    union tfm_mem_addr_t p_dest, p_src;

    p_dest.uint_addr = (uintptr_t)dest;
    p_src.uint_addr = (uintptr_t)src;

    /* byte copy for unaligned address. check the last bit of address. */
    while (n && (GET_MEM_ADDR_BIT0(p_dest.uint_addr) ||
                 GET_MEM_ADDR_BIT0(p_src.uint_addr))) {
        *p_dest.p_byte++ = *p_src.p_byte++;
        n--;
    }

    /* dbyte-copy for aligned address. check the 2nd last bit of address. */
    while (n >= sizeof(uint16_t) && (GET_MEM_ADDR_BIT1(p_dest.uint_addr) ||
                                     GET_MEM_ADDR_BIT1(p_src.uint_addr))) {
        *(p_dest.p_dbyte)++ = *(p_src.p_dbyte)++;
        n -= sizeof(uint16_t);
    }

    /* qbyte-copy for aligned address. */
    while (n >= sizeof(uint32_t)) {
        *(p_dest.p_qbyte)++ = *(p_src.p_qbyte)++;
        n -= sizeof(uint32_t);
    }

    /* byte copy for the remaining bytes. */
    while (n--) {
        *p_dest.p_byte++ = *p_src.p_byte++;
    }

    return dest;
}

static void *tfm_memmove_backward(void *dest, const void *src, size_t n)
{
    union tfm_mem_addr_t p_dest, p_src;

    p_dest.uint_addr = (uintptr_t)dest + n;
    p_src.uint_addr = (uintptr_t)src + n;

    /* byte copy for unaligned address. check the last bit of address. */
    while (n && (GET_MEM_ADDR_BIT0(p_dest.uint_addr) ||
                 GET_MEM_ADDR_BIT0(p_src.uint_addr))) {
        *(--p_dest.p_byte) = *(--p_src.p_byte);
        n--;
    }

    /* dbyte-copy for aligned address. check the 2nd last bit of address. */
    while (n >= sizeof(uint16_t) && (GET_MEM_ADDR_BIT1(p_dest.uint_addr) ||
                                     GET_MEM_ADDR_BIT1(p_src.uint_addr))) {
        *(--p_dest.p_dbyte) = *(--p_src.p_dbyte);
        n -= sizeof(uint16_t);
    }

    /* qbyte-copy for aligned address. */
    while (n >= sizeof(uint32_t)) {
        *(--p_dest.p_qbyte) = *(--p_src.p_qbyte);
        n -= sizeof(uint32_t);
    }

    /* byte copy for the remaining bytes. */
    while (n--) {
        *(--p_dest.p_byte) = *(--p_src.p_byte);
    }

    return dest;
}

void *tfm_sprt_c_memmove(void *dest, const void *src, size_t n)
{
    /*
     * FixMe: Add a "assert (dest == NULL || src == NULL)" here
     * after "assert()" for sprtl is implemented.
     */
    if (src < dest) {
        tfm_memmove_backward(dest, src, n);
    } else {
        tfm_memmove_forward(dest, src, n);
    }

    return dest;
}
