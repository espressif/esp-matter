/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_CC_API

#include "cc_bitops.h"
#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_common.h"
#include "mbedtls_cc_srp.h"
#include "srp_driver.h"
#include "srp.h"
#include "mbedtls_cc_srp_error.h"


/************************ private Functions **********************/
bool isSrpVerValid(mbedtls_srp_version_t    srpVer)
{
    if ((srpVer != CC_SRP_VER_3) &&
        (srpVer != CC_SRP_VER_6) &&
        (srpVer != CC_SRP_VER_6A) &&
        (srpVer != CC_SRP_VER_HK)) {
        return false;
    }
    return true;
}

bool isSrpModulusSizeValid(size_t modSizeInBits)
{
    if ((modSizeInBits != CC_SRP_MODULUS_SIZE_1024_BITS) &&
        (modSizeInBits != CC_SRP_MODULUS_SIZE_1536_BITS) &&
        (modSizeInBits != CC_SRP_MODULUS_SIZE_2048_BITS) &&
        (modSizeInBits != CC_SRP_MODULUS_SIZE_3072_BITS)) {
        return false;
    }
    return true;
}

uint32_t getHashDigestSize(CCHashOperationMode_t  hashMode)
{
    switch(hashMode) {
    case CC_HASH_SHA1_mode:
        return CC_HASH_SHA1_DIGEST_SIZE_IN_BYTES;
    case CC_HASH_SHA224_mode:
        return CC_HASH_SHA224_DIGEST_SIZE_IN_BYTES;
    case CC_HASH_SHA256_mode:
        return CC_HASH_SHA256_DIGEST_SIZE_IN_BYTES;
    case CC_HASH_SHA384_mode:
        return CC_HASH_SHA384_DIGEST_SIZE_IN_BYTES;
    case CC_HASH_SHA512_mode:
        return CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES;
    case CC_HASH_MD5_mode:
        return CC_HASH_MD5_DIGEST_SIZE_IN_BYTES;
    default:
        return 0;
    }
}

/************************ SRP common Functions **********************/
CIMPORT_C CCError_t  mbedtls_srp_init(
                mbedtls_srp_entity_t    srpType,
                mbedtls_srp_version_t   srpVer,
                mbedtls_srp_modulus srpModulus,
                uint8_t         srpGen,
                size_t          modSizeInBits,
                CCHashOperationMode_t   hashMode,
                uint8_t         *pUserName,
                size_t                  userNameSize,
                uint8_t         *pPwd,
                size_t                  pwdSize,
                CCRndContext_t  *pRndCtx,
                mbedtls_srp_context *pCtx)
{
    CCError_t   rc = 0;
    uint32_t    hashDigestSize = 0;

    // Verify inputs
    hashDigestSize = getHashDigestSize(hashMode);
    if (((srpType != CC_SRP_HOST) && (srpType != CC_SRP_USER)) ||
        (isSrpVerValid(srpVer) == false) ||
        (srpModulus == NULL) ||
        (isSrpModulusSizeValid(modSizeInBits) == false) ||
        (pUserName == NULL) || (userNameSize == 0) ||
        ((srpType == CC_SRP_USER) && ((pPwd == NULL) || (pwdSize == 0))) ||
        (pRndCtx == NULL) ||
        (pCtx == NULL)) {
        return CC_SRP_PARAM_INVALID_ERROR;
    }

    // clear SRP ctx
    CC_PalMemSetZero(pCtx, sizeof(*pCtx));

    // Keep inputs in context
    pCtx->srpType = srpType;
    pCtx->srpVer = srpVer;
    pCtx->groupParam.gen = srpGen;
    pCtx->groupParam.modSizeInBits = modSizeInBits;
    CC_PalMemCopy(pCtx->groupParam.modulus, srpModulus, CALC_FULL_BYTES(modSizeInBits));
    pCtx->hashMode = hashMode;
    pCtx->hashDigestSize = hashDigestSize;
    pCtx->pRndCtx = pRndCtx;

    //calculate multiplier k and keep it in SRP context
    rc = SRP_InitAndkMultiplierCalc(pCtx);
    if (rc != 0) {
                rc = CC_SRP_INTERNAL_ERROR;
                goto end_with_error;
    }

    //calculate user credential Digest, if exists, and keep it in SRP context
    if ((pPwd != NULL) && (pwdSize != 0)) {
        rc = SRP_UserCredDigCalc(pUserName, userNameSize,
                pPwd, pwdSize,
                pCtx);
        if (rc != 0) {
            goto end_with_error;
        }
    }

    // start calculating the proof with existing values
    rc = SRP_UserNameDigCalc(pUserName,
            userNameSize,
            pCtx);
    if (rc != 0) {
        goto end_with_error;
    }

    return CC_OK;

end_with_error:
    CC_PalMemSetZero(pCtx, sizeof(*pCtx));
    return rc;

}


