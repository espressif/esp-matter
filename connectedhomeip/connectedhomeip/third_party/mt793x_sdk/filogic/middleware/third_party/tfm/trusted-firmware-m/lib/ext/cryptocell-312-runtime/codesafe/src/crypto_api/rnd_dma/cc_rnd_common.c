/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/************* Include Files ****************/

#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_pal_log.h"
#include "cc_rng_plat.h"
#include "cc_common.h"
#include "cc_common_math.h"
#include "cc_rnd_common.h"
#include "cc_rnd_error.h"
#include "cc_rnd_local.h"
#include "llf_rnd.h"
#include "llf_rnd_trng.h"
#include "llf_rnd_error.h"
#include "cc_pal_abort.h"
#include "cc_pal_mutex.h"
#include "cc_fips_defs.h"
#include "cc_util_pm.h"
#ifdef CC_IOT
#include "ctr_drbg.h"
#include "entropy.h"
#endif

/* CC RND module version compliant to NIST 800-90 standard. Based on CTR DRBG Block Cipher (AES) */

/************************ Defines ******************************/

/*********************************** Enums ******************************/

/*********************************Typedefs ******************************/
/* rotate 32-bits word by 16 bits */
#define RND_ROT32(x) ( (x) >> 16 | (x) << 16 )

/* inverse the bytes order in a word */
#define RND_REVERSE32(x)  ( ((RND_ROT32((x)) & 0xff00ff00UL) >> 8) | ((RND_ROT32((x)) & 0x00ff00ffUL) << 8) )

/**************** Global Data to be read by RNG function ****************/

/************************************************************************************/
/***********************           Private functions            *********************/
/************************************************************************************/

/********************************************************************************
  @brief This function subtracts a value from a large vector presented in the buffer.
         The LSB of the counter is stored in the left most cell.

  @return signed value of carry (borrow)
 */

static uint8_t AddInt8ValueToUin8Vector(uint8_t  *vect, /*! [in]  vect - the buffer containing the vector. */
                    int8_t    val,  /*! [in]  val  - the value to add/subtract (according to its sign). */
                    uint32_t  vectSizeInBytes) /*! [in]  vectSizeInBytes - the vector size in bytes. */
{
        /* DECLARATIONS */

        /* loop index */
    uint32_t i;
    int32_t temp;

    /* FUNCTION LOGIC */

    temp = val;

        for (i = 0; i < vectSizeInBytes; i++) {
                temp = vect[i] + val;
                vect[i] = (uint32_t)temp & 0xFF;
                val = (temp >> 8) & 0xFF;
    }

    return val;

}/* End of AddInt8ValueToUin8Vector() */





/****************************************************************************************/
/*****************************       Public Functions      ******************************/
/****************************************************************************************/


/****************************************************************************************/
/**

  @brief The function set the RND Generate vector function, provided by the User.

  @param [in/out] rndContext_ptr  - Pointer to the RND context buffer.
  @param [in] rndGenerateVectFunc - The pointer to RND Generate vector function.

  @return CCError_t - no return value
 */
CCError_t CC_RndSetGenerateVectorFunc(CCRndContext_t *rndContext_ptr,
                       CCRndGenerateVectWorkFunc_t rndGenerateVectFunc)
{

    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /* check parameters */
        if (rndContext_ptr == NULL)
                return CC_RND_CONTEXT_PTR_INVALID_ERROR;
        if (rndGenerateVectFunc == NULL)
                return CC_RND_GEN_VECTOR_FUNC_ERROR;


        rndContext_ptr->rndGenerateVectFunc = rndGenerateVectFunc;

        return 0;
}






#ifndef _INTERNAL_CC_ONE_SEED


