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
 *  ======== Task.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Idle.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Intrinsics.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/TaskSupport.h>

#include <ti/sysbios/runtime/Assert.h>
#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/Memory.h>
#include <ti/sysbios/runtime/Startup.h>
#include <ti/sysbios/runtime/Types.h>

Queue_Object Task_readyQs[Task_numPriorities_D];

static const Task_Params Task_Params_default = {
    .name = NULL,
    .arg0 = 0,
    .arg1 = 0,
    .priority = 1,
    .stack = NULL,
    .stackSize = Task_defaultStackSize_D,
    .vitalTaskFlag = true,
};

Task_Module_State Task_Module_state = {
    .objQ.next = &Task_Module_state.objQ,
    .objQ.prev = &Task_Module_state.objQ,
    .locked = 1,
    .curSet = 0x0,
    .workFlag = 0,
    .vitalTasks = Task_numVitalTasks_D,
    .curTask = 0,
    .curQ = 0,
    .readyQ = (Queue_Handle)Task_readyQs,
    .idleTask = NULL,
    .inactiveQ.next = &Task_Module_state.inactiveQ,
    .inactiveQ.prev = &Task_Module_state.inactiveQ,
    .terminatedQ.next = &Task_Module_state.terminatedQ,
    .terminatedQ.prev = &Task_Module_state.terminatedQ,
    .initDone = 0
};

const Task_Hook Task_hooks = {
    .length = TaskHooks_numHooks_D,
    .elem = TaskHooks_array
};

uint32_t Task_moduleStateCheckValue;
IHeap_Handle Task_defaultStackHeap;

const Task_AllBlockedFuncPtr Task_allBlockedFunc = Task_allBlockedFunc_D;
const Task_ModuleStateCheckFuncPtr Task_moduleStateCheckFxn = Task_moduleStateCheck;
const Task_ModuleStateCheckValueFuncPtr Task_moduleStateCheckValueFxn = Task_getModuleStateCheckValue;
const bool Task_moduleStateCheckFlag = 0;
const Task_ObjectCheckFuncPtr Task_objectCheckFxn = Task_objectCheck;
const Task_ObjectCheckValueFuncPtr Task_objectCheckValueFxn = Task_getObjectCheckValue;
const bool Task_objectCheckFlag = 0;

const unsigned int Task_numPriorities = Task_numPriorities_D;
const size_t Task_defaultStackSize = Task_defaultStackSize_D;
const bool Task_initStackFlag = Task_initStackFlag_D;
const bool Task_checkStackFlag = Task_checkStackFlag_D;
const bool Task_deleteTerminatedTasks = Task_deleteTerminatedTasks_D;

#if 0
/*
 *  ======== instrumentation ========
 *  Must define these macros before including Log.h
 */
#undef Log_moduleName
#define Log_moduleName ti_sysbios_knl_Task

#ifdef Task_log_D
#define ti_utils_runtime_Log_ENABLE_PPO
#ifdef Task_logInfo1_D
#define ti_utils_runtime_Log_ENABLE_INFO1
#endif
#ifdef Task_logInfo2_D
#define ti_utils_runtime_Log_ENABLE_INFO2
#endif
#ifdef Task_logInfo3_D
#define ti_utils_runtime_Log_ENABLE_INFO3
#endif
#endif

#include <ti/utils/runtime/Log.h>

Log_EVENT(Task_switchLogEvt, Log_Type_TaskSwitch,
    "task switch: prev tsk: 0x%x func: 0x%x, current tsk: 0x%x func: 0x%x");
Log_EVENT(Task_sleepLogEvt, Log_Type_TaskBlock,
    "task sleep: tsk: 0x%x, func: 0x%x, timeout: %d");
Log_EVENT(Task_readyLogEvt, Log_Type_TaskReady,
    "task ready: tsk: 0x%x, func: 0x%x, pri: %d");
Log_EVENT(Task_blockLogEvt, Log_Type_TaskBlock,
    "task block: tsk: 0x%x, func: 0x%x");
Log_EVENT(Task_yieldLogEvt, Log_Type_Data,
    "task yield: tsk: 0x%x, func: 0x%x, thread type: %d");
Log_EVENT(Task_setPriLogEvt, Log_Type_Data,
    "task setPri: tsk: 0x%x, func: 0x%x, oldPri: %d, newPri %d");
Log_EVENT(Task_exitLogEvt, Log_Type_Data,
    "task exit: tsk: 0x%x, func: 0x%x");
#else
#define Log_write(module, level, ...)
#endif

/*
 *  ======== Task_schedule ========
 *  Find highest priority task and invoke it.
 *
 *  Must be called with interrupts disabled.
 */
/* REQ_TAG(SYSBIOS-456), REQ_TAG(SYSBIOS-570) */
void Task_schedule(void)
{
    Queue_Handle maxQ;
    Task_Object *prevTask;
    Task_Object *curTask;
#ifdef Task_ENABLE_SWITCH_HOOKS
    int i;
#endif

    do {
        Task_module->workFlag = 0;

        /* stall until a task is ready */
        while (Task_module->curSet == 0U) {
            Task_allBlockedFunction();
        }

        /* Determine current max ready Task priority */
        maxQ = (Queue_Handle)((uint8_t *)(Task_module->readyQ) +
               (Intrinsics_maxbit(Task_module->curSet)*(2U*sizeof(void *))));

        /* if a higher priority task is ready - switch to it */
        if (maxQ > Task_module->curQ) {
            prevTask = Task_module->curTask;
            Task_module->curQ = maxQ;
            Task_module->curTask = (Task_Handle)Queue_head(maxQ);
            curTask = Task_module->curTask;

            if (Task_checkStackFlag != false) {
                /* UNREACH.GEN */
                Task_checkStacks(prevTask, curTask);
            }

#if defined(Task_ENABLE_SWITCH_HOOKS) || defined(Task_logInfo1_D)
            /* It's safe to enable intrs here */
            (void)Hwi_enable();

#ifdef Task_ENABLE_SWITCH_HOOKS
            for (i = 0; i < Task_hooks.length; i++) {
                if (Task_hooks.elem[i].switchFxn != NULL) {
                    Task_hooks.elem[i].switchFxn(prevTask, curTask);
                }
            }
#endif

            /* MISRA.CAST.FUNC_PTR.2012 MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
            Log_write(Log_INFO1, Task_switchLogEvt, prevTask, prevTask->fxn,
                    curTask, curTask->fxn);

            /* Hard-disable intrs - this fxn is called with them disabled */
            (void)Hwi_disable();
#elif defined(Task_minimizeLatency_D) \
    && (Task_minimizeLatency_D == true)
            Hwi_enable();
            Hwi_disable();
#endif

            TaskSupport_swap((void *)&prevTask->context,
                            (void *)&curTask->context);
        }
    } while (Task_module->workFlag != false);
}

