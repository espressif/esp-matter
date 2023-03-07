/*
 * Copyright (c) 2017-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file       PKAResourceCC26XX.h
 *
 *  @brief      Shared resources to arbitrate access to the PKA engine
 *
 */

#ifndef ti_drivers_cryptoutils_sharedresources_PKAResourceCC26XX__include
#define ti_drivers_cryptoutils_sharedresources_PKAResourceCC26XX__include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/dpl/SemaphoreP.h>

#ifdef __cplusplus
extern "C" {
#endif

/* PKA driver semaphore used to synchronize accesses to the PKA engine */
extern SemaphoreP_Struct    PKAResourceCC26XX_accessSemaphore;
extern SemaphoreP_Struct    PKAResourceCC26XX_operationSemaphore;

extern volatile bool        PKAResourceCC26XX_pollingFlag;

extern HwiP_Struct          PKAResourceCC26XX_hwi;



void PKAResourceCC26XX_constructRTOSObjects(void);
void PKAResourceCC26XX_destructRTOSObjects(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_cryptoutils_sharedresources_PKAResourceCC26XX__include */
