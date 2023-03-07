/*
 * Copyright (c) 2015-2018 Texas Instruments Incorporated
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
 *  ======== Hwi.c ========
 */

#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Assert.h>

#include <ti/sysbios/interfaces/IHwi.h>

//#include <ti/sysbios/hal/Core.h>
#include <ti/sysbios/family/arm/ducati/Core.h>
#define ti_sysbios_knl_Task__internalaccess
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>

#include <ti/sysbios/BIOS.h>

#include <string.h>

#include "package/internal/Hwi.xdc.h"

extern Char *ti_sysbios_family_xxx_Hwi_switchToIsrStack();
extern Void ti_sysbios_family_xxx_Hwi_switchToTaskStack(Char *oldTaskSP);
extern Void ti_sysbios_family_xxx_Hwi_switchAndRunFunc(Void (*func)());

extern UInt32 ti_sysbios_family_arm_m3_Hwi_dispatchTable[];

#define Hwi_switchToIsrStack ti_sysbios_family_xxx_Hwi_switchToIsrStack
#define Hwi_switchToTaskStack ti_sysbios_family_xxx_Hwi_switchToTaskStack
#define Hwi_switchAndRunFunc ti_sysbios_family_xxx_Hwi_switchAndRunFunc

#ifdef ti_sysbios_family_arm_m3_Hwi_dispatcherTaskSupport__D
#define TASK_DISABLE Task_disable
#define TASK_RESTORE Task_restoreHwi
#else
#define TASK_DISABLE Hwi_taskDisable
#define TASK_RESTORE Hwi_taskRestoreHwi
#endif

#ifdef ti_sysbios_family_arm_m3_Hwi_dispatcherSwiSupport__D
#define SWI_DISABLE Swi_disable
#define SWI_RESTORE Swi_restoreHwi
#else
#define SWI_DISABLE Hwi_swiDisable
#define SWI_RESTORE Hwi_swiRestoreHwi
#endif

volatile Hwi_NVIC ti_sysbios_family_arm_m3_Hwi_vnvic;
Bool ti_sysbios_family_arm_m3_Hwi_vnvicInitialized = FALSE;

/*
 *  ======== Hwi_Module_startup ========
 */
Int Hwi_Module_startup(Int phase)
{
    int i;

    /*
     *  perform the BIOS specific interrupt disable operation so
     *  main is entered with interrupts disabled.
     */
    Hwi_disable();

#ifndef ti_sysbios_hal_Hwi_DISABLE_ALL_HOOKS
    for (i = 0; i < Hwi_hooks.length; i++) {
        if (Hwi_hooks.elem[i].registerFxn != NULL) {
            Hwi_hooks.elem[i].registerFxn(i);
        }
    }
#endif

    /*
     * Initialize the pointer to the isrStack.
     *
     * The dispatcher's SP must be aligned on a long word boundary
     */
    Hwi_module->isrStack =
        (Char *)(((UInt32)(Hwi_module->isrStackBase) & 0xfffffff8) +
                (UInt32)Hwi_module->isrStackSize - 2 * sizeof(Int));

    Hwi_module->taskSP = (Char *)-1;    /* signal that we're executing */
                                        /* on the ISR stack */

    for (i = 0; i < Hwi_Object_count(); i++) {
        Hwi_postInit(Hwi_Object_get(NULL, i), NULL);
    }

    /* force the NVIC on both cores to get updated */
    Hwi_module->vnvicFlushRequired = 3;

    /* flush to core 0's NVIC now */
    Hwi_flushVnvic();

    return Startup_DONE;
}

/*
 *  ======== Hwi_Instance_init ========
 */
Int Hwi_Instance_init(Hwi_Object *hwi, Int intNum,
                      Hwi_FuncPtr fxn, const Hwi_Params *params,
                      Error_Block *eb)
{
    Int status;

    hwi->intNum = intNum;

    /* check vector table entry for already in use vector */
    if (*((UInt32 *)Hwi_module->vectorTableBase + intNum) !=
         (UInt32)Hwi_nullIsrFunc) {
        Error_raise(eb, Hwi_E_alreadyDefined, intNum, 0);
        return (1);
    }

#ifndef ti_sysbios_hal_Hwi_DISABLE_ALL_HOOKS
    if (Hwi_hooks.length > 0) {
        /* Allocate environment space for each hook instance. */
        hwi->hookEnv = Memory_calloc(Hwi_Object_heap(),
                Hwi_hooks.length * sizeof(Ptr), 0, eb);

        if (hwi->hookEnv == NULL) {
            return (2);
        }
    }
#endif

    Hwi_disableInterrupt(intNum);

    hwi->fxn = fxn;
    hwi->arg = params->arg;

    /*
     * the -1 sentinel priority is the default passed by hal Hwi_create().
     * Translate it to 255, which is our default priority.
     */
    if (params->priority == -1) {
        hwi->priority = 255;
    }
    else {
        hwi->priority = params->priority;
    }

    /*
     * hwi->irp field is overloaded during initialization
     * to reduce Hwi Object footprint
     *
     * for postInit(), encode irp with enableInt
     * and useDispatcher info.
     */
    hwi->irp = 0;

    /* encode 'enableInt' in bit 0 */
    if (params->enableInt) {
        hwi->irp = 0x1;
    }

    /* encode 'useDispatcher' in bit 1 */
    if (params->useDispatcher) {
        hwi->irp |= 0x2;
    }

    status = Hwi_postInit(hwi, eb);

    if (status) {
        return (2 + status);
    }

    return (0);
}

/*
 *  ======== Hwi_postInit ========
 *  Function to be called during module startup to complete the
 *  initialization of any statically created or constructed Hwi.
 *  returns (0) and clean 'eb' on success
 *  returns 'eb' *and* 'n' for number of successful createFxn() calls iff
 *      one of the createFxn() calls fails
 */
