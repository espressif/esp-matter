/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_CC_API

#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "mbedtls_cc_srp.h"
#include "mbedtls_cc_srp_error.h"
#include "pka.h"


extern const int8_t regTemps[PKA_MAX_COUNT_OF_PHYS_MEM_REGS];

#define SRP_NP_VALID 1

// v=g^x%N
uint32_t  SrpPwdVerifierCalc(mbedtls_srp_digest xBuff,
                             mbedtls_srp_modulus    pwdVerifier,
                             mbedtls_srp_context    *pCtx)
{
    CCError_t   rc = 0;
    uint32_t    genWord;
    uint32_t    pkaRegCount = 7; // adding 2 for HW temp regs PKA_REG_T0 & PKA_REG_T1
    /* usage of PKA registers */
    int8_t  rN = PKA_REG_N;
    int8_t  rNp = PKA_REG_NP;
    int8_t  rT2 = regTemps[2];
    int8_t  rT3 = regTemps[3];
    int8_t  rT4 = regTemps[4];

    // Verify input
    if ((pCtx == NULL) ||
            (xBuff == NULL) ||
            (pwdVerifier == NULL)) {
        return CC_SRP_PARAM_INVALID_ERROR;
    }

    genWord = pCtx->groupParam.gen;
    rc = PkaInitAndMutexLock(pCtx->groupParam.modSizeInBits, &pkaRegCount);
    if (rc != 0) {
        return rc;
    }
    // first copy N and calculate Np
    PkaCopyBeByteBuffIntoPkaReg(rN, LEN_ID_MAX_BITS, pCtx->groupParam.modulus, CALC_FULL_32BIT_WORDS(pCtx->groupParam.modSizeInBits));
    rc = PkaCalcNpIntoPkaReg(LEN_ID_N_BITS, pCtx->groupParam.modSizeInBits, rN, rNp, rT2, rT3);
    if (rc != 0) {
        goto end;
    }
    pCtx->groupParam.validNp = SRP_NP_VALID;
    // save Np into pCtx
    PkaCopyDataFromPkaReg(pCtx->groupParam.Np, CALC_32BIT_WORDS_FROM_BYTES(sizeof(pCtx->groupParam.Np)), rNp);

    // calculate v=g^x%N
    PkaCopyDataIntoPkaReg(rT2, LEN_ID_MAX_BITS, &genWord, CALC_32BIT_WORDS_FROM_BYTES(sizeof(genWord)));
    PkaCopyBeByteBuffIntoPkaReg(rT3, LEN_ID_MAX_BITS, xBuff, CALC_32BIT_WORDS_FROM_BYTES(pCtx->hashDigestSize));
    PKA_MOD_EXP(LEN_ID_N_BITS, rT4, rT2/*g*/, rT3 /*x*/);
    // copy the verifier
    PkaCopyPkaRegIntoBeByteBuff(pwdVerifier, CALC_FULL_32BIT_WORDS(pCtx->groupParam.modSizeInBits), rT4);

    end:
    PkaFinishAndMutexUnlock(pkaRegCount);
    return rc;
}

