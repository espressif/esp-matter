/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* this file contains the definitions of the hashes used in the rsa */
#ifdef CC_IOT
    #if defined(MBEDTLS_CONFIG_FILE)
    #include MBEDTLS_CONFIG_FILE
    #endif
#endif

#if !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C))

#include "cc_rsa_local.h"
#include "cc_hash_defs.h"
#include "cc_rsa_types.h"

const RsaHash_t RsaHashInfo_t[CC_RSA_HASH_NumOfModes] = {
    /*CC_RSA_HASH_MD5_mode          */        {CC_HASH_MD5_DIGEST_SIZE_IN_WORDS,        CC_HASH_MD5_mode},
    /*CC_RSA_HASH_SHA1_mode         */        {CC_HASH_SHA1_DIGEST_SIZE_IN_WORDS,       CC_HASH_SHA1_mode},
    /*CC_RSA_HASH_SHA224_mode       */        {CC_HASH_SHA224_DIGEST_SIZE_IN_WORDS,     CC_HASH_SHA224_mode},
    /*CC_RSA_HASH_SHA256_mode       */        {CC_HASH_SHA256_DIGEST_SIZE_IN_WORDS,     CC_HASH_SHA256_mode},
    /*CC_RSA_HASH_SHA384_mode       */        {CC_HASH_SHA384_DIGEST_SIZE_IN_WORDS,     CC_HASH_SHA384_mode},
    /*CC_RSA_HASH_SHA512_mode       */        {CC_HASH_SHA512_DIGEST_SIZE_IN_WORDS,     CC_HASH_SHA512_mode},
    /*CC_RSA_After_MD5_mode         */        {CC_HASH_MD5_DIGEST_SIZE_IN_WORDS,        CC_HASH_MD5_mode},
    /*CC_RSA_After_SHA1_mode        */        {CC_HASH_SHA1_DIGEST_SIZE_IN_WORDS,       CC_HASH_SHA1_mode},
    /*CC_RSA_After_SHA224_mode      */        {CC_HASH_SHA224_DIGEST_SIZE_IN_WORDS,     CC_HASH_SHA224_mode},
    /*CC_RSA_After_SHA256_mode      */        {CC_HASH_SHA256_DIGEST_SIZE_IN_WORDS,     CC_HASH_SHA256_mode},
    /*CC_RSA_After_SHA384_mode      */        {CC_HASH_SHA384_DIGEST_SIZE_IN_WORDS,     CC_HASH_SHA384_mode},
    /*CC_RSA_After_SHA512_mode      */        {CC_HASH_SHA512_DIGEST_SIZE_IN_WORDS,     CC_HASH_SHA512_mode},
    /*CC_RSA_After_HASH_NOT_KNOWN_mode   */       {0,CC_HASH_NumOfModes},
    /*CC_RSA_HASH_NO_HASH_mode      */        {0,CC_HASH_NumOfModes},
};

#ifdef USE_MBEDTLS_CRYPTOCELL
const mbedtls_md_type_t RsaHash_CC_mbedtls_Info[CC_HASH_NumOfModes] = {
    /* CC_HASH_SHA1_mode */     MBEDTLS_MD_SHA1,
    /* CC_HASH_SHA224_mode */   MBEDTLS_MD_SHA224,
    /* CC_HASH_SHA256_mode */   MBEDTLS_MD_SHA256,
    /* CC_HASH_SHA384_mode */   MBEDTLS_MD_SHA384,
    /* CC_HASH_SHA512_mode */   MBEDTLS_MD_SHA512,
    /* CC_HASH_MD5_mode */      MBEDTLS_MD_MD5
};
#endif
const uint8_t RsaSupportedHashModes_t[CC_RSA_HASH_NumOfModes] = {

        /*CC_RSA_HASH_MD5_mode          */ CC_FALSE,
        /*CC_RSA_HASH_SHA1_mode         */ CC_TRUE,
        /*CC_RSA_HASH_SHA224_mode       */ CC_TRUE,
        /*CC_RSA_HASH_SHA256_mode       */ CC_TRUE,
        /*CC_RSA_HASH_SHA384_mode       */ CC_TRUE,
        /*CC_RSA_HASH_SHA512_mode       */ CC_TRUE,
        /*CC_RSA_After_MD5_mode         */ CC_FALSE,
        /*CC_RSA_After_SHA1_mode        */ CC_TRUE,
        /*CC_RSA_After_SHA224_mode      */ CC_TRUE,
        /*CC_RSA_After_SHA256_mode      */ CC_TRUE,
        /*CC_RSA_After_SHA384_mode      */ CC_TRUE,
        /*CC_RSA_After_SHA512_mode      */ CC_TRUE,
        /*CC_RSA_After_HASH_NOT_KNOWN_mode   */ CC_FALSE,
        /*CC_RSA_HASH_NO_HASH_mode           */ CC_FALSE,
};

#endif /* defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C)) */
