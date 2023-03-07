/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/************* Include Files ****************/
#include "cc_pal_mem.h"
#include "cc_pal_types_plat.h"
#include "cc_pal_types.h"
#include "cc_rnd_common.h"
#include "cc_common.h"
#include "cc_common_math.h"
#include "cc_rsa_types.h"
#include "cc_hash_defs.h"
#include "mbedtls_cc_hkdf.h"
#include "pki.h"
#include "rsa.h"
#include "cc_ffc_domain_error.h"
#include "cc_ffc_domain.h"
#include "cc_rnd_error.h"
#include "cc_fips_defs.h"
#include "cc_general_defs.h"


/************************ Defines *******************************/

/************************ Enums *********************************/

/************************ macros ********************************/

/** @brief This macro is required to remove compilers warnings if the HASH or PKI is not supported */


/*********************** Global data  ***************************/

//extern CCFfcDomainParamSizes_t ffcDomainParamSizes[CC_FFC_DOMAIN_PARAMS_SET_NUM_OFF_MODE];
//extern CCFfcDhHashBlockAndDigestSizes_t FfcDhHashBlockAndDigestSizes[CC_FFCDH_HASH_NUM_OFF_MODE];

extern CCError_t RsaPrimeTestCall(CCRndContext_t *pRndContext, uint32_t *pPrimeP, int32_t sizeWords,
                                  int32_t rabinTestsCount, int8_t *pIsPrime, uint32_t *pTempBuff,
                  CCRsaDhPrimeTestMode_t primeTestMode);

/************* External functions prototypes  *****************************/

//static
CCError_t RndGenerateWordsArrayInRange(CCRndContext_t *pRndContext,
                      uint32_t   rndSizeInBits,
                      uint32_t  *maxVect_ptr,
                      uint32_t  *rndVect_ptr,
                      uint32_t  *tmp_ptr);

/************************ Private Functions ******************************/

/* This function translates the DH-Hash modes into HASH and KDF-Hash modes
 * and gives HASH block and digest sizes (in bytes). Note: the function sets on output
 * only required parameters, which pointers are not NULL.
 * */
CCError_t FfcGetHashMode(CCHashOperationMode_t *pHashMode, /* optional */
             mbedtls_hkdf_hashmode_t *pHkdfHashMode, /* optional */
             uint32_t *pBlockSize,                /* optional */
             uint32_t *pDigestSize,               /* optional */
             CCFfcHashOpMode_t ffcHashMode)     /* FFC HASH mode */
{
    CCError_t err = CC_OK;
        CCHashOperationMode_t hashMode;
        mbedtls_hkdf_hashmode_t hkdfHashMode;
        size_t blockize, digestSize;

        blockize = CC_HASH_BLOCK_SIZE_IN_BYTES; /* for all modes besides SHA384, SHA512 */
        switch (ffcHashMode) {
        case CC_FFC_HASH_SHA1_MODE:
            hashMode = CC_HASH_SHA1_mode;
            hkdfHashMode = CC_HKDF_HASH_SHA1_mode;
            digestSize = CC_HASH_SHA1_DIGEST_SIZE_IN_BYTES;
                break;
        case CC_FFC_HASH_SHA224_MODE:
            hashMode = CC_HASH_SHA224_mode;
            hkdfHashMode = CC_HKDF_HASH_SHA224_mode;
            digestSize = CC_HASH_SHA224_DIGEST_SIZE_IN_BYTES;
                break;
        case CC_FFC_HASH_SHA256_MODE:
            hashMode = CC_HASH_SHA256_mode;
            hkdfHashMode = CC_HKDF_HASH_SHA256_mode;
            digestSize = CC_HASH_SHA256_DIGEST_SIZE_IN_BYTES;
                break;
        case CC_FFC_HASH_SHA384_MODE:
            hashMode = CC_HASH_SHA384_mode;
            hkdfHashMode = CC_HKDF_HASH_SHA384_mode;
        blockize = CC_HASH_SHA512_BLOCK_SIZE_IN_BYTES;
            digestSize = CC_HASH_SHA384_DIGEST_SIZE_IN_BYTES;
               break;
        case CC_FFC_HASH_SHA512_MODE:
            hashMode = CC_HASH_SHA512_mode;
            hkdfHashMode = CC_HKDF_HASH_SHA512_mode;
            digestSize = CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES;
        blockize = CC_HASH_SHA512_BLOCK_SIZE_IN_BYTES;
                break;
        default:
            return CC_FFC_DOMAIN_INVALID_HASH_MODE_ERROR;
        }

        /* output required parameters only */
        if(pHashMode != NULL)
            *pHashMode = hashMode;
        if(pHkdfHashMode != NULL)
            *pHkdfHashMode = hkdfHashMode;
        if(pDigestSize != NULL)
            *pDigestSize = digestSize;
        if(pBlockSize != NULL)
            *pBlockSize = blockize;

        return err;
}

/*!
 * The function returns FFC Domain parameters sizes according to given ID of
 * approved set of Domain parameters FA,FB or FC (see SP 800-56A and FIPS 186-4 sec.1 standards).
 *
 */
CCError_t FfcGetDomainSizes(
        uint32_t *pMaxSecurStrength,        /*!< Maximum security strength supported, in bits. */
        uint32_t *pPrimeLen,                /*!< Field (prime P) length in bytes. */
        uint32_t *pOrderLen,                /*!< Subgroup order Q length in bytes. */
        uint32_t *pMinHashLen,              /*!< Minimum length of HASH output in bytes. */
        CCFfcParamSetId_t  ffcParamSetId) /*!< Enum. defining set of lengths of domain parameters, approved
                                                     by SP 800-56A and FIPS 186-4 standards */
{
    CCError_t err = CC_OK;
    CCFfcDomainParamSizes_t  *pParamsSet;
    /*! Define and initialize DH domain parameters sizes array */
    CCFfcDomainParamSizes_t ffcDomainParamSizes[(uint32_t)CC_FFC_PARAMS_SET_NUM_OFF_MODE] =
                                           CC_FFC_DOMAIN_PARAM_SIZES_SET;

    if(ffcParamSetId >= CC_FFC_PARAMS_SET_NUM_OFF_MODE)
        return CC_FFC_DOMAIN_INVALID_SIZES_SET_ID_ERROR;

    pParamsSet = &ffcDomainParamSizes[ffcParamSetId];

    if(pMaxSecurStrength != NULL)
        *pMaxSecurStrength = pParamsSet->maxSecurStrength;
    if(pPrimeLen != NULL)
        *pPrimeLen = pParamsSet->primeSize >> 3;
    if(pOrderLen != NULL)
        *pOrderLen = pParamsSet->orderSize >> 3;
    if(pMinHashLen != NULL)
        *pMinHashLen = pParamsSet->minHashLen >> 3;

    return err;
}





/******************************************************************************************/
/************************         Private Functions          ******************************/
/******************************************************************************************/

/********************************************************************************/
/**
 *      The function adds value to the number N, presented as bytes array, where MSbyte
 *      is a most left one.
 *
 *      Algorithm: N = (N + val) mod 2^(8*sizeBytes).
 *      Assumed: The array and its size are aligned to 32-bit words.
 *
 * @return carry from last addition
 */
static
uint8_t FfcAddValueToMsbLsbBytesArray(uint8_t *pArr, uint32_t val, uint32_t sizeBytes)
{
        int32_t i;
        uint32_t word, carry;

        /* check input data */
//        if(((uint32_t)pArr & 3) || (sizeBytes % 4) || (sizeBytes == 0)) {
//          return CC_FFC_DOMAIN_BUFFER_ALIGNMENTS_ERROR;
//        }

        carry = val;
        for (i = sizeBytes - 1; i >= 0; i -= 4) {
            /*set input bytes to word */
            word =  pArr[i-3]; word = (word<<8) | pArr[i-2];
            word = (word<<16) | pArr[i-1]; word = (word<<24) | pArr[i];
            carry += word;
            /* set 4 bytes of sum into array */
            pArr[i] = carry & 0xFF; pArr[i-1] = (carry>>8) & 0xFF;
            pArr[i-2] = (carry>>16) & 0xFF; pArr[i-3] = (carry>>24) & 0xFF;

            carry = (carry < word);
        }

        return (uint8_t)(carry & 1);
}

