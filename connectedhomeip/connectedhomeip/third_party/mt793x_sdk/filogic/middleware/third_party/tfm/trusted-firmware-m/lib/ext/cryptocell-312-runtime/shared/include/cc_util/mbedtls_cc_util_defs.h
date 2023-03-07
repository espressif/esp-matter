/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_utils_defs
 @{
 */

/*!
 @file
 @brief This file contains general definitions of the CryptoCell utility APIs.
 */

#ifndef  _MBEDTLS_CC_UTIL_DEFS_H
#define  _MBEDTLS_CC_UTIL_DEFS_H



#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_types_plat.h"
#include "mbedtls_cc_util_key_derivation_defs.h"


/******************************************************************************
*                           DEFINITIONS
******************************************************************************/
/*! The size of the AES 128-bit key in bytes. */
#define CC_UTIL_AES_128BIT_SIZE 16
/*! The size of the AES 192-bit key in bytes. */
#define CC_UTIL_AES_192BIT_SIZE 24
/*! The size of the AES 256-bit key in bytes. */
#define CC_UTIL_AES_256BIT_SIZE 32
/*****************************************/
/* CMAC derive key definitions*/
/*****************************************/
/*! The minimal size of the data for the CMAC derivation operation. */
#define CC_UTIL_CMAC_DERV_MIN_DATA_IN_SIZE  CC_UTIL_FIX_DATA_MIN_SIZE_IN_BYTES+2
/*! The maximal size of the data for CMAC derivation operation. */
#define CC_UTIL_CMAC_DERV_MAX_DATA_IN_SIZE  CC_UTIL_MAX_KDF_SIZE_IN_BYTES
/*! The size of the AES CMAC result in bytes. */
#define CC_UTIL_AES_CMAC_RESULT_SIZE_IN_BYTES   0x10UL
/*! The size of the AES CMAC result in words. */
#define CC_UTIL_AES_CMAC_RESULT_SIZE_IN_WORDS   (CC_UTIL_AES_CMAC_RESULT_SIZE_IN_BYTES/sizeof(uint32_t))

/*! Util error type. */
typedef uint32_t CCUtilError_t;



/*! The key data. */
typedef struct mbedtls_util_keydata {
    /*! A pointer to the key. */
    uint8_t*  pKey;
    /*! The size of the key in bytes. */
    size_t    keySize;
}mbedtls_util_keydata;

#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif /*_MBEDTLS_CC_UTIL_DEFS_H*/

