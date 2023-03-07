/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_CRYPTO_BOOT_DEFS_H
#define _CC_CRYPTO_BOOT_DEFS_H

/*! @file
@brief This file contains Secure Boot And Secure Debug definitions.
*/
#include "cc_pal_types.h"
/*! Maximal size of secure boot's nonce. */
#define CC_SB_MAX_SIZE_NONCE_BYTES      (2*sizeof(uint32_t))

/*! HASH boot key definition. */
typedef enum {
    CC_SB_HASH_BOOT_KEY_0_128B  = 0,        /*!< 128-bit truncated SHA256 digest of public key 0. */
    CC_SB_HASH_BOOT_KEY_1_128B  = 1,        /*!< 128-bit truncated SHA256 digest of public key 1. */
    CC_SB_HASH_BOOT_KEY_256B    = 2,        /*!< 256-bit SHA256 digest of public key. */
    CC_SB_HASH_BOOT_NOT_USED    = 0xF,
    CC_SB_HASH_MAX_NUM          = 0x7FFFFFFF,   /*!\internal use external 128-bit truncated SHA256 digest */
}CCSbPubKeyIndexType_t;


/*! SW image code encryption type definition. */
typedef enum {
    CC_SB_NO_IMAGE_ENCRYPTION   = 0,        /*!< Plain SW image. */
    CC_SB_ICV_CODE_ENCRYPTION   = 1,        /*!< use Kceicv for cipher SW image. */
    CC_SB_OEM_CODE_ENCRYPTION   = 2,        /*!< use Kce for cipher SW image. */
    CC_SB_CODE_ENCRYPTION_MAX_NUM   = 0x7FFFFFFF,   /*!\internal NA */
}CCswCodeEncType_t;

/*! SW image load and verify scheme. */
typedef enum {
    CC_SB_LOAD_AND_VERIFY       = 0,        /*!< Load & Verify from flash to memory. */
    CC_SB_VERIFY_ONLY_IN_FLASH  = 1,        /*!< Verify only in flash. */
    CC_SB_VERIFY_ONLY_IN_MEM    = 2,        /*!< Verify only in memory. */
    CC_SB_LOAD_ONLY             = 3,        /*!< Load only from flash to memory. */
    CC_SB_LOAD_VERIFY_MAX_NUM   = 0x7FFFFFFF,   /*!\internal NA */
}CCswLoadVerifyScheme_t;

/*! SW image cryptographic type. */
typedef enum {
    CC_SB_HASH_ON_DECRYPTED_IMAGE   = 0,        /*!< AES to HASH. */
    CC_SB_HASH_ON_ENCRYPTED_IMAGE   = 1,        /*!< AES and HASH. */
    CC_SB_CRYPTO_TYPE_MAX_NUM   = 0x7FFFFFFF,   /*!\internal NA */
}CCswCryptoType_t;

/*! Table nonce used in composing IV for SW-component decryption. */
typedef uint8_t CCSbNonce_t[CC_SB_MAX_SIZE_NONCE_BYTES];

/*! SW components data.*/
typedef struct {
    /*! Num of SW components. */
    uint32_t  numOfSwComps;

    /*! SW image code encryption type. */
    CCswCodeEncType_t swCodeEncType;

    /*! SW image load & verify scheme. */
    CCswLoadVerifyScheme_t swLoadVerifyScheme;

    /*! SW image crypto type. */
    CCswCryptoType_t swCryptoType;

    /*! Nonce. */
    CCSbNonce_t nonce;

    /*! Pointer to start of sw components data. */
    uint8_t *pSwCompsData;

}CCSbCertParserSwCompsInfo_t;

/*! SW version */
typedef struct {
    CCSbPubKeyIndexType_t keyIndex;     /*!< Enumeration defining the key hash to retrieve: 128-bit HBK0, 128-bit HBK1, or 256-bit HBK. */
    uint32_t swVersion;         /*!< Sw version.*/
}CCSbSwVersion_t;

#endif
