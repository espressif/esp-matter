/*
 * Copyright (c) 2018, Qorvo Inc
 *
 *   silexCryptoSoc_Init.c
 *   Initialization function for the Silex CryptoSoc library.
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
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
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_SILEXCRYPTOSOC

#include "gpAssert.h"
#include "gpLog.h"

#include "silexCryptoSoc.h"
#include "silexCryptoSoc_Defs.h"
#ifdef GP_DIVERSITY_JUMPTABLES
#include "gpJumpTables.h"
#endif

// Silex specific functionality
#include "sx_rng.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifdef AES_GCM_EMABLED
#endif
#ifdef AES_HW_KEYS_ENABLED
#endif
#ifdef AES_MASK_ENABLED
#endif
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/* @brief Memory store for library operations requiring globally accessible variables */
silexCryptoSoc_GlobalVars_t silexCryptoSoc_GlobalVars;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void silexCryptoSoc_Init(void)
{
    UInt32 result;

    /*
     * Silex documentation says sx_rng_init() must be called before using any
     * other function in the CryptoSoc library.
    */

    /* Initialize */

    /* Startup time of RNG is roughly 281 us */
    result = sx_rng_init(false, RNG_CLKDIV);

    if (result != CRYPTOLIB_SUCCESS)
    {
        GP_LOG_PRINTF("ERROR: sx_rng_init failed result=%u", 0, (unsigned int)result);
    }
    else
    {
        GP_LOG_PRINTF("OK: sx_rng_init SUCCESS result=%u", 0, (unsigned int)result);
    }

    GP_ASSERT_SYSTEM(result == CRYPTOLIB_SUCCESS);
}

void silexCryptoSoc_InitFuncptr(void)
{
}