/*
 *  ======== Task_enter ========
 *  Here on task's first invocation.
 *
 *  Unlock the Task Scheduler to enter task as though we
 *  returned through Task_restore()
 */
void Task_enter(void)
{
    if (Task_module->workFlag != false) {
        Task_schedule();
    }
    Task_module->locked = false;
    (void)Hwi_enable();
}

/*
 *************************************************************************
 *                      Module wide functions
 *************************************************************************
 */

/*
 *  ======== Task_init ========
 *  Initialize and start the Task Module.
 *  Called at system init time before main().
 */
/* MISRA.FUNC.UNUSEDPAR.2012 */
/* REQ_TAG(SYSBIOS-464) */
void Task_init (void)
{
    /* Interrupts are disabled at this point */
    if (Task_module->initDone) {
        return;
    }
    Task_module->initDone = true;

    /*
     * Need to wait for SupportProxy. Other modules safe.
     */
    if (BIOS_taskEnabled != false) {  /* minimize code foot print if Task is disabled */
        int i;

        /* The TaskSupport delegate cannnot depend on Task during Startup */
        TaskSupport_init();

#ifdef Task_ENABLE_REGISTER_HOOKS
        for (i = 0; i < Task_hooks.length; i++) {
            if (Task_hooks.elem[i].registerFxn != NULL) {
                Task_hooks.elem[i].registerFxn(i);
            }
        }
#endif
        /* initialize ReadyQs */
        for (i = 0; i < Task_numPriorities; i++) {
            Queue_construct(&Task_module->readyQ[i], NULL);
        }

        if (Task_moduleStateCheckFlag != false) {
            Task_moduleStateCheckValue =
                /* UNREACH.GEN */
                Task_moduleStateCheckValueFxn(Task_module);
        }
    }
}

/*
 *  ======== Task_startup ========
 */
void Task_startup(void)
{
    Queue_Handle maxQ;
    Task_Object *prevTask;
    Task_Struct dummyTask;
#ifdef Task_ENABLE_SWITCH_HOOKS
    int i;
#endif

    (void)Hwi_disable();      /* re-enabled in Task_enter of first task */

    /* Use dummyTask as initial task to swap from */
    prevTask = &dummyTask;

    /* stall until a task is ready */
    while (Task_module->curSet == 0U) {
        Task_allBlockedFunction();
    }

    /* Determine current max ready Task priority */
    maxQ = (Queue_Handle)((uint8_t *)(Task_module->readyQ) +
           (Intrinsics_maxbit(Task_module->curSet)*(2U*sizeof(void *))));

    Task_module->curQ = maxQ;
    Task_module->curTask = (Task_Handle)Queue_head(maxQ);

    /* we've done the scheduler's work */
    Task_module->workFlag = 0;

    /* Signal that we are entering task thread mode */
    (void)BIOS_setThreadType(BIOS_ThreadType_Task);

    if (Task_checkStackFlag != false) {
        /* UNREACH.GEN */
        Task_checkStacks(NULL, Task_module->curTask);
    }

    /* should be safe to enable intrs here */
    (void)Hwi_enable();

#ifdef Task_ENABLE_SWITCH_HOOKS
    /* Run switch hooks for first real Task */
    for (i = 0; i < Task_hooks.length; i++) {
        if (Task_hooks.elem[i].switchFxn != NULL) {
            Task_hooks.elem[i].switchFxn(NULL, Task_module->curTask);
        }
    }
#endif

    /* MISRA.CAST.FUNC_PTR.2012 MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write(Log_INFO1, Task_switchLogEvt, 0, 0, Task_module->curTask,
        Task_module->curTask->fxn);

    /* must leave this function with ints disabled */
    (void)Hwi_disable();

    /* inform dispatcher that we're running on task stack */
    Hwi_switchFromBootStack();

   /* start first task by way of enter() */
    TaskSupport_swap((void *)&prevTask->context,
                (void *)&Task_module->curTask->context);
/* LCOV_EXCL_START */
}
/* LCOV_EXCL_STOP */

/*
 *  ======== Task_enabled ========
 */
bool Task_enabled(void)
{
    if (BIOS_swiEnabled == false) {
        /* UNREACH.GEN */
        return (bool)(Task_module->locked == false);
    }
    else {
        return (bool)((Task_module->locked == false) && Swi_enabled() == true);
    }
}

/*
 *  ======== Task_disable ========
 */
unsigned int Task_disable(void)
{
    unsigned int key = (unsigned int)Task_module->locked;

    Task_module->locked = true;

    if (Task_moduleStateCheckFlag != false) {
        /* UNREACH.GEN */
        if (Task_moduleStateCheckFxn(Task_module,
                                     Task_moduleStateCheckValue) != 0) {
            Error_raise(NULL, Task_E_moduleStateCheckFailed, 0, 0);
        }
    }

    return (key);
}

/*
 *  ======== Task_enable ========
 */
void Task_enable(void)
{
    Task_restore(0);
}

/*
 *  ======== Task_restore ========
 */
