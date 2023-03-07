/*
 *  Copyright (c) 2022, The OpenThread Authors.
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
 * @brief
 *   This file includes the platform-specific initializers.
 */

#include OPENTHREAD_PROJECT_CORE_CONFIG_FILE
#include <assert.h>
#include <string.h>

#include "openthread-system.h"
#include "utils/uart.h"

#include "common/logging.hpp"

#include "em_chip.h"
#include "em_emu.h"
#include "em_system.h"
#include "rail.h"
#include "sl_mpu.h"
#include "sl_sleeptimer.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#include "sl_system_init.h"

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif // SL_CATALOG_POWER_MANAGER_PRESENT

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "sl_system_kernel.h"
#else
#include "sl_system_process_action.h"
#endif

#if OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE
#include "sl_malloc.h"
#include "openthread/heap.h"
#endif

#include "platform-efr32.h"
#include "sl_openthread.h"

#define USE_EFR32_LOG (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED)

otInstance *sInstance;

#ifndef SL_COMPONENT_CATALOG_PRESENT
__WEAK void sl_openthread_init(void)
{
    // Placeholder for enabling Silabs specific features available only through Simplicity Studio
}
#endif // SL_COMPONENT_CATALOG_PRESENT

/**
 * @brief Application initialization
 */
OT_TOOL_WEAK void app_init(void)
{
    // Placeholder for any application specific initialization
}

void otSysInit(int argc, char *argv[])
{
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
    // Initialize Silicon Labs device, system, service(s) and protocol stack(s).
    // Note that if the kernel is present, processing task(s) will be created by
    // this call.
    sl_system_init();

    // Initialize the application. For example, create periodic timer(s) or
    // task(s) if the kernel is present.
    app_init();

    sl_ot_sys_init();
}

void sl_ot_sys_init(void)
{
#if USE_EFR32_LOG
    efr32LogInit();
#endif
    efr32RadioInit();
    efr32AlarmInit();
    efr32MiscInit();
}

bool otSysPseudoResetWasRequested(void)
{
    return false;
}

void otSysDeinit(void)
{
    efr32RadioDeinit();

#if USE_EFR32_LOG
    efr32LogDeinit();
#endif
}

void otSysProcessDrivers(otInstance *aInstance)
{
    sInstance = aInstance;

    // should sleep and wait for interrupts here

#if !defined(SL_CATALOG_KERNEL_PRESENT)
    // Do not remove this call: Silicon Labs components process action routine
    // must be called from the super loop.
    sl_system_process_action();
#endif

#if OPENTHREAD_CONFIG_NCP_HDLC_ENABLE
    efr32UartProcess();
#elif OPENTHREAD_CONFIG_NCP_CPC_ENABLE
    efr32CpcProcess();
#elif OPENTHREAD_CONFIG_NCP_SPI_ENABLE
    efr32SpiProcess();
#endif
    efr32RadioProcess(aInstance);

    // See alarm.c: Wrapped in a critical section
    efr32AlarmProcess(aInstance);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_CATALOG_KERNEL_PRESENT)
    // Let the CPU go to sleep if the system allows it.
    sl_power_manager_sleep();
#endif
}

__WEAK void otSysEventSignalPending(void)
{
    // Intentionally empty
}
