/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_util_asset_prov
 @{
*/

/*!
 @file mbedtls_cc_util_asset_prov.h
 @brief This file contains CryptoCell runtime-library ICV and OEM asset-provisioning APIs and definitions.
 */


#ifndef  _MBEDTLS_CC_UTIL_ASSET_PROV_H
#define  _MBEDTLS_CC_UTIL_ASSET_PROV_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "cc_pal_types_plat.h"

/*! The maximal size of an asset package. */
#define CC_ASSET_PROV_MAX_ASSET_PKG_SIZE  4144

/*! The type of key used to pack the asset. */
typedef enum {
       /*! The ICV provisioning key (Kpicv) key was used to pack the asset. */
       ASSET_PROV_KEY_TYPE_KPICV = 1,
       /*! The OEM provisioning key (Kcp) key was used to pack the asset. */
       ASSET_PROV_KEY_TYPE_KCP = 2,
       /*! Reserved. */
       ASSET_PROV_KEY_TYPE_RESERVED    = 0x7FFFFFFF,
} CCAssetProvKeyType_t;


/*!
 @brief This function securely provisions ICV or OEM assets to devices using
 CryptoCell.

 The function:
 <ol>
 <li>Receives an encrypted and autenticated asset package.
 \note This asset package is produced by the ICV or OEM asset-packaging
 offline utility (using AES-CCM with key derived from Kpicv or Kcp
 respectively, and the asset identifier).</li>
 <li>Authenticates the asset package.</li>
 <li>Decrypts the asset package.</li>
 <li>Returns the decrypted asset data to the caller.</li></ol>

 @note  The function is valid in all life-cycle states. However, an error
 is returned if the requested key is locked.

 @return \c CC_UTIL_OK on success.
 @return A non-zero value on failure, as defined in cc_util_error.h.
 */
CCError_t mbedtls_util_asset_pkg_unpack(
        /*! [in] The type of key used to pack the asset.*/
        CCAssetProvKeyType_t        keyType,
        /*! [in] A 32-bit index identifying the asset, in big-endian order. */
        uint32_t                    assetId,
        /*! [in] The encrypted and authenticated asset package. */
        uint32_t                    *pAssetPackage,
        /*! [in] The length of the asset package. Must not exceed
        CC_ASSET_PROV_MAX_ASSET_PKG_SIZE. */
        size_t                      assetPackageLen,
        /*! [out] The buffer for retrieving the decrypted asset data. */
        uint32_t                    *pAssetData,
        /*! [in, out] In: The size of the available asset-data buffer. Maximal
        size is 4KB. Out: A pointer to the actual length of the decrypted
        asset data. */
        size_t                      *pAssetDataLen
        );


#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif /*_MBEDTLS_CC_UTIL_ASSET_PROV_H*/

