/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef  _CHACHA_DRIVER_EXT_DMA_H
#define  _CHACHA_DRIVER_EXT_DMA_H

#include "driver_defs.h"




drvError_t FinishChachaExtDma(hashMode_t mode, uint32_t * digest);
drvError_t InitChachaExtDma(uint32_t *nonceBuf, chachaNonceSize_t nonceSizeFlag, uint32_t *keyBuf, uint32_t initialCounter, uint32_t dataLen);
drvError_t terminateChachaExtDma(void);




#endif // #_CHACHA_DRIVER_EXT_DMA_H
