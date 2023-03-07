/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hkdf-sha.h"
#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SRAM0_BGN (0x00100000U)
#define SRAM0_END (0x0015C000U)
#define SRAM1_BGN (0x20000000U)
#define SRAM1_END (0x2001B000U)
#define NVRAM_BGN (0x480C0000U)
#define NVRAM_END (0x480C1000U)

/* We use 64 bytes before keystore region to store generated hash buffer. */
#define HASH_BUF (0x0015EFC0U)
#define HASH_LEN (0x0015EFF0U)

#define MIN_UNINIT_RAM_REQ_FOR_128BIT_ENTROPY (44UL * 1024U)

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t BOARD_GetUninitRamBase(void)
{
    uint32_t base;
#if defined(__GNUC__)
    extern uint32_t __DATA_END[];
    base = (uint32_t)__DATA_END;
#elif defined(__ICCARM__)
#pragma segment = "RO"
    base = (uint32_t)__sfe("RO");
#else
#error compiler not supported!
#endif
    if (base < SRAM0_BGN || base >= SRAM0_END)
    {
        /* In XIP case, all the SRAM hasn't been populated. */
        base = SRAM0_BGN;
    }

    return base;
}

static uint32_t BOARD_GetUninitNvramBase(void)
{
    uint32_t base;
#if defined(__GNUC__)
    extern uint32_t __nvram_end__[];
    base = (uint32_t)__nvram_end__;
#elif defined(__ICCARM__)
#pragma segment = "NVRAM_UNINIT"
    base = (uint32_t)__sfe("NVRAM_UNINIT");
#else
#error compiler not supported!
#endif

    return base;
}

void BOARD_GetHash(uint8_t *buf, uint32_t *len)
{
    uint8_t *hash    = (uint8_t *)HASH_BUF;
    uint8_t *hashLen = (uint8_t *)HASH_LEN;

    assert(buf);
    assert(len);

    *len = MIN(*len, *hashLen);
    memcpy(buf, hash, *len);
}

void SystemInitHook(void)
{
    int32_t uninit_len       = 0;
    int32_t total_uninit_len = 0;
    uint32_t base;
    SHA256Context ctx;
    uint8_t *hash    = (uint8_t *)HASH_BUF;
    uint8_t *hashLen = (uint8_t *)HASH_LEN;

    SHA256Reset(&ctx);

    base       = BOARD_GetUninitNvramBase();
    uninit_len = NVRAM_END - base;
    if (uninit_len > 0)
    {
        SHA256Input(&ctx, (uint8_t *)base, uninit_len);
        total_uninit_len += uninit_len;
    }
    else if (uninit_len < 0)
    {
        /* Override HASH_BUF area, cannot go ahead. */
        while (true)
        {
        }
    }

    base       = BOARD_GetUninitRamBase();
    uninit_len = SRAM0_END - base;
    if (uninit_len > 0)
    {
        SHA256Input(&ctx, (uint8_t *)base, uninit_len);
        total_uninit_len += uninit_len;
    }

    base       = SRAM1_BGN; /* SRAM1 region not populated. */
    uninit_len = SRAM1_END - base;
    if (uninit_len > 0)
    {
        SHA256Input(&ctx, (uint8_t *)base, uninit_len);
        total_uninit_len += uninit_len;
    }

    SHA256Result(&ctx, hash);

    if (total_uninit_len >= MIN_UNINIT_RAM_REQ_FOR_128BIT_ENTROPY)
    {
        *hashLen = SHA256HashSize;
    }
    else
    {
        *hashLen = 0U;
    }
}