void Task_restore(unsigned int tskKey)
{
    if (tskKey == false) {
        (void)Hwi_disable();
        if (Task_module->workFlag == true &&
            (BIOS_swiEnabled == false ||
             (BIOS_swiEnabled == true && Swi_enabled() == true))) {
            Task_schedule();
        }
        Task_module->locked = false;
        (void)Hwi_enable();
    }
}

/*
 *  ======== Task_restoreHwi ========
 *  Task_restore for Hwi dispatcher.
 *
 *  Called with interrupts disabled from dispatcher.
 *
 *  Returns with interrupts still disabled.
 *
 *  @param(key) key to restore previous Task scheduler state
 */
void Task_restoreHwi(unsigned int tskKey)
{
    if (tskKey == false) {
        if (Task_module->workFlag == true &&
            (BIOS_swiEnabled == false ||
             (BIOS_swiEnabled == true && Swi_enabled() == true))) {
            Task_schedule();
        }
        Task_module->locked = false;
    }
}

/*
 *  ======== Task_self ========
 */
/* REQ_TAG(SYSBIOS-511) */
Task_Handle Task_self(void)
{
    return (Task_module->curTask);
}

/*
 *  ======== Task_checkStacks ========
 */
void Task_checkStacks(Task_Handle oldTask, Task_Handle newTask)
{
    unsigned int oldTaskStack; /* used to obtain current (oldTask) stack address */

    if (Task_objectCheckFlag != false) {
        /* UNREACH.GEN */
        if (oldTask != NULL) {
            if (Task_objectCheckFxn(oldTask, oldTask->checkValue) != 0) {
                Error_raise(NULL, Task_E_objectCheckFailed, oldTask, 0);
            }
        }
        if (Task_objectCheckFxn(newTask, newTask->checkValue) != 0) {
            Error_raise(NULL, Task_E_objectCheckFailed, newTask, 0);
        }
    }

    /*
     * oldTask is NULL for the very first stack switch, skip it
     */
    if (oldTask == NULL) {
        return;
    }

    /* check top of stacks for 0xbe */
    if (TaskSupport_checkStack(oldTask->stack, oldTask->stackSize) ==
        false) {
        Error_raise(NULL, Task_E_stackOverflow, oldTask, 0);
    }

    if (TaskSupport_checkStack(newTask->stack, newTask->stackSize) ==
        false) {
        Error_raise(NULL, Task_E_stackOverflow, newTask, 0);
    }

    /* check sp's for being in bounds */
    if (((uintptr_t)&oldTaskStack < (uintptr_t)oldTask->stack) ||
        ((uintptr_t)&oldTaskStack > (uintptr_t)(oldTask->stack+oldTask->stackSize))) {
        /* MISRA.CAST.VOID_PTR_TO_INT.2012 */
        Error_raise(NULL, Task_E_spOutOfBounds, oldTask, &oldTaskStack);
    }

    if ((newTask->context < (void *)newTask->stack) ||
        (newTask->context > (void *)(newTask->stack+newTask->stackSize))) {
        /* MISRA.CAST.VOID_PTR_TO_INT.2012 */
        Error_raise(NULL, Task_E_spOutOfBounds, newTask, newTask->context);
    }
}

/*
 *  ======== Task_processVitalTasks ========
 *  Call BIOS_exit() when last vitalTask exits or is
 *  deleted.
 */
void Task_processVitalTaskFlag(Task_Object *tsk)
{
    unsigned int hwiKey;
    if (tsk->vitalTaskFlag == true) {
        hwiKey = Hwi_disable();
        Task_module->vitalTasks--;
        if (Task_module->vitalTasks == 0U) {
            Hwi_restore(hwiKey);
            BIOS_exit(0);
        }
        Hwi_restore(hwiKey);
    }
}

/*
 *  ======== Task_exit ========
 */
void Task_exit(void)
{
    unsigned int tskKey, hwiKey;
    Task_Object *tsk;
#ifdef Task_ENABLE_EXIT_HOOKS
    int i;
#endif

    tsk = Task_self();

#ifdef Task_ENABLE_EXIT_HOOKS
    /*
     * Process Task_exit hooks.  Should be called outside the Task kernel.
     */
    for (i = 0; i < Task_hooks.length; i++) {
        if (Task_hooks.elem[i].exitFxn != NULL) {
            Task_hooks.elem[i].exitFxn(tsk);
        }
    }
#endif

    /* MISRA.CAST.FUNC_PTR.2012 MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write(Log_INFO2, Task_exitLogEvt, tsk, tsk->fxn);

    tskKey = Task_disable();
    hwiKey = Hwi_disable();

    Task_blockI(tsk);

    tsk->mode = Task_Mode_TERMINATED;

    Task_processVitalTaskFlag(tsk);

    Hwi_restore(hwiKey);

    Queue_elemClear(&tsk->qElem);

    /* add to terminated task list if it was dynamically created */
    if (Task_deleteTerminatedTasks == true) {
        Task_Handle dynTask;

        /* UNREACH.GEN */
        dynTask = Task_Object_first();

        while (dynTask != NULL) {
            if (tsk == dynTask) {
                tsk->readyQ = &Task_module->terminatedQ;
                Queue_put(tsk->readyQ, &tsk->qElem);
                break;
            }
            else {
                dynTask = Task_Object_next(dynTask);
            }
        }
    }

    Task_restore(tskKey);
}

/*
 *  ======== Task_sleepTimeout ========
 *  called by Clock when timeout for a Task_sleep() expires
 */
void Task_sleepTimeout(uintptr_t arg)
{
    unsigned int hwiKey;
    Task_PendElem *elem = (Task_PendElem *)Types_uargToPtr(arg);

    hwiKey = Hwi_disable();

    /*
     * put tsk back into readyQ
     * No need for Task_disable/restore sandwich since this
     * is called within Swi (or Hwi) thread
     */
    Task_unblockI(elem->taskHandle, hwiKey);

    Hwi_restore(hwiKey);
}

