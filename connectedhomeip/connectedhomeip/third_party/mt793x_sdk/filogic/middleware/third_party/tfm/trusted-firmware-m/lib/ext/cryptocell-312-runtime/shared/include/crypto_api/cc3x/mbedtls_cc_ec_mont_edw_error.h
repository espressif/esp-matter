/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _MBEDTLS_CC_EC_MONT_EDW_ERROR_H
#define _MBEDTLS_CC_EC_MONT_EDW_ERROR_H


/*!
@file
@brief This file contains the definitions of the CryptoCell ECC-25519 errors.
@defgroup cc_ecmontedw_error CryptoCell ECC-25519 errors
@{
@ingroup cryptocell_api
*/

#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

/**********************************************************************************************************
 * CryptoCell ECC-25519 MODULE ERRORS    base address - 0x00F02100                *
 **********************************************************************************************************/
/*! Illegal input pointer */
#define CC_EC_EDW_INVALID_INPUT_POINTER_ERROR                 (CC_EC_MONT_EDW_MODULE_ERROR_BASE + 0x00UL)
/*! Illegal input size */
#define CC_EC_EDW_INVALID_INPUT_SIZE_ERROR                (CC_EC_MONT_EDW_MODULE_ERROR_BASE + 0x01UL)
/*! Illegal scalar size */
#define CC_EC_EDW_INVALID_SCALAR_SIZE_ERROR               (CC_EC_MONT_EDW_MODULE_ERROR_BASE + 0x02UL)
/*! Illegal scalar data */
#define CC_EC_EDW_INVALID_SCALAR_DATA_ERROR               (CC_EC_MONT_EDW_MODULE_ERROR_BASE + 0x03UL)
/*! Invalid RND context pointer */
#define CC_EC_EDW_RND_CONTEXT_PTR_INVALID_ERROR              (CC_EC_MONT_EDW_MODULE_ERROR_BASE + 0x04UL)
/*! Invalid RND generate vector functions pointer */
#define CC_EC_EDW_RND_GEN_VECTOR_FUNC_ERROR               (CC_EC_MONT_EDW_MODULE_ERROR_BASE + 0x05UL)
/*! Signing or verification operation failed */
#define CC_EC_EDW_SIGN_VERIFY_FAILED_ERROR                (CC_EC_MONT_EDW_MODULE_ERROR_BASE + 0x20UL)
/*! Illegal input pointer */
#define CC_EC_MONT_INVALID_INPUT_POINTER_ERROR              (CC_EC_MONT_EDW_MODULE_ERROR_BASE + 0x30UL)
/*! Illegal input size */
#define CC_EC_MONT_INVALID_INPUT_SIZE_ERROR                 (CC_EC_MONT_EDW_MODULE_ERROR_BASE + 0x31UL)
/*! Illegal domain id */
#define CC_EC_MONT_INVALID_DOMAIN_ID_ERROR                  (CC_EC_MONT_EDW_MODULE_ERROR_BASE + 0x32UL)
/*! Internal PKI error */
#define CC_ECEDW_INTERNAL_ERROR                             (CC_EC_MONT_EDW_MODULE_ERROR_BASE + 0x33UL)
/*! Internal PKI error */
#define CC_ECMONT_INTERNAL_ERROR                            (CC_EC_MONT_EDW_MODULE_ERROR_BASE + 0x34UL)


/************************************************************************************************************
 *    NOT SUPPORTED MODULES ERROR IDs                                                                       *
 ************************************************************************************************************/
/*! EC montgomery is not supported */
#define CC_EC_MONT_IS_NOT_SUPPORTED                         (CC_ECPKI_MODULE_ERROR_BASE + 0xFEUL)
/*! EC edwards is not supported */
#define CC_EC_EDW_IS_NOT_SUPPORTED                          (CC_ECPKI_MODULE_ERROR_BASE + 0xFFUL)



/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  ******************************/

/************************ Public Variables **********************/

/************************ Public Functions **********************/

#ifdef __cplusplus
}
#endif
/**
@}
*/
#endif//_MBEDTLS_CC_EC_MONT_EDW_ERROR_H


