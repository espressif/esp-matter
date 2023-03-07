/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @file
 @brief This file contains the error definitions of the CryptoCell HKDF APIs.
 */

/*!
 @defgroup cc_hkdf_error Specific errors of the HKDF key-derivation APIs
 @brief Contains the CryptoCell HKDF-API error definitions. See mbedtls_cc_hkdf_error.h.
 @{
 @ingroup cc_hkdf
 @}
 */

#ifndef _MBEDTLS_CC_HKDF_ERROR_H
#define _MBEDTLS_CC_HKDF_ERROR_H

#include "cc_error.h"


#ifdef __cplusplus
extern "C"
{
#endif

/************************ Defines *******************************/

/* The base address for the CryptoCell HKDF module errors - 0x00F01100. */
/*! Invalid argument. */
#define CC_HKDF_INVALID_ARGUMENT_POINTER_ERROR      (CC_HKDF_MODULE_ERROR_BASE + 0x0UL)
/*! Invalid argument size. */
#define CC_HKDF_INVALID_ARGUMENT_SIZE_ERROR         (CC_HKDF_MODULE_ERROR_BASE + 0x1UL)
/*! Illegal hash mode. */
#define CC_HKDF_INVALID_ARGUMENT_HASH_MODE_ERROR        (CC_HKDF_MODULE_ERROR_BASE + 0x3UL)
/*! HKDF not supported. */
#define CC_HKDF_IS_NOT_SUPPORTED                              (CC_HKDF_MODULE_ERROR_BASE + 0xFFUL)

/************************ Enums *********************************/

/************************ Typedefs  *****************************/

/************************ Structs  ******************************/

/************************ Public Variables **********************/

/************************ Public Functions **********************/




#ifdef __cplusplus
}
#endif

#endif //_MBEDTLS_CC_HKDF_ERROR_H

