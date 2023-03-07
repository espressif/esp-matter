/*
 * Copyright (c) 2012-2020, Texas Instruments Incorporated
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
 *  ======== Load.c ========
 */

#include <stdint.h>

#include <xdc/std.h>

#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Core.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/gates/GateMutex.h>

/* Internal access to add Task function address to Task Load event. */
#define ti_sysbios_knl_Task__internalaccess
#include <ti/sysbios/knl/Task.h>

#include "package/internal/Load.xdc.h"

/*
 *************************************************************************
 *                       Internal functions
 *************************************************************************
 */

/*
 *  ======== Load_taskRegHook ========
 *  The Task register hook is called once per hookset, before main and before
 *  any Task initialization has been done. 
 *  
 *  This function allows the Load module to store its hookset id, which is 
 *  passed to Task_get/setHookContext. The HookContext can be an arbitrary
 *  structure. The Load module has defined a HookContext to store thread
 *  statistics. 
 */
Void Load_taskRegHook(Int id)
{
    Load_module->taskHId = id;
}

/*
 *  ======== Load_taskCreateHook ========
 *  Create and initialize the HookContext for this Task.
 */
Void Load_taskCreateHook(Task_Handle task, Error_Block *eb)
{
    Load_HookContext *context = NULL;
    UInt key;

    key = Hwi_disable();

    if (Load_module->taskNum < Load_module->taskEnvLen) {
        context = &(Load_module->taskEnv[Load_module->taskNum]);
        Load_module->taskNum++;
    }

    Hwi_restore(key);

    /* Create the HookContext for this Task. */
    if (context == NULL) {
        context = Memory_calloc(Task_Object_heap(), sizeof(Load_HookContext),
                0, eb);
        if (context == NULL) {
            return;
        }
    }
    Task_setHookContext(task, Load_module->taskHId, NULL);

    /* Initialize the HookContext for this Task. */
    if (Load_autoAddTasks == TRUE) {
        Load_addTask(task, context);
    }
}

/*
 *  ======== Load_taskDeleteHook ========
 */
Void Load_taskDeleteHook(Task_Handle task)
{
    Load_HookContext *pStoredContext;

    pStoredContext =
        (Load_HookContext *)Task_getHookContext(task, Load_module->taskHId);

    if (Load_autoAddTasks == TRUE) {
        Load_removeTask(task);
    }

    /* Only free dynamically allocated contexts */
    if (((uintptr_t)pStoredContext < (uintptr_t)(Load_module->taskEnv)) ||
            (uintptr_t)pStoredContext >=
            (uintptr_t)(Load_module->taskEnv + Load_module->taskEnvLen)) {
        Memory_free(Task_Object_heap(), pStoredContext,
                sizeof(Load_HookContext));
    }
}

/*
 *  ======== Load_taskSwitchHook ========
 */
Void Load_taskSwitchHook(Task_Handle curTask, Task_Handle nextTask)
{
    UInt32 curTime;
    UInt32 delta;
    UInt key;
    Load_HookContext *pCurTaskEnv;
    UInt coreId;

    if (BIOS_smpEnabled == TRUE) {
        coreId = Core_getId();
    }
    else {
        coreId = 0;
    }

    /* curTask is invalid during 1st Task switch */
    if (curTask == NULL) {

        /* Get the current system time */
        curTime = Timestamp_get32();

        /* Update the start time for next task */
        Load_module->taskStartTime[coreId] = curTime;

        /*
         * record Task handle, in case hwi or swi gets run before next switch
         * note that time for the dummy task may be counted towards 1st task.
         * We chose to do this to simplify code.
         */
        Load_module->runningTask[coreId] = nextTask;
        return;
    }

    key = Hwi_disable();

    /* Get the current system time */
    curTime = Timestamp_get32();

    /* Update the total time the task has run */
    delta = curTime - Load_module->taskStartTime[coreId];

    /* record it if task has been registered */
    pCurTaskEnv = (Load_HookContext *)Task_getHookContext(curTask,
        Load_module->taskHId);
    if (pCurTaskEnv != NULL) {
        pCurTaskEnv->nextTotalTime += delta;
    }

    /* Update the start time for next task */
    Load_module->taskStartTime[coreId] = curTime;

    /* record Task handle */
    Load_module->runningTask[coreId] = nextTask;
    Hwi_restore(key);
}