CIMPORT_C CCError_t  mbedtls_srp_pwd_ver_create(
                size_t                  saltSize,
                uint8_t         *pSalt,
                mbedtls_srp_modulus         pwdVerifier,
                mbedtls_srp_context *pCtx)
{
    CCError_t   rc = 0;
    mbedtls_srp_digest  xBuff;
    CCRndContext_t *pRndCtx;

    // verify input
    if ((saltSize < CC_SRP_MIN_SALT_SIZE) || (saltSize > CC_SRP_MAX_SALT_SIZE) ||
        (pSalt == NULL) ||
        (pwdVerifier == NULL) ||
        (pCtx == NULL) || (pCtx->pRndCtx->rndGenerateVectFunc == NULL)) {
        return CC_SRP_PARAM_INVALID_ERROR;
    }
    pRndCtx = pCtx->pRndCtx;

    // generate random pSalt
    rc = pRndCtx->rndGenerateVectFunc(pRndCtx->rndState, (unsigned char *)pSalt, saltSize);
    if (rc != CC_OK) {
        goto end_with_error;
    }
    // calc x = SHA(pSalt | pCtx->credDigest)
    rc = SRP_xBuffCalc(pSalt,
        saltSize,
        xBuff,
        pCtx);
    if (rc != 0) {
        goto end_with_error;
    }

    // Use PKA to calculate v=g^x%N
    rc = SrpPwdVerifierCalc(xBuff, pwdVerifier, pCtx);
    if (rc != 0) {
                rc = CC_SRP_INTERNAL_ERROR;
                goto end_with_error;
    }
    return CC_OK;

end_with_error:
    CC_PalMemSetZero(pSalt, saltSize);
    CC_PalMemSetZero(pwdVerifier, sizeof(mbedtls_srp_modulus));
    CC_PalMemSetZero(pCtx, sizeof(mbedtls_srp_context));
    return rc;
}


CIMPORT_C CCError_t  mbedtls_srp_clear(
        mbedtls_srp_context *pCtx)
{
    if (pCtx == NULL) {
        return CC_SRP_PARAM_INVALID_ERROR;
    }
    CC_PalMemSetZero(pCtx, sizeof(*pCtx));
    return CC_OK;
}


/************************ SRP Host Functions **********************/
CIMPORT_C CCError_t  mbedtls_srp_host_pub_key_create(
                size_t                  ephemPrivSize,
                mbedtls_srp_modulus         pwdVerifier,
                mbedtls_srp_modulus         hostPubKeyB,
                mbedtls_srp_context *pCtx)
{
    CCError_t   rc = 0;
    CCRndContext_t *pRndCtx;

    // verify input
    if ((ephemPrivSize < CC_SRP_PRIV_NUM_MIN_SIZE) ||
        (ephemPrivSize > CC_SRP_PRIV_NUM_MAX_SIZE) ||
        (ephemPrivSize % CC_32BIT_WORD_SIZE) ||
        (pwdVerifier == NULL) ||
        (hostPubKeyB == NULL) ||
        (pCtx == NULL) || (pCtx->pRndCtx->rndGenerateVectFunc == NULL)) {
        return CC_SRP_PARAM_INVALID_ERROR;
    }
    pRndCtx = pCtx->pRndCtx;
    // generate ephemeral private key b
    rc = pRndCtx->rndGenerateVectFunc(pRndCtx->rndState, (unsigned char *)pCtx->ephemPriv, ephemPrivSize);
    if (rc != CC_OK) {
        goto end_with_error;
    }
    pCtx->ephemPrivSize = ephemPrivSize;

    // Use PKA to calculate B=(k*v+g^b)%N
    rc = SrpHostPublicKeyCalc(pwdVerifier, hostPubKeyB, pCtx);
    if (rc != 0) {
                rc = CC_SRP_INTERNAL_ERROR;
                goto end_with_error;
    }
    return CC_OK;

end_with_error:
    CC_PalMemSetZero(hostPubKeyB, sizeof(mbedtls_srp_modulus));
    CC_PalMemSetZero(pCtx, sizeof(mbedtls_srp_context));
    return rc;
}


