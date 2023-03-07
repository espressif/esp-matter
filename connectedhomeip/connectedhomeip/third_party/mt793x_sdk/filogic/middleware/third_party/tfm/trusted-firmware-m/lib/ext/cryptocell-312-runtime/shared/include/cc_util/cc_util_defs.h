/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  _CC_UTIL_DEFS_H
#define  _CC_UTIL_DEFS_H

/*!
@defgroup cc_utils CryptoCell utility APIs
@{
@ingroup cryptocell_api
@brief This group is the utility apis group
@}

@file
@brief This file contains CryptoCell Util general definitions.
@defgroup cc_utils_defs CryptoCell utils general definitions
@{
@ingroup cc_utils

*/


#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_types_plat.h"
#include "cc_util_key_derivation_defs.h"


/******************************************************************************
*                           DEFINITIONS
******************************************************************************/
/*! Supported AES key size in bits. */
#define CC_UTIL_AES_128BIT_SIZE 16  // same as CC_AES_128_BIT_KEY_SIZE
#define CC_UTIL_AES_192BIT_SIZE 24  // same as CC_AES_192_BIT_KEY_SIZE
#define CC_UTIL_AES_256BIT_SIZE 32  // same as CC_AES_256_BIT_KEY_SIZE
/*****************************************/
/* CMAC derive key definitions*/
/*****************************************/
/*! Minimal data size for CMAC derivation operation. */
#define CC_UTIL_CMAC_DERV_MIN_DATA_IN_SIZE  CC_UTIL_FIX_DATA_MIN_SIZE_IN_BYTES+2
/*! Maximal data size for CMAC derivation operation. */
#define CC_UTIL_CMAC_DERV_MAX_DATA_IN_SIZE  CC_UTIL_MAX_KDF_SIZE_IN_BYTES
/*! AES CMAC result size in bytes. */
#define CC_UTIL_AES_CMAC_RESULT_SIZE_IN_BYTES   0x10UL
/*! AES CMAC result size in words. */
#define CC_UTIL_AES_CMAC_RESULT_SIZE_IN_WORDS   (CC_UTIL_AES_CMAC_RESULT_SIZE_IN_BYTES/sizeof(uint32_t))

/*! Util Error type. */
typedef uint32_t CCUtilError_t;
/*! Defines the CMAC result buffer. */
typedef uint8_t CCUtilAesCmacResult_t[CC_UTIL_AES_CMAC_RESULT_SIZE_IN_BYTES];


/*! Key Data. */
typedef struct CCKeyData_t {
    uint8_t*  pKey;     /*!< Pointer to the key. */
    size_t    keySize;  /*!< The key size in bytes. */
}CCKeyData_t;

#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif /*_CC_UTIL_DEFS_H*/