/**********************************************************************************************************/
/**
  @brief The function generates a random vector Rand in range  1 < RandVect < MaxVect
              by testing candidates (described and used in FIPS 186-4: B.1.2, B.4.2 etc.):
         The function performs the following:
         1.  Check input parameters, in partial, check that value of max. vector > 3 (our requirement).
         2.  If maxVect != 0 (maxVect is provided), then calculate required size of random
             equaled to actual bit size of MaxVector, else set it = rndSizeInBits.
         3.  Calls the CC_RndGenerateVector() function for generating random vector
             RndVect of required size.
         4.  If maxVect is provided, then:
              4.0. Subtract maxVect  -= 2;
              4.1. Sets all high bits of RndVect, greatest than MSBit of MaxVector, to 0.
              4.2. If size of random vector > 16 bytes, then:
                      4.2.1. Compares high 16 bytes of randVect to maxVect.
                      4.2.2. If condition is not satisfied, then generate new high 16 bytes
                             of rndVect and go to step 4.2.1.
              4.3. Compare the full RndVect with MaxVector. If condition is not satisfied,
                   then generate new random RndVect and go to step 4.1, else go to 6.
         5. Else if maxVect is not provided, then set MSBit of rndVect to 1.
         6. Output the result and Exit.

          Note: Random and Max vectors are given as sequence of bytes, where LSB is most left byte
                and MSB = most right one.

  @param rndContext_ptr [in/out] - Pointer to the RND context buffer.
  @param rndSizeInBits [in]   - If maxVect_ptr is not given, then rndSizeInBits defining the exact size (in bits)
                         of generated random vector. If maxVect is given, then it defines the
                      size (rounded up to bytes) of the maxVect_ptr buffer.
  @param maxVect_ptr [in]     - The pointer to vector defining a high limit
                         of random vector.
  @param rndVect_ptr [in,out] - The output buffer for the random vector.

  @return CCError_t  - On success CC_OK is returned, on failure - a value,
              defined in cc_rnd_error.h.
 */