CIMPORT_C CCError_t  mbedtls_srp_host_proof_verify_and_calc(
                size_t                  saltSize,
                uint8_t         *pSalt,
                mbedtls_srp_modulus         pwdVerifier,
                mbedtls_srp_modulus         userPubKeyA,
                mbedtls_srp_modulus         hostPubKeyB,
                mbedtls_srp_digest  userProof,
                mbedtls_srp_digest  hostProof,
                mbedtls_srp_sessionKey  sessionKey,
                mbedtls_srp_context *pCtx)
{
    CCError_t   rc = 0;
    mbedtls_srp_digest  digBuff; // used for uScramble and expected user proof
    mbedtls_srp_modulus         sharedSecret;

    // Verify input
    if ((saltSize < CC_SRP_MIN_SALT_SIZE) || (saltSize > CC_SRP_MAX_SALT_SIZE) ||
        (pSalt == NULL) ||
        (pwdVerifier == NULL) ||
        (userPubKeyA == NULL) ||
        (hostPubKeyB == NULL) ||
        (userProof == NULL) ||
        (hostProof == NULL) ||
        (sessionKey == NULL) ||
        (pCtx == NULL) ) {
        return CC_SRP_PARAM_INVALID_ERROR;
    }

    // Generate u
    rc = SRP_uScrambleCalc(userPubKeyA,
            hostPubKeyB,
            digBuff,
            pCtx);
    if (rc != 0) {
        goto end_with_error;
    }

    // Use PKA to calculate shared secret S=(((A*v^u)^b)%N
    rc = SrpHostSharedSecretCalc(userPubKeyA,
                pwdVerifier,
                digBuff,
                sharedSecret,
                pCtx);
    if (rc != 0) {
                rc = CC_SRP_INTERNAL_ERROR;
                goto end_with_error;
    }

    // Generate session key K
    rc = SRP_SessionKeyCalc((uint8_t *)sharedSecret,
                CALC_FULL_BYTES(pCtx->groupParam.modSizeInBits),
                sessionKey, pCtx);
    if (rc != 0) {
        goto end_with_error;
    }

    CC_PalMemSetZero(digBuff, sizeof(digBuff));
    // generate the expected user proof
    rc = SRP_UserProofCalc2(pSalt, saltSize,
        userPubKeyA, hostPubKeyB,
        sessionKey,
        digBuff,
        pCtx);
    if (rc != 0) {
        goto end_with_error;
    }
    rc = SRP_SecureMemCmp(digBuff, userProof, pCtx->hashDigestSize);
    if (rc != 0) {
        rc = CC_SRP_RESULT_ERROR;
        goto end_with_error;
    }

    // generate the host proof
    rc = SRP_HostProofCalc(userPubKeyA,
        userProof,
        sessionKey,
        hostProof,
        pCtx);
    if (rc != 0) {
        goto end_with_error;
    }
    return CC_OK;

end_with_error:
    CC_PalMemSetZero(hostProof, sizeof(mbedtls_srp_digest));
    CC_PalMemSetZero(sessionKey, sizeof(mbedtls_srp_sessionKey));
    CC_PalMemSetZero(pCtx, sizeof(mbedtls_srp_context));
    return rc;
}


/************************ SRP User Functions **********************/
CIMPORT_C CCError_t  mbedtls_srp_user_pub_key_create(
                size_t                  ephemPrivSize,
                mbedtls_srp_modulus         userPubKeyA,
                mbedtls_srp_context *pCtx)
{
    CCError_t   rc = 0;
    CCRndContext_t *pRndCtx;

    // verify input
    if ((ephemPrivSize < CC_SRP_PRIV_NUM_MIN_SIZE) ||
        (ephemPrivSize > CC_SRP_PRIV_NUM_MAX_SIZE) ||
        (ephemPrivSize % CC_32BIT_WORD_SIZE) ||
        (userPubKeyA == NULL) ||
        (pCtx == NULL) || (pCtx->pRndCtx->rndGenerateVectFunc == NULL)) {
        return CC_SRP_PARAM_INVALID_ERROR;
    }
    pRndCtx = pCtx->pRndCtx;
    // generate ephemeral private key a
    rc = pRndCtx->rndGenerateVectFunc(pRndCtx->rndState, (unsigned char *)pCtx->ephemPriv, ephemPrivSize);
    if (rc != CC_OK) {
        goto end_with_error;
    }
    pCtx->ephemPrivSize = ephemPrivSize;

    // Use PKA to calculate A=(g^a)%N
    rc = SrpUserPublicKeyCalc(userPubKeyA, pCtx);
    if (rc != 0) {
                rc = CC_SRP_INTERNAL_ERROR;
                goto end_with_error;
    }
    return CC_OK;

end_with_error:
    CC_PalMemSetZero(userPubKeyA, sizeof(mbedtls_srp_modulus));
    CC_PalMemSetZero(pCtx, sizeof(mbedtls_srp_context));
    return rc;
}


