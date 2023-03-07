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

/*
 *  ======== Swi.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Intrinsics.h>
#include <ti/sysbios/knl/Swi.h>

#include <ti/sysbios/runtime/Assert.h>
#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/Memory.h>
#include <ti/sysbios/runtime/Startup.h>
#include <ti/sysbios/runtime/Types.h>

#if BIOS_taskEnabled__D
#define TASK_DISABLE Task_disable
#define TASK_RESTORE Task_restore
#else
#define TASK_DISABLE (Swi_taskDisable)
#define TASK_RESTORE (Swi_taskRestore)
#endif

#if defined(__ti__) && !defined(__clang__)
/* disable unused local variable warning during optimized compile */
#pragma diag_suppress=179
#endif

static const Swi_Params Swi_Params_default = {
    .name = NULL,
    .priority = (Swi_numPriorities_D-1),
};

Queue_Elem Swi_readyQs[Swi_numPriorities_D];

Swi_Module_State Swi_Module_state = {
    .objQ.next = &Swi_Module_state.objQ,
    .objQ.prev = &Swi_Module_state.objQ,
    .locked = 1,
    .readyQ = Swi_readyQs,
    .initDone = 0
};

const Swi_Hook Swi_hooks = {
    .length = SwiHooks_numHooks_D,
    .elem = SwiHooks_array
};

/* numPriorities */
const unsigned int Swi_numPriorities = Swi_numPriorities_D;

#if 0
/*
 *  ======== instrumentation ========
 *  Must define these macros before including Log.h
 */
#undef Log_moduleName
#define Log_moduleName ti_sysbios_knl_Swi

#ifdef Swi_log_D
#define ti_utils_runtime_Log_ENABLE_PPO
#ifdef Swi_logInfo1_D
#define ti_utils_runtime_Log_ENABLE_INFO1
#endif
#ifdef Swi_logInfo2_D
#define ti_utils_runtime_Log_ENABLE_INFO2
#endif
#ifdef Swi_logInfo3_D
#define ti_utils_runtime_Log_ENABLE_INFO3
#endif
#endif

#include <ti/utils/runtime/Log.h>

Log_EVENT(Swi_beginLogEvt, Log_Type_SwiBegin,
    "swi begin: swi: 0x%x, func: 0x%x, preThread: %d");
Log_EVENT(Swi_endLogEvt, Log_Type_SwiEnd, "swi end: swi: 0x%x");
Log_EVENT(Swi_postLogEvt, Log_Type_SwiPost,
    "swi post: swi: 0x%x, func: 0x%x, pri: %d");
#else
#define Log_write(module, level, ...)
#endif

/*
 *************************************************************************
 *                       Internal functions
 *************************************************************************
 */

/*
 *  ======== Swi_run ========
 *  Set up and run Swi.
 *  Enter with Hwi's disabled
 *  Calls Swi function with interrupts enabled
 *  Exits with Hwi's enabled
 *  When no Swis are running, curQ is NULL
 */
