/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_hash_defs.h"
#include "cc_rnd_common.h"
#include "cc_ec_edw_api.h"
#include "mbedtls_cc_ec_mont_edw_error.h"

#include "ec_edw.h"
#include "ec_edw_local.h"

/******************************************************************************/
/**  Ed25519 Sign/Verify two types APIs: 1. For signature concatenated with   *
 *    message; 2. For detached signature and message with separate in/out      */
/******************************************************************************/
/******************************************************************************/
/**
 * The function creates EC Edwards signature on the message.
 *
 *    Note: Used detached form of signature, separated from the message.
 *          Implemented algorithm of Bernstein D. etc. sign ed25519.
 *
 *  @return CC_OK on success,
 *  @return A non-zero value on failure as defined mbedtls_cc_ec_mont_edw_error.h.
 */
CEXPORT_C CCError_t CC_EcEdwSign (
        uint8_t       *pSign,                /*!< [out] Pointer to the detached signature. */
        size_t        *pSignSize,            /*!< [in/out] Pointer to the total size of the signature ;
                                                            In  - the buffer size, which (must be at least 2*EC order size);
                                                            Out - the actual size of output data. */
        const uint8_t *pMsg,                 /*!< [in] Pointer to the message. */
        size_t         msgSize,              /*!< [in] Message size in bytes: must be less, than
                                                            (CC_HASH_UPDATE_DATA_MAX_SIZE_IN_BYTES - 2*(EC_EDW modulus size)). */
        const uint8_t *pSignSecrKey,         /*!< [in] Pointer to the signer secret key (seed || pulKey) */
        size_t         secrKeySize,          /*!< [in] Size of signer secret key in bytes: (must be 2*EC order size). */
        CCEcEdwTempBuff_t *pTempBuff)  /*!< [in] pointer to the temp buffer. */
{
    CCError_t err = CC_OK;
    uint32_t ecEdwKeySizeBytes = CC_32BIT_WORD_SIZE*CC_EC_EDW_MOD_SIZE_IN_32BIT_WORDS;
    uint32_t ecEdwSignSizeBytes = 2*ecEdwKeySizeBytes;
    /* the pointer to EC domain */
    const CCEcEdwDomain_t *pEcDomain;

    /* check input pointers */
    if (pSign == NULL || pSignSize == NULL ||
            ((pMsg  == NULL)^(msgSize == 0)) ||
            pSignSecrKey == NULL ||
            pTempBuff == NULL) {
        return CC_EC_EDW_INVALID_INPUT_POINTER_ERROR;
    }

    /* max size of message according to HASH update requirements */
    if(msgSize >= CC_HASH_UPDATE_DATA_MAX_SIZE_IN_BYTES) {
        return  CC_EC_EDW_INVALID_INPUT_SIZE_ERROR;
    }
    /* conditions for secret key size and buff. size for signature output */
    if (secrKeySize != 2*ecEdwKeySizeBytes  ||
            *pSignSize < ecEdwSignSizeBytes) {
        return CC_EC_EDW_INVALID_INPUT_SIZE_ERROR;
    }

    /* get domain */
    pEcDomain = EcEdwGetDomain25519();

    /***********************************************/
    /**      calculate signature on the massage   **/
    /***********************************************/

    err = EcEdwSign(pSign, pMsg, msgSize,
                    pSignSecrKey, pEcDomain, pTempBuff);
    if (err) {
        goto End;
    } else {
        *pSignSize = 2*ecEdwKeySizeBytes;
    }

    End:

    return err;
}

/******************************************************************************/
/**
 * The function verifies the EC Edwards ed25519 signature on the message.
 *
 *    Note: The input signature is in detached form, i.e. separated from the
 *          message.
 *
 *     Verification is performed using EC Edwards ed25519 signature algorithm.
 *
 * @return CC_OK on success,
 * @return A non-zero value on failure as defined mbedtls_cc_ec_mont_edw_error.h.
 */
