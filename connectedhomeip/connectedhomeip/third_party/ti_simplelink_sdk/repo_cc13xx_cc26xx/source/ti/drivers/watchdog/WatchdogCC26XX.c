/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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

#include <stdint.h>
#include <stdlib.h>

#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include <ti/drivers/watchdog/WatchdogCC26XX.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/watchdog.h)

/* Function prototypes */
void WatchdogCC26XX_clear(Watchdog_Handle handle);
void WatchdogCC26XX_close(Watchdog_Handle handle);
int_fast16_t  WatchdogCC26XX_control(Watchdog_Handle handle, uint_fast16_t cmd,
        void *arg);
void WatchdogCC26XX_init(Watchdog_Handle handle);
Watchdog_Handle WatchdogCC26XX_open(Watchdog_Handle handle, Watchdog_Params *params);
int_fast16_t WatchdogCC26XX_setReload(Watchdog_Handle handle,
        uint32_t ticks);
uint32_t WatchdogCC26XX_convertMsToTicks(Watchdog_Handle handle,
    uint32_t milliseconds);

/* WatchdogCC26XX internal functions */
static void WatchdogCC26XX_initHw(Watchdog_Handle handle);

/* WatchdogCC26XX internal callback function */
static void WatchdogCC26XX_callbackFxn(void);

/* WatchdogCC26XX global variables */
static Watchdog_Handle    watchdogHandle;
static Watchdog_Callback  watchdogUserCallback;

/* Watchdog function table for CC26XX implementation */
const Watchdog_FxnTable WatchdogCC26XX_fxnTable = {
    WatchdogCC26XX_clear,
    WatchdogCC26XX_close,
    WatchdogCC26XX_control,
    WatchdogCC26XX_init,
    WatchdogCC26XX_open,
    WatchdogCC26XX_setReload,
    WatchdogCC26XX_convertMsToTicks
};

/* Maximum allowable setReload value */
#define MAX_RELOAD_VALUE        0xFFFFFFFF
#define WATCHDOG_DIV_RATIO      32            /* Watchdog division ratio */
#define MS_RATIO                1000          /* millisecond to second ratio */

/*
 *  ======== WatchdogCC26XX_callbackFxn ========
 */
static void WatchdogCC26XX_callbackFxn(void)
{
    /* Call user callback function */
    (watchdogUserCallback)((uintptr_t)watchdogHandle);
}

/*
 *  ======== WatchdogCC26XX_clear ========
 */
void WatchdogCC26XX_clear(Watchdog_Handle handle)
{
    WatchdogIntClear();
}

/*
 *  ======== WatchdogCC26XX_close ========
 */
void WatchdogCC26XX_close(Watchdog_Handle handle)
{
    /*
     *  Not supported for CC26XX - Once the INTEN bit of the WDTCTL
     *  register has been set, it can only be cleared by a hardware
     *  reset.
     */
    DebugP_assert(false);
}

/*
 *  ======== WatchdogCC26XX_control ========
 *  @pre    Function assumes that the handle is not NULL
 */
int_fast16_t WatchdogCC26XX_control(Watchdog_Handle handle, uint_fast16_t cmd,
        void *arg)
{
    /* No implementation yet */
    return (Watchdog_STATUS_UNDEFINEDCMD);
}

/*
 *  ======== Watchdog_init ========
 */
void WatchdogCC26XX_init(Watchdog_Handle handle)
{
    WatchdogCC26XX_Object *object = handle->object;

    object->isOpen = false;
}

/*
 *  ======== WatchdogCC26XX_open ========
 */
Watchdog_Handle WatchdogCC26XX_open(Watchdog_Handle handle, Watchdog_Params *params)
{
    unsigned int                   key;
    WatchdogCC26XX_Object         *object;

    /* get the pointer to the object and hwAttrs */
    object = handle->object;

    /* disable preemption while checking if the WatchDog is open. */
    key = HwiP_disable();

    /* Check if the Watchdog is open already with the HWAttrs */
    if (object->isOpen == true) {
        HwiP_restore(key);
        DebugP_log1("Watchdog: Handle %x already in use.", (uintptr_t)handle);
        return (NULL);
    }

    object->isOpen = true;
    HwiP_restore(key);

    /* initialize the Watchdog object */
    object->debugStallMode = params->debugStallMode;
    object->resetMode      = params->resetMode;

    /* setup callback function if defined */
    if (params->callbackFxn != NULL) {
        watchdogHandle = handle;
        watchdogUserCallback = params->callbackFxn;
        HwiP_plug(INT_NMI_FAULT, (void *)WatchdogCC26XX_callbackFxn);
    }

    /* initialize the watchdog hardware */
    WatchdogCC26XX_initHw(handle);

    DebugP_log1("Watchdog: handle %x opened" ,(uintptr_t)handle);

    /* return handle of the Watchdog object */
    return (handle);
}

