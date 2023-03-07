/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  _CC_PROD_ASSET_H
#define  _CC_PROD_ASSET_H

/*!
@file
@brief This file contains the functions and definitions for the OEM Asset provisioning.
*/

#ifdef __cplusplus
extern "C"
{
#endif
#include "cc_bitops.h"
#include "cc_prod.h"

#define PROD_ASSET_PROV_TOKEN   0x50726F64UL   // "Prov"
#define PROD_ASSET_PROV_VERSION     0x10000UL

// parameters for generating the temporary key
#define PROD_KEY_RTL_KEY_SIZE       16
#define PROD_KEY_TMP_KEY_SIZE       16
#define PROD_KEY_TMP_LABEL_SIZE     7
#define PROD_ICV_KEY_TMP_LABEL      "KEY ICV"
#define PROD_OEM_KEY_TMP_LABEL       "KEY OEM"
#define PROD_KEY_TMP_CONTEXT_SIZE   16
#define PROD_KEY_TMP_CONTEXT_WORD_SIZE  (PROD_KEY_TMP_CONTEXT_SIZE/CC_32BIT_WORD_SIZE)

// parameters for generating the provisioning key
#define PROD_KPROV_KEY_SIZE     16
#define PROD_KPROV_LABEL_SIZE       1
#define PROD_LABEL  "P"
#define PROD_KPROV_CONTEXT_SIZE     4
#define PROD_ICV_ENC_CONTEXT    "EICV"
#define PROD_ICV_PROV_CONTEXT  "PICV"
#define PROD_OEM_ENC_CONTEXT    "Kce "
#define PROD_OEM_PROV_CONTEXT  "Kcp "

// production asset patameters
#define PROD_ASSET_NONCE_SIZE   12
#define PROD_ASSET_TAG_SIZE     16
#define PROD_ASSET_RESERVED1_VAL    0x52657631UL  // Rev1
#define PROD_ASSET_RESERVED2_VAL    0x52657632UL  // Rev2
#define PROD_ASSET_RESERVED_WORD_SIZE   2
#define PROD_ASSET_RESERVED_SIZE    (PROD_ASSET_RESERVED_WORD_SIZE*CC_32BIT_WORD_SIZE)
#define PROD_ASSET_ADATA_SIZE   (3*CC_32BIT_WORD_SIZE+PROD_ASSET_RESERVED_SIZE)  // token||version||size||reserved

typedef enum {
    PROD_ASSET_ENTITY_TYPE_ICV  = 1,
    PROD_ASSET_ENTITY_TYPE_OEM  = 2,
    PROD_ASSET_ENTITY_TYPE_RESERVED     = 0x7FFFFFFF,
}CCProductionEntityType_t;

typedef enum {
    PROD_ASSET_TYPE_KCE     = 1,
    PROD_ASSET_TYPE_KCP     = 2,
    PROD_ASSET_TYPE_KEY_RESERVED    = 0x7FFFFFFF,
}CCProductionAssetKeyType_t;


typedef struct {
        uint32_t  token;
        uint32_t  version;
        uint32_t  assetSize;
        uint32_t  reserved[PROD_ASSET_RESERVED_WORD_SIZE];
        uint8_t   nonce[PROD_ASSET_NONCE_SIZE];
        uint8_t   encAsset[PROD_ASSET_SIZE+PROD_ASSET_TAG_SIZE];
}CCProdAssetPkg_t;  // Total size must be PROD_ASSET_PKG_SIZE

#ifdef __cplusplus
}
#endif

#endif /*_CC_PROD_ASSET_H */
