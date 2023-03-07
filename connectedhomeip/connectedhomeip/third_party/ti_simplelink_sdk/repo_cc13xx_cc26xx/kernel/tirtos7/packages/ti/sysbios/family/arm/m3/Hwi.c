/*
 * Copyright (c) 2015-2020 Texas Instruments Incorporated - http://www.ti.com
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
/* REQ_TAG(SYSBIOS-1006) */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Task.h>

/* REQ_TAG(SYSBIOS-547) - enable/disable/restore implementations in header: */
#include <ti/sysbios/family/arm/m3/Hwi.h>

#include <ti/sysbios/runtime/Assert.h>
#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/Memory.h>
#include <ti/sysbios/runtime/Startup.h>
#include <ti/sysbios/runtime/System.h>
#include <ti/sysbios/runtime/Types.h>

extern uint32_t ti_sysbios_family_arm_m3_Hwi_dispatchTable[];

extern void * __TI_STACK_BASE;
extern void * __TI_STACK_SIZE;

/*
 *  Below pointer to swiTaskKeys is provided to immunize asm code
 *  from changes in Hwi module state
 */
unsigned int *const Hwi_swiTaskKeyAddress = &Hwi_module->swiTaskKeys;

/*
 *  ======== Hwi_Params_default ========
 */
static const Hwi_Params Hwi_Params_default = {
    .enableInt = 1,
    .arg = 0,
    .priority = 0xe0,
    .useDispatcher = 1
};

#if Hwi_numSparseInterrupts_D
uint32_t Hwi_dispatchTable[Hwi_numSparseInterrupts_D * 3 + 1];
#else
uint32_t Hwi_dispatchTable[Hwi_NUM_INTERRUPTS_D];
#endif

#ifdef __IAR_SYSTEMS_ICC__

/* place holder for RAM vector table */
#pragma location=Hwi_vectorTableAddress_D
__root __no_init uint32_t Hwi_ramVectors[Hwi_NUM_INTERRUPTS_D];

#else /* __IAR_SYSTEMS_ICC__ */

#if defined(__GNUC__) && !defined(__ti__)
__attribute__ ((section (".vtable")))
#else
__attribute__ ((section (".vecs")))
#endif

/* place holder for RAM vector table */
uint32_t Hwi_ramVectors[Hwi_NUM_INTERRUPTS_D];

#endif /* __IAR_SYSTEMS_ICC__ */

Hwi_Module_State Hwi_Module_state = {
    .taskSP = ((char*)NULL),
    .excActive = 0,
    .excContext = NULL,
    .excStack = NULL,
    .isrStack = NULL,
    .isrStackBase = NULL,   /* initialized at runtime */
    .isrStackSize = NULL,   /* initialized at runtime */
    .vectorTableBase = ((void *)((void*)&Hwi_ramVectors[0])),
    .swiTaskKeys = (unsigned int)0x101,
    .dispatchTable = ((void *)((void*)&Hwi_dispatchTable[0])),
    .vnvicFlushRequired = 0,
    .intAffinity = ((void*)0),
    .intAffinityMasks = ((void*)0),
    .vectorTable = ((void*)0),
    .initDone = 0
};

const Hwi_Hook Hwi_hooks = {
    .length = HwiHooks_numHooks_D,
    .elem = HwiHooks_array
};

const bool Hwi_dispatcherAutoNestingSupport = Hwi_dispatcherAutoNestingSupport_D;
const bool Hwi_dispatcherSwiSupport = Hwi_dispatcherSwiSupport_D;
const bool Hwi_dispatcherTaskSupport = Hwi_dispatcherTaskSupport_D;
const unsigned int Hwi_disablePriority = Hwi_disablePriority_D;
const int Hwi_NUM_INTERRUPTS = Hwi_NUM_INTERRUPTS_D;
const unsigned int Hwi_numSparseInterrupts = Hwi_numSparseInterrupts_D;
const uint32_t Hwi_ccr = (uint32_t)Hwi_nvicCCR_D;
const unsigned int Hwi_priGroup = Hwi_priGroup_D;
const Hwi_ExcHandlerFuncPtr Hwi_excHandlerFunc = (Hwi_ExcHandlerFuncPtr)Hwi_excHandlerFunc_D;
const Hwi_VectorFuncPtr Hwi_nullIsrFunc = (Hwi_VectorFuncPtr)Hwi_excHandlerAsm;
const Hwi_ExcHookFunc Hwi_excHookFunc = NULL;

#ifdef __IAR_SYSTEMS_ICC__

extern void *CSTACK$$Base;
extern void *CSTACK$$Limit;
extern void __iar_program_start();

#pragma segment="CSTACK"
typedef union {uint32_t __uint32; void * __ptr;} intvec_elem;

#pragma location = ".intvec"
const uint32_t __vector_table[] =
{
    (uint32_t)(&CSTACK$$Limit),
    (uint32_t)(&__iar_program_start),
    (uint32_t)(&Hwi_nmiFunc_D),           /* NMI */
    (uint32_t)(&Hwi_hardFaultFunc_D),     /* Hard Fault */
    (uint32_t)(&Hwi_memFaultFunc_D),      /* Mem Fault */
    (uint32_t)(&Hwi_busFaultFunc_D),      /* Bus Fault */
    (uint32_t)(&Hwi_usageFaultFunc_D),    /* Usage Fault */
    (uint32_t)(&Hwi_reservedFunc_D),      /* reserved */
    (uint32_t)(&Hwi_reservedFunc_D),      /* reserved */
    (uint32_t)(&Hwi_reservedFunc_D),      /* reserved */
    (uint32_t)(&Hwi_reservedFunc_D),      /* reserved */
    (uint32_t)(&Hwi_svCallFunc_D),        /* SVCall */
    (uint32_t)(&Hwi_debugMonFunc_D),      /* Debug Mon */
    (uint32_t)(&Hwi_reservedFunc_D),      /* reserved */
    (uint32_t)(&Hwi_pendSV),              /* pendSV */
};

#define ti_sysbios_family_arm_m3_Hwi_resetVectors __vector_table

#else /* __IAR_SYSTEMS_ICC__ */

extern void _c_int00(void);