/*
 *  ======== Load_swiBeginHook ========
 */
Void Load_swiBeginHook(Swi_Handle swi)
{
    UInt32 curTime;
    UInt32 delta;
    UInt key;
    Load_HookContext *pTaskEnv;
    UInt coreId;

    if (BIOS_smpEnabled == TRUE) {
        coreId = Core_getId();
    }
    else {
        coreId = 0;
    }

    key = Hwi_disable();

    ++(Load_module->swiCnt);

    if (Load_module->swiCnt == 1) {   /* If first SWI */

        /* Get the current system time */
        curTime = Timestamp_get32();

        /* Update the total time the task has run */
        delta = curTime - Load_module->taskStartTime[coreId];

        /*
         * may be NULL if interrupt happened early before first task
         * switch, or if task monitoring is disabled.
         */
        if ((Load_taskEnabled == TRUE) && (Load_module->runningTask[coreId] != NULL)) {

            /* record it if task has been registered */
            pTaskEnv = (Load_HookContext *)Task_getHookContext(
                Load_module->runningTask[coreId], Load_module->taskHId);
            /*
             * (NULL is possible if task not registered)
             */
            if (pTaskEnv != NULL) {
                pTaskEnv->nextTotalTime += delta;
            }
        }

        /* Update the start time for next swi */
        Load_module->swiStartTime = curTime;
    }

    Hwi_restore(key);
}

/*
 *  ======== Load_swiEndHook ========
 */
Void Load_swiEndHook(Swi_Handle swi)
{
    UInt32 curTime;
    UInt32 delta;
    UInt key;
    UInt coreId;

    if (BIOS_smpEnabled == TRUE) {
        coreId = Core_getId();
    }
    else {
        coreId = 0;
    }

    key = Hwi_disable();  /* read swiStartTime atomically after decrement */

    --(Load_module->swiCnt);

    if (Load_module->swiCnt == 0) {  /* If last Swi */
        /* Get the current system time */
        curTime = Timestamp_get32();

        /* Update the total time the swi has run */
        delta = curTime - Load_module->swiStartTime;
        Load_module->swiEnv.nextTotalTime += delta;

        /* Update the start time for next task */
        Load_module->taskStartTime[coreId] = curTime;
    }

    Hwi_restore(key);
}

/*
 *  ======== Load_hwiEndHook ========
 */
Void Load_hwiEndHook(ti_sysbios_interfaces_IHwi_Handle hwi)
{
    UInt32 curTime;
    UInt32 delta;
    UInt key;
    UInt coreId;

    if (BIOS_smpEnabled == TRUE) {
        coreId = Core_getId();
    }
    else {
        coreId = 0;
    }

    key = Hwi_disable();  /* read swiStartTime atomically after decrement */

    --(Load_module->hwiCnt);

    if (Load_module->hwiCnt == 0) {  /* If last Hwi */
        /* Get the current system time */
        curTime = Timestamp_get32();

        /* Update the total time the hwi has run */
        delta = curTime - Load_module->hwiStartTime;
        Load_module->hwiEnv.nextTotalTime += delta;

        if (Load_module->swiCnt == 0) { /* we came from a Task */
            /* Update the start time for next task */
            Load_module->taskStartTime[coreId] = curTime;
        }
        else {   /* we came from a Swi */
            /* Update the start time for next swi */
            Load_module->swiStartTime = curTime;
        }
    }

    Hwi_restore(key);
}

/*
 *  ======== Load_addTask ========
 */