CIMPORT_C CCError_t  mbedtls_srp_user_proof_calc(
                size_t                  saltSize,
                uint8_t         *pSalt,
                mbedtls_srp_modulus userPubKeyA,
                mbedtls_srp_modulus hostPubKeyB,
                mbedtls_srp_digest  userProof,
                mbedtls_srp_sessionKey  sessionKey,
                mbedtls_srp_context *pCtx)
{
    CCError_t   rc = 0;
    mbedtls_srp_digest  uScramble;
    mbedtls_srp_modulus         sharedSecret;
    mbedtls_srp_digest      xBuff;

    // Verify input
    if ((saltSize < CC_SRP_MIN_SALT_SIZE) || (saltSize > CC_SRP_MAX_SALT_SIZE) ||
        (pSalt == NULL) ||
        (userPubKeyA == NULL) ||
        (hostPubKeyB == NULL) ||
        (userProof == NULL) ||
        (sessionKey == NULL) ||
        (pCtx == NULL) ) {
        return CC_SRP_PARAM_INVALID_ERROR;
    }

    // Generate u
    rc = SRP_uScrambleCalc(userPubKeyA,
            hostPubKeyB,
            uScramble,
            pCtx);
    if (rc != 0) {
        goto end_with_error;
    }

    // calc x = SHA(pSalt | pCtx->credDigest)
    rc = SRP_xBuffCalc(pSalt, saltSize, xBuff, pCtx);
    if (rc != 0) {
        goto end_with_error;
    }

    // Use PKA to calculate S=((B-k*g^x)^(a+u*x))%N
    rc = SrpUserSharedSecretCalc(hostPubKeyB,
                xBuff,
                uScramble,
                sharedSecret,
                pCtx);
    if (rc != 0) {
                rc = CC_SRP_INTERNAL_ERROR;
                goto end_with_error;
    }

    // generate shared secret K
    rc = SRP_SessionKeyCalc((uint8_t *)sharedSecret,
                CALC_FULL_BYTES(pCtx->groupParam.modSizeInBits),
                sessionKey,
                pCtx);
    if (rc != 0) {
        goto end_with_error;
    }

    // generate the expected user proof
    rc = SRP_UserProofCalc2(pSalt, saltSize,
        userPubKeyA, hostPubKeyB,
        sessionKey,
        userProof,
        pCtx);
    if (rc != 0) {
        goto end_with_error;
    }
    return CC_OK;

end_with_error:
    CC_PalMemSetZero(userProof, sizeof(mbedtls_srp_digest));
    CC_PalMemSetZero(sessionKey, sizeof(mbedtls_srp_sessionKey));
    CC_PalMemSetZero(pCtx, sizeof(mbedtls_srp_context));
    return rc;
}


CIMPORT_C CCError_t  mbedtls_srp_user_proof_verify(
                mbedtls_srp_sessionKey  sessionKey,
                mbedtls_srp_modulus userPubKeyA,
                mbedtls_srp_digest  userProof,
                mbedtls_srp_digest  hostProof,
                mbedtls_srp_context *pCtx)
{
    CCError_t   rc = 0;
    mbedtls_srp_digest  expHostProof = {0};

    // verify input
    if ((sessionKey == NULL) ||
        (userPubKeyA == NULL) ||
        (userProof == NULL) ||
        (hostProof == NULL) ||
        (pCtx == NULL)) {
        return CC_SRP_PARAM_INVALID_ERROR;
    }

    // generate the expected proof
    rc = SRP_HostProofCalc(userPubKeyA,
        userProof,
        sessionKey,
        expHostProof,
        pCtx);
    if (rc != 0) {
        goto end_with_error;
    }

    // compare the expected proof to the actual one
    rc = SRP_SecureMemCmp(expHostProof, hostProof, pCtx->hashDigestSize);
    if (rc != 0) {
        rc = CC_SRP_RESULT_ERROR;
        goto end_with_error;
    }
    return CC_OK;

end_with_error:
    CC_PalMemSetZero(pCtx, sizeof(mbedtls_srp_context));
    return rc;

}