#define FFC_CMP_BE2LE_FIRST_GREAT   1
#define FFC_CMP_BE2LE_FIRST_SMALL  -1
#define FFC_CMP_BE2LE_EQUALLED      0


/**  The function compares two big numbers, presented by:
 *   - the first as BE bytes array, the second - as LE 32-bit words array.
 *
 *   Note: assumed, that sizes in bytes of both arrays are equalled.
 *         in the first buffer
 */
static
int32_t FfcCmpBeBytes2LeWordsBigNum(uint8_t *pBeBytes, uint32_t *pLeWords, size_t sizeWords)
{
        int32_t i, j, cmp;
        uint32_t word;

        cmp = FFC_CMP_BE2LE_EQUALLED;
        i = 0;
        for (j = sizeWords-1; j >= 0; j--) {
            /*set 4 input bytes to word */
            word =  pBeBytes[i++]; word = (word<<8) | pBeBytes[i++];
            word = (word<<8) | pBeBytes[i++]; word = (word<<8) | pBeBytes[i++];
            if(word > pLeWords[i]) {
                cmp = FFC_CMP_BE2LE_FIRST_GREAT;
            } else if(word < pLeWords[i]){
                cmp = FFC_CMP_BE2LE_FIRST_SMALL;
            }
        }

        return cmp;
}



#ifdef FFC_FURTHER_USING
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

//static
uint32_t FfcGetSizeInBitsOfMsbLsbBytesArray(uint8_t  *arr_ptr,
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

}/* END OF  FfcDhKgGetSizeInBitsOfMsbLsbBytesArray */
#endif


/**
 * The function returns count of Rabin-Miller tests, required for generation
 * primes in FFC DSA and DH algorithms according to FIPS 186-4, sec. C.3: Tab. C.1.
 *
 * If input size not meets one of standard sizes of FFC Prime modulus or Order,
 * then the function returns an error.
 */
static
CCError_t FfcGetCountOfRabMilTests(uint32_t *pCountTests, size_t primeSizeBits)
{
    switch(primeSizeBits) {
    /* sizes set for FFC Order */
    case 160:
        *pCountTests = 19;
        break;
    case 224:
        *pCountTests = 24;
        break;
    case 256:
        *pCountTests = 27;
        break;
    /* sizes set for FFC Prime modulus */
    case 1024:
        *pCountTests = 3;
        break;
    case 2048:
        *pCountTests = 3;
        break;
    case 3072:
        *pCountTests = 2;
        break;
    default:
        return CC_FFC_DOMAIN_INVALID_ARGUMENT_SIZE_ERROR;
    }

    return CC_OK;
}


/**
 * The function checks that ptr != NULL and outSize <= buffSize .
 *
 * In case the user gives both output pointer and size equal to zero,
 * then the function ignores any checking and exits with OK
 */
static
CCError_t FfcCheckPtrAndSize(void *pOut, size_t *pBuffSizeBytes, void *pIn, size_t inSizeBytes) {

    CCError_t err = CC_OK;

    if((pOut == NULL) && (pBuffSizeBytes == NULL)) {
        goto End;
    }

    if((pOut == NULL) || (pIn == NULL)) {
        err = CC_FFC_DOMAIN_INVALID_ARGUMENT_PTR_ERROR;
        goto End; \
    }

    if(inSizeBytes > *pBuffSizeBytes) {
        err = CC_FFC_DOMAIN_INVALID_ARGUMENT_SIZE_ERROR;
        goto End;
    }

End:
    return err;
}


/**
 * The function finds prime Order Q for FFC Domain generation according to FIPS 186-4 sec.A.i.2.
 *
 * Note: Sizes in bytes of P, Q, HASH should be multiple of 4.
 *       Size of pTmp buffer = 3*PrimeSizeWords + orderSizeWords + 1.
 */
static
CCError_t FfcFips186v4FindOrderQ(
            CCFfcDomain_t *pDomain,           /* [in/out]  pointer to FFC Domain. */
            CCRndContext_t *pRndContext,      /* [in] context defining used random function and state. */
            CCFfcGenerateSeed_t generateSeed, /*in*/
            CCFfcDomainTmpBuff_t *pTmpBuff)   /*!< [in] pointer to FFC Domain temp buffer structure. */{

    /* FUNCTION DECLARATIONS */

    /* The return error identifier */
    CCError_t err = CC_OK;

    /* random function given by the user */
    CCRndState_t *pRndState;
    CCRndGenerateVectWorkFunc_t RndGenerateVectFunc;
    uint32_t *pOrder; /*out, LE words */
    uint32_t    orderSizeWords, orderSizeBytes; /*in*/
    uint8_t  *pSeed; /*BE bytes array */
    uint32_t  seedSizeBytes, hashDigestSize;
    /* primality flag (if prime, then isPrime = 1, else 0 ) */
    int8_t  isPrime;
    uint32_t *pHashData, *pTmp1;
    uint32_t countRabMilTests;
    CCHashOperationMode_t hashMode;
    const mbedtls_md_info_t *md_info=NULL;

    /* check some parameters */
    CHECK_AND_SET_ERROR((pDomain == NULL), CC_FFC_DOMAIN_INVALID_DOMAIN_PTR_ERROR);
    CHECK_AND_SET_ERROR((pRndContext == NULL), CC_FFC_DOMAIN_INVALID_RND_CTX_PTR_ERROR);

    /* INITIALIZATION  */

    pRndState = (CCRndState_t*)&(pRndContext->rndState);
    RndGenerateVectFunc = pRndContext->rndGenerateVectFunc;

    pOrder = pDomain->order;
    orderSizeBytes = pDomain->ordLenWords * CC_32BIT_WORD_SIZE;
    orderSizeWords = pDomain->ordLenWords;
    pSeed = pDomain->seed;
    seedSizeBytes = pDomain->seedSizeBytes;
    hashDigestSize = pDomain->hashDigestSize;
    /* set temp buffers */
    pHashData = (uint32_t*)&pTmpBuff->TmpBuff[0]; /*orderSizeWords+1*/
    pTmp1 = pHashData + orderSizeWords + 1; /*buffer 3*PrimeSizeWords*/

    /* FUNCTION  LOGIC */

    /* zeroing word, next after MSWord of Q, for carry, which can occur
     * in P generation function. */
    pOrder[orderSizeWords] = 0;

    /* get count of R-M tests */
    CHECK_ERROR(FfcGetCountOfRabMilTests(&countRabMilTests, orderSizeBytes*CC_BITS_IN_BYTE));
    /* get HASH related parameters */
    CHECK_ERROR(FfcGetHashMode(&hashMode, NULL/*pHkdfHashMode*/, NULL/*pBlockSize*/, NULL, pDomain->ffcHashMode/*in*/));

    md_info = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[hashMode] );
    if (NULL == md_info) {
        err = CC_FFC_DOMAIN_GENERATION_FAILURE_ERROR;
        goto End;
    }

    /*-------------------------------------------------*/
    /*        FFC prime Order generation  loop         */
    /*-------------------------------------------------*/
    while(1) {
    /*     Create random domain seed sec.A.1.1.2.     */
        if(generateSeed == CC_FFC_GENERATE_NEW_SEED) {
            CHECK_ERROR(RndGenerateVectFunc(pRndState, pSeed, seedSizeBytes));
        }

        err = mbedtls_md(md_info, pSeed, seedSizeBytes, (unsigned char *)pHashData);
        if (err) {
             goto End;
        }

        /* copy min(orderSizeBytes, hashDigestSize) bytes from HASH result to order Q buffer:
         * steps 5,6,7 of sec.A.1.1.2 process: U = HASH(seed) mod 2^(OrdSizeInBits-1) */
        if(orderSizeBytes >= hashDigestSize) {
            CC_PalMemCopy((uint8_t*)pOrder + orderSizeBytes - hashDigestSize, (uint8_t*)pHashData, hashDigestSize);
        } else {
            CC_PalMemCopy((uint8_t*)pOrder, (uint8_t*)pHashData + hashDigestSize - orderSizeBytes, orderSizeBytes);
        }
        /* set MS and LS bits of order Q to 1 */
        pOrder[0] |= 0x80000000;
        pOrder[orderSizeWords - 1] |= 0x00000001;

        /* convert pOrder to LE words array for primality testing */
        CHECK_ERROR(CC_CommonConvertMsbLsbBytesToLswMswWords(
                               pOrder, orderSizeBytes, (uint8_t*)pOrder, orderSizeBytes));

        /* test primality of Order */
        CHECK_ERROR(RsaPrimeTestCall(pRndContext, pOrder, orderSizeWords,
                             countRabMilTests, &isPrime, pTmp1,
                             (CCFfcPrimeTestMode_t)CC_FFC_PRIME_TEST_MODE));
        if(isPrime) {
            goto End;
        } else {
            /* if seed is given, but Q is not prime, then return an error */
            CHECK_AND_SET_ERROR((generateSeed == CC_FFC_USE_GIVEN_SEED) && (isPrime == 0),
                        CC_FFC_DOMAIN_GENERATION_FAILURE_ERROR);
        }
    }


    /* End of function */
  End:
    if(err) {
        CC_PalMemSetZero(pDomain, sizeof(CCFfcDomain_t));
        CC_PalMemSetZero(pTmpBuff, sizeof(CCFfcDomainTmpBuff_t));
    }

    return err;

} /* End of FfcFips186v4FindOrderQ */


