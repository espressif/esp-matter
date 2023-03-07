/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CMPU_LLF_RND_DEFS_H
#define _CMPU_LLF_RND_DEFS_H

/************* Include Files ****************/
#include "prod_hw_defs.h"
#include "cc_otp_defs.h"

#define CC_PROD_RND_Fast                                        0
#define CC_PROD_REQUIRED_ENTROPY_BITS                           256

uint32_t CC_PROD_LLF_RND_GetTrngSource(uint32_t           **ppSourceOut,
                       uint32_t           *pSourceOutSize,
                                       uint32_t *pRndWorkBuff);

uint32_t CC_PROD_LLF_RND_VerifyGeneration(uint8_t *pBuff);

#endif //_CMPU_LLF_RND_DEFS_H
