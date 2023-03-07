/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @file
 @brief This file contains the errors definitions of the CryptoCell ChaCha-POLY APIs.
 */

/*!
 @defgroup cc_chacha_poly_error Specific errors of the CryptoCell ChaCha-POLY APIs
 @brief Contains the CryptoCell ChaCha-POLY-API errors definitions. See mbedtls_cc_chacha_poly_error.h.
 @{
 @ingroup cc_chacha_poly
 @}
 */


#ifndef _MBEDTLS_CC_CHACHA_POLY_ERROR_H
#define _MBEDTLS_CC_CHACHA_POLY_ERROR_H


#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

/* The base address of errors for the ChaCha-POLY module - 0x00F02400. */
/*! Invalid additional data. */
#define CC_CHACHA_POLY_ADATA_INVALID_ERROR                      (CC_CHACHA_POLY_MODULE_ERROR_BASE + 0x01UL)
/*! Invalid input data. */
#define CC_CHACHA_POLY_DATA_INVALID_ERROR                   (CC_CHACHA_POLY_MODULE_ERROR_BASE + 0x02UL)
/*! Illegal encryption mode. */
#define CC_CHACHA_POLY_ENC_MODE_INVALID_ERROR               (CC_CHACHA_POLY_MODULE_ERROR_BASE + 0x03UL)
/*! Illegal data size. */
#define CC_CHACHA_POLY_DATA_SIZE_INVALID_ERROR              (CC_CHACHA_POLY_MODULE_ERROR_BASE + 0x04UL)
/*! Key-generation error. */
#define CC_CHACHA_POLY_GEN_KEY_ERROR                    (CC_CHACHA_POLY_MODULE_ERROR_BASE + 0x05UL)
/*! ChaCha key-generation error. */
#define CC_CHACHA_POLY_ENCRYPTION_ERROR                 (CC_CHACHA_POLY_MODULE_ERROR_BASE + 0x06UL)
/*! Authentication error. */
#define CC_CHACHA_POLY_AUTH_ERROR                   (CC_CHACHA_POLY_MODULE_ERROR_BASE + 0x07UL)
/*! MAC comparison error. */
#define CC_CHACHA_POLY_MAC_ERROR                                (CC_CHACHA_POLY_MODULE_ERROR_BASE + 0x08UL)

/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  *****************************/

/************************ Public Variables *********************/

/************************ Public Functions *********************/

#ifdef __cplusplus
}
#endif

#endif //_MBEDTLS_CC_CHACHA_POLY_ERROR_H