/**
 * The function finds prime Order Q for FFC Domain generation according to FIPS 186-4 sec.A.i.2.
 *
 * Note: Sizes in bytes of P, Q, HASH should be multiple of 4.
 *       Size in words of pTmp buffer should be not less than
 *          (2MaxModSize + MaxHashSize + 2MaxOrdeSize + 2).
 */
static
CCError_t FfcFips186v4FindPrimeP(
            CCFfcDomain_t *pDomain,           /* [in/out]  pointer to FFC Domain. */
            CCRndContext_t *pRndContext,      /* [in] context defining used random function and state. */
            int8_t *pIsPrime,                 /* [out] pointer to indication, that prime is found. */
            CCFfcDomainTmpBuff_t *pTmpBuff)   /*!< [in] pointer to FFC Domain temp buffer structure. */{

    /* FUNCTION DECLARATIONS */

    /* The return error identifier */
    CCError_t err = CC_OK;

    /* primality flag (if prime, then isPrime = 1, else 0 ) */
    uint32_t  j, offset;
    uint32_t  counter, countBlocks;
    uint32_t  primeSizeBytes, primeSizeWords, remainBytes;
    uint32_t *pOrder, *pPrimeP; /*LE words */
    uint32_t *pCurrPtr;
    uint32_t  orderSizeBytes, orderSizeWords;
    uint8_t  *pHashInput; /*BE bytes array*/
    uint32_t  seedSizeBytes, hashDigestSize;
    uint32_t  countRabMilTests;   /*count of Rabin-Miller tests*/
    /* temp buffers */
    uint32_t *p2Q, *pRemC, *pTmp1;
    uint32_t  addValue;
    CCHashOperationMode_t hashMode;
    const mbedtls_md_info_t *md_info=NULL;

    /* minimal check of some parameters */
    CHECK_AND_SET_ERROR(pDomain == NULL, CC_FFC_DOMAIN_INVALID_DOMAIN_PTR_ERROR);
    CHECK_AND_SET_ERROR(pRndContext == NULL, CC_FFC_DOMAIN_INVALID_RND_CTX_PTR_ERROR);

    /* INITIALIZATION  */

    pPrimeP = pDomain->prime;
    primeSizeWords = pDomain->modLenWords;
    primeSizeBytes = primeSizeWords*CC_32BIT_WORD_SIZE;
    pOrder = pDomain->order;
    orderSizeBytes = pDomain->ordLenWords*CC_32BIT_WORD_SIZE;
    orderSizeWords = orderSizeBytes;
    seedSizeBytes = pDomain->seedSizeBytes;
    hashDigestSize = pDomain->hashDigestSize;

    /* set pointers to temp buffers. Total max size of pTmp buffer in words:
     * (2MaxModSize + MaxHashSize + 2MaxOrdeSize + 2) =  194 words for
     * ModSz=2048bit and OrdSz=256bit, or 258 words for ModSz=3072bit */
    p2Q = (uint32_t*)&pTmpBuff->TmpBuff[0]; /*buffer for 2*Q, size=orderSizeWords+1. */
    pRemC = p2Q + orderSizeWords+1; /*buffer for remainder C of size = orderSizeWords+1.*/
    pHashInput = (uint8_t*)(pRemC + orderSizeWords+1); /*buffer for hash-input data of size = seedSizeWords.*/
    pTmp1 = pRemC + orderSizeWords+1 + seedSizeBytes/CC_32BIT_WORD_SIZE; /*buffer of size = 3*primeSizeWords.*/

    /* n = count of full HASH blocks in prime P. */
    countBlocks = primeSizeBytes / hashDigestSize; /* n */
    remainBytes = primeSizeBytes % hashDigestSize; /*not full block size*/


    /* FUNCTION  LOGIC */

    *pIsPrime = CC_FALSE;
    offset = 1;

    /* get count of R-M tests */
    CHECK_ERROR(FfcGetCountOfRabMilTests(&countRabMilTests, primeSizeBytes*CC_BITS_IN_BYTE));

    /* get HASH related parameters */
    CHECK_ERROR(FfcGetHashMode(&hashMode, NULL/*pHkdfHashMode*/, NULL/*pBlockSize*/, NULL, pDomain->ffcHashMode/*in*/));

    /* calculate p2Q = 2*Q with setting carry into MS word */
    p2Q[orderSizeWords] = CC_CommonAdd2vectors(pOrder, pOrder, orderSizeWords, p2Q/*res*/);
    /* copy seed into hashing buffer */
    CC_PalMemCopy(pHashInput, pDomain->seed, seedSizeBytes);

    md_info = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[hashMode] );
    if (NULL == md_info) {
         err = CC_FFC_DOMAIN_GENERATION_FAILURE_ERROR;
         goto End;
    }

    /* try to generate prime P up to 4*primeSizeInBits times; if not successes,
     * then return an error (sec. A.1.1.2, process: 11). */
    for(counter = 0; counter < 4*primeSizeBytes*CC_BITS_IN_BYTE; counter++) {
        /*  Create primeP using HASH and Seed sec.A.1.1.2.  */
        pCurrPtr = pPrimeP;

        for(j = 0; j < countBlocks; j++) {
            addValue = offset + j;
            FfcAddValueToMsbLsbBytesArray(pHashInput, addValue, seedSizeBytes/CC_32BIT_WORD_SIZE);

            err = mbedtls_md(md_info, pHashInput, seedSizeBytes, (unsigned char*)pCurrPtr);
            if (err) {
                    goto End;
            }

            pCurrPtr += hashDigestSize/CC_32BIT_WORD_SIZE;
        }

        if(remainBytes > 0) {
            addValue = offset + j;
            FfcAddValueToMsbLsbBytesArray(pHashInput, addValue, seedSizeBytes/CC_32BIT_WORD_SIZE);

            err = mbedtls_md(md_info, pHashInput, seedSizeBytes, (unsigned char*)pRemC); /*temporary use pRemC*/
            if (err) {
                    goto End;
            }

            CC_PalMemCopy((uint8_t*)pCurrPtr, (uint8_t*)pRemC, remainBytes);
        }

        /* zero remaining (redundant) last bytes */

        /* set MsBit of prime to 1 */
        ((uint8_t*)pPrimeP)[0] |= 0x80;

        /*zero additional words for carry */
        pPrimeP[primeSizeWords] = 0;
        p2Q[orderSizeWords] = 0;

            /* convert prime to LE words */
        CC_CommonInPlaceConvertBytesWordsAndArrayEndianness(pPrimeP, primeSizeWords);

            /* calculate P so that P = 1 mod(2*Q) */
        /*------------------------------------*/
            /* calculate remainder C = P % 2Q */
        CHECK_ERROR(PkiLongNumDiv(pPrimeP/*numerator*/, primeSizeWords, p2Q/*divider*/,
                orderSizeWords+1, pRemC/*remainder C*/, pTmp1/*DivRes - not used*/));

        /* P = P-(C-1):  if P odd then C=C-1, else P=P+1 and then P=P-C */
        if((pPrimeP[0] & 1UL) == 1) {
            pRemC[0]--;
        } else {
            pPrimeP[0]++;
        }
        CC_CommonSubtractUintArrays(pPrimeP, pRemC, primeSizeWords, pPrimeP/*res*/);

        /* if P>2^(L-1) then test prime, else increase offset and repeat generation */
        if((pPrimeP[primeSizeWords-1] & 0x80000000) != 0) {
            CHECK_ERROR(RsaPrimeTestCall(pRndContext, pPrimeP, primeSizeWords, countRabMilTests,
                    pIsPrime, pTmp1, (CCRsaDhPrimeTestMode_t)CC_FFC_PRIME_TEST_MODE));

            if(*pIsPrime == 1) { /* the prime is found */
                /* set counter into Domain */
                pDomain->genCounter = counter;
                goto End;
            }
        }

        /* repeat generation of prime P with new offset*/
        offset += (countBlocks+1);
    }

        /* if reached this row, then the function exits for further
         * continuing generation of new Q and P */

        /* End of function */
  End:

    if(err) {
        CC_PalMemSetZero(pDomain, sizeof(CCFfcDomain_t));
        CC_PalMemSetZero(pTmpBuff, sizeof(CCFfcDomainTmpBuff_t));
    }

    return err;

} /* End of FfcFips186v4FindPrimeP */