CEXPORT_C CCError_t CC_EcEdwVerify(
        const uint8_t *pSign,                /*!< [in] Pointer to detached signature, i.e. the
                                                                         signature is separated from the message. */
        size_t         signSize,             /*!< [in] Size of the signature in bytes, it must be
                                                                         equal to two EC Order size in bytes. */
        const uint8_t *pSignPublKey,         /*!< [in] Pointer to signer public key. */
        size_t         publKeySize,          /*!< [in] Size of the signer public key in bytes; must be
                                                                         equal to EC modulus size. */
        uint8_t       *pMsg,                 /*!< [in] Pointer to the message. */
        size_t         msgSize,              /*!< [in] Pointer to the message size in bytes. Must be less than
                                                                         (CC_HASH_UPDATE_DATA_MAX_SIZE_IN_BYTES - 2*(EC_EDW modulus size)). */
        CCEcEdwTempBuff_t *pTempBuff)  /*!< [in] the pointer to temp buffer. */

{
    CCError_t err = CC_OK;
    uint32_t ecEdwSizeBytes = CC_32BIT_WORD_SIZE*CC_EC_EDW_MOD_SIZE_IN_32BIT_WORDS;
    uint32_t ecEdwSignSizeBytes = 2*ecEdwSizeBytes;
    const CCEcEdwDomain_t *pEcDomain;    /*the pointer to Edw. EC domain*/

    /* set pointers to temp buffers */
    if (pSign == NULL || pSignPublKey == NULL ||
            ((pMsg  == NULL)^(msgSize == 0)) || pTempBuff == NULL) {
        return  CC_EC_EDW_INVALID_INPUT_POINTER_ERROR;
    }

    if(signSize != ecEdwSignSizeBytes ) {
        return CC_EC_EDW_INVALID_INPUT_SIZE_ERROR;
    }

    if(msgSize > CC_HASH_UPDATE_DATA_MAX_SIZE_IN_BYTES) {
        return  CC_EC_EDW_INVALID_INPUT_SIZE_ERROR;
    }
    if (publKeySize != ecEdwSizeBytes) {
        return CC_EC_EDW_INVALID_INPUT_SIZE_ERROR;
    }

    /* get domain */
    pEcDomain = EcEdwGetDomain25519();

    /* perform scalar mult. and adding of two points */
    err = EcEdwSignVerify (pSign, pMsg, msgSize,
                           pSignPublKey, pEcDomain, pTempBuff);

    return err;
}


/*******************************************************************/
/**          Edwards Key Pair generation from seeds API            */
/*******************************************************************/
/**
 @brief The function randomly generates Ec ed25519 private and public keys
        using given seed.

        The generation is performed using EC Edwards ed25519 algorithm.

 @return CC_OK on success,
 @return A non-zero value on failure as defined mbedtls_cc_ec_mont_edw_error.h.
 */
CEXPORT_C CCError_t CC_EcEdwSeedKeyPair (
        const uint8_t *pSeed,                  /*!< [in] Pointer to the given seed. */
        size_t         seedSize,               /*!< [in] Size of the seed in bytes, must be equal the EC order size
                                                                             in bytes. */
        uint8_t       *pSecrKey,               /*!< [out] Pointer to the secret key, including the seed, concatenated
                                                                             with the public key. */
        size_t        *pSecrKeySize,           /*!< [in/out] Pointer to the size of the secret key buffer in bytes
                                                                             (must be at least 2*EC order size). */
        uint8_t       *pPublKey,               /*!< [out] Pointer to the public key. */
        size_t        *pPublKeySize,           /*!< [in/out] Pointer to the size of the public key in bytes.
                                                                                In  - the size of buffer must be at least EC modulus size;
                                                                                Out - the actual size. */
        CCEcEdwTempBuff_t *pTempBuff)
{
    CCError_t err = CC_OK;
    uint32_t ecEdwSizeBytes = CC_32BIT_WORD_SIZE*CC_EC_EDW_MOD_SIZE_IN_32BIT_WORDS;
    uint32_t ecEdwSecrKeySizeBytes = 2*ecEdwSizeBytes;
    const CCEcEdwDomain_t *pEcDomain;    /*the pointer to Edw. EC domain*/


    if (pSeed  == NULL || pSecrKey == NULL || pSecrKeySize == NULL ||
            pPublKey == NULL || pPublKeySize == NULL ) {
        return   CC_EC_EDW_INVALID_INPUT_POINTER_ERROR;
    }

    if(pTempBuff == NULL)
        return CC_EC_EDW_INVALID_INPUT_POINTER_ERROR;

    if(*pSecrKeySize < ecEdwSecrKeySizeBytes) {
        return CC_EC_EDW_INVALID_INPUT_SIZE_ERROR;
    }
    if (*pPublKeySize < ecEdwSizeBytes) {
        return CC_EC_EDW_INVALID_INPUT_SIZE_ERROR;
    }

    if (seedSize != ecEdwSizeBytes) {
        return CC_EC_EDW_INVALID_INPUT_SIZE_ERROR;
    }

    /* get domain */
    pEcDomain = EcEdwGetDomain25519();

    /* calculate publ/priv keys and clean temp buffer */
    err = EcEdwSeedKeyPair(pPublKey, pSecrKey,
                           pSeed, pEcDomain, (uint32_t*)pTempBuff);
    if (err)
        goto End;

    *pSecrKeySize = ecEdwSecrKeySizeBytes;
    *pPublKeySize = ecEdwSizeBytes;

    End:
    return err;
}