void Swi_run(Swi_Object *swi)
{
    unsigned int saved_curTrigger = Swi_module->curTrigger;
    Swi_Object *saved_curSwi = (Swi_Object *)Swi_module->curSwi;
    Queue_Handle saved_curQ = Swi_module->curQ;
    BIOS_ThreadType prevThreadType;
#if defined(Swi_ENABLE_BEGIN_HOOKS) || defined(Swi_ENABLE_END_HOOKS)
    int i;
#endif

    Swi_module->curQ = swi->readyQ;
    Swi_module->curSwi = swi;
    Swi_module->curTrigger = swi->trigger;

    swi->trigger = swi->initTrigger;
    swi->posted = false;

    Swi_module->locked = false; /* unlock the scheduler while */
                                /* Swi is running  */

    /* set thread type to Swi */
    prevThreadType = BIOS_setThreadType(BIOS_ThreadType_Swi);

    (void)Hwi_enable();

#ifdef Swi_ENABLE_BEGIN_HOOKS
    for (i = 0; i < Swi_hooks.length; i++) {
        if (Swi_hooks.elem[i].beginFxn != NULL) {
            Swi_hooks.elem[i].beginFxn(swi);
        }
    }
#endif

    /* MISRA.CAST.FUNC_PTR.2012 MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write(Log_INFO1, Swi_beginLogEvt, swi, swi->fxn, prevThreadType);

    (swi->fxn)(swi->arg0, swi->arg1);

    /* MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write(Log_INFO2, Swi_endLogEvt, swi);

#ifdef Swi_ENABLE_END_HOOKS
    for (i = 0; i < Swi_hooks.length; i++) {
        if (Swi_hooks.elem[i].endFxn != NULL) {
            Swi_hooks.elem[i].endFxn(swi);
        }
    }
#endif

    (void)Hwi_disable();

    /* restore thread type */
    (void)BIOS_setThreadType(prevThreadType);

    Swi_module->locked = true;       /* relock the scheduler */

    Swi_module->curQ = saved_curQ;
    Swi_module->curSwi = saved_curSwi;
    Swi_module->curTrigger = saved_curTrigger;
}

/*
 *  ======== Swi_runLoop ========
 */
void Swi_runLoop(void)
{
    Queue_Handle curQ, maxQ;
    Swi_Object *swi;

    /* Remember current Swi priority */
    curQ = Swi_module->curQ;

    /* refresh maxQ */
    maxQ = (Queue_Handle)((uint8_t *)(Swi_module->readyQ) +
           (Intrinsics_maxbit(Swi_module->curSet)*(2U*sizeof(void *))));

    /*
     * Run all Swis of higher priority than
     * the current Swi priority.
     * Will pre-empt any currently running swi.
     */
    do {
        swi = (Swi_Object *)Queue_dequeue(maxQ);

        /* remove from curSet if last one in this ready list */
        if (Queue_empty(maxQ)) {
            Swi_module->curSet &= ~swi->mask;
        }

        Swi_run(swi);

        if (Swi_module->curSet == 0U) {
            break;
        }

        maxQ = (Queue_Handle)((uint8_t *)(Swi_module->readyQ) +
               (Intrinsics_maxbit(Swi_module->curSet)*(2U*sizeof(void *))));
    }
    while (maxQ > curQ);
}

/*
 *  ======== Swi_schedule ========
 *  Run all ready (posted) Swis of higher priority than the currently
 *  running Swi.
 *  Invoked from Task and Swi threads.
 *  Called by Swi_startup(), Swi_restore() and Swi_post().
 *  Entered with Swi_module->locked = true;
 *  Exits with Swi_module->locked = false;
 */

void Swi_schedule(void)
{
    Queue_Handle curQ, maxQ;
    unsigned int hwiKey;
    unsigned int tskKey;

    /* Remember current Swi priority */
    curQ = Swi_module->curQ;

    hwiKey = Hwi_disable();     /* required for Swi's posted from Tasks */

    /* determine current highest priority Q */
    maxQ = (Queue_Handle)((uint8_t *)(Swi_module->readyQ) +
           (Intrinsics_maxbit(Swi_module->curSet)*(2U*sizeof(void *))));

    if (maxQ > curQ) {
        if (BIOS_taskEnabled) {
            tskKey = TASK_DISABLE();  /* required for Swi's posted from Tasks */

            /* Switch stacks and call Swi_runLoop() */
            Hwi_switchAndRunFunc(&Swi_runLoop);
        }
        else {
            /* Call Swi_runLoop() */
            /* UNREACH.GEN */
            Swi_runLoop();
        }

        /* MUST(!) unlock the scheduler before enabling interrupts */
        Swi_module->locked = false;

        Hwi_restore(hwiKey);

        if (BIOS_taskEnabled) {
            TASK_RESTORE(tskKey);
        }
    }
    else {
        Swi_module->locked = false;
        Hwi_restore(hwiKey);
    }
}

