/*
 * Copyright (c) 2010-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * gpBaseComps.h
 *
 * Initialization function of the BaseComps components.
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */
#ifndef _GPBASECOMPS_H_
#define _GPBASECOMPS_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpBaseComps_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include "global.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpBaseComps_CodeJumpTableFlash_Defs.h"
#include "gpBaseComps_CodeJumpTableRom_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

GP_API void gpBaseComps_StackInit(void);
GP_API void gpBaseComps_StackDeInit(void);

GP_API void gpBaseComps_RegisterPreReInitCallback(void (*callback)(void));
GP_API void gpBaseComps_RegisterPostReInitCallback(void (*callback)(void));
GP_API void gpBaseComps_StackReInit(void);

#ifdef GP_DIVERSITY_LINUXKERNEL
int gpBaseComps_StackInitKernel(void);
void gpBaseComps_StackDeInitKernel(void);
#endif



GP_API void gpBaseComps_MacDeInit(void);
GP_API void gpBaseComps_MacInit(void);

#ifdef __cplusplus
}
#endif

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif // _GPBASECOMPS_H_

