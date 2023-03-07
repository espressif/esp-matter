/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpHal_MISC.c
 *
 * This file contains miscellaneous functions
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

#include "gpHal.h"
#include "gpHal_DEFS.h"
#include "gpHal_MISC.h"

//GP hardware dependent register definitions
#include "gpHal_HW.h"
#include "gpHal_reg.h"

#include "gpRandom.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

//-------------------------------------------------------------------------------------------------------
//  RANDOM GENERATION FUNCTIONS
//-------------------------------------------------------------------------------------------------------

void gpHal_InitRandom(void)
{
// #ifdef GP_HAL_DIVERSITY_USE_RADIO_FOR_ENTROPY
    //Enable PRG settings - clk will be enabled when taking a sample
    GP_WB_WRITE_PRG_ENABLE_RANDOM_SEED(1);
    GP_WB_WRITE_PRG_ENABLE_RANDOM_SOURCE(1); //Add extra random data

    GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_TX_OUT_OF_LOCK_INTERRUPT(0);
    GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_TX_OUT_OF_RANGE_INTERRUPT(0);
    GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_RX_OUT_OF_RANGE_INTERRUPT(0);

    //Enable Pseudo random block
    GP_WB_WRITE_PRG_ENABLE_CLK_PRG_BY_UC(1);

#ifndef GP_COMP_CHIPEMU
    //Toggle radio (on unused channel) to generate I/Q sample for PRG
    GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH5(1);
    HAL_WAIT_US(100);
    GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH5(0);
#else
#endif


    //Disable Pseudo random block
    GP_WB_WRITE_PRG_ENABLE_CLK_PRG_BY_UC(0);

    GP_WB_FLL_CLR_RX_OUT_OF_RANGE_INTERRUPT();
    GP_WB_FLL_CLR_TX_OUT_OF_RANGE_INTERRUPT();
    GP_WB_FLL_CLR_TX_OUT_OF_LOCK_INTERRUPT();

    GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_TX_OUT_OF_LOCK_INTERRUPT(1);
    GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_TX_OUT_OF_RANGE_INTERRUPT(1);
    GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_RX_OUT_OF_RANGE_INTERRUPT(1);
// #else /* use QRNG for entropy generation */
//     /* enable all ring oscillators */
//     GP_WB_WRITE_QRNG_OSC_ENABLE_MASK(GP_WB_QRNG_OSC_ENABLE_MASK_MASK);
//     // GP_WB_WRITE_QRNG_SAMPLE_CLK_DIV_RATIO(0); /* same as the reset value */

// #endif //def GP_HAL_DIVERSITY_USE_RADIO_FOR_ENTROPY
}

// Recommendation to only use this function for seeding an RBG (such as a DRBG or LCG)
// In case you just need a random number (e.g. for a random delay), use gpRandom_GetNewSequence() or gpRandom_GenerateLargeRandom()
UInt8 gpHal_GetRandomSeed(void)
{
    UInt8 seed;

//#ifdef GP_HAL_DIVERSITY_USE_RADIO_FOR_ENTROPY
    //Enable Pseudo random block
    GP_WB_WRITE_PRG_ENABLE_CLK_PRG_BY_UC(1);

    seed = GP_WB_READ_PRG_RANDOM_VALUE();

    //Disable Pseudo random block
    GP_WB_WRITE_PRG_ENABLE_CLK_PRG_BY_UC(0);

// #else /* use QRNG for entropy generation */
//     gpHal_GetQRNGRandomSeed(1, &seed);
// #endif //def GP_HAL_DIVERSITY_USE_RADIO_FOR_ENTROPY

    return seed;
}

// static UInt32 gpHal_getQRNGSeed(void)
// {
//     GP_WB_QRNG_TRIGGER_SEED_GEN();
//     GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_QRNG_SEED_GEN_BUSY() == 1, 1000000UL);
//     return GP_WB_READ_QRNG_RANDOM_SEED();
// }

// void gpHal_GetQRNGRandomSeed(UInt8 size, UInt8* buffer)
// {
//     UInt32 seed32;

//     /* forcing the oscillator on before the trigger */
//     /* this is required in case the ratio is 0 */
//      if the ration is 1 or higher this could be skipped
//     GP_WB_WRITE_QRNG_FORCE_ON_OSCILLATORS(1);

//     while(size > 4)
//     {
//         seed32 = gpHal_getQRNGSeed();
//         MEMCPY(buffer, &seed32, 4);
//         size -= 4;
//     }
//     seed32 = gpHal_getQRNGSeed();
//     MEMCPY(buffer, &seed32, size);

//     /* make sure we don't burn power in the oscillators */
//     GP_WB_WRITE_QRNG_FORCE_ON_OSCILLATORS(0);
// }