#if defined(__clang__)
__attribute__ ((section (".resetVecs")))
#elif defined(__GNUC__) && !defined(__ti__)
__attribute__ ((section (".intvecs")))
#elif defined(__ti__)
__attribute__ ((section (".resetVecs")))
#endif
/* const because it is meant to be placed in ROM */
const uint32_t Hwi_resetVectors[] = {
    (uint32_t)(&__TI_STACK_BASE),
    (uint32_t)(&Hwi_resetFunc_D),
    (uint32_t)(&Hwi_nmiFunc_D),           /* NMI */
    (uint32_t)(&Hwi_hardFaultFunc_D),     /* Hard Fault */
    (uint32_t)(&Hwi_memFaultFunc_D),      /* Mem Fault */
    (uint32_t)(&Hwi_busFaultFunc_D),      /* Bus Fault */
    (uint32_t)(&Hwi_usageFaultFunc_D),    /* Usage Fault */
    (uint32_t)(&Hwi_reservedFunc_D),      /* reserved */
    (uint32_t)(&Hwi_reservedFunc_D),      /* reserved */
    (uint32_t)(&Hwi_reservedFunc_D),      /* reserved */
    (uint32_t)(&Hwi_reservedFunc_D),      /* reserved */
    (uint32_t)(&Hwi_svCallFunc_D),        /* SVCall */
    (uint32_t)(&Hwi_debugMonFunc_D),      /* Debug Mon */
    (uint32_t)(&Hwi_reservedFunc_D),      /* reserved */
    (uint32_t)(&Hwi_pendSV),              /* pendSV */
};

#endif /* __IAR_SYSTEMS_ICC__ */

#if 0
/*
 *  ======== instrumentation ========
 *  Must define these macros before including Log.h
 */
#undef Log_moduleName
#define Log_moduleName ti_sysbios_family_arm_m3_Hwi

#ifdef Hwi_log_D
#define ti_utils_runtime_Log_ENABLE_PPO
#ifdef Hwi_logInfo1_D
#define ti_utils_runtime_Log_ENABLE_INFO1
#endif
#ifdef Hwi_logInfo2_D
#define ti_utils_runtime_Log_ENABLE_INFO2
#endif
#endif

#include <ti/utils/runtime/Log.h>

Log_EVENT(Hwi_beginLogEvt, Log_Type_HwiBegin,
    "hwi begin: hwi: 0x%x, prev thread type: %d, intNum: %d, irp: 0x%x");
Log_EVENT(Hwi_endLogEvt, Log_Type_HwiEnd, "hwi end: 0x%x");

#else
#define Log_write(module, level, ...)
#endif

/*
 *  ======== Hwi_init ========
 */
void Hwi_init(void)
{
    /* Interrupts are disabled at this point */
    if (Hwi_module->initDone) {
        return;
    }
    Hwi_module->initDone = true;

    /*
     *  perform the BIOS specific interrupt disable operation so
     *  main is entered with interrupts disabled.
     */
    Hwi_disable();

#ifdef Hwi_ENABLE_REGISTER_HOOKS
    int i;

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
        (char *)(((uint32_t)(Hwi_module->isrStackBase) & 0xfffffff8U) +
                (uint32_t)Hwi_module->isrStackSize - 2U * sizeof(int));

    Hwi_module->taskSP = (char *)-1;    /* signal that we're executing */
                                        /* on the ISR stack */

#ifdef ti_sysbios_family_arm_m3_Hwi_FIX_MSP432_DISPATCH_TABLE_ADDRESS
    /* map dispatchTable's SRAM_DATA address into its corresponding SRAM_CODE memory region */
    uint32_t dispatchTable = (uint32_t)Hwi_module->dispatchTable;
    dispatchTable = (dispatchTable & 0x00fffff) | 0x01000000;
    Hwi_module->dispatchTable = (void *)dispatchTable;
#endif

#if Hwi_numSparseInterrupts_D
    /*
     * place address of Hwi_dispatch at end of sparse table,
     * is fetched at runtime
     */
    Hwi_dispatchTable[Hwi_numSparseInterrupts*3] = (uint32_t)Hwi_dispatch;
#endif
}

/*
 *  ======== Hwi_construct2 ========
 */
Hwi_Handle Hwi_construct2(Hwi_Struct2 *hwiStruct2, int intNum,
        Hwi_FuncPtr hwiFxn, const Hwi_Params *prms)
{
    Hwi_Handle hwi, status;

    /* check vector table entry for already in use vector */
    if (*(Hwi_module->vectorTable + intNum) != Hwi_nullIsrFunc) {
        return (NULL);
    }

    status = Hwi_construct((Hwi_Struct *)hwiStruct2, intNum, hwiFxn, prms,
            Error_IGNORE);
    if (status == NULL) {
        return (NULL);
    }

    /* check vector table entry for success */
    if (*(Hwi_module->vectorTable + intNum) == Hwi_nullIsrFunc) {
        return (NULL);
    }

    hwi = hwiStruct2;
    return (hwi);
}

/*
 *  ======== Hwi_construct ========
 */
Hwi_Handle Hwi_construct(Hwi_Object *hwi,
        int intNum, Hwi_FuncPtr fxn, const Hwi_Params *params, Error_Block *eb)
{
    int status = 0;

    if (params == NULL) {
        params = &Hwi_Params_default;
    }

    status = Hwi_Instance_init(hwi, intNum, fxn, params, eb);

    if (status != 0) {
        Hwi_Instance_finalize(hwi, status);
        hwi = NULL;
    }

    return (hwi);
}

/*
 *  ======== Hwi_create ========
 */
Hwi_Handle Hwi_create(
        int intNum, Hwi_FuncPtr fxn, const Hwi_Params *params, Error_Block *eb)
{
    Hwi_Handle hwi, status;

    hwi = Memory_alloc(NULL, sizeof(Hwi_Object), 0, eb);

    if (hwi != NULL) {
        status = Hwi_construct(hwi, intNum, fxn, params, eb);
        if (status == NULL) {
            Memory_free(NULL, hwi, sizeof(Hwi_Object));
            hwi = NULL;
        }
    }

    return (hwi);
}

/*
 *  ======== Hwi_delete ========
 */
void Hwi_delete(Hwi_Handle *hwi)
{
    Hwi_destruct(*hwi);

    Memory_free(NULL, *hwi, sizeof(Hwi_Object));

    *hwi = NULL;
}

/*
 *  ======== Hwi_delete ========
 */
void Hwi_destruct(Hwi_Handle hwi)
{
    Hwi_Instance_finalize(hwi, 0);
}

/*
 *  ======== Hwi_Instance_init ========
 */
