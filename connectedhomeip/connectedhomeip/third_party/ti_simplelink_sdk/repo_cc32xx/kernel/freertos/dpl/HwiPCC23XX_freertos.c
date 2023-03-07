/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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
/*
 *  ======== HwiPCC23XX_freertos ========
 */

#include <stdbool.h>
#include <stdlib.h>

#include <ti/drivers/dpl/HwiP.h>
#include <FreeRTOS.h>
#include <task.h>
#include <portmacro.h>

/* Driverlib includes */
#include <ti/devices/DeviceFamily.h>

#include DeviceFamily_constructPath(cmsis/cc23x0.h)
#include DeviceFamily_constructPath(cmsis/core/core_cm0plus.h)

#include DeviceFamily_constructPath(driverlib/interrupt.h)

#include DeviceFamily_constructPath(inc/hw_ints.h)

/* Masks off all bits but the VECTACTIVE bits in the ICSR register. */
#define portVECTACTIVE_MASK  (0xFFUL)

/* interrupt reserved for SwiP */
int HwiP_swiPIntNum = INT_CPUIRQ4;

typedef struct _HwiP_Obj {
    uint32_t intNum;
    HwiP_Fxn fxn;
    uintptr_t arg;
} HwiP_Obj;

static HwiP_Obj* HwiP_dispatchTable[NUM_INTERRUPTS] = { 0 };

void HwiP_dispatch(void);

/*
 *  ======== HwiP_clearInterrupt ========
 */
void HwiP_clearInterrupt(int interruptNum)
{
    IntPendClear((uint32_t)interruptNum);
}

/*
 *  ======== HwiP_construct ========
 */
HwiP_Handle HwiP_construct(HwiP_Struct *handle,
                           int interruptNum,
                           HwiP_Fxn hwiFxn,
                           HwiP_Params *params)
{
    HwiP_Params defaultParams;
    HwiP_Obj *obj = (HwiP_Obj *)handle;

    if (handle != NULL) {
        if (params == NULL) {
            params = &defaultParams;
            HwiP_Params_init(&defaultParams);
        }

        if ((params->priority & 0xFF) == 0xFF) {
            /* SwiP_freertos.c uses INT_PRI_LEVEL7 as its scheduler */
            params->priority = INT_PRI_LEVEL6;
        }

        if (interruptNum != HwiP_swiPIntNum &&
            params->priority == INT_PRI_LEVEL7) {
            handle = NULL;
        }
        else {
            HwiP_dispatchTable[interruptNum] = obj;
            obj->fxn = hwiFxn;
            obj->arg = params->arg;
            obj->intNum = (uint32_t)interruptNum;

            IntRegister((uint32_t)interruptNum, HwiP_dispatch);
            IntPrioritySet((uint32_t)interruptNum, (uint8_t)params->priority);

            if (params->enableInt) {
                IntEnable((uint32_t)interruptNum);
            }
        }
    }

    return ((HwiP_Handle)handle);
}

/*
 *  ======== HwiP_create ========
 */
HwiP_Handle HwiP_create(int interruptNum, HwiP_Fxn hwiFxn, HwiP_Params *params)
{
    HwiP_Handle handle;
    HwiP_Handle retHandle;

    handle = (HwiP_Handle)malloc(sizeof(HwiP_Obj));

    /*
     * Even though HwiP_construct will check handle for NULL and not do
     * anything, we should check it here so that we can know afterwards
     * that construct failed with non-NULL pointer and that we need to
     * free the handle.
     */
    if (handle != NULL) {
        retHandle = HwiP_construct((HwiP_Struct *)handle,
                                   interruptNum,
                                   hwiFxn,
                                   params);
        if (retHandle == NULL) {
            free(handle);
            handle = NULL;
        }
    }

    return (handle);
}

/*
 *  ======== HwiP_delete ========
 */
void HwiP_delete(HwiP_Handle handle)
{
    HwiP_destruct((HwiP_Struct *)handle);

    free(handle);
}

