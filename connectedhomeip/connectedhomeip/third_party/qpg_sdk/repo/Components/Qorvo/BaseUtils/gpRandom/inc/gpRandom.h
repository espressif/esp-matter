
/*
 * Copyright (c) 2012-2013, GreenPeak Technologies
 * Copyright (c) 2017-2018, Qorvo Inc
 *
 *
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
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */


#ifndef _GPRANDOM_H_
#define _GPRANDOM_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpRandom_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

//DOCUMENTATION GENERATION: no @file required as all documented items are refered to a group

/**
 *@defgroup DEF_RANDOM General Random Definitions
 * @brief The general Random Definitions
 *
 * The gpRandom component is intended for generation of the random number sequences. The randomness of
 * the internal random number generator is configurable at compile time and if not changed, the default
 * randomness is 128bits. The randomness is introduced via the call to randomize the generator and the
 * consequent random numbers are produced by mashing up the random seed with 32bit based pseudo random
 * generator. Unless disabled in compile time, the mashing process is modulated with time based low
 * strength random generator.
 *
 * Note: Randomize function is lengthy and should be avoided to call it from points in code executing
 * under tight timeout.
 * Note: The driver is not made thread safe so care must be taken to avoid reentrancy. The consequence
 * might be that two callers getting random sequences end up with highly correlated random sequences.
 *
 *@defgroup GEN_RANDOM General Random Public Functions
 * @brief The general public Random functionality is implemented in these functions
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include <global.h>


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
 /** @typedef gpRandom_Result_t
 *  @brief The gpRandom_Result_t type defines the result of various gpRandom functions.
*/
typedef UInt8 gpRandom_Result_t;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpRandom_CodeJumpTableFlash_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */


/**@ingroup GEN_RANDOM
 * @brief This function generates new random number sequence
 *
 * @param pBuffer is reference to memory location where new random number sequence is to be stored
 * @param nmbrRandomBytes is number of random bytes in sequence to be stored
 */
void gpRandom_GetNewSequence(UInt8 nmbrRandomBytes, UInt8* pBuffer);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

/**@ingroup GEN_RANDOM
 * @brief This function seeds the generator with truly random numbers
 *
 * Calling this function costs a lot of time being blocked in its execution.
 */
void gpRandom_RandomizeSeed(void);

void gpRandom_Init(void);

void gpRandom_Reset(void);

void gpRandom_GetFromDRBG(UInt8 nmbrRandomBytes, UInt8* pBuffer);

UInt32 gpRandom_GenerateLargeRandom(void);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif // _GPRANDOM_H_


