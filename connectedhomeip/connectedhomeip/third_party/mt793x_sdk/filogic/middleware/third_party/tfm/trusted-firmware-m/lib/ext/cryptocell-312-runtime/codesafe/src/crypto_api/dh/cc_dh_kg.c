/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/************* Include Files ****************/
#include "cc_pal_mem.h"
#include "cc_rnd_common.h"
#include "cc_common.h"
#include "cc_common_math.h"
#include "cc_hash_defs.h"
#include "pki.h"
#include "rsa.h"
#include "rsa_public.h"
#include "rsa_private.h"
#include "cc_dh_error.h"
#include "cc_dh.h"
#include "cc_dh_kg.h"
#include "cc_rnd_error.h"
#include "cc_fips_defs.h"


/************************ Defines *******************************/

/************************ Enums *********************************/

/************************ macros ********************************/

/** @brief This macro is required to remove compilers warnings if the HASH or PKI is not supported */


/*********************** Global data  ***************************/

/************* External functions prototypes  *****************************/

CCError_t RndGenerateWordsArrayInRange(CCRndContext_t *rndContext_ptr,
                                              uint32_t   rndSizeInBits,
                                              uint32_t  *maxVect_ptr,
                                              uint32_t  *rndVect_ptr,
                                              uint32_t  *tmp_ptr);

/******************************************************************************************/
/************************         Private Functions          ******************************/
/******************************************************************************************/

/********************************************************************************/
/**
 *      The function adds value to the number N, presented as bytes array in the buffer,
 *      given by uint32_t pointer n_ptr, where MSbyte is a most left one.
 *
 *      Algorithm:
 *          n = (N + val) mod 2^(8*sizeBytes).
 *      Assumed: The array and its size are aligned to 32-bit words.
 *           val > 0.
 *
 * @author reuvenl (7/1/2012)
 *
 * @param n_ptr
 * @param sizeBytes
 * @param val - value to add
 *
 * @return carry from last addition
 */
static uint32_t DhKgAddValueToMsbLsbBytesArray(uint32_t *arr_ptr, uint32_t val, uint32_t sizeBytes)
{
        int32_t i;
        uint32_t *ptr = (uint32_t*)arr_ptr;
        uint32_t tmp, curr;

        for (i = sizeBytes/CC_32BIT_WORD_SIZE - 1; i >= 0; i--) {

#ifndef BIG__ENDIAN
                tmp = curr = CC_COMMON_REVERSE32(ptr[i]);
#else
                tmp = curr = ptr[i];
#endif
                tmp += val;

#ifndef BIG__ENDIAN
                ptr[i] = CC_COMMON_REVERSE32(tmp);
#else
                ptr[i] = tmp;
#endif

                if (tmp < curr) {
                        val = 1;
                } else {
                        val = 0;
                }
        }

        return val; /* carry */
}


/********************************************************************************/
/**
 * @brief This function returns the effective size in bits of the MSB bytes array.
 *
 *        Assumed, that MSB > 0 is stored in the most left cell in the array.
 *
 * @param[in] arr_ptr -  The counter buffer.
 * @param[in] sizeInBytes -  The counter size in bytes.
 *
 * @return result - The effective counters size in bits.
 */

static uint32_t DhKgGetSizeInBitsOfMsbLsbBytesArray(uint8_t  *arr_ptr,
                                                         uint32_t  sizeInBytes)
{
        /* FUNCTION LOCAL DECLERATIONS */

        /* loop variable */
        int32_t i;

        /* the effective size in bits */
        uint32_t sizeInBits = 8 * sizeInBytes;

        /* the effective MS byte */
        uint8_t msbVal = arr_ptr[0], mask = 0x80;

        /* FUNCTION LOGIC */

        /* adjusting the effective size in bits */
        for (i = 0; i < 8 ; i++) {
                /* if the MS bit is set exit the loop */
                if (msbVal & mask) {
                        break;
                }

                sizeInBits--;

                mask >>= 1;

        }

        return sizeInBits;

}/* END OF  DhKgGetSizeInBitsOfMsbLsbBytesArray */



/** @brief The function finds prime number Q for key generation according to X9.42-2001.
 *
 *
 * @param[in]  rndContext_ptr     - Pointer to the RND context buffer.
 * @param[in]  QSizeBits          - The size of order of generator in bits. According to ANSI X9.42:
 *                                  m must be multiple of 32 bits and 160 <= m. According to ANSI X9.30-1:
 *                                  m = 160 bit. We allow using Q as multiplies of 32 in range 160 - 256 bits (see
 *                                  FIPS 186-4 Tab. C.1).
 * @param[in]  seedSizeBits      - The  seed size in bits.
 * @param[in]  generateSeed       - The  flag defining whether the seed to be generated (1) or not (0),
 * @param[out] Q_ptr              - The pointer to the order Q of generator. The buffer must be aligned to 4 bytes.
 *                                  Note: The order Q is set as Words array, where LSWord is left most.
 * @param[out] S_ptr              - The random seed used for generation of primes. The buffer must be aligned to 4 bytes.
 *                                  Note: The seed is set in the buffer as BE bytes array.
 * @param[in]  TempBuff1_ptr      - The temp buffer of size not less than max modulus size, aligned to 4 bytes.
 * @param[in]  TempBuff2_ptr      - The temp buffer of size not less than max
 *                                  modulus size, aligned to 4 bytes.
 * @param[in]  TempBuff3_ptr      - The large temp buffer (aligned to 4 bytes) of size:
 *                                    - on HW platform not less than 8*CC_DH_MAX_MOD_BUFFER_SIZE_IN_WORDS.
 *                                    - on SW platform not less than 41*CC_DH_MAX_MOD_BUFFER_SIZE_IN_WORDS.
 *
 *   Note: The function is static, sizes of its input arrays (mod, ord, seed) are checked in
 *         caller functions and don't need to be chcked again.
 *
 * @return CCError_t - On success CC_OK is returned, on failure a predefined error code.
 *
 *
 */