/**
 * The function creates FFC sub-group Generator Q according to FIPS 186-4 sec.A.2.3
 *
 * Note: Sizes in bytes of P, Q, HASH should be multiple of 4.
 *       Size in words of pTmp buffer should be not less than
 *          (2MaxModSize + MaxHashSize + 2MaxOrdeSize + 2).
 */
static
CCError_t FfcFips186v4CreateGenerator(
            CCFfcDomain_t *pDomain,          /* [in/out]  pointer to FFC Domain. */
            CCRndContext_t *pRndContext,     /* [in] context defining used random function and state. */
            uint8_t index,                   /* [in] index of FFC Generator, allowing to generate different
                                                Generators for the same FFC parameters Prime P and Order Q. */
            CCFfcDomainTmpBuff_t *pTmpBuff)  /*!< [in] pointer to FFC Domain temp buffer structure. */{
    /* FUNCTION DECLARATIONS */


    CCError_t err = CC_OK;
    uint16_t  count = 0; /*loop counter*/
    size_t    primeSizeBytes;
    uint32_t  *pPrimeP; /*LE words */
    size_t    hashDataSizeBytes;
    uint8_t  *pIndex;/* point on index inside the hashData*/
    uint8_t   ggen[4] = {0x67,0x67,0x65,0x6E};
    uint32_t  one = 1;
    CCCommonCmpCounter_t cmp;
    CCHashOperationMode_t hashMode;
    const mbedtls_md_info_t *md_info=NULL;

    /* pointers to temp buffers: for variables E, U, W,
     * named in the said standard sec;                       */
    uint32_t  *pDivResE/*e*/, *pHashDataU/*u*/, *pHashResW/*W*/, *pTmp1/*dummy*/;

    CHECK_AND_SET_ERROR((pDomain == NULL), CC_FFC_DOMAIN_INVALID_DOMAIN_PTR_ERROR);
    CHECK_AND_SET_ERROR((pRndContext == NULL), CC_FFC_DOMAIN_INVALID_RND_CTX_PTR_ERROR);
    CHECK_AND_SET_ERROR((pDomain->validTag != CC_FFC_DOMAIN_VALIDATION_TAG),
                        CC_FFC_DOMAIN_VALIDATION_TAG_ERROR);

        /* FUNCTION  LOGIC */

    primeSizeBytes = pDomain->modLenWords * CC_32BIT_WORD_SIZE;
    pPrimeP = pDomain->prime;

    /* set pointers and calculate W = HASH of U */

    pHashResW = (uint32_t*)&pTmpBuff->TmpBuff[0]; /* buffer size CC_FFC_DOMAIN_MAX_GENER_ORDER_SIZE_IN_BYTES + 12 */
    pHashDataU = pHashResW + pDomain->hashDigestSize / CC_32BIT_WORD_SIZE;


    /* repeat generation while count != 0 */
    count = 1;

    while(count != 0) {
        /*-----------------------------------------*/
        /* set U data = Seed||"ggen"||index||count */
        /*-----------------------------------------*/
        pIndex = (uint8_t*)pHashDataU; /*now used as temporary pointer*/
        CC_PalMemCopy(pIndex, pDomain->seed, pDomain->seedSizeBytes);
        pIndex += pDomain->seedSizeBytes;
        CC_PalMemCopy(pIndex, ggen, sizeof(ggen));
        pIndex += sizeof(ggen);
        *pIndex = index;
        /* copy counter after address, pointed by pIndex*/
        pIndex[1] = count & 0xFF; pIndex[2] = (count>>8) & 0xFF;
        hashDataSizeBytes = pDomain->seedSizeBytes + sizeof(ggen) + sizeof(count) + 1/*index*/;

        /* get HASH related parameters */
        CHECK_ERROR(FfcGetHashMode(&hashMode, NULL/*pHkdfHashMode*/, NULL/*pBlockSize*/, NULL, pDomain->ffcHashMode/*in*/));

        md_info = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[hashMode] );
        if (NULL == md_info) {
             err = CC_FFC_DOMAIN_GENERATION_FAILURE_ERROR;
             goto End;
        }

        /* calculate W = HASH(U) */
        err = mbedtls_md(md_info, (uint8_t*)pHashDataU, hashDataSizeBytes, (unsigned char *)pHashResW);
        if (err) {
                goto End;
        }

        /* convert Hash result to LE words array */
        CC_CommonConvertMsbLsbBytesToLswMswWords(pHashResW, pDomain->hashDigestSize,
                                     (uint8_t*)pHashResW, pDomain->hashDigestSize);

        /*--------------------------*/
        /*   calculate  e = P / Q   */
        /*--------------------------*/

        pDomain->prime[primeSizeBytes-1] ^= 1; /* temporary: P = p-1;*/
        pDivResE = pHashDataU; /* pHashDataU not used now */
        pTmp1 = pDivResE + pDomain->ordLenWords;

        /* e = (p-1) / q */
        CHECK_ERROR(PkiLongNumDiv(pPrimeP/*numerator*/, pDomain->modLenWords,
                      pDomain->order/*divider*/, pDomain->ordLenWords,
                      pTmp1/*remainder C*/, pDivResE/*DivRes - not used*/));
        pDomain->prime[primeSizeBytes-1] ^= 1; /*reset: P = P+1*/

        /* calculate Generator g = W^e mod P */
        err = PkiExecModExpLeW(
                pDomain->genG, /*G res*/
                pHashResW, /*W*/
                pDomain->ordLenWords, /*W size*/
                &pDomain->prime[0],  /*prime P - modulus*/
                pDomain->modLenWords*CC_BITS_IN_32BIT_WORD, /*P size in bits*/
                pDivResE, /*E exponent*/
                pDomain->ordLenWords); /* Q size in words*/

        /* check error */
        if (err != CC_OK) {
            goto End;
        }

        /* check that g > 1, else repeat generation with count++. */
        cmp = CC_CommonCmpLsWordsUnsignedCounters(pDomain->genG, pDomain->modLenWords, &one, sizeof(one));
        if(cmp == CC_COMMON_CmpCounter1GreaterThenCounter2) {
            /* set results into Domain */
            pDomain->indexOfGenerator = index;
            break;
        }
        count++;
    }

        /* End of function */
  End:

    if(err) {
        CC_PalMemSetZero(pDomain, sizeof(CCFfcDomain_t));
        CC_PalMemSetZero((uint8_t*)pTmpBuff, sizeof(CCFfcDomainTmpBuff_t));
    }

        return err;

} /* End of FfcFips186v4FindPrimeP */


/** The function performs preliminary checking of Domain generation input parameters.
 *
 *   This checking is related only to pointers and sizes of input parameters in
 *   accordance with  NIST SP 56A rev.2, referring to FIPS 184-4 standards, but not
 *   means standard full validation of them.
 *
 */
