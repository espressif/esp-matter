/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @file
 @brief This file contains the CryptoCell HKDF key-derivation function API.

 This function is as defined in
 <em>RFC-5869: HMAC-based Extract-and-Expand Key Derivation Function (HKDF)</em>.
 */

/*!
 @defgroup cc_hkdf CryptoCell HKDF key-derivation function API
 @brief Contains the CryptoCell HMAC key-derivation function API. See mbedtls_cc_hkdf.h.

 @{
 @ingroup cryptocell_api
 @}
 */

#ifndef _MBEDTLS_CC_HKDF_H
#define _MBEDTLS_CC_HKDF_H


#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_types.h"

/*! The maximal size of the HKDF key in words. */
#define CC_HKDF_MAX_HASH_KEY_SIZE_IN_BYTES        512

/*! The maximal size of the HKDF hash-digest in Bytes. */
#define CC_HKDF_MAX_HASH_DIGEST_SIZE_IN_BYTES     CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES

/************************ Defines ******************************/

/************************ Enums ********************************/
/*! Supported HKDF hash modes. */
typedef enum
{
    /*! SHA-1 mode. */
    CC_HKDF_HASH_SHA1_mode      = 0,
    /*! SHA-224 mode. */
    CC_HKDF_HASH_SHA224_mode  = 1,
    /*! SHA-256 mode. */
    CC_HKDF_HASH_SHA256_mode  = 2,
    /*! SHA-384 mode. */
    CC_HKDF_HASH_SHA384_mode  = 3,
    /*! SHA-512 mode. */
    CC_HKDF_HASH_SHA512_mode  = 4,
    /*! The maximal number of hash modes. */
    CC_HKDF_HASH_NumOfModes,
    /*! Reserved. */
    CC_HKDF_HASH_OpModeLast    = 0x7FFFFFFF,

}mbedtls_hkdf_hashmode_t;

/************************ Typedefs  ****************************/

/************************ Structs  ******************************/

/************************ Public Variables **********************/

/************************ Public Functions **********************/

/****************************************************************/


/*********************************************************************************************************/
/*!
  @brief mbedtls_hkdf_key_derivation() performs the HMAC-based key derivation, as define by
  <em>RFC-5869: HMAC-based Extract-and-Expand Key Derivation Function (HKDF)</em>.

  @return \c CC_OK on success.
  @return A non-zero value on failure as defined in cc_kdf_error.h, or in md.h.
*/
CCError_t  mbedtls_hkdf_key_derivation(
            mbedtls_hkdf_hashmode_t    HKDFhashMode,   /*!< [in] The HKDF identifier of the hash function to be used. */
            uint8_t*                   Salt_ptr,       /*!< [in] A pointer to a non-secret random value. Can be NULL. */
            size_t                     SaltLen,        /*!< [in] The size of the \p Salt_ptr. */
            uint8_t*                   Ikm_ptr,        /*!< [in] A pointer to an input key message. */
            uint32_t                   IkmLen,         /*!< [in] The size of the input key message */
            uint8_t*                   Info,           /*!< [in] A pointer to an optional context and application-specific information. Can be NULL */
            uint32_t                   InfoLen,        /*!< [in] The size of the application-specific information. */
            uint8_t*                   Okm,            /*!< [in] A pointer to an output key material. */
            uint32_t                   OkmLen,         /*!< [in] The size of the output key material. */
            CCBool                     IsStrongKey     /*!< [in] If TRUE, no need to perform the extraction phase. */
);

#ifdef __cplusplus
}
#endif

#endif

