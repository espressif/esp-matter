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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/runtime/Memory.h>
#include <ti/sysbios/runtime/Startup.h>
#include <ti/sysbios/runtime/Timestamp.h>
#include <ti/sysbios/runtime/Types.h>
#include <ti/sysbios/runtime/Assert.h>
#include <ti/sysbios/runtime/Error.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/gates/GateMutex.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/utils/Load.h>

#if 0
#include <ti/utils/runtime/Log.h>
#else
#include <ti/sysbios/runtime/Log.h>
#endif

Load_Module_State Load_Module_state = {
    .taskHId = 0,
    .taskStartTime = 0,
    .runningTask = NULL,
    .timeElapsed = 0,
    .firstSwitchDone = false,
    .swiStartTime = 0,
    .swiEnv.totalTimeElapsed = 0,
    .swiEnv.totalTime = 0,
    .swiEnv.nextTotalTime = 0,
    .swiEnv.timeOfLastUpdate = 0,
    .swiEnv.threadHandle = NULL,
    .swiCnt = 0,
    .hwiStartTime = 0,
    .hwiEnv.totalTimeElapsed = 0,
    .hwiEnv.totalTime = 0,
    .hwiEnv.nextTotalTime = 0,
    .hwiEnv.timeOfLastUpdate = 0,
    .hwiEnv.threadHandle = NULL,
    .hwiCnt = 0,
    .timeSlotCnt = 0,
    .minLoop = 0xFFFFFFFF,
    .t0 = 0,
    .idleCnt = 0,
    .cpuLoad = 0,
};

/*
 *************************************************************************
 *                       Internal functions
 *************************************************************************
 */

/*
 *  ======== Load_init ========
 */
void Load_init(void)
{
    Queue_construct(&Load_module->taskList, NULL);

    Queue_elemClear(&Load_module->swiEnv.qElem);
    Queue_elemClear(&Load_module->hwiEnv.qElem);
}

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
void Load_taskRegHook(int id)
{
    Load_module->taskHId = id;
}

/*
 *  ======== Load_taskCreateHook ========
 *  Create and initialize the HookContext for this Task.
 */
void Load_taskCreateHook(Task_Handle task, Error_Block *eb)
{
    Load_HookContext *context = NULL;

    /* Create the HookContext for this Task. */
    context = Memory_calloc(Task_Object_heap(), sizeof(Load_HookContext),
            0, eb);
    if (context == NULL) {
        return;
    }
    
    Task_setHookContext(task, Load_module->taskHId, NULL);

    /* Initialize the HookContext for this Task. */
    if (Load_autoAddTasks_D == true) {
        Load_addTask(task, context);
    }
}

/*
 *  ======== Load_taskDeleteHook ========
 */
void Load_taskDeleteHook(Task_Handle task)
{
    Load_HookContext *pStoredContext;

    pStoredContext =
        (Load_HookContext *)Task_getHookContext(task, Load_module->taskHId);

    if (Load_autoAddTasks_D == true) {
        Load_removeTask(task);
    }

    Memory_free(Task_Object_heap(), pStoredContext,
            sizeof(Load_HookContext));
}

/*
 *  ======== Load_taskSwitchHook ========
 */
void Load_taskSwitchHook(Task_Handle curTask, Task_Handle nextTask)
{
    uint32_t curTime;
    uint32_t delta;
    unsigned int key;
    Load_HookContext *pCurTaskEnv;

    /* curTask is invalid during 1st Task switch */
    if (curTask == NULL) {

        /* Get the current system time */
        curTime = Timestamp_get32();

        /* Update the start time for next task */
        Load_module->taskStartTime = curTime;

        /*
         * record Task handle, in case hwi or swi gets run before next switch
         * note that time for the dummy task may be counted towards 1st task.
         * We chose to do this to simplify code.
         */
        Load_module->runningTask = nextTask;
        return;
    }

    key = Hwi_disable();

    /* Get the current system time */
    curTime = Timestamp_get32();

    /* Update the total time the task has run */
    delta = curTime - Load_module->taskStartTime;

    /* record it if task has been registered */
    pCurTaskEnv = (Load_HookContext *)Task_getHookContext(curTask,
        Load_module->taskHId);
    if (pCurTaskEnv != NULL) {
        pCurTaskEnv->nextTotalTime += delta;
    }

    /* Update the start time for next task */
    Load_module->taskStartTime = curTime;

    /* record Task handle */
    Load_module->runningTask = nextTask;
    Hwi_restore(key);
}