static CCError_t DhX942FindPrimeQ(
                                        CCRndContext_t *rndContext_ptr,
                                        uint32_t          QsizeBits,           /*in */
                                        uint32_t          seedSizeBits,        /*in */
                                        uint32_t          generateSeed,        /*in */
                                        uint32_t         *Q_ptr,               /*out*/
                                        uint8_t          *S_ptr,               /*out*/
                                        uint32_t         *TempBuff1_ptr,       /*in */
                                        uint32_t         *TempBuff2_ptr,       /*in - large buffer*/
                                        uint32_t         *TempBuff3_ptr )      /*in*/
{

        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t Error;

        /* size of order in 160-bit blocks: M1 */
        uint32_t  M1;

        /* primality flag (if prime, then isPrime = 1, else 0 ) */
        int8_t  isPrime;

        /* flag of first hash calculating */
        uint8_t  isFirst = 1;


        /* HASH input and result pointers */
        uint32_t  *hashDataIn1_ptr, *hashDataIn2_ptr;
        CCHashResultBuf_t   *hashRes1_ptr, *hashRes2_ptr;

        /* current data pointer and sizes */
        uint8_t  *current_ptr;

        /* order size in bytes and in words */
        uint32_t  QsizeBytes;

        /* exact seed size in bits and in words */
        uint32_t  seedSizeBytes, remainingSize;

        /* shift value (in bits) for adding counter to seed */
        uint32_t shift;
        uint8_t  mask, mask1;

        /* loop counters */
        uint32_t i, j;
        uint32_t countMilRabTests;


        CCRndState_t   *rndState_ptr;
        CCRndGenerateVectWorkFunc_t RndGenerateVectFunc;

        /* FUNCTION  LOGIC */

        Error = CC_OK;

        /* check parameters */
        if (rndContext_ptr == NULL)
                return CC_RND_CONTEXT_PTR_INVALID_ERROR;
        if (rndContext_ptr->rndGenerateVectFunc == NULL)
                return CC_RND_GEN_VECTOR_FUNC_ERROR;

        rndState_ptr = (CCRndState_t *)(rndContext_ptr->rndState);
        RndGenerateVectFunc = rndContext_ptr->rndGenerateVectFunc;


        /* Step 1. Check input parameters */
        /*------------------------------- */

        /* check pointers: modP, generator and tempBuff. Note: other pointers may be NULL  */
        if (Q_ptr == NULL ||
            S_ptr == NULL ||
            TempBuff1_ptr == NULL ||
            TempBuff2_ptr == NULL ||
            TempBuff3_ptr == NULL) {
                return CC_DH_INVALID_ARGUMENT_POINTER_ERROR;
        }

        /* --------------------------------- */
        /*  Step 2.  Initializations         */
        /* --------------------------------- */

        /* order and seed sizes */
        QsizeBytes = CALC_FULL_BYTES(QsizeBits);
        seedSizeBytes = CALC_FULL_BYTES(seedSizeBits);

        /* order size M1 in 160-bit blocks (rounded up) */
        M1 = (QsizeBits + CC_DH_SEED_MIN_SIZE_IN_BITS - 1) / CC_DH_SEED_MIN_SIZE_IN_BITS;

        /* if M1 > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS / CC_DH_SEED_MIN_SIZE_IN_BITS,
        *  then return error. This checking is for preventing KW warnings   */
        if (M1 > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS / CC_DH_SEED_MIN_SIZE_IN_BITS) {
                return CC_DH_INVALID_ORDER_SIZE_ERROR;
        }

        /* RL  seed size must allow adding counters for hashing without overflow of temp buffers:*/
        /* we limit this size relating to max buffer */
        if (seedSizeBytes > (CC_DH_MAX_MOD_SIZE_IN_WORDS - 1) * sizeof(uint32_t))
                return CC_DH_INVALID_SEED_SIZE_ERROR;

        /* zeroing  Q buffer */
        CC_PalMemSetZero(Q_ptr, QsizeBytes);

        /* set HASH pointers to temp buffer */
        hashDataIn1_ptr = TempBuff1_ptr;
        hashDataIn2_ptr = TempBuff2_ptr;
        hashRes1_ptr = (CCHashResultBuf_t*)TempBuff3_ptr;
        hashRes2_ptr = hashRes1_ptr + 1;

        /*------------------------------- */
        /* Step 3. Create random prime Q  */
        /*------------------------------- */

        /* check size and copy seed S into HASH input buffers */
        if (generateSeed != 1) {
                if (seedSizeBits != DhKgGetSizeInBitsOfMsbLsbBytesArray(S_ptr, seedSizeBytes))
                        return CC_DH_INVALID_SEED_SIZE_ERROR;

                /* check that (seed + DH_MAX_HASH_COUNTER_VALUE) is less than
                   2^seedSizeBits, i.e. prevent addition overflow in
                   generation process */
                CC_PalMemCopy((uint8_t *)hashDataIn2_ptr, S_ptr, seedSizeBytes);
                if (DhKgAddValueToMsbLsbBytesArray(hashDataIn2_ptr, DH_SEED_MAX_ADDING_VAL, seedSizeBytes) != 0)
                        return CC_DH_PASSED_INVALID_SEED_ERROR;
        }

        /* shift value to bit position of MSbit of the seed  */
        shift = 8*seedSizeBytes - seedSizeBits;
        mask = 0xFF >> shift;
        mask1 = 0x80 >> shift;

        /* initialize isPrime, orderSizeInBlocks, and Q buffer */
        isPrime = CC_FALSE;

        /* set count of M-R tests for Q sccording FIPS 186-4 C.3: Tab. C.1. */
        if (QsizeBits <= 160) {
                countMilRabTests = 19;
        } else if (QsizeBits <= 224) {
                countMilRabTests = 24;
        } else if (QsizeBits <= 256) {
                countMilRabTests = 27;
        } else {
                countMilRabTests = 28;
        }

        /* Step 3.1. Try Q candidates     */
        /*--------------------------------*/
        while (isPrime != CC_TRUE) {

                uint32_t isSeedValid = 0;

                /* Step 3.1.1. Create random seed  S  */
                if (generateSeed == 1) {
                        /* generation of random vector */
                        while (isSeedValid == 0) {
                                Error = RndGenerateVectFunc((void *)rndState_ptr, (unsigned char *)S_ptr, (size_t)seedSizeBytes);

                                if (Error != CC_OK) {
                                        goto EndWithError;
                                }

                                /* Set the MS bit of S and provide exact size of seed in bits */
                                S_ptr[0] = (S_ptr[0] & mask) | mask1;

                                /* check that (seed + DH_MAX_HASH_COUNTER_VALUE) is less than
                                   2^seedSizeBits, i.e. prevent addition overflow */
                                CC_PalMemCopy((uint8_t *)hashDataIn2_ptr, S_ptr, seedSizeBytes);
                                if (DhKgAddValueToMsbLsbBytesArray(hashDataIn2_ptr, DH_SEED_MAX_ADDING_VAL, seedSizeBytes) == 0)
                                        isSeedValid = 1;
                        }

                } else if (isFirst == 0) {
                        return  CC_DH_PASSED_INVALID_SEED_ERROR;
                }



                /* copy seed into hashDataIn1/2 buffers */
                CC_PalMemCopy((uint8_t *)hashDataIn1_ptr, S_ptr, seedSizeBytes);
                CC_PalMemCopy((uint8_t *)hashDataIn2_ptr, S_ptr, seedSizeBytes);
                DhKgAddValueToMsbLsbBytesArray(hashDataIn2_ptr, M1, seedSizeBytes);

                /* set current pointer and size for copying HASH results into *
                *  TempBuff3 as big endian bytes                  */
                current_ptr = &((uint8_t*)Q_ptr)[QsizeBytes - CC_HASH_SHA1_DIGEST_SIZE_IN_BYTES];
                remainingSize = QsizeBytes;

                /* Step 3.1.2. Create Q candidate:  For i=0 to M1 do:
                Q = Q + (SHA1(S+i) XOR SHA1(S+M1+i))*(2^(160*i)) */
                for (i = 0; i < M1; i++) {
                        if (i != 0) {
                                /* increment hashDataIn1 and hashDataIn2 by 1 *
                                *  starting from second cycle             */
                                DhKgAddValueToMsbLsbBytesArray(hashDataIn1_ptr, 1, seedSizeBytes);
                                DhKgAddValueToMsbLsbBytesArray(hashDataIn2_ptr, 1, seedSizeBytes);
                        }

                        /* calculate first HASH result */
                        Error = CC_Hash(
                                         CC_HASH_SHA1_mode,
                                         (uint8_t *)hashDataIn1_ptr,
                                         seedSizeBytes,
                                         *hashRes1_ptr );

                        if (Error != CC_OK) {
                                goto EndWithError;
                        }


                        /* calculate  second HASH result */
                        Error = CC_Hash(
                                         CC_HASH_SHA1_mode,
                                         (uint8_t*)hashDataIn2_ptr,
                                         seedSizeBytes,
                                         *hashRes2_ptr);

                        if (Error != CC_OK) {
                                goto EndWithError;
                        }

                        /* XOR HASH results */
                        for (j = 0; j < CC_HASH_SHA1_DIGEST_SIZE_IN_WORDS; j++) {
                                (*hashRes1_ptr)[j] ^= (*hashRes2_ptr)[j];
                        }

                        /* copying HASH results into Q buffer */
                        if (remainingSize >= CC_DH_SEED_MIN_SIZE_IN_BYTES) {
                                CC_PalMemCopy(current_ptr, hashRes1_ptr, CC_DH_SEED_MIN_SIZE_IN_BYTES);
                                remainingSize -=  CC_DH_SEED_MIN_SIZE_IN_BYTES;
                                current_ptr -= CC_DH_SEED_MIN_SIZE_IN_BYTES;
                        } else {   /* copy remaining low bytes to Q_ptr */
                                CC_PalMemCopy((uint8_t*)Q_ptr,
                                                (uint8_t*)hashRes1_ptr + CC_DH_SEED_MIN_SIZE_IN_BYTES - remainingSize,
                                                remainingSize);
                        }

                        /* set flag */
                        isFirst = 0;

                } /* end of for() loop */

                /* set the High and Low bits of Q equal to 1 */
                ((uint8_t*)Q_ptr)[0] |= 0x80;           /* MS bit - big endian */
                ((uint8_t*)Q_ptr)[QsizeBytes-1] |= 0x01;  /* LS bit - big endian */


                /* Step 3.2. Perform primality tests on Q: 8 Miller-Rabin and 1 Lucas tests (X9.42-2001) */
                /*---------------------------------------------------------------------------------------*/

                /* convert Q to words */
                Error = CC_CommonConvertMsbLsbBytesToLswMswWords(Q_ptr, QsizeBytes, (uint8_t*)Q_ptr, QsizeBytes);
                if (Error) {
                        Error = CC_DH_INVALID_ORDER_SIZE_ERROR;
                        goto EndWithError;
                }

                Error = RsaPrimeTestCall(
                                                 rndContext_ptr,
                                                 Q_ptr,
                                                 QsizeBytes/CC_32BIT_WORD_SIZE,
                                                 countMilRabTests,
                                                 &isPrime,
                                                 TempBuff2_ptr,
                                                 CC_DH_PRIME_TEST_MODE);

                if (Error != CC_OK) {
                        Error = CC_DH_PRIME_Q_GENERATION_FAILURE_ERROR;
                        goto EndWithError;
                }

        } /* END of while() loop */


        /* End of function */

        return Error;

        EndWithError:

        CC_PalMemSetZero((uint8_t*)Q_ptr, QsizeBytes);
        CC_PalMemSetZero((uint8_t*)S_ptr, seedSizeBytes);

        return Error;

} /* End of DhX942FindPrimeQ */