Int Hwi_postInit (Hwi_Object *hwi, Error_Block *eb)
{
    UInt intNum;

#ifndef ti_sysbios_hal_Hwi_DISABLE_ALL_HOOKS
    Int i;
    Error_Block localEB;
    Error_Block *leb;

    if (eb != Error_IGNORE) {
        leb = eb;
    }
    else {
        Error_init(&localEB);
        leb = &localEB;
    }

    for (i = 0; i < Hwi_hooks.length; i++) {
        hwi->hookEnv[i] = (Ptr)0;
        if (Hwi_hooks.elem[i].createFxn != NULL) {
            Hwi_hooks.elem[i].createFxn((IHwi_Handle)hwi, leb);

            if (Error_check(leb)) {
                return (i + 1);
            }
        }
    }
#endif

    /*
     * Bypass dispatcher for zero-latency interrupts
     * and interrupts with useDispatcher == FALSE
     */

    /* save intNum for use by Hwi_enableInterrupt */
    intNum = hwi->intNum;

    if (((hwi->irp & 0x2) == 0) ||
        (hwi->priority < Hwi_disablePriority)) {
        Hwi_plug(hwi->intNum, (Void *)(UArg)hwi->fxn);
        /*
         * encode useDispatcher == FALSE as a negative intNum
         * This is done to inform ROV that this is a non-dispatched interrupt
         * without adding a new field to the Hwi object.
         */
        hwi->intNum = 0 - hwi->intNum;
    }
    else {
        if (Hwi_numSparseInterrupts) {
            Int i;
            UInt32 *sparseInterruptTableEntry = Hwi_module->dispatchTable;
            Char *vectorPtr;
            Bool found = FALSE;

            /* find an unused sparseTableEntry */
            for (i = 0; i < (Int)Hwi_numSparseInterrupts; i++) {
                if (sparseInterruptTableEntry[2] == 0) {
                    found = TRUE;
                    break;
                }
                else {
                    sparseInterruptTableEntry += 3;
                }
            }

            if (found) {
                /* point it to the Hwi object */
                sparseInterruptTableEntry[2] = (UInt32)hwi;

                /* plug the vector table with the sparseTable entry */
                vectorPtr = (Char *)sparseInterruptTableEntry;
                vectorPtr += 1;    /* make it a thumb func vector */
                Hwi_plug(hwi->intNum, (Void *)vectorPtr);
            }
            else {
                Error_raise(eb, Hwi_E_hwiLimitExceeded, 0, 0);
#ifndef ti_sysbios_hal_Hwi_DISABLE_ALL_HOOKS
                return (Hwi_hooks.length); /* unwind all Hwi_hooks */
#else
                return (1);
#endif
            }
        }
        else {
            Hwi_Object **dispatchTable = (Hwi_Object **)Hwi_module->dispatchTable;
            dispatchTable[intNum] = hwi;
            Hwi_plug(intNum, (Void *)(UArg)Hwi_dispatch);
        }
    }

    Hwi_setPriority(intNum, hwi->priority);

    if ((hwi->irp & 0x1) != 0) {
        Hwi_enableInterrupt(intNum);
    }

    hwi->irp = 0;

    return (0);
}

/*
 *  ======== Hwi_Instance_finalize ========
 */
Void Hwi_Instance_finalize(Hwi_Object *hwi, Int status)
{
    UInt intNum;

    if (status == 1) {  /* vector in use */
        return;
    }

#ifndef ti_sysbios_hal_Hwi_DISABLE_ALL_HOOKS
    if (Hwi_hooks.length > 0) {
        Int i, cnt;

        /* return if failed to allocate Hook Env */
        if (status == 2) {
            return;
        }

        if (status == 0) {
            cnt = Hwi_hooks.length;
        }
        else {
            cnt = status - 3;
        }

        for (i = 0; i < cnt; i++) {
            if (Hwi_hooks.elem[i].deleteFxn != NULL) {
                Hwi_hooks.elem[i].deleteFxn((IHwi_Handle)hwi);
            }
        }

        Memory_free(Hwi_Object_heap(), hwi->hookEnv,
                Hwi_hooks.length * sizeof(Ptr));
    }
#endif

    /* compensate for encoded intNum */
    if (hwi->intNum < 0) {
        intNum = 0 - hwi->intNum;
    }
    else {
        intNum = hwi->intNum;
    }

    Hwi_disableInterrupt(intNum);
    Hwi_plug(intNum, (Void *)(UArg)Hwi_nullIsrFunc);

    if (Hwi_numSparseInterrupts) {
        Int i;
        UInt32 *sparseInterruptTableEntry = Hwi_module->dispatchTable;

        for (i = 0; i < (Int)Hwi_numSparseInterrupts; i++) {
            if (sparseInterruptTableEntry[2] == (UInt32)hwi) {
                sparseInterruptTableEntry[2] = 0;
                break;
            }
            else {
                sparseInterruptTableEntry += 3;
            }
        }
    }
    else {
        Hwi_Object **dispatchTable = (Hwi_Object **)Hwi_module->dispatchTable;
        dispatchTable[intNum] = 0;
    }
}

/*
 *  ======== Hwi_initNVIC ========
 *  run as a Startup.firstFxn
 */
extern const UInt32 ti_sysbios_family_arm_m3_Hwi_resetVectors[];
#define Hwi_resetVectors ti_sysbios_family_arm_m3_Hwi_resetVectors

