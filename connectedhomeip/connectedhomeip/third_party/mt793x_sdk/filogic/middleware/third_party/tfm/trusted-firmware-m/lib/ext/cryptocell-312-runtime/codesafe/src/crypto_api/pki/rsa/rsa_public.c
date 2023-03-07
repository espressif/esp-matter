/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef CC_IOT
    #if defined(MBEDTLS_CONFIG_FILE)
    #include MBEDTLS_CONFIG_FILE
    #endif
#endif

#if !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C))


/************* Include Files ****************/

#include "cc_rsa_error.h"
#include "rsa_public.h"
#include "cc_rsa_types.h"
#include "pka.h"
#include "pki.h"
#include "cc_pal_mutex.h"
#include "pka_error.h"

extern const int8_t regTemps[PKA_MAX_COUNT_OF_PHYS_MEM_REGS];

/***********    RsaInitPubKeyDb   function      **********************/
/**
 * @brief Initializes the low level key database public structure.
 *        On the HW platform the Barrett tag is initialized
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
CCError_t RsaInitPubKeyDb(CCRsaPubKey_t *pPubKey)  /*!< [in]  Public key structure. */
{
    CCError_t error = CC_OK;

    if (pPubKey == NULL) {
        return CC_RSA_INVALID_PUB_KEY_STRUCT_POINTER_ERROR;
    }

    /* calculate Barrett tag NP by initialization PKA for modular operations.
       Default settings: N=PKA_REG_N, NP=PKA_REG_NP, T0=30, T1=31.
       Our settings for temps: rT0=2, rT1=3, rT2=4 */
    error = PkiCalcNp( ((RsaPubKeyDb_t*)(pPubKey->ccRSAIntBuff))->NP, /*out*/
                pPubKey->n,          /*in*/
                pPubKey->nSizeInBits);

    return error;


}

/***********     RsaExecPubKeyExp  function      **********************/
/**
 * @brief Executes the RSA primitive public key exponent :
 *
 *    pPubData->DataOut =  pPubData->DataIn ** pPubKey->e  mod  pPubKey->n,
 *    where: ** - exponent symbol.
 *
 *    Note: PKA registers used: r0-r4,   r30,r31, size of registers - Nsize.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */

CCError_t RsaExecPubKeyExp( CCRsaPubKey_t     *pPubKey,     /*!< [in]  Public key structure. */
                       CCRsaPrimeData_t *pPubData )   /*!< [in]  Containing input data and output buffer. */
{
    CCError_t error = CC_OK;
    uint32_t  nSizeInWords, eSizeInWords;
    uint32_t  pkaReqRegs = 7;

    uint8_t rT2 = regTemps[2];
    uint8_t rT3 = regTemps[3];
    uint8_t rT4 = regTemps[4];

    /* modulus size in bytes */
    nSizeInWords = CALC_FULL_32BIT_WORDS(pPubKey->nSizeInBits);
    eSizeInWords = CALC_FULL_32BIT_WORDS(pPubKey->eSizeInBits);
    if (nSizeInWords > CALC_FULL_32BIT_WORDS(CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS)) {
        return CC_RSA_INVALID_MODULUS_SIZE;
    }

    error = PkaInitAndMutexLock(pPubKey->nSizeInBits, &pkaReqRegs);
    if (error != CC_OK) {
        return error;
    }

    /* copy modulus N into r0 register */
    PkaCopyDataIntoPkaReg(PKA_REG_N/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/,  pPubKey->n/*srcPtr*/,
                   nSizeInWords);

    /* copy the NP into r1 register NP */
    PkaCopyDataIntoPkaReg(PKA_REG_NP/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/, ((RsaPubKeyDb_t*)(pPubKey->ccRSAIntBuff))->NP/*srcPtr*/,
                   CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS);

    /* copy input data into PKI register: DataIn=>r2 */
    PkaCopyDataIntoPkaReg( rT2/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/,
                pPubData->DataIn, nSizeInWords);

    /* copy exponent data PKI register: e=>r3 */
    PkaCopyDataIntoPkaReg(rT3/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/,
                   pPubKey->e, eSizeInWords);

    /* .. calculate the exponent Res = OpA**OpB mod N; */
    PKA_MOD_EXP(LEN_ID_N_BITS/*LenID*/, rT4/*Res*/, rT2/*OpA*/, rT3/*OpB*/);

    /* copy result into output: r4 =>DataOut */
    PkaCopyDataFromPkaReg(pPubData->DataOut, nSizeInWords, rT4/*srcReg*/);

    PkaFinishAndMutexUnlock(pkaReqRegs);


    return error;
}

#endif /* !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C)) */
