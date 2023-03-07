/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _BSV_DEFS_H
#define _BSV_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif

/*! @file
@brief This file contains definitions used for the Boot Services APIs.
*/

/*! Defines the maximal hash boot key size in words. */
#define CC_BSV_MAX_HASH_SIZE_IN_WORDS       8
/*! Defines the maximal hash boot key size in bytes. */
#define CC_BSV_MAX_HASH_SIZE_IN_BYTES       (CC_BSV_MAX_HASH_SIZE_IN_WORDS*sizeof(uint32_t))
/*! Defines the maximal full-hash boot key size in words. */
#define CC_BSV_256B_HASH_SIZE_IN_WORDS      CC_BSV_MAX_HASH_SIZE_IN_WORDS
/*! Defines the maximal dual-hash boot key size in words. */
#define CC_BSV_128B_HASH_SIZE_IN_WORDS      CC_BSV_MAX_HASH_SIZE_IN_WORDS/2

/*! ICV Firmware minimal version maximal size. */
#define CC_BSV_MAX_HBK0_VERSION_COUNTER     64
/*! OEM Firmware minimal version maximal size. */
#define CC_BSV_MAX_HBK1_VERSION_COUNTER     96
/*! OEM Firmware minimal version maximal size (no ICV). */
#define CC_BSV_MAX_HBK_VERSION_COUNTER      160

/*! HUK status bit definition. */
#define DX_BSV_STAUS_HUK_ERR_BIT_SHIFT         0x0UL
/*! HUK status size bit definition. */
#define DX_BSV_STAUS_HUK_ERR_BIT_SIZE          0x1UL
/*! Kpicv status bit definition. */
#define DX_BSV_STAUS_KPICV_ERR_BIT_SHIFT         0x1UL
/*! Kpicv status size bit definition. */
#define DX_BSV_STAUS_KPICV_ERR_BIT_SIZE          0x1UL
/*! Kceicv status bit definition. */
#define DX_BSV_STAUS_KCEICV_ERR_BIT_SHIFT         0x2UL
/*! Kceicv status size bit definition. */
#define DX_BSV_STAUS_KCEICV_ERR_BIT_SIZE          0x1UL
/*! Kcp status bit definition. */
#define DX_BSV_STAUS_KCP_ERR_BIT_SHIFT         0x3UL
/*! Kcp status size bit definition. */
#define DX_BSV_STAUS_KCP_ERR_BIT_SIZE          0x1UL
/*! Kce status bit definition. */
#define DX_BSV_STAUS_KCE_ERR_BIT_SHIFT         0x4UL
/*! Kce status size bit definition. */
#define DX_BSV_STAUS_KCE_ERR_BIT_SIZE          0x1UL

/*! Definition for all ones word. */
#define CC_BSV_ALL_ONES_VALUE   0xffffffffUL
/*! Definition for number of bits in a 32bit word. */
#define CC_BSV_ALL_ONES_NUM_BITS 32

/* ********************** Macros ******************************* */
/*! This macro counts the number of zeroes in a 32bits word. */
#define CC_BSV_COUNT_ZEROES(regVal, regZero)                    \
    do {                                    \
        uint32_t val = regVal;                                          \
        val = val - ((val >> 1) & 0x55555555);                          \
        val = (val & 0x33333333) + ((val >> 2) & 0x33333333);           \
        val = ((((val + (val >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24);   \
        regZero += (32 - val);                      \
    }while(0)


#ifdef __cplusplus
}
#endif

#endif