/*******************************************************************/
/**          Edwards Key Pair (random) generation  API             */
/*******************************************************************/
/**
 @brief The function randomly generates the EC Edwards ed25519 private and
         public keys.

       The generation performed using EC Edwards ed25519 algorithm.

        @return CC_OK on success,
        @return A non-zero value on failure as defined mbedtls_cc_ec_mont_edw_error.h.
 */
CEXPORT_C CCError_t CC_EcEdwKeyPair (
        uint8_t       *pSecrKey,               /*!< [out] Pointer to the secret key (including seed and public key). */
        size_t        *pSecrKeySize,           /*!< [in/out] Pointer to the size of the secret key in bytes,
                                                                          (must be at least 2*EC order size). */
        uint8_t       *pPublKey,               /*!< [out] Pointer to the public key. */
        size_t        *pPublKeySize,           /*!< [in/out] - Pointer to the size of the public key in bytes.
                                                                            In  - the size of buffer must be at least EC modulus size;
                                                                            Out - the actual size. */
        CCRndContext_t *pRndContext,       /*!< [in/out] Pointer to the RND context buffer. */
        CCEcEdwTempBuff_t *pTempBuff)
{
    CCError_t err = CC_OK;
    uint32_t ecEdwSizeBytes;
    uint8_t *pSeed = (uint8_t*)pTempBuff;
    const CCEcEdwDomain_t *pEcDomain;    /*the pointer to Edw. EC domain*/
    CCRndState_t   *pRndState;
    CCRndGenerateVectWorkFunc_t RndGenerateVectFunc;


    /* FUNCTION LOGIC */

    /* check parameters */
    if (pSecrKey == NULL || pSecrKeySize == NULL ||
            pPublKey == NULL || pPublKeySize == NULL ||
            pTempBuff == NULL) {
        return   CC_EC_EDW_INVALID_INPUT_POINTER_ERROR;
    }
    if (pRndContext == NULL)
        return CC_EC_EDW_RND_CONTEXT_PTR_INVALID_ERROR;

    /* get domain */
    pEcDomain = EcEdwGetDomain25519();

    ecEdwSizeBytes = CC_32BIT_WORD_SIZE * pEcDomain->ecModSizeInWords;

    if(*pSecrKeySize < 2*ecEdwSizeBytes) {
        return CC_EC_EDW_INVALID_INPUT_SIZE_ERROR;
    }
    if (*pPublKeySize < ecEdwSizeBytes) {
        return CC_EC_EDW_INVALID_INPUT_SIZE_ERROR;
    }

    pRndState = (CCRndState_t *)(pRndContext->rndState);
    RndGenerateVectFunc = pRndContext->rndGenerateVectFunc;

    if (RndGenerateVectFunc == NULL)
        return CC_EC_EDW_RND_GEN_VECTOR_FUNC_ERROR;


    /* generate random seed */
    err = RndGenerateVectFunc((void *)pRndState, (unsigned char *)pSeed, (size_t)ecEdwSizeBytes);
    if (err) {
        goto End;
    }


    /* generate key pair */
    err = EcEdwSeedKeyPair(pPublKey, pSecrKey,
                           pSeed, pEcDomain,
                           (uint32_t*)pTempBuff + CC_EC_EDW_MOD_SIZE_IN_32BIT_WORDS);
    if(err)
        goto End;

    *pSecrKeySize = 2*ecEdwSizeBytes;
    *pPublKeySize = ecEdwSizeBytes;

    End:
    CC_PalMemSetZero(pTempBuff, ecEdwSizeBytes);
    return err;
}