CEXPORT_C CCError_t CC_RndGenerateVectorInRange(
                                                    CCRndContext_t *rndContext_ptr,
                                                    size_t   rndSizeInBits,
                                                    uint8_t  *maxVect_ptr,
                                                    uint8_t  *rndVect_ptr )
{
        /* FUNCTION DECLARATIONS */

        CCError_t Error = CC_OK;
        int32_t   k, extraBytes;
        int8_t    shift;
        uint8_t   mask;
        uint32_t   rndSizeInBytes, checkingSizeBytes = 0;
        uint32_t   maxVectSizeBits;
        uint32_t   maxVectSizeBytes = 0;
        CCCommonCmpCounter_t CompRes;
        /* RND state and function pointers */
        CCRndState_t   *rndState_ptr;
        CCRndGenerateVectWorkFunc_t RndGenerateVectFunc;

        /* FUNCTION LOGIC */
    CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /*  Check input parameters */
        if (rndVect_ptr == NULL)
                return CC_RND_VECTOR_OUT_PTR_ERROR;

    /* verify that rndSizeInBits is not greater than 2^19 -1 */
    if (rndSizeInBits > 0x7FFFF)
        return CC_RND_VECTOR_OUT_SIZE_ERROR;

    /* given size of random vector in bytes */
    rndSizeInBytes = CALC_FULL_BYTES(rndSizeInBits);

    if (rndSizeInBits <= 1 || (uint32_t)rndSizeInBytes > CC_RND_MAX_GEN_VECTOR_SIZE_BYTES)
            return CC_RND_VECTOR_OUT_SIZE_ERROR;

    /* check parameters */
    if (rndContext_ptr == NULL)
            return CC_RND_CONTEXT_PTR_INVALID_ERROR;

    rndState_ptr = (CCRndState_t *)(rndContext_ptr->rndState);
    RndGenerateVectFunc = rndContext_ptr->rndGenerateVectFunc;

    if (RndGenerateVectFunc == NULL)
            return CC_RND_GEN_VECTOR_FUNC_ERROR;


    /*--------------------------------------*/
    /* generation in case of exact bit size */
    /*--------------------------------------*/

    if (maxVect_ptr == NULL) {

            Error = RndGenerateVectFunc((void *)rndState_ptr, (unsigned char *)rndVect_ptr, (size_t)rndSizeInBytes);

            if (Error != CC_OK)
                    goto End;

            /* swap from big endian to little*/
            CC_CommonReverseMemcpy(rndVect_ptr, rndVect_ptr, rndSizeInBytes);

            /* correction of bit size */
            rndVect_ptr[rndSizeInBytes-1] |= 0x80;
            if (rndSizeInBits%8 != 0) {
                    rndVect_ptr[rndSizeInBytes-1] >>= (8 - (rndSizeInBits&0x7))&0x7;
            }

            goto End;
        }

        /*------------------------------------------*/
        /* generation in case of given max. vector  */
        /*------------------------------------------*/

        /* calculate actual size of MaxVector in bits*/
        maxVectSizeBits = CC_CommonGetBytesCounterEffectiveSizeInBits(
                                                                        maxVect_ptr, (uint16_t)rndSizeInBytes);
        /* if maxVect < 4 then return an error */
        if (maxVectSizeBits < 3 || (maxVectSizeBits == 3 && maxVect_ptr[0] < 4)) {
                Error = CC_RND_MAX_VECTOR_IS_TOO_SMALL_ERROR;
        goto End;
    }

        /* temporary subtract 2 from maxVect */
        AddInt8ValueToUin8Vector(maxVect_ptr, -2/*val*/, maxVectSizeBytes);

        maxVectSizeBytes = CALC_FULL_BYTES(maxVectSizeBits);

        /* calculate count of extra 0-bytes in maxVector */
        extraBytes = rndSizeInBytes - maxVectSizeBytes;

        /* zeroing 0-bytes in rndVect_ptr buffer */
        CC_PalMemSetZero(rndVect_ptr + maxVectSizeBytes, extraBytes);

        /* calc. intermediate checking size */
        if ((uint32_t)maxVectSizeBytes > CC_AES_BLOCK_SIZE_IN_BYTES) {
                checkingSizeBytes = CC_AES_BLOCK_SIZE_IN_BYTES;
        } else {
                checkingSizeBytes = maxVectSizeBytes;
        }

        /* calculate count of extra 0-bits for mask shifting */
        shift = (int8_t)(8 - (maxVectSizeBits & 7))&7;
        mask = 0xFF >> shift;

        /* main loop for generating random number    */
        /*-------------------------------------------*/
        k = 0;

        while (k < 0xFFFF) {
                /* generate full size random vector */
                Error = RndGenerateVectFunc((void *)rndState_ptr, (unsigned char *)rndVect_ptr, (size_t)maxVectSizeBytes);

                if (Error != CC_OK)
                        goto End;

                /* swap from big endian to little*/
                CC_CommonReverseMemcpy(rndVect_ptr, rndVect_ptr, maxVectSizeBytes);

                /* mask the non significant high bits */
                rndVect_ptr[maxVectSizeBytes - 1] &= mask;

                /* step1 check high part of random */
                if (checkingSizeBytes == CC_AES_BLOCK_SIZE_IN_BYTES) {
                        while (1) {
                                CompRes = CC_CommonCmpLsbUnsignedCounters(
                                                                            rndVect_ptr + maxVectSizeBytes - CC_AES_BLOCK_SIZE_IN_BYTES,
                                                                            CC_AES_BLOCK_SIZE_IN_BYTES,
                                                                            maxVect_ptr + maxVectSizeBytes - CC_AES_BLOCK_SIZE_IN_BYTES,
                                                                            CC_AES_BLOCK_SIZE_IN_BYTES);

                                if (CompRes == CC_COMMON_CmpCounter2GreaterThenCounter1) {
                                        goto End;  /* random is found */
                                } else if (CompRes == CC_COMMON_CmpCounter1AndCounter2AreIdentical)
                                        break; /* go to check full size */

                                /* generate new 16 random high bytes - without*
                                *  repeat the same Additional Data            */
                                Error = RndGenerateVectFunc((void *)rndState_ptr,
                                                            (unsigned char *)rndVect_ptr + maxVectSizeBytes - CC_AES_BLOCK_SIZE_IN_BYTES,
                                                            CC_AES_BLOCK_SIZE_IN_BYTES);
                                if (Error != CC_OK)
                                        goto End;

                                /* mask the non significant high bits */
                                rndVect_ptr[maxVectSizeBytes - 1] &= mask;
                        }
                }

                /* check full size relating to max vector */
                CompRes = CC_CommonCmpLsbUnsignedCounters(rndVect_ptr, (uint16_t)maxVectSizeBytes,
                                                             maxVect_ptr, (uint16_t)maxVectSizeBytes);

                if (CompRes == CC_COMMON_CmpCounter2GreaterThenCounter1) {
                        goto End;
                }

                /* increment counter and continue the loop */
                k++;
        }

        /* if all tries are Fail, then return the Error */
        Error = CC_RND_CAN_NOT_GENERATE_RAND_IN_RANGE;

End:

    if (Error != CC_OK) {
        CC_PalMemSetZero(rndVect_ptr, rndSizeInBytes);
                return Error;
    }


        if (maxVect_ptr != NULL) {
            /* reset the maxVect to (original value -1) */
            AddInt8ValueToUin8Vector(maxVect_ptr, 1/*val*/, maxVectSizeBytes);

            if (CC_CommonGetBytesCounterEffectiveSizeInBits(rndVect_ptr, maxVectSizeBytes) < 2) { /* vect == 1*/
                /* replace 1 with(maxVect -1) */
                    CC_PalMemCopy(rndVect_ptr, maxVect_ptr, maxVectSizeBytes);
            }
            /* reset the maxVect to original value */
            AddInt8ValueToUin8Vector(maxVect_ptr, 1/*val*/, maxVectSizeBytes);

        }

        return Error;

} /* End of CC_RndGenerateVectorInRange function */