static CCError_t FfcDomainMinCheckInput(
                CCFfcDomain_t *pDomain,          /*!< [out] pointer to  FFC Domain structure. */
                CCRndContext_t *pRndContext,     /*!< [in] random generation function context. */
                size_t primeSizeBits,            /*!< [in] size of domain's prime modulus in bits (see requirements above). */
                size_t orderSizeBits,            /*!< [in] size of domain's sub-group order in bits. The size should meet
                                                           to one of allowed sizes according given. */
                uint8_t *pSeed,                  /*!< [in] optional pointer to the seed for domain generation and validation.
                                                           If Seed should be generated internally, then Seed pointer and size
                                                           should be zero. */
                size_t seedSizeBytes,            /*!< [in] optional seed size in bytes */
                CCFfcGenerateSeed_t generateSeed,/*!< [in] enumerator indicates to generate a new domain Seed. */
                uint32_t genCounter,             /*!< [in] optional value of count of iterations, required for generation
                               of FFC Domain from given Seed. If Seed is given, and actually calculated
                               count is not equal to given, then the function returns an error.
                               If a new Seed is required (i.e. the counter is not known), then input
                               genCounter should be set 0. */
                CCFfcParamSetId_t ffcParamSetId, /*!< [in] enumerator, defining the set of FFC domain parameters
                                                           according to SP 56A rev.2 section 5.5.1.1, tab.1. */
                CCFfcHashOpMode_t ffcHashMode,   /*!< [in] enumerator ID of used SHAXXX HASH mode, supported by the product.
                                                           Note: HASH SHA1 function may be used only with SA set of domain parameters
                                                           (sec. 5.8.1, tab.6); with other sets the function returns an error. */
               uint8_t indexOfGener             /*!< [in] index of FFC Generator, allowing to generate different FFC generators with
                                                   the same FFC parameters (Prime and Order), existed in the domain. */
)
{
    /* The return error identifier */
    CCError_t err = CC_OK;

    uint32_t hashMinLen;
    uint32_t primeSizeBytes, orderSizeBytes;
    CCHashOperationMode_t hashMode;

    /* Step 1. Check input parameters */
    /*------------------------------- */
    CHECK_AND_SET_ERROR((pDomain == NULL), CC_FFC_DOMAIN_INVALID_DOMAIN_PTR_ERROR);
    CHECK_AND_SET_ERROR((pRndContext == NULL), CC_RND_CONTEXT_PTR_INVALID_ERROR);
    CHECK_AND_SET_ERROR((pRndContext->rndGenerateVectFunc == NULL), CC_FFC_DOMAIN_INVALID_RND_FUNCTION_PTR_ERROR);
    /* check enumerators */
    CHECK_AND_SET_ERROR(ffcParamSetId >= CC_FFC_PARAMS_SET_NUM_OFF_MODE, CC_FFC_DOMAIN_INVALID_HASH_MODE_ERROR);
    CHECK_AND_SET_ERROR(ffcParamSetId >= CC_FFC_PARAMS_SET_NUM_OFF_MODE, CC_FFC_DOMAIN_INVALID_HASH_MODE_ERROR);
    CHECK_AND_SET_ERROR(ffcParamSetId >= CC_FFC_PARAMS_SET_NUM_OFF_MODE, CC_FFC_DOMAIN_INVALID_SEED_GENERATION_MODE_ERROR);

    /* check seed and generation counter */
    if((generateSeed == CC_FFC_GENERATE_NEW_SEED) || (generateSeed == CC_FFC_SEED_NOT_USED)) {
    /* case that the seed and generation counter not known and not given. */
    CHECK_AND_SET_ERROR(pSeed != NULL, CC_FFC_DOMAIN_SEED_IS_NOT_REQUIRED_ERROR);
    CHECK_AND_SET_ERROR(genCounter != 0, CC_FFC_DOMAIN_GEN_COUNTER_NOT_VALID_ERROR);
    } else {
        /* case of actually used input of seed and counter */
        CHECK_AND_SET_ERROR(pSeed == NULL, CC_FFC_DOMAIN_INVALID_SEED_PTR_ERROR);
        CHECK_AND_SET_ERROR(genCounter == 0, CC_FFC_DOMAIN_GEN_COUNTER_NOT_VALID_ERROR);
    }

    /* get HASH mode and sizes, FFC sizes set and insert them into Domain. */
    CHECK_AND_SET_ERROR(FfcGetHashMode(&hashMode, NULL/*pHkdfHashMode*/,
                &pDomain->hashBlockSize, &pDomain->hashDigestSize, ffcHashMode/*in*/),
                          CC_FFC_DOMAIN_INVALID_HASH_MODE_ERROR);
    CHECK_AND_SET_ERROR(FfcGetDomainSizes(NULL/*pMaxSecurStrength*/, &primeSizeBytes,
                          &orderSizeBytes, &hashMinLen, ffcParamSetId/*in*/),
                                  CC_FFC_DOMAIN_INVALID_SIZES_SET_ID_ERROR);

    /* check that FFC Prime and Order sizes are meet to given ffcParamSetId */
    CHECK_AND_SET_ERROR(primeSizeBits != primeSizeBytes*CC_BITS_IN_BYTE, CC_FFC_DOMAIN_INVALID_PRIME_SIZE_ERROR);
    CHECK_AND_SET_ERROR(orderSizeBits != orderSizeBytes*CC_BITS_IN_BYTE, CC_FFC_DOMAIN_INVALID_ORDER_SIZE_ERROR);

    /* check Hash and Seed sizes according to required FFC parameters Set ID */
    CHECK_AND_SET_ERROR(pDomain->hashDigestSize < hashMinLen, CC_FFC_DOMAIN_INVALID_LOW_HASH_SIZE_ERROR);
    CHECK_AND_SET_ERROR((seedSizeBytes < orderSizeBytes) || (seedSizeBytes > CC_FFC_DOMAIN_SEED_MAX_SIZE_IN_BYTES),
                 CC_FFC_DOMAIN_INVALID_SEED_SIZE_ERROR);

    /* set other parameters into FFC Domain */
    pDomain->ffcParamSetId = ffcParamSetId;
    pDomain->ffcHashMode = ffcHashMode;
    pDomain->indexOfGenerator = indexOfGener;
    pDomain->seedSizeBytes = seedSizeBytes;
    pDomain->modLenWords = primeSizeBytes/CC_32BIT_WORD_SIZE;
    pDomain->ordLenWords = orderSizeBytes/CC_32BIT_WORD_SIZE;
    pDomain->seedSizeBytes = seedSizeBytes;
    pDomain->genCounter = genCounter;

    /* End of function */
End:

    return err;

}



/***********************************************************************/
/*!
@brief This function generates FFC domain parameters according to NIST SP 56A rev.2, referring to FIPS 184-4 standard.
\par<ol><li>
<li> The function generates FFC Domain from given Seed or generates a new random Seed and then the Domain, depending on
geneSeedId value: 0,1 (see type definition). </li>
<li> The function calculates prime modulus P, subgroup generator G with order Q using Seed and HASH function and
parameters and then compares count of performed iterations with given right value. In other words, the function recalculates
previously generated FFC domain from given Seed (sections SP 56A rev.2 5.5.1.1 and FIPS 184-4 A.1.1.2, A.1.1.3, A.2.3). </li>
<li> The function allows generation domains with approved set of parameters sizes (SP 56A rev.2 5.5.1.1), given by
enumerator CCFfcParamSetId_t. </li>
<li> The function validates received FFC domain parameters and sets them into Domain structure. </li></ol>
@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h, cc_rnd_error.h.
 */