/******************************************************************************************/
/**
 * @brief The function finds prime modulus P for key generation according to X9.42-2001.
 *
 * @param[in]  rndContext_ptr     - Pointer to the RND context buffer.
 * @param[in]  modPSizeBits       - The  modulus (prime) P size in bits equal 256*n, where n >= 4.
 * @param[in]  QSizeBbytes        - The size of order of generator in bytes. Must be m >= 20 bytes and
 *                                  multiple of 4 bytes. According to ANSI X9.30-1: size = 20.
 * @param[in]  orderQSizeBits     - The size of order of generator in bits. Must be m >= 160 and
 *                                  multiple of 32 bits. According to ANSI X9.30-1: m = 160.
 * @param[in]  seedSizeBits       - The  seed size in bits (the size must be:  seedSizeBits >= 160,
 *                                  seedSizeBits <= modPSizeBits - 1 (the last required by implementation).
 * @param[out] P_ptr              - The prime modulus P of structure P = j*Q + 1, where Q is prime
 *                                  and j is an integer.The buffer must be aligned to 4 bytes.
 * @param[out] Q_ptr              - The pointer to the order Q of generator. The buffer must be aligned to 4 bytes.
 * @param[out] S_ptr              - The random seed used for generation of primes. The buffer must be aligned to 4 bytes.
 * @param[out] pgenCounter_ptr    - The pointer to counter of tries to generate the primes.
 * @param[in]  TempBuff1_ptr      - The temp buffer of size not less than max modulus size, aligned to 4 bytes.
 * @param[in]  TempBuff2_ptr      - The large temp buffer of size:
 *                                - on HW platform not less than 8*CC_DH_MAX_MOD_BUFFER_SIZE_IN_WORDS.
 *                                - on SW platform not less than 41*CC_DH_MAX_MOD_BUFFER_SIZE_IN_WORDS.
 * @param[in]  TempBuff3_ptr      - The temp buffer of size: 2*CC_DH_MAX_MOD_BUFFER_SIZE_IN_WORDS.
 *
 * @return CCError_t - On success CC_OK is returned, on failure a predefined error code.
 *
 *   Note: The function is static, therefore sizes of its input arrays (mod, ord, seed) are checked in
 *         caller functions and don't need to be chcked again.
 *
 */
static CCError_t DhX942FindPrimeP(
                                        CCRndContext_t *rndContext_ptr,
                                        uint32_t          modPsizeBits,         /*in */
                                        uint32_t          orderQsizeBits,       /*in */
                                        uint32_t          seedSizeBits,         /*in */
                                        uint32_t          *P_ptr,               /*out*/
                                        uint32_t          *Q_ptr,               /*out*/
                                        uint8_t           *S_ptr,               /*out*/
                                        uint32_t          *pgenCounter_ptr,     /*out*/
                                        uint32_t          *TempBuff1_ptr,       /*in */
                                        uint32_t          *TempBuff2_ptr,       /*in - large buffer*/
                                        uint32_t          *TempBuff_ptr )       /*in*/ // RL used in SW only
{

        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t Error;

        /* mod size in bytes and in words */
        uint32_t  modSizeBytes, modSizeWords;

        /* seed size in bytes and words */
        uint32_t seedSizeBytes;

        /* mod size in 160 bits blocks (rounded up) */
        uint32_t  L1;

        /* order sizes: M1 - in 160-bit blocks (rounded up) */
        uint32_t  orderSizeWords, M1;

        /* flag of first hash calculating */
        uint8_t  isFirst = 1;

        /* primality flag (if prime, then isPrime = 1, else 0 ) */
        uint8_t  isPrime;

        /* HASH input and result pointers */
        uint32_t  *hashDataIn_ptr;
        CCHashResultBuf_t   *hashRes_ptr;

        /* current data pointer and size */
        uint8_t  *current_ptr;
        int32_t  remainingSize;

        CCCommonCmpCounter_t  cmpRes;

        /* loop counter and carry */
        uint32_t i, carry;

        /* temp buffers pointers */
        uint32_t  *TempBuff3_ptr, *TempBuff4_ptr;
        uint32_t  countMilRabTests;

        /* FUNCTION  LOGIC */

        Error = CC_OK;


        /* --------------------------------- */
        /* Step 1. Check input parameters    */
        /*---------------------------------- */

        /* check pointers: modP, generator and tempBuff. Note: other pointers may be NULL  */
        if (P_ptr == NULL ||
            Q_ptr == NULL ||
            S_ptr == NULL ||
            pgenCounter_ptr == NULL ||
            TempBuff1_ptr == NULL   ||
            TempBuff2_ptr == NULL   ||
            TempBuff_ptr == NULL) {
                return CC_DH_INVALID_ARGUMENT_POINTER_ERROR;
        }

        /* --------------------------------- */
        /*  Step 2.  Initializations         */
        /* --------------------------------- */

        /* mod sizes in bytes */
        modSizeBytes = CALC_FULL_BYTES(modPsizeBits);
        modSizeWords = CALC_FULL_32BIT_WORDS(modPsizeBits);
        /* mod size in 160 bit blocks */
        L1 = (modPsizeBits + CC_DH_SEED_MIN_SIZE_IN_BITS - 1) / CC_DH_SEED_MIN_SIZE_IN_BITS;

        /* order size: M1 - in 160-bit blocks (rounded up) */
        M1 = (orderQsizeBits + CC_DH_SEED_MIN_SIZE_IN_BITS - 1) / CC_DH_SEED_MIN_SIZE_IN_BITS;
        orderSizeWords = CALC_FULL_32BIT_WORDS(orderQsizeBits);

        /* seedSize in bytes */
        seedSizeBytes = CALC_FULL_BYTES(seedSizeBits);

        /* zeroing of P  */
        CC_PalMemSetZero(P_ptr, modSizeBytes + 2);

        /* temp buffers pointers */
        TempBuff3_ptr = TempBuff2_ptr + modSizeWords + 2;
        TempBuff4_ptr = TempBuff3_ptr + 2*modSizeWords + 2;

        /*------------------------------------------------------ */
        /* Step 3.   Create random prime P = (Q*J + 1)           */
        /*------------------------------------------------------ */

        /* set pgenCounter 0 */
        *pgenCounter_ptr = 0;

        /* set HASH pointers to temp buffer */
        hashDataIn_ptr = TempBuff1_ptr;
        hashRes_ptr = (CCHashResultBuf_t*)TempBuff4_ptr;   /* used as temp buffer */

        /* Calculating R = seed + 2*M1 , where R is set in hashDataIn:
          copy the seed into hashDataIn_ptr (big endian);
          set other bytes to 0; add M1 */

        CC_PalMemCopy((uint8_t *)hashDataIn_ptr, S_ptr, seedSizeBytes);
        DhKgAddValueToMsbLsbBytesArray(hashDataIn_ptr, 2*M1, seedSizeBytes);

        /* set count of M-R tests for Q sccording FIPS 186-4 C.3: Tab. C.1. */
        if (modPsizeBits < 3072) {
                countMilRabTests = 3;
        } else {
                countMilRabTests = 2;
        }

        isPrime = CC_FALSE;

        /* Step 3.1. Main loop - try P candidates */
        /*----------------------------------------*/
        while (isPrime != CC_TRUE) {
                /* Step 3.1. Create P candidate:
                For i=0 to L1 do:  P = P + SHA1(R+i) *(2^(160*i)) */

                /* set current pointer and size for copying HASH results into *
                *  mod P as big endian bytes              */
                current_ptr = &((uint8_t*)P_ptr)[modSizeBytes - CC_DH_SEED_MIN_SIZE_IN_BYTES];
                remainingSize = modSizeBytes;

                for (i = 0; i < L1; i++) {
                        /* Adding 1 to hashDataIn excluding the first hashing operation */
                        if (isFirst != 1) {
                                DhKgAddValueToMsbLsbBytesArray(hashDataIn_ptr, 1, seedSizeBytes);
                        }

                        /* set 0 to isFirst */
                        isFirst = 0;

                        /* calculate HASH result */
                        Error = CC_Hash(
                                         CC_HASH_SHA1_mode,
                                         (uint8_t *)hashDataIn_ptr,
                                         seedSizeBytes,
                                         *hashRes_ptr );

                        if (Error != CC_OK) {
                                goto EndWithError;
                        }

                        /* set size for copying HASH result into P buffer */
                        if (remainingSize >= CC_DH_SEED_MIN_SIZE_IN_BYTES) {
                                CC_PalMemCopy(current_ptr, hashRes_ptr, CC_DH_SEED_MIN_SIZE_IN_BYTES);
                                remainingSize -=  CC_DH_SEED_MIN_SIZE_IN_BYTES;
                                current_ptr -= CC_DH_SEED_MIN_SIZE_IN_BYTES;
                        } else {
                                CC_PalMemCopy((uint8_t*)P_ptr,
                                                (uint8_t*)hashRes_ptr + CC_DH_SEED_MIN_SIZE_IN_BYTES - remainingSize,
                                                remainingSize);
                        }

                } /* end of j - loop */

                /*-----------------------------------------------------------------------*/

                /* convert P to LSW array */
                Error = CC_CommonConvertMsbLsbBytesToLswMswWords(P_ptr, modSizeWords*4, (uint8_t*)P_ptr, modSizeBytes);
                if (Error) {
                        Error = CC_DH_INVALID_MODULUS_SIZE_ERROR;
                        return Error;
                }

                /*----------------------------------------*/
                /* Step 3.2. Set P = P - (P mod 2*Q) + 1  */
                /* Note: Now all operations on LSW arrays */
                /*----------------------------------------*/

                /* set the High and Low bits of Q equal to 1 */
                P_ptr[modSizeWords-1] |= 0x80000000; /* MS bit */

                /* set TempBuff3 = 2*Q. Note: Result size is large by 1 byte (and word), than Q size */
                carry = CC_CommonAdd2vectors(Q_ptr, Q_ptr, CC_MIN(modSizeWords, orderSizeWords), TempBuff3_ptr);

                /* if carry occurs, set next word of TempBuff3 to 1, else to 0 */
                if (carry != 0) {
                        if(orderSizeWords + 1 > modSizeWords)
                                return CC_DH_INVALID_ORDER_SIZE_ERROR;
                        TempBuff3_ptr[orderSizeWords] = 1;
                } else {
                        TempBuff3_ptr[orderSizeWords] = 0;
                }

                /* calculate TempBuff4 = P mod 2*Q */
                CC_PalMemSetZero((uint8_t*)TempBuff4_ptr, modSizeBytes);

                Error = PkiLongNumDiv(
                                            P_ptr,                      /*numerator P*/
                                            modSizeWords,               /*P_size in words*/
                                            TempBuff3_ptr,              /*divider */
                                            orderSizeWords + 1,         /*divider_size in words*/
                                            TempBuff4_ptr,              /*ModRes_ptr*/
                                            TempBuff2_ptr              /*DivRes_ptr*/);
                if (Error) {
                        Error = CC_DH_PRIME_P_GENERATION_FAILURE_ERROR;
                        return Error;
                }

                /* subtract: P = P - TempBuff4 */
                CC_CommonSubtractUintArrays(P_ptr, TempBuff4_ptr, modSizeWords, P_ptr);

                /* add 1 to P */
                CC_CommonIncLsbUnsignedCounter(P_ptr, 1, (uint8_t)modSizeWords);

                /* check: if P > 2^(L-1), then perform step 3.3. */
                /*-----------------------------------------------*/

                /*  set TempBuff5 = 2^(L-1): Note: L = modPsizeBits is        *
                *   multiple of 32 bits                       */
                CC_PalMemSetZero((uint8_t*)TempBuff4_ptr, modSizeBytes);
                TempBuff4_ptr[modSizeWords - 1] = 0x80000000;

                /* compare */
                cmpRes = CC_CommonCmpLsWordsUnsignedCounters(
                                                               P_ptr, (uint16_t)modSizeWords, TempBuff4_ptr, (uint16_t)modSizeWords);


                /* Step 3.3. If P is not diverted, then perform primality               *
                 *  tests on P: 8 Rabin-Miller and 1 Lucas tests (X9.42-2001)           *
                 *----------------------------------------------------------------------*/

                if (cmpRes == CC_COMMON_CmpCounter1GreaterThenCounter2) {
                        Error = RsaPrimeTestCall(rndContext_ptr,
                                                          P_ptr,
                                                          modSizeWords,
                                                          countMilRabTests,
                                                          (int8_t*)&isPrime,
                                                          TempBuff2_ptr,
                                                          CC_DH_PRIME_TEST_MODE);
                        if (Error != CC_OK) {
                                Error = CC_DH_PRIME_P_GENERATION_FAILURE_ERROR;
                                goto EndWithError;
                        }
                }

                /* RL defines: 4096 -> PGEN_COUNTER_MAX_VAL,  L = 1024 -> PRIME_MOD_MIN_VAL */
                /* update pgenCounter_ptr */
                *pgenCounter_ptr += 1;

                /* if pgenCounter >= 4096*N then return "generation is fail" */
                if (*pgenCounter_ptr >= DH_X942_PGEN_COUNTER_CONST*(modPsizeBits + DH_X942_PRIME_MOD_MIN_VAL - 1)/DH_X942_PRIME_MOD_MIN_VAL) {
                        Error = CC_DH_PRIME_P_GENERATION_FAILURE_ERROR;
                        goto EndWithError;
                }

        } /* END of while(isPrime != CC_TRUE)*/

        /* correction of pgenCounter */
        *pgenCounter_ptr -= 1;

        /* End of function */
        return Error;

        EndWithError:

        CC_PalMemSetZero(P_ptr, modSizeBytes);

        return Error;

}/* End of DhX942FindPrimeP */