/* calculates S=(((A*v^u)^b)%N */
uint32_t   SrpHostSharedSecretCalc(mbedtls_srp_modulus   userPubKeyA,
                                   mbedtls_srp_modulus pwdVerifier,
                                   mbedtls_srp_digest uScramble,
                                   mbedtls_srp_modulus  sharedSecret,
                                   mbedtls_srp_context  *pCtx)
{
    CCError_t   rc = 0;
    uint32_t    status;
    uint32_t    pkaRegCount = 8; // adding 2 for HW temp regs PKA_REG_T0 & PKA_REG_T1
    /* usage of PKA registers */
    int8_t  rN = PKA_REG_N;
    int8_t  rNp = PKA_REG_NP;
    int8_t  rT2 = regTemps[2];
    int8_t  rT3 = regTemps[3];
    int8_t  rT4 = regTemps[4];
    int8_t  rT5 = regTemps[5];

    // Verify input
    if ((pCtx == NULL) ||
            (pwdVerifier == NULL) ||
            (userPubKeyA == NULL) ||
            (uScramble == NULL) ||
            (sharedSecret == NULL)) {
        return CC_SRP_PARAM_INVALID_ERROR;
    }

    if (pCtx->groupParam.validNp != SRP_NP_VALID) {
        return CC_SRP_STATE_UNINITIALIZED_ERROR;
    }
    rc = PkaInitAndMutexLock(pCtx->groupParam.modSizeInBits, &pkaRegCount);
    if (rc != 0) {
        return rc;
    }
    // firt copy buffers into PKA reg
    PkaCopyBeByteBuffIntoPkaReg(rN, LEN_ID_MAX_BITS, pCtx->groupParam.modulus, CALC_FULL_32BIT_WORDS(pCtx->groupParam.modSizeInBits));
    PkaCopyDataIntoPkaReg(rNp, LEN_ID_MAX_BITS, pCtx->groupParam.Np, CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS);
    PkaCopyBeByteBuffIntoPkaReg(rT2, LEN_ID_MAX_BITS, pwdVerifier, CALC_FULL_32BIT_WORDS(pCtx->groupParam.modSizeInBits));
    PkaCopyBeByteBuffIntoPkaReg(rT3, LEN_ID_MAX_BITS, uScramble, CALC_32BIT_WORDS_FROM_BYTES(pCtx->hashDigestSize));
    PkaCopyBeByteBuffIntoPkaReg(rT4, LEN_ID_MAX_BITS, userPubKeyA, CALC_FULL_32BIT_WORDS(pCtx->groupParam.modSizeInBits));
    PkaCopyBeByteBuffIntoPkaReg(rT5, LEN_ID_MAX_BITS, pCtx->ephemPriv, CALC_32BIT_WORDS_FROM_BYTES(pCtx->ephemPrivSize));

    // verify 0 < userPubKeyA < N
    /* if userPubKeyA == 0, return with error */
    PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, rT4, 0, status);
    if (status == 1) {
        rc = CC_SRP_PARAM_ERROR;
        goto end;
    }
    /* if userPubKeyA > N, return with error */
    PKA_SUB(LEN_ID_N_PKA_REG_BITS, RES_DISCARD, rT4, rN);
    PKA_GET_STATUS_CARRY(status);
    if (status == 1) {
        rc = CC_SRP_PARAM_ERROR;
        goto end;
    }

    // calculate rT2 = (v^u)%N
    PKA_MOD_EXP(LEN_ID_N_BITS, rT2, rT2/*v*/, rT3 /*u*/);
    // calculate rT4 = (A*(v^u))%N
    PKA_MOD_MUL(LEN_ID_N_BITS, rT4, rT4/*A*/, rT2 /*v^u*/);

    // verify (A*(v^u)) != 0
    PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, rT4, 0, status);
    if (status == 1) {
        rc = CC_SRP_PARAM_ERROR;
        goto end;
    }
    // verify (A*(v^u)) != 1
    PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, rT4, 1, status);
    if (status == 1) {
        rc = CC_SRP_PARAM_ERROR;
        goto end;
    }
    // verify (A*(v^u)) != -1
    PKA_ADD_IM(LEN_ID_N_BITS, rT2, rT4, 1);
    PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, rT2, 0, status);
    if (status == 1) {
        rc = CC_SRP_PARAM_ERROR;
        goto end;
    }

    // calculate rT4 = (rT4 ^ b)%N
    PKA_MOD_EXP(LEN_ID_N_BITS, rT4, rT4/*(A*(v^u))*/, rT5 /*(b)*/);
    // copy the generated shared secret
    PkaCopyPkaRegIntoBeByteBuff(sharedSecret, CALC_FULL_32BIT_WORDS(pCtx->groupParam.modSizeInBits), rT4);
    end:
    PkaFinishAndMutexUnlock(pkaRegCount);
    return rc;
}


