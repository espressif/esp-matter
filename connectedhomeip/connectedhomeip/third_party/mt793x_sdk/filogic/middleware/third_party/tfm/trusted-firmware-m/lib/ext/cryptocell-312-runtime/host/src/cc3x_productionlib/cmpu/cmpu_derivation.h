/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CMPU_DERIVATION_H
#define _CMPU_DERIVATION_H


uint32_t CC_PROD_Derivation_Instantiate (uint32_t *pEntrSrc,
                                         uint32_t  sourceSize,
                                         uint8_t *pKey,
                                         uint8_t *pIv);
uint32_t CC_PROD_Derivation_Generate(uint8_t *pKey,
                                     uint8_t *pIv,
                                     uint32_t *pOutputBuff,
                                     uint32_t  outDataSize);

#endif  //_CMPU_DERIVATION_H