/*
 *  ======== Task_sleep ========
 */
/* REQ_TAG(SYSBIOS-518) */
void Task_sleep(uint32_t timeout)
{
    Task_PendElem elem;
    unsigned int hwiKey, tskKey;
    Clock_Struct clockStruct;

    if (timeout == BIOS_NO_WAIT) {
        return;
    }

    Assert_isTrue((timeout != BIOS_WAIT_FOREVER),
                    Task_A_badTimeout);

    /*
     * BIOS_clockEnabled check is here to eliminate Clock module
     * references in the custom library
     */
    if (BIOS_clockEnabled != false) {
        /* init Clock object */
        Clock_initI(Clock_handle(&clockStruct), (Clock_FuncPtr)Task_sleepTimeout, timeout, (uintptr_t)&elem);
        elem.clockHandle = Clock_handle(&clockStruct);
    }

    /* MISRA.CAST.FUNC_PTR.2012 MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write(Log_INFO1, Task_sleepLogEvt, Task_self(), Task_self()->fxn,
               timeout);

    hwiKey = Hwi_disable();

    /*
     * Verify that THIS core hasn't already disabled the scheduler
     * so that the Task_restore() call below will indeed block
     */
    Assert_isTrue(Task_enabled() == true, Task_A_sleepTaskDisabled);

    /* lock scheduler */
    tskKey = Task_disable();

    /* get task handle and block tsk */
    elem.taskHandle = Task_self();

    Task_blockI(elem.taskHandle);

    /*
     * BIOS_clockEnabled check is here to eliminate Clock module
     * references in the custom library
     */
    if (BIOS_clockEnabled != false) {
        Clock_enqueueI(elem.clockHandle);
        Clock_startI(elem.clockHandle);
    }

    /* Only needed for Task_delete() */
    Queue_elemClear(&elem.qElem);

    elem.taskHandle->pendElem = (void *)(&elem);

    Hwi_restore(hwiKey);

    /* unlock task scheduler and block */
    Task_restore(tskKey);       /* the calling task will block here */

    /*
     * BIOS_clockEnabled check is here to eliminate Clock module
     * references in the custom library
     */
    if (BIOS_clockEnabled != false) {
        hwiKey = Hwi_disable();
        /* remove Clock object from Clock Q */
        Clock_removeI(elem.clockHandle);
        elem.clockHandle = NULL;
        Hwi_restore(hwiKey);
    }

    elem.taskHandle->pendElem = NULL;
}


/*
 *  ======== Task_yield ========
 */
