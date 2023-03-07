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
 * This file implements functions to run the cli on freertos
 *
 * This file is just for example, but not for production.
 *
 */

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "openthread-system.h"
#include <openthread-core-config.h>
#include <openthread/cli.h>
#include <openthread/diag.h>
#include <openthread/tasklet.h>

#include <assert.h>

#ifndef OT_MAIN_TASK_PRIORITY
#define OT_MAIN_TASK_PRIORITY 3
#endif

#ifndef OT_MAIN_TASK_SIZE
#define OT_MAIN_TASK_SIZE ((configSTACK_DEPTH_TYPE)8192 / sizeof(portSTACK_TYPE))
#endif

#if configAPPLICATION_ALLOCATED_HEAP
uint8_t __attribute__((section(".heap"))) ucHeap[configTOTAL_HEAP_SIZE];
#endif

static otInstance * sInstance = NULL;
static TaskHandle_t sMainTask = NULL;

extern void otAppCliInit(otInstance *aInstance);
extern void otSysRunIdleTask(void);

static void appOtInit()
{
#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
    size_t   otInstanceBufferLength = 0;
    uint8_t *otInstanceBuffer       = NULL;
#endif

    otSysInit(0, NULL);

#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
    // Call to query the buffer size
    (void)otInstanceInit(NULL, &otInstanceBufferLength);

    // Call to allocate the buffer
    otInstanceBuffer = (uint8_t *)pvPortMalloc(otInstanceBufferLength);
    assert(otInstanceBuffer);

    // Initialize OpenThread with the buffer
    sInstance = otInstanceInit(otInstanceBuffer, &otInstanceBufferLength);
#else
    sInstance = otInstanceInitSingle();
#endif

#if OPENTHREAD_ENABLE_DIAG
    otDiagInit(sInstance);
#endif
    /* Init the CLI */
    otAppCliInit(sInstance);
}

static void mainloop(void *aContext)
{
    OT_UNUSED_VARIABLE(aContext);
    appOtInit();
    otSysProcessDrivers(sInstance);
    while (!otSysPseudoResetWasRequested())
    {
        otTaskletsProcess(sInstance);
        if (!otTaskletsArePending(sInstance))
        {
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }
        otSysProcessDrivers(sInstance);
    }

    otInstanceFinalize(sInstance);
    vTaskDelete(NULL);
}

void appOtStart(int argc, char *argv[])
{
    xTaskCreate(mainloop, "ot", OT_MAIN_TASK_SIZE, NULL, OT_MAIN_TASK_PRIORITY, &sMainTask);
    vTaskStartScheduler();
}

void otTaskletsSignalPending(otInstance *aInstance)
{
    (void)aInstance;
    xTaskNotifyGive(sMainTask);
}

void otSysEventSignalPending(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(sMainTask, &xHigherPriorityTaskWoken);
    /* Context switch needed? */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

#if (defined(configUSE_IDLE_HOOK) && (configUSE_IDLE_HOOK > 0))
void vApplicationIdleHook(void)
{
    otSysRunIdleTask();
}
#endif

#if (defined(configCHECK_FOR_STACK_OVERFLOW) && (configCHECK_FOR_STACK_OVERFLOW > 0))
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    assert(0);
}
#endif

#if (defined(configUSE_MALLOC_FAILED_HOOK) && (configUSE_MALLOC_FAILED_HOOK > 0))
void vApplicationMallocFailedHook(void)
{
    assert(0);
}
#endif