/******************************************************************************************/
/**
 * @brief The function creates generator of GF(P) subgroup for key generation according to X9.42-2001.
 *
 *
 * @param[in]  rndContext_ptr     - Pointer to the RND context buffer.
 * @param[out] P_ptr              - The prime modulus P of structure P = j*Q + 1, where Q is prime
 *                                  and j is an integer.The buffer must be aligned to 4 bytes.
 * @param[in]  modPSizeBits       - The  modulus (prime) P size in bytes must be multiple of 4 bytes.
 * @param[out] Q_ptr              - The pointer to the order Q of generator. The buffer must be aligned to 4 bytes.
 * @param[in]  orderSizeBits      - The size of order of generator in bytes. Must be multiple of 4 bytes.
 * @param[out] G_ptr              - The generator of GF(P) subgroup. The buffer must be aligned to 4 bytes.
 *                                  size of buffer not less than modPSize in bytes.
 * @param[in]  tempBuff1_ptr      - The temp buffer of size not less than DH max modulus size, aligned to 4 bytes.
 * @param[in]  expTempBuff_ptr    - The temp buffer of defined structure.
 *
 * @return CCError_t - On success CC_OK is returned, on failure a predefined error code.
 *
 *
 */
static CCError_t DhX942CreateGenerator(CCRndContext_t *rndContext_ptr,
                                              uint32_t           *P_ptr,               /*in */
                                              uint32_t           modSizeBits,          /*in */
                                              uint32_t           *Q_ptr,               /*in */
                                              uint32_t           orderSizeBits,        /*in */
                                              uint32_t           *G_ptr,               /*out*/
                                              uint32_t           *tempBuff1_ptr,       /*in */
                                              CCDhExpTemp_t  *expTempBuff_ptr)     /*in */
{

        /* FUNCTION DECLARATIONS */

        // RL  J-factor is used in other functions
        /* The return error identifier */
        CCError_t Error = CC_OK;

        /* modulus and order sizes in words */
        uint32_t modSizeBytes, modSizeWords, orderSizeBytes;

        uint32_t J_effectiveSizeBits;

        /* compare flag */
        CCCommonCmpCounter_t  compFlag;


        /* INITIALIZATIONS */

        modSizeBytes = CALC_FULL_BYTES(modSizeBits);
        modSizeWords = CALC_FULL_32BIT_WORDS(modSizeBits);
        orderSizeBytes = CALC_FULL_BYTES(orderSizeBits);


        /* FUNCTION  LOGIC */

        /*-------------------------------------*/
        /* Step 1. Calculate J = (P - 1)/Q     */
        /*-------------------------------------*/

        /*  copy modulus into TempBuff1  */
        CC_PalMemCopy((uint8_t*)expTempBuff_ptr->PubKey.n, (uint8_t*)P_ptr, modSizeBytes);
        CC_PalMemSetZero((uint8_t*)expTempBuff_ptr->PubKey.n + modSizeBytes, CC_DH_MAX_MOD_SIZE_IN_BYTES - modSizeBytes);
        /* copy order Q into aligned buffer */
        CC_PalMemCopy((uint8_t*)expTempBuff_ptr->TempBuff, (uint8_t*)Q_ptr, orderSizeBytes);
        CC_PalMemSetZero((uint8_t*)expTempBuff_ptr->TempBuff + orderSizeBytes, CC_DH_MAX_MOD_SIZE_IN_BYTES - orderSizeBytes);

        /* subtract: P - 1 */
        CC_CommonDecrLsbUnsignedCounter(expTempBuff_ptr->PubKey.n, 1, modSizeWords);

        /* divide (P - 1)/Q */
        PkiLongNumDiv(
                            expTempBuff_ptr->PubKey.n,            /*numerator B*/
                            modSizeWords,                         /*B_size in words*/
                            expTempBuff_ptr->TempBuff,            /*Q - divider*/
                            CALC_32BIT_WORDS_FROM_BYTES(orderSizeBytes),                 /*Q_size in words*/
                            expTempBuff_ptr->PubKey.e,            /*ModRes_ptr*/
                            tempBuff1_ptr                        /*DivRes_ptr*/);

        /* calculate actual size of J in bits: Use min() to prevent warnings  */
        J_effectiveSizeBits = CC_MIN(modSizeBits, CC_CommonGetWordsCounterEffectiveSizeInBits(
                                                                                                  tempBuff1_ptr, modSizeWords));

        /*---------------------------------------------------------------*/
        /* Step 2. Generate random G : 1 < G < (P-1)  and                */
        /*         set it into DataIn buffer, other bytes of buffer = 0  */
        /*---------------------------------------------------------------*/
        /* cleaning of temp buffer */
        CC_PalMemSetZero((uint8_t*)&expTempBuff_ptr->PrimeData, sizeof(CCDhPrimeData_t));

        /* generating rnd vector */

        Error = RndGenerateWordsArrayInRange(rndContext_ptr,
                                                  modSizeBits, expTempBuff_ptr->PubKey.n /*P-1*/,
                                                  expTempBuff_ptr->PrimeData.DataIn /*RND*/,
                                                  expTempBuff_ptr->PrimeData.DataOut/*temp*/);
        if (Error != CC_OK) {
                goto End;
        }

        /*----------------------------------------------------*/
        /* Step 3. Initialization of PubKey and PrivData      */
        /*         structures for exponentiation              */
        /*----------------------------------------------------*/

        /* cleaning of temp buffer */
        CC_PalMemSetZero((uint8_t*)&expTempBuff_ptr->PubKey, sizeof(expTempBuff_ptr->PubKey));

        /* set modulus in DH_PubKey structure for exponentiation G^J mod P */
        CC_PalMemCopy((uint8_t*)expTempBuff_ptr->PubKey.n, (uint8_t*)P_ptr, modSizeBytes);
        expTempBuff_ptr->PubKey.nSizeInBits = modSizeBits;
        /* set exponent J and its size */
        CC_PalMemCopy( (uint8_t*)expTempBuff_ptr->PubKey.e, (uint8_t*)tempBuff1_ptr, CALC_FULL_BYTES(J_effectiveSizeBits) );
        expTempBuff_ptr->PubKey.eSizeInBits = J_effectiveSizeBits;

        /*  initialize the H value in LLF of PubKey for exponentiation  */
        Error = RsaInitPubKeyDb(&expTempBuff_ptr->PubKey);
        if (Error != CC_OK) {
                Error = CC_DH_INTERNAL_ERROR;
                goto End;
        }


        /*-----------------------------------------------------------*/
        /* Step 4. Calculate G = G ^ J mod P , if G == 1, change     */
        /*         G (DataIn) and repeat exponentiation              */
        /*-----------------------------------------------------------*/

        compFlag = CC_COMMON_CmpCounter1AndCounter2AreIdentical;  /* 0 - means G == 1 */
        /* set 1 to tempBuff1_ptr for comparing */// RL
        CC_PalMemSetZero((uint8_t*)tempBuff1_ptr, modSizeBytes);
        tempBuff1_ptr[0] = 1;

        while (compFlag == 0) {
                /* exponentiation DataOut = DataIn ^ Exp mod P */
                Error = RsaExecPubKeyExp(&expTempBuff_ptr->PubKey, &expTempBuff_ptr->PrimeData);

                if (Error != CC_OK) {
                        Error = CC_DH_INTERNAL_ERROR;
                        return Error;
                }

                /* compare DataOut to 1: */
                compFlag = CC_CommonCmpLsWordsUnsignedCounters(
                                                                 expTempBuff_ptr->PrimeData.DataOut, modSizeWords,
                                                                 tempBuff1_ptr, modSizeWords);

                /* if G == 1 change DataIn (by adding 1) for trying next G value */
                if (compFlag == 0) {
                        CC_CommonIncLsbUnsignedCounter(
                                                         expTempBuff_ptr->PrimeData.DataIn, 1, (uint8_t)modSizeWords);
                }
        }

        /* copy generator into output */
        CC_PalMemCopy((uint8_t*)G_ptr, (uint8_t*)expTempBuff_ptr->PrimeData.DataOut, modSizeBytes);

        /* End of function */
        End:
        return Error;

}/* End of DhX942CreateGenerator */