Void Load_addTask(Task_Handle task, Load_HookContext *env)
{
    UInt taskKey;
    Load_HookContext *pCurTaskEnv;

    if (Load_taskEnabled) {
        taskKey = Task_disable();
        pCurTaskEnv = (Load_HookContext *)
            Task_getHookContext(task, Load_module->taskHId);

        /* Check if task has already been registered */
        if (pCurTaskEnv != NULL) {
            Task_restore(taskKey);
        }

        /* Setup environment */
        env->totalTime = 0;
        env->nextTotalTime = 0;
        env->totalTimeElapsed = 0;
        env->timeOfLastUpdate = Timestamp_get32();
        env->threadHandle = (Ptr)task;

        Task_setHookContext(task, Load_module->taskHId, env);

        Queue_enqueue(Load_Module_State_taskList(),(Queue_Elem *)env);

        Task_restore(taskKey);
    }
}

/*
 *  ======== Load_removeTask ========
 */
Bool Load_removeTask(Task_Handle task)
{
    UInt taskKey;
    Load_HookContext *pCurTaskEnv;

    if (Load_taskEnabled) {
        taskKey = Task_disable();
        pCurTaskEnv = (Load_HookContext *)
            Task_getHookContext(task, Load_module->taskHId);
        if (pCurTaskEnv == NULL) { /* If task is not registered, return fail */
            Task_restore(taskKey);
            return (FALSE);
        }
        Queue_remove((Queue_Elem *)pCurTaskEnv);

        /*
         * could think of taking this outside the critical section, but
         * it's here in case a higher pri thread adds this task right
         * back (though very unlikely)
         */
        Task_setHookContext(task, Load_module->taskHId, NULL);

        Task_restore(taskKey);
        return (TRUE);
     }
     else {
        return (FALSE);
     }
}

/*
 *  ======== Load_idleFxn ========
 */
Void Load_idleFxn(Void)
{
    Types_FreqHz freq;
    UInt32 window;
    UInt32 t1;
    UInt32 delta;
    UInt key;

    /*
     *  We do this initialization in the idle loop in case the timestamp
     *  frequency changes (e.g due to frequency scaling), in which case the
     *  window would be automatically adjusted.
     */
    Timestamp_getFreq(&freq);
    Assert_isTrue(freq.hi == 0, NULL); /* Does not support CPU over 4 GHz */
    window = (freq.lo / 1000) * Load_windowInMs;

    /*
     * Compute the time difference since the last call to Load_idleFxn.
     */
    key = Hwi_disable();        /* In case reset or update is called */
    t1 = Timestamp_get32();
    delta = t1 - Load_module->t0;
    Load_module->t0 = t1;

    Load_module->timeElapsed += delta;  /* accumulate time */
    Load_module->idleCnt++;             /* bump # of times thru IDL loop */

    /* 
     * 'minLoop' keeps track of the shortest time thru the IDL loop.
     */
    if (delta < Load_module->minLoop) {
        Load_module->minLoop = delta;
    }

    Hwi_restore(key);

    /*
     * Compute the load if the time window has expired.
     * We try to minimize the amount of code between Timestamp_get32()
     * and Load_update() in idleFxn to ensure minloop is not
     * affected significantly when an update occurs.
     */
    if ((Load_module->timeElapsed >= window) && (Load_updateInIdle)) {
        Load_update();
    }
}

/*
 *  ======== Load_logCPULoad ========
 */
Void Load_logCPULoad(Void)
{
    /* Log global CPU load */
    Log_write1(Load_LS_cpuLoad, (IArg)Load_getCPULoad());
}

/*
 *  ======== Load_logLoads ========
 */
