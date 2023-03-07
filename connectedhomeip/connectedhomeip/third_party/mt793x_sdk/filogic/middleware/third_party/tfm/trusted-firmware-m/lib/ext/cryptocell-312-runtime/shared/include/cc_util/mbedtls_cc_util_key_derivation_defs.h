/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_utils_key_defs
 @{
*/

/*!
 @file
 @brief This file contains the definitions for the key-derivation API.
 */


#ifndef  _CC_UTIL_KEY_DERIVATION_DEFS_H
#define  _CC_UTIL_KEY_DERIVATION_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
*                           DEFINITIONS
******************************************************************************/

/*! The maximal length of the label in bytes. */
#define CC_UTIL_MAX_LABEL_LENGTH_IN_BYTES   64
/*! The maximal length of the context in bytes. */
#define CC_UTIL_MAX_CONTEXT_LENGTH_IN_BYTES     64
/*! The minimal size of the fixed data in bytes. */
#define CC_UTIL_FIX_DATA_MIN_SIZE_IN_BYTES  3 /*!< \internal counter, 0x00, lengt(-0xff) */
/*! The maximal size of the fixed data in bytes. */
#define CC_UTIL_FIX_DATA_MAX_SIZE_IN_BYTES  4 /*!< \internal counter, 0x00, lengt(0x100-0xff0) */
/*! The maximal size of the derived-key material in bytes. */
#define CC_UTIL_MAX_KDF_SIZE_IN_BYTES (CC_UTIL_MAX_LABEL_LENGTH_IN_BYTES+CC_UTIL_MAX_CONTEXT_LENGTH_IN_BYTES+CC_UTIL_FIX_DATA_MAX_SIZE_IN_BYTES)
/*! The maximal size of the derived-key in bytes. */
#define CC_UTIL_MAX_DERIVED_KEY_SIZE_IN_BYTES 4080

#ifdef __cplusplus
}
#endif

/*!
 @}
*/
#endif /*_CC_UTIL_KEY_DERIVATION_DEFS_H*/
