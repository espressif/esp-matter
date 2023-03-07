/******************************************************************************

 @file cli.c

 @brief Main application file for the Command Line Interpreter TIRTOS example

 Group: CMCU, LPC
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#include <openthread/config.h>
#include <openthread-core-config.h>

/* Standard Library Header files */
#include <assert.h>
#include <stddef.h>

/* POSIX Header files */
#include <sched.h>
#include <pthread.h>
#include <unistd.h>

/* RTOS Header files */
#include <ti/drivers/GPIO.h>

/* OpenThread public API Header files */
#include <openthread/cli.h>
#include <openthread/instance.h>

/* OpenThread Internal/Example Header files */
#include "otsupport/otrtosapi.h"
#include "otsupport/otinstance.h"

/* Example/Board Header files */
#include "task_config.h"
#include "ti_drivers_config.h"

#if TIOP_OAD
/* OAD required Header files */
#include "oad_image_header.h"
/* Low level driverlib files (non-rtos) */
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/flash.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/cpu.h)
#endif /* TIOP_OAD */

/* Application thread */
void *cli_task(void *arg0);

/* Application thread call stack */
static char cli_stack[TASK_CONFIG_CLI_TASK_STACK_SIZE];

/**
 * Documented in task_config.h.
 */
void cli_taskCreate(void)
{
    pthread_t           thread;
    pthread_attr_t      pAttrs;
    struct sched_param  priParam;
    int                 retc;

    retc = pthread_attr_init(&pAttrs);
    assert(retc == 0);

    retc = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
    assert(retc == 0);

    priParam.sched_priority = TASK_CONFIG_CLI_TASK_PRIORITY;
    retc = pthread_attr_setschedparam(&pAttrs, &priParam);
    assert(retc == 0);

    retc = pthread_attr_setstack(&pAttrs, (void *)cli_stack,
                                 TASK_CONFIG_CLI_TASK_STACK_SIZE);
    assert(retc == 0);

    retc = pthread_create(&thread, &pAttrs, cli_task, NULL);
    assert(retc == 0);

    retc = pthread_attr_destroy(&pAttrs);
    assert(retc == 0);

    (void) retc;

    GPIO_setConfig(CONFIG_GPIO_BTN1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING);
    GPIO_setConfig(CONFIG_GPIO_BTN2, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING);
    GPIO_setConfig(CONFIG_GPIO_GLED, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH |
                                    GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_RLED, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH |
                                    GPIO_CFG_OUT_LOW);
}

/**
 * @brief Invalidate the OAD IMAGE HEADER.
 *
 * This provides a way to revert to factory image by invalidating the existing
 * stack/application image and doing a system reset. On boot, the BIM
 * on finding the internal invalidated image will restore to factory image.
 *
 * @return None
 */
#if TIOP_OAD
static void TIOP_OAD_invalidate_image_header(void)
{
    /* our data buffer cannot be in flash... so it is on the stack */
    uint8_t zeros[sizeof(oad_image_header.h.imgID)];
    /*
     * We only need to invalidate the IMAGE header
     * We can do this by writing a zero  "zeros" over the signature.
     */

    /* no IRQ chance, we disable here */
    CPUcpsid();

    memset(zeros, 0, sizeof(zeros));
    FlashProgram(&zeros[0],
                 (uint32_t)(&oad_image_header.h.imgID),
                 sizeof(oad_image_header.h.imgID));

    /* press the virtual reset button */
    SysCtrlSystemReset();
}
#endif /* TIOP_OAD */

/**
 * Main thread starting the CLI example within OpenThread.
 */
void *cli_task(void *arg0)
{
    GPIO_write(CONFIG_GPIO_RLED, 1);

    OtStack_taskCreate();

#if TIOP_OAD
    if (!GPIO_read(CONFIG_GPIO_BTN1))
    {
        TIOP_OAD_invalidate_image_header();
        /* does not return */
    }
#endif /* TIOP_OAD */

#ifndef TIOP_POWER_MEASUREMENT
    /* If button 2 is pressed on boot, reset the OpenThread settings */
    if (!GPIO_read(CONFIG_GPIO_BTN2))
    {
        OtRtosApi_lock();
        otInstanceFactoryReset(OtInstance_get());
        OtRtosApi_unlock();
    }
#endif /* !TIOP_POWER_MEASUREMENT */

    OtRtosApi_lock();
    otCliUartInit(OtInstance_get());
    OtRtosApi_unlock();

    while (1)
    {
        sleep(2);
        /* ignoring unslept return value */
        GPIO_toggle(CONFIG_GPIO_RLED);
    }
}

/*
 * Provide, if required an "otPlatLog()" function
 *
 * This function is used by the OpenThread stack, be very careful logging
 * application data with it.
 */
#if OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_APP
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    OT_UNUSED_VARIABLE(aLogLevel);
    OT_UNUSED_VARIABLE(aLogRegion);
    OT_UNUSED_VARIABLE(aFormat);

    va_list ap;
    va_start(ap, aFormat);
    /* This is a callback from OpenThread, do not lock the API here */
    otCliPlatLogv(aLogLevel, aLogRegion, aFormat, ap);
    va_end(ap);
}
#endif