/*
 *************************************************************************
 *                      Module wide functions
 *************************************************************************
 */

/*
 *  ======== Swi_init ========
 */
void Swi_init(void)
{
    /* Interrupts are disabled at this point */
    if (Swi_module->initDone) {
        return;
    }
    Swi_module->initDone = true;

    /*
     * Need not wait for anybody because Swi and task do no work
     * before BIOS_start(). Assuming hooks are OK.
     */
    if (BIOS_swiEnabled) {  /* minimize code foot print if Swi is disabled */
        int i;

#ifdef Swi_ENABLE_REGISTER_HOOKS
        for (i = 0; i < Swi_hooks.length; i++) {
            if (Swi_hooks.elem[i].registerFxn != NULL) {
                Swi_hooks.elem[i].registerFxn(i);
            }
        }
#endif
        /* initialize ReadyQs */
        for (i = 0; i < Swi_numPriorities; i++) {
            Queue_construct(&Swi_module->readyQ[i], NULL);
        }
    }
}

/*
 *  ======== Swi_startup ========
 */
void Swi_startup(void)
{
    Swi_restore(false);
}

/*
 *  ======== Swi_enabled ========
 */
bool Swi_enabled(void)
{
    return (bool)(Swi_module->locked == false);
}

/*
 *  ======== Swi_disable ========
 */
unsigned int Swi_disable(void)
{
    unsigned int key = (unsigned int)Swi_module->locked;

    Swi_module->locked = true;

    return (key);
}

/*
 *  ======== Swi_enable ========
 */
void Swi_enable(void)
{
    Swi_restore(false);
}

/*
 *  ======== Swi_restore ========
 */
void Swi_restore(unsigned int swiKey)
{
    unsigned int hwiKey;

    if (swiKey == false) {
        hwiKey = Hwi_disable();

        if (Swi_module->curSet) {
            Hwi_restore(hwiKey);
            Swi_schedule();     /* sets locked to false */
        }
        else {
            Swi_module->locked = false;
            Hwi_restore(hwiKey);
        }

        if (BIOS_taskEnabled) {
            /* run task scheduler if its unlocked */
            TASK_RESTORE(TASK_DISABLE());
        }
    }
}

/*
 *  ======== Swi_restoreHwi ========
 *  Restore Swi scheduler state.
 *
 *  Called by interrupt dispatcher.
 *  Optimized for running within dispatcher context.
 *  No Swi_taskDisable/restore, Hwi_disable/restore,
 *  SwitchToIsrStack/SwitchToTaskStack.
 *
 *  Assumes Hwis and Tasks already disabled and currently
 *  running on ISR stack.
 *
 *  if restored to unlocked state and there are Swis ready
 *  to run, then run all ready (posted) Swis of higher
 *  priority than the currently running Swi.
 *
 *  Entered with ints and Tasks disabled.
 */

void Swi_restoreHwi(unsigned int swiKey)
{
    Queue_Handle curQ, maxQ;
    Swi_Object *swi;

    if (swiKey == false) {
        if (Swi_module->curSet) {
            /* latency reduction */
            (void)Hwi_enable();
            (void)Hwi_disable();

            /* Remember current Swi priority */
            curQ = Swi_module->curQ;

            /* determine current highest priority Q */
            maxQ = (Queue_Handle)((uint8_t *)(Swi_module->readyQ) +
                   (Intrinsics_maxbit(Swi_module->curSet) * (2U*sizeof(void *))));
            /*
             * Run all Swis of higher priority than the current Swi priority
             * Will pre-empt any currently running swi.
             */
            while (maxQ > curQ) {
                swi = (Swi_Object *)Queue_dequeue(maxQ);

                /* remove from curSet if last one in this ready list */
                if (Queue_empty(maxQ)) {
                    Swi_module->curSet &= ~swi->mask;
                }

                Swi_run(swi);

                if (Swi_module->curSet == 0U) {
                    break;
                }
                maxQ = (Queue_Handle)((uint8_t *)(Swi_module->readyQ) +
                       (Intrinsics_maxbit(Swi_module->curSet) *
                        (2U*sizeof(void *))));
            }
        }
        Swi_module->locked = false;
    }
}