void Task_yield(void)
{
    unsigned int tskKey, hwiKey;

    tskKey = Task_disable();
    hwiKey = Hwi_disable();

    if (Task_module->curQ != NULL) {
        /* move current task to end of curQ */
        Queue_enqueue(Task_module->curQ,
            Queue_dequeue(Task_module->curQ));
    }
    Task_module->curQ = NULL;  /* force a Task_switch() */
    Task_module->workFlag = 1;

    Hwi_restore(hwiKey);

    /* MISRA.CAST.FUNC_PTR.2012 MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write(Log_INFO1, Task_yieldLogEvt, Task_module->curTask,
        Task_module->curTask->fxn, BIOS_getThreadType());

    Task_restore(tskKey);
}


/*
 *  ======== Task_getIdleTask ========
 */
Task_Handle Task_getIdleTask(void)
{
    return(Task_module->idleTask);
}

/*
 *************************************************************************
 *                       Instance functions
 *************************************************************************
 */


/*
 *  ======== Task_Instance_init ========
 */
/*
 * REQ_TAG(SYSBIOS-575), REQ_TAG(SYSBIOS-463), REQ_TAG(SYSBIOS-636)
 * REQ_TAG(SYSBIOS-570)
 */
int Task_Instance_init(Task_Object *tsk, Task_FuncPtr fxn,
                const Task_Params *params, Error_Block *eb)
{
    unsigned int align;
    int status;
    size_t stackSize;

    Assert_isTrue((BIOS_taskEnabled == true), Task_A_taskDisabled);

    Assert_isTrue(((BIOS_getThreadType() != BIOS_ThreadType_Hwi) &&
                   (BIOS_getThreadType() != BIOS_ThreadType_Swi)), Task_A_badThreadType);

    Assert_isTrue(((
                       (params->priority == -1) || (params->priority > 0) ||
                       ((params->priority == 0) && (fxn == Idle_loop))
                   ) &&
                   (params->priority < (int)Task_numPriorities)),
                   Task_A_badPriority);

    tsk->name = params->name;
    tsk->priority = params->priority;

    /* deal with undefined Task_Params defaults */
    if (params->stackHeap == NULL) {
        tsk->stackHeap = Task_defaultStackHeap;
    }
    else {
        tsk->stackHeap = params->stackHeap;
    }

    if (params->stackSize == 0U) {
        stackSize = (size_t)Task_defaultStackSize;
    }
    else {
        stackSize = params->stackSize;
    }

    align = TaskSupport_getStackAlignment();

    if (params->stack != NULL) {
        if (align != 0U) {
            uintptr_t stackTemp;
            /* align low address to stackAlignment */
            /* MISRA.CAST.VOID_PTR_TO_INT.2012 */
            stackTemp = (uintptr_t)params->stack;
            stackTemp = stackTemp + (align - 1U);
            stackTemp = stackTemp & ~(align - 1U);
            tsk->stack = (char *)Types_uargToPtr(stackTemp);

            /* subtract what we removed from the low address from stackSize */
            /* MISRA.CAST.VOID_PTR_TO_INT.2012 */
            tsk->stackSize = stackSize - (stackTemp - (uintptr_t)params->stack);

            /* lower the high address as necessary */
            tsk->stackSize = tsk->stackSize & (size_t)~(align - 1U);
        }
        else {
            tsk->stack = (char *)params->stack;
            tsk->stackSize = stackSize;
        }
        /* tell Task_delete that stack was provided */
        tsk->stackHeap = (IHeap_Handle)(-1);
    }
    else {
        if (BIOS_runtimeCreatesEnabled != false) {
            if (align != 0U) {
                /*
                 * round stackSize up to the nearest multiple of the alignment.
                 */
                tsk->stackSize = (stackSize + (align - 1U)) & ~(align - 1U);
            }
            else {
                tsk->stackSize = stackSize;
            }

            tsk->stack = (char *)Memory_alloc(tsk->stackHeap, tsk->stackSize,
                                      align, eb);

            if (tsk->stack == NULL) {
                return (1);
            }
        }
        else {
            return (1); /* fail to construct if stack is not provided in construct-only model */
        }
    }

    tsk->fxn = fxn;
    tsk->arg0 = params->arg0;
    tsk->arg1 = params->arg1;

    tsk->env = params->env;

    tsk->vitalTaskFlag = params->vitalTaskFlag;
    if (tsk->vitalTaskFlag == true) {
        Task_module->vitalTasks += 1U;
    }

    tsk->tls = NULL;

#ifdef Task_ENABLE_HOOKS
    if (Task_hooks.length > 0) {
        tsk->hookEnv = (void * *)Memory_calloc(Task_Object_heap(),
                (unsigned int)Task_hooks.length * sizeof (void *), 0, eb);

        if (tsk->hookEnv == NULL) {
            return (2);
        }
    }
#endif

    status = Task_postInit(tsk, eb);

    if (status != 0) {
        return (2 + status);
    }

    /* put task on global task list (Queue_put is atomic) */
    Queue_put(&Task_module->objQ, &tsk->objElem);

    return (0);   /* no failure states */
}

/*
 *  ======== Task_postInit ========
 *  Initialize stack.
 *  Build Initial stack image.
 *  Add task to corresponding ready Queue.
 *
 *  returns (0) and clean 'eb' on success
 *  returns (0) and 'eb' if TaskSupport_start() fails.
 *  returns (n) and 'eb' for number of successful createFxn() calls iff
 *     one of the createFxn() calls fails
 */
int Task_postInit(Task_Object *tsk, Error_Block *eb)
{
    unsigned int tskKey, hwiKey;
    Queue_Handle readyQ;
#ifdef Task_ENABLE_CREATE_HOOKS
    int i;
    Error_Block localEB;
    Error_Block *leb;
#endif

    tsk->context = TaskSupport_start(tsk,
                (TaskSupport_FuncPtr)Task_enter,
                (TaskSupport_FuncPtr)Task_exit,
                eb);

    if (tsk->context == NULL) {
        return (1);
    }

    tsk->mode = Task_Mode_READY;

    tsk->pendElem = NULL;

    if (Task_objectCheckFlag != false) {
        /* UNREACH.GEN */
        if (Task_objectCheckValueFxn == (Task_ObjectCheckValueFuncPtr)Task_getObjectCheckValue) {
            tsk->checkValue = Task_getObjectCheckValue(tsk);
        }
        else {
            tsk->checkValue = Task_objectCheckValueFxn(tsk);
        }
    }

#ifdef Task_ENABLE_CREATE_HOOKS
    if (eb != Error_IGNORE) {
        leb = eb;
    }
    else {
        Error_init(&localEB);
        leb = &localEB;
    }

    for (i = 0; i < Task_hooks.length; i++) {
        tsk->hookEnv[i] = (void *)0;
        if (Task_hooks.elem[i].createFxn != NULL) {
            Task_hooks.elem[i].createFxn(tsk, leb);

            if (Error_check(leb)) {
                return (i + 2);
            }
        }
    }
#endif

    if (tsk->priority < 0) {
        tsk->mask = 0;
        tsk->readyQ = &Task_module->inactiveQ;
        Queue_put(tsk->readyQ, &tsk->qElem);
    }
    else {
        tsk->mask = (unsigned int)1 << (unsigned int)tsk->priority;
        readyQ = &Task_module->readyQ[tsk->priority];
        tsk->readyQ = readyQ;

        tskKey = Task_disable();
        hwiKey = Hwi_disable();
        Task_unblock(tsk);
        Hwi_restore(hwiKey);
        Task_restore(tskKey);
    }

    return (0);
}

/*
 *  ======== Task_Instance_finalize ========
 */
/* REQ_TAG(SYSBIOS-636) */
void Task_Instance_finalize(Task_Object *tsk, int status)
{
#ifdef Task_ENABLE_DELETE_HOOKS
    int i, cnt;
#endif
    unsigned int taskKey, hwiKey;

    /*
     * Tasks can only be deleted from main and task threads.
     * Running Tasks can not be deleted.
     */
    if (status == 0) {
        taskKey = Task_disable();

        /*
         * Bar users from calling Task_delete() on terminated tasks
         * if deleteTerminatedTasks is enabled.
         */
        if ((Task_deleteTerminatedTasks == true)
             && (Task_getMode(tsk) == Task_Mode_TERMINATED)
             && (tsk->readyQ == &Task_module->terminatedQ)) {
            /* UNREACH.GEN */
            Error_raise(NULL, Task_E_deleteNotAllowed, tsk, 0);
        }

        Assert_isTrue((Task_getMode(tsk) != Task_Mode_RUNNING),
                        Task_A_badTaskState);

        Assert_isTrue((BIOS_getThreadType() == BIOS_ThreadType_Main) ||
                      (BIOS_getThreadType() == BIOS_ThreadType_Task),
                        Task_A_badThreadType);

        hwiKey = Hwi_disable();

        if (tsk->mode == Task_Mode_READY) {
            /* remove task from its ready list */
            Queue_remove(&tsk->qElem);
            /* if last task in readyQ, remove corresponding bit in curSet */
            if (Queue_empty(tsk->readyQ) != false) {
                Task_module->curSet &= ~tsk->mask;
            }

            /*
             * if task was made ready by a pend timeout but hasn't run yet
             * then its clock object is still on the Clock service Q.
             */
            if (tsk->pendElem != NULL) {
                if (BIOS_clockEnabled && tsk->pendElem->clockHandle) {
                    Clock_removeI(tsk->pendElem->clockHandle);
                }
            }
        }

        if (tsk->mode == Task_Mode_BLOCKED) {
            Assert_isTrue(tsk->pendElem != NULL, Task_A_noPendElem);

            /* Seemingly redundant test in case Asserts are disabled */
            if (tsk->pendElem != NULL) {
                Queue_remove(&(tsk->pendElem->qElem));
                if (BIOS_clockEnabled && tsk->pendElem->clockHandle) {
                    Clock_removeI(tsk->pendElem->clockHandle);
                }
            }
        }

        if (tsk->mode == Task_Mode_TERMINATED) {
            /* remove task from terminated task list */
            Queue_remove(&tsk->qElem);
        }
        else {
            Task_processVitalTaskFlag(tsk);
        }

        Hwi_restore(hwiKey);

        Task_restore(taskKey);
    }

    /* return if failed before allocating stack */
    if (status == 1) {
        return;
    }

    if (BIOS_runtimeCreatesEnabled != false) {
        /* free stack if it was allocated dynamically */
        if (tsk->stackHeap != (IHeap_Handle)(-1)) {
            Memory_free(tsk->stackHeap, tsk->stack, tsk->stackSize);
        }
    }

    /* return if failed to allocate Hook Env */
    if (status == 2) {
        return;
    }

    /* return if TaskSupport_start() failed to construct a task stack image */
    if (status == 3) {
        return;
    }

    /* status == 0 or status == 4 - in both cases create hook was called */

#ifdef Task_ENABLE_HOOKS
    /* free any allocated Hook Envs */
    if (Task_hooks.length > 0) {
#ifdef Task_ENABLE_DELETE_HOOKS
        if (status == 0) {
            cnt = Task_hooks.length;
        }
        else {
            cnt = status - 4;   /* # successful createFxn() calls */
        }

        /*
         * only call deleteFxn() if createFxn() was successful
         */
        for (i = 0; i < cnt; i++) {
            if (Task_hooks.elem[i].deleteFxn != NULL) {
                Task_hooks.elem[i].deleteFxn(tsk);
            }
        }
#endif
        Memory_free(Task_Object_heap(), tsk->hookEnv,
                (unsigned int)Task_hooks.length * sizeof (void *));
    }
#endif
}

/*
 *  ======== Task_getArg0 ========
 */
uintptr_t Task_getArg0(Task_Object *tsk)
{
    return (tsk->arg0);
}

/*
 *  ======== Task_getArg1 ========
 */
uintptr_t Task_getArg1(Task_Object *tsk)
{
    return (tsk->arg1);
}

/*
 *  ======== Task_getEnv ========
 */
void * Task_getEnv(Task_Object *tsk)
{
    return (tsk->env);
}

/*
 *  ======== Task_FuncPtr ========
 */
/* REQ_TAG(SYSBIOS-455) */
Task_FuncPtr Task_getFunc(Task_Object *task, uintptr_t *arg0, uintptr_t *arg1)
{
    if (arg0 != NULL) {
        *arg0 = task->arg0;
    }

    if (arg1 != NULL) {
        *arg1 = task->arg1;
    }

    return (task->fxn);
}

/*
 *  ======== Task_getHookContext ========
 */
/* REQ_TAG(SYSBIOS-454) */
void * Task_getHookContext(Task_Object *tsk, int id)
{
    return (tsk->hookEnv[id]);
}

/*
 *  ======== Task_setHookContext ========
 */
/* REQ_TAG(SYSBIOS-454) */
void Task_setHookContext(Task_Object *tsk, int id, void * hookContext)
{
    tsk->hookEnv[id] = hookContext;
}

/*
 *  ======== Task_getName ========
 */
char * Task_getName(Task_Object *tsk)
{
    return (tsk->name);
}

/*
 *  ======== Task_getPri ========
 */
/* REQ_TAG(SYSBIOS-510) */
int Task_getPri(Task_Object *tsk)
{
    return (tsk->priority);
}

/*
 *  ======== Task_setArg0 ========
 */
void Task_setArg0(Task_Object *tsk, uintptr_t arg)
{
    tsk->arg0 = arg;
}

/*
 *  ======== Task_setArg1 ========
 */
void Task_setArg1(Task_Object *tsk, uintptr_t arg)
{
    tsk->arg1 = arg;
}

/*
 *  ======== Task_setEnv ========
 */
void Task_setEnv(Task_Object *tsk, void * env)
{
    tsk->env = env;
}

/*
 *  ======== Task_setPri ========
 */
/* REQ_TAG(SYSBIOS-510) */
int Task_setPri(Task_Object *tsk, int priority)
{
    int oldPri;
    unsigned int newMask, tskKey, hwiKey;
    Queue_Handle newQ;

    Assert_isTrue((((priority == -1) || (priority > 0) ||
                  ((priority == 0 && Task_module->idleTask == NULL))) &&
                   (priority < (int)Task_numPriorities)),
                   Task_A_badPriority);

    /* MISRA.CAST.FUNC_PTR.2012 MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write(Log_INFO1, Task_setPriLogEvt, tsk, tsk->fxn, tsk->priority,
        priority);

    tskKey = Task_disable();
    hwiKey = Hwi_disable();

    oldPri = tsk->priority;

    if (oldPri == priority) {
        Hwi_restore(hwiKey);
        Task_restore(tskKey);
        return (oldPri);
    }

    if (priority < 0) {
        newMask = 0;
        newQ = &Task_module->inactiveQ;
    }
    else {
        newMask = (unsigned int)1 << (unsigned int)priority;
        newQ = (Queue_Handle)((uint8_t *)(Task_module->readyQ) +
                (unsigned int)((unsigned int)priority*(2U*sizeof(void *))));
    }

    if (tsk->mode == Task_Mode_READY) {
        Queue_remove(&tsk->qElem);

        /* if last task in readyQ, remove corresponding bit in curSet */
        if (Queue_empty(tsk->readyQ) != false) {
            Task_module->curSet &= ~tsk->mask;
        }

        if (Task_module->curTask == tsk) {
            Task_module->curQ = newQ;   /* force a Task_switch() */
                                        /* if no longer maxQ */
            /* Put current task at front of its new readyQ */
            Queue_insert(((Queue_Elem *)(newQ))->next, &tsk->qElem);
        }
        else {
            /* place task at end of its readyQ */
            Queue_enqueue(newQ, &tsk->qElem);
        }

        Task_module->curSet |= newMask;
    }

    tsk->priority = priority;
    tsk->mask = newMask;
    tsk->readyQ = newQ;

    if (priority < 0) {
        Task_module->curQ = NULL;   /* force a Task_switch() */
    }

    Task_module->workFlag = 1;

    Hwi_restore(hwiKey);
    Task_restore(tskKey);

    return (oldPri);
}

