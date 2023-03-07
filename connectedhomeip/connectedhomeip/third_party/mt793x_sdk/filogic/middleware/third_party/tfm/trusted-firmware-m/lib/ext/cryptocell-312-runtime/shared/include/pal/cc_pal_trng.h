/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_pal_trng
 @{
 */

/*!
 @file
 @brief This file contains APIs for retrieving TRNG user parameters.
 */

#ifndef _CC_PAL_TRNG_H
#define _CC_PAL_TRNG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_types.h"

#if (CC_CONFIG_TRNG_MODE==1)
/*!
 @brief The random-generator parameters of CryptoCell for TRNG mode.

 This is as defined in <em>NIST SP 90B: Recommendation for
 the Entropy Sources Used for Random Bit Generation</em>.
 */
typedef struct  CC_PalTrngModeParams_t
{
    /*! The amount of bytes for the required entropy bits. It is calculated as
    ROUND_UP(ROUND_UP(((required entropy bits)/(entropy per bit)), 1024),
    (EHR width in bytes)) / 8.
    The 1024 bits is the multiple of the window size. The multiple of the EHR
    width, which is 192 bits. */
    uint32_t  numOfBytes;
    /*!  The repetition counter cutoff, as defined in <em>NIST SP 90B:
    Recommendation for the Entropy Sources Used for Random Bit
    Generation</em>, section 4.4.1.
    This is calculated as C = ROUND_UP(1+(-log(W)/H)), W = 2^(-40),
    H=(entropy per bit). */
    uint32_t  repetitionCounterCutoff;
    /*!  The adaptive proportion cutoff, as defined in <em>NIST SP 90B:
    Recommendation for the Entropy Sources Used for Random Bit
    Generation</em>, section 4.4.2.
    This is calculated as C =CRITBINOM(W, power(2,(-H)),1-a), W = 1024,
    a = 2^(-40), H=(entropy per bit). */
    uint32_t  adaptiveProportionCutOff;

} CC_PalTrngModeParams_t;
#endif

/*! Definition for the structure of the random-generator parameters
    of CryptoCell, containing the user-given parameters. */
typedef struct  CC_PalTrngParams_t
{
    /*! The sampling ratio of ROSC #1.*/
    uint32_t  SubSamplingRatio1;
    /*! The sampling ratio of ROSC #2.*/
    uint32_t  SubSamplingRatio2;
    /*! The sampling ratio of ROSC #3.*/
    uint32_t  SubSamplingRatio3;
    /*! The sampling ratio of ROSC #4.*/
    uint32_t  SubSamplingRatio4;
#if (CC_CONFIG_TRNG_MODE==1)
    /*! Specific parameters of the TRNG mode.*/
    CC_PalTrngModeParams_t   trngModeParams;
#endif
} CC_PalTrngParams_t;

/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*!
  @brief This function returns the TRNG user parameters.

  @return \c 0 on success.
  @return A non-zero value on failure.
 */
CCError_t CC_PalTrngParamGet(
        /*! [out] A pointer to the TRNG user parameters. */
        CC_PalTrngParams_t *pTrngParams,
        /*! [in/out] A pointer to the size of the TRNG-user-parameters
        structure used. Input: the function must verify its size is the
        same as #CC_PalTrngParams_t. Output: the function returns the size
        of #CC_PalTrngParams_t for library-size verification. */
        size_t *pParamsSize
                             );

#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif


