/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_CC_API

#ifndef _SRP_H
#define _SRP_H

#include "mbedtls_cc_srp.h"



uint32_t  SrpPwdVerifierCalc(mbedtls_srp_digest xBuff,
        mbedtls_srp_modulus pwdVerifier,
        mbedtls_srp_context *pCtx);

/* calculates S=(((A*v^u)^b)%N */
uint32_t   SrpHostSharedSecretCalc(mbedtls_srp_modulus   userPubKeyA,
        mbedtls_srp_modulus pwdVerifier,
        mbedtls_srp_digest uScramble,
                mbedtls_srp_modulus  sessionKey,
                mbedtls_srp_context  *pCtx);


// Use PKA to calculate S=((B-g^x)^(a+u*x))%N
uint32_t   SrpUserSharedSecretCalc(mbedtls_srp_modulus  hostPubKeyB,
        mbedtls_srp_digest    xBuff,
        mbedtls_srp_digest    uScramble,
                mbedtls_srp_modulus   sessionKey,
                mbedtls_srp_context   *pCtx);

/* calculates B = (k*v+ g^b)%N */
uint32_t  SrpHostPublicKeyCalc(mbedtls_srp_modulus  pwdVerifier,    // in
        mbedtls_srp_modulus hostPubKey,     // out
        mbedtls_srp_context *pCtx);

/* calculates A = (g^a)%N */
uint32_t  SrpUserPublicKeyCalc(mbedtls_srp_modulus  userPubKeyA,    // out
        mbedtls_srp_context *pCtx);

#endif