//static
CCError_t FfcFips186v4GenerateDomain(
                                CCFfcDomain_t *pDomain,          /*!< [out] pointer to  FFC Domain structure. */
                                CCRndContext_t *pRndContext,     /*!< [in] random generation function context. */
                                size_t primeSizeBits,            /*!< [in] size of domain's prime modulus in bits (see requirements above). */
                                size_t orderSizeBits,            /*!< [in] size of domain's sub-group order in bits. The size should meet
                                                                           to one of allowed sizes according given. */
                                uint8_t *pSeed,                  /*!< [in] optional pointer to the seed for domain generation and validation.
                                                                           If Seed should be generated internally, then Seed pointer and size
                                                                           should be zero, because the Seed will be saved in the domain. */
                                size_t seedSizeBytes,            /*!< [in] seed size in bytes, should meets to given ffcParamSetId. */
                                CCFfcGenerateSeed_t generateSeed,/*!< [in] enumerator indicates to generate a new domain Seed. */
                                uint32_t genCounter,             /*!< [in] optional value of count of iterations, required for generation
                                                                       of FFC Domain from given Seed. If Seed is given, and actually calculated
                                                                       count is not equal to given, then the function returns an error.
                                                                       If a new Seed is required (i.e. the counter is not known), then input
                                                                       genCounter should be set 0. */
                                CCFfcParamSetId_t ffcParamSetId, /*!< [in] enumerator, defining the set of FFC domain parameters
                                                                      according to SP 56A rev.2 section 5.5.1.1, tab.1. */
                                CCFfcHashOpMode_t ffcHashMode,   /*!< [in] enumerator ID of used SHAXXX HASH mode, supported by the product.
                                                                      Note: HASH SHA1 function may be used only with SA set of domain parameters
                                                                      (sec. 5.8.1, tab.6); with other sets the function returns an error. */
                                uint8_t generIndex,              /*!< [in] index of FFC Generator, allowing to generate different FFC generators with
                                                                      the same FFC parameters prime P and Order Q, existed in the domain. */
                                CCFfcDomainTmpBuff_t *pTmpBuff   /*!< [in] pointer to FFC Domain temp buffer structure. */
)
{

    /* FUNCTION DECLARATIONS */

    /* The return error identifier */
    CCError_t err = CC_OK;
    int8_t isPrime; /* indication, that prime is found (0, 1). */


    /* FUNCTION  LOGIC */

    /* preliminary checking of input pointers and sizes according to given ffcParamSetId
     * and ffcHashMode; save checked data into Domain structure */
    CHECK_ERROR(FfcDomainMinCheckInput(pDomain, pRndContext, primeSizeBits, orderSizeBits,
                                  pSeed, seedSizeBytes, generateSeed, genCounter,
                                  ffcParamSetId, ffcHashMode, generIndex));
        /* check temp buffer */
    CHECK_AND_SET_ERROR((pTmpBuff == NULL), CC_FFC_DOMAIN_INVALID_ARGUMENT_PTR_ERROR);

    /* main loop of FFC domain generation:
     * !!! Note: the loop, according to Standard algorithm, is not limited
     * therefore we need insert an  WatchDog timer. */
    while(1) {
        /* generate FFC Order Q */
        CHECK_ERROR(FfcFips186v4FindOrderQ(pDomain, pRndContext, generateSeed, pTmpBuff));
        /* generate FFC Prime modulus P */
        CHECK_ERROR(FfcFips186v4FindPrimeP(pDomain, pRndContext, &isPrime, pTmpBuff));

        /* check that both P,Q are prime */
        if(isPrime == 1) {
            break;
        }
    }

    /* if the Seed is given, then check iterations counter  */
    CHECK_AND_SET_ERROR((generateSeed == CC_FFC_USE_GIVEN_SEED) && (pDomain->genCounter != genCounter),
                CC_FFC_DOMAIN_GENERATION_FAILURE_ERROR);

    /* create FFC Generator with given Index */
    CHECK_ERROR(FfcFips186v4CreateGenerator(pDomain,  pRndContext, generIndex, pTmpBuff));

    /* End of function */
  End:
    if(err) {
        CC_PalMemSetZero(pDomain, sizeof(CCFfcDomain_t));
        CC_PalMemSetZero(pTmpBuff, sizeof(CCFfcDomainTmpBuff_t));
    }

        return err;

}





/*******************************************************************************************/
/*!
@brief This function generates FFC domain parameters according to NIST SP 56A rev.2, referring to FIPS 184-4 standard.
\par<ol><li>
<li> The function generates FFC Domain from given Seed and iterations count and sets them into Domain structure.
If actual count of iterations is not equalled to given value, then the function returns an error. </li>
<li> The function calculates prime modulus P, subgroup generator G with order Q using Seed and given Generator
index, allowing to generate different FFC generators with same P and Q, according to SP 56A rev.2 sec.5.5.1.1
and FIPS 184-4 A.1.1.2, A.2.3. </li>
<li> The function allows generation domains only for approved set of parameters sizes (SP 56A rev.2 5.5.1.1),
given by enumerator ID of type CCFfcParamSetId_t. </li></ol>
@return CC_OK on success.
@return A non-zero value on failure as defined cc_ffc_domain_error.h.

*/
CEXPORT_C CCError_t CC_FfcGenerateDomainFromSeed(
                CCFfcDomain_t *pDomain,        /*!< [out] pointer to  FFC Domain structure. */
                CCRndContext_t *pRndContext,   /*!< [in] random generation function context. */
                size_t primeSizeBits,          /*!< [in] size of domain's prime modulus in bits (see requirements above). */
                size_t orderSizeBits,          /*!< [in] size of domain's sub-group order in bits (see requirements above). */
                uint8_t  *pSeed,               /*!< [in] pointer to the seed for domain generation and validation; */
                size_t   seedSizeBytes,        /*!< [in] seed size in bytes */
                uint32_t genCounter,           /*!< [in] exact value of count of main loop iterations, required for generation
                                     FFC Domain from given Seed. If actual count is not equal to given,
                                     then the function returns an error. */
                CCFfcParamSetId_t ffcParamSetId,/*!< [in] enumerator, defining the set of FFC domain parameters
                                     according to SP 56A rev.2 section 5.5.1.1, tab.1. */
                CCFfcHashOpMode_t ffcHashMode, /*!< [in] enumerator ID of SHAx HASH mode. Note: HASH SHA1 mode may be
                                     used only with SA set of domain parameters (sec. 5.8.1, tab.6). */
                uint8_t generIndex,            /*!< [in] an index of FFC Generator,  allowing to generate different FFC generators with
                                    the same FFC parameters prime P and Order Q, existed in the domain. */
                CCFfcDomainTmpBuff_t *pTmpBuff /*!< [in] pointer to FFC Domain temp buffer structure. */
)
{
        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t err = CC_OK;

        /* call generation function */
        err = FfcFips186v4GenerateDomain(
            pDomain, pRndContext, primeSizeBits, orderSizeBits,
                        pSeed, seedSizeBytes, CC_FFC_USE_GIVEN_SEED, genCounter,
                        ffcParamSetId, ffcHashMode, generIndex, pTmpBuff);

        if(err == CC_OK) {
            pDomain->validTag = CC_FFC_DOMAIN_VALIDATION_TAG;
        }

        return err;
}


/*******************************************************************************************/
/*!
@brief This function generates FFC Domain parameters including new Seed Seed according to
 NIST SP 56A rev.2 with referring to FIPS 184-4 standard.
\par<ol><li>
<li> The function generates a new Seed, calculates FFC Domain parameters and sets them into Domain. </li>
<li> The function calculates prime modulus P, subgroup generator G with order Q using Seed and given Generator
index, allowing to generate different FFC generators with same P and Q, according to SP 56A rev.2 sec.5.5.1.1
and FIPS 184-4 A.1.1.2, A.2.3. </li>
<li> The function allows generation Domain only for approved set of parameters sizes (SP 56A rev.2 5.5.1.1),
given by enumerator ID of type CCFfcParamSetId_t. </li></ol>
@return CC_OK on success.
@return A non-zero value on failure as defined cc_ffc_domain_error.h.

*/
CEXPORT_C CCError_t CC_FfcGenerateDomainAndSeed(
                CCFfcDomain_t *pDomain,         /*!< [out] pointer to  FFC Domain structure. */
                CCRndContext_t *pRndContext,    /*!< [in] random generation function context. */
                size_t primeSizeBits,           /*!< [in] size of domain's prime modulus in bits (see requirements above). */
                size_t orderSizeBits,           /*!< [in] size of domain's sub-group order in bits (see requirements above). */
                size_t seedSizeBytes,           /*!< [in] required size of the seed in bytes; it must be not less than
                                       HASH security strength, defined in given ffcParamsSet. */
                CCFfcParamSetId_t ffcParamSetId,/*!< [in] enumerator, defining the set of FFC domain parameters
                                          according to SP 56A rev.2 section 5.5.1.1, tab.1. */
                CCFfcHashOpMode_t ffcHashMode,  /*!< [in] enumerator ID of SHAx HASH mode. Note: HASH SHA1 mode may be
                                      used only with SA set of domain parameters (sec. 5.8.1, tab.6). */
                uint8_t generIndex,             /*!< [in] an index of FFC Generator, allowing to generate different FFC generators with
                                          the same FFC parameters prime P and Order Q, existed in the domain. */
                CCFfcDomainTmpBuff_t *pTmpBuff  /*!< [in] pointer to FFC Domain temp buffer structure. */
)
{
        /* FUNCTION DECLARATIONS */

        /* The return error identifier */
        CCError_t err = CC_OK;

        /* call generation function */
        err = FfcFips186v4GenerateDomain(
            pDomain, pRndContext, primeSizeBits, orderSizeBits, NULL/*pSeed*/,
            seedSizeBytes, CC_FFC_GENERATE_NEW_SEED, 0/*generCounter*/,
                        ffcParamSetId, ffcHashMode, generIndex, pTmpBuff);

        if(err == CC_OK) {
            pDomain->validTag = CC_FFC_DOMAIN_VALIDATION_TAG;
        }

        return err;
}

