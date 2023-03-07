/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_srp_errors
 @{
 */

/*!
 @file
 @brief This file contains the error definitions of the CryptoCell SRP APIs.
 */


#ifndef _MBEDTLS_CC_SRP_ERROR_H
#define _MBEDTLS_CC_SRP_ERROR_H


#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

/************************ Defines ******************************/

/* The base address errors of the CryptoCell SRP module - 0x00F02600 */
/*! Illegal parameter. */
#define CC_SRP_PARAM_INVALID_ERROR           (CC_SRP_MODULE_ERROR_BASE + 0x01UL)
/*! Illegal modulus size. */
#define CC_SRP_MOD_SIZE_INVALID_ERROR        (CC_SRP_MODULE_ERROR_BASE + 0x02UL)
/*! Illegal state (uninitialized) . */
#define CC_SRP_STATE_UNINITIALIZED_ERROR     (CC_SRP_MODULE_ERROR_BASE + 0x03UL)
/*! Result validation error. */
#define CC_SRP_RESULT_ERROR                  (CC_SRP_MODULE_ERROR_BASE + 0x04UL)
/*! Invalid parameter. */
#define CC_SRP_PARAM_ERROR                   (CC_SRP_MODULE_ERROR_BASE + 0x05UL)
/*! Internal PKI error. */
#define CC_SRP_INTERNAL_ERROR                (CC_SRP_MODULE_ERROR_BASE + 0x06UL)

/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  *****************************/

/************************ Public Variables *********************/

/************************ Public Functions *********************/

#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif //_MBEDTLS_CC_SRP_ERROR_H

