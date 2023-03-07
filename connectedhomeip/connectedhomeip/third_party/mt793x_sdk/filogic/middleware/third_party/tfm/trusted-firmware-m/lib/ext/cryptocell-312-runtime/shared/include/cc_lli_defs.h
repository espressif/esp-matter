/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_LLI_DEFS_H_
#define _CC_LLI_DEFS_H_
#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif
#include "cc_bitops.h"

/* Max DLLI size */
#define DLLI_SIZE_BIT_SIZE  0x18    // DX_DSCRPTR_QUEUE_WORD1_DIN_SIZE_BIT_SIZE

#define CC_MAX_MLLI_ENTRY_SIZE 0x10000

#define LLI_SET_ADDR(lli_p, addr) \
        BITFIELD_SET(((uint32_t *)(lli_p))[LLI_WORD0_OFFSET], LLI_LADDR_BIT_OFFSET, LLI_LADDR_BIT_SIZE, (addr & UINT32_MAX)); \
        BITFIELD_SET(((uint32_t *)(lli_p))[LLI_WORD1_OFFSET], LLI_HADDR_BIT_OFFSET, LLI_HADDR_BIT_SIZE, ((addr >> 32) & UINT16_MAX));

#define LLI_SET_SIZE(lli_p, size) \
        BITFIELD_SET(((uint32_t *)(lli_p))[LLI_WORD1_OFFSET], LLI_SIZE_BIT_OFFSET, LLI_SIZE_BIT_SIZE, size)


/* Size of entry */
#define LLI_ENTRY_WORD_SIZE 2
#define LLI_ENTRY_BYTE_SIZE (LLI_ENTRY_WORD_SIZE * sizeof(uint32_t))

/* Word0[31:0] = ADDR[31:0] */
#define LLI_WORD0_OFFSET 0
#define LLI_LADDR_BIT_OFFSET 0
#define LLI_LADDR_BIT_SIZE 32
/* Word1[31:16] = ADDR[47:32]; Word1[15:0] = SIZE */
#define LLI_WORD1_OFFSET 1
#define LLI_SIZE_BIT_OFFSET 0
#define LLI_SIZE_BIT_SIZE 16
#define LLI_HADDR_BIT_OFFSET 16
#define LLI_HADDR_BIT_SIZE 16


#endif /*_CC_LLI_DEFS_H_*/