int Hwi_Instance_init(Hwi_Object *hwi, int intNum,
                      Hwi_FuncPtr fxn, const Hwi_Params *params,
                      Error_Block *eb)
{
    int status;

    if (intNum >= Hwi_NUM_INTERRUPTS) {
        Error_raise(eb, Hwi_E_badIntNum, intNum, 0);
        return (1);
    }

    hwi->intNum = (int16_t)intNum;

    /* check vector table entry for already in use vector */
    if (*(Hwi_module->vectorTable + intNum) != Hwi_nullIsrFunc) {
        Error_raise(eb, Hwi_E_alreadyDefined, intNum, 0);
        return (1);
    }

#ifdef Hwi_ENABLE_HOOKS
    if (Hwi_hooks.length > 0) {
        /* Allocate environment space for each hook instance. */
        hwi->hookEnv = Memory_calloc(Hwi_Object_heap(),
                Hwi_hooks.length * sizeof(void *), 0, eb);

        if (hwi->hookEnv == NULL) {
            return (2);
        }
    }
#endif

    Hwi_disableInterrupt((unsigned int)intNum);

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
        hwi->priority = (uint8_t)params->priority;
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
    if (params->enableInt != false) {
        hwi->irp = 0x1;
    }

    /* encode 'useDispatcher' in bit 1 */
    if (params->useDispatcher != false) {
        hwi->irp |= 0x2U;
    }

    status = Hwi_postInit(hwi, eb);

    if (status != 0) {
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
/* REQ_TAG(SYSBIOS-1007) */
int Hwi_postInit (Hwi_Object *hwi, Error_Block *eb)
{
    unsigned int intNum;

#ifdef Hwi_ENABLE_CREATE_HOOKS
    int i;
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
        hwi->hookEnv[i] = (void *)0;
        if (Hwi_hooks.elem[i].createFxn != NULL) {
            Hwi_hooks.elem[i].createFxn(hwi, leb);

            if (Error_check(leb)) {
                return (i + 1);
            }
        }
    }
#endif

    /*
     * Bypass dispatcher for zero-latency interrupts
     * and interrupts with useDispatcher == false
     */

    /* save intNum for use by Hwi_enableInterrupt */
    intNum = (unsigned int)hwi->intNum;

    if (((hwi->irp & 0x2U) == 0U) ||
        (hwi->priority < Hwi_disablePriority)) {
        Hwi_plug((unsigned int)hwi->intNum, (void *)hwi->fxn);
        /*
         * encode useDispatcher == false as a negative intNum
         * This is done to inform ROV that this is a non-dispatched interrupt
         * without adding a new field to the Hwi object.
         */
        hwi->intNum = 0 - hwi->intNum;
    }
    else {
        if (Hwi_numSparseInterrupts != 0U) {
            int i = 0;
            uint32_t *sparseInterruptTableEntry = Hwi_module->dispatchTable;
            char *vectorPtr;
            bool found = false;

            /* find an unused sparseTableEntry */
            do {
                if (sparseInterruptTableEntry[2] == 0U) {
                    found = true;
                    break;
                }
                else {
                    sparseInterruptTableEntry += 3;
                }
                i++;
            } while (i < (int)Hwi_numSparseInterrupts);

            if (found != false) {
                /* point to the Hwi object.
                 * Inject this code into words 0, 1, 2:
                 * ldr r3, hwiObjAddr
                 * ldr pc, Hwi_dispatchAddr
                 * hwiObjAddr .word hwi
                 */
                sparseInterruptTableEntry[0] = 0xf8df4b01;
                sparseInterruptTableEntry[1] = 0x0000f000 + (3-i) * 0xc - 4;
                sparseInterruptTableEntry[2] = (uint32_t)hwi;

                /* plug the vector table with the sparseTable entry */
                vectorPtr = (char *)sparseInterruptTableEntry;
                vectorPtr += 1;    /* make it a thumb func vector */
                Hwi_plug((unsigned int)hwi->intNum, (void *)vectorPtr);
            }
            else {
                Error_raise(eb, Hwi_E_hwiLimitExceeded, 0, 0);
#ifdef Hwi_ENABLE_HOOKS
                return (Hwi_hooks.length); /* unwind all Hwi_hooks */
#else
                return (1);
#endif
            }
        }
        else {
            Hwi_Object **dispatchTable = (Hwi_Object **)Hwi_module->dispatchTable;
            dispatchTable[intNum] = hwi;
            Hwi_plug(intNum, (void *)Hwi_dispatch);
        }
    }

    Hwi_setPriority(intNum, hwi->priority);

    if ((hwi->irp & 0x1U) != 0U) {
        Hwi_enableInterrupt(intNum);
    }

    hwi->irp = 0;

    return (0);
}

/*
 *  ======== Hwi_Instance_finalize ========
 */
void Hwi_Instance_finalize(Hwi_Object *hwi, int status)
{
    unsigned int intNum;

    if (status == 1) {  /* vector in use */
        return;
    }

#ifdef Hwi_ENABLE_HOOKS
    if (Hwi_hooks.length > 0) {
#ifdef Hwi_ENABLE_DELETE_HOOKS
        int i, cnt;

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
                Hwi_hooks.elem[i].deleteFxn(hwi);
            }
        }
#endif
        Memory_free(Hwi_Object_heap(), hwi->hookEnv,
                Hwi_hooks.length * sizeof(void *));
    }
