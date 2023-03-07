/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



/************* Include Files ****************/
#include "cc_pal_types.h"
#include "cc_pal_trng.h"
#include "cc_pal_log.h"

#if (CC_CONFIG_TRNG_MODE==0)
#define CC_CONFIG_SAMPLE_CNT_ROSC_1     5000
#define CC_CONFIG_SAMPLE_CNT_ROSC_2     1000
#define CC_CONFIG_SAMPLE_CNT_ROSC_3     500
#define CC_CONFIG_SAMPLE_CNT_ROSC_4     0
#elif (CC_CONFIG_TRNG_MODE==1)
/* amount of bytes for the required entropy bits = ROUND_UP(ROUND_UP(((required entropy bits)/(entropy per bit)), 1024), (EHR width in bytes)) / 8
   (multiple of the window size 1024 bits and multiple of the EHR width 192 bits) */
#define CC_CONFIG_TRNG90B_AMOUNT_OF_BYTES                      144  /* ROUND_UP(ROUND_UP((384/0.5), 1024), 192) / 8 = 144 */

/*** NIST SP 800-90B (2nd Draft) 4.4.1 ***/
/* C = ROUND_UP(1+(-log(W)/H)), W = 2^(-40), H=(entropy per bit) */
#define CC_CONFIG_TRNG90B_REPETITION_COUNTER_CUTOFF            81  /* ROUND_UP(1+(40/0.5)) = 81 */

/*** NIST SP 800-90B (2nd Draft) 4.4.2 ***/
/* C =CRITBINOM(W, power(2,(-H)),1-a), W = 1024, a = 2^(-40), H=(entropy per bit) */
#define CC_CONFIG_TRNG90B_ADAPTIVE_PROPORTION_CUTOFF           823      /* =CRITBINOM(1024, power(2,(-0.5)),1-2^(-40)) */

/* sample count for each ring oscillator */
/* for unallowed rosc, sample count = 0 */
#define CC_CONFIG_SAMPLE_CNT_ROSC_1     1000
#define CC_CONFIG_SAMPLE_CNT_ROSC_2     1000
#define CC_CONFIG_SAMPLE_CNT_ROSC_3     500
#define CC_CONFIG_SAMPLE_CNT_ROSC_4     0
#else
#error "CC_CONFIG_TRNG_MODE not defined or not supported"
#endif


/**
 * @brief This function return the TRNG user parameters.
 *
 *
 * @return Zero on success.
 * @return A non-zero value on failure.
 */
CCError_t CC_PalTrngParamGet(CC_PalTrngParams_t *pTrngParams, /*!< [out] A pointer to the TRNG user parameters. */
                             size_t *pParamsSize)     /*!< [in/out] A poiinter to size of the TRNG user parameters structure used.
                                                           As input: the function needs to verify its size is the same as CC_PalTrngParams_t.
                                                           As output: return the size of CC_PalTrngParams_t for Library size verification */
{
    CCError_t  error = CC_OK;

    if ((pTrngParams == NULL) ||
             (pParamsSize == NULL) ||
             (*pParamsSize != sizeof(CC_PalTrngParams_t))){
            return CC_FAIL;
    }

    *pParamsSize = sizeof(CC_PalTrngParams_t);

    pTrngParams->SubSamplingRatio1 = CC_CONFIG_SAMPLE_CNT_ROSC_1;
    pTrngParams->SubSamplingRatio2 = CC_CONFIG_SAMPLE_CNT_ROSC_2;
    pTrngParams->SubSamplingRatio3 = CC_CONFIG_SAMPLE_CNT_ROSC_3;
    pTrngParams->SubSamplingRatio4 = CC_CONFIG_SAMPLE_CNT_ROSC_4;

#if (CC_CONFIG_TRNG_MODE==1)
    pTrngParams->trngModeParams.numOfBytes = CC_CONFIG_TRNG90B_AMOUNT_OF_BYTES;
    pTrngParams->trngModeParams.repetitionCounterCutoff = CC_CONFIG_TRNG90B_REPETITION_COUNTER_CUTOFF;
    pTrngParams->trngModeParams.adaptiveProportionCutOff = CC_CONFIG_TRNG90B_ADAPTIVE_PROPORTION_CUTOFF;

#endif
    return error;
}
