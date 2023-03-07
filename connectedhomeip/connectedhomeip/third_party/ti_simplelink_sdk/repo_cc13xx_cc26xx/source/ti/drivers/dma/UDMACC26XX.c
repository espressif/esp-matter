/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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

#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>

#include <ti/drivers/dma/UDMACC26XX.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(driverlib/udma.h)
#include DeviceFamily_constructPath(driverlib/prcm.h)

/* Externs */
extern const UDMACC26XX_Config UDMACC26XX_config[];

static void UDMACC26XX_initHw(UDMACC26XX_Handle handle);

/*
 *  ======== UDMACC26XX_errorDMAHwi ========
 */

/*!
 *  @brief  Handler called if the DMA gets an error during transfer.
 *
 *  This function will clear the error.
 *
 *  @param  arg  A user defined argument.
 *
 *  @return none
 */
void UDMACC26XX_hwiIntFxn(uintptr_t arg)
{
    UDMACC26XX_HWAttrs const *hwAttrs;

    /* Get the pointer to the hwAttrs */
    hwAttrs = ((UDMACC26XX_Handle)arg)->hwAttrs;

    /* Log the error and clear it */
    DebugP_log1("DMA error code: %d\n", uDMAErrorStatusGet(hwAttrs->baseAddr));
    uDMAErrorStatusClear(hwAttrs->baseAddr);
}

/*
 *  ======== UDMACC26XX_open ========
 *
 */
UDMACC26XX_Handle UDMACC26XX_open()
{
    HwiP_Params               hwiParams;
    unsigned int             key;
    UDMACC26XX_Object        *object;
    UDMACC26XX_HWAttrs const *hwAttrs;
    UDMACC26XX_Handle        handle;

    /* Get the pointer to the object and the hwAttrs */
    handle = (UDMACC26XX_Handle)&(UDMACC26XX_config[0]);
    hwAttrs = handle->hwAttrs;
    object = handle->object;

    /* Power up and enable clocks for uDMA. */
    Power_setDependency(hwAttrs->powerMngrId);

    /* Disable preemption while checking if the UDMACC26XX is open. */
    key = HwiP_disable();

    if(!object->isOpen){
        HwiP_Params_init(&hwiParams);
        hwiParams.arg = (uintptr_t) handle;
        hwiParams.priority = hwAttrs->intPriority;
        HwiP_construct(&(object->hwi), (int) hwAttrs->intNum, UDMACC26XX_hwiIntFxn, &hwiParams);

        /* make sure to mark the uDMA as opened */
        object->isOpen = true;

        /* initialize the UDMACC26XX hardware */
        UDMACC26XX_initHw(handle);
    }

    HwiP_restore(key);
    return (handle);
}

/*
 *  ======== UDMACC26XX_close ========
 *
 */
void UDMACC26XX_close(UDMACC26XX_Handle handle)
{
    unsigned int             key;
    UDMACC26XX_Object        *object;
    UDMACC26XX_HWAttrs const *hwAttrs;

    /* Get the pointer to the object and hwAttrs */
    hwAttrs = handle->hwAttrs;
    object = handle->object;

    /* Disable preemption while checking if the UDMACC26XX is open. */
    key = HwiP_disable();

    /* Only consider to take anything down if uDMA is initialized. */
    if (object->isOpen) {
        if (Power_getDependencyCount(hwAttrs->powerMngrId) == 1) {
            uDMADisable(hwAttrs->baseAddr);
            HwiP_destruct(&(object->hwi));
            object->isOpen = false;
        }
        Power_releaseDependency(hwAttrs->powerMngrId);
    }

    HwiP_restore(key);
}

/*
 *  ======== UDMACC26XX_initHw ========
 *  This functions initializes the UDMACC26XX hardware module.
 *
 */
static void UDMACC26XX_initHw(UDMACC26XX_Handle handle)
{
    UDMACC26XX_HWAttrs const *hwAttrs;

    /* Get the pointer to the hwAttrs */
    hwAttrs = handle->hwAttrs;

    /* Disable all channels */
    UDMACC26XX_channelDisable(handle, 0xFFFFFFFF);

    /* Set the base for the channel control table. */
    uDMAControlBaseSet(hwAttrs->baseAddr, (void *) UDMACC26XX_CONFIG_BASE);

    /* Enable uDMA. */
    uDMAEnable(hwAttrs->baseAddr);
}
