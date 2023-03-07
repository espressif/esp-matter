/******************************************************************************

 @file  watchdogcc26XX.c

 @brief Watchdog driver implementation for CC26XX

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2022, Texas Instruments Incorporated
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

#include <stdint.h>
#include <stdlib.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Types.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include "watchdogcc26XX.h"

#include <driverlib/watchdog.h>

/* Function prototypes */
void WatchdogCC26XX_clear(Watchdog_Handle handle);
void WatchdogCC26XX_close(Watchdog_Handle handle);
int  WatchdogCC26XX_control(Watchdog_Handle handle, unsigned int cmd, void *arg);
void WatchdogCC26XX_init(Watchdog_Handle handle);
Watchdog_Handle WatchdogCC26XX_open(Watchdog_Handle handle, Watchdog_Params *params);
void WatchdogCC26XX_setReload(Watchdog_Handle handle, uint32_t value);
static void WatchdogCC26XX_initHw(Watchdog_Handle handle);
static uint32_t watchdogPreNotify(uint32_t eventType, uint32_t clientArg);
static uint32_t watchdogPostNotify(uint32_t eventType, uint32_t clientArg);

/* Watchdog function table for CC26XX implementation */
const Watchdog_FxnTable WatchdogCC26XX_fxnTable = {
    WatchdogCC26XX_clear,
    WatchdogCC26XX_close,
    WatchdogCC26XX_control,
    WatchdogCC26XX_init,
    WatchdogCC26XX_open,
    WatchdogCC26XX_setReload
};

/* Default Watchdog params */
extern const Watchdog_Params Watchdog_defaultParams;

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
    /* Not supported for CC26XX */
    Assert_isTrue(false, NULL);
}

/*
 *  ======== WatchdogCC26XX_control ========
 *  @pre    Function assumes that the handle is not NULL
 */
int WatchdogCC26XX_control(Watchdog_Handle handle, unsigned int cmd, void *arg)
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
    Hwi_Params                     hwiParams;
    Watchdog_Params                watchdogParams;
    WatchdogCC26XX_HWAttrs const  *hwAttrs;
    WatchdogCC26XX_Object         *object;
    Types_FreqHz                   freq;

    /* get the pointer to the object and hwAttrs */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* disable preemption while checking if the WatchDog is open. */
    key = Hwi_disable();

    /* Check if the Watchdog is open already with the HWAttrs */
    if (object->isOpen == true) {
        Hwi_restore(key);
        Log_warning1("Watchdog: Handle %x already in use.", (UArg)handle);
        return (NULL);
    }

    object->isOpen = true;
    Hwi_restore(key);

    /* if params are NULL use defaults. */
    if (params == NULL) {
        Watchdog_Params_init(&watchdogParams);
        params = &watchdogParams;
    }

    /* initialize the Watchdog object */
    object->debugStallMode = params->debugStallMode;
    object->resetMode      = params->resetMode;

    /* 1 second period at default CPU clock frequency */
    BIOS_getCpuFreq(&freq);
    object->reloadValue = freq.lo/2;

    /* Construct Hwi object for Watchdog */
    Hwi_Params_init(&hwiParams);
    hwiParams.arg = (UArg)handle;

    /* setup callback function if defined */
    if (params->callbackFxn != NULL) {
        Hwi_construct(&(object->hwi), hwAttrs->intNum, params->callbackFxn,
                      &hwiParams, NULL);
    }

    /* initialize the watchdog hardware */
    // WatchdogIntClear();
    WatchdogCC26XX_initHw(handle);

    Log_print1(Diags_USER1, "Watchdog: handle %x opened" ,(UArg)handle);

    /* register notification functions */
    Power_registerNotify(&object->watchdogPreObj, PowerCC26XX_ENTERING_STANDBY, (Fxn)watchdogPreNotify, (uint32_t)handle);
    Power_registerNotify(&object->watchdogPostObj, PowerCC26XX_AWAKE_STANDBY, (Fxn)watchdogPostNotify, (uint32_t)handle);

    /* return handle of the Watchdog object */
    return (handle);
}

/*
 *  ======== WatchdogCC26XX_setReload ========
 */
void WatchdogCC26XX_setReload(Watchdog_Handle handle, uint32_t value)
{
    WatchdogCC26XX_Object        *object;

    /* get the pointer to the object and hwAttrs */
    object = handle->object;

    /* udpate the watchdog object with the new reload value */
    object->reloadValue = value;

    /* unlock the Watchdog configuration registers */
    WatchdogUnlock();

    /* make sure the Watchdog is unlocked before continuing */
    while(WatchdogLockState() == WATCHDOG_LOCK_LOCKED)
    { }

    /* update the reload value */
    WatchdogReloadSet(object->reloadValue);

    /* lock register access */
    WatchdogLock();

    Log_print2(Diags_USER1, "Watchdog: WDT with handle 0x%x has been set to "
               "reload to 0x%x", (UArg)handle, value);
}

/*
 *  ======== WatchdogCC26XX_hwInit ========
 *  This functions initializes the Watchdog hardware module.
 *
 *  @pre    Function assumes that the Watchdog handle is pointing to a hardware
 *          module which has already been opened.
 */
static void WatchdogCC26XX_initHw(Watchdog_Handle handle) {
    WatchdogCC26XX_Object        *object;

    /* get the pointer to the object and hwAttrs */
    object = handle->object;

    /* unlock the Watchdog configuration registers */
    WatchdogUnlock();

    /* make sure the Watchdog is unlocked before continuing */
    while(WatchdogLockState() == WATCHDOG_LOCK_LOCKED)
    { }

    WatchdogReloadSet(object->reloadValue);

#ifndef CCWARE
    /* set reset mode */
    if (object->resetMode == Watchdog_RESET_ON) {
        WatchdogResetEnable();
    }
    else {
        WatchdogResetDisable();
    }
#endif

    /* set debug stall mode */
    if (object->debugStallMode == Watchdog_DEBUG_STALL_ON) {
        WatchdogStallEnable();
    }
    else {
        WatchdogStallDisable();
    }

    /* enable the Watchdog interrupt as a non-maskable interrupt */
    WatchdogIntTypeSet(WATCHDOG_INT_TYPE_NMI);

    /* enable interrupts */
    WatchdogIntEnable();

    /* enable the Watchdog */
    WatchdogEnable();

    /* lock the Watchdog configuration registers */
    WatchdogLock();
}

/*
 *  ======== watchdogPreNotify ========
 *  This functions is called to notify the Watchdog driver of a imminent transition
 *  in to sleep mode.
 *
 *  @pre    Function assumes that the Watchdog handle (clientArg) is pointing to a
 *          hardware module which has already been opened.
 */
uint32_t watchdogPreNotify(uint32_t eventType, uint32_t clientArg)
{
    /* return value */
    uint32_t res;

    res = Power_NOTIFYDONE;
    return res;
}

/*
 *  ======== watchdogPostNotify ========
 *  This functions is called to notify the Watchdog driver of an ongoing transition
 *  out of sleep mode.
 *
 *  @pre    Function assumes that the Watchdog handle (clientArg) is pointing to a
 *          hardware module which has already been opened.
 */
uint32_t watchdogPostNotify(uint32_t eventType, uint32_t clientArg)
{
    /* Currently no action is needed */
    return Power_NOTIFYDONE;
}