/*
 *  ======== Swi_self ========
 */
Swi_Handle Swi_self(void)
{
    return (Swi_module->curSwi);
}


/*
 *  ======== Swi_getTrigger ========
 */
unsigned int Swi_getTrigger(void)
{
    return (Swi_module->curTrigger);
}

/*
 *  ======== Swi_raisePri ========
 *  !!! Warning, while raisePri is in effect, the !!!
 *  !!! current Swi's priority and mask don't     !!!
 *  !!! match curQ.                               !!!
 */
unsigned int Swi_raisePri(unsigned int priority)
{
    unsigned int hwiKey;

    Queue_Handle oldCurQ, newCurQ;

    hwiKey = Hwi_disable();

    oldCurQ = Swi_module->curQ;
    newCurQ = &Swi_module->readyQ[priority];
    Swi_module->curQ = newCurQ > oldCurQ ? newCurQ : oldCurQ;

    Hwi_restore(hwiKey);

    return (Swi_module->curSwi->priority);
}

/*
 *  ======== Swi_restorePri ========
 */
void Swi_restorePri(unsigned int priority)
{
    unsigned int swiKey;

    swiKey = Swi_disable();

    Swi_module->curQ = &Swi_module->readyQ[priority];

    Swi_restore(swiKey);
}

/*
 *************************************************************************
 *                       Instance functions
 *************************************************************************
 */

/*
 *  ======== Swi_construct2 ========
 */
Swi_Handle Swi_construct2(Swi_Struct2 *swiStruct2, Swi_FuncPtr swiFxn,
        const Swi_Params *prms)
{
    Swi_Handle   swi;

    Swi_construct(swiStruct2, swiFxn, prms, Error_IGNORE);
    if (swiStruct2->fxn == NULL) {
        swi = (Swi_Handle)NULL;
    }
    else {
        swi = swiStruct2;
    }

    return (swi);
}

/*
 *  ======== Swi_Instance_init ========
 */
int Swi_Instance_init(Swi_Object *swi, Swi_FuncPtr fxn,
                        const Swi_Params *params, Error_Block *eb)
{
    int status;

    Assert_isTrue((BIOS_swiEnabled == true), Swi_A_swiDisabled);

    Queue_elemClear(&swi->qElem);

    swi->name = params->name;
    swi->fxn = fxn;
    swi->arg0 = params->arg0;
    swi->arg1 = params->arg1;

    if (params->priority == ~(0U)) {
        swi->priority = Swi_numPriorities - 1U;
    }
    else {
        swi->priority = params->priority;
    }

    Assert_isTrue((swi->priority < Swi_numPriorities),
                   Swi_A_badPriority);

    swi->mask = (unsigned int)1 << swi->priority;
    swi->posted = false;
    swi->trigger = params->trigger;
    swi->initTrigger = params->trigger;
    swi->readyQ = &Swi_module->readyQ[(int)swi->priority];

#ifdef Swi_ENABLE_HOOKS
    if (Swi_hooks.length > 0) {
        swi->hookEnv = (void * *)Memory_alloc(Swi_Object_heap(),
                (unsigned int)Swi_hooks.length * sizeof(void *), 0, eb);

        if (swi->hookEnv == NULL) {
            /*
             *  Set swi->fxn to NULL to allow Swi_construct2() to
             *  determine that a failure occurred.
             */
            swi->fxn = NULL;

            return (1);   /* see 2 below */
        }
    }
#endif

    status = Swi_postInit(swi, eb);

    /*
     * floor of 2 here is to differentiate Swi_postInit errors
     * from Instance_init errors
     */
    if (status != 0) {
        /* UNREACH.GEN */
        return (2 + status);
    }

    /* put swi on global swi list (Queue_put is atomic) */
    Queue_put(&Swi_module->objQ, &swi->objElem);

    return (0);
}