Void Hwi_initNVIC()
{
    UInt i;
    UInt intNum;
    UInt32 *ramVectors;

    /* Initialize VNVIC data structure */
    if (!ti_sysbios_family_arm_m3_Hwi_vnvicInitialized) {
        memset((void *)&Hwi_vnvic, 0, sizeof(Hwi_vnvic));
        ti_sysbios_family_arm_m3_Hwi_vnvicInitialized = TRUE;
    }

    /* configure Vector Table Offset Register */
    Hwi_nvic.VTOR = (UInt32)Hwi_module->vectorTableBase;

    /* copy ROM vector table contents to RAM vector table */
    if (Hwi_nvic.VTOR != (UInt32)Hwi_resetVectors) {
        ramVectors = Hwi_module->vectorTableBase;

        for (intNum = 0; intNum < Hwi_NUM_INTERRUPTS; intNum++) {
            if (intNum < 15) {
                ramVectors[intNum] = Hwi_resetVectors[intNum];
            }
            else {
                ramVectors[intNum] = (UInt32)Hwi_nullIsrFunc;
            }
        }
    }

    for (i = 0; i < 8; i++) {
        /* Disable all interrupts to start with */
        Hwi_nvic.ICER[i] = 0xffffffff;
        Hwi_vnvic.ICER[i] = 0xffffffff;

        /* Clear any currently pending interrupts */
        Hwi_nvic.ICPR[i] = 0xffffffff;
        Hwi_vnvic.ICPR[i] = 0xffffffff;

        /* disable the SysTick timer interrupt */
        Hwi_nvic.STCSR &= ~2;
        Hwi_vnvic.STCSR = 0;
    }

    /* Set the configured PRIGROUP value */
    Hwi_nvic.AIRCR = (Hwi_nvic.AIRCR & 0xffff00ff) + (Hwi_priGroup << 8) + 0x05fa0000;

    /* set pendSV interrupt priority to Hwi_disablePriority */
    Hwi_vnvic.SHPR[10] = Hwi_nvic.SHPR[10] = Hwi_disablePriority;

    /* set CCR per user's preference */
    Hwi_nvic.CCR = Hwi_ccr;

#if (defined(__TI_VFP_SUPPORT__) || \
    (defined(__VFP_FP__) && !defined(__SOFTFP__)) || \
    defined(__ARMVFP__))
    /* disable lazy stacking mode fp indications in control register */
    Hwi_nvic.FPCCR &= ~0xc0000000; /* clear ASPEN and LSPEN bits */
#endif
}

/*
 *  ======== Hwi_startup ========
 */
Void Hwi_startup()
{
    Hwi_enable();
}

/*
 *  ======== Hwi_disableFxn ========
 */
UInt Hwi_disableFxn()
{
    /* acquire Inter-core lock */
    return (Core_lock());
}

/*
 *  ======== Hwi_restoreFxn ========
 */
Void Hwi_restoreFxn(UInt key)
{
    if (key == 0) {
        /* call Core_unlock() with interrupts disabled */
        Core_hwiDisable();

        /* release Inter-core lock */
        Core_unlock();
    }

    Core_hwiRestore(key);
}

/*
 *  ======== Hwi_enableFxn ========
 */
UInt Hwi_enableFxn()
{
    /* call Core_unlock() with interrupts disabled */
    Core_hwiDisable();

    /* release Inter-core lock */
    Core_unlock();

    return (Core_hwiEnable());
}

/*
 *  ======== Hwi_disableInterrupt ========
 */
UInt Hwi_disableInterrupt(UInt intNum)
{
    UInt oldEnableState, index, mask, hwiKey;

    hwiKey = Hwi_disable();

    if (intNum >= 16) {

        index = (intNum-16) >> 5;

        mask = 1 << ((intNum-16) & 0x1f);

        oldEnableState = Hwi_vnvic.ISER[index] & mask;

        Hwi_vnvic.ICER[index] = mask;
        Hwi_vnvic.ISER[index] &= ~mask;
    }
    else if (intNum == 15) {
        oldEnableState = Hwi_vnvic.STCSR & 0x00000002;
        Hwi_vnvic.STCSR &= ~0x00000002;  /* disable SysTick Int */
    }
    else {
        oldEnableState = 0;
    }

    Hwi_updateNvic(intNum);

    Hwi_restore(hwiKey);

    return (oldEnableState);
}

/*
 *  ======== Hwi_enableInterrupt ========
 */
UInt Hwi_enableInterrupt(UInt intNum)
{
    UInt oldEnableState, index, mask, hwiKey;

    hwiKey = Hwi_disable();

    if (intNum >= 16) {

        index = (intNum-16) >> 5;

        mask = 1 << ((intNum-16) & 0x1f);

        oldEnableState = Hwi_vnvic.ISER[index] & mask;

        Hwi_vnvic.ISER[index] |= mask;
    }
    else if (intNum == 15) {
        oldEnableState = Hwi_vnvic.STCSR & 0x00000002;
        Hwi_vnvic.STCSR |= 0x00000002;   /* enable SysTick Int */
    }
    else {
        oldEnableState = 0;
    }

    Hwi_updateNvic(intNum);

    Hwi_restore(hwiKey);

    return oldEnableState;
}

/*
 *  ======== Hwi_restoreInterrupt ========
 */
Void Hwi_restoreInterrupt(UInt intNum, UInt key)
{
    UInt index, mask, hwiKey;

    hwiKey = Hwi_disable();

    if (intNum >= 16) {

        index = (intNum-16) >> 5;

        mask = 1 << ((intNum-16) & 0x1f);

        if (key) {
            Hwi_vnvic.ISER[index] |= mask;
        }
        else {
            Hwi_vnvic.ICER[index] = mask;
            Hwi_vnvic.ISER[index] &= ~mask;
        }
    }
    else if (intNum == 15) {
        if (key) {
            Hwi_vnvic.STCSR |= 0x00000002;       /* enable SysTick Int */
        }
        else {
            Hwi_vnvic.STCSR &= ~0x00000002;      /* disable SysTick Int */
        }
    }

    Hwi_updateNvic(intNum);

    Hwi_restore(hwiKey);
}

/*
 *  ======== Hwi_clearInterrupt ========
 */
Void Hwi_clearInterrupt(UInt intNum)
{
    UInt index, mask, hwiKey;

    hwiKey = Hwi_disable();

    if (intNum >= 16) {

        index = (intNum-16) >> 5;

        mask = 1 << ((intNum-16) & 0x1f);

        Hwi_vnvic.ICPR[index] = mask;
    }
    else {

    }

    Hwi_updateNvic(intNum);

    Hwi_restore(hwiKey);
}

/*
 *  ======== Hwi_getHandle ========
 */
Hwi_Handle Hwi_getHandle(UInt intNum)
{
    UInt32 *func;

    func = (UInt32 *)Hwi_module->vectorTableBase + intNum;

    if (Hwi_numSparseInterrupts) {
        if (*func != (UInt32)Hwi_nullIsrFunc) {
            Char *vectorPtr = (Char *)func;
            vectorPtr -= 1;
            UInt32 *sparseTableEntry = (UInt32 *)vectorPtr;
            /* Hwi handle is the 3rd word in the sparseTableEntry array */
            return ((Hwi_Handle)sparseTableEntry[2]);
        }
        else {
            return (NULL);
        }
    }
    else {
        Hwi_Object **dispatchTable = (Hwi_Object **)Hwi_module->dispatchTable;
        return (dispatchTable[intNum]);
    }
}