// Use PKA to calculate S=((B-g^x)^(a+u*x))%N
uint32_t   SrpUserSharedSecretCalc(mbedtls_srp_modulus   hostPubKeyB,
                                   mbedtls_srp_digest   xBuff,
                                   mbedtls_srp_digest   uScramble,
                                   mbedtls_srp_modulus  sharedSecret,
                                   mbedtls_srp_context  *pCtx)
{
    CCError_t   rc = 0;
    uint32_t    status;
    uint32_t    genWord;
    // For 4KBytes PKA we have 10 PKA registers that supports 3072 bit modulus
    uint32_t    pkaRegCount = 10; // adding 2 for HW temp regs PKA_REG_T0 & PKA_REG_T1
    /* usage of PKA registers */
    int8_t  rN = PKA_REG_N;
    int8_t  rNp = PKA_REG_NP;
    int8_t  rT2 = regTemps[2];
    int8_t  rT3 = regTemps[3];
    int8_t  rT4 = regTemps[4];
    int8_t  rT5 = regTemps[5];
    int8_t  rT6 = regTemps[6];
    int8_t  rT7 = regTemps[7];

    // Verify input
    if ((pCtx == NULL) ||
            (hostPubKeyB == NULL) ||
            (xBuff == NULL) ||
            (uScramble == NULL) ||
            (sharedSecret == NULL)) {
        return CC_SRP_PARAM_INVALID_ERROR;
    }

    if (pCtx->groupParam.validNp != SRP_NP_VALID) {
        return CC_SRP_STATE_UNINITIALIZED_ERROR;
    }

    genWord = pCtx->groupParam.gen;
    rc = PkaInitAndMutexLock(pCtx->groupParam.modSizeInBits, &pkaRegCount);
    if (rc != 0) {
        return rc;
    }
    // firt copy buffers into PKA reg
    PkaCopyBeByteBuffIntoPkaReg(rN, LEN_ID_MAX_BITS, pCtx->groupParam.modulus, CALC_FULL_32BIT_WORDS(pCtx->groupParam.modSizeInBits));
    PkaCopyDataIntoPkaReg(rNp, LEN_ID_MAX_BITS, pCtx->groupParam.Np, CALC_32BIT_WORDS_FROM_BYTES(sizeof(pCtx->groupParam.Np)));
    PkaCopyDataIntoPkaReg(rT2, LEN_ID_MAX_BITS, &genWord, CALC_FULL_32BIT_WORDS(sizeof(genWord)));
    PkaCopyBeByteBuffIntoPkaReg(rT3, LEN_ID_MAX_BITS, xBuff, CALC_32BIT_WORDS_FROM_BYTES(pCtx->hashDigestSize));
    PkaCopyBeByteBuffIntoPkaReg(rT4, LEN_ID_MAX_BITS, hostPubKeyB, CALC_FULL_32BIT_WORDS(pCtx->groupParam.modSizeInBits));
    PkaCopyBeByteBuffIntoPkaReg(rT5, LEN_ID_MAX_BITS, uScramble, CALC_32BIT_WORDS_FROM_BYTES(pCtx->hashDigestSize));
    PkaCopyBeByteBuffIntoPkaReg(rT6, LEN_ID_MAX_BITS, pCtx->ephemPriv, CALC_32BIT_WORDS_FROM_BYTES(pCtx->ephemPrivSize));

    // verify 0 < hostPubKeyB < N
    /* if hostPubKeyB % N == 0, return with error */
    PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, rT4, 0, status);
    if (status == 1) {
        rc = CC_SRP_PARAM_ERROR;
        goto end;
    }
    /* if hostPubKeyB > N, return with error */
    PKA_SUB(LEN_ID_N_PKA_REG_BITS, RES_DISCARD, rT4, rN);
    PKA_GET_STATUS_CARRY(status);
    if (status == 1) {
        rc = CC_SRP_PARAM_ERROR;
        goto end;
    }
    PkaCopyBeByteBuffIntoPkaReg(rT7, LEN_ID_MAX_BITS, pCtx->kMult, CALC_32BIT_WORDS_FROM_BYTES(pCtx->hashDigestSize));

    // calculate rT2 = (g^x)%N
    PKA_MOD_EXP(LEN_ID_N_BITS, rT2, rT2/*g*/, rT3 /*x*/);
    // calculate rT2 = (k*rT2)%N
    PKA_MOD_MUL(LEN_ID_N_BITS, rT2, rT7/*k*/, rT2 /*g^x*/);
    // calculate rT2 = (B-rT2)%N
    PKA_MOD_SUB(LEN_ID_N_BITS, rT2, rT4/*B*/, rT2 /*g^x*/);
    // calculate rT5 = (u*x)%N
    PKA_MOD_MUL(LEN_ID_N_BITS, rT5, rT5/*u*/, rT3 /*x*/);
    // calculate rT5 = (a + rT5)%N
    PKA_MOD_ADD(LEN_ID_N_BITS, rT5, rT6/*(a)*/, rT5 /*(u*x)*/);
    // calculate rT5 = (rT2^rT5)%N
    PKA_MOD_EXP(LEN_ID_N_BITS, rT5, rT2/*(B-k*g^x)*/, rT5 /*(a+u*x)*/);
    // copy the generated session key
    PkaCopyPkaRegIntoBeByteBuff(sharedSecret, CALC_FULL_32BIT_WORDS(pCtx->groupParam.modSizeInBits), rT5);
    end:
    PkaFinishAndMutexUnlock(pkaRegCount);
    return rc;
}