Void Load_logLoads(Void)
{
    Task_Handle task;
    Queue_Elem *qElem;
    Load_Stat stat;
    UInt taskKey;

    /* Log Hwi load */
    if ((Load_hwiEnabled) && (Load_getGlobalHwiLoad(&stat) == TRUE)) {
        Log_write2(Load_LS_hwiLoad, (IArg) stat.threadTime,
                   (IArg) stat.totalTime);
    }

    /* Log Swi load */
    if ((Load_swiEnabled) && (Load_getGlobalSwiLoad(&stat) == TRUE)) {
        Log_write2(Load_LS_swiLoad, (IArg) stat.threadTime,
                   (IArg) stat.totalTime);
    }

    /* Log Task load for each Task */
    if (Load_taskEnabled) {

        /* Protect access to Load module's taskList */
        taskKey = Task_disable();

        qElem = Queue_head(Load_Module_State_taskList());

        /* For each task... */
        while (qElem != (Queue_Elem *)Load_Module_State_taskList()) {
            task = ((Load_HookContext *)qElem)->threadHandle;

            /* Log the Task load. */
            if (Load_getTaskLoad(task, &stat) == TRUE) {
                Log_write4(Load_LS_taskLoad, (IArg)task, (IArg)stat.threadTime,
                           (IArg)stat.totalTime, (IArg)task->fxn);
            }

            qElem = Queue_next(qElem);
        }

        Task_restore(taskKey);
    }
}

/*
 *************************************************************************
 *                      Module wide functions
 *************************************************************************
 */

/*
 *  ======== Load_calculateLoad ========
 */
UInt32 Load_calculateLoad(Load_Stat *stat)
{
    UInt32 load, divisor;

    /*
     * We do a double division here to avoid overflow when multiplying
     * the threadTime by 100, since threadTime can often be very large
     * on some platform. This works well assuming that totalTime is much
     * larger than 100, which is the case when the number of timestamp
     * counts in a benchmark time window is much greater than 100.
     */
    divisor = stat->totalTime / 100;
    load = (divisor ? stat->threadTime / divisor : 0);

    return (load);
}

/*
 *  ======== Load_getCPULoad ========
 */
UInt32 Load_getCPULoad(void)
{
    return (Load_module->cpuLoad);
}

/*
 *  ======== Load_getGlobalHwiLoad ========
 */
Bool Load_getGlobalHwiLoad(Load_Stat *stat) 
{
    UInt key;

    if (Load_hwiEnabled) {
        key = Hwi_disable();
        stat->threadTime = Load_module->hwiEnv.totalTime;
        stat->totalTime = Load_module->hwiEnv.totalTimeElapsed;
        Hwi_restore(key);
        return (TRUE);
    }
    else {
        return (FALSE);
    }
}

/*
 *  ======== Load_getGlobalSwiLoad ========
 */
Bool Load_getGlobalSwiLoad(Load_Stat *stat)
{
    UInt key;

    if (Load_swiEnabled) {
        key = Hwi_disable();
        stat->threadTime = Load_module->swiEnv.totalTime;
        stat->totalTime = Load_module->swiEnv.totalTimeElapsed;
        Hwi_restore(key);
        return (TRUE);
    }
    else {
        return (FALSE);
    }
}

/*
 *  ======== Load_getTaskLoad ========
 */
Bool Load_getTaskLoad(Task_Handle task, Load_Stat *stat)
{
    UInt key;
    Load_HookContext *pTaskEnv;

    if (Load_taskEnabled == FALSE) {
        return FALSE;
    }
    else {
        pTaskEnv = (Load_HookContext *)Task_getHookContext(task,
            Load_module->taskHId);

        if (pTaskEnv != NULL) {
            key = Hwi_disable();
            stat->threadTime = pTaskEnv->totalTime;
            stat->totalTime = pTaskEnv->totalTimeElapsed;
            Hwi_restore(key);
            return (TRUE);
        }
        else {
            return (FALSE);   /* Task was not registered */
        }
    }
}

/*
 *  ======== Load_setMinIdle ========
 */
UInt32 Load_setMinIdle(UInt32 new)
{
    UInt32 old = Load_module->minIdle;
    Load_module->minIdle = new;
    return (old);
}

