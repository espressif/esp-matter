/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* this file contains the definitions of the hashes used in the rsa */

#include "cc_hash_defs.h"
#include "cc_general_defs.h"

const HmacHash_t HmacHashInfo_t[CC_HASH_NumOfModes] = {
    /*CC_HASH_SHA1_mode         */        {CC_HASH_SHA1_DIGEST_SIZE_IN_BYTES,   CC_HASH_SHA1_mode},
    /*CC_HASH_SHA224_mode       */        {CC_HASH_SHA224_DIGEST_SIZE_IN_BYTES, CC_HASH_SHA224_mode},
    /*CC_HASH_SHA256_mode       */        {CC_HASH_SHA256_DIGEST_SIZE_IN_BYTES, CC_HASH_SHA256_mode},
    /*CC_HASH_SHA384_mode       */        {CC_HASH_SHA384_DIGEST_SIZE_IN_BYTES, CC_HASH_SHA384_mode},
    /*CC_HASH_SHA512_mode       */        {CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES, CC_HASH_SHA512_mode},
    /*CC_HASH_MD5_mode          */        {CC_HASH_MD5_DIGEST_SIZE_IN_BYTES,    CC_HASH_MD5_mode},
};

const uint8_t HmacSupportedHashModes_t[CC_HASH_NumOfModes] = {
    /*CC_HASH_SHA1_mode         */ CC_TRUE,
    /*CC_HASH_SHA224_mode       */ CC_TRUE,
    /*CC_HASH_SHA256_mode       */ CC_TRUE,
    /*CC_HASH_SHA384_mode       */ CC_TRUE,
    /*CC_HASH_SHA512_mode       */ CC_TRUE,
    /*CC_HASH_MD5_mode          */ CC_FALSE,
};

const char HashAlgMode2mbedtlsString[CC_HASH_NumOfModes][CC_HASH_NAME_MAX_SIZE] = {
   "SHA1",
   "SHA224",
   "SHA256",
   "SHA384",
   "SHA512",
   "MD5"
};

