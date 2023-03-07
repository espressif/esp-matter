/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @file
 @brief This file contains all of the CryptoCell POLY APIs, their enums and definitions.
 */

 /*!
 @defgroup cc_poly CryptoCell POLY APIs
 @brief Contains all CryptoCell POLY APIs. See mbedtls_cc_poly.h.

 @{
 @ingroup cryptocell_api
 @}
 */

#ifndef _MBEDTLS_CC_POLY_H
#define _MBEDTLS_CC_POLY_H


#include "cc_pal_types.h"
#include "cc_error.h"


#ifdef __cplusplus
extern "C"
{
#endif

/************************ Defines ******************************/
/*! The size of the POLY key in words. */
#define CC_POLY_KEY_SIZE_IN_WORDS       8
/*! The size of the POLY key in Bytes. */
#define CC_POLY_KEY_SIZE_IN_BYTES       (CC_POLY_KEY_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE)
/*! The size of the POLY MAC in words. */
#define CC_POLY_MAC_SIZE_IN_WORDS       4
/*! The size of the POLY MAC in Bytes. */
#define CC_POLY_MAC_SIZE_IN_BYTES       (CC_POLY_MAC_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE)

/************************ Typedefs  ****************************/

/*! The definition of the ChaCha-MAC buffer. */
typedef uint32_t mbedtls_poly_mac[CC_POLY_MAC_SIZE_IN_WORDS];

/*! The definition of the ChaCha-key buffer. */
typedef uint32_t mbedtls_poly_key[CC_POLY_KEY_SIZE_IN_WORDS];

/************************ Public Functions **********************/

/****************************************************************************************************/
/*!
  @brief This function performs the POLY MAC Calculation.

  @return \c CC_OK on success.
  @return A non-zero value on failure, as defined in mbedtls_cc_poly_error.h.
 */
CIMPORT_C CCError_t  mbedtls_poly(
        mbedtls_poly_key     pKey,         /*!< [in] A pointer to the key buffer of the user. */
        uint8_t              *pDataIn,     /*!< [in] A pointer to the buffer of the input data to the ChaCha. Must not be null. */
        size_t               dataInSize,   /*!< [in] The size of the input data. Must not be zero. */
        mbedtls_poly_mac     macRes        /*!< [in/out] A pointer to the MAC-result buffer.*/
);



#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MBEDTLS_CC_POLY_H */
