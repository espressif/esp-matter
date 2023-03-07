/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  _AES_DRIVER_EXT_DMA_H
#define  _AES_DRIVER_EXT_DMA_H

#include "driver_defs.h"
#include "aes_driver.h"





drvError_t finalizeAesExtDma(aesMode_t mode, uint32_t *pIv);
drvError_t terminateAesExtDma(void);


drvError_t AesExtDmaSetIv(aesMode_t mode, uint32_t *pIv);


drvError_t AesExtDmaSetKey(aesMode_t mode, uint32_t *keyBuf, keySizeId_t keySizeId);


void       AesExtDmaSetDataSize(uint32_t dataSize);


drvError_t AesExtDmaInit(cryptoDirection_t encryptDecryptFlag,
        aesMode_t operationMode,
        keySizeId_t keySizeId);



#endif