/*
 *  ======== Task_getMode ========
 */
Task_Mode Task_getMode(Task_Object *tsk)
{
    if (tsk->mode == Task_Mode_READY && tsk == Task_self()) {
        return (Task_Mode_RUNNING);
    }
    else if (tsk->priority == -1) {
        return (Task_Mode_INACTIVE);
    }
    else {
        return (tsk->mode);
    }
}

/*
 *  ======== Task_stat ========
 */
void Task_stat(Task_Object *tsk, Task_Stat *statbuf)
{
    statbuf->priority = tsk->priority;
    statbuf->stack = (void *)tsk->stack;
    statbuf->stackSize = tsk->stackSize;
    statbuf->stackHeap = tsk->stackHeap;
    statbuf->env = tsk->env;
    statbuf->mode = Task_getMode(tsk);
    statbuf->sp = tsk->context;

    statbuf->used = TaskSupport_stackUsed((char *)tsk->stack,
        tsk->stackSize);
}

/*
 *  ======== Task_block ========
 */
void Task_block(Task_Object *tsk)
{
    unsigned int hwiKey;

    hwiKey = Hwi_disable();

    Task_blockI(tsk);

    Hwi_restore(hwiKey);
}

/*
 *  ======== Task_blockI ========
 *  Block a task.
 *
 *  Remove a task from its ready list.
 *  Must be called within Task_disable/Task_restore block
 *  and with interrupts disabled
 */
