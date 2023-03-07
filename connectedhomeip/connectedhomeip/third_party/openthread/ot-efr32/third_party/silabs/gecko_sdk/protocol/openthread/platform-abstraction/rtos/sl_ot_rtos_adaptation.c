/***************************************************************************/
/**
 * @file
 * @brief CMSIS RTOS2 adaptation for running OpenThread in RTOS
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 *
 * https://www.silabs.com/about-us/legal/master-software-license-agreement
 *
 * This software is distributed to you in Source Code format and is governed by
 * the sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <assert.h>
#include <stdio.h>

#include <openthread-core-config.h>
#include <openthread-system.h>
#include <openthread/tasklet.h>

#include <mbedtls/platform.h>

#include "cmsis_os2.h"
#include "sl_ot_rtos_adaptation.h"
#include "sl_ot_init.h"

#include "sl_component_catalog.h"

#ifdef SL_CATALOG_BLUETOOTH_PRESENT
#include "sl_bt_rtos_config.h"

// Configure OpenThread initialization task to use the highest priority to ensure
// OpenThread instantiation and CLI initialization is complete, before other tasks
// have started
#define SL_OPENTHREAD_RTOS_TASK_PRIORITY_STARTUP   (SL_BT_RTOS_LINK_LAYER_TASK_PRIORITY + 1)
#else
#define SL_OPENTHREAD_RTOS_TASK_PRIORITY_STARTUP   (SL_OPENTHREAD_RTOS_TASK_PRIORITY + 1)
#endif // SL_CATALOG_BLUETOOTH_PRESENT

static osThreadId_t      sInitThread   = NULL;
static osThreadId_t      sMainThread   = NULL;

otInstance *otGetInstance(void);

const osThreadAttr_t otInitThreadAttr = {
  .name  = "otInitThread",
  .attr_bits = 0u,
  .stack_size = 2048,
  .priority = (osPriority_t) SL_OPENTHREAD_RTOS_TASK_PRIORITY_STARTUP
};

const osThreadAttr_t otMainThreadAttr = {
  .name  = "otMainThread",
  .attr_bits = 0u,
  .stack_size = 4096,
  .priority = (osPriority_t) SL_OPENTHREAD_RTOS_TASK_PRIORITY
};

static void mainloop(void *aContext)
{
    (void)aContext;

    otInstance *instance = otGetInstance();
    while (!otSysPseudoResetWasRequested())
    {
        otSysProcessDrivers(instance);
        otTaskletsProcess(instance);
        sl_ot_rtos_application_tick();

        if (!otTaskletsArePending(instance)) {
            osThreadFlagsWait (0x0001, osFlagsWaitAny, osWaitForever);
        }
    }

    otInstanceFinalize(instance);
    osThreadTerminate(sMainThread);
}

static void otInit(void *aContext)
{
    (void)aContext;

    // Handling OpenThread initialization within a thread instead of
    // sl_event_handler ensures that, any functions calling mbedTLS API
    // (and subsequently OSMutexPend), are called after the kernel has started.
    sl_ot_init();
    sMainThread =  osThreadNew(mainloop, NULL, &otMainThreadAttr);
    osThreadExit();
}

void sl_ot_rtos_init(void)
{
    sInitThread =  osThreadNew(otInit, NULL, &otInitThreadAttr);
}

static void resumeThread()
{
    if (sMainThread != NULL)
    {
        osThreadFlagsSet(sMainThread, 0x0001);
    }
}

void otTaskletsSignalPending(otInstance *aInstance)
{
    (void)aInstance;
    resumeThread();
}

void otSysEventSignalPending(void)
{
    resumeThread();
}

__WEAK void sl_ot_rtos_application_tick(void)
{
}