/******************************************************************************************/
/**
 * @brief The function generates a DH (DLP) domain parameters in GF(P) (see X9.42-2001)
 *
 *   The function parameters are the same as in CC_DhCreateDomainParams() function (see below)
 *   besides one difference: this function not checks input parameters, because it is also used locally
 *   in some other functions with input pointers = NULL.
 *
 *   Note: The function is static, therefore sizes of its input arrays (mod, ord, seed) are checked in
 *         caller functions and don't need to be chcked again.
 *
 * @return CCError_t - On success CC_OK is returned, on failure - a predefined error code.
 *
 */
static CCError_t DhCreateDomainParams(
                                           CCRndContext_t *rndContext_ptr,
                                           uint32_t          modPsizeBits,             /*in */
                                           uint32_t          orderQsizeBits,           /*in */
                                           uint32_t          seedSizeBits,             /*in */
                                           uint8_t          *modP_ptr,                 /*out*/
                                           uint8_t          *orderQ_ptr,               /*out*/
                                           uint8_t          *generatorG_ptr,           /*out*/
                                           uint32_t         *generGsizeBytes_ptr,      /*in/out*/
                                           uint8_t          *factorJ_ptr,              /*out*/
                                           uint32_t         *JsizeBytes_ptr,           /*in/out*/
                                           uint8_t          *seedS_ptr,                /*in/out*/
                                           int8_t            generateSeed,             /*in*/
                                           uint32_t         *pgenCounter_ptr,          /*out*/
                                           CCDhKgData_t  *DHKGbuff_ptr              /*in */)
{

        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t Error = CC_OK;

        /* pointers to temp buffers for candidates to order Q, modulus P, seed S, generator G */
        uint32_t  *Q_ptr, *P_ptr, *G_ptr, *J_ptr;
        uint8_t  *S_ptr;

        /* tries counter */
        uint32_t  pgenCounter;

        uint32_t  modSizeBytes, generatorSizeBits;

        /* temp buffer pointers */
        uint32_t  *TempBuff1_ptr, *TempBuff2_ptr;


        /* --------------------------------- */
        /*  Step 2.  Initializations         */
        /* --------------------------------- */

        /* clean DHKGbuff_ptr */
        CC_PalMemSetZero(DHKGbuff_ptr, sizeof(CCDhKgData_t));

        /* set Q, S and G- pointers on DHKGbuff_ptr->PrimData temp buffers */
        Q_ptr = DHKGbuff_ptr->TempBuff2;
        P_ptr = DHKGbuff_ptr->TempBuff3;
        G_ptr = DHKGbuff_ptr->TempBuff4;
        J_ptr = DHKGbuff_ptr->TempBuff5;
        S_ptr = (uint8_t*)J_ptr;

        /* set 32-bit temp pointers on KGData and PrimData temp buffers */
        TempBuff1_ptr = DHKGbuff_ptr->TempBuff1;
        TempBuff2_ptr = (uint32_t*)&(DHKGbuff_ptr->ExpTemps);

        if (generateSeed == 0) {
                CC_PalMemCopy((uint8_t*)S_ptr, seedS_ptr, CALC_FULL_BYTES(seedSizeBits));
        }

        modSizeBytes = CALC_FULL_BYTES(modPsizeBits);


        /*------------------------------------------------------------------- */
        /* Step 1. Find random prime Q and its Seed S according to ANSI X9.42 */
        /*------------------------------------------------------------------- */

        Error = DhX942FindPrimeQ(rndContext_ptr,
                                      orderQsizeBits,            /*in */
                                      seedSizeBits,              /*in */
                                      generateSeed,              /*in */
                                      Q_ptr,                     /*out*/
                                      S_ptr,                     /*in/out*/
                                      TempBuff1_ptr,             /*in */
                                      TempBuff2_ptr,             /*in */
                                      DHKGbuff_ptr->TempBuff6);  /*in */
        if (Error != CC_OK) {
                goto EndWithError;
        }


        /*------------------------------------------------------ */
        /* Step 2.   Create random prime P = (Q*J + 1)           */
        /*------------------------------------------------------ */

        Error = DhX942FindPrimeP(rndContext_ptr,
                                      modPsizeBits,        /*in */
                                      orderQsizeBits,      /*in */
                                      seedSizeBits,        /*in */
                                      P_ptr,               /*out*/
                                      Q_ptr,               /*out*/
                                      S_ptr,               /*in */
                                      &pgenCounter,        /*out*/
                                      TempBuff1_ptr,       /*in */
                                      TempBuff2_ptr,       /*in */
                                      DHKGbuff_ptr->TempBuff6); /*in */

        if (Error != CC_OK) {
                goto EndWithError;
        }


        /*------------------------------------------------------ */
        /* Step 3.   Create generator of GF(P) subgroup          */
        /*------------------------------------------------------ */
        if (generatorG_ptr != NULL) {
                Error = DhX942CreateGenerator(rndContext_ptr,
                                                   P_ptr,                                       /*in */
                                                   modPsizeBits,                                /*in */
                                                   Q_ptr,                                       /*in */
                                                   orderQsizeBits,                              /*in */
                                                   G_ptr,                                       /*out*/
                                                   TempBuff1_ptr,                               /*in */
                                                   (CCDhExpTemp_t*)&DHKGbuff_ptr->ExpTemps  /*in */);

                if (Error != CC_OK) {
                        goto EndWithError;
                }

                /* calculate size of generator and output it in big endianness */
                generatorSizeBits = CC_CommonGetWordsCounterEffectiveSizeInBits(G_ptr, (uint16_t)modSizeBytes/CC_32BIT_WORD_SIZE);
                *generGsizeBytes_ptr = CALC_FULL_BYTES(generatorSizeBits);

                Error = CC_CommonConvertLswMswWordsToMsbLsbBytes(
                                                                   generatorG_ptr, *generGsizeBytes_ptr, G_ptr, *generGsizeBytes_ptr);
                if (Error != CC_OK) {
                        Error = CC_DH_INVALID_GENERATOR_PTR_OR_SIZE_ERROR;
                        goto EndWithError;
                }
        }

        /* output of result parameters (in big endianness) */
        Error = CC_CommonConvertLswMswWordsToMsbLsbBytes(modP_ptr, modSizeBytes, P_ptr, modSizeBytes);
        if (Error != CC_OK) {
                Error = CC_DH_INVALID_MODULUS_SIZE_ERROR;
                goto EndWithError;
        }

        if (orderQ_ptr != NULL) {
                Error = CC_CommonConvertLswMswWordsToMsbLsbBytes(
                                                                   orderQ_ptr, CALC_FULL_BYTES(orderQsizeBits), Q_ptr, CALC_FULL_BYTES(orderQsizeBits));
                if (Error != CC_OK) {
                        Error = CC_DH_INVALID_ORDER_SIZE_ERROR;
                        goto EndWithError;
                }
        }

        /* copy generated seed into output */
        if (generateSeed == 1) {
                CC_PalMemCopy(seedS_ptr, (uint8_t*)S_ptr, CALC_FULL_BYTES(seedSizeBits));
        }

        /* if factorJ_ptr != NULL, then calculate this factor and its size. J = (P-1) / Q */

        // RL Use J-factor from previous function
        if (factorJ_ptr != NULL) {
                PkiLongNumDiv(
                                    P_ptr,                 /*numerator B*/
                                    CALC_FULL_32BIT_WORDS(modPsizeBits),     /*B_size in words*/
                                    Q_ptr,                 /*divider N */
                                    CALC_FULL_32BIT_WORDS(orderQsizeBits),   /*N_size in words*/
                                    TempBuff1_ptr,         /*ModRes_ptr*/
                                    J_ptr                 /*DivRes_ptr*/);

                /* calculate size of J in bits */
                *JsizeBytes_ptr = CC_CommonGetWordsCounterEffectiveSizeInBits(J_ptr, (uint16_t)modSizeBytes/CC_32BIT_WORD_SIZE);

                /* calculate size of J in bytes */
                *JsizeBytes_ptr = CALC_FULL_BYTES(*JsizeBytes_ptr);

                /* convert result to MSB bytes and output into factorJ_ptr buffer */
                Error = CC_CommonConvertLswMswWordsToMsbLsbBytes(
                                                                   factorJ_ptr, *JsizeBytes_ptr,
                                                                   J_ptr, *JsizeBytes_ptr);
                if (Error != CC_OK) {
                        Error = CC_DH_INVALID_J_FACTOR_PTR_OR_SIZE_ERROR;
                        goto EndWithError;
                }
        }

        /*  if pgenCounter_ptr != NULL put out pgenCounter */
        if (pgenCounter_ptr != NULL) {
                *pgenCounter_ptr = pgenCounter;
        }

        goto End;

        /* End of function */

        EndWithError:

        /* cleaning output buffers used also in internal computations */
        CC_PalMemSetZero(modP_ptr, CALC_FULL_BYTES(modPsizeBits));

        if (generatorG_ptr != NULL) {
                CC_PalMemSetZero(generatorG_ptr, *generGsizeBytes_ptr);
        }

        if (orderQ_ptr != NULL) {
                CC_PalMemSetZero(orderQ_ptr, CALC_FULL_BYTES(orderQsizeBits));
        }

        if (factorJ_ptr != NULL) {
                CC_PalMemSetZero(factorJ_ptr, *JsizeBytes_ptr);
        }

        if (generateSeed == 1) {
                CC_PalMemSetZero(seedS_ptr, CALC_FULL_BYTES(seedSizeBits));
        }

        End:
        /* cleaning of temp buffer */
        CC_PalMemSetZero(DHKGbuff_ptr, sizeof(CCDhKgData_t));

        return Error;

}/* End of DhCreateDomainParams */