/*
 *  ======== Hwi_plug ========
 */
Void Hwi_plug(UInt intNum, Void *fxn)
{
    UInt32 *func;

    func = (UInt32 *)Hwi_module->vectorTableBase + intNum;

    /* guard against writing to static const vector table in flash */
    if (*func !=  (UInt32)fxn) {
        *func = (UInt32)fxn;
    }
}

/*
 *  ======== switchFromBootStack ========
 *  Indicate that we are leaving the boot stack and
 *  are about to switch to a task stack.
 *
 *  Must be (and is) called with interrupts disabled from
 *  Task_startup().
 */
Void Hwi_switchFromBootStack()
{
    /* split thread and handler stacks */
    Hwi_initStacks(Hwi_module->isrStack);

    Hwi_module->taskSP = 0;
}

/*
 *  ======== Hwi_getStackInfo ========
 *  Used to get Hwi stack usage info.
 */
Bool Hwi_getStackInfo(Hwi_StackInfo *stkInfo, Bool computeStackDepth)
{
    Char *isrSP;
    Bool stackOverflow;

    /* Copy the stack base address and size */
    stkInfo->hwiStackSize = (SizeT)Hwi_module->isrStackSize;
    stkInfo->hwiStackBase = Hwi_module->isrStackBase;

    isrSP = stkInfo->hwiStackBase;

    /* Check for stack overflow */
    stackOverflow = (*isrSP != (Char)0xbe ? TRUE : FALSE);

    if (computeStackDepth && !stackOverflow) {
        /* Compute stack depth */
        do {
        } while(*isrSP++ == (Char)0xbe);
        stkInfo->hwiStackPeak = stkInfo->hwiStackSize - (SizeT)(--isrSP - (Char *)stkInfo->hwiStackBase);
    }
    else {
        stkInfo->hwiStackPeak = 0;
    }

    return stackOverflow;
}

/*
 *  ======== Hwi_getCoreStackInfo ========
 *  Used to get Hwi stack usage info on the specified core
 */
Bool Hwi_getCoreStackInfo(Hwi_StackInfo *stkInfo, Bool computeStackDepth,
    UInt coreId)
{
    Char *isrSP;
    Bool stackOverflow;

    /* Copy the stack base address and size */
    if (coreId == 0) {
        stkInfo->hwiStackSize = (SizeT)Hwi_module->isrStackSize;
        stkInfo->hwiStackBase = Hwi_module->isrStackBase;
    }
    else {
        stkInfo->hwiStackSize = Core_core1HwiStackSize;
        stkInfo->hwiStackBase = Core_getCore1HwiStackBase();
    }

    isrSP = stkInfo->hwiStackBase;

    /* Check for stack overflow */
    stackOverflow = (*isrSP != (Char)0xbe ? TRUE : FALSE);

    if (computeStackDepth && !stackOverflow) {
        /* Compute stack depth */
        do {
        } while(*isrSP++ == (Char)0xbe);
        stkInfo->hwiStackPeak = stkInfo->hwiStackSize - (SizeT)(--isrSP - (Char *)stkInfo->hwiStackBase);
    }
    else {
        stkInfo->hwiStackPeak = 0;
    }

    return stackOverflow;
}

/*
 *  ======== Hwi_setPriority ========
 *  Set an interrupt's priority.
 *
 *  @param(intNum)      ID of interrupt
 *  @param(priority)    priority
 */
Void Hwi_setPriority(UInt intNum, UInt priority)
{
    UInt hwiKey;

    hwiKey = Hwi_disable();

    /* User interrupt (id >= 16) priorities are set in the IPR registers */
    if (intNum >= 16) {
        Hwi_vnvic.IPR[intNum-16] = priority;
        Hwi_nvic.IPR[intNum-16] = priority;     /* update local copy */
    }
    /* System interrupt (id >= 4) priorities are set in the SHPR registers */
    else if (intNum >= 4) {
        Hwi_vnvic.SHPR[intNum-4] = priority;
        Hwi_nvic.SHPR[intNum-4] = priority;     /* update local copy */
    }
    /* System interrupts (id < 4) priorities  are fixed in hardware */

    Hwi_updateNvic(intNum);

    Hwi_restore(hwiKey);
}

/*
 *  ======== Hwi_reconfig ========
 *  Reconfigure a dispatched interrupt.
 */
Void Hwi_reconfig(Hwi_Object *hwi, Hwi_FuncPtr fxn, const Hwi_Params *params)
{
    UInt intNum;

    /* compensate for encoded intNum */
    if (hwi->intNum < 0) {
        intNum = 0 - hwi->intNum;
    }
    else {
        intNum = hwi->intNum;
    }

    Hwi_disableInterrupt(intNum);

    hwi->fxn = fxn;
    hwi->arg = params->arg;

    /*
     * the -1 sentinel priority is the default passed by hal Hwi_create().
     * Translate it to 255, which is our default priority.
     */
    if (params->priority == -1) {
        hwi->priority = 255;
    }
    else {
        hwi->priority = params->priority;
    }

    Hwi_setPriority(intNum, hwi->priority);

    if (params->enableInt) {
        Hwi_enableInterrupt(intNum);
    }
}

/*
 *  ======== Hwi_getfunc ========
 */
Hwi_FuncPtr Hwi_getFunc(Hwi_Object *hwi, UArg *arg)
{
    *arg = hwi->arg;

    return (hwi->fxn);
}

/*
 *  ======== Hwi_setfunc ========
 */
Void Hwi_setFunc(Hwi_Object *hwi, Hwi_FuncPtr fxn, UArg arg)
{
    hwi->fxn = fxn;
    hwi->arg = arg;
}

/*
 *  ======== Hwi_getIrp ========
 */
Hwi_Irp Hwi_getIrp(Hwi_Object *hwi)
{
    return (hwi->irp);
}

/*
 *  ======== Hwi_getHookContext ========
 */
Ptr Hwi_getHookContext(Hwi_Object *hwi, Int id)
{
    return (hwi->hookEnv[id]);
}

/*
 *  ======== Hwi_setHookContext ========
 */
Void Hwi_setHookContext(Hwi_Object *hwi, Int id, Ptr hookContext)
{
    hwi->hookEnv[id] = hookContext;
}

/*
 *  ======== Hwi_post ========
 */
Void Hwi_post(UInt intNum)
{
    if (intNum >= 16) {
        Hwi_nvic.STI = intNum - 16;
    }
}

/*
 *  ======== Hwi_excSetBuffers ========
 */
Void Hwi_excSetBuffers(Ptr excContextBuffer, Ptr excStackBuffer)
{
    UInt coreId = Core_getId();
    Hwi_module->excContext[coreId] = excContextBuffer;
    Hwi_module->excStack[coreId] = excStackBuffer;
}

/*
 *  ======== Hwi_excHandler ========
 */
Void Hwi_excHandler(UInt *excStack, UInt lr)
{
    Hwi_excHandlerFunc(excStack, lr);
}

/*
 *  ======== Hwi_excHandlerMin ========
 */
Void Hwi_excHandlerMin(UInt *excStack, UInt lr)
{
    Hwi_ExcContext excContext;
    UInt excNum;
    UInt coreId = Core_getId();

    Hwi_module->excActive[coreId] = TRUE;

    if (Hwi_module->excContext[coreId] == NULL) {
        Hwi_module->excContext[coreId] = &excContext;
    }

    Hwi_excFillContext(excStack);

    /* Force MAIN threadtype so hooks can safely call System_printf */

    BIOS_setThreadType(BIOS_ThreadType_Main);

    /* Call user's exception hook */
    if (Hwi_excHookFuncs[coreId] != NULL) {
        Hwi_excHookFuncs[coreId](Hwi_module->excContext[coreId]);
    }

    excNum = Hwi_nvic.ICSR & 0xff;

    /* distinguish between an interrupt and an exception */
    if ( excNum < 15 ) {
        Error_raise(0, Hwi_E_exception, excNum, excStack[14]);
    }
    else {
        Error_raise(0, Hwi_E_noIsr, excNum, excStack[14]);
    }
}

/*
 *  ======== Hwi_excHandlerMax ========
 */
Void Hwi_excHandlerMax(UInt *excStack, UInt lr)
{
    Hwi_ExcContext excContext;
    UInt excNum;
    UInt coreId = Core_getId();

    Hwi_module->excActive[coreId] = TRUE;

    if (Hwi_module->excContext[coreId] == NULL) {
        Hwi_module->excContext[coreId] = &excContext;
    }

    Hwi_excFillContext(excStack);

    /* Force MAIN threadtype so we can safely call System_printf */

    BIOS_setThreadType(BIOS_ThreadType_Main);

    Hwi_disable();

    excNum = Hwi_nvic.ICSR & 0xff;

    switch (excNum) {
        case 2:
            Hwi_excNmi(excStack);       /* NMI */
            break;
        case 3:
            Hwi_excHardFault(excStack); /* Hard Fault */
            break;
        case 4:
            Hwi_excMemFault(excStack);  /* Mem Fault */
            break;
        case 5:
            Hwi_excBusFault(excStack);  /* Bus Fault */
            break;
        case 6:
            Hwi_excUsageFault(excStack);/* Usage Fault */
            break;
        case 11:
            Hwi_excSvCall(excStack);    /* SVCall */
            break;
        case 12:
            Hwi_excDebugMon(excStack);  /* Debug Mon */
            break;
        case 7:
        case 8:
        case 9:
        case 10:
        case 13:
            Hwi_excReserved(excStack, excNum); /* reserved */
            break;
        default:
            Hwi_excNoIsr(excStack, excNum);     /* no ISR */
            break;
    }

    Hwi_excDumpRegs(lr);

    /* Call user's exception hook */
    if (Hwi_excHookFuncs[coreId] != NULL) {
        Hwi_excHookFuncs[coreId](Hwi_module->excContext[coreId]);
    }

    System_printf("Terminating execution...\n");

    Hwi_enable();

    BIOS_exit(0);
}

/*
 *  ======== Hwi_excFillContext ========
 */
Void Hwi_excFillContext(UInt *excStack)
{
    Hwi_ExcContext *excContext;
    Char *stack = NULL;
    SizeT stackSize = 0;
    UInt coreId = Core_getId();

    excContext = Hwi_module->excContext[coreId];

    excContext->threadType = BIOS_getThreadType();

    /* copy registers from stack to excContext */
    excContext->r0 = (Ptr)excStack[8];       /* r0 */
    excContext->r1 = (Ptr)excStack[9];       /* r1 */
    excContext->r2 = (Ptr)excStack[10];      /* r2 */
    excContext->r3 = (Ptr)excStack[11];      /* r3 */
    excContext->r4 = (Ptr)excStack[0];       /* r4 */
    excContext->r5 = (Ptr)excStack[1];       /* r5 */
    excContext->r6 = (Ptr)excStack[2];       /* r6 */
    excContext->r7 = (Ptr)excStack[3];       /* r7 */
    excContext->r8 = (Ptr)excStack[4];       /* r8 */
    excContext->r9 = (Ptr)excStack[5];       /* r9 */
    excContext->r10 = (Ptr)excStack[6];      /* r10 */
    excContext->r11 = (Ptr)excStack[7];      /* r11 */
    excContext->r12 = (Ptr)excStack[12];     /* r12 */
    excContext->sp  = (Ptr)(UInt32)(excStack+16); /* sp */
    excContext->lr  = (Ptr)excStack[13];     /* lr */
    excContext->pc  = (Ptr)excStack[14];     /* pc */
    excContext->psr = (Ptr)excStack[15];     /* psr */

    switch (excContext->threadType) {
        case BIOS_ThreadType_Task: {
            if (BIOS_taskEnabled) {
                excContext->threadHandle = (Ptr)Task_self();
                stack = (Task_self())->stack;
                stackSize = (Task_self())->stackSize;
            }
            break;
        }
        case BIOS_ThreadType_Swi: {
            if (BIOS_swiEnabled) {
                excContext->threadHandle = (Ptr)Swi_self();
                stack = Hwi_module->isrStackBase;
                stackSize = (SizeT)Hwi_module->isrStackSize;
            }
            break;
        }
        case BIOS_ThreadType_Hwi: {
            excContext->threadHandle =
                (Ptr)Hwi_getHandle((UInt)(excContext->psr) & 0xff);
            stack = Hwi_module->isrStackBase;
            stackSize = (SizeT)Hwi_module->isrStackSize;
            break;
        }
        case BIOS_ThreadType_Main: {
            excContext->threadHandle = NULL;
            stack = Hwi_module->isrStackBase;
            stackSize = (SizeT)Hwi_module->isrStackSize;
            break;
        }
    }

    excContext->threadStackSize = stackSize;
    excContext->threadStack = (Ptr)stack;

    /* copy thread's stack contents if user has provided a buffer */
    if (Hwi_module->excStack[coreId] != NULL) {
        Char *from, *to;
        from = stack;
        to = (Char *)Hwi_module->excStack[coreId];
        while (stackSize--) {
            *to++ = *from++;
        }
    }


    excContext->ICSR = (Ptr)Hwi_nvic.ICSR;
    excContext->MMFSR = (Ptr)(UInt32)Hwi_nvic.MMFSR;
    excContext->BFSR = (Ptr)(UInt32)Hwi_nvic.BFSR;
    excContext->UFSR = (Ptr)(UInt32)Hwi_nvic.UFSR;
    excContext->HFSR = (Ptr)Hwi_nvic.HFSR;
    excContext->DFSR = (Ptr)Hwi_nvic.DFSR;
    excContext->MMAR = (Ptr)Hwi_nvic.MMAR;
    excContext->BFAR = (Ptr)Hwi_nvic.BFAR;
    excContext->AFSR = (Ptr)Hwi_nvic.AFSR;
}

/*
 *  ======== Hwi_excNmi ========
 */
Void Hwi_excNmi(UInt *excStack)
{
    Error_Block eb;
    Error_init(&eb);

    Error_raise(&eb, Hwi_E_NMI, NULL, 0);
}

/*
 *  ======== Hwi_excHardFault ========
 */
Void Hwi_excHardFault(UInt *excStack)
{
    Char *fault;
    Error_Block eb;
    Error_init(&eb);

    if (Hwi_nvic.HFSR & 0x40000000) {
        Error_raise(&eb, Hwi_E_hardFault, "FORCED", 0);
        Hwi_excUsageFault(excStack);
        Hwi_excBusFault(excStack);
        Hwi_excMemFault(excStack);
        return;
    }
    else if (Hwi_nvic.HFSR & 0x80000000) {
        Error_raise(&eb, Hwi_E_hardFault, "DEBUGEVT", 0);
        Hwi_excDebugMon(excStack);
        return;
    }
    else if (Hwi_nvic.HFSR & 0x00000002) {
        fault = "VECTBL";
    }
    else {
        fault = "Unknown";
    }
    Error_raise(&eb, Hwi_E_hardFault, fault, 0);
}

/*
 *  ======== Hwi_excMemFault ========
 */
Void Hwi_excMemFault(UInt *excStack)
{
    Char *fault;
    Error_Block eb;
    Error_init(&eb);

    if (Hwi_nvic.MMFSR) {
        if (Hwi_nvic.MMFSR & 0x10) {
            fault = "MSTKERR: Stacking Error (RD/WR failed), Stack Push";
        }
        else if (Hwi_nvic.MMFSR & 0x08) {
            fault = "MUNSTKERR: Unstacking Error (RD/WR failed), Stack Pop";
        }
        else if (Hwi_nvic.MMFSR & 0x02) {
            fault = "DACCVIOL: Data Access Violation (RD/WR failed)";
        }
        else if (Hwi_nvic.MMFSR & 0x01) {
            fault = "IACCVIOL: Instruction Access Violation";
        }
        else {
            fault = "Unknown";
        }
        Error_raise(&eb, Hwi_E_memFault, fault, Hwi_nvic.MMAR);
    }
}

/*
 *  ======== Hwi_excBusFault ========
 */
Void Hwi_excBusFault(UInt *excStack)
{
    Char *fault;
    Error_Block eb;
    Error_init(&eb);

    if (Hwi_nvic.BFSR) {
        if (Hwi_nvic.BFSR & 0x10) {
            fault = "STKERR: Bus Fault caused by Stack Push";
        }
        else if (Hwi_nvic.BFSR & 0x08) {
            fault = "UNSTKERR: Bus Fault caused by Stack Pop";
        }
        else if (Hwi_nvic.BFSR & 0x04) {
            fault = "IMPRECISERR: Delayed Bus Fault, exact addr unknown";
        }
        else if (Hwi_nvic.BFSR & 0x02) {
            fault = "PRECISERR: Immediate Bus Fault, exact addr known";
        }
        else if (Hwi_nvic.BFSR & 0x01) {
            fault = "IBUSERR: Instruction Access Violation";
        }
        else {
            fault = "Unknown";
        }
        Error_raise(&eb, Hwi_E_busFault, fault, Hwi_nvic.BFAR);
    }
}

/*
 *  ======== Hwi_excUsageFault ========
 */
Void Hwi_excUsageFault(UInt *excStack)
{
    Char *fault;
    Error_Block eb;
    Error_init(&eb);

    if (Hwi_nvic.UFSR) {
        if (Hwi_nvic.UFSR & 0x0001) {
            fault = "UNDEFINSTR: Undefined instruction";
        }
        else if (Hwi_nvic.UFSR & 0x0002) {
            fault = "INVSTATE: Invalid EPSR and instruction combination";
        }
        else if (Hwi_nvic.UFSR & 0x0004) {
            fault = "INVPC: Invalid PC";
        }
        else if (Hwi_nvic.UFSR & 0x0008) {
            fault = "NOCP: Attempting to use co-processor";
        }
        else if (Hwi_nvic.UFSR & 0x0100) {
            fault = "UNALIGNED: Unaligned memory access";
        }
        else if (Hwi_nvic.UFSR & 0x0200) {
            fault = "DIVBYZERO";
        }
        else {
            fault = "Unknown";
        }
        Error_raise(&eb, Hwi_E_usageFault, fault, 0);
    }
}

/*
 *  ======== Hwi_excSvCall ========
 */
Void Hwi_excSvCall(UInt *excStack)
{
    UInt8 *pc;
    Error_Block eb;

    Error_init(&eb);

    pc = (UInt8 *)excStack[14];

    Error_raise(&eb, Hwi_E_svCall, pc[-2], 0);
}

/*
 *  ======== Hwi_excDebugMon ========
 */
Void Hwi_excDebugMon(UInt *excStack)
{
    Char *fault;
    Error_Block eb;
    Error_init(&eb);

    if (Hwi_nvic.DFSR) {
        if (Hwi_nvic.DFSR & 0x00000010) {
            fault = "EXTERNAL";
        }
        else if (Hwi_nvic.DFSR & 0x00000008) {
            fault = "VCATCH";
        }
        else if (Hwi_nvic.DFSR & 0x00000004) {
            fault = "DWTTRAP";
        }
        else if (Hwi_nvic.DFSR & 0x00000002) {
            fault = "BKPT";
        }
        else if (Hwi_nvic.DFSR & 0x00000001) {
            fault = "HALTED";
        }
        else {
            fault = "Unknown";
        }
        Error_raise(&eb, Hwi_E_debugMon, fault, 0);
    }
}

/*
 *  ======== Hwi_excReserved ========
 */
Void Hwi_excReserved(UInt *excStack, UInt excNum)
{
    Error_Block eb;
    Error_init(&eb);

    Error_raise(&eb, Hwi_E_reserved, "Exception #:", excNum);
}

/*
 *  ======== Hwi_excNoIsr ========
 */
Void Hwi_excNoIsr(UInt *excStack, UInt excNum)
{
    Error_Block eb;
    Error_init(&eb);

    Error_raise(&eb, Hwi_E_noIsr, excNum, excStack[14]);
}

/*
 *  ======== Hwi_excDumpRegs ========
 */
Void Hwi_excDumpRegs(UInt lr)
{
    Hwi_ExcContext *excp;
    Char *ttype;
    UInt coreId = Core_getId();
    Char *name;

    excp = Hwi_module->excContext[coreId];

    switch (lr) {
        case 0xfffffff1:
            System_printf("Exception occurred in ISR thread at PC = 0x%08x.\n", excp->pc);
            break;
        case 0xfffffff9:
        case 0xfffffffd:
            System_printf("Exception occurred in background thread at PC = 0x%08x.\n", excp->pc);
            break;
        default:
            System_printf("Bogus Exception return value: %08x.\n", lr);
            break;
    }

    switch (excp->threadType) {
        case BIOS_ThreadType_Task: {
            ttype = "Task";
            if (BIOS_taskEnabled) {
                name = Task_Handle_name(excp->threadHandle);
            }
            else {
                name = "Not known";
            }
            break;
        }
        case BIOS_ThreadType_Swi: {
            ttype = "Swi";
            if (BIOS_swiEnabled) {
                name = Swi_Handle_name(excp->threadHandle);
            }
            else {
                name = "Not known";
            }
            break;
        }
        case BIOS_ThreadType_Hwi: {
            ttype = "Hwi";
            name = Hwi_Handle_name(excp->threadHandle);
            break;
        }
        case BIOS_ThreadType_Main: {
            ttype = "Main";
            name = "main()";
            break;
        }
    }

    if (name == NULL) {
        name = "(unnamed)";
    }

    System_printf("Core %d: Exception occurred in ThreadType_%s.\n", coreId, ttype);

    System_printf("%s name: %s, handle: 0x%x.\n", ttype, name, excp->threadHandle);
    System_printf("%s stack base: 0x%x.\n", ttype, excp->threadStack);
    System_printf("%s stack size: 0x%x.\n", ttype, excp->threadStackSize);

    System_printf("R0 = 0x%08x  R8  = 0x%08x\n", excp->r0, excp->r8);
    System_printf("R1 = 0x%08x  R9  = 0x%08x\n", excp->r1, excp->r9);
    System_printf("R2 = 0x%08x  R10 = 0x%08x\n", excp->r2, excp->r10);
    System_printf("R3 = 0x%08x  R11 = 0x%08x\n", excp->r3, excp->r11);
    System_printf("R4 = 0x%08x  R12 = 0x%08x\n", excp->r4, excp->r12);
    System_printf("R5 = 0x%08x  SP(R13) = 0x%08x\n", excp->r5, excp->sp);
    System_printf("R6 = 0x%08x  LR(R14) = 0x%08x\n", excp->r6, excp->lr);
    System_printf("R7 = 0x%08x  PC(R15) = 0x%08x\n", excp->r7, excp->pc);
    System_printf("PSR = 0x%08x\n", excp->psr);
    System_printf("ICSR = 0x%08x\n", Hwi_nvic.ICSR);
    System_printf("MMFSR = 0x%02x\n", Hwi_nvic.MMFSR);
    System_printf("BFSR = 0x%02x\n", Hwi_nvic.BFSR);
    System_printf("UFSR = 0x%04x\n", Hwi_nvic.UFSR);
    System_printf("HFSR = 0x%08x\n", Hwi_nvic.HFSR);
    System_printf("DFSR = 0x%08x\n", Hwi_nvic.DFSR);
    System_printf("MMAR = 0x%08x\n", Hwi_nvic.MMAR);
    System_printf("BFAR = 0x%08x\n", Hwi_nvic.BFAR);
    System_printf("AFSR = 0x%08x\n", Hwi_nvic.AFSR);
}

/*
 *  ======== Hwi_flushVnvic ========
 */
Void Hwi_flushVnvic()
{
    UInt i;
    UInt coreId = Core_getId();

    if ((Hwi_module->vnvicFlushRequired & (1 << coreId)) == 0) {
        return;
    }

    /* do new user interrupt priorities */
    for (i = 0; i < Hwi_NUM_INTERRUPTS - 16; i++) {
        if (Hwi_nvic.IPR[i] != Hwi_vnvic.IPR[i]) {
            Hwi_nvic.IPR[i] = Hwi_vnvic.IPR[i];
        }
    }

    /* do new system interrupt priorities */
    for (i = 0; i < 12; i++) {
        if (Hwi_nvic.SHPR[i] != Hwi_vnvic.SHPR[i]) {
            Hwi_nvic.SHPR[i] = Hwi_vnvic.SHPR[i];
        }
    }

    /*
     * Ducati M3's support only 64 user interrupts
     * which are represented in the first 2 ISER,
     * ICER, and ICPR register sets.
     *
     * The following register processing order is followed
     * to support the common, disable/clear/enable procedure
     * followed by various functions that manage Hwis.
     */
    for (i = 0; i < 2; i++) {
        /* do new interrupt diasbles */
        if (Hwi_vnvic.ICER[i] != 0) {
            Hwi_nvic.ICER[i] = Hwi_vnvic.ICER[i] & Hwi_module->intAffinityMasks[coreId][i];
            Hwi_vnvic.ICER[i] &= ~Hwi_module->intAffinityMasks[coreId][i];
        }

        /* do new interrupt clears */
        if (Hwi_vnvic.ICPR[i] != 0) {
            Hwi_nvic.ICPR[i] = Hwi_vnvic.ICPR[i] & Hwi_module->intAffinityMasks[coreId][i];
            Hwi_vnvic.ICPR[i] &= ~Hwi_module->intAffinityMasks[coreId][i];
        }

        /* do new interrupt enables */
        if (Hwi_nvic.ISER[i] != Hwi_vnvic.ISER[i]) {
            Hwi_nvic.ISER[i] = Hwi_vnvic.ISER[i] & Hwi_module->intAffinityMasks[coreId][i];
        }
    }

    Hwi_module->vnvicFlushRequired &= ~(1 << coreId);
}

/*
 *  ======== Hwi_updateNvic ========
 */
Void Hwi_updateNvic(UInt intNum)
{
    UInt coreId, otherId, affinity;

    coreId = Core_getId();
    affinity = Hwi_module->intAffinity[intNum];

    if (coreId == affinity) {
        Hwi_module->vnvicFlushRequired |= (1 << coreId);
        Hwi_flushVnvic();
    }
    else {
        otherId = coreId ^ 1;
        Hwi_module->vnvicFlushRequired |= (1 << otherId);
        Core_interruptCore(otherId);
    }
}

/*
 *  ======== Hwi_dispatchC ========
 *  "Top Half" of Configurable IRQ interrupt dispatcher.
 */
UInt Hwi_dispatchC(Hwi_Irp irp, UInt32 dummy1, UInt32 dummy2, Hwi_Object *hwi)
{
    /*
     * Enough room is reserved above the isr stack to handle
     * as many as 16 32-bit stack resident local variables.
     * If the dispatcher requires more than this, you must
     * handle this in Hwi_Module_startup().
     */

    BIOS_ThreadType prevThreadType;
    UInt tskKey = 1;
    UInt swiKey = 1;
#ifndef ti_sysbios_hal_Hwi_DISABLE_ALL_HOOKS
    Int i;
#endif

    Core_hwiDisable();

    Core_lock();

    if (Hwi_dispatcherTaskSupport) {
        tskKey = TASK_DISABLE();
    }

    if (Hwi_dispatcherSwiSupport) {
        swiKey = SWI_DISABLE();
    }

    /* set thread type to Hwi */
    prevThreadType = BIOS_setThreadType(BIOS_ThreadType_Hwi);

    if (Hwi_numSparseInterrupts == 0) {
        UInt intNum = (Hwi_nvic.ICSR & 0x000000ff);
        Hwi_Object **dispatchTable = (Hwi_Object **)Hwi_module->dispatchTable;
        hwi = dispatchTable[intNum];
    }

    /* IRP tracking is always enabled for M3 */
    hwi->irp = irp;

#ifndef ti_sysbios_hal_Hwi_DISABLE_ALL_HOOKS
    /* call the begin hooks */
    for (i = 0; i < Hwi_hooks.length; i++) {
        if (Hwi_hooks.elem[i].beginFxn != NULL) {
            Hwi_hooks.elem[i].beginFxn((IHwi_Handle)hwi);
        }
    }
#endif

    Log_write5(Hwi_LM_begin, (IArg)hwi, (IArg)hwi->fxn,
               (IArg)prevThreadType, (IArg)hwi->intNum, hwi->irp);

    /* call the user's isr */
    if (Hwi_dispatcherAutoNestingSupport) {
        Core_hwiEnable();
        (hwi->fxn)(hwi->arg);
        Core_hwiDisable();
    }
    else {
        (hwi->fxn)(hwi->arg);
    }

    Log_write1(Hwi_LD_end, (IArg)hwi);

#ifndef ti_sysbios_hal_Hwi_DISABLE_ALL_HOOKS
    /* call the end hooks */
    for (i = 0; i < Hwi_hooks.length; i++) {
        if (Hwi_hooks.elem[i].endFxn != NULL) {
            Hwi_hooks.elem[i].endFxn((IHwi_Handle)hwi);
        }
    }
#endif

    /* restore thread type */
    BIOS_setThreadType(prevThreadType);

    /* encode both tskKey and swiKey in return */
    return ((tskKey << 8) + swiKey);
}

/*
 *  ======== Hwi_doSwiRestore ========
 *  Run swi scheduler. (Executes on Hwi stack)
 */
Void Hwi_doSwiRestore(UInt swiTskKey)
{
    /* Run Swi scheduler */
    if (Hwi_dispatcherSwiSupport) {
        SWI_RESTORE(swiTskKey & 0xff);    /* Run Swi scheduler */
    }
}

/*
 *  ======== Hwi_doTaskRestore ========
 *  Run task scheduler. (Executes on Task stack)
 */
Void Hwi_doTaskRestore(UInt swiTskKey)
{
    /* Run Task scheduler */
    if (Hwi_dispatcherTaskSupport) {
        TASK_RESTORE(swiTskKey >> 8);   /* returns with ints disabled */
    }
}