/*
 *  ======== WatchdogCC26XX_setReload ========
 */
int_fast16_t WatchdogCC26XX_setReload(Watchdog_Handle handle, uint32_t ticks)
{
    unsigned int                   key;

    /* disable preemption while unlocking WatchDog registers */
    key = HwiP_disable();

    /* unlock the Watchdog configuration registers */
    WatchdogUnlock();

    /* make sure the Watchdog is unlocked before continuing */
    while(WatchdogLockState() == WATCHDOG_LOCK_LOCKED)
    { }

    /* update the reload value */
    WatchdogReloadSet(ticks);

    /* lock register access */
    WatchdogLock();

    HwiP_restore(key);

    DebugP_log2("Watchdog: WDT with handle 0x%x has been set to "
        "reload to 0x%x", (uintptr_t)handle, ticks);

    return (Watchdog_STATUS_SUCCESS);
}

/*
 *  ======== WatchdogCC26XX_hwInit ========
 *  This function initializes the Watchdog hardware module.
 *
 *  @pre    Function assumes that the Watchdog handle is pointing to a hardware
 *          module which has already been opened.
 */
static void WatchdogCC26XX_initHw(Watchdog_Handle handle) {
    unsigned int                   key;
    uint32_t                       tickValue;
    WatchdogCC26XX_Object          *object;
    WatchdogCC26XX_HWAttrs const   *hwAttrs;

    /* get the pointer to the object and hwAttrs */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* convert milliseconds to watchdog timer ticks */
    tickValue = WatchdogCC26XX_convertMsToTicks(handle, hwAttrs->reloadValue);

    /* disable preemption while unlocking WatchDog registers */
    key = HwiP_disable();

    /* unlock the Watchdog configuration registers */
    WatchdogUnlock();

    /* make sure the Watchdog is unlocked before continuing */
    while(WatchdogLockState() == WATCHDOG_LOCK_LOCKED)
    { }

    WatchdogReloadSet(tickValue);

    /* set reset mode */
    if (object->resetMode == Watchdog_RESET_ON) {
        WatchdogResetEnable();
    }
    else {
        WatchdogResetDisable();
    }

    /* set debug stall mode */
    if (object->debugStallMode == Watchdog_DEBUG_STALL_ON) {
        WatchdogStallEnable();
    }
    else {
        WatchdogStallDisable();
    }

    /* enable the Watchdog interrupt as a non-maskable interrupt */
    WatchdogIntTypeSet(WATCHDOG_INT_TYPE_NMI);

    /* enable the Watchdog */
    WatchdogEnable();

    /* lock the Watchdog configuration registers */
    WatchdogLock();

    HwiP_restore(key);
}

/*
 *  ======== WatchdogCC26XX_convertMsToTicks ========
 *  This function converts the input value from milliseconds to
 *  Watchdog clock ticks.
 */
uint32_t WatchdogCC26XX_convertMsToTicks(Watchdog_Handle handle,
    uint32_t milliseconds)
{
    uint32_t                       tickValue;
    uint32_t                       convertRatio;
    uint32_t                       maxConvertMs;
    ClockP_FreqHz                   freq;

    /* Determine milliseconds to clock ticks conversion ratio */
    /* Watchdog clock ticks/sec = CPU clock / WATCHDOG_DIV_RATIO */
    /* Watchdog clock ticks/ms = CPU clock / WATCHDOG_DIV_RATIO / 1000 */
    ClockP_getCpuFreq(&freq);
    convertRatio = freq.lo / WATCHDOG_DIV_RATIO / MS_RATIO;
    maxConvertMs = MAX_RELOAD_VALUE / convertRatio;

    /* convert milliseconds to watchdog timer ticks */
    /* check if value exceeds maximum */
    if (milliseconds > maxConvertMs) {
        tickValue = 0;  /* return zero to indicate overflow */
    }
    else {
        tickValue = (uint32_t)(milliseconds * convertRatio);
    }

    return(tickValue);
}
