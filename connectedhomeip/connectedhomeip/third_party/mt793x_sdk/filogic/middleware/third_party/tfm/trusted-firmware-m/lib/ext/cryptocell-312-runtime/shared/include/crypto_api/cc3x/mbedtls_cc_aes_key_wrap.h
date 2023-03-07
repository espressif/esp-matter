/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @file
 @brief This file contains all of the CryptoCell key-wrapping APIs, their enums and definitions.

 The APIs support AES key wrapping as defined in <em>NIST SP 800-38F: Recommendation for
 Block Cipher Modes of Operation: Methods for Key Wrapping</em>.
 */

/*!
 @defgroup cc_aes_keywrap CryptoCell AES key-wrapping APIs
 @brief Contains CryptoCell key-wrapping APIs.

 See mbedtls_cc_aes_key_wrap.h.
 @{
 @ingroup cc_aes
 @}
 */

#ifndef _MBEDTLS_CC_AES_KEY_WRAP_H
#define _MBEDTLS_CC_AES_KEY_WRAP_H

#include "cc_pal_types.h"
#include "cc_error.h"


#ifdef __cplusplus
extern "C"
{
#endif
/************************ Defines ******************************/
/*! The size of the AES key-wrapping semiblock in Bytes. */
#define CC_AES_KEYWRAP_SEMIBLOCK_SIZE_BYTES     (CC_AES_BLOCK_SIZE_IN_BYTES >> 1)
/*! The size of the AES key-wrapping semiblock in words. */
#define CC_AES_KEYWRAP_SEMIBLOCK_SIZE_WORDS     (CC_AES_KEYWRAP_SEMIBLOCK_SIZE_BYTES >> 2)
/*! The AES key-wrapping semiblock to Bytes shift. */
#define CC_AES_KEYWRAP_SEMIBLOCK_TO_BYTES_SHFT      3
/*! AES key-wrapping with padding (KWP) maximum Bytes of padding. */
#define CC_AES_KEYWRAP_MAX_PAD_LEN                  7

/**********************************/
/** ICVs - Integrity Check Value **/
/**********************************/

/*! The 64-bit default ICV for KW mode. */
#define CC_AES_KEYWRAP_ICV1             {0xA6A6A6A6, 0xA6A6A6A6}
/*! The 32-bit default ICV for KWP mode. */
#define CC_AES_KEYWRAP_ICV2             {0xA65959A6, 0x00000000}

/************************ Typedefs  ****************************/
/*! Supported modes of the AES key-wrapping operation: KW and KWP, as defined in
 <em>NIST SP 800-38F: Recommendation for Block Cipher Modes of Operation: Methods for Key Wrapping</em>. */
typedef enum keyWrapMode {
    CC_AES_KEYWRAP_KW_MODE      = 0, /*!< KW mode. */
    CC_AES_KEYWRAP_KWP_MODE     = 1, /*!< KWP mode. */
    CC_AES_KEYWRAP_NUM_OF_MODES = 2, /*!< Allowed number of AES key-wrapping modes. */
    CC_AES_KEYWRAP_RESERVE32B   = INT32_MAX /*!< Reserved. */
}mbedtls_keywrap_mode_t;


/******************************************* Public Functions *****************************************/

/******************************************************************************************************/
/********                       AES key-wrapping FUNCTION                                     *********/
/******************************************************************************************************/

/*!
 @brief This is the AES wrapping or encryption function.

 AES key-wrapping specifies a deterministic authenticated-encryption mode of operation of the
 AES, according to <em>NIST SP 800-38F: Recommendation for Block Cipher Modes of Operation: Methods for Key Wrapping</em>.
 Its purpose is to protect cryptographic keys.
 It uses units of 8 Bytes called semiblocks. The minimal number of input semiblocks is:
 <ul><li>For KW mode: 2 semiblocks.</li>
 <li>For KWP mode: 1 semiblock.</li></ul>

 The maximal size of the output in Bytes is 64KB. This is a system restriction.
 The input to key-wrapping includes the following elements:
 <ul><li>Payload - text data that is both authenticated and encrypted.</li>
 <li>Key - The encryption key for the AES operation.</li></ul>

 @return \c CC_OK on success.
 @return A non-zero value on failure, as defined in mbedtls_cc_aes_key_wrap_error.h.
 */
CCError_t mbedtls_aes_key_wrap(
            mbedtls_keywrap_mode_t keyWrapFlag,      /*!< [in] The key-wrapping mode: KW or KWP. */
            uint8_t*      keyBuf,                    /*!< [in] A pointer to AES key-wrapping key. */
            size_t        keySize,                   /*!< [in] The size of the key in Bytes. Valid values are:
                                                          16 Bytes, 24 Bytes, or 32 Bytes. */
            uint8_t*      pPlainText,                /*!< [in] A pointer to the plain-text data for encryption. The buffer must be contiguous. */
            size_t        plainTextSize,             /*!< [in] The size of the plain-text data in Bytes. */
            uint8_t*      pCipherText,               /*!< [out] A pointer to the cipher-text output data. The buffer must be contiguous. */
            size_t*       pCipherTextSize            /*!< [in/out] Input: A pointer to the size of the cipher-text output data buffer.
                                                          Output: The actual size of the cipher-text output data in Bytes. */
);

/*!
 @brief This is the AES unwrapping or decryption function.

 AES key-wrapping specifies a deterministic authenticated-encryption mode of operation of the
 AES, according to <em>NIST SP 800-38F: Recommendation for Block Cipher Modes of Operation: Methods for Key Wrapping</em>.
 Its purpose is to protect cryptographic keys.
 It uses units of 8 Bytes called semiblocks. The minimal number of input semiblocks is:
 <ul><li>For KW mode: 2 semiblocks.</li>
 <li>For KWP mode: 1 semiblock.</li></ul>
 The maximal size of the output in bytes is 64KB. This is a system restriction.
 Input to key-wrapping includes the following elements:
 <ul><li>Payload - text data that is both authenticated and encrypted.</li>
 <li>Key - The encryption key for the AES operation.</li></ul>

 @return \c CC_OK on success.
 @return A non-zero value on failure, as defined in mbedtls_cc_aes_key_wrap_error.h.
 */
CCError_t mbedtls_aes_key_unwrap(
              mbedtls_keywrap_mode_t keyWrapFlag,    /*!< [in] The enumerator defining the key-wrapping mode: KW or KWP. */
              uint8_t*      keyBuf,                  /*!< [in] A pointer to AES key-wrapping key. */
              size_t        keySize,                 /*!< [in] The size of the key in Bytes. Valid values are:
                                                          16 Bytes, 24 Bytes, or 32 Bytes.              */
              uint8_t*      pCipherText,             /*!< [in] A pointer to the cipher-text data for decryption. The buffer must be contiguous. */
              size_t        cipherTextSize,          /*!< [in] The size of the cipher-text data in Bytes. */
              uint8_t*      pPlainText,              /*!< [out] A pointer to the plain-text output data. The buffer must be contiguous. */
              size_t*       pPlainTextSize           /*!< [in/out] Input: A pointer to the size of the plain-text output data buffer.
                                                           Output: The actual size of the plain-text output data in Bytes. */
);

#ifdef __cplusplus
}
#endif

#endif /*#ifndef _MBEDTLS_CC_AES_KEY_WRAP_H*/