void Task_blockI(Task_Object *tsk)
{
    Queue_Object *readyQ = tsk->readyQ;
    unsigned int curset = Task_module->curSet;
    unsigned int mask = tsk->mask;

    if (Task_objectCheckFlag != false) {
        /* UNREACH.GEN */
        if (Task_objectCheckFxn(tsk, tsk->checkValue) != 0) {
            Error_raise(NULL, Task_E_objectCheckFailed, tsk, 0);
        }
    }

    /* MISRA.CAST.FUNC_PTR.2012 MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write(Log_INFO1, Task_blockLogEvt, tsk, tsk->fxn);

    Queue_remove(&tsk->qElem);

    /* if last task in readyQ, remove corresponding bit in curSet */
    if (Queue_empty(readyQ) != false) {
        Task_module->curSet = curset & ~mask;
    }
    if (Task_module->curTask == tsk) {
        Task_module->curQ = NULL;  /* force a Task_switch() */
    }
    tsk->mode = Task_Mode_BLOCKED;

    Task_module->workFlag = 1;
}

/*
 *  ======== Task_unblock ========
 */
void Task_unblock(Task_Object *tsk)
{
    unsigned int hwiKey;

    hwiKey = Hwi_disable();

    Task_unblockI(tsk, hwiKey);

    Hwi_restore(hwiKey);
}

/*
 *  ======== Task_unblockI ========
 *  Unblock a task.
 *
 *  Place task in its ready list.
 *  Must be called within Task_disable/Task_restore block
 *  and with interrupts disabled
 */
void Task_unblockI(Task_Object *tsk, unsigned int hwiKey)
{
#ifdef Task_ENABLE_READY_HOOKS
    int i;
#endif
    unsigned int curset = Task_module->curSet;
    unsigned int mask = tsk->mask;

    if (Task_objectCheckFlag != false) {
        /* UNREACH.GEN */
        if (Task_objectCheckFxn(tsk, tsk->checkValue) != 0) {
            Error_raise(NULL, Task_E_objectCheckFailed, tsk, 0);
        }
    }

    Queue_enqueue(tsk->readyQ, &tsk->qElem);

    Task_module->curSet = curset | mask;
    tsk->mode = Task_Mode_READY;
    Task_module->workFlag = 1;

    /* It's safe to enable intrs here */
    Hwi_restore(hwiKey);

#ifdef Task_ENABLE_READY_HOOKS
    for (i = 0; i < Task_hooks.length; i++) {
        if (Task_hooks.elem[i].readyFxn != NULL) {
            Task_hooks.elem[i].readyFxn(tsk);
        }
    }
#endif

    /* MISRA.CAST.FUNC_PTR.2012 MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write(Log_INFO1, Task_readyLogEvt, tsk, tsk->fxn, tsk->priority);

    /* Hard-disable intrs - this fxn is called with them disabled */
    (void)Hwi_disable();
}

/*
 *  ======== Task_allBlockedFunction ========
 */
void Task_allBlockedFunction(void)
{
    volatile unsigned int delay;

    if (Task_allBlockedFunc == &Idle_run) {
        (void)Hwi_enable();
        Idle_run();
        (void)Hwi_disable();
    }
    else if (Task_allBlockedFunc == NULL) {
        (void)Hwi_enable();
        /* Guarantee that interrupts are enabled briefly */
        for (delay = 0; delay < 1U; delay = delay + 1U) {
           ;
        }
        (void)Hwi_disable();
    }
    else {
        Task_allBlockedFunc();
        /*
         * disable ints just in case the
         * allBlockedFunc left them enabled
         */
        (void)Hwi_disable();
    }
}