/* calculates B = (k*v+ g^b)%N */
uint32_t  SrpHostPublicKeyCalc(mbedtls_srp_modulus  pwdVerifier,    // in
                               mbedtls_srp_modulus  hostPubKeyB,    // out
                               mbedtls_srp_context  *pCtx)      // in
{
    CCError_t   rc = 0;
    uint32_t    status;
    uint32_t    genWord;
    uint32_t    pkaRegCount = 8; // adding 2 for HW temp regs PKA_REG_T0 & PKA_REG_T1
    /* usage of PKA registers */
    int8_t  rN = PKA_REG_N;
    int8_t  rNp = PKA_REG_NP;
    int8_t  rT2 = regTemps[2];
    int8_t  rT3 = regTemps[3];
    int8_t  rT4 = regTemps[4];
    int8_t  rT5 = regTemps[5];

    // Verify input
    if ((pCtx == NULL) ||
            (pwdVerifier == NULL) ||
            (hostPubKeyB == NULL)) {
        return CC_SRP_PARAM_INVALID_ERROR;
    }
    genWord = pCtx->groupParam.gen;
    rc = PkaInitAndMutexLock(pCtx->groupParam.modSizeInBits, &pkaRegCount);
    if (rc != 0) {
        return rc;
    }
    // firt copy buffers into PKA reg
    PkaCopyBeByteBuffIntoPkaReg(rN, LEN_ID_MAX_BITS, pCtx->groupParam.modulus, CALC_FULL_32BIT_WORDS(pCtx->groupParam.modSizeInBits));
    if (pCtx->groupParam.validNp != SRP_NP_VALID) {
        rc = PkaCalcNpIntoPkaReg(LEN_ID_N_BITS, pCtx->groupParam.modSizeInBits, rN, rNp, rT2, rT3);
        if (rc != 0) {
            goto end;
        }
        // save Np into pCtx
        PkaCopyDataFromPkaReg(pCtx->groupParam.Np, CALC_32BIT_WORDS_FROM_BYTES(sizeof(pCtx->groupParam.Np)), rNp);
        pCtx->groupParam.validNp = SRP_NP_VALID;
    } else {
        PkaCopyDataIntoPkaReg(rNp, LEN_ID_MAX_BITS, pCtx->groupParam.Np, CALC_32BIT_WORDS_FROM_BYTES(sizeof(pCtx->groupParam.Np)));
    }
    PkaCopyBeByteBuffIntoPkaReg(rT2, LEN_ID_MAX_BITS, pCtx->kMult, CALC_32BIT_WORDS_FROM_BYTES(pCtx->hashDigestSize));
    PkaCopyBeByteBuffIntoPkaReg(rT3, LEN_ID_MAX_BITS, pwdVerifier, CALC_FULL_32BIT_WORDS(pCtx->groupParam.modSizeInBits));
    PkaCopyDataIntoPkaReg(rT4, LEN_ID_MAX_BITS, &genWord, CALC_FULL_32BIT_WORDS(sizeof(genWord)));
    PkaCopyBeByteBuffIntoPkaReg(rT5, LEN_ID_MAX_BITS, pCtx->ephemPriv, CALC_32BIT_WORDS_FROM_BYTES(pCtx->ephemPrivSize));

    // calculate rT2 = (k*v)%N
    PKA_MOD_MUL(LEN_ID_N_BITS, rT2, rT2/*k*/, rT3 /*v*/);
    // calculate rT4 = (g^b)%N
    PKA_MOD_EXP(LEN_ID_N_BITS, rT4, rT4/*g*/, rT5 /*b*/);
    // calculate rT4 = (rT2 + rT4)%N
    PKA_MOD_ADD(LEN_ID_N_BITS, rT4, rT2/*(k*v)*/, rT4 /*(g^b)*/);

    // verify hostPubKeyB
    /* if hostPubKeyB % N == 0, return with error */
    PKA_MOD_ADD_IM(LEN_ID_N_BITS, rT5, rT4, 0);
    PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, rT5, 0, status);
    if (status == 1) {
        rc = CC_SRP_PARAM_ERROR;
        goto end;
    }

    // copy the generated Host public key
    PkaCopyPkaRegIntoBeByteBuff(hostPubKeyB, CALC_FULL_32BIT_WORDS(pCtx->groupParam.modSizeInBits), rT4);
    end:
    PkaFinishAndMutexUnlock(pkaRegCount);
    return rc;
}