/*
 *  ======== HwiP_destruct ========
 */
void HwiP_destruct(HwiP_Struct *handle)
{
    HwiP_Obj *obj = (HwiP_Obj *)handle;

    IntDisable(obj->intNum);
    IntUnregister(obj->intNum);
}

/*
 *  ======== HwiP_disable ========
 */
uintptr_t HwiP_disable(void)
{
    uintptr_t key;

    /*
     *  If we're not in ISR context, use the FreeRTOS macro, since
     *  it handles nesting.
     */
    if ((portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK) == 0) {
        /* Cannot be called from an ISR! */
        portENTER_CRITICAL();
        key = 0;
    }
    else {
        key = __get_PRIMASK();
        __set_PRIMASK(1);
    }

    return (key);
}

/*
 *  ======== HwiP_disableInterrupt ========
 */
void HwiP_disableInterrupt(int interruptNum)
{
    IntDisable((uint32_t)interruptNum);
}

/*
 *  ======== HwiP_dispatch ========
 */
void HwiP_dispatch(void)
{
    uint32_t intNum = (SCB->ICSR & 0x000000ff);
    HwiP_Obj* obj = HwiP_dispatchTable[intNum];
    if (obj) {
        (obj->fxn)(obj->arg);
        taskYIELD();
    }
}

/*
 *  ======== HwiP_enableInterrupt ========
 */
void HwiP_enableInterrupt(int interruptNum)
{
    IntEnable(interruptNum);
}

/*
 *  ======== HwiP_interruptsEnabled ========
 */
bool HwiP_interruptsEnabled(void)
{
    return (__get_PRIMASK() == 0L);
}

/*

 *  ======== HwiP_inISR ========
 */
bool HwiP_inISR(void)
{
    bool stat;

    if ((portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK) == 0) {
        /* Not currently in an ISR */
        stat = false;
    }
    else {
        stat = true;
    }

    return (stat);
}

/*
 *  ======== HwiP_inSwi ========
 */
bool HwiP_inSwi(void)
{
    uint32_t intNum  = portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK;
    if (intNum == HwiP_swiPIntNum) {
        /* Currently in a Swi */
        return (true);
    }

    return (false);
}

/*
 *  ======== HwiP_Params_init ========
 */
void HwiP_Params_init(HwiP_Params *params)
{
    if (params != NULL) {
        params->arg = 0;
        params->priority = (~0);
        params->enableInt = true;
    }
}

/*
 *  ======== HwiP_plug ========
 */
void HwiP_plug(int interruptNum, void *fxn)
{
    IntRegister((uint32_t)interruptNum, (void (*)(void))fxn);
}

/*
 *  ======== HwiP_post ========
 */
void HwiP_post(int interruptNum)
{
    IntPendSet(interruptNum);
}

/*
 *  ======== HwiP_restore ========
 */
void HwiP_restore(uintptr_t key)
{
    if ((portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK) == 0) {
        /* Cannot be called from an ISR! */
        portEXIT_CRITICAL();
    }
    else {
        __set_PRIMASK(key);
    }
}

/*
 *  ======== HwiP_setFunc ========
 */
void HwiP_setFunc(HwiP_Handle hwiP, HwiP_Fxn fxn, uintptr_t arg)
{
    HwiP_Obj *obj = (HwiP_Obj *)hwiP;

    uintptr_t key = HwiP_disable();

    obj->fxn = fxn;
    obj->arg = arg;

    HwiP_restore(key);
}

/*
 * ======== HwiP_setPri ========
 */
void HwiP_setPriority(int interruptNum, uint32_t priority)
{
    IntPrioritySet((uint32_t)interruptNum, (uint8_t)priority);
}

#if (configSUPPORT_STATIC_ALLOCATION == 1)
/*
 *  ======== HwiP_staticObjectSize ========
 */
size_t HwiP_staticObjectSize(void)
{
    return (sizeof(HwiP_Obj));
}
#endif