/*
 *  ======== Load_swiBeginHook ========
 */
void Load_swiBeginHook(Swi_Handle swi)
{
    uint32_t curTime;
    uint32_t delta;
    unsigned int key;
    Load_HookContext *pTaskEnv;

    key = Hwi_disable();

    ++(Load_module->swiCnt);

    if (Load_module->swiCnt == 1) {   /* If first SWI */

        /* Get the current system time */
        curTime = Timestamp_get32();

        /* Update the total time the task has run */
        delta = curTime - Load_module->taskStartTime;

        /*
         * may be NULL if interrupt happened early before first task
         * switch, or if task monitoring is disabled.
         */
        if ((Load_taskEnabled_D == true) && (Load_module->runningTask != NULL)) {

            /* record it if task has been registered */
            pTaskEnv = (Load_HookContext *)Task_getHookContext(
                Load_module->runningTask, Load_module->taskHId);
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
void Load_swiEndHook(Swi_Handle swi)
{
    uint32_t curTime;
    uint32_t delta;
    unsigned int key;

    key = Hwi_disable();  /* read swiStartTime atomically after decrement */

    --(Load_module->swiCnt);

    if (Load_module->swiCnt == 0) {  /* If last Swi */
        /* Get the current system time */
        curTime = Timestamp_get32();

        /* Update the total time the swi has run */
        delta = curTime - Load_module->swiStartTime;
        Load_module->swiEnv.nextTotalTime += delta;

        /* Update the start time for next task */
        Load_module->taskStartTime = curTime;
    }

    Hwi_restore(key);
}

/*
 *  ======== Load_hwiEndHook ========
 */
void Load_hwiEndHook(Hwi_Handle hwi)
{
    uint32_t curTime;
    uint32_t delta;
    unsigned int key;

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
            Load_module->taskStartTime = curTime;
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
void Load_addTask(Task_Handle task, Load_HookContext *env)
{
    unsigned int taskKey;
    Load_HookContext *pCurTaskEnv;

    if (Load_taskEnabled_D) {
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
        env->threadHandle = (void *)task;

        Task_setHookContext(task, Load_module->taskHId, env);

        Queue_enqueue(&Load_module->taskList,(Queue_Elem *)env);

        Task_restore(taskKey);
    }
}

/*
 *  ======== Load_removeTask ========
 */
bool Load_removeTask(Task_Handle task)
{
    unsigned int taskKey;
    Load_HookContext *pCurTaskEnv;

    if (Load_taskEnabled_D) {
        taskKey = Task_disable();
        pCurTaskEnv = (Load_HookContext *)
            Task_getHookContext(task, Load_module->taskHId);
        if (pCurTaskEnv == NULL) { /* If task is not registered, return fail */
            Task_restore(taskKey);
            return (false);
        }
        Queue_remove((Queue_Elem *)pCurTaskEnv);

        /*
         * could think of taking this outside the critical section, but
         * it's here in case a higher pri thread adds this task right
         * back (though very unlikely)
         */
        Task_setHookContext(task, Load_module->taskHId, NULL);

        Task_restore(taskKey);
        return (true);
     }
     else {
        return (false);
     }
}

/*
 *  ======== Load_idleFxn ========
 */
void Load_idleFxn(void)
{
    Types_FreqHz freq;
    uint32_t window;
    uint32_t t1;
    uint32_t delta;
    unsigned int key;

    /*
     *  We do this initialization in the idle loop in case the timestamp
     *  frequency changes (e.g due to frequency scaling), in which case the
     *  window would be automatically adjusted.
     */
    Timestamp_getFreq(&freq);
    Assert_isTrue(freq.hi == 0, NULL); /* Does not support CPU over 4 GHz */
    window = (freq.lo / 1000) * Load_windowInMs_D;

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
    if ((Load_module->timeElapsed >= window) && (Load_updateInIdle_D)) {
        Load_update();
    }
}

/*
 *  ======== Load_logCPULoad ========
 */
void Load_logCPULoad(void)
{
    /* Log global CPU load */
    Log_write1(Load_LS_cpuLoad, (intptr_t)Load_getCPULoad());
}

/*
 *  ======== Load_logLoads ========
 */
void Load_logLoads(void)
{
    Task_Handle task;
    Queue_Elem *qElem;
    Load_Stat stat;
    unsigned int taskKey;

    /* Log Hwi load */
    if ((Load_hwiEnabled_D) && (Load_getGlobalHwiLoad(&stat) == true)) {
        Log_write2(Load_LS_hwiLoad, (intptr_t) stat.threadTime,
                   (intptr_t) stat.totalTime);
    }

    /* Log Swi load */
    if ((Load_swiEnabled_D) && (Load_getGlobalSwiLoad(&stat) == true)) {
        Log_write2(Load_LS_swiLoad, (intptr_t) stat.threadTime,
                   (intptr_t) stat.totalTime);
    }

    /* Log Task load for each Task */
    if (Load_taskEnabled_D) {

        /* Protect access to Load module's taskList */
        taskKey = Task_disable();

        qElem = Queue_head(&Load_module->taskList);

        /* For each task... */
        while (qElem != (Queue_Elem *)&Load_module->taskList) {
            task = ((Load_HookContext *)qElem)->threadHandle;

            /* Log the Task load. */
            if (Load_getTaskLoad(task, &stat) == true) {
                Log_write4(Load_LS_taskLoad, (intptr_t)task, (intptr_t)stat.threadTime,
                           (intptr_t)stat.totalTime, (intptr_t)task->fxn);
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
uint32_t Load_calculateLoad(Load_Stat *stat)
{
    uint32_t load, divisor;

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
uint32_t Load_getCPULoad(void)
{
    return (Load_module->cpuLoad);
}

/*
 *  ======== Load_getGlobalHwiLoad ========
 */
bool Load_getGlobalHwiLoad(Load_Stat *stat)
{
    unsigned int key;

    if (Load_hwiEnabled_D) {
        key = Hwi_disable();
        stat->threadTime = Load_module->hwiEnv.totalTime;
        stat->totalTime = Load_module->hwiEnv.totalTimeElapsed;
        Hwi_restore(key);
        return (true);
    }
    else {
        return (false);
    }
}

/*
 *  ======== Load_getGlobalSwiLoad ========
 */
bool Load_getGlobalSwiLoad(Load_Stat *stat)
{
    unsigned int key;

    if (Load_swiEnabled_D) {
        key = Hwi_disable();
        stat->threadTime = Load_module->swiEnv.totalTime;
        stat->totalTime = Load_module->swiEnv.totalTimeElapsed;
        Hwi_restore(key);
        return (true);
    }
    else {
        return (false);
    }
}

/*
 *  ======== Load_getTaskLoad ========
 */
bool Load_getTaskLoad(Task_Handle task, Load_Stat *stat)
{
    unsigned int key;
    Load_HookContext *pTaskEnv;

    if (Load_taskEnabled_D == false) {
        return false;
    }
    else {
        pTaskEnv = (Load_HookContext *)Task_getHookContext(task,
            Load_module->taskHId);

        if (pTaskEnv != NULL) {
            key = Hwi_disable();
            stat->threadTime = pTaskEnv->totalTime;
            stat->totalTime = pTaskEnv->totalTimeElapsed;
            Hwi_restore(key);
            return (true);
        }
        else {
            return (false);   /* Task was not registered */
        }
    }
}

/*
 *  ======== Load_setMinIdle ========
 */
uint32_t Load_setMinIdle(uint32_t new)
{
    uint32_t old = Load_module->minIdle;
    Load_module->minIdle = new;
    return (old);
}

/*
 *  ======== Load_startup ========
 */
void Load_startup()
{
    uint32_t curTime;
    Queue_Elem *qElem;
    Load_HookContext *pTaskEnv;

    /* Update each task hook context */
    if (Load_taskEnabled_D) {
        qElem = Queue_head(&Load_module->taskList);
        while (qElem != (Queue_Elem *)&Load_module->taskList) {
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
void Load_updateCurrentThreadTime(void)
{
    uint32_t curTime;
    uint32_t delta;
    unsigned int key;
    Load_HookContext *pTaskEnv;

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
            if (Load_hwiEnabled_D) {
                /* Update the total time the hwi has run */
                delta = curTime - Load_module->hwiStartTime;
                Load_module->hwiEnv.nextTotalTime += delta;

                /* Update the start time for current hwi */
                Load_module->hwiStartTime = curTime;
            }
            break;
        case BIOS_ThreadType_Swi:
            if (Load_swiEnabled_D) {
                /* Update the total time the swi has run */
                delta = curTime - Load_module->swiStartTime;
                Load_module->swiEnv.nextTotalTime += delta;

                /* Update the start time for current swi */
                Load_module->swiStartTime = curTime;
            }
            break;
        case BIOS_ThreadType_Task:
            if (Load_taskEnabled_D) {
                /* Update the total time the task has run */
                delta = curTime - Load_module->taskStartTime;

                /* record it if task has been registered */
                pTaskEnv = (Load_HookContext *)Task_getHookContext(Task_self(),
                    Load_module->taskHId);
                if (pTaskEnv != NULL) {
                    pTaskEnv->nextTotalTime += delta;
                }

                /* Update the start time for current task */
                Load_module->taskStartTime = curTime;
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

    Load_module->timeElapsed += delta;

    Hwi_restore(key);
}

/*
 *  ======== Load_updateThreadContexts ========
 */
void Load_updateThreadContexts()
{
    uint32_t curTime;
    unsigned int key, taskKey;
    Queue_Elem *qElem;
    Load_HookContext *pTaskEnv;

    /* Update each task hook context */
    if (Load_taskEnabled_D) {
        taskKey = Task_disable();
        qElem = Queue_head(&Load_module->taskList);
        while (qElem != (Queue_Elem *)&Load_module->taskList) {
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

    if (Load_swiEnabled_D) {
        Load_module->swiEnv.totalTime =
            Load_module->swiEnv.nextTotalTime;
        Load_module->swiEnv.nextTotalTime = 0;
        Load_module->swiEnv.totalTimeElapsed = curTime -
            Load_module->swiEnv.timeOfLastUpdate;
        Load_module->swiEnv.timeOfLastUpdate = curTime;
    }

    if (Load_hwiEnabled_D) {
        Load_module->hwiEnv.totalTime =
            Load_module->hwiEnv.nextTotalTime;
        Load_module->hwiEnv.nextTotalTime = 0;
        Load_module->hwiEnv.totalTimeElapsed = curTime -
            Load_module->hwiEnv.timeOfLastUpdate;
        Load_module->hwiEnv.timeOfLastUpdate = curTime;
    }

    Hwi_restore(key);

    /* TODO: Should add this back when Log/Diags is available */
    /*if (Diags_query(Diags_USER4)) {
        Load_logLoads();
    }*/
}


/*
 *  ======== Load_updateLoads ========
 *  Update Task, Swi, Hwi loads if enabled, and update CPU load.
 */
void Load_updateLoads(void)
{
    unsigned int key;
    Load_FuncPtr postUpdate;
    Load_HookContext *pTaskEnv;
    uint32_t idle;

    postUpdate = Load_postUpdate_D; /* from config */

    Load_updateCurrentThreadTime();

    /* Update Global CPU load */
    idle = Load_module->minLoop;
    if (idle < Load_module->minIdle) {
        idle = Load_module->minIdle;
    }

    key = Hwi_disable();

    /* If task load is enabled, use the idle task time for the idle time. */
    if (Load_taskEnabled_D) {
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
    if (Load_module->timeElapsed > ((uint32_t)1 << 24)) {
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

    /* TODO: Should add this back when Log/Diags is available */
    /*if (Diags_query(Diags_USER4)) {
        Load_logCPULoad();
    }*/

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
void Load_updateContextsAndPost()
{
    Load_FuncPtr postUpdate;

    postUpdate = Load_postUpdate_D; /* from config */

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
void Load_reset(void)
{
    uint32_t curTime;
    unsigned int key, taskKey;
    Queue_Elem *qElem;
    Load_HookContext *pTaskEnv;

    /* Reset each task hook context */
    if (Load_taskEnabled_D) {
        taskKey = Task_disable();
        qElem = Queue_head(&Load_module->taskList);
        while (qElem != (Queue_Elem *)&Load_module->taskList) {
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
                Load_module->taskStartTime = curTime;
            }
            Hwi_restore(key);
            qElem = Queue_next(qElem);
        }
        Task_restore(taskKey);
    }

    key = Hwi_disable();
    curTime = Timestamp_get32();

    /* Reset Swi hook context */
    if (Load_swiEnabled_D) {
        Load_module->swiEnv.totalTime = 0;
        Load_module->swiEnv.nextTotalTime = 0;
        Load_module->swiEnv.totalTimeElapsed = 0;
        Load_module->swiEnv.timeOfLastUpdate = curTime;
        /* If in Swi or Hwi, time slot starts now. Otherwise we don't care */
        Load_module->swiStartTime = curTime;
    }

    /* Reset Hwi hook context */
    if (Load_hwiEnabled_D) {
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

    Hwi_restore(key);
}

/*
 *  ======== Load_update ========
 */
void Load_update(void)
{
    if (Load_enableCPULoadCalc_D) {
        /*
         * Update and Log CPU load, Task, Swi, and Hwi loads
         * (if enabled).
         */
        Load_updateLoads();
    }
    else {
        /* Update thread contexts and call post update function. */
        Load_updateContextsAndPost();
    }
}

/*
 *  ======== Load_hwiBeginHook ========
 */
void Load_hwiBeginHook(Hwi_Handle hwi)
{
    unsigned int key;

    key = Hwi_disable();
    if (Load_hwiEnabled_D) {
        uint32_t delta;

        if (++(Load_module->hwiCnt) == 1) {   /* If first Hwi */
            /* Get the interrupt time */
            uint32_t intrTime = Timestamp_get32();

            if (Load_module->swiCnt == 0) {  /* we were in a Task */
                /* Update the total time the task has run */
                delta = intrTime - Load_module->taskStartTime;
                if (Load_taskEnabled_D) {
                    Load_HookContext *pTaskEnv;

                    /*
                     * may be NULL if interrupt happened early before first task
                     * switch, or if task monitoring is disabled.
                     */
                    if (Load_module->runningTask != NULL) {
                        pTaskEnv = (Load_HookContext *)Task_getHookContext(
                            Load_module->runningTask,
                            Load_module->taskHId);

                        /* record time, if task has been registered */
                        if (pTaskEnv != NULL) {
                            pTaskEnv->nextTotalTime += delta;
                        }
                    }
                }
            }
            else { /* we were in a Swi */
                /* Update the total time the task has run */
                delta = intrTime - Load_module->swiStartTime;

                /* record it */
                Load_module->swiEnv.nextTotalTime += delta;
            }
            /* Update the start time for next hwi */
            Load_module->hwiStartTime = intrTime;
        }

    }

    Hwi_restore(key);
}
