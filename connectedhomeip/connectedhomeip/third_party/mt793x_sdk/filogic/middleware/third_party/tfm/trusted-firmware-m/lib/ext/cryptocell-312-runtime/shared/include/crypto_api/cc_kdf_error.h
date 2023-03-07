/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_KDF_ERROR_H
#define _CC_KDF_ERROR_H

#include "cc_error.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*!
@file
@brief This file contains the definitions of the CryptoCell KDF errors.
@defgroup cc_kdf_error CryptoCell Key Derivation specific errors
@{
@ingroup cc_kdf

 */


/************************ Defines *******************************/

/*! The CryptoCell KDF module errors / base address - 0x00F01100*/
/*! Illegal input pointer. */
#define CC_KDF_INVALID_ARGUMENT_POINTER_ERROR           (CC_KDF_MODULE_ERROR_BASE + 0x0UL)
/*! Illegal input size. */
#define CC_KDF_INVALID_ARGUMENT_SIZE_ERROR          (CC_KDF_MODULE_ERROR_BASE + 0x1UL)
/*! Illegal operation mode. */
#define CC_KDF_INVALID_ARGUMENT_OPERATION_MODE_ERROR        (CC_KDF_MODULE_ERROR_BASE + 0x2UL)
/*! Illegal hash mode. */
#define CC_KDF_INVALID_ARGUMENT_HASH_MODE_ERROR         (CC_KDF_MODULE_ERROR_BASE + 0x3UL)
/*! Illegal key derivation mode. */
#define CC_KDF_INVALID_KEY_DERIVATION_MODE_ERROR                (CC_KDF_MODULE_ERROR_BASE + 0x4UL)
/*! Illegal shared secret value size. */
#define CC_KDF_INVALID_SHARED_SECRET_VALUE_SIZE_ERROR           (CC_KDF_MODULE_ERROR_BASE + 0x5UL)
/*! Illegal otherInfo size. */
#define CC_KDF_INVALID_OTHER_INFO_SIZE_ERROR                    (CC_KDF_MODULE_ERROR_BASE + 0x6UL)
/*! Illegal key data size. */
#define CC_KDF_INVALID_KEYING_DATA_SIZE_ERROR                   (CC_KDF_MODULE_ERROR_BASE + 0x7UL)
/*! Illegal algorithm ID pointer. */
#define CC_KDF_INVALID_ALGORITHM_ID_POINTER_ERROR               (CC_KDF_MODULE_ERROR_BASE + 0x8UL)
/*! Illegal algorithm ID size. */
#define CC_KDF_INVALID_ALGORITHM_ID_SIZE_ERROR                  (CC_KDF_MODULE_ERROR_BASE + 0x9UL)
/*! KDF is not supproted. */
#define CC_KDF_IS_NOT_SUPPORTED                                 (CC_KDF_MODULE_ERROR_BASE + 0xFFUL)

/************************ Enums *********************************/

/************************ Typedefs  *****************************/

/************************ Structs  ******************************/

/************************ Public Variables **********************/

/************************ Public Functions **********************/




#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif




