/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef  _HASH_DRIVER_EXT_DMA_H
#define  _HASH_DRIVER_EXT_DMA_H

#include "driver_defs.h"




drvError_t FinishHashExtDma(hashMode_t mode, uint32_t * digest);
drvError_t InitHashExtDma(hashMode_t mode, uint32_t dataSize);
drvError_t terminateHashExtDma(void);




#endif // #_HASH_DRIVER_EXT_DMA_H