/*
 *  ======== Swi_postInit ========
 *  Function to be called during module startup to complete the
 *  initialization of any statically created or constructed Swi.
 *
 *  returns (0) and clean 'eb' on success
 *  returns 'eb' *and* 'n' for number of successful createFxn() calls iff
 *      one of the createFxn() calls fails
 */
int Swi_postInit (Swi_Object *swi, Error_Block *eb)
{
#ifdef Swi_ENABLE_CREATE_HOOKS
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

    for (i = 0; i < Swi_hooks.length; i++) {
        swi->hookEnv[i] = (void *)0;
        if (Swi_hooks.elem[i].createFxn != NULL) {
            Swi_hooks.elem[i].createFxn(swi, leb);

            if (Error_check(leb)) {
                /*
                 *  Set swi->fxn to NULL to allow Swi_construct2() to
                 *  determine that a failure occurred.  We do this inside
                 *  the Swi_DISABLE_ALL_HOOKS block instead of Instance_init()
                 *  since this code will not go in ROM.
                 */
                swi->fxn = NULL;

                return (i + 1);
            }
        }
    }
#endif

    return (0);
}

/*
 *  ======== Swi_Instance_finalize ========
 *  free hookEnv if alloced during create
 */
void Swi_Instance_finalize(Swi_Object *swi, int status)
{
#ifdef Swi_ENABLE_DELETE_HOOKS
    int i, cnt;
#endif

    if (status == 1) {
        return;
    }

    /* Can't delete a currently posted Swi */
    Assert_isTrue((swi->posted == 0U), NULL);

#ifdef Swi_ENABLE_HOOKS
    if (Swi_hooks.length > 0) {
#ifdef Swi_ENABLE_DELETE_HOOKS
        if (status == 0) {
            cnt = Swi_hooks.length;
        }
        else {
            cnt = status - 2;
        }

        for (i = 0; i < cnt; i++) {
            if (Swi_hooks.elem[i].deleteFxn != NULL) {
                Swi_hooks.elem[i].deleteFxn(swi);
            }
        }
#endif
        Memory_free(Swi_Object_heap(), swi->hookEnv,
                (unsigned int)Swi_hooks.length * sizeof (void *));
    }
#endif
}

/*
 *  ======== Swi_post ========
 */
void Swi_post(Swi_Object *swi)
{
    unsigned int hwiKey;
    unsigned int swiKey;
#ifdef Swi_ENABLE_READY_HOOKS
    int i;
#endif

    /* MISRA.CAST.FUNC_PTR.2012 MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write(Log_INFO3, Swi_postLogEvt, swi, swi->fxn, swi->priority);

    hwiKey = Hwi_disable();

    if (swi->posted) {
        Hwi_restore(hwiKey);
        return;
    }

    swi->posted = true;

    swiKey = Swi_disable();

    /* Place the Swi in the appropriate ready Queue. */
    Queue_enqueue(swi->readyQ, &swi->qElem);

    /* Add this priority to current set */
    Swi_module->curSet |= swi->mask;

    Hwi_restore(hwiKey);  /* Not in FIFO order. OK! */

#ifdef Swi_ENABLE_READY_HOOKS
    /*
     * This hook location supports the STS "set ready time" operation on
     * Swis.  This is equivalent to pre-BIOS 6.
     */
    for (i = 0; i < Swi_hooks.length; i++) {
        if (Swi_hooks.elem[i].readyFxn != NULL) {
            Swi_hooks.elem[i].readyFxn(swi);
        }
    }
#endif

    /*
     * Modified Swi_restore().
     * No need to check curSet since we just set it.
     */
    if (swiKey == false) {
        Swi_schedule(); /* unlocks Swi scheduler on return */
    }
}