//static
//CCError_t

/*******************************************************************************************/
/*!
@brief The function validates received FFC domain parameters and sets them into Domain structure.
<ol><li> Validation of performed according to NIST SP 56A rev.2, sec. 5.5.2 and to FIPS 184-4 standard. </li>
</li> If optional parameters (Seed and pgenCounter) are given, then the function performs full validation by generation
primes P,Q from the given Seed and compares calculated and received parameters according to the FIPS 184-4, A.1.1.3. </li>
</li> Generator G is validated according to sec. A.2.3. </li>
</li> If optional parameters pSeed, seedSize, pgenCounter are zero, and the user explicitly sets validation mode to
"Trusted Data", then the function performs only checking of pointers, sizes and some relations between parameters. <li>.
</li> All  input byte-arrays should be set with big endianness order of bytes, i.e. MS Byte is a leftmost one. </li></ol>
@return CC_OK on success.
@return A non-zero value on failure, as defined in cc_dh_error.h, cc_rnd_error.h.
 */
CEXPORT_C CCError_t CC_FfcValidateAndImportDomain(
                CCFfcDomain_t *pDomain,          /*!< [out] optional (used on Full Validation mode only), pointer to FFC Domain. */
                CCRndContext_t *pRndContext,     /*!< [in] optional (used on Full Validation mode only), random generation
                                                           function context. */
                uint8_t *pPrime,                 /*!< [in] pointer to prime modulus of the finite field (P). */
                size_t  primeSizeBits,           /*!< [in] prime P size in bits. */
                uint8_t *pOrder,                 /*!< [in] pointer to the order Q of the generator. */
                size_t  orderSizeBits,           /*!< [in] order size in bits. */
                uint8_t *pGenerator,             /*!< [in] pointer to generator G of subgroup of FFC. */
                size_t  generSizeBytes,          /*!< [in] generator G size in bytes (see note bellow). */
                uint8_t *pSeed,                  /*!< [in] optional (used on Full Validation mode only), pointer to the Seed,
                                                           if the Seed is not given, then should be set to NULL. */
                size_t  seedSizeBytes,           /*!< [in] optional size of Seed in bytes; if Seed not given, then
                                       should be set to 0. */
                CCFfcParamSetId_t ffcParamSetId, /*!< [in] enumerator, defining the set of FFC domain parameters
                                       according to SP 56A rev.2 section 5.5.1.1, tab.1. */
                CCFfcHashOpMode_t ffcHashMode,   /*!< [in] enumerator ID of SHAx HASH mode. Note: HASH SHA1 mode may be
                                       used only with SA set of domain parameters (sec. 5.8.1, tab.6). */
                uint32_t genCounter,             /*!< [in] optional, counter of main iterations loop, performed during
                                       domain generation with Seed. */
                uint8_t generIndex,              /*!< [in] an index of FFC Generator, allowing to generate different FFC generators with
                                                          the same FFC parameters prime P and Order Q, existed in the domain. */
                CCFfcDomainValidMode_t validMode,/*!< [in] enumerator, defining validation mode of of domain parameters:
                                       "full" (approved by FIPS standard), "partial"
                                       and "trusted" (validated previously); using of both second
                                       modes is not approved by standards and is fully on the user
                                       responsibility. */
                CCFfcDomainTmpBuff_t *pTmpBuff   /*!< [in] optional pointer to FFC Domain temp buffer structure. Used only
                                                           on Full validation mode, on Trusted mode may be set to NULL. */

)
{
    /* FUNCTION DECLARATIONS */

    /* The return error identifier */
    CCError_t err = CC_OK;
    size_t primeSizeBytes, orderSizeBytes;


    /*      INITIALIZATIONS       */

    primeSizeBytes = primeSizeBits/CC_BITS_IN_BYTE;
    orderSizeBytes = orderSizeBits/CC_BITS_IN_BYTE;

    /* check parameters */
    CHECK_AND_SET_ERROR((pGenerator == NULL) != (generSizeBytes == 0), CC_FFC_DOMAIN_INVALID_OPTIONAL_PARAM_ERROR);
    CHECK_AND_SET_ERROR(validMode >= CC_FFC_DOMAIN_VALIDAT_NUM_OFF_MODE, CC_FFC_DOMAIN_INVALID_VALIDAT_MODE_ERROR);

    /*----------------------------------------------------------------------------*
     *                 case of full validation mode                               *
     *--------------------------------------------------------------------------- */
    if(validMode == CC_FFC_DOMAIN_VALIDAT_FULL_MODE) {
        CHECK_AND_SET_ERROR(pSeed == NULL, CC_FFC_DOMAIN_INVALID_SEED_PTR_ERROR);

        /* generate Domain from given Seed. */
        CHECK_ERROR(FfcFips186v4GenerateDomain(
            pDomain, pRndContext, primeSizeBits, orderSizeBits, pSeed, seedSizeBytes,
            CC_FFC_USE_GIVEN_SEED, genCounter, ffcParamSetId, ffcHashMode, generIndex, pTmpBuff));

        /* compare input data with generated parameters */
        CHECK_AND_SET_ERROR(FfcCmpBeBytes2LeWordsBigNum(pPrime, pDomain->prime, primeSizeBytes/CC_32BIT_WORD_SIZE),
                    CC_FFC_DOMAIN_PRIME_NOT_VALID_ERROR);
        CHECK_AND_SET_ERROR(FfcCmpBeBytes2LeWordsBigNum(pOrder, pDomain->order, orderSizeBytes/CC_32BIT_WORD_SIZE),
                    CC_FFC_DOMAIN_ORDER_NOT_VALID_ERROR);
//      CHECK_AND_SET_ERROR(CC_PalMemCmp(pSeed, pDomain->seed, orderSizeBytes/CC_32BIT_WORD_SIZE),
//                  CC_FFC_DOMAIN_SEED_IS_NOT_VALID_ERROR);
        CHECK_AND_SET_ERROR(genCounter != pDomain->genCounter, CC_FFC_DOMAIN_GEN_COUNTER_NOT_VALID_ERROR);

        /* create FFC sub-group generator G with given index */
        if(pGenerator != NULL) {
            CHECK_ERROR(FfcFips186v4CreateGenerator(pDomain, pRndContext, generIndex, pTmpBuff));
            /* check the generator */
            CHECK_AND_SET_ERROR(FfcCmpBeBytes2LeWordsBigNum(pGenerator, &pDomain->genG[0], primeSizeBytes),
                        CC_FFC_DOMAIN_GENERATOR_NOT_VALID_ERROR);
        }
    }
    /*--------------------------------------------------------------------------------------
     * the case, that the user has obtained insurance, that all Domain input data is correct,
     * therefore the following checking is minimal and not means validation of the Domain. *
     *-------------------------------------------------------------------------------------*/
    else if (validMode == CC_FFC_DOMAIN_TRUSTED_DATA_MODE) {

            /* check input pointers and sizes according to given ffcParamSetId and ffcHashMode */
            CHECK_ERROR(FfcDomainMinCheckInput(pDomain, pRndContext, primeSizeBits, orderSizeBits,
                                          pSeed, seedSizeBytes, CC_FFC_USE_GIVEN_SEED, genCounter,
                                          ffcParamSetId, ffcHashMode, generIndex));

            /* check Prime, Order and Generator  */
            CHECK_AND_SET_ERROR((pPrime == NULL) || (pOrder == NULL)  || (pGenerator == NULL),
                        CC_FFC_DOMAIN_INVALID_ARGUMENT_PTR_ERROR);

            /* set data into Domain as LE words array */
            CC_CommonConvertMsbLsbBytesToLswMswWords(pDomain->prime, primeSizeBytes, pPrime, primeSizeBytes);
            CC_CommonConvertMsbLsbBytesToLswMswWords(pDomain->order, orderSizeBytes, pOrder, primeSizeBytes);
            CC_CommonConvertMsbLsbBytesToLswMswWords(pDomain->genG, primeSizeBytes, pGenerator, primeSizeBytes);
            if(pSeed != NULL) {/*set data as is */
                CC_PalMemCopy(pDomain->seed, pSeed, seedSizeBytes);
            }
            /* Note: part of settings is done by FfcDomainMinCheckInput() function. */
    }

    if(err == CC_OK) {
        pDomain->validTag = CC_FFC_DOMAIN_VALIDATION_TAG;
    }

End:

    if(err) {
        CC_PalMemSetZero(pDomain, sizeof(CCFfcDomain_t));
        if(pTmpBuff != NULL) {
            CC_PalMemSetZero((uint8_t*)pTmpBuff, sizeof(CCFfcDomainTmpBuff_t));
        }
    }

        return err;
}