#endif

    /* compensate for encoded intNum */
    if (hwi->intNum < 0) {
        intNum = 0U - (unsigned int)hwi->intNum;
    }
    else {
        intNum = (unsigned int)hwi->intNum;
    }

    Hwi_disableInterrupt(intNum);
    Hwi_plug(intNum, (void *)Hwi_nullIsrFunc);

    if (Hwi_numSparseInterrupts != 0U) {
        int i = 0;
        uint32_t *sparseInterruptTableEntry = Hwi_module->dispatchTable;

        do {
            if (sparseInterruptTableEntry[2] == (uint32_t)hwi) {
                sparseInterruptTableEntry[2] = 0;
                break;
            }
            else {
                sparseInterruptTableEntry += 3;
            }
            i++;
        } while (i < (int)Hwi_numSparseInterrupts);
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

/* REQ_TAG(SYSBIOS-1007) */
void Hwi_initNVIC()
{
    int intNum;
    uint32_t *ramVectors;
    Hwi_VectorFuncPtr *ramVectorFuncs;

    Hwi_disable();

#if defined(__IAR_SYSTEMS_ICC__)
    #pragma section = "CSTACK"
    Hwi_module->isrStackBase = (void *)__section_begin("CSTACK");
    size_t size = (uint8_t *)__section_end("CSTACK") - (uint8_t *)__section_begin("CSTACK");
    Hwi_module->isrStackSize = (void *)size;
#else
    Hwi_module->isrStackBase = ((void *)((void*)&__TI_STACK_BASE));
    Hwi_module->isrStackSize = ((void *)((void*)&__TI_STACK_SIZE));
#endif

    /*
     *  Startup.firstFxns are called before Module.startup functions, so
     *  initializate vector table base here.
     */
    Hwi_module->vectorTable = (Hwi_VectorFuncPtr *)Hwi_module->vectorTableBase;

    /* configure Vector Table Offset Register */
    Hwi_nvic.VTOR = (uint32_t)Hwi_module->vectorTableBase;

    /* copy ROM vector table contents to RAM vector table */
    if (Hwi_nvic.VTOR != (uint32_t)Hwi_resetVectors) {
        ramVectors = Hwi_module->vectorTableBase;
        ramVectorFuncs = Hwi_module->vectorTable;

        for (intNum = 0; intNum < Hwi_NUM_INTERRUPTS; intNum++) {
            if (intNum < 15) {
#if defined(__IAR_SYSTEMS_ICC__)
                ramVectors[intNum] = __vector_table[intNum];
#else
                ramVectors[intNum] = Hwi_resetVectors[intNum];
#endif
            }
            else {
                ramVectorFuncs[intNum] = Hwi_nullIsrFunc;
            }
        }
    }

    /* Set the configured PRIGROUP value */
    Hwi_nvic.AIRCR = (Hwi_nvic.AIRCR & 0xffff00ffU) + (Hwi_priGroup << 8) + 0x05fa0000U;

    /* set pendSV interrupt priority to Hwi_disablePriority */
    Hwi_nvic.SHPR[10] = Hwi_disablePriority;

    /* set CCR per user's preference */
    Hwi_nvic.CCR = Hwi_ccr;

#if ((defined(__ti__) && defined(__ARM_FP)) || \
    (defined(__VFP_FP__) && !defined(__SOFTFP__)) || \
    defined(__ARMVFP__))
    /* disable lazy stacking mode fp indications in control register */
    Hwi_nvic.FPCCR &= ~0xc0000000; /* clear ASPEN and LSPEN bits */
#endif

#if (defined(__IAR_SYSTEMS_ICC__) && (__CORE__ == __ARM8M_MAINLINE__)) || \
    (defined(__GNUC__) && !defined(__ti__) && \
     defined(__ARM_ARCH_8M_MAIN__))
    Hwi_setStackLimit(Hwi_module->isrStackBase);
#endif
}

/*
 *  ======== Hwi_startup ========
 */
void Hwi_startup()
{
    Hwi_enable();
}

/*
 *  ======== Hwi_disableInterrupt ========
 */
unsigned int Hwi_disableInterrupt(unsigned int intNum)
{
    unsigned int oldEnableState, index, mask;

    if (intNum >= 16U) {

        index = (intNum - 16U) >> 5;

        mask = (unsigned int)1U << ((intNum - 16U) & 0x1fU);

        oldEnableState = Hwi_nvic.ISER[index] & mask;

        Hwi_nvic.ICER[index] = mask;
    }
    else if (intNum == 15U) {
        oldEnableState = Hwi_nvic.STCSR & 0x00000002U;
        Hwi_nvic.STCSR &= ~0x00000002U;  /* disable SysTick int */
    }
    else {
        oldEnableState = 0;
    }

    return oldEnableState;
}

/*
 *  ======== Hwi_enableInterrupt ========
 */
unsigned int Hwi_enableInterrupt(unsigned int intNum)
{
    unsigned int oldEnableState, index, mask;

    if (intNum >= 16U) {

        index = (intNum - 16U) >> 5;

        mask = (unsigned int)1U << ((intNum - 16U) & 0x1fU);

        oldEnableState = Hwi_nvic.ISER[index] & mask;

        Hwi_nvic.ISER[index] = mask;
    }
    else if (intNum == 15U) {
        oldEnableState = Hwi_nvic.STCSR & 0x00000002U;
        Hwi_nvic.STCSR |= 0x00000002U;   /* enable SysTick int */
    }
    else {
        oldEnableState = 0;
    }

    return oldEnableState;
}

/*
 *  ======== Hwi_restoreInterrupt ========
 */
void Hwi_restoreInterrupt(unsigned int intNum, unsigned int key)
{
    unsigned int index, mask;

    if (intNum >= 16U) {

        index = (intNum - 16U) >> 5;

        mask = (unsigned int)1U << ((intNum - 16U) & 0x1fU);

        if (key != 0U) {
            Hwi_nvic.ISER[index] = mask;
        }
        else {
            Hwi_nvic.ICER[index] = mask;
        }
    }
    else {
        if (intNum == 15U) {
            if (key != 0U) {
                Hwi_nvic.STCSR |= 0x00000002U;       /* enable SysTick int */
            }
            else {
                Hwi_nvic.STCSR &= ~0x00000002U;      /* disable SysTick int */
            }
        }
    }
}

/*
 *  ======== Hwi_clearInterrupt ========
 */
/* REQ_TAG(SYSBIOS-546) */
void Hwi_clearInterrupt(unsigned int intNum)
{
    unsigned int index, mask;

    if (intNum >= 16U) {

        index = (intNum - 16U) >> 5;

        mask = (unsigned int)1U << ((intNum - 16U) & 0x1fU);

        Hwi_nvic.ICPR[index] = mask;
    }
    else {

    }
}

/*
 *  ======== Hwi_getHandle ========
 */
