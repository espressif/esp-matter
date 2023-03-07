/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_general_defs
 @{
 */

/*!
 @file
 @brief This file contains general definitions of the CryptoCell runtime SW APIs.
 */


#ifndef _CC_GENERAL_DEFS_H
#define _CC_GENERAL_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_hash_defs.h"

/************************ Defines ******************************/
/*! Hash parameters for HMAC operation. */
typedef struct {
    /*! The size of the HMAC hash result. */
    uint16_t hashResultSize;
    /*! The hash operation mode. */
    CCHashOperationMode_t hashMode;
}HmacHash_t;

/*! The maximal size of the hash string. */
#define CC_HASH_NAME_MAX_SIZE  10
/*! Hash parameters for HMAC operation. */
extern const HmacHash_t HmacHashInfo_t[CC_HASH_NumOfModes];
/*! Supported hash modes. */
extern const uint8_t HmacSupportedHashModes_t[CC_HASH_NumOfModes];
/*! Hash string names. */
extern const char HashAlgMode2mbedtlsString[CC_HASH_NumOfModes][CC_HASH_NAME_MAX_SIZE];


/* general definitions */
/*-------------------------*/
/*! Maximal size of AES HUK in bytes. */
#define CC_AES_KDR_MAX_SIZE_BYTES   32
/*! Maximal size of AES HUK in words. */
#define CC_AES_KDR_MAX_SIZE_WORDS   (CC_AES_KDR_MAX_SIZE_BYTES/sizeof(uint32_t))


/* Life-cycle states. */
/*! The Chip Manufacturer (CM) LCS value. */
#define CC_LCS_CHIP_MANUFACTURE_LCS     0x0
/*! The Secure LCS value. */
#define CC_LCS_SECURE_LCS               0x5

#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif

