/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef LLF_RSA_PUBLIC_H
#define LLF_RSA_PUBLIC_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */

#include "cc_error.h"
#include "cc_rsa_types.h"


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct {
    /* the Barrett mod tag  NP for N-modulus - used in the modular multiplication and
       exponentiation, calculated in CC_RsaPrivKeyBuild function */
    uint32_t NP[CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS];
}RsaPubKeyDb_t;

CCError_t RsaExecPubKeyExp(CCRsaPubKey_t     *pPubKey,
                CCRsaPrimeData_t *pPubData );

CCError_t RsaInitPubKeyDb(CCRsaPubKey_t *pPubKey);

#ifdef __cplusplus
}
#endif

#endif
