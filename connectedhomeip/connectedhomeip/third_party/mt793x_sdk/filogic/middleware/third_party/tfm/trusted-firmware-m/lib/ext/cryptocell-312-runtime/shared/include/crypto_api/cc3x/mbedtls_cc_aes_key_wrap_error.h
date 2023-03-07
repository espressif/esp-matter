/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @file mbedtls_cc_aes_key_wrap_error.h
 @brief This file contains the error definitions of the CryptoCell AES key-wrapping APIs.
 */

/*!
 @defgroup cc_aes_keywrap_error Specific errors of the CryptoCell AES key-wrapping APIs
 @brief Contains the CryptoCell AES key-wrapping-API error definitions.

 See mbedtls_cc_aes_key_wrap_error.h.
 @{
 @ingroup cc_aes_keywrap
 @}
 */

#ifndef _CC_AES_KEYWRAP_ERROR_H
#define _CC_AES_KEYWRAP_ERROR_H


#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

/* CryptoCell AES key-wrapping module errors. #CC_AES_KEYWRAP_MODULE_ERROR_BASE = 0x00F02800 */

/*! Invalid data-in text pointer. */
#define CC_AES_KEYWRAP_DATA_IN_POINTER_INVALID_ERROR        (CC_AES_KEYWRAP_MODULE_ERROR_BASE + 0x00UL)
/*! Invalid data-out text pointer. */
#define CC_AES_KEYWRAP_DATA_OUT_POINTER_INVALID_ERROR       (CC_AES_KEYWRAP_MODULE_ERROR_BASE + 0x01UL)
/*! Invalid key pointer. */
#define CC_AES_KEYWRAP_INVALID_KEY_POINTER_ERROR            (CC_AES_KEYWRAP_MODULE_ERROR_BASE + 0x02UL)
/*! Invalid key size. */
#define CC_AES_KEYWRAP_ILLEGAL_KEY_SIZE_ERROR           (CC_AES_KEYWRAP_MODULE_ERROR_BASE + 0x03UL)
/*! Illegal semiblocks number. */
#define CC_AES_KEYWRAP_SEMIBLOCKS_NUM_ILLEGAL           (CC_AES_KEYWRAP_MODULE_ERROR_BASE + 0x04UL)
/*! Invalid parameter pointer. */
#define CC_AES_KEYWRAP_ILLEGAL_PARAMETER_PTR_ERROR              (CC_AES_KEYWRAP_MODULE_ERROR_BASE + 0x05UL)
/*! Invalid encryption mode. */
#define CC_AES_KEYWRAP_INVALID_ENCRYPT_MODE_ERROR       (CC_AES_KEYWRAP_MODULE_ERROR_BASE + 0x06UL)
/*! Illegal data-in size. */
#define CC_AES_KEYWRAP_DATA_IN_SIZE_ILLEGAL         (CC_AES_KEYWRAP_MODULE_ERROR_BASE + 0x07UL)
/*! Illegal data-out size. */
#define CC_AES_KEYWRAP_DATA_OUT_SIZE_ILLEGAL            (CC_AES_KEYWRAP_MODULE_ERROR_BASE + 0x08UL)
/*! Illegal key-wrapping mode. */
#define CC_AES_KEYWRAP_INVALID_KEYWRAP_MODE_ERROR       (CC_AES_KEYWRAP_MODULE_ERROR_BASE + 0x09UL)
/*! Key Unwrap comparison failure. */
#define CC_AES_KEYWRAP_UNWRAP_COMPARISON_ERROR          (CC_AES_KEYWRAP_MODULE_ERROR_BASE + 0x0AUL)

/*! Not supported. */
#define CC_AES_KEYWRAP_IS_NOT_SUPPORTED             (CC_AES_KEYWRAP_MODULE_ERROR_BASE + 0xFFUL)

/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  *****************************/

/************************ Public Variables *********************/

/************************ Public Functions *********************/

#ifdef __cplusplus
}
#endif

#endif /* _CC_AES_KEYWRAP_ERROR_H */