/******************************************************************************************/
/************************         Public Functions           ******************************/
/******************************************************************************************/

/******************************************************************************************/
/**
* @brief The function generates a DH (DLP) domain parameters in GF(P) (see X9.42-2001)
*
*
* @param [in]  rndContext_ptr     - Pointer to the RND context buffer.
* @param [in]  modPSizeBits       - Size of the modulus (Prime) in bits equal 256*n, where n >= 4. FIPS 186-4
*                                   defines 1024 and 2048 bit.
* @param [in]  orderQSizeBits     - Size of the Generator's order in bits. FIPS 186-4 defines orderQSizeBits = 160
*                                   for modulus 1024 bit and 224 or 256 bit for modPSizeBits = 2048. We not recommend
                                    orderQSizeBits > 256 and returns an error if it > modPSizeBits/4 .
* @param [in]  seedSizeBits       - The  seed size in bits. Requirements:
*                                  seedSizeBits >= orderQSizeBits and seedSizeBits <= modPSizeBits ( the
*                                  last is required by our implementation).
* @param [out] modP_ptr           - The prime modulus P of structure P = J*Q + 1, where Q is prime
*                                  and j is an integer. Size of the buffer for output generated value must
*                                  be not less, than modulus size.
* @param [out] orderQ_ptr         - The pointer to the order Q of generator. The size of the buffer for output
*                                  generated value must be not less, than order size.
* @param [out] generatorG_ptr     - The pointer to the generator of multiplicative subgroup in GF(P).
*                                  If the pointer == NULL, the function returns an error. Size of the buffer
*                               for output generated value must be not less, than modulus size.
* @param [in/out]generGsizeBytes_ptr - The pointer to the one-word buffer, containing the generator size value (in bytes).
*                                  The user must set the size of allocated buffer, and the function returns the
*                               actual size of the generator in bytes.
* @param [out] factorJ_ptr        - The pointer to buffer for integer factor J. If the pointer == NULL, the function
*                                  not puts this parameter out. In this case JsizeBytes_ptr must be set to NULL,
*                                  otherwise the function returns an error. The size of the buffer must be not less,
*                                  than ( modPSizesBytes - orderQSizeBytes + 1 ).
* @param [in/out] JsizeBytes_ptr  - The pointer to the size of integer factor J. If the pointer == NULL,
*                                  the function not puts this parameter out. If output of the factor J is needed, the
*                                  user must set the J size value equal to the size of allocated buffer, and the
*                                  function returns the actual size of J in bytes.
* @param [in/out] seedS_ptr       - The random seed used for generation of primes. The size of the buffer for output
*                           generated value must be not less, than passed seed size (see above) and not less
*                           20 bytes (160 bits).
* @param [in] generateSeed        - The flag, defining whether the seed generated randomly by the function
*                                  (generateSeed = 1), or it is passed by the input (generateSeed = 0).
* @param [out] pgenCounter_ptr    - The pointer to counter of tries to generate the primes. If the pointer == NULL,
*                                  the function not puts this parameter out.
* @param [out] DHKGBuff_ptr       - The temp buffer for internal calculations. The buffer is defined as structure.
 *
 * @return CCError_t - On success CC_OK is returned, on failure - a predefined error code.
 *
*     Note:  1. Input and Output vectors are in big endianness (high most bit is left most one).
*            2. For reliability of checking of input parameters, in case that the user don't wont output of
*               some parameters (generator or factorJ), he must set both - a pointer to appropriate buffer and a
*               pointer to its size equaled to NULL for these parameters, otherwise the function returns an error.
 *            2. In case of any error the function may clean the output buffers.
 *
 */