/* calculates A = (g^a)%N */
uint32_t  SrpUserPublicKeyCalc(mbedtls_srp_modulus  userPubKeyA,    // out
                               mbedtls_srp_context  *pCtx)      // in
{
    CCError_t   rc = 0;
    uint32_t    status;
    uint32_t    genWord;
    uint32_t    pkaRegCount = 6; // adding 2 for HW temp regs PKA_REG_T0 & PKA_REG_T1
    /* usage of PKA registers */
    int8_t  rN = PKA_REG_N;
    int8_t  rNp = PKA_REG_NP;
    int8_t  rT2 = regTemps[2];
    int8_t  rT3 = regTemps[3];

    // Verify input
    if ((userPubKeyA == NULL) ||
            (pCtx == NULL)) {
        return CC_SRP_PARAM_INVALID_ERROR;
    }

    genWord = pCtx->groupParam.gen;
    rc = PkaInitAndMutexLock(pCtx->groupParam.modSizeInBits, &pkaRegCount);
    if (rc != 0) {
        return rc;
    }
    // firt copy buffers into PKA reg
    PkaCopyBeByteBuffIntoPkaReg(rN, LEN_ID_MAX_BITS, pCtx->groupParam.modulus, CALC_FULL_32BIT_WORDS(pCtx->groupParam.modSizeInBits));
    if (pCtx->groupParam.validNp != SRP_NP_VALID) {
        rc = PkaCalcNpIntoPkaReg(LEN_ID_N_BITS, pCtx->groupParam.modSizeInBits, rN, rNp, rT2, rT3);
        if (rc != 0) {
            goto end;
        }
        // save Np into pCtx
        PkaCopyDataFromPkaReg(pCtx->groupParam.Np, CALC_32BIT_WORDS_FROM_BYTES(sizeof(pCtx->groupParam.Np)), rNp);
        pCtx->groupParam.validNp = SRP_NP_VALID;
    } else {
        PkaCopyDataIntoPkaReg(rNp, LEN_ID_MAX_BITS, pCtx->groupParam.Np, CALC_32BIT_WORDS_FROM_BYTES(sizeof(pCtx->groupParam.Np)));
    }
    PkaCopyDataIntoPkaReg(rT2, LEN_ID_MAX_BITS, &genWord, CALC_FULL_32BIT_WORDS(sizeof(genWord)));
    PkaCopyBeByteBuffIntoPkaReg(rT3, LEN_ID_MAX_BITS, pCtx->ephemPriv, CALC_32BIT_WORDS_FROM_BYTES(pCtx->ephemPrivSize));

    // calculate rT4 = (g^b)%N
    PKA_MOD_EXP(LEN_ID_N_BITS, rT2, rT2/*g*/, rT3 /*a*/);

    // verify userPubKeyA
    /* if userPubKeyA % N == 0, return with error */
    PKA_MOD_ADD_IM(LEN_ID_N_BITS, rT3, rT2, 0);
    PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, rT3, 0, status);
    if (status == 1) {
        rc = CC_SRP_PARAM_ERROR;
        goto end;
    }
    // copy the generated User public key
    PkaCopyPkaRegIntoBeByteBuff(userPubKeyA, CALC_FULL_32BIT_WORDS(pCtx->groupParam.modSizeInBits), rT2);
    end:
    PkaFinishAndMutexUnlock(pkaRegCount);
    return rc;
}
