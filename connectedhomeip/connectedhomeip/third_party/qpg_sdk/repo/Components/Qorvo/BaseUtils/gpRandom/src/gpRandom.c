/*
 * Copyright (c) 2012-2016, GreenPeak Technologies
 * Copyright (c) 2017-2018, Qorvo Inc
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

// General includes
#include "gpRandom.h"
#ifdef GP_HAL_DIVERSITY_SEC_CRYPTOSOC
#include "gpRandom_HASH_DRBG.h"
#endif /* GP_HAL_DIVERSITY_SEC_CRYPTOSOC */
#include "gpSched.h"
#include "gpHal.h"


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_RANDOM

#ifndef GP_RANDOM_RANDOMNESS_STRENGTH_OCTETS
#define GP_RANDOM_RANDOMNESS_STRENGTH_OCTETS             16UL
#endif /* GP_RANDOM_RANDOMNESS_STRENGTH_OCTETS */
#define GP_RANDOM_RANDOMNESS_STRENGTH_WORDS             ((GP_RANDOM_RANDOMNESS_STRENGTH_OCTETS + 3)>>2)
/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
static void Random_MashUpSeed(void);
static void Random_PseudoAdvance(void);
static UInt32 Random_PseudoRead32(void);
static void Random_GetLastSequence(UInt8* pBuffer, UInt8 nmbrRandomBytes);
/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
static UInt32 Random_Seed[GP_RANDOM_RANDOMNESS_STRENGTH_WORDS];
static UInt8 RandomSeedWordOffset = 0;
static UInt32 Random_m_w = 521288629UL; /* https://groups.google.com/forum/?fromgroups#!topic/sci.crypt/yoaCpGWKEk0[1-25] */
static UInt32 Random_m_z = 362436069UL; /* https://groups.google.com/forum/?fromgroups#!topic/sci.crypt/yoaCpGWKEk0[1-25] */
/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
extern UInt8 gpHal_GetRandomSeed(void) ;

 void Random_MashUpSeed(void)
{
    UInt8 seedWordCount;
    UInt32 hwTime;
    hwTime=gpSched_GetCurrentTime();
    RandomSeedWordOffset = (UInt8)(hwTime % GP_RANDOM_RANDOMNESS_STRENGTH_WORDS);
    for(seedWordCount = 0; seedWordCount < GP_RANDOM_RANDOMNESS_STRENGTH_WORDS; seedWordCount++)
    {
        Random_PseudoAdvance();
        Random_Seed[(RandomSeedWordOffset+seedWordCount)%GP_RANDOM_RANDOMNESS_STRENGTH_WORDS] ^= Random_PseudoRead32();
    }
}

void Random_PseudoAdvance(void) {
    Random_m_z = 36969 * (Random_m_z & 65535) + (Random_m_z >> 16);
    Random_m_w = 18000 * (Random_m_w & 65535) + (Random_m_w >> 16);
}

UInt32 Random_PseudoRead32(void) {
    return (Random_m_z << 16) + Random_m_w;
}

void Random_GetLastSequence(UInt8* pBuffer, UInt8 nmbrRandomBytes)
{
    UInt8 randomByteIndex;

    for(randomByteIndex=0; randomByteIndex < nmbrRandomBytes; randomByteIndex++)
    {
        pBuffer[randomByteIndex] = ((UInt8*)Random_Seed)[((RandomSeedWordOffset<<2) + randomByteIndex)%(GP_RANDOM_RANDOMNESS_STRENGTH_WORDS<<2)];
    }
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
void gpRandom_RandomizeSeed(void)
{
    UInt8 seedByteIndex;

    gpHal_GoToSleepWhenIdle(false);
    for(seedByteIndex = 0; seedByteIndex < (GP_RANDOM_RANDOMNESS_STRENGTH_WORDS<<2); seedByteIndex++)
    {
        ((UInt8*)Random_Seed)[seedByteIndex] = gpHal_GetRandomSeed();
    }
    gpHal_GoToSleepWhenIdle(true);


}

// Generate random numbers, using a Linear Congruential Generator algorithm
// This API must only be used for non-cryptographical purposes
void gpRandom_GetNewSequence(UInt8 nmbrRandomBytes, UInt8* pBuffer)
{
    UInt8 randomBytesGenerated = 0;

    while(randomBytesGenerated < nmbrRandomBytes)
    {
        UInt8 bytesToGenerate = (GP_RANDOM_RANDOMNESS_STRENGTH_WORDS<<2) < (nmbrRandomBytes - randomBytesGenerated) ?
                                (GP_RANDOM_RANDOMNESS_STRENGTH_WORDS<<2) : (nmbrRandomBytes - randomBytesGenerated);
        Random_MashUpSeed();
        Random_GetLastSequence(&pBuffer[randomBytesGenerated], bytesToGenerate);
        randomBytesGenerated += bytesToGenerate;
    }
}

// Generate random numbers, using a FIPS PUB 140-2 compliant generator
// This API can be used for cryptographical purposes
void gpRandom_GetFromDRBG(UInt8 nmbrRandomBytes, UInt8* pBuffer)
{
#if defined(GP_HAL_DIVERSITY_SEC_CRYPTOSOC)
    gpRandom_HASH_DRBG_Generate(nmbrRandomBytes, pBuffer);
#else
    // cryptographic mechanisms cannot be supported
    GP_ASSERT_DEV_EXT(false);
#endif /* GP_HAL_DIVERSITY_SEC_CRYPTOSOC */
}

void gpRandom_Init(void)
{


#if defined(GP_HAL_DIVERSITY_SEC_CRYPTOSOC)
    gpRandom_HASH_DRBG_Init();
#endif /* GP_HAL_DIVERSITY_SEC_CRYPTOSOC */

    // seed the Linear Congruential Generator
    gpRandom_RandomizeSeed();
}

void gpRandom_Reset(void)
{
    RandomSeedWordOffset = 0;
    Random_m_w = 521288629UL; /* https://groups.google.com/forum/?fromgroups#!topic/sci.crypt/yoaCpGWKEk0[1-25] */
    Random_m_z = 362436069UL; /* https://groups.google.com/forum/?fromgroups#!topic/sci.crypt/yoaCpGWKEk0[1-25] */
    MEMSET(Random_Seed, 0, sizeof(Random_Seed));
}

UInt32 gpRandom_GenerateLargeRandom(void)
{
    UInt32 r = 0;

    gpRandom_GetFromDRBG(sizeof(r),(UInt8*)&r);

    return r;
}