/*
 *  ======== Swi_getHookContext ========
 */
void * Swi_getHookContext(Swi_Object *swi, int id)
{
    return swi->hookEnv[id];
}

/*
 *  ======== Swi_setHookContext ========
 */
void Swi_setHookContext(Swi_Object *swi, int id, void * hookContext)
{
    swi->hookEnv[id] = hookContext;
}

/*
 *  ======== Swi_getName ========
 */
char * Swi_getName(Swi_Object *swi)
{
    return (swi->name);
}

/*
 *  ======== Swi_getPri ========
 */
unsigned int Swi_getPri(Swi_Object *swi)
{
    return (swi->priority);
}

/*
 *  ======== Swi_FuncPtr ========
 */
Swi_FuncPtr Swi_getFunc(Swi_Object *swi, uintptr_t *arg0, uintptr_t *arg1)
{
    if (arg0 != NULL) {
        *arg0 = swi->arg0;
    }

    if (arg1 != NULL) {
        *arg1 = swi->arg1;
    }

    return (swi->fxn);
}

/*
 *  ======== Swi_inc ========
 */
void Swi_inc(Swi_Object *swi)
{
    unsigned int hwiKey;

    hwiKey = Hwi_disable();
    swi->trigger += 1U;
    Hwi_restore(hwiKey);
    Swi_post(swi);
}

/*
 *  ======== Swi_or ========
 */
void Swi_or(Swi_Object *swi, unsigned int mask)
{
    unsigned int hwiKey;

    hwiKey = Hwi_disable();
    swi->trigger |= mask;
    Hwi_restore(hwiKey);
    Swi_post(swi);
}

/*!
 *  ======== Swi_andn ========
 */
void Swi_andn(Swi_Object *swi, unsigned int mask)
{
    unsigned int hwiKey;

    hwiKey = Hwi_disable();

    if (swi->trigger != 0U) {
        swi->trigger &= ~mask;
        if (swi->trigger == 0U) {
            Hwi_restore(hwiKey);
            Swi_post(swi);
            return;
        }
    }

    Hwi_restore(hwiKey);
}

/*
 *  ======== Swi_dec ========
 */
void Swi_dec(Swi_Object *swi)
{
    unsigned int hwiKey;

    hwiKey = Hwi_disable();

    if (swi->trigger != 0U) {
        swi->trigger -= 1U;
        if (swi->trigger == 0U) {
            Hwi_restore(hwiKey);
            Swi_post(swi);
            return;
        }
    }

    Hwi_restore(hwiKey);
}

/*
 *  ======== Swi_setAttrs ========
 */
void Swi_setAttrs(Swi_Object *swi, Swi_FuncPtr fxn, Swi_Params *p)
{
    unsigned int hwiKey;
    Swi_Params swiParams;
    Swi_Params *params;

    params = p;
    if (p == NULL) {
        Swi_Params_init(&swiParams);
        params = &swiParams;
    }

    hwiKey = Hwi_disable();

    /* defensively remove swi from its readyQ */
    Queue_remove(&swi->qElem);

    if (fxn != NULL) {
        swi->fxn = fxn;
    }

    swi->posted = false;

    swi->arg0 = params->arg0;
    swi->arg1 = params->arg1;

    if (params->priority == ~(0U)) {
        swi->priority = Swi_numPriorities - 1U;
    }
    else {
        swi->priority = params->priority;
    }

    Assert_isTrue((swi->priority < Swi_numPriorities),
                   Swi_A_badPriority);

    swi->mask = (unsigned int)1 << swi->priority;
    swi->trigger = params->trigger;
    swi->initTrigger = params->trigger;
    swi->readyQ = &Swi_module->readyQ[swi->priority];

    Hwi_restore(hwiKey);
}