/*******************************************************************************************/
/*!
@brief This function extracts FFC domain parameters from Domain structure for external using.
<ol><li> Assumed, that FFC domain is properly generated or validated according to the FIPS 184-4, standard. </li>
<li> The function checks input/output pointers and buffers sizes, converts the DH Domain parameters
to big endianness output arrays (with leading zeros if exists). </li>
<li> If the user not need any domain parameters, then appropriate pointers (for buffer and size)
should be set to NULL </li>
<li> Note: The sizes of buffers are given by pointers, were the referred [in/out] values are:
[in] - the buffer size, [out] - actual data size. </li></ol>

@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h, cc_rnd_error.h.
*/
CEXPORT_C CCError_t CC_FfcExportDomain(
                CCFfcDomain_t *pDomain,          /*!< [in] pointer to FFC Domain to be exported. */
                uint8_t *pPrime,                 /*!< [out] pointer to prime modulus of the finite field (P). */
                size_t  *pPrimeSize,             /*!< [in/out] pointer to prime P size in bytes. */
                uint8_t *pGenerator,             /*!< [out] pointer to generator of subgroup (G). */
                size_t  *pGeneratorSize,         /*!< [in/out] pointer to generator G size in bytes. */
                uint8_t *pOrder,                 /*!< [out] pointer to the order of the generator G. */
                size_t  *pOrderSize,             /*!< [in/out] pointer to order of generator Q size in bytes. */
                uint8_t *pSeed,                  /*!< [out] pointer to the Seed, used for Domain generation;
                                                      if Seed is not required, then the pointer and size should be NULL. */
                size_t  *pSeedSize,              /*!< [in/out] size of the Seed in bytes - if the Seed not exist,
                                                      in the Domain, the function sets the size = 0. */
                CCFfcParamSetId_t *pFfcParamSetId, /*!< [in] pointer to enumerator ID, defining the set of FFC domain parameters
                                                      parameters according to SP 56A rev.2 section 5.5.1.1, tab.1. */
                CCFfcHashOpMode_t *pFfcHashMode, /*!< [in] pointer to enumerator ID of SHAx HASH mode. Note: HASH SHA1 mode
                                                      may be used only with SA set of domain parameters (sec. 5.8.1, tab.6). */
                uint32_t *pGenCounter,           /*!< [out] pointer to count of iterations, which were performed
                                                      during Domain generation. */
                uint8_t  *pIndexOfGenerator      /*!< pointer to index, of  FFC Generator existed in the Domain. */
)
{

        /* The return error identifier */
    CCError_t err = CC_OK;
    size_t primeSizeBytes;
    size_t orderSizeBytes;

    /* check input Domain */
    CHECK_AND_SET_ERROR((pDomain == NULL), CC_FFC_DOMAIN_INVALID_DOMAIN_PTR_ERROR);
    CHECK_AND_SET_ERROR((pDomain->validTag != CC_FFC_DOMAIN_VALIDATION_TAG),
                         CC_FFC_DOMAIN_VALIDATION_TAG_ERROR);

    primeSizeBytes = pDomain->modLenWords * CC_32BIT_WORD_SIZE;
    orderSizeBytes = pDomain->ordLenWords * CC_32BIT_WORD_SIZE;

    /*----------------------------------------------------------------------------------------*
     * Check output buffers: ptr != NULL, outSize <= buffSize and set data to output buffers. *
     *----------------------------------------------------------------------------------------*/
    /* Prime P */
    if((pPrime != NULL) && (pPrimeSize != NULL)) {
        CHECK_ERROR(FfcCheckPtrAndSize(pPrime, pPrimeSize, pDomain->prime, primeSizeBytes));
        CHECK_ERROR(CC_CommonConvertLswMswWordsToMsbLsbBytes(pPrime, primeSizeBytes, pDomain->prime, primeSizeBytes));
        *pPrimeSize = primeSizeBytes;
    }
    /* Order Q */
    if((pOrder != NULL) && (pOrderSize != NULL)) {
        CHECK_ERROR(FfcCheckPtrAndSize(pOrder, pOrderSize, pDomain->order, orderSizeBytes));
        CHECK_ERROR(CC_CommonConvertLswMswWordsToMsbLsbBytes(pOrder, orderSizeBytes, pDomain->order, orderSizeBytes));
        *pOrderSize = orderSizeBytes;
    }
    /* Generator G */
    if((pGenerator != NULL) && (pGeneratorSize != NULL)) {
        CHECK_ERROR(FfcCheckPtrAndSize(pGenerator, pGeneratorSize, pDomain->genG, primeSizeBytes));
        CHECK_ERROR(CC_CommonConvertLswMswWordsToMsbLsbBytes(pGenerator, primeSizeBytes, pDomain->genG, primeSizeBytes));
        *pGeneratorSize = primeSizeBytes;
    }
    /* Seed */
    if((pSeed != NULL) && (pSeedSize != NULL)) {
        CHECK_ERROR(FfcCheckPtrAndSize(pSeed, pSeedSize, pDomain->seed, pDomain->seedSizeBytes));
        CHECK_AND_SET_ERROR(CC_PalMemCopy(pSeed, pDomain->seed, pDomain->seedSizeBytes),
                       CC_FFC_DOMAIN_INVALID_OPTIONAL_PARAM_ERROR);
        *pSeedSize = pDomain->seedSizeBytes;
    }

    /* Output enumerator and number parameters */
    if(pFfcParamSetId != NULL) {
        *pFfcParamSetId = pDomain->ffcParamSetId;
    }
    if(pFfcHashMode != NULL) {
        *pFfcHashMode =  pDomain->ffcHashMode;
    }
    if(pGenCounter != NULL) {
        *pGenCounter = pDomain->genCounter;
    }
    if(pIndexOfGenerator != NULL) {
        *pIndexOfGenerator = pDomain->indexOfGenerator;
    }

End:
    /* Note: because the Domain is not secret,
     * we don't zeroing buffers in case of in/out errors
     */
    return err;
}



/*******************************************************************************************/
/*!
@brief The function creates a new FFC subgroup Generator for existed FFC Domain.
<ol><li> Assumed, that FFC domain is properly generated or imported previously and meets
to the FIPS 184-4, sec. A.1.1.2 standard. </li>
<li> The function checks input/output pointers and buffers sizes and creates new Generator
according to sec. A.2.3. and sets it into Domain structure. </li></ol>

@return CC_OK on success.
@return A non-zero value on failure as defined cc_dh_error.h, cc_rnd_error.h.
*/
CEXPORT_C CCError_t CC_FfcCreateNewGenerator(
                CCFfcDomain_t *pDomain,        /*!< [in/out] pointer to  FFC Domain structure. */
                CCRndContext_t *pRndContext,   /*!< [in] random generation function context. */
                uint8_t index,                 /*!< [in] index allowing to generate some FFC generators with
                                                    the same FFC parameters prime P and Order Q, existed in the domain. */
                CCFfcDomainTmpBuff_t *pTmpBuff /*!< [in] pointer to FFC Domain temp buffer structure. */
)
{
    /* The return error identifier */
    CCError_t err = CC_OK;

    CHECK_AND_SET_ERROR((pDomain == NULL), CC_FFC_DOMAIN_INVALID_RND_CTX_PTR_ERROR);
    CHECK_AND_SET_ERROR((pDomain->validTag != CC_FFC_DOMAIN_VALIDATION_TAG),
                         CC_FFC_DOMAIN_VALIDATION_TAG_ERROR);

    return FfcFips186v4CreateGenerator(pDomain, pRndContext, index, pTmpBuff);

End:
    return err;

}


