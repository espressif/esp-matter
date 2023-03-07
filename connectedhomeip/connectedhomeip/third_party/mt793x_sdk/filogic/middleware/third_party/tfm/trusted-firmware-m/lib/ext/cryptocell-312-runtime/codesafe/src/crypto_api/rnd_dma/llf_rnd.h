/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LLF_RND_H
#define LLF_RND_H

#include "cc_rnd_local.h"

#ifdef __cplusplus
extern "C"
{
#endif



/************************ Defines ******************************/

/* Definitions describing the TRNG Entropy estimator parameters:
width of bits prefix and correlation table size */
#define CC_RND_nb   8
#define CC_RND_NB  (1 << CC_RND_nb)
#define H_BUFF_SIZE_WORDS  CC_RND_NB
#define EC_BUFF_SIZE_WORDS (CC_RND_NB/2)


/* macro for calculation max. allowed time for */
#define LLF_RND_CalcMaxTrngTime(ehrSamples, SubSamplingRatio) \
    (((ehrSamples) * LLF_RND_TRNG_MAX_TIME_COEFF * \
    LLF_RND_TRNG_VON_NEUMAN_COEFF * \
    LLF_RND_HW_TRNG_EHR_WIDTH_IN_BITS * \
    (SubSamplingRatio)) >> LLF_RND_TRNG_MAX_TIME_SCALE)

/* Macro defining Multiplication  using 16x16 multiplier  */
#define    Mult16x16(a, b) (((a)&0xffff)*((b)&0xffff))
uint64_t Mult32x32(uint32_t a, uint32_t b);
uint64_t Mult48x16(uint64_t a, uint32_t b);


/************************ Enums ********************************/
/************************ Typedefs  ****************************/
/************************ Structs  *****************************/
/* structure containing parameters and buffers for entropy estimator */
typedef struct
{
   /* estimated entropy size */
   uint32_t EstimEntropySizeBits;
   /* estimated error of entropy size */
   uint32_t EstimEntropySizeErrorInBits;

   /* special buffers */
   uint32_t h[CC_RND_NB];         /* histogram */
   uint32_t ec[CC_RND_NB/2];      /* equality counter for prefix */

}LLF_rnd_entr_estim_db_t;

/******************** Public Functions *************************/

/**
 * @brief The LLF_RND_GetRngBytes returns size of random source needed for collection
 *        required entropy .
 *
 *        The function returns size of source needed for required entropy.
 *
 * @param[in/out] trngParams - The pointer to structure, containing TRNG parameters.
 * @entropySizeWords[in/out] - The pointer to size of random source. The user sets
 *                    size of entropy that is required and the function returns
 *                    the actual size of source needed for this count of entropy.
 *
 * @return CCError_t - On success CC_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CCError_t LLF_RND_GetEntropySourceSize(
                                 CCRndParams_t    *trngParams, /*in*/
                                 uint16_t   *entropySizeWords_ptr);   /*in/out*/


/****************************************************************************************/
/**
 * @brief The function gets user provided parameters of RNG HW.
 *
 *   This implementation is in user competence. Temporary a pseudo function
 *   is implemented for testing goals. To use this implementation the user must define
 *   compilation flag "CC_RND_TEST_MODE", otherwise
 *
 *   Note: In temporary implementation assumed, that users parameters are placed
 *         in global structure UserRngParameters (now placed in ATP tests).
 *
 * @param[in] KeySizeWords - The key size: 4 or 8 words according to security
 *                           strength 128 bits or 256 bits;
 * @param[in] TrngMode -  TRNG mode: 0 - SWEE mode, 1 - FE mode.
 * @param[in] RoscsAllowed - Ring oscillator length level: should be set
 *            as 2 bits value: 0,1,2,3.
 * @param[in] SampleCount - The sampling count - count of RND blocks of RNG HW
 *            output, required for needed entropy accumulation:
 *              - in "fe" mode a possible values are 4095 to 65535, in steps of 4096;
 *              - in "swee" mode, sampling counter limit is set to a low value -
 *                typically 1 or 2.
 * @param[in] MaxTrngTimeCoeff - coefficient defining relation between maximal allowed and expected
 *                  time for random generation (in percents).
 *
 * @return CCError_t - CC_OK
 */
 CCError_t  LLF_RND_GetRngParams(
            uint32_t  *KeySizeWords,
            uint32_t  *TrngMode,
            uint32_t  *RoscsAllowed,
            uint32_t  *SampleCount,
            uint32_t  *MaxTrngTimeCoeff);


/************************************************************************************/
/**
 * @brief The LLF_RND_TurnOffTrng stops the hardware random bits collection
 *        closes RND clocks and releases HW semaphore.
 *
 *
 *
 * @return CCError_t - On success CC_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
void LLF_RND_TurnOffTrng(void);


CCError_t LLF_RND_GetFastestRosc(
                                         CCRndParams_t *trngParams_ptr,
                                         uint32_t *rosc_ptr    /*in/out*/);

CCError_t LLF_RND_GetRoscSampleCnt(
                     uint32_t rosc,
                     CCRndParams_t *pTrngParams);

CCError_t LLF_RND_WaitRngInterrupt(uint32_t *isr_ptr);

uint32_t LLF_RND_GetCountRoscs(
                                       uint32_t roscsAllowed,
                                       uint32_t roscToStart);

void LLF_RND_TurnOffTrng(void);

CCError_t LLF_RND_EntropyEstimateFull(
              uint32_t *ramAddr,      /*in*/
              uint32_t  blockSizeWords, /*in*/
              uint32_t  countBlocks,      /*in*/
              uint32_t *entrSize_ptr,     /*out*/
              uint32_t  *rndWorkBuff_ptr);   /*in*/

/**
* @brief: The function performs CPRNGT (Continued PRNG Test) according
*         to NIST 900-80 and FIPS (if defined) standards.
*
* @param[in] prev_ptr - The pointer to previous saved generated random
*                       value of size 16 bytes.
* @param[in] buff_ptr - The pointer to generated random buffer.
* @param[in] last_ptr - The pointer to last generated random block
*                       of size 16 bytes used for output last bytes.
* @param[in] countBlocks - The count of generated random blocks, including
*                          the last block. Assumed countBlocks > 0.
*
* @return CCError_t - On success CC_OK is returned, on failure a
*                        value MODULE_* as defined in cc_error.h
*/
CCError_t LLF_RND_RndCprngt(uint8_t            *prev_ptr,        /*in*/
                  uint8_t            *buff_ptr,        /*in*/
                  uint8_t            *last_ptr,        /*in*/
                  int32_t             countBlocks);   /*in*/




#ifdef __cplusplus
#endif

#endif