/*
 *  ======== Swi_setPri ========
 */
void Swi_setPri(Swi_Object *swi, unsigned int pri)
{
    unsigned int hwiKey;
    unsigned int priority = pri;

    if (priority == ~(0U)) {
        priority = Swi_numPriorities - 1U;
    }

    Assert_isTrue(priority < (unsigned int)Swi_numPriorities, Swi_A_badPriority);

    hwiKey = Hwi_disable();

    swi->priority = priority;
    swi->mask = (unsigned int)1 << priority;
    swi->readyQ = &Swi_module->readyQ[priority];

    Hwi_restore(hwiKey);
}

/*
 *  ======== Swi_getAttrs ========
 */
void Swi_getAttrs(Swi_Object *swi, Swi_FuncPtr *fxn, Swi_Params *params)
{
    unsigned int hwiKey;

    hwiKey = Hwi_disable();

    if (params != NULL) {
        params->arg0 = swi->arg0;
        params->arg1 = swi->arg1;
        params->priority = swi->priority;
        params->trigger = swi->initTrigger;
    }

    if (fxn != NULL) {
        *fxn = swi->fxn;
    }

    Hwi_restore(hwiKey);
}

Swi_Handle Swi_getSwiFromObjElem(Queue_Elem *swiQelem)
{
    if (swiQelem == (Queue_Elem *)&Swi_module->objQ) {
        return (NULL);
    }

    return ((Swi_Handle)((char *)swiQelem - offsetof(Swi_Struct, objElem)));
}

/* -------- The following functions were generated in BIOS 6.x -------- */

/*
 *  ======== Swi_Object_first ========
 */
Swi_Handle Swi_Object_first()
{
    return (Swi_getSwiFromObjElem(Queue_head(&(Swi_module->objQ))));
}

/*
 *  ======== Swi_Object_next ========
 */
Swi_Handle Swi_Object_next(Swi_Handle swi)
{
    return (Swi_getSwiFromObjElem(Queue_next(&swi->objElem)));
}

/*
 *  ======== Swi_Params_init ========
 */
void Swi_Params_init(Swi_Params *params)
{
    *params = Swi_Params_default;
}

/*
 *  ======== Swi_construct ========
 */
Swi_Handle Swi_construct(Swi_Struct *swi,
        Swi_FuncPtr swiFxn, const Swi_Params *prms, Error_Block *eb)
{
    int status;

    if (prms == NULL) {
        prms = &Swi_Params_default;
    }

    status = Swi_Instance_init(swi, swiFxn, prms, eb);

    if (status != 0) {
        Swi_Instance_finalize(swi, status);
        swi = NULL;
    }

    return (swi);
}

/*
 *  ======== Swi_create ========
 */
Swi_Handle Swi_create(
        Swi_FuncPtr swiFxn, const Swi_Params *prms, Error_Block *eb)
{
    Swi_Handle swi, status;

    swi = Memory_alloc(NULL, sizeof(Swi_Object), 0, eb);

    if (swi != NULL) {
        status = Swi_construct(swi, swiFxn, prms, eb);
        if (status == NULL) {
            Memory_free(NULL, swi, sizeof(Swi_Object));
            swi = NULL;
        }
    }

    return (swi);
}

/*
 *  ======== Swi_delete ========
 */
void Swi_delete(Swi_Handle *swi)
{
    Swi_destruct(*swi);

    Memory_free(NULL, *swi, sizeof(Swi_Object));

    *swi = NULL;
}

/*
 *  ======== Swi_destruct ========
 */
void Swi_destruct(Swi_Handle swi)
{
    unsigned int hwiKey;

    /* remove swi from global swi list (Queue_remove is not atomic) */
    hwiKey = Hwi_disable();
    Queue_remove(&swi->objElem);
    Hwi_restore(hwiKey);

    Swi_Instance_finalize(swi, 0);
}