/*
 *  ======== Task_deleteTerminatedTasksFunc ========
 */
void Task_deleteTerminatedTasksFunc(void)
{
    unsigned int hwiKey, taskKey;
    Task_Handle tsk;

    taskKey = Task_disable();

    hwiKey = Hwi_disable();

    if (Queue_empty(&Task_module->terminatedQ) == false) {
        tsk = (Task_Handle)Queue_head(&Task_module->terminatedQ);
        Hwi_restore(hwiKey);
        tsk->readyQ = NULL;
        Task_delete(&tsk);
    }
    else {
        Hwi_restore(hwiKey);
    }

    Task_restore(taskKey);
}

/*
 *  ======== Task_moduleStateCheck ========
 */
int Task_moduleStateCheck(Task_Module_State *moduleState, uint32_t checkValue)
{
    uint32_t newCheckValue;

    newCheckValue = Task_moduleStateCheckValueFxn(moduleState);
    if (newCheckValue != checkValue) {
        return (-1);
    }

    /* Check if curQ is within range */
    if (((moduleState->curQ != NULL) &&
        ((moduleState->curQ < (Queue_Handle)(moduleState->readyQ)) ||
         (moduleState->curQ > (Queue_Handle)((uint8_t *)(moduleState->readyQ) +
         ((unsigned int)Task_numPriorities * (unsigned int)(2U * sizeof(void *)))))))) {
        return (-1);
    }

    return (0);
}

/*
 *  ======== Task_getModuleStateCheckValue ========
 */
uint32_t Task_getModuleStateCheckValue(Task_Module_State *moduleState)
{
    /* PORTING.CMPSPEC.TYPE.LONGLONG */
    uint64_t checksum;

    checksum = (uint64_t)(unsigned int)((uintptr_t)moduleState->readyQ +
               (uintptr_t)moduleState->idleTask);
    checksum = (checksum >> 32) + (checksum & 0xFFFFFFFFU);
    checksum = checksum + (checksum >> 32);

    return ((uint32_t)(~checksum));
}

/*
 *  ======== Task_objectCheck ========
 */
int Task_objectCheck(Task_Handle handle, uint32_t checkValue)
{
    uint32_t newCheckValue;

    newCheckValue = Task_objectCheckValueFxn(handle);
    if (newCheckValue != checkValue) {
        return (-1);
    }

    return (0);
}

/*
 *  ======== Task_getObjectCheckValue ========
 */
uint32_t Task_getObjectCheckValue(Task_Handle taskHandle)
{
    /* PORTING.CMPSPEC.TYPE.LONGLONG */
    uint64_t checksum;

    checksum = (uint64_t)(unsigned int)(taskHandle->stackSize +
               (uintptr_t)taskHandle->stack +
               (uintptr_t)taskHandle->stackHeap +
#if defined(__IAR_SYSTEMS_ICC__)
               (uint64_t)taskHandle->fxn +
#else
               (uintptr_t)taskHandle->fxn +
#endif
               taskHandle->arg0 +
               taskHandle->arg1 +
               (uintptr_t)taskHandle->hookEnv +
               (unsigned int)taskHandle->vitalTaskFlag);
    checksum = (checksum >> 32) + (checksum & 0xFFFFFFFFU);
    checksum = checksum + (checksum >> 32);

    return ((uint32_t)(~checksum));
}

/*
 *  ======== Task_getTaskFromObjElem ========
 */
Task_Handle Task_getTaskFromObjElem(Queue_Elem *tskQelem)
{
    if (tskQelem == (Queue_Elem *)&Task_module->objQ) {
        return (NULL);
    }

    return ((Task_Handle)((char *)tskQelem - offsetof(Task_Struct, objElem)));
}

/* -------- The following functions were generated in BIOS 6.x -------- */

/*
 *  ======== Task_Object_first ========
 */
Task_Handle Task_Object_first()
{
    return (Task_getTaskFromObjElem(Queue_head(&(Task_module->objQ))));
}

/*
 *  ======== Task_Object_next ========
 */
Task_Handle Task_Object_next(Task_Handle tsk)
{
    return (Task_getTaskFromObjElem(Queue_next(&tsk->objElem)));
}

/*
 *  ======== Task_Params_init ========
 */
void Task_Params_init(Task_Params *params)
{
    *params = Task_Params_default;
}

/*
 *  ======== Task_construct ========
 */
Task_Handle Task_construct(Task_Object *task, Task_FuncPtr fxn,
                const Task_Params *params, Error_Block *eb)
{
    int status = 0;

    if (params == NULL) {
        params = &Task_Params_default;
    }

    status = Task_Instance_init(task, fxn, params, eb);

    if (status != 0) {
        Task_Instance_finalize(task, status);
        task = NULL;
    }

    return (task);
}

/*
 *  ======== Task_create ========
 */
Task_Handle Task_create(Task_FuncPtr fxn,
                const Task_Params *params, Error_Block *eb)
{
    Task_Handle task, status;

    task = Memory_alloc(NULL, sizeof(Task_Object), 0, eb);

    if (task != NULL) {
       status = Task_construct(task, fxn, params, eb);
       if (status == NULL) {
           Memory_free(NULL, task, sizeof(Task_Object));
           task = NULL;
       }
    }

    return (task);
}

/*
 *  ======== Task_destruct ========
 */
void Task_destruct(Task_Object *tsk)
{
    unsigned int hwiKey;

    /* remove task from global task list (Queue_remove is not atomic) */
    hwiKey = Hwi_disable();
    Queue_remove(&tsk->objElem);
    Hwi_restore(hwiKey);

    Task_Instance_finalize(tsk, 0);
}

/*
 *  ======== Task_delete ========
 */
void Task_delete(Task_Handle *tsk)
{
    Task_destruct(*tsk);
 
    Memory_free(NULL, *tsk, sizeof(Task_Object));

    *tsk = NULL;
}
