/*
 * Copyright (c) 2017, Qorvo Inc
 *
 * This file implements a random number generator
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright law
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#ifndef GP_HAL_DIVERSITY_SEC_CRYPTOSOC
#error "The gpRandom_HASH_DRBG.c module requires the CRYPTOSOC library + (>K8c)HW"
#else
// This module provides a wrapper for the sx_drbg_hash_generate() function
// sx_drbg_hash_generate() implements a HASH_DRBG as desribed in NIST.SP.800-90A r1
#endif /* GP_HAL_DIVERSITY_SEC_CRYPTOSOC */

// General includes
#include "gpRandom.h"
#include "gpHal.h"
#include "sx_drbg_hash.h"
#include "sx_generic.h"
#include "sx_rng.h"



/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_RANDOM

// Note: BLE requires 256 bits entropy for its LE Secure Connection feature
#define SX_MAX_STRENGTH 256
#define SX_ENTROPY_SIZE (SX_MAX_STRENGTH / 8)

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
static gpRandom_Result_t Translate_Sc_resultcode(UInt32 res);

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
sx_drbg_hash_ctx_t sx_drbg_ctx;  // context for the sx hash drbg: this contains the internal (secret) state of the generator

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
gpRandom_Result_t Translate_Sc_resultcode(UInt32 res)
{
    // simple success/fail for now . . .
    return (CRYPTOLIB_SUCCESS == res) ? gpHal_ResultSuccess : gpHal_ResultInvalidRequest;
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
void gpRandom_FillEntropyBuffer(UInt8* buf, UInt16 size)
{
    sx_enable_clock();
    sx_rng_get_rand(buf, (UInt32)size);
    sx_disable_clock();
}

gpRandom_Result_t gpRandom_HASH_DRBG_Init(void)
{
    UInt32 res;

    UInt8 entropy_for_init[SX_ENTROPY_SIZE];
    UInt8 personalization[SX_ENTROPY_SIZE];
    UInt8 nonce[SX_ENTROPY_SIZE];

    MEMSET(&sx_drbg_ctx, 0, sizeof(sx_drbg_hash_ctx_t));

    gpRandom_FillEntropyBuffer(entropy_for_init, SX_ENTROPY_SIZE);
    gpRandom_FillEntropyBuffer(personalization, SX_ENTROPY_SIZE);
    gpRandom_FillEntropyBuffer(nonce, SX_ENTROPY_SIZE);
    sx_enable_clock();
    res = sx_drbg_hash_instantiate(&sx_drbg_ctx,
                                   SX_MAX_STRENGTH,
                                   entropy_for_init, SX_ENTROPY_SIZE,
                                   personalization, SX_ENTROPY_SIZE,
                                   nonce, SX_ENTROPY_SIZE);
    sx_disable_clock();

    return Translate_Sc_resultcode(res);
}

gpRandom_Result_t gpRandom_HASH_DRBG_Reseed(void)
{
    if (!sx_drbg_ctx.is_instantiated)
    {
        return gpHal_ResultInvalidRequest;
    }

    UInt32 res;
    UInt8 entropy_for_reseed[SX_ENTROPY_SIZE];
    gpRandom_FillEntropyBuffer(entropy_for_reseed, SX_ENTROPY_SIZE);
    sx_enable_clock();
    res = sx_drbg_hash_reseed(&sx_drbg_ctx,
                              entropy_for_reseed, SX_ENTROPY_SIZE);
    sx_disable_clock();
    return Translate_Sc_resultcode(res);
}

gpRandom_Result_t gpRandom_HASH_DRBG_Generate(UInt32 numbytes, UInt8* pData)
{
    if (!sx_drbg_ctx.is_instantiated)
    {
        return gpHal_ResultInvalidRequest;
    }

    UInt32 res;
    res = gpRandom_HASH_DRBG_Reseed(); // always reseed
    if ( (CRYPTOLIB_SUCCESS != res) || (0 == numbytes) )
    {
        return Translate_Sc_resultcode(res);
    }

    block_t sx_block;
    sx_block.addr = pData;
    sx_block.len = (UInt32)numbytes;
    sx_block.flags = BLOCK_S_INCR_ADDR; /* pData is a buffer, not a FiFo */

    sx_enable_clock();
    res = sx_drbg_hash_generate(&sx_drbg_ctx, sx_block);
    sx_disable_clock();

    return Translate_Sc_resultcode(res);
}


