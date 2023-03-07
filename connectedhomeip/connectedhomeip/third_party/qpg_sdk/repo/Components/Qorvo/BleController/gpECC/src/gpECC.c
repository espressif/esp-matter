/*
 * Copyright (c) 2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
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
#define GP_COMPONENT_ID GP_COMPONENT_ID_ECC

#include "gpECC.h"
#include "hal.h"

#ifdef GP_ECC_DIVERSITY_USE_EXT_LIB
#include "uECC.h"
#else
/* #include "uECC.h" */
#endif


/*****************************************************************************
 *                    Constants and checks
 *****************************************************************************/

#ifdef GP_ECC_DIVERSITY_USE_SLICING

#ifndef GP_ECC_DIVERSITY_USE_EXT_LIB
#error "GP_ECC_DIVERSITY_USE_SLICING only works with the uECC library"
#endif /* GP_ECC_DIVERSITY_USE_SLICING */

#endif /* GP_ECC_DIVERSITY_USE_SLICING */

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Local vars
 *****************************************************************************/

/*****************************************************************************
 *                    Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Private Function Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

gpECC_Curve gpECC_secp256r1(void)
{
#ifdef GP_ECC_DIVERSITY_USE_EXT_LIB
    return (gpECC_Curve) uECC_secp256r1();
#else
     /*return (gpECC_Curve) uECC_secp256r1();*/
    return NULL;
#endif
}

Int32 gpECC_make_key(UInt8 *public_key, UInt8 *private_key, gpECC_Curve curve)
{
#ifdef GP_ECC_DIVERSITY_USE_EXT_LIB
    /* uECC lib functions have some very expensive loops, make sure watchdog will not be triggered */
    HAL_WDT_DISABLE();
    Int32 result = (Int32) uECC_make_key(public_key, private_key, (uECC_Curve)curve);
    HAL_WDT_ENABLE(0xFFFF);

    return result;
#else
    /* uECC lib functions have some very expensive loops, make sure watchdog will not be triggered */
    /* HAL_WDT_DISABLE(); */
    /* Int32 key = (Int32)uECC_make_key(public_key, private_key, (uECC_Curve)curve); */
    /* HAL_WDT_ENABLE(0xFFFF); */

    /* return key; */

    return 0;
#endif
}

#if defined(GP_ECC_DIVERSITY_USE_SLICING)
void gpECC_make_key_sliced(const UInt8 *public_key, const UInt8 *private_key, gpECC_Curve curve, gpECC_MakeKey_SliceCb CbFunction, UInt8 **ContextPtrs, UInt16 *ContextSizes)
{
#ifdef GP_ECC_DIVERSITY_USE_EXT_LIB
    uECC_make_key_sliced(public_key, private_key, (uECC_Curve)curve, CbFunction, ContextPtrs, ContextSizes);
#else
    /* uECC_make_key_sliced(public_key, private_key, (uECC_Curve)curve, CbFunction, ContextPtrs, ContextSizes); */

#endif
}
#endif

Int32 gpECC_shared_secret(const UInt8 *public_key, const UInt8 *private_key, UInt8* secret, gpECC_Curve curve)
{
#ifdef GP_ECC_DIVERSITY_USE_EXT_LIB
    if (0 == gpECC_valid_public_key(public_key))
    {
        return false;
    }
    /* uECC lib functions have some very expensive loops, make sure watchdog will not be triggered */
    HAL_WDT_DISABLE();
    Int32 result = (Int32) uECC_shared_secret(public_key, private_key, secret, (uECC_Curve)curve);
    HAL_WDT_ENABLE(0xFFFF);
    return result; // note that this return value is not checked in Cordio
#else
    /* uECC lib functions have some very expensive loops, make sure watchdog will not be triggered */

    /* HAL_WDT_DISABLE(); */
    /* Int32 sharedSecret = (Int32)uECC_shared_secret(public_key, private_key, secret, (uECC_Curve)curve); */
    /* HAL_WDT_ENABLE(0xFFFF); */
    /* return sharedSecret; */

    return 0;
#endif
}

#if defined(GP_ECC_DIVERSITY_USE_SLICING)
void gpECC_shared_secret_sliced(const UInt8 *public_key, const UInt8 *private_key, UInt8* secret, gpECC_Curve curve, gpECC_SharedSecret_SliceCb CbFunction, UInt8 **ContextPtrs, UInt16 *ContextSizes)
{
#ifdef GP_ECC_DIVERSITY_USE_EXT_LIB
    uECC_shared_secret_sliced(public_key, private_key, secret, (uECC_Curve)curve, CbFunction, ContextPtrs, ContextSizes);
#else
    /* uECC_shared_secret_sliced(public_key, private_key, secret, (uECC_Curve)curve, CbFunction, ContextPtrs, ContextSizes); */

#endif
}
#endif

void gpECC_set_rng(gpECC_RNG_Function rng_function)
{
#ifdef GP_ECC_DIVERSITY_USE_EXT_LIB
    uECC_set_rng(rng_function);
#else
     /*uECC_set_rng(rng_function);*/
#endif
}

Int32 gpECC_valid_public_key(const UInt8 *public_key)
{
#ifdef GP_ECC_DIVERSITY_USE_EXT_LIB
    return uECC_valid_public_key(public_key, (uECC_Curve) gpECC_secp256r1());
#else
    /* return uECC_valid_public_key(public_key, (uECC_Curve) gpECC_secp256r1()); */

    return 0;
#endif
}