Hwi_Handle Hwi_getHandle(unsigned int intNum)
{
    Hwi_VectorFuncPtr *func;

    func = Hwi_module->vectorTable + intNum;

    if (Hwi_numSparseInterrupts != 0U) {
        if (*func != Hwi_nullIsrFunc) {
            char *vectorPtr = (char *)func;
            vectorPtr -= 1;
            uint32_t *sparseTableEntry = (uint32_t *)vectorPtr;
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
void Hwi_plug(unsigned int intNum, void *fxn)
{
    uint32_t *func;

    func = (uint32_t *)Hwi_module->vectorTableBase + intNum;

    /* guard against writing to static const vector table in flash */
    if (*func != (uint32_t)fxn) {
        *func = (uint32_t)fxn;
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
void Hwi_switchFromBootStack()
{
    /* split thread and handler stacks */
    Hwi_initStacks(Hwi_module->isrStack);

    Hwi_module->taskSP = 0;
}

/*
 *  ======== Hwi_getStackInfo ========
 *  Used to get Hwi stack usage info.
 */
bool Hwi_getStackInfo(Hwi_StackInfo *stkInfo, bool computeStackDepth)
{
    char *isrSP;
    bool stackOverflow;

    /* Copy the stack base address and size */
    stkInfo->hwiStackSize = (size_t)Hwi_module->isrStackSize;
    stkInfo->hwiStackBase = Hwi_module->isrStackBase;

    isrSP = stkInfo->hwiStackBase;

    /* Check for stack overflow */
    stackOverflow = (*isrSP != (char)0xbe ? true : false);

    if ((computeStackDepth != false) && (stackOverflow == false)) {
        /* Compute stack depth */
        do {
            isrSP++;
        } while(*isrSP == (char)0xbe);
        stkInfo->hwiStackPeak = stkInfo->hwiStackSize - (size_t)(--isrSP - (uint32_t)stkInfo->hwiStackBase);
    }
    else {
        stkInfo->hwiStackPeak = 0;
    }

    return stackOverflow;
}

/*
 *  ======== Hwi_getCoreStackInfo ========
 *  Used to get Hwi stack usage info.
 */
bool Hwi_getCoreStackInfo(Hwi_StackInfo *stkInfo, bool computeStackDepth, unsigned int coreId)
{
    return (Hwi_getStackInfo(stkInfo, computeStackDepth));
}

/*
 *  ======== Hwi_setPriority ========
 *  Set an interrupt's priority.
 *
 *  @param(intNum)      ID of interrupt
 *  @param(priority)    priority
 */
void Hwi_setPriority(unsigned int intNum, unsigned int priority)
{
    /* User interrupt (id >= 16) priorities are set in the IPR registers */
    if (intNum >= 16U) {
        Hwi_nvic.IPR[intNum - 16U] = (uint8_t)priority;
    }
    /* System interrupt (id >= 4) priorities are set in the SHPR registers */
    else {
        if (intNum >= 4U) {
            Hwi_nvic.SHPR[intNum - 4U] = (uint8_t)priority;
        }
        /* System interrupts (id < 4) priorities  are fixed in hardware */
    }
}

/*
 *  ======== Hwi_reconfig ========
 *  Reconfigure a dispatched interrupt.
 */
void Hwi_reconfig(Hwi_Object *hwi, Hwi_FuncPtr fxn, const Hwi_Params *params)
{
    unsigned int intNum;

    /* compensate for encoded intNum */
    if (hwi->intNum < 0) {
        intNum = 0U - (unsigned int)hwi->intNum;
    }
    else {
        intNum = (unsigned int)hwi->intNum;
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
        hwi->priority = (uint8_t)params->priority;
    }

    Hwi_setPriority(intNum, hwi->priority);

    if (params->enableInt != false) {
        Hwi_enableInterrupt(intNum);
    }
}

/*
 *  ======== Hwi_getfunc ========
 */
Hwi_FuncPtr Hwi_getFunc(Hwi_Object *hwi, uintptr_t *arg)
{
    *arg = hwi->arg;

    return (hwi->fxn);
}

/*
 *  ======== Hwi_setfunc ========
 */
void Hwi_setFunc(Hwi_Object *hwi, Hwi_FuncPtr fxn, uintptr_t arg)
{
    hwi->fxn = fxn;
    hwi->arg = arg;
}

/*
 *  ======== Hwi_getIrp ========
 */
uintptr_t Hwi_getIrp(Hwi_Object *hwi)
{
    return (hwi->irp);
}

/*
 *  ======== Hwi_getHookContext ========
 */
void * Hwi_getHookContext(Hwi_Object *hwi, int id)
{
    return (hwi->hookEnv[id]);
}

/*
 *  ======== Hwi_setHookContext ========
 */
void Hwi_setHookContext(Hwi_Object *hwi, int id, void * hookContext)
{
    hwi->hookEnv[id] = hookContext;
}

/*
 *  ======== Hwi_post ========
 */
void Hwi_post(unsigned int intNum)
{
    if (intNum >= 16U) {
        Hwi_nvic.STI = intNum - 16U;
    }
}

/*
 *  ======== Hwi_excSetBuffers ========
 */
void Hwi_excSetBuffers(void * excContextBuffer, void * excStackBuffer)
{
    Hwi_module->excContext = excContextBuffer;
    Hwi_module->excStack = excStackBuffer;
}

/*
 *  ======== Hwi_excHandler ========
 */
void Hwi_excHandler(unsigned int *excStack, unsigned int lr)
{
    Hwi_module->excActive = true;

    /* return to spin loop if no exception handler is plugged */
    if (Hwi_excHandlerFunc == NULL) {
        return;
    }

    Hwi_excHandlerFunc(excStack, lr);
}

/*
 *  ======== Hwi_excHandlerMin ========
 */
void Hwi_excHandlerMin(unsigned int *excStack, unsigned int lr)
{
    Hwi_ExcContext excContext;
    unsigned int excNum;

    Hwi_module->excActive = true;

    if (Hwi_module->excContext == NULL) {
        Hwi_module->excContext = &excContext;
    }

    Hwi_excFillContext(excStack);

    /* Force MAIN threadtype so hooks can safely call System_printf */

    BIOS_setThreadType(BIOS_ThreadType_Main);

    /* Call user's exception hook */
    if (Hwi_excHookFunc != NULL) {
        Hwi_excHookFunc(Hwi_module->excContext);
    }

    excNum = Hwi_nvic.ICSR & 0xffU;

    /* distinguish between an interrupt and an exception */
    if (excNum < 15U) {
        Error_raise(0, Hwi_E_exception, excNum, excStack[14]);
    }
    else {
        Error_raise(0, Hwi_E_noIsr, excNum, excStack[14]);
    }
/* LCOV_EXCL_START */
}
/* LCOV_EXCL_STOP */

/*
 *  ======== Hwi_excHandlerMax ========
 */
/* LCOV_EXCL_START */
void Hwi_excHandlerMax(unsigned int *excStack, unsigned int lr)
{
    Hwi_ExcContext excContext;
    unsigned int excNum;

    Hwi_module->excActive = true;

    if (Hwi_module->excContext == NULL) {
        Hwi_module->excContext = &excContext;
    }

    Hwi_excFillContext(excStack);

    /* Force MAIN threadtype so we can safely call System_printf */

    BIOS_setThreadType(BIOS_ThreadType_Main);

    Hwi_disable();

    excNum = Hwi_nvic.ICSR & 0xffU;

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
    if (Hwi_excHookFunc != NULL) {
        Hwi_excHookFunc(Hwi_module->excContext);
    }

    System_printf("Terminating execution...\n");

    Hwi_enable();

    BIOS_exit(0);
}
/* LCOV_EXCL_STOP */

/*
 *  ======== Hwi_excFillContext ========
 */
/* LCOV_EXCL_START */
void Hwi_excFillContext(unsigned int *excStack)
{
    Hwi_ExcContext *excContext;
    char *stack = NULL;
    size_t stackSize = 0;

    excContext = Hwi_module->excContext;

    excContext->threadType = BIOS_getThreadType();

    /* copy registers from stack to excContext */
    excContext->r0 = (void *)excStack[8];       /* r0 */
    excContext->r1 = (void *)excStack[9];       /* r1 */
    excContext->r2 = (void *)excStack[10];      /* r2 */
    excContext->r3 = (void *)excStack[11];      /* r3 */
    excContext->r4 = (void *)excStack[0];       /* r4 */
    excContext->r5 = (void *)excStack[1];       /* r5 */
    excContext->r6 = (void *)excStack[2];       /* r6 */
    excContext->r7 = (void *)excStack[3];       /* r7 */
    excContext->r8 = (void *)excStack[4];       /* r8 */
    excContext->r9 = (void *)excStack[5];       /* r9 */
    excContext->r10 = (void *)excStack[6];      /* r10 */
    excContext->r11 = (void *)excStack[7];      /* r11 */
    excContext->r12 = (void *)excStack[12];     /* r12 */
    excContext->sp  = (void *)(uint32_t)(excStack+16); /* sp */
    excContext->lr  = (void *)excStack[13];     /* lr */
    excContext->pc  = (void *)excStack[14];     /* pc */
    excContext->psr = (void *)excStack[15];     /* psr */

    switch (excContext->threadType) {
        case BIOS_ThreadType_Task: {
            if (BIOS_taskEnabled != false) {
                excContext->threadHandle = (void *)Task_self();
                stack = (Task_self())->stack;
                stackSize = (Task_self())->stackSize;
            }
            break;
        }
        case BIOS_ThreadType_Swi: {
            if (BIOS_swiEnabled != false) {
                excContext->threadHandle = (void *)Swi_self();
                stack = Hwi_module->isrStackBase;
                stackSize = (size_t)Hwi_module->isrStackSize;
            }
            break;
        }
        case BIOS_ThreadType_Hwi: {
            excContext->threadHandle =
                (void *)Hwi_getHandle((unsigned int)(excContext->psr) & 0xffU);
            stack = Hwi_module->isrStackBase;
            stackSize = (size_t)Hwi_module->isrStackSize;
            break;
        }
        case BIOS_ThreadType_Main: {
            excContext->threadHandle = NULL;
            stack = Hwi_module->isrStackBase;
            stackSize = (size_t)Hwi_module->isrStackSize;
            break;
        }
        default:
            break;
    }

    excContext->threadStackSize = stackSize;
    excContext->threadStack = (void *)stack;

    /* copy thread's stack contents if user has provided a buffer */
    if (Hwi_module->excStack != NULL) {
        char *from, *to;
        from = stack;
        to = (char *)Hwi_module->excStack;
        while (stackSize > 0U) {
            *to++ = *from++;
            stackSize--;
        }
    }


    excContext->ICSR = (void *)Hwi_nvic.ICSR;
    excContext->MMFSR = (void *)(uint32_t)Hwi_nvic.MMFSR;
    excContext->BFSR = (void *)(uint32_t)Hwi_nvic.BFSR;
    excContext->UFSR = (void *)(uint32_t)Hwi_nvic.UFSR;
    excContext->HFSR = (void *)Hwi_nvic.HFSR;
    excContext->DFSR = (void *)Hwi_nvic.DFSR;
    excContext->MMAR = (void *)Hwi_nvic.MMAR;
    excContext->BFAR = (void *)Hwi_nvic.BFAR;
    excContext->AFSR = (void *)Hwi_nvic.AFSR;
}
/* LCOV_EXCL_STOP */

/*
 *  ======== Hwi_excNmi ========
 */
/* LCOV_EXCL_START */
void Hwi_excNmi(unsigned int *excStack)
{
    Error_Block eb;
    Error_init(&eb);

    Error_raise(&eb, Hwi_E_NMI, NULL, 0);
}
/* LCOV_EXCL_STOP */

/*
 *  ======== Hwi_excHardFault ========
 */
/* LCOV_EXCL_START */
void Hwi_excHardFault(unsigned int *excStack)
{
    const char *fault;
    Error_Block eb;
    Error_init(&eb);

    if ((Hwi_nvic.HFSR & 0x40000000U) != 0U) {
        Error_raise(&eb, Hwi_E_hardFault, "FORCED", 0);
        Hwi_excUsageFault(excStack);
        Hwi_excBusFault(excStack);
        Hwi_excMemFault(excStack);
        return;
    }
    else if ((Hwi_nvic.HFSR & 0x80000000U) != 0U) {
        Error_raise(&eb, Hwi_E_hardFault, "DEBUGEVT", 0);
        Hwi_excDebugMon(excStack);
        return;
    }
    else if ((Hwi_nvic.HFSR & 0x00000002U) != 0U) {
        fault = "VECTBL";
    }
    else {
        fault = "Unknown";
    }
    Error_raise(&eb, Hwi_E_hardFault, fault, 0);
}
/* LCOV_EXCL_STOP */

/*
 *  ======== Hwi_excMemFault ========
 */
/* LCOV_EXCL_START */
void Hwi_excMemFault(unsigned int *excStack)
{
    const char *fault;
    Error_Block eb;
    Error_init(&eb);

    if (Hwi_nvic.MMFSR != 0U) {
        if ((Hwi_nvic.MMFSR & 0x10U) != 0U) {
            fault = "MSTKERR: Stacking Error (RD/WR failed), Stack Push";
        }
        else if ((Hwi_nvic.MMFSR & 0x08U) != 0U) {
            fault = "MUNSTKERR: Unstacking Error (RD/WR failed), Stack Pop";
        }
        else if ((Hwi_nvic.MMFSR & 0x02U) != 0U) {
            fault = "DACCVIOL: Data Access Violation (RD/WR failed)";
        }
        else if ((Hwi_nvic.MMFSR & 0x01U) != 0U) {
            fault = "IACCVIOL: Instruction Access Violation";
        }
        else {
            fault = "Unknown";
        }
        Error_raise(&eb, Hwi_E_memFault, fault, Hwi_nvic.MMAR);
    }
}
/* LCOV_EXCL_STOP */

/*
 *  ======== Hwi_excBusFault ========
 */
/* LCOV_EXCL_START */
void Hwi_excBusFault(unsigned int *excStack)
{
    const char *fault;
    Error_Block eb;
    Error_init(&eb);

    if (Hwi_nvic.BFSR != 0U) {
        if ((Hwi_nvic.BFSR & 0x10U) != 0U) {
            fault = "STKERR: Bus Fault caused by Stack Push";
        }
        else if ((Hwi_nvic.BFSR & 0x08U) != 0U) {
            fault = "UNSTKERR: Bus Fault caused by Stack Pop";
        }
        else if ((Hwi_nvic.BFSR & 0x04U) != 0U) {
            fault = "IMPRECISERR: Delayed Bus Fault, exact addr unknown";
        }
        else if ((Hwi_nvic.BFSR & 0x02U) != 0U) {
            fault = "PRECISERR: Immediate Bus Fault, exact addr known";
        }
        else if ((Hwi_nvic.BFSR & 0x01U) != 0U) {
            fault = "IBUSERR: Instruction Access Violation";
        }
        else {
            fault = "Unknown";
        }
        Error_raise(&eb, Hwi_E_busFault, fault, Hwi_nvic.BFAR);
    }
}
/* LCOV_EXCL_STOP */

/*
 *  ======== Hwi_excUsageFault ========
 */
/* LCOV_EXCL_START */
void Hwi_excUsageFault(unsigned int *excStack)
{
    const char *fault;
    Error_Block eb;
    Error_init(&eb);

    if (Hwi_nvic.UFSR != 0U) {
        if ((Hwi_nvic.UFSR & 0x0001U) != 0U) {
            fault = "UNDEFINSTR: Undefined instruction";
        }
        else if ((Hwi_nvic.UFSR & 0x0002U) != 0U) {
            fault = "INVSTATE: Invalid EPSR and instruction combination";
        }
        else if ((Hwi_nvic.UFSR & 0x0004U) != 0U) {
            fault = "INVPC: Invalid PC";
        }
        else if ((Hwi_nvic.UFSR & 0x0008U) != 0U) {
            fault = "NOCP: Attempting to use co-processor";
        }
        else if ((Hwi_nvic.UFSR & 0x0010U) != 0U) {
            fault = "STKOF: Stack overflow error has occurred";
        }
        else if ((Hwi_nvic.UFSR & 0x0100U) != 0U) {
            fault = "UNALIGNED: Unaligned memory access";
        }
        else if ((Hwi_nvic.UFSR & 0x0200U) != 0U) {
            fault = "DIVBYZERO";
        }
        else {
            fault = "Unknown";
        }
        Error_raise(&eb, Hwi_E_usageFault, fault, 0);
    }
}
/* LCOV_EXCL_STOP */

/*
 *  ======== Hwi_excSvCall ========
 */
/* LCOV_EXCL_START */
void Hwi_excSvCall(unsigned int *excStack)
{
    uint8_t *pc;
    Error_Block eb;

    Error_init(&eb);

    pc = (uint8_t *)excStack[14];

    Error_raise(&eb, Hwi_E_svCall, pc[-2], 0);
}
/* LCOV_EXCL_STOP */

/*
 *  ======== Hwi_excDebugMon ========
 */
/* LCOV_EXCL_START */
void Hwi_excDebugMon(unsigned int *excStack)
{
    const char *fault;
    Error_Block eb;
    Error_init(&eb);

    if (Hwi_nvic.DFSR != 0U) {
        if ((Hwi_nvic.DFSR & 0x00000010U) != 0U) {
            fault = "EXTERNAL";
        }
        else if ((Hwi_nvic.DFSR & 0x00000008U) != 0U) {
            fault = "VCATCH";
        }
        else if ((Hwi_nvic.DFSR & 0x00000004U) != 0U) {
            fault = "DWTTRAP";
        }
        else if ((Hwi_nvic.DFSR & 0x00000002U) != 0U) {
            fault = "BKPT";
        }
        else if ((Hwi_nvic.DFSR & 0x00000001U) != 0U) {
            fault = "HALTED";
        }
        else {
            fault = "Unknown";
        }
        Error_raise(&eb, Hwi_E_debugMon, fault, 0);
    }
}
/* LCOV_EXCL_STOP */

/*
 *  ======== Hwi_excReserved ========
 */
/* LCOV_EXCL_START */
void Hwi_excReserved(unsigned int *excStack, unsigned int excNum)
{
    Error_Block eb;
    Error_init(&eb);

    Error_raise(&eb, Hwi_E_reserved, "Exception #:", excNum);
}
/* LCOV_EXCL_STOP */

/*
 *  ======== Hwi_excNoIsr ========
 */
/* LCOV_EXCL_START */
void Hwi_excNoIsr(unsigned int *excStack, unsigned int excNum)
{
    Error_Block eb;
    Error_init(&eb);

    Error_raise(&eb, Hwi_E_noIsr, excNum, excStack[14]);
}
/* LCOV_EXCL_STOP */

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif

#if defined(__GNUC__)
#pragma GCC diagnostic warning "-Wint-to-pointer-cast"
#endif

/*
 *  ======== Hwi_excDumpRegs ========
 */
/* LCOV_EXCL_START */
void Hwi_excDumpRegs(unsigned int lr)
{
    Hwi_ExcContext *excp;
    char *ttype;
    char *name = "Unknown";

    excp = Hwi_module->excContext;

    switch (lr) {
        case 0xfffffff1U:
            System_printf("Exception occurred in ISR thread at PC = 0x%08x.\n", excp->pc);
            break;
        case 0xfffffff9U:
        case 0xfffffffdU:
            System_printf("Exception occurred in background thread at PC = 0x%08x.\n", excp->pc);
            break;
        default:
            System_printf("Bogus Exception return value: %08x.\n", lr);
            break;
    }

    switch (excp->threadType) {
        case BIOS_ThreadType_Task: {
            ttype = "Task";
            if (BIOS_taskEnabled != false) {
                name = Task_Handle_name(excp->threadHandle);
            }
            break;
        }
        case BIOS_ThreadType_Swi: {
            ttype = "Swi";
            if (BIOS_swiEnabled != false) {
                name = Swi_Handle_name(excp->threadHandle);
            }
            break;
        }
        case BIOS_ThreadType_Hwi: {
            ttype = "Hwi";
            break;
        }
        case BIOS_ThreadType_Main: {
            ttype = "Main";
            name = "main()";
            break;
        }
        default:
            ttype = "Unknown";
            break;
    }

    System_printf("Exception occurred in ThreadType_%s.\n", ttype);

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
/* LCOV_EXCL_STOP */

/*
 *  ======== Hwi_swiDisableNull ========
 */
unsigned int Hwi_swiDisableNull()
{
    return (0);
}

/*
 *  ======== Hwi_swiRestoreNull ========
 */
void Hwi_swiRestoreNull(unsigned int key)
{
}

/*
 *  ======== Hwi_taskDisableNull ========
 */
unsigned int Hwi_taskDisableNull()
{
    return (0);
}

/*
 *  ======== Hwi_taskRestoreHwiNull ========
 */
void Hwi_taskRestoreHwiNull(unsigned int key)
{
}


/*
 *  ======== Hwi_swiDisable ========
 */
unsigned int Hwi_swiDisable()
{
    return (Hwi_swiDisable_D());
}

/*
 *  ======== Hwi_swiRestore ========
 */
void Hwi_swiRestore(unsigned int key)
{
    Hwi_swiRestore_D(key);
}

/*
 *  ======== Hwi_swiRestoreHwi ========
 */
void Hwi_swiRestoreHwi(unsigned int key)
{
    Hwi_swiRestoreHwi_D(key);
}

/*
 *  ======== Hwi_taskDisable ========
 */
unsigned int Hwi_taskDisable()
{
    return (Hwi_taskDisable_D());
}

/*
 *  ======== Hwi_taskRestoreHwi ========
 */
void Hwi_taskRestoreHwi(unsigned int key)
{
    Hwi_taskRestoreHwi_D(key);
}

/*
 *  ======== Hwi_dispatchC ========
 *  "Top Half" of Configurable IRQ interrupt dispatcher.
 */
unsigned int Hwi_dispatchC(uintptr_t irp, uint32_t dummy1, uint32_t dummy2, Hwi_Object *hwi)
{
    /*
     * Enough room is reserved above the isr stack to handle
     * as many as 16 32-bit stack resident local variables.
     * If the dispatcher requires more than this, you must
     * handle this in Hwi_Module_startup().
     */

    BIOS_ThreadType prevThreadType;
    unsigned int tskKey = 1;
    unsigned int swiKey = 1;
#if defined(Hwi_ENABLE_BEGIN_HOOKS) || defined(Hwi_ENABLE_END_HOOKS)
    int i;
#endif

    Hwi_disable();

    if (Hwi_dispatcherTaskSupport != false) {
        tskKey = Hwi_taskDisable_D();
    }

    if (Hwi_dispatcherSwiSupport != false) {
        swiKey = Hwi_swiDisable_D();
    }

    /* set thread type to Hwi */
    prevThreadType = BIOS_setThreadType(BIOS_ThreadType_Hwi);

    if (Hwi_numSparseInterrupts == 0U) {
        unsigned int intNum = (Hwi_nvic.ICSR & 0x000000ffU);
        Hwi_Object **dispatchTable = (Hwi_Object **)Hwi_module->dispatchTable;
        hwi = dispatchTable[intNum];
    }

    /* IRP tracking is always enabled for M3 */
    hwi->irp = irp;

#ifdef Hwi_ENABLE_BEGIN_HOOKS
    /* call the begin hooks */
    for (i = 0; i < Hwi_hooks.length; i++) {
        if (Hwi_hooks.elem[i].beginFxn != NULL) {
            Hwi_hooks.elem[i].beginFxn(hwi);
        }
    }
#endif

    Log_write(Log_INFO1, Hwi_beginLogEvt, hwi, prevThreadType, hwi->intNum,
        hwi->irp);

    /* call the user's isr */
    if (Hwi_dispatcherAutoNestingSupport != false) {
        Hwi_enable();
        (hwi->fxn)(hwi->arg);
        Hwi_disable();
    }
    else {
        (hwi->fxn)(hwi->arg);
    }

    Log_write(Log_INFO2, Hwi_endLogEvt, hwi);

#ifdef Hwi_ENABLE_END_HOOKS
    /* call the end hooks */
    for (i = 0; i < Hwi_hooks.length; i++) {
        if (Hwi_hooks.elem[i].endFxn != NULL) {
            Hwi_hooks.elem[i].endFxn(hwi);
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
void Hwi_doSwiRestore(unsigned int swiTskKey)
{
    /* Run Swi scheduler */
    Hwi_swiRestoreHwi_D(swiTskKey & 0xffU); /* Run Swi scheduler */
}

/*
 *  ======== Hwi_doTaskRestore ========
 *  Run task scheduler. (Executes on Task stack)
 */
void Hwi_doTaskRestore(unsigned int swiTskKey)
{
    /* Run Task scheduler */
    Hwi_taskRestoreHwi_D(swiTskKey >> 8);   /* returns with ints disabled */
}

/*
 *  ======== Hwi_testStaticInlines ========
 */
void Hwi_testStaticInlines()
{
    Hwi_Params hwiParams;

    Hwi_Params_init(NULL);
    Hwi_Params_init(&hwiParams);
}

/*
 *  ======== Hwi_initStack ========
 *  Initialize the Common Interrupt Stack
 */
void Hwi_initStack(void)
{
    Hwi_StackInfo stkInfo;
    uintptr_t curStack;

    /* Get stack base and size */
    Hwi_getStackInfo(&stkInfo, false);

    curStack = (uintptr_t)(stkInfo.hwiStackBase);

#if Hwi_initStackFlag_D
    while (curStack < (uintptr_t)(&curStack)) {
        *((volatile uint8_t *)curStack) = 0xbe;
        curStack++;
    }
#else
    *((volatile uint8_t *)curStack) = 0xbe;
#endif
}

/*
 *  ======== Hwi_checkStack ========
 *  Check the Common Interrupt Stack for overflow.
 */
void Hwi_checkStack(void)
{
    bool overflow;
    Hwi_StackInfo stkInfo;

    overflow = Hwi_getStackInfo(&stkInfo, false);

    if (overflow != false) {
        Error_raise(NULL, Hwi_E_stackOverflow, 0, 0);
    }
}

/*
 *  ======== Hwi_Params_init ========
 */
void Hwi_Params_init(Hwi_Params *params)
{
    *params = Hwi_Params_default;
}

/*
 *  ======== Hwi_Params_copy ========
 */
void Hwi_Params_copy(Hwi_Params *dst, Hwi_Params *src)
{
    if (dst != NULL) {
        *dst = *src;
    }
}