CEXPORT_C CCError_t CC_DhCreateDomainParams(
                                                CCRndContext_t *rndContext_ptr,
                                                uint32_t          modPsizeBits,             /*in */
                                                uint32_t          orderQsizeBits,           /*in */
                                                uint32_t          seedSizeBits,             /*in */
                                                uint8_t          *modP_ptr,                 /*out*/
                                                uint8_t          *orderQ_ptr,               /*out*/
                                                uint8_t          *generatorG_ptr,           /*out*/
                                                uint32_t         *generGsizeBytes_ptr,      /*in/out*/
                                                uint8_t          *factorJ_ptr,              /*out*/
                                                uint32_t         *JsizeBytes_ptr,           /*in/out*/
                                                uint8_t          *seedS_ptr,                /*in/out*/
                                                int8_t            generateSeed,             /*in*/
                                                uint32_t         *pgenCounter_ptr,          /*out*/
                                                CCDhKgData_t  *DHKGbuff_ptr              /*in */ )
{

        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t Error = CC_OK;

        uint32_t  modSizeBytes, orderSizeBytes;

        CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /* check pointers: modP, orderQ and temp buffer. Note: other pointers may be NULL, if not used  */
        if (modP_ptr == NULL   ||
            orderQ_ptr == NULL ||
            seedS_ptr == NULL  ||
            DHKGbuff_ptr == NULL) {
                return CC_DH_INVALID_ARGUMENT_POINTER_ERROR;
        }
        /* check sizes */
        if (modPsizeBits < CC_DH_MIN_VALID_KEY_SIZE_VALUE_IN_BITS ||   /* check sizes */
            modPsizeBits % 256 != 0 ||
            modPsizeBits > CC_DH_MAX_VALID_KEY_SIZE_VALUE_IN_BITS) {
                return CC_DH_INVALID_MODULUS_SIZE_ERROR;
        }

        /* init the sizes */
        modSizeBytes = CALC_FULL_BYTES(modPsizeBits);
        orderSizeBytes = CALC_FULL_BYTES(orderQsizeBits);

        if (orderQsizeBits < CC_DH_SEED_MIN_SIZE_IN_BITS ||
            orderQsizeBits >  modPsizeBits / 4   ||
            orderQsizeBits % CC_BITS_IN_32BIT_WORD != 0) {
                return CC_DH_INVALID_ORDER_SIZE_ERROR;
        }

        if (seedSizeBits < orderQsizeBits/* according to X9.42-2001 */  ||
            seedSizeBits > modPsizeBits /* our limitation of buffer size */) {
                return CC_DH_INVALID_SEED_SIZE_ERROR;
        }

    /* check generator G pointers and buffer size, it is allowed that both generator and size are NULL */
    if (((generatorG_ptr != NULL) && (generGsizeBytes_ptr == NULL)) ||
        ((generatorG_ptr == NULL) && (generGsizeBytes_ptr != NULL)))
        return CC_DH_INVALID_GENERATOR_PTR_OR_SIZE_ERROR;

        /* check generator G buffer size */
         if ((generGsizeBytes_ptr != NULL) && (*generGsizeBytes_ptr < modSizeBytes)) {
                return CC_DH_INVALID_GENERATOR_PTR_OR_SIZE_ERROR;
        }

        /* check J-factor pointers and buffer size */
        if ((factorJ_ptr == NULL && JsizeBytes_ptr != NULL) ||
            (factorJ_ptr != NULL && JsizeBytes_ptr == NULL) ||
            ((JsizeBytes_ptr != NULL) && (*JsizeBytes_ptr < (modSizeBytes - orderSizeBytes + 1)))) {
                return CC_DH_INVALID_J_FACTOR_PTR_OR_SIZE_ERROR;
        }

        /* check generateSeed parameter */
        if (generateSeed != 0 && generateSeed != 1) {
                return CC_DH_INVALID_ARGUMENT_OPERATION_MODE_ERROR;
        }

        /*   call exec function */
        Error = DhCreateDomainParams(
                                        rndContext_ptr,
                                        modPsizeBits,             /*in */
                                        orderQsizeBits,           /*in */
                                        seedSizeBits,             /*in */
                                        modP_ptr,                 /*out*/
                                        orderQ_ptr,               /*out*/
                                        generatorG_ptr,           /*out*/
                                        generGsizeBytes_ptr,      /*in/out*/
                                        factorJ_ptr,              /*out*/
                                        JsizeBytes_ptr,           /*in/out*/
                                        seedS_ptr,                /*in/out*/
                                        generateSeed,             /*in*/
                                        pgenCounter_ptr,          /*out*/
                                        DHKGbuff_ptr);            /*in */

        return Error;
}/* End of CC_DhCreateDomainParams */


