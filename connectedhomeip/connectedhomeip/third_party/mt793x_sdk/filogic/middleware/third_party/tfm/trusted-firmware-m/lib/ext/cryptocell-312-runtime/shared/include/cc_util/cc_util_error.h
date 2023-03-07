/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_utils_errors
 @{
 */

/*!
 @file
 @brief This file contains the error definitions of the CryptoCell utility APIs.
 */

#ifndef  _CC_UTIL_ERROR_H
#define  _CC_UTIL_ERROR_H

#ifdef __cplusplus
extern "C"
{
#endif

/***********************/
/* Util return codes   */
/***********************/
/*! Success definition. */
#define CC_UTIL_OK                                      0x00UL
/*! The error base address definition. */
#define CC_UTIL_MODULE_ERROR_BASE                       0x80000000
/*! Illegal key type. */
#define CC_UTIL_INVALID_KEY_TYPE                        (CC_UTIL_MODULE_ERROR_BASE + 0x00UL)
/*! Illegal data-in pointer. */
#define CC_UTIL_DATA_IN_POINTER_INVALID_ERROR           (CC_UTIL_MODULE_ERROR_BASE + 0x01UL)
/*! Illegal data-in size. */
#define CC_UTIL_DATA_IN_SIZE_INVALID_ERROR              (CC_UTIL_MODULE_ERROR_BASE + 0x02UL)
/*! Illegal data-out pointer. */
#define CC_UTIL_DATA_OUT_POINTER_INVALID_ERROR          (CC_UTIL_MODULE_ERROR_BASE + 0x03UL)
/*! Illegal data-out size. */
#define CC_UTIL_DATA_OUT_SIZE_INVALID_ERROR             (CC_UTIL_MODULE_ERROR_BASE + 0x04UL)
/*! Fatal error. */
#define CC_UTIL_FATAL_ERROR                             (CC_UTIL_MODULE_ERROR_BASE + 0x05UL)
/*! Illegal parameters. */
#define CC_UTIL_ILLEGAL_PARAMS_ERROR                    (CC_UTIL_MODULE_ERROR_BASE + 0x06UL)
/*! Invalid address given. */
#define CC_UTIL_BAD_ADDR_ERROR                          (CC_UTIL_MODULE_ERROR_BASE + 0x07UL)
/*! Illegal domain for endorsement key. */
#define CC_UTIL_EK_DOMAIN_INVALID_ERROR                 (CC_UTIL_MODULE_ERROR_BASE + 0x08UL)
/*! HUK is not valid. */
#define CC_UTIL_KDR_INVALID_ERROR                       (CC_UTIL_MODULE_ERROR_BASE + 0x09UL)
/*! LCS is not valid. */
#define CC_UTIL_LCS_INVALID_ERROR                       (CC_UTIL_MODULE_ERROR_BASE + 0x0AUL)
/*! Session key is not valid. */
#define CC_UTIL_SESSION_KEY_ERROR                       (CC_UTIL_MODULE_ERROR_BASE + 0x0BUL)
/*! Illegal user key size. */
#define CC_UTIL_INVALID_USER_KEY_SIZE                   (CC_UTIL_MODULE_ERROR_BASE + 0x0DUL)
/*! Illegal LCS for the required operation. */
#define CC_UTIL_ILLEGAL_LCS_FOR_OPERATION_ERR           (CC_UTIL_MODULE_ERROR_BASE + 0x0EUL)
/*! Invalid PRF type. */
#define CC_UTIL_INVALID_PRF_TYPE                        (CC_UTIL_MODULE_ERROR_BASE + 0x0FUL)
/*! Invalid hash mode. */
#define CC_UTIL_INVALID_HASH_MODE                       (CC_UTIL_MODULE_ERROR_BASE + 0x10UL)
/*! Unsupported hash mode. */
#define CC_UTIL_UNSUPPORTED_HASH_MODE                   (CC_UTIL_MODULE_ERROR_BASE + 0x11UL)
/*! Key is unusable */
#define CC_UTIL_KEY_UNUSABLE_ERROR                       (CC_UTIL_MODULE_ERROR_BASE + 0x12UL)
#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif /*_CC_UTIL_ERROR_H*/
