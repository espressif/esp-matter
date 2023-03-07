/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @file
 @brief This file contains the error definitions of the CryptoCell POLY APIs.

 @defgroup cc_poly_errors Specific errors of the CryptoCell POLY APIs
 @brief Contains the CryptoCell POLY-API error definitions. See mbedtls_cc_poly_error.h.
 @{
 @ingroup cc_poly
 @}
 */

#ifndef _MBEDTLS_CC_POLY_ERROR_H
#define _MBEDTLS_CC_POLY_ERROR_H


#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

/************************ Defines ******************************/

/* The base address of errors for the CryptoCell POLY module - 0x00F02500 */
/*! Invalid key. */
#define CC_POLY_KEY_INVALID_ERROR               (CC_POLY_MODULE_ERROR_BASE + 0x01UL)
/*! Invalid input data. */
#define CC_POLY_DATA_INVALID_ERROR              (CC_POLY_MODULE_ERROR_BASE + 0x02UL)
/*! Illegal input data size. */
#define CC_POLY_DATA_SIZE_INVALID_ERROR         (CC_POLY_MODULE_ERROR_BASE + 0x03UL)
/*! MAC calculation error. */
#define CC_POLY_RESOURCES_ERROR                 (CC_POLY_MODULE_ERROR_BASE + 0x04UL)

/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  *****************************/

/************************ Public Variables *********************/

/************************ Public Functions *********************/

#ifdef __cplusplus
}
#endif

#endif //_MBEDTLS_CC_POLY_ERROR_H


