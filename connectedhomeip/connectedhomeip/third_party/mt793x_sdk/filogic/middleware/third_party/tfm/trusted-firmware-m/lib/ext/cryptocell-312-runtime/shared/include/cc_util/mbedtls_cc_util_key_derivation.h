/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_utils_key_derivation
 @{
 */

/*!
 @file
 @brief This file contains the CryptoCell utility key-derivation function APIs.

 The key-derivation function is defined as specified in the
 <em>KDF in Counter Mode</em> section in <em>NIST Special Publication
 800-108: Recommendation for Key Derivation Using Pseudorandom Functions</em>.
 */

#ifndef  _MBEDTLS_CC_UTIL_KEY_DERIVATION_H
#define  _MBEDTLS_CC_UTIL_KEY_DERIVATION_H

#ifdef __cplusplus
extern "C"
{
#endif


#include "mbedtls_cc_util_defs.h"
#include "mbedtls_cc_util_key_derivation_defs.h"
#include "cc_hash_defs.h"

/******************************************************************************
*                           DEFINITIONS
******************************************************************************/

/*! Derivation type of the input key. */
typedef enum  {
    /*! The user key.*/
    CC_UTIL_USER_KEY = 0,
    /*! The device root key (the HUK).*/
    CC_UTIL_ROOT_KEY = 1,
    /*! Total number of keys.*/
    CC_UTIL_TOTAL_KEYS = 2,
    /*! Reserved.*/
    CC_UTIL_END_OF_KEY_TYPE = 0x7FFFFFFF
}mbedtls_util_keytype_t;

/*! Pseudo-random function type for key derivation. */
typedef enum {
    /*! The CMAC function.*/
    CC_UTIL_PRF_CMAC = 0,
    /*! The HMAC function.*/
    CC_UTIL_PRF_HMAC = 1,
    /*! The total number of pseudo-random functions.*/
    CC_UTIL_TOTAL_PRFS = 2,
    /*! Reserved.*/
    CC_UTIL_END_OF_PRF_TYPE = 0x7FFFFFFF
}mbedtls_util_prftype_t;


/*!
  @brief  This function performs key derivation.

  It is defined as specified in the <em>KDF in Counter Mode</em> section in
  <em>NIST Special Publication 800-108: Recommendation for Key Derivation
  Using Pseudorandom Functions</em>.

  The derivation is based on length l, label L, context C, and derivation key
  Ki.

  AES-CMAC or HMAC are used as the pseudo-random function (PRF).

  @note   You must define the label and context for each use-case well
  when using this API.

  @return \c CC_UTIL_OK on success.
  @return A non-zero value from cc_util_error.h on failure.
 */
/*  A key-derivation function can iterates n times until l bits of keying material are generated.
        For each of the iterations of the PRF, i=1 to n, do:
        result(0) = 0;
        K(i) = PRF (Ki, [i] || Label || 0x00 || Context || length);
        results(i) = result(i-1) || K(i);

        concisely, result(i) = K(i) || k(i-1) || .... || k(0)*/
CCUtilError_t mbedtls_util_key_derivation(
    /*! [in] The key type that is used as an input to a key-derivation
    function: \p CC_UTIL_USER_KEY or \p CC_UTIL_ROOT_KEY. */
    mbedtls_util_keytype_t        keyType,
    /*! [in] A pointer to the key buffer of the user, in case of \p
    CC_UTIL_USER_KEY. */
    mbedtls_util_keydata        *pUserKey,
    /*! [in] The PRF type that is used as an input to a key-derivation
    function: \p CC_UTIL_PRF_CMAC or \p CC_UTIL_PRF_HMAC. */
    mbedtls_util_prftype_t        prfType,
    /*! [in] One of the supported hash modes that are defined in \p
    CCHashOperationMode_t. */
    CCHashOperationMode_t       hashMode,
    /*! [in] A string that identifies the purpose for the derived keying
    material.*/
    const uint8_t               *pLabel,
    /*! [in] The label size must be in range of 1 to 64 bytes in length. */
    size_t                      labelSize,
    /*! [in] A binary string containing the information related to the derived
    keying material. */
    const uint8_t               *pContextData,
    /*! [in] The context size must be in range of 1 to 64 bytes in length. */
    size_t                      contextSize,
    /*! [out] Keying material output. Must be at least the size of \p
    derivedKeySize. */
    uint8_t                     *pDerivedKey,
    /*! [in] The size of the derived keying material in bytes, up to 4080
    bytes. */
    size_t                      derivedKeySize
    );


/*!
  @brief  This function performs key derivation using using AES-CMAC.

  It is defined as specified in the <em>KDF in Counter Mode</em> section in
  <em>NIST Special Publication 800-108: Recommendation for Key Derivation
  Using Pseudorandom Functions</em>.

  The derivation is based on length l, label L, context C, and derivation key
  Ki.

  @return \c CC_UTIL_OK on success.
  @return A non-zero value from cc_util_error.h on failure.
 */
#define mbedtls_util_key_derivation_cmac(keyType, pUserKey, pLabel, labelSize, pContextData, contextSize, pDerivedKey, derivedKeySize) \
    mbedtls_util_key_derivation(keyType, pUserKey, CC_UTIL_PRF_CMAC, CC_HASH_OperationModeLast, pLabel, labelSize, pContextData, contextSize, pDerivedKey, derivedKeySize)


/*!
  @brief  This function performs key derivation using HMAC.

  It is defined as specified in the <em>KDF in Counter Mode</em> section in
  <em>NIST Special Publication 800-108: Recommendation for Key Derivation
  Using Pseudorandom Functions</em>.

  The derivation is based on length l, label L, context C, and derivation key
  Ki.

  HMAC is used as the pseudo-random function (PRF).

 @return \c CC_UTIL_OK on success.
 @return A non-zero value from cc_util_error.h on failure.
 */
#define mbedtls_util_key_derivation_hmac(keyType, pUserKey, hashMode, pLabel, labelSize, pContextData, contextSize, pDerivedKey, derivedKeySize) \
    mbedtls_util_key_derivation(keyType, pUserKey, CC_UTIL_PRF_HMAC, hashMode, pLabel, labelSize, pContextData, contextSize, pDerivedKey, derivedKeySize)


#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif /*_MBEDTLS_CC_UTIL_KEY_DERIVATION_H*/

