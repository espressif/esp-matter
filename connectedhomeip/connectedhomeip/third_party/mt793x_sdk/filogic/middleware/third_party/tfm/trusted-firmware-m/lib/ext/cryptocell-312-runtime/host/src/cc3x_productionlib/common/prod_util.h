/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PROD_UTIL_H
#define _PROD_UTIL_H

#include <stdint.h>
#include "cc_production_asset.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define PROD_MIN(a , b ) ( ( (a) < (b) ) ? (a) : (b) )

uint32_t CC_PROD_PkgVerify(CCProdAssetPkg_t *pPkgAsset,
                            const uint8_t      *pAssetId, uint32_t assetIdSize,
                          const uint8_t     *pLabel, uint32_t labelSize,
                          uint8_t     *pContext, uint32_t contextSize,
                                                    CCPlainAsset_t pPlainAsset,
                               unsigned long workspaceAddr,
                            uint32_t     workspaceSize);

uint32_t  CC_PROD_BitListFromNum(uint32_t *pWordBuff,
                                        uint32_t wordBuffSize,
                                        uint32_t numVal);

uint32_t  CC_PROD_GetZeroCount(uint32_t *pBuff,
                               uint32_t buffWordSize,
                               uint32_t  *pZeroCount);


uint32_t  CCProd_Init(void);

void  CCPROD_Fini(void);


#ifdef __cplusplus
}
#endif
#endif  //_PROD_UTIL_H

