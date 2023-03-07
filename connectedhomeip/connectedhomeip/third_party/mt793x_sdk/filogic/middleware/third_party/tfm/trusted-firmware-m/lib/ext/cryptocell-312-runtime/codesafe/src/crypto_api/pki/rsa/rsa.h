/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef LLF_RSA_H
#define LLF_RSA_H


#include "cc_rsa_types.h"
#include "cc_rnd_common.h"
#ifdef FIPS_CERTIFICATION
#include "fips_tests_rsa_def.h"
#include "cc_common.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/************************ Defines ******************************/

#define PKA_MAX_RSA_KEY_GENERATION_SIZE_BITS   CC_RSA_MAX_KEY_GENERATION_SIZE_BITS
/* max allowed size of pprimes P, Q in RSA KG */
#define PKA_RSA_KG_MAX_PQ_SIZE_BITS  (PKA_MAX_RSA_KEY_GENERATION_SIZE_BITS/2)
/* max. total count of avaliable PKA registers in RSA KG */
#define PKA_RSA_KG_MAX_COUNT_REGS  \
        CC_MIN(PKA_MAX_COUNT_OF_PHYS_MEM_REGS, \
                (8*CC_SRAM_PKA_SIZE_IN_BYTES) / (PKA_RSA_KG_MAX_PQ_SIZE_BITS + CC_PKA_WORD_SIZE_IN_BITS))
/* max. count of avaliable registers in RSA KG without auxiliary regs. 30,31  */
#define PKA_RSA_KG_MAX_REG_ID (PKA_RSA_KG_MAX_COUNT_REGS - 2)

/* define size of auxiliary prime numbers for RSA  *
*  (see FIPS 186-4 C3 tab.C3)                      */
#define PKA_RSA_KEY_1024_AUX_PRIME_SIZE_BITS   104   /* for P,Q size 1024 bit aux.size > 100 bits */
#define PKA_RSA_KEY_2048_AUX_PRIME_SIZE_BITS   144   /* for P,Q size 2048 bit aux.size > 140 bits */
#define PKA_RSA_KEY_3072_AUX_PRIME_SIZE_BITS   176   /* for P,Q size 3072 bit aux.size > 170 bits */

#define PKA_RSA_AUX_PRIME_BUFF_SIZE_IN_32BIT_WORDS 8 /* max size of temp buffer for auxiliary prime */

/* define count of Miller-Rabin tests for P,Q and  auxiliary prime numbers *
*  for RSA key generation (see FIPS 186-4 C3 tab.C3)                       */
#define PKA_RSA_KEY_1024_AUX_PRIME_RM_TST_COUNT 38
#define PKA_RSA_KEY_1024_PQ_PRIME_RM_TST_COUNT   7
#define PKA_RSA_KEY_2048_AUX_PRIME_RM_TST_COUNT 32
#define PKA_RSA_KEY_2048_PQ_PRIME_RM_TST_COUNT   4
#define PKA_RSA_KEY_3072_AUX_PRIME_RM_TST_COUNT 27
#define PKA_RSA_KEY_3072_PQ_PRIME_RM_TST_COUNT   3

#define CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_WORDS        CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS / CC_BITS_IN_32BIT_WORD

#define LLF_PKI_PKA_DEBUG 1

#ifdef FIPS_CERTIFICATION
rsaKgInternalDataStruct_t rsaKgOutParams;
#endif


/*  RSA   key generation parameters structure */
typedef struct
{
    uint32_t auxPrimesSizeInBits;
    uint32_t pqPrimesMilRabTestsCount;
    uint32_t auxPrimesMilRabTestsCount;
}RsaKgParams_t;