/*
 *  ======== Load_startup ========
 */
Void Load_startup()
{
    UInt32 curTime;
    Queue_Elem *qElem;
    Load_HookContext *pTaskEnv;

    /* Update each task hook context */
    if (Load_taskEnabled) {
        qElem = Queue_head(Load_Module_State_taskList());
        while (qElem != (Queue_Elem *)Load_Module_State_taskList()) {
            pTaskEnv = (Load_HookContext *)Task_getHookContext(
                ((Load_HookContext *)qElem)->threadHandle,
                Load_module->taskHId);
            Assert_isTrue(pTaskEnv == (Load_HookContext *)qElem, NULL);

            curTime = Timestamp_get32();
            pTaskEnv->timeOfLastUpdate = curTime;

            qElem = Queue_next(qElem);
        }
    }
}

/*
 *  ======== Load_updateCurrentThreadTime ========
 */
Void Load_updateCurrentThreadTime(Void)
{
    UInt32 curTime;
    UInt32 delta;
    UInt key;
    Load_HookContext *pTaskEnv;
    UInt coreId;

    if (BIOS_smpEnabled == TRUE) {
        coreId = Core_getId();
    }
    else {
        coreId = 0;
    }

    /*
     * Finalize the current time slot by updating the current thread's
     * counters to account for the time we have spent in it before calling
     * update().
     *
     * There is still a very small time delta unaccounted for before the
     * current thread's record get updated. But that is expected to be very
     * small compared to the benchmark time window, and can thus be ignored.
     */
    key = Hwi_disable();
    /* Get the current system time */
    curTime = Timestamp_get32();
    switch (BIOS_getThreadType()) {
        case BIOS_ThreadType_Hwi:
            if (Load_hwiEnabled) {
                /* Update the total time the hwi has run */
                delta = curTime - Load_module->hwiStartTime;
                Load_module->hwiEnv.nextTotalTime += delta;

                /* Update the start time for current hwi */
                Load_module->hwiStartTime = curTime;
            }
            break;
        case BIOS_ThreadType_Swi:
            if (Load_swiEnabled) {
                /* Update the total time the swi has run */
                delta = curTime - Load_module->swiStartTime;
                Load_module->swiEnv.nextTotalTime += delta;

                /* Update the start time for current swi */
                Load_module->swiStartTime = curTime;
            }
            break;
        case BIOS_ThreadType_Task:
            if (Load_taskEnabled) {
                /* Update the total time the task has run */
                delta = curTime - Load_module->taskStartTime[coreId];

                /* record it if task has been registered */
                pTaskEnv = (Load_HookContext *)Task_getHookContext(Task_self(),
                    Load_module->taskHId);
                if (pTaskEnv != NULL) {
                    pTaskEnv->nextTotalTime += delta;
                }

                /* Update the start time for current task */
                Load_module->taskStartTime[coreId] = curTime;
            }
            break;
        default: /* We are in the idle loop with no Task_idle */
            break;
    }

    /*
     * Update timeElapsed.
     *
     * Note that we are not updating minloop here, since update might have been
     * called outside the idle loop, and we haven't yet made an entire trip
     * around the loop yet. Also, we are not counting the current trip through
     * the idle loop as idle time, which is a small error assuming minloop is
     * small relative to the benchmark time window.
     */
    delta = curTime - Load_module->t0;
    Load_module->t0 = curTime;

    if (!Load_module->powerEnabled) {
        Load_module->timeElapsed += delta;
    }
    Hwi_restore(key);
}

/*
 *  ======== Load_updateThreadContexts ========
 */
Void Load_updateThreadContexts()
{
    UInt32 curTime;
    UInt key, taskKey;
    Queue_Elem *qElem;
    Load_HookContext *pTaskEnv;

    /* Update each task hook context */
    if (Load_taskEnabled) {
        taskKey = Task_disable();
        qElem = Queue_head(Load_Module_State_taskList());
        while (qElem != (Queue_Elem *)Load_Module_State_taskList()) {
            pTaskEnv = (Load_HookContext *)Task_getHookContext(
                ((Load_HookContext *)qElem)->threadHandle,
                Load_module->taskHId);
            Assert_isTrue(pTaskEnv == (Load_HookContext *)qElem, NULL);

            key = Hwi_disable();
            curTime = Timestamp_get32();
            pTaskEnv->totalTime = pTaskEnv->nextTotalTime;
            pTaskEnv->nextTotalTime = 0; /* Reset working counter */
            pTaskEnv->totalTimeElapsed = curTime -
                pTaskEnv->timeOfLastUpdate;
            pTaskEnv->timeOfLastUpdate = curTime;

            Hwi_restore(key);

            qElem = Queue_next(qElem);
        }
        Task_restore(taskKey);
    }

    Load_module->timeSlotCnt++;

    /* Update Global Hwi and Swi Hook Contexts atomically */
    key = Hwi_disable();
    curTime = Timestamp_get32();

    if (Load_swiEnabled) {
        Load_module->swiEnv.totalTime =
            Load_module->swiEnv.nextTotalTime;
        Load_module->swiEnv.nextTotalTime = 0;
        Load_module->swiEnv.totalTimeElapsed = curTime -
            Load_module->swiEnv.timeOfLastUpdate;
        Load_module->swiEnv.timeOfLastUpdate = curTime;
    }

    if (Load_hwiEnabled) {
        Load_module->hwiEnv.totalTime =
            Load_module->hwiEnv.nextTotalTime;
        Load_module->hwiEnv.nextTotalTime = 0;
        Load_module->hwiEnv.totalTimeElapsed = curTime -
            Load_module->hwiEnv.timeOfLastUpdate;
        Load_module->hwiEnv.timeOfLastUpdate = curTime;
    }

    Hwi_restore(key);

    if (Diags_query(Diags_USER4)) {
        Load_logLoads();
    }
}


/*
 *  ======== Load_updateLoads ========
 *  Update Task, Swi, Hwi loads if enabled, and update CPU load.
 */
Void Load_updateLoads(Void)
{
    UInt key;
    Load_FuncPtr postUpdate;
    Load_HookContext *pTaskEnv;
    UInt32 idle;

    postUpdate = Load_postUpdate; /* from config */

    Load_updateCurrentThreadTime();

    /* Update Global CPU load */
    idle = Load_module->minLoop;
    if (idle < Load_module->minIdle) {
        idle = Load_module->minIdle;
    }

    key = Hwi_disable();

    /* If task load is enabled, use the idle task time for the idle time. */
    if (Load_taskEnabled) {
        pTaskEnv = (Load_HookContext *)Task_getHookContext(Task_getIdleTask(),
                Load_module->taskHId);
        idle = pTaskEnv->nextTotalTime;
    }
    else {
        idle = Load_module->idleCnt * idle;
    }

    if (idle > Load_module->timeElapsed) {
        idle = Load_module->timeElapsed;
    }
    if (Load_module->timeElapsed > ((UInt32)1 << 24)) {
        /* if total's too big, we scale it down to avoid overflow */
        Load_module->cpuLoad = (
            (100 * ((Load_module->timeElapsed - idle) >> 7))
                + (Load_module->timeElapsed >> 8)
            ) / (Load_module->timeElapsed >> 7);
    }
    else if (Load_module->timeElapsed != 0) {
        Load_module->cpuLoad =
            100 - ((100 * idle + (Load_module->timeElapsed >> 1))
               / Load_module->timeElapsed);
    }
    else {
        Load_module->cpuLoad = 0;
    }

    Load_module->idleCnt = Load_module->timeElapsed = 0;

    Hwi_restore(key);

    /*
     *  Update each task hook context, Swi, Hwi hook contexts, and Log
     *  the thread loads.
     */
    Load_updateThreadContexts();

    if (Diags_query(Diags_USER4)) {
        Load_logCPULoad();
    }

    if (postUpdate != NULL) {
        postUpdate();   /* Call user-defined postUpdate fxn */
    }
}