/******************************************************************************************/
/**
* @brief The function checks the obtained DH domain parameters according X9.42-2001.
*
*        There may be 3 case of checking:
*        1. Checking of primes only ( modulus P and order Q according to passed seed S and pgenCounter).
*           In this case all pointers and sizes of said parameters must be passed (not NULL), but generator
*           G pointer and it size must be both set to NULL.
*        2. Checking of generator G only in assuming that primes parameters P, Q are valid. In ths case
*           the user must to pass the P,Q,G pointers and sizes. The seed S pointer and size must be both
*           set to NULL, otherwise the function returns an error.
*        3. Checking all domain parameters. In this case all input parameters must be passed to the function.
*
*        If any of checked domain parameters is not compliant to X9.42-2001 standard and our implementation
*        limitation, the function returns an error according to cc_dh_error.h file.
*
*        NOTE:  Detailed requirements to all used parameters are described above in CC_DhCreateDomainParams
*               functions API.
*
* @param[in]  rndContext_ptr     - Pointer to the RND context buffer.
* @param[out] modP_ptr           - The prime modulus P. Must be of structure P = j*Q + 1,
*                                  where Q is prime and j is an integer.
* @param[in]  modPSizeBits       - The  modulus (prime) P size in bits equal 256*n, where n >= 4.
* @param[out] orderQ_ptr         - The pointer to the order Q of generator.
* @param[in]  orderQSizeBytes    - The size of order of generator in bytes. According to ANSI X9.43:
*                                  m must be multiple of 32 bits and m >= 160. According to ANSI X9.30-1:
*                                  m = 160 bit. In our implementation required, that orderQSize <= modPSizeBytes/4.
* @param[in]  generatorG_ptr     - The pointer to the generator of multiplicative subgroup in GF(P).
* @param[in]  generatorSizeBytes - The size of generator in bytes (must be set if generator will be checked).
* @param[in]  seedS_ptr          - The random seed used for generation of primes (must be set if
*                                  primes will be checked).
* @param[in]  seedSizeBits       - The seed size in bits. If the seed is used,
*                      then its size must be:
*                      seedSizeBits >= orderQSizeBits and
*                      seedSizeBits <= modPSizeBits ( the last is
*                      required by our implementation).
* @param[in]  pgenCounter        - The counter of tries to generate the primes (must be set if primes
*                                  will be checked).
* @param[in] TempBuff_ptr        - The temp buffer of defined structure.
*
* @return CCError_t - On success CC_OK is returned, on failure or if one or more domain
*                       parameters are invalid the function returns a predefined error code.
*
*     Note:  Input vectors are in big endianness.
*
*/
CEXPORT_C CCError_t CC_DhCheckDomainParams(
                                               CCRndContext_t *rndContext_ptr,
                                               uint8_t               *modP_ptr,           /*in */
                                               uint32_t               modPsizeBytes,      /*in */
                                               uint8_t               *orderQ_ptr,         /*in */
                                               uint32_t               orderQsizeBytes,    /*in */
                                               uint8_t               *generatorG_ptr,     /*in */
                                               uint32_t               generatorSizeBytes, /*in */
                                               uint8_t               *seedS_ptr,          /*in */
                                               uint32_t               seedSizeBits,       /*in */
                                               uint32_t               pgenCounter,        /*in */
                                               CCDhKgCheckTemp_t   *checkTempBuff_ptr   /*in */ )
{
        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t Error = CC_OK;


        /* pointers to temp buffers */
        uint32_t  *Q_ptr, *P_ptr;
        CCDhKgData_t *DHKGbuff_ptr;
        uint32_t  *TempBuff_ptr;

        /* size of modulus in bits and in words */
        uint32_t  modPsizeBits, modPsizeWords;

        /* size  order Q (in bits) */
        uint32_t  orderQsizeBits;

        /* counter of trying to generate modulus P; pgenCounter*/
        uint32_t  pgenCounter1;

        CHECK_AND_RETURN_ERR_UPON_FIPS_ERROR();

        /* check pointers: modP, generator and tempBuff. Note: other pointers may be NULL  */
        if (modP_ptr == NULL ||
            orderQ_ptr == NULL ||
            checkTempBuff_ptr == NULL) {
                return CC_DH_INVALID_ARGUMENT_POINTER_ERROR;
        }

        /* check modulus and order sizes */
        if (modPsizeBytes < CC_DH_MIN_VALID_KEY_SIZE_VALUE_IN_BITS / 8 ||
            modPsizeBytes % CC_BITS_IN_32BIT_WORD != 0 ||
            modPsizeBytes > CC_DH_MAX_MOD_SIZE_IN_BYTES) {
                return CC_DH_INVALID_MODULUS_SIZE_ERROR;
        }

        if (orderQsizeBytes < CC_DH_SEED_MIN_SIZE_IN_BITS / 8 ||
            orderQsizeBytes % CC_32BIT_WORD_SIZE != 0 ||
            orderQsizeBytes > modPsizeBytes/4) {
                return CC_DH_INVALID_ORDER_SIZE_ERROR;
        }

        /* Seed pointer and size checking:
           If pointer or size of seed are illegal, then output an error.
           Note: In case that primes checking is not needed, the seed pointer and size must be
                     set to NULL  and are legal */
        if ((seedSizeBits == 0 && seedS_ptr != NULL) ||
            (seedSizeBits != 0 && seedS_ptr == NULL)) {
                return CC_DH_CHECK_SEED_SIZE_OR_PTR_NOT_VALID_ERROR;
        }

        /* Generator pointer and size checking:
           If pointer or size of generator are illegal, then output an error.
           Note: In case that generator checking is not needed, its pointer and size are equaled to NULL */
        if ((generatorSizeBytes == 0 && generatorG_ptr != NULL) ||
            (generatorSizeBytes != 0 && generatorG_ptr == NULL)) {
                return CC_DH_CHECK_GENERATOR_SIZE_OR_PTR_NOT_VALID_ERROR;
        }




        /* --------------------------------- */
        /*  Step 2.  Initializations         */
        /* --------------------------------- */

        DHKGbuff_ptr = &checkTempBuff_ptr->DhKgBuff;
        TempBuff_ptr = (uint32_t*)&checkTempBuff_ptr->CheckTempBuff;

        /* clean TempBuff_ptr */
        CC_PalMemSetZero(checkTempBuff_ptr, sizeof(CCDhKgCheckTemp_t));

        /* calculate P and Q size in bits */
        modPsizeWords = CALC_32BIT_WORDS_FROM_BYTES(modPsizeBytes);

        /* set Q, P and G- pointers on DHKGbuff_ptr->PrimData temp buffers */
        Q_ptr = TempBuff_ptr;
        P_ptr = Q_ptr + modPsizeWords;

        if (seedS_ptr != NULL) {
                /*--------------------------------------------- */
                /* Step 3. Calculate and check primes sizes     */
                /*--------------------------------------------- */

                /* temporary convert P and Q to little endian bytes arrays  *
                *  for calculating their sizes in bits                 */
                CC_CommonReverseMemcpy((uint8_t*)P_ptr, modP_ptr, modPsizeBytes);
                CC_CommonReverseMemcpy((uint8_t*)Q_ptr, orderQ_ptr, orderQsizeBytes);

                modPsizeBits = CC_MIN(8*modPsizeBytes, CC_CommonGetBytesCounterEffectiveSizeInBits((uint8_t*)P_ptr, (uint16_t)modPsizeBytes));
                orderQsizeBits = CC_MIN(8*orderQsizeBytes, CC_CommonGetBytesCounterEffectiveSizeInBits((uint8_t*)Q_ptr, (uint16_t)orderQsizeBytes));

                /*------------------------------------------------------------------- */
                /* Step 4. Generate random primes P,Q for given seed Seed S according */
                /*         to ANSI X9.42 for comparing with input parameters          */
                /*         The called CreateDomainParams also checks sizes of input   */
                /*         parameters                                                 */
                /*------------------------------------------------------------------- */

                Error =  DhCreateDomainParams(
                                                 rndContext_ptr,
                                                 modPsizeBits,                    /*in */
                                                 orderQsizeBits,                  /*in */
                                                 seedSizeBits,                    /*in */
                                                 (uint8_t*)P_ptr,               /*out*/
                                                 (uint8_t*)Q_ptr,               /*out*/
                                                 NULL /*generatorG_ptr*/,      /*out*/
                                                 NULL /*generatorSize_ptr*/,   /*out*/
                                                 NULL /*factorJ_ptr*/,         /*out*/
                                                 NULL /*JsizeBytes_ptr*/,      /*out*/
                                                 seedS_ptr,                       /*in/out*/
                                                 CC_FALSE /*generateSeed*/,       /*in*/
                                                 &pgenCounter1,                   /*out*/
                                                 DHKGbuff_ptr);                   /*in */

                if (Error != CC_OK) {
                        goto End;
                }

                /* -------------------------------------------------------------------*/
                /* Step 5. Compare generated primes with input, if one of compares   */
                /*         is not "equal", the output error                          */
                /* -------------------------------------------------------------------*/

                if (CC_PalMemCmp(modP_ptr, (uint8_t*)P_ptr, modPsizeBytes) != 0) {
                        Error = CC_DH_CHECK_DOMAIN_PRIMES_NOT_VALID_ERROR;
                        goto End;
                }

                else if (CC_PalMemCmp(orderQ_ptr, (uint8_t*)Q_ptr, orderQsizeBytes ) != 0) {
                        Error = CC_DH_CHECK_DOMAIN_PRIMES_NOT_VALID_ERROR;
                        goto End;
                }

                /* compare pgen counters */
                else if (pgenCounter != pgenCounter1) {
                        Error = CC_DH_CHECK_DOMAIN_PRIMES_NOT_VALID_ERROR;
                        goto End;
                }
        }

        /*-----------------------------------------------------------------*/
        /* Step 4. Check generator using the function for checking of      */
        /*    the public key because both perform identical operations     */
        /*    with appropriate parameters. In this case:                   */
        /*    if G > P-2, or G < 2, or G^Q != 1, then output an error      */
        /*-----------------------------------------------------------------*/

        if (generatorG_ptr != NULL) {
                Error = CC_DhCheckPubKey(
                                           modP_ptr,                                   /*in */
                                           modPsizeBytes,                              /*in */
                                           orderQ_ptr,                                 /*in */
                                           orderQsizeBytes,                            /*in */
                                           generatorG_ptr,                             /*in */
                                           generatorSizeBytes,                         /*in */
                                           &DHKGbuff_ptr->ExpTemps);                   /*in */

                /* Set error code according to checked parameter issue */
                if (Error == CC_DH_INVALID_PUBLIC_KEY_SIZE_ERROR ||
                    Error == CC_DH_INVALID_PUBLIC_KEY_ERROR) {

                        Error =  CC_DH_CHECK_GENERATOR_NOT_VALID_ERROR;
                }
        }

        End:
        /* cleaning of temp buffers */
        CC_PalMemSetZero(&DHKGbuff_ptr->ExpTemps, sizeof(DHKGbuff_ptr->ExpTemps));

        /* End of function */

        return Error;

}