#endif /*_INTERNAL_CC_ONE_SEED*/


/**********************************************************************************************************/
/**
 * @brief The RndGenerateWordsArrayInRange function generates a random words vector in range:
 *            1 < RndVect < MaxVect,   using the FIPS-PUB 186-2 standard appendix 3 :
 *
 *        The function generates random array  using CC_RndGenerateVectorInRange function and
 *        conversion of bytes to words.
 *
 *         Note: RndVect and MaxVect arrayss are given as sequence of words, where LSWord is most left byte
 *               and MSWord - most right.
 *
 * @param rndContext_ptr [in/out]  - Pointer to the RND context buffer.
 * @param rndSizeInBits [in]   - If maxVect_ptr is not given, then rndSizeInBits defining the exact size (in bits)
 *                        of generated random vector. If maxVect is given, then it defines the
 *                    size (rounded up to words) of the maxVect_ptr buffer. The size must be not greate
 *                than CC_RND_MAX_SIZE_OF_OUTPUT_BYTES/4
 * @param maxVect_ptr [in]     - The pointer to vector defining a high limit of random vector.
 * @param rndVect_ptr [out]    - The output buffer for the random vector.
 * @param tmp_ptr [int]        - The temp buffer for the random generation. The size must be not
 *                less, than rndSizeInBits converted to words (rounded up).
 *
 * @return CCError_t  - On success CC_OK is returned, on failure - a value,
 *                defined in cc_rnd_error.h.
 */
CCError_t RndGenerateWordsArrayInRange(CCRndContext_t *rndContext_ptr,
                                       uint32_t   rndSizeInBits,
                                       uint32_t  *maxVect_ptr,
                                       uint32_t  *rndVect_ptr,
                                       uint32_t  *tmp_ptr)
{
        /* FUNCTION DECLARATIONS */

        CCError_t err;
        uint32_t rndSizeInWords = CALC_FULL_32BIT_WORDS(rndSizeInBits);


        /* check parameters */
        if (rndVect_ptr == NULL)
                return CC_RND_DATA_OUT_POINTER_INVALID_ERROR;

        /* given size of random vector in bytes */
        if (rndSizeInBits == 0 || rndSizeInWords*4 > CC_RND_MAX_GEN_VECTOR_SIZE_BYTES)
                return CC_RND_VECTOR_SIZE_ERROR;

        /* copy the maxVector into temp buffer and set endiannes as LE bytes  *
        *  array                                  */
        CC_PalMemMove((uint8_t*)tmp_ptr, (uint8_t*)maxVect_ptr, rndSizeInWords*sizeof(uint32_t));

#ifdef BIG__ENDIAN
        CC_COMMON_INVERSE_UINT32_IN_ARRAY(tmp_ptr, rndSizeInWords);
#endif
        /* generate vector in range [1...MaxVect] as LE bytes array */
        rndVect_ptr[rndSizeInWords-1] = 0;
        err = CC_RndGenerateVectorInRange(rndContext_ptr, rndSizeInBits, (uint8_t*)tmp_ptr, (uint8_t*)rndVect_ptr);

        if (err)
                return err;

        /* set endianness in output words according to LE words array */
#ifdef BIG__ENDIAN
        CC_COMMON_INVERSE_UINT32_IN_ARRAY(rndVect_ptr, rndSizeInWords);
#endif

        return err;
}