/*  RSA   Barrett tags for modular operations with N, P, Q */
typedef struct
{
    uint32_t tmpBuf[3*CC_PKA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    uint32_t barN[CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS]; /*!< Barrett tag for modulus N */
    uint32_t barP[CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS]; /*!< Barrett tag for modulus P */
    uint32_t barQ[CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS]; /*!< Barrett tag for modulus Q */
}RsaKgIntBuff_t;

CCError_t RsaGenPandQ(CCRndContext_t *rndContext_ptr,
                 size_t          KeySize,
                 uint32_t        eSizeInBits,
                 uint32_t       *pPubExp,
                 CCRsaKgData_t  *KeyGenData_ptr);

CCError_t RsaPrimeTestCall(CCRndContext_t *rndContext_ptr,
                 uint32_t *P_ptr,
                 int32_t   sizeWords,
                 int32_t   rabinTestsCount,
                 int8_t   *isPrime_ptr,
                 uint32_t *TempBuff_ptr,
                 CCRsaDhPrimeTestMode_t primeTestMode);

CCError_t RsaCalculateNandD(CCRsaPubKey_t    *pCcPubKey, /*!< [in] pointer to the public key structure */
                CCRsaPrivKey_t   *pCcPrivKey, /*!< [in] pointer to the private key structure */
                CCRsaKgData_t    *KeyGenData_ptr, /*!< [in] pointer to a structure required for the KeyGen operation, holding P and Q */
                            uint32_t          primeSizeInBits); /*!< [in] Size of the prime factors in bits. */


CCError_t RsaCalculateCrtParams(uint32_t *pPubExp,      /*!< [in]  Pointer to the public exponent. */
                                         uint32_t eSizeInBits,  /*!< [in]  Public exponent size in bits. */
                                         uint32_t nSizeInBits,  /*!< [in]  Size of the key modulus in bits. */
                                         uint32_t *pPrimeP,     /*!< [out]  First factor pointer - p. */
                                         uint32_t *pPrimeQ,     /*!< [out]  Second factor pointer - Q. */
                                         uint32_t *pPrivExp1dp, /*!< [out]  Private exponent for first factor - dP. */
                                         uint32_t *pPrivExp2dq, /*!< [out]  Private exponent for second factor - dQ. */
                                         uint32_t *pQInv);


//#if (defined RSA_KG_FIND_BAD_RND || defined RSA_KG_NO_RND) && defined DEBUG
extern uint8_t   RSA_KG_debugPvect[CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES/2];
extern uint8_t   RSA_KG_debugQvect[CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES/2];
extern uint8_t PQindex;
extern uint8_t  rBuff1[PKA_RSA_AUX_PRIME_BUFF_SIZE_IN_32BIT_WORDS * sizeof(uint32_t) ];
extern uint8_t  rBuff2[PKA_RSA_AUX_PRIME_BUFF_SIZE_IN_32BIT_WORDS * sizeof(uint32_t) ];
extern uint8_t  rBuff3[PKA_RSA_AUX_PRIME_BUFF_SIZE_IN_32BIT_WORDS * sizeof(uint32_t) ];
extern uint8_t  rBuff4[PKA_RSA_AUX_PRIME_BUFF_SIZE_IN_32BIT_WORDS * sizeof(uint32_t) ];
/* temp buffers for output results of generation P1,P2 for P and Q  */
extern uint32_t P1pR[PKA_RSA_AUX_PRIME_BUFF_SIZE_IN_32BIT_WORDS];
extern uint32_t P2pR[PKA_RSA_AUX_PRIME_BUFF_SIZE_IN_32BIT_WORDS];
extern uint32_t P1qR[PKA_RSA_AUX_PRIME_BUFF_SIZE_IN_32BIT_WORDS];
extern uint32_t P2qR[PKA_RSA_AUX_PRIME_BUFF_SIZE_IN_32BIT_WORDS];
/* final values of P1,P2 for P and Q */
extern uint32_t P1pPr[PKA_RSA_AUX_PRIME_BUFF_SIZE_IN_32BIT_WORDS];
extern uint32_t P2pPr[PKA_RSA_AUX_PRIME_BUFF_SIZE_IN_32BIT_WORDS];
extern uint32_t P1qPr[PKA_RSA_AUX_PRIME_BUFF_SIZE_IN_32BIT_WORDS];
extern uint32_t P2qPr[PKA_RSA_AUX_PRIME_BUFF_SIZE_IN_32BIT_WORDS];
extern uint32_t *P1R_ptr;
extern uint32_t *P2R_ptr;
extern uint32_t *P1Pr_ptr;
extern uint32_t *P2Pr_ptr;

/* temp buffers and pointer for output the P,Q  after generation */
extern uint32_t rBuffP[CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_WORDS / 2];
extern uint32_t rBuffQ[CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_WORDS / 2];
extern uint32_t  *PQ_ptr;
//#endif


#ifdef __cplusplus
}
#endif

#endif
