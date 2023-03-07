/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/** \file
 * \brief This file contains common cryptographic definitions.
 *
 */

#ifndef _CC_CRYPTO_DEFS_H
#define _CC_CRYPTO_DEFS_H


#ifdef __cplusplus
extern "C"
{
#endif


/************************ Hash Definitions ******************************/

#define HASH_MD5_DIGEST_SIZE_IN_BYTES       16
#define HASH_SHA1_DIGEST_SIZE_IN_BYTES      20
#define HASH_SHA224_DIGEST_SIZE_IN_BYTES    28
#define HASH_SHA256_DIGEST_SIZE_IN_BYTES    32
#define HASH_SHA384_DIGEST_SIZE_IN_BYTES    48
#define HASH_SHA512_DIGEST_SIZE_IN_BYTES    64

#define HASH_MD5_BLOCK_SIZE_IN_BYTES        64
#define HASH_SHA1_BLOCK_SIZE_IN_BYTES       64
#define HASH_SHA224_BLOCK_SIZE_IN_BYTES     64
#define HASH_SHA256_BLOCK_SIZE_IN_BYTES     64
#define HASH_SHA384_BLOCK_SIZE_IN_BYTES     128
#define HASH_SHA512_BLOCK_SIZE_IN_BYTES     128



/************************ AES Definitions ******************************/

#define AES_BLOCK_SIZE_IN_BYTES     16

#define AES_IV_SIZE_IN_BYTES        AES_BLOCK_SIZE_IN_BYTES


/* AES-CCM Definitions */
#define AES_CCM_NONCE_LENGTH_MIN    7
#define AES_CCM_NONCE_LENGTH_MAX    13

#define AES_CCM_TAG_LENGTH_MIN      4
#define AES_CCM_TAG_LENGTH_MAX      16



/************************ DES Definitions ******************************/

#define DES_IV_SIZE_IN_BYTES        8






#ifdef __cplusplus
}
#endif

#endif

