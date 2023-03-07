/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file includes the platform-specific initializers.
 *
 */
#include "NVM_Interface.h"
#include "fsl_os_abstraction.h"
#include "ot_platform_common.h"
#include <stdlib.h>
#include <openthread/platform/alarm-milli.h>

#if (defined(LOG_ENABLE) && (LOG_ENABLE > 0))
#include "fsl_component_log_backend_debugconsole.h"
#endif

#ifdef OT_PLAT_SYS_CRYPTO_INIT
#include "ksdk_mbedtls.h"
#endif

#define SYSTEM_BUFFER_LOG_SIZE 256

extern void BOARD_InitHardware(void);

#ifdef OT_PLAT_SYS_LOG_MANAGEMENT
#if (defined(LOG_ENABLE) && (LOG_ENABLE > 0)) && ((defined LOG_ENABLE_ASYNC_MODE) && (LOG_ENABLE_ASYNC_MODE))
static uint8_t bufferLog[SYSTEM_BUFFER_LOG_SIZE];
#endif
#endif

void otPlatExitFunction(void)
{
    OT_PLAT_ERR("======> OT error stack blocked ");
    OSA_TimeDelay(osaWaitForever_c);
}

void otSysInit(int argc, char *argv[])
{
    bool bHwInit = true;

    if ((argc == 1) && (!strcmp(argv[0], "app")))
    {
        bHwInit = false;
    }

    if (bHwInit)
    {
        BOARD_InitHardware();
    }
#ifdef OT_PLAT_SYS_LOG_MANAGEMENT
#if (defined(LOG_ENABLE) && (LOG_ENABLE > 0))
    LOG_Init();
#if (defined(LOG_ENABLE_TIMESTAMP) && (LOG_ENABLE_TIMESTAMP > 0))
    (void)LOG_SetTimestamp((unsigned int (*)(void))otPlatAlarmMilliGetNow);
#endif
    LOG_InitBackendDebugconsole();
#endif
#endif

#ifdef OT_PLAT_SYS_NVM_SUPPORT
    /* Init the NVM module */
    NvModuleInit();
#endif

#ifdef OT_PLAT_SYS_CRYPTO_INIT
    CRYPTO_InitHardware();
#endif

    otPlatRadioInit();
    otPlatAlarmInit();
#ifndef OT_PLAT_SYS_RANDOM_DISABLE
    otPlatRandomInit();
#endif
    atexit(otPlatExitFunction);
}

void otSysDeinit(void)
{
    otPlatRadioDeinit();
    otPlatAlarmDeinit();
#ifndef OT_PLAT_SYS_RANDOM_DISABLE
    otPlatRandomDeinit();
#endif
}

bool otSysPseudoResetWasRequested(void)
{
    return false;
}

void otSysProcessDrivers(otInstance *aInstance)
{
    otPlatAlarmProcess(aInstance);
    otPlatRadioProcess(aInstance);
    otPlatCliUartProcess();
}

void otSysRunIdleTask(void)
{
#ifdef OT_PLAT_SYS_NVM_SUPPORT
    NvIdle();
#endif
#ifdef OT_PLAT_SYS_LOG_MANAGEMENT
#if (defined(LOG_ENABLE) && (LOG_ENABLE > 0)) && ((defined LOG_ENABLE_ASYNC_MODE) && (LOG_ENABLE_ASYNC_MODE))
    LOG_Dump(bufferLog, sizeof(bufferLog), NULL);
#endif
#endif
}
