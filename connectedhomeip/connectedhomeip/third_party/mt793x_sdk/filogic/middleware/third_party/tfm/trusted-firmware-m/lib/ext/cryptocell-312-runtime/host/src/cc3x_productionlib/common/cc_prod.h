/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup prod_mem
 @{
 */

/*!
 @file
 @brief This file contains all of the enums and definitions that are used for
 the ICV and OEM production libraries.
 */

#ifndef _PROD_H
#define _PROD_H


/************************ Defines ******************************/

/*! The definition of the number of bytes in a word. */
#define CC_PROD_32BIT_WORD_SIZE    sizeof(uint32_t)
/*! The size of the plain-asset in bytes. */
#define PROD_ASSET_SIZE     16
/*! The size of the asset-package in bytes. */
#define PROD_ASSET_PKG_SIZE     64
/*! The size of the asset-package in words. */
#define PROD_ASSET_PKG_WORD_SIZE    (PROD_ASSET_PKG_SIZE/CC_PROD_32BIT_WORD_SIZE)
/*! The number of words of the DCU lock. */
#define PROD_DCU_LOCK_WORD_SIZE     4

/************************ Enums ********************************/

/*! The type of the provided asset. */
typedef enum {
    /*! The asset is not provided. */
        ASSET_NO_KEY = 0,
    /*! The asset is provided as plain, not in a package. */
        ASSET_PLAIN_KEY = 1,
    /*! The asset is provided as a package. */
        ASSET_PKG_KEY = 2,
    /*! Reserved. */
        ASSET_TYPE_RESERVED     = 0x7FFFFFFF,
} CCAssetType_t;

/************************ Typedefs  ****************************/

/*! Defines the buffer of the plain asset, as a 16-byte array. */
typedef uint8_t CCPlainAsset_t[PROD_ASSET_SIZE];
/*! Defines the buffer of the asset-package, as a 64-byte array. */
typedef uint32_t CCAssetPkg_t[PROD_ASSET_PKG_WORD_SIZE];


/************************ Structs  ******************************/

/*! @brief The asset buffer.

  If the asset is provided as plain asset, the \p plainAsset field is used.
  Otherwise, the \p pkgAsset field is used.
  */
typedef union {
    /*! Plain asset buffer. */
        CCPlainAsset_t     plainAsset;
    /*! Asset-package buffer. */
        CCAssetPkg_t       pkgAsset;
} CCAssetBuff_t;

/*!
 @}
 */
#endif  //_PROD_H