/*
 *  ======== Load_updateContextsAndPost ========
 *  Update the thread times and call the postUpdate() function.  Do not
 *  do the CPU load calculation.  This function is called by Load_update()
 *  when Load.enableCPULoadCalc is false.
 */
Void Load_updateContextsAndPost()
{
    Load_FuncPtr postUpdate;

    postUpdate = Load_postUpdate; /* from config */

    Load_updateCurrentThreadTime();
    Load_module->idleCnt = Load_module->timeElapsed = 0;

    /*
     *  Update each task hook context, Swi, Hwi hook contexts, and Log
     *  the thread loads.
     */
    Load_updateThreadContexts();

    if (postUpdate != NULL) {
        postUpdate();   /* Call user-defined postUpdate fxn */
    }
}

/*
 *  ======== Load_reset ========
 */
Void Load_reset(Void)
{
    UInt32 curTime;
    UInt key, taskKey;
    Queue_Elem *qElem;
    Load_HookContext *pTaskEnv;
    UInt coreId;

    if (BIOS_smpEnabled == TRUE) {
        coreId = Core_getId();
    }
    else {
        coreId = 0;
    }

    /* Reset each task hook context */
    if (Load_taskEnabled) {
        taskKey = Task_disable();
        qElem = Queue_head(Load_Module_State_taskList());
        while (qElem != (Queue_Elem *)Load_Module_State_taskList()) {
            pTaskEnv = (Load_HookContext *)Task_getHookContext(
                ((Load_HookContext *)qElem)->threadHandle,
                Load_module->taskHId);
            Assert_isTrue(pTaskEnv == (Load_HookContext *)qElem, NULL);
            key = Hwi_disable();
            curTime = Timestamp_get32();
            pTaskEnv->totalTime = pTaskEnv->nextTotalTime = 0;
            pTaskEnv->totalTimeElapsed = 0;
            pTaskEnv->timeOfLastUpdate = curTime;
            if (pTaskEnv->threadHandle == Task_self()) {
                /*
                 * Done to ensure the start time of current task is >= its
                 * timeOfLastUpdate, i.e. its current time slot's start time
                 */
                Load_module->taskStartTime[coreId] = curTime;
            }
            Hwi_restore(key);
            qElem = Queue_next(qElem);
        }
        Task_restore(taskKey);
    }

    key = Hwi_disable();
    curTime = Timestamp_get32();

    /* Reset Swi hook context */
    if (Load_swiEnabled) {
        Load_module->swiEnv.totalTime = 0;
        Load_module->swiEnv.nextTotalTime = 0;
        Load_module->swiEnv.totalTimeElapsed = 0;
        Load_module->swiEnv.timeOfLastUpdate = curTime;
        /* If in Swi or Hwi, time slot starts now. Otherwise we don't care */
        Load_module->swiStartTime = curTime;
    }

    /* Reset Hwi hook context */
    if (Load_hwiEnabled) {
        Load_module->hwiEnv.totalTime = 0;
        Load_module->hwiEnv.nextTotalTime = 0;
        Load_module->hwiEnv.totalTimeElapsed = 0;
        Load_module->hwiEnv.timeOfLastUpdate = curTime;
        /* If in Swi or Hwi, time slot starts now. Otherwise we don't care */
        Load_module->hwiStartTime = curTime;
    }

    /* Reset counters for computing global CPU load */
    Load_module->timeElapsed = 0;
    Load_module->minLoop = 0xffffffff;
    Load_module->idleCnt = 0;
    Load_module->t0 = curTime;

    Load_module->busyTime = 0;
    Load_module->idleStartTime = 0;
    Load_module->busyStartTime = 0;

    Hwi_restore(key);
}
