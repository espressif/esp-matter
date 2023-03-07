/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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
 *  ======== Task_smp.c ========
 */

#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Diags.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Core.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Idle.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Intrinsics.h>

#include "package/internal/Task.xdc.h"

#if defined(__ti__) && !defined(__clang__)
/* disable unused local variable warning during optimized compile */
#pragma diag_suppress=179
#endif

#define Task_moduleSmp  \
    ((Task_Module_StateSmp *)(&ti_sysbios_knl_Task_Module__stateSmp__V))
#define Task_moduleStateCheckValue   \
    ti_sysbios_knl_Task_moduleStateCheckValue

extern Task_Module_StateSmp ti_sysbios_knl_Task_Module__stateSmp__V;
#if ((defined xdc_target__isaCompatible_64) || \
     (defined xdc_target__isaCompatible_64P) || \
     (defined xdc_target__isaCompatible_66) || \
     (defined xdc_target__isaCompatible_67) || \
     (defined xdc_target__isaCompatible_67P) || \
     (defined xdc_target__isaCompatible_674))
__extern __FAR__ UInt32 ti_sysbios_knl_Task_moduleStateCheckValue;
#else
extern UInt32 ti_sysbios_knl_Task_moduleStateCheckValue;
#endif

#define SORT_RUNQ(newPri, coreId) {                                     \
    /*                                                                  \
     * Iterate through at most Core_NUM_CORES queue elems once to       \
     * find the right position for a queue elem with priority newPri    \
     */                                                                 \
    Task_RunQEntry *runQIter =                                          \
         Queue_head(Task_moduleSmp->sortedRunQ);                        \
                                                                        \
    while (((Queue_Elem *)runQIter !=                                   \
            (Queue_Elem *)(Task_moduleSmp->sortedRunQ)) &&              \
            (runQIter->priority < newPri)){                             \
        runQIter = Queue_next((Queue_Elem *)runQIter);                  \
    }                                                                   \
                                                                        \
    if ((Queue_Elem *)runQIter !=                                       \
        (Queue_Elem *)&(Task_moduleSmp->smpRunQ[coreId])) {             \
        /* Remove entry from queue */                                   \
        Queue_remove((Queue_Elem *)&(Task_moduleSmp->smpRunQ[coreId])); \
                                                                        \
        /* Insert entry to new position */                              \
        Queue_insert((Queue_Elem *)runQIter,                            \
            (Queue_Elem *)&(Task_moduleSmp->smpRunQ[coreId]));          \
    }                                                                   \
                                                                        \
    /* Update priority */                                               \
    (Task_moduleSmp->smpRunQ[coreId]).priority = newPri;                \
}

/*
 *  Task Scheduling Rules
 *  =====================
 *
 *  - The Highest Priority ready task is always running.
 *  - The N Highest Priority ready tasks will be running at any given
 *    time provided task affinities are not used (where N is the number
 *    of cores).
 *
 *  Task Scheduling Goals
 *  =====================
 *
 *  - Automatic Load Balancing
 *      The scheduler should distribute tasks on the different processing
 *      cores to ensure CPU utilization is as high as possible.
 *
 *      If a given core becomes idle, the scheduler should immediately
 *      kick in, detect this and automatically schedule a ready task
 *      (pending work) on this idle core, thereby increasing CPU
 *      utilization.
 *
 *      There would be a need for some cooperation from the developer.
 *      In particular, the developer must:
 *       - Not use task affinities and priorities in such a way that it
 *         prevents the scheduler from distributing tasks between cores
 *         and keeping the CPU load balanced. In other words, let all
 *         tasks run with don't care affinity whenever possible.
 *       - Appropriately partition the workload between tasks.
 *
 *  - Minimize task migration to improve cache performance and reduce
 *    the scheduling overhead.
 *
 *  - Support hard processor affinity
 *      Developers should be able to specify hard processor affinity
 *      for each task. The task affinity feature can be leveraged to
 *      get better cache performance as well as implementing AMP
 *      alongside SMP.
 *
 *      Here's an example priority and affinity assignment scheme that
 *      can be followed to achieve AMP alongside SMP and in the process
 *      guarantee good cache performance for tasks running in AMP mode:
 *       - Assume we have a 4 core system and a total of 16 different
 *         priorities.
 *       - The lower 8 priorities (i.e. priority 1-8) can be reserved
 *         for don't care affinity tasks while the upper 7 priorities
 *         (i.e. priority 9-15) are reserved for fixed affinity tasks.
 *       - Now, say we have 6 tasks in all out of which 2 are timing
 *         critical and should have a fast response time. We assign a
 *         priority in the 9-15 range to these tasks and assign them
 *         an affinity of 2 and 3 respectively. This will allow them
 *         to always run on core 2 and core 3 without any interference
 *         from other tasks. The lesser critical tasks are assigned a
 *         priority between 1-8 and are left as don't care.
 *       - When the system starts up, the 2 timing-critical tasks will
 *         always run on Core 2 & Core 3, and perform well as their
 *         L1 cache is always warm. The 4 lesser critical tasks are
 *         running on one of the 2 other cores, namely, core 0 or 1.
 *         Since these are don't care affinity, they can migrate from
 *         one core to the other depending on which core is idle.
 *         Occasionally, they may run on Core 2 and Core 3 too, if the
 *         2 timing-critical fixed affinity tasks block, but due to the
 *         priority banding rule that we are enforcing, they will get
 *         pre-empted as soon as a fixed priority task becomes ready.
 *
 *  - Keep optimized implementation for 2 core case.
 *       - Benchmark the 2 approaches.
 */

/*
 *  ======== Task_schedule ========
 *  Find highest priority task and invoke it.
 *
 *  In SMP mode, the RUNNING task must be removed from its readyQ so that
 *  It doesn't appear to the other core as a task that could possibly be
 *  scheduled to run locally.
 *
 *  Must be called with interrupts disabled.
 */
Void Task_schedule()
{
    Queue_Handle maxQ;
    Task_Object *prevTask;
    Task_Object *curTask;
    Task_Object *readyQTask;
    Int newPri;
    UInt coreId;
    UInt curSetPriX;
    UInt curPriLocal;
    UInt curSetPriLocal;
    Task_RunQEntry *lowestPriRunQ;
    Bool earlyExit;
#ifdef ti_sysbios_knl_Task_ENABLE_SWITCH_HOOKS
    Int i;
#endif

    coreId = Core_getId();

    Log_write5(Task_LM_schedule,
                (UArg)coreId,
                (UArg)Task_module->workFlag,
                (UArg)Task_module->smpCurSet[coreId],
                (UArg)Task_module->smpCurSet[Core_numCores],
                (UArg)Task_module->smpCurMask[coreId]);

    do {
        Task_module->workFlag &= ~(1 << coreId);

readyTasksLoop:

        /* Translate the curset bitmasks into UInts 0-32 */
        curSetPriX = Task_module->smpCurSet[Core_numCores] ?
            Intrinsics_maxbit(Task_module->smpCurSet[Core_numCores]) + 1 : 0;
        curPriLocal = Task_module->smpCurMask[coreId] ?
            Intrinsics_maxbit(Task_module->smpCurMask[coreId]) + 1 : 0;
        curSetPriLocal = Task_module->smpCurSet[coreId] ?
            Intrinsics_maxbit(Task_module->smpCurSet[coreId]) + 1 : 0;

        /*
         * Check if either ready queues have a task with higher priority
         * than the currently running task on this core.
         */
        maxQ = NULL;
        earlyExit = FALSE;

        if ((curSetPriX > curPriLocal) && (curSetPriX > curSetPriLocal)) {
            newPri = curSetPriX - 1;
            maxQ = (Queue_Handle)((UInt8 *)(Task_module->smpReadyQ[Core_numCores]) +
                   (UInt)(newPri*(2*sizeof(Ptr))));
            lowestPriRunQ = Queue_head(Task_moduleSmp->sortedRunQ);

            if (curPriLocal != (lowestPriRunQ->priority + 1)) {
                earlyExit = TRUE;
                readyQTask = Queue_head(maxQ);

                /*
                 * Scan the entire newPri priority readyQ and find any task that
                 * can be run on this core.
                 */
                while (((Queue_Elem *)readyQTask != (Queue_Elem *)maxQ) &&
                       ((readyQTask->priority) > (lowestPriRunQ->priority))) {
                    /* force a Task switch */
                    Task_module->workFlag |= (1 << (lowestPriRunQ->coreId));

                    if (lowestPriRunQ->coreId == coreId) {
                        earlyExit = FALSE;
                        break;
                    }
                    else {
                        Core_interruptCore(lowestPriRunQ->coreId);
                    }

                    lowestPriRunQ = Queue_next((Queue_Elem *)lowestPriRunQ);
                    readyQTask = Queue_next((Queue_Elem *)readyQTask);
                }

                if (earlyExit) {
                    maxQ = NULL;
                }
            }
        }

        if (maxQ == NULL) {
            if (curSetPriLocal > curPriLocal) {
                newPri = curSetPriLocal - 1;
                maxQ = (Queue_Handle)((UInt8 *)(Task_module->smpReadyQ[coreId]) +
                       (UInt)(newPri*(2*sizeof(Ptr))));
            }
            else if (earlyExit) {
                /*
                 * If execution reaches here then, this core is sure to be
                 * running a task and is not idle. Therefore, no need to
                 * run idle loop and we should be able to return to the
                 * current task.
                 */
                return;
            }
            else if ((curSetPriLocal + curSetPriX + curPriLocal) == 0) {
                Task_allBlockedFunction();
                goto readyTasksLoop;
            }
        }

        if (maxQ != NULL) {
            prevTask = Task_module->smpCurTask[coreId];

            /* put pre-empted task back at the head of its readyQ */
            if (prevTask->mode == Task_Mode_RUNNING) {
                prevTask->mode = Task_Mode_READY;
                Queue_insert( ((Queue_Elem *)(prevTask->readyQ))->next,
                        (Queue_Elem *)prevTask);
                Task_module->smpCurSet[prevTask->affinity] |= prevTask->mask;
            }

            /* remove new curtask from its readyQ */
            curTask = Queue_dequeue(maxQ);

            /* adjust its readyQ curset accordingly */
            if (Queue_empty(maxQ)) {
                Task_module->smpCurSet[curTask->affinity] &= ~(curTask->mask);
            }

            Task_module->smpCurTask[coreId] = curTask;
            Task_module->smpCurMask[coreId] = curTask->mask;

            curTask->curCoreId = coreId;
            curTask->mode = Task_Mode_RUNNING;

            /* Update the Sorted RunQ */
            if ((Task_moduleSmp->smpRunQ[coreId]).priority != newPri) {
                SORT_RUNQ(newPri, coreId);
            }

            if (Task_checkStackFlag) {
                Task_checkStacks(prevTask, curTask);
            }

#if defined(ti_sysbios_knl_Task_ENABLE_SWITCH_HOOKS) \
    || (xdc_runtime_Log_DISABLE_ALL == 0)
            /* It's safe to enable intrs here */
            Core_hwiEnable();

#ifdef ti_sysbios_knl_Task_ENABLE_SWITCH_HOOKS
            for (i = 0; i < Task_hooks.length; i++) {
                if (Task_hooks.elem[i].switchFxn != NULL) {
                    Task_hooks.elem[i].switchFxn(prevTask, curTask);
                }
            }
#endif

            Log_write4(Task_LM_switch, (UArg)prevTask, (UArg)prevTask->fxn,
                       (UArg)curTask, (UArg)curTask->fxn);

            /* Hard-disable intrs - this fxn is called with them disabled */
            Core_hwiDisable();
#endif

            Task_SupportProxy_swap((Ptr)&prevTask->context,
                            (Ptr)&curTask->context);

            /* refresh coreIds since we may have switched cores in the swap */
            coreId = Core_getId();
        }
        else {
            Log_write4(Task_LM_noWork,
                (UArg)coreId,
                (UArg)Task_module->smpCurSet[coreId],
                (UArg)Task_module->smpCurSet[Core_numCores],
                (UArg)Task_module->smpCurMask[coreId]);
        }
    } while (Task_module->workFlag & (1 << coreId));

    /* Need to read the mask again as the task may have switched */
    curSetPriX = Task_module->smpCurSet[Core_numCores] ?
        Intrinsics_maxbit(Task_module->smpCurSet[Core_numCores]) + 1 : 0;

    /* Need to perform a check only if there are any don't care ready tasks */
    if (curSetPriX) {
        lowestPriRunQ = Queue_head(Task_moduleSmp->sortedRunQ);

        if (lowestPriRunQ->coreId == coreId) {
            /*
             * In case there are multiple Queue elems of same priority, we
             * need to ensure the current core is not selected.
             */
            lowestPriRunQ = Queue_next((Queue_Elem *)lowestPriRunQ);
        }

        if (curSetPriX > (lowestPriRunQ->priority + 1)) {
            /* force the other core's Hwi to do a Task switch */
            Task_module->workFlag |= (1 << (lowestPriRunQ->coreId));
            Core_interruptCore(lowestPriRunQ->coreId);
        }
    }
}

/*
 *  ======== Task_setPri ========
 */
Int Task_setPri(Task_Object *tsk, Int priority)
{
    Int oldPri;
    UInt newMask, tskKey, hwiKey, tskAffinity;
    Queue_Handle newQ;
    UInt coreId, curCoreId, otherCoreMask, i;

    Assert_isTrue((((priority == -1) || (priority > 0) ||
                  ((priority == 0 && Task_module->idleTask == NULL))) &&
                   (priority < (Int)Task_numPriorities)),
                   Task_A_badPriority);

    Log_write4(Task_LM_setPri, (UArg)tsk, (UArg)tsk->fxn,
                       (UArg)tsk->priority, (UArg)priority);

    tskKey = Task_disable();
    hwiKey = Core_hwiDisable();

    coreId = Core_getId();
    oldPri = tsk->priority;

    if (oldPri == priority) {
        Core_hwiRestore(hwiKey);
        Task_restore(tskKey);
        return (oldPri);
    }

    if (priority < 0) {
        newMask = 0;
        newQ = Task_Module_State_inactiveQ();
    }
    else {
        newMask = 1 << priority;
        newQ = (Queue_Handle)((UInt8 *)(Task_module->smpReadyQ[tsk->affinity]) +
                (UInt)(priority*(2*sizeof(Ptr))));
    }

    if (tsk->mode == Task_Mode_READY) {
        Queue_remove((Queue_Elem *)tsk);

        /* if last task in readyQ, remove corresponding bit in curSet */
        if (Queue_empty(tsk->readyQ)) {
            Task_module->smpCurSet[tsk->affinity] &= ~(tsk->mask);
        }

        /* place task at end of its readyQ */
        Queue_enqueue(newQ, (Queue_Elem *)tsk);

        tskAffinity = tsk->affinity;
        Task_module->smpCurSet[tskAffinity] |= newMask;

        if (tskAffinity == Core_numCores) {
            Task_module->workFlag |= (1 << coreId);
        }
        else if (newMask > Task_module->smpCurMask[tskAffinity]) {
            Task_module->workFlag |= (1 << tskAffinity);

            if (tskAffinity != coreId) {
                Core_interruptCore(tskAffinity);
            }
        }
    }

    tsk->priority = priority;
    tsk->mask = newMask;
    tsk->readyQ = newQ;
    curCoreId = tsk->curCoreId;

    /*
     * force a scheduling re-evaluation if the task is the currently
     * running task on any core
     */
    if (tsk->mode == Task_Mode_RUNNING) {
        /*
         * Update the Sorted RunQ now as the task switch may not happen
         * in which case this function needs to explicitly re-sort the
         * RunQ.
         */
        SORT_RUNQ(priority, curCoreId);

        if (curCoreId == coreId) {
            Task_module->smpCurMask[coreId] = newMask; /* force a Task switch */
            Task_module->workFlag |= (1 << coreId);
        }
        else {
            otherCoreMask = (1 << curCoreId);
            Task_module->smpCurMask[curCoreId] = newMask;
            Task_module->workFlag |= otherCoreMask;
            Core_interruptCore(curCoreId);

            /* Wait for scheduler to run */
            while ((Task_module->workFlag & otherCoreMask) != 0) {
                Task_enableOtherCores();

                /*
                 * Only other cores' task schedulers will run here.
                 * Leave window open long enough for other core to
                 * grab the intercore lock.
                 */
                for (i = 0; i < 20; i++) {
                    if ((Task_module->workFlag & otherCoreMask) == 0) {
                        break;
                    }
                }

                Task_disable();
                Core_hwiRestore(hwiKey); /* local interrupt latency reduction */
                hwiKey = Core_hwiDisable();
            }
        }
    }

    Core_hwiRestore(hwiKey);
    Task_restore(tskKey);

    return (oldPri);
}

/*
 *  ======== Task_setAffinity ========
 */
UInt Task_setAffinity(Task_Object *tsk, UInt newAffinity)
{
    UInt tskKey, hwiKey;
    Queue_Handle oldQ, newQ;
    UInt coreId, oldAffinity;
    UInt tskMask, otherCoreMask;

    Assert_isTrue((BIOS_getThreadType() == BIOS_ThreadType_Task), Task_A_badThreadType);

    Assert_isTrue((newAffinity < Core_numCores) ||
                  (newAffinity == Task_AFFINITY_NONE), Task_A_badAffinity);

    coreId = Core_getId();

    /*
     * case 0: oldAffinity == newAffinity
     * case 1: task is inactive (priority == -1)
     * case 2: task is ready (not running)
     * case 3: task is blocked
     * case 4: task is running on current core
     *         4a = setting affinity to don't care.
     *         4b = changing affinity from don't care to this core
     *         4c = setting affinity to a core other than current
     *              core (ie moving to new core)
     * case 5: task is running on other core
     */

    /* case 0: oldAffinity == newAffinity */
    if (newAffinity == Task_AFFINITY_NONE) {
        newAffinity = Core_numCores;
    }

    /*
     * KNOWN ISSUE:
     * Since tasking is enabled here, it is possible for this task to migrate
     * and the coreId to change. Therefore, the below log may occasionally show
     * an incorrect coreId.
     */
    Log_write5(Task_LM_setAffinity, (UArg)tsk, (UArg)tsk->fxn,
                       (UArg)coreId, (UArg)tsk->affinity, (UArg)newAffinity);

    /*
     * bail out really early if no change
     */

    if (newAffinity == tsk->affinity) {
        return ((tsk->affinity == Core_numCores) ? Task_AFFINITY_NONE : tsk->affinity);
    }

    tskKey = Task_disable();

    coreId = Core_getId();

    hwiKey = Core_hwiDisable();

    oldAffinity = tsk->affinity;

    /*
     * Can leave early if task is already RUNNING on the newAffinity core
     * or if the task is RUNNING and the new Affinity is "Don't Care"
     */

    if (tsk->mode == Task_Mode_RUNNING) {
        /* case 4a and 4b covered in this check */
        if ((newAffinity == tsk->curCoreId) || (newAffinity == Core_numCores)) {
            tsk->affinity = newAffinity;

            tsk->readyQ =
                (Queue_Handle)((UInt8 *)(Task_module->smpReadyQ[tsk->affinity]) +
                 (UInt)(tsk->priority * (2*sizeof(Ptr))));

            Core_hwiRestore(hwiKey);
            Task_restore(tskKey);
            return ((oldAffinity == Core_numCores) ? Task_AFFINITY_NONE : oldAffinity);
        }
    }

    oldQ = tsk->readyQ;

    if (tsk->priority == -1) {
        newQ = oldQ;
    }
    else {
        newQ = (Queue_Handle)((UInt8 *)(Task_module->smpReadyQ[newAffinity]) +
                (UInt)(tsk->priority * (2*sizeof(Ptr))));
    }

    tskMask = tsk->mask;

    switch (Task_getMode(tsk)) {
        /* case 1: task is inactive (priority == -1) */
        case Task_Mode_INACTIVE:
            tsk->affinity = newAffinity;
            tsk->readyQ = newQ;
            break;

        /* case 2: task is ready (not running) */
        case Task_Mode_READY:
            Queue_remove((Queue_Elem *)tsk);
            if (Queue_empty(oldQ)) {
                Task_module->smpCurSet[oldAffinity] &= ~tskMask;
            }

            /* place task at end of its readyQ */
            Queue_enqueue(newQ, (Queue_Elem *)tsk);
            Task_module->smpCurSet[newAffinity] |= tskMask;
            tsk->affinity = newAffinity;
            tsk->readyQ = newQ;

            /*
             * Need to run the scheduler to check if ready task
             * is eligible to start running.
             */
            if (newAffinity == Core_numCores) {
                Task_module->workFlag |= (1 << coreId);
            }
            else if (tsk->mask > Task_module->smpCurMask[newAffinity]) {
                Task_module->workFlag |= (1 << newAffinity);

                if (newAffinity != coreId) {
                    Core_interruptCore(newAffinity);
                }
            }
            break;

        /* case 3: task is blocked */
        case Task_Mode_BLOCKED:
            tsk->affinity = newAffinity;
            tsk->readyQ = newQ;
            break;

        /* case 4c, 5 */
        case Task_Mode_RUNNING:
            /* case 4: task is running on current core */
            if (tsk->curCoreId == coreId) {
                /* case 4c: we're moving to the newAffinity core */
                tsk->affinity = newAffinity;
                tsk->readyQ = newQ;

                /* force a Task switch */
                Task_module->smpCurMask[coreId] = 0;
                SORT_RUNQ(-1, coreId);

                if (tsk->mask > Task_module->smpCurMask[newAffinity]) {
                    Task_module->workFlag |=
                        ((1 << newAffinity) | (1 << coreId));
                    Core_interruptCore(newAffinity);
                }
                else {
                    Task_module->workFlag |= (1 << coreId);
                }
            }
            /* case 5: task is running on other core */
            else {
                /*
                 * Block tsk and run Task scheduler on the other core
                 * Task_blockI() will set the work flag and interrupt the
                 * other core as necessary.
                 */
                Task_blockI(tsk);

                otherCoreMask = (1 << tsk->curCoreId);

                /* Wait for scheduler to run */
                while ((Task_module->workFlag & otherCoreMask) != 0) {
                    Core_hwiRestore(hwiKey);
                    Task_enable();
                    /* Add a one loop delay here ? */
                    Task_disable();
                    hwiKey = Core_hwiDisable();
                }

                /* Set tsk's affinity to new value and change its readyQ */
                tsk->affinity = newAffinity;
                tsk->readyQ = newQ;

                /*
                 * Make tsk READY. Task_unblockI() will interrrupt the
                 * other core as necessary.
                 */
                Task_unblockI(tsk, hwiKey);
            }
            break;

        /* shouldn't get here ... */
        default:
            break;
    }

    Core_hwiRestore(hwiKey);

    Task_restore(tskKey);

    return ((oldAffinity == Core_numCores) ? Task_AFFINITY_NONE : oldAffinity);
}

/*
 *  ======== Task_blockI ========
 *  Block a task.
 *
 *  Must be called within Task_disable/Task_restore block
 *  and with interrupts disabled
 *
 *  Assumes task IS NOT on any queue.
 */
Void Task_blockI(Task_Object *tsk)
{
    UInt curCoreId;
    UInt32 *checkValue;

    if (Task_objectCheckFlag) {
        checkValue = Task_SupportProxy_getCheckValueAddr(tsk);
        if (Task_objectCheckFxn(tsk, *checkValue) != 0) {
            Error_raise(NULL, Task_E_objectCheckFailed, tsk, 0);
        }
    }

    Log_write2(Task_LD_block, (UArg)tsk, (UArg)tsk->fxn);

    if (tsk->mode == Task_Mode_RUNNING) {
        curCoreId = tsk->curCoreId;

        /* force a task switch */
        Task_module->smpCurMask[curCoreId] = 0;
        SORT_RUNQ(-1, curCoreId);
        Task_module->workFlag |= (1 << curCoreId);

        if (curCoreId != Core_getId()) {
            Core_interruptCore(curCoreId);
        }
    }

    tsk->mode = Task_Mode_BLOCKED;
}

/*
 *  ======== Task_unblockI ========
 *  Unblock a task.
 *
 *  Place task in its ready list.
 *  Must be called within Task_disable/Task_restore block
 *  and with interrupts disabled
 */
Void Task_unblockI(Task_Object *tsk, UInt hwiKey)
{
#ifdef ti_sysbios_knl_Task_ENABLE_READY_HOOKS
    Int i;
#endif
    UInt coreId;
    UInt tskAffinity = tsk->affinity;
    volatile UInt *cursetp = &Task_module->smpCurSet[tskAffinity];
    UInt mask = tsk->mask;
    UInt32 *checkValue;

    if (Task_objectCheckFlag) {
        checkValue = Task_SupportProxy_getCheckValueAddr(tsk);
        if (Task_objectCheckFxn(tsk, *checkValue) != 0) {
            Error_raise(NULL, Task_E_objectCheckFailed, tsk, 0);
        }
    }

    Queue_enqueue(tsk->readyQ, (Queue_Elem *)tsk);

    *cursetp |= mask;
    tsk->mode = Task_Mode_READY;
    coreId = Core_getId();

    if (tskAffinity == Core_numCores) {
        Task_module->workFlag |= (1 << coreId);
    }
    else if (mask > Task_module->smpCurMask[tskAffinity]) {
        Task_module->workFlag |= (1 << tskAffinity);

        /*
         * Do not interrupt other core if creating/constructing a task in
         * main()
         */
        if ((BIOS_getThreadType() != BIOS_ThreadType_Main) &&
            (tskAffinity != coreId)) {
            Core_interruptCore(tskAffinity);
        }
    }

    /* It's safe to enable intrs here */
    Hwi_restore(hwiKey);

#ifdef ti_sysbios_knl_Task_ENABLE_READY_HOOKS
    for (i = 0; i < Task_hooks.length; i++) {
        if (Task_hooks.elem[i].readyFxn != NULL) {
            Task_hooks.elem[i].readyFxn(tsk);
        }
    }
#endif

    Log_write3(Task_LD_ready, (UArg)tsk, (UArg)tsk->fxn, (UArg)tsk->priority);

    /* Hard-disable intrs - this fxn is called with them disabled */
    Hwi_disable();
}

/*
 *  ======== Task_yield ========
 */
Void Task_yield()
{
    UInt tskKey, hwiKey;
    Task_Object *curTask;
    UInt coreId;

    tskKey = Task_disable();

    /* Read coreId after disabling tasking */
    coreId = Core_getId();

    hwiKey = Hwi_disable();

    curTask = Task_module->smpCurTask[coreId];

    /*
     * Only disturb the currently running thread
     * if it is not in a transient state (ie in Task_exit())
     */
    if ((Task_module->smpCurMask[coreId]) &&
        (curTask->mode == Task_Mode_RUNNING)) {
        /* Change from RUNNING to READY */
        curTask->mode = Task_Mode_READY;
        /* And place it at the end of its readyQ */
        Queue_enqueue(curTask->readyQ, (Queue_Elem *)curTask);
        Task_module->smpCurSet[curTask->affinity] |= curTask->mask;
    }

    /* force a task switch */
    Task_module->smpCurMask[coreId] = 0;
    SORT_RUNQ(-1, coreId);
    Task_module->workFlag |= (1 << coreId);

    Hwi_restore(hwiKey);

    Log_write3(Task_LM_yield, (UArg)Task_module->smpCurTask[coreId], (UArg)(Task_module->smpCurTask[coreId]->fxn), (UArg)(BIOS_getThreadType()));

    Task_restore(tskKey);
}

/*
 *  ======== Task_enter ========
 *  Here on task's first invocation.
 *
 *  Unlock the Task Scheduler to enter task as though we
 *  returned through Task_restore()
 */
Void Task_enter()
{
    Task_schedule();

    Task_module->locked = FALSE;

    Hwi_enable();
}

/*
 *************************************************************************
 *                      Module wide functions
 *************************************************************************
 */

/*
 *  ======== Task_Module_startup ========
 *  Initialize and start the Task Module.
 *  Called at system init time before main().
 */
Int Task_Module_startup (Int phase)
{
    Queue_Params queueParams;

    /*
     * Need to wait for SupportProxy. Other modules safe.
     */
    if (BIOS_taskEnabled) {  /* minimize code foot print if Task is disabled */
        Int i;
        UInt j;

        /* The TaskSupport delegate cannnot depend on Task during Startup */
        /*
         * Startup_rtsDone() is called because we do Error_check() in
         * instanceStartup
         */
        if (Task_SupportProxy_Module_startupDone() ) {

#ifndef ti_sysbios_knl_Task_DISABLE_ALL_HOOKS
            for (i = 0; i < Task_hooks.length; i++) {
                if (Task_hooks.elem[i].registerFxn != NULL) {
                    Task_hooks.elem[i].registerFxn(i);
                }
            }
#endif

            if (Task_moduleStateCheckFlag) {
                Task_moduleStateCheckValue =
                    Task_moduleStateCheckValueFxn(Task_module);
            }

            /* Initialize Task_Module_StateSmp */

            /* Initialize sortedRunQ object */
            Queue_Params_init(&queueParams);
            Queue_construct((Queue_Struct *)(Task_moduleSmp->sortedRunQ),
                &queueParams);

            /*
             * init module smpRunQ elems.
             */
            for (i = 0; i < Core_numCores; i++) {
                Queue_insert((Queue_Elem *)(Task_moduleSmp->sortedRunQ),
                    (Queue_Elem *)&(Task_moduleSmp->smpRunQ[i]));
            }

            /*
             * init module readyQ pointers.
             * One for each core plus the don't care readyQs.
             */
            for (i = 0; i <= Core_numCores; i++) {
                Task_module->smpReadyQ[i] =
                        Queue_Object_get(Task_module->readyQ,
                                        i*Task_numPriorities);
            }

            /* do post init on all statically Created tasks */
            for (i = 0; i < Task_Object_count(); i++) {
                Task_postInit(Task_Object_get(NULL, i), NULL);
            }

            /* do post init on all statically Constructed tasks */
            for (j = 0; j < Task_numConstructedTasks; j++) {
                Task_postInit(Task_module->constructedTasks[j], NULL);
            }

            return (Startup_DONE);
        }
        else {
            return (Startup_NOTDONE);
        }
    }
    else {
        return (Startup_DONE);
    }
}

/*
 *  ======== Task_startup ========
 */
Void Task_startup()
{
    Task_startCore(0);
}

/*
 *  ======== Task_startCore ========
 */
Void Task_startCore(UInt coreId)
{
    Queue_Handle maxQ;
    Task_Object *prevTask;
    Task_Object *curTask;
    Task_Struct dummyTask;
    UInt curSetPriLocal, curSetPriX, curPriLocal;
    Int newPri;
#ifdef ti_sysbios_knl_Task_ENABLE_SWITCH_HOOKS
    Int i;
#endif

    Hwi_disable();      /* re-enabled in Task_enter of first task */

    /* Use dummyTask as initial task to swap from */
    prevTask = Task_handle(&dummyTask);

    /* stall until a task is ready */
    while (Task_module->smpCurSet[coreId] == 0) {
        Task_allBlockedFunction();
    }

    curSetPriLocal = Task_module->smpCurSet[coreId]        ? Intrinsics_maxbit(Task_module->smpCurSet[coreId]) + 1 : 0;
    curSetPriX     = Task_module->smpCurSet[Core_numCores] ? Intrinsics_maxbit(Task_module->smpCurSet[Core_numCores]) + 1 : 0;
    curPriLocal    = Task_module->smpCurMask[coreId]       ? Intrinsics_maxbit(Task_module->smpCurMask[coreId]) + 1 : 0;

    if (curSetPriLocal >= curSetPriX) {
        newPri = curSetPriLocal - 1;
        maxQ = (Queue_Handle)((UInt8 *)(Task_module->smpReadyQ[coreId]) +
        (UInt)(newPri*(2*sizeof(Ptr))));
    }
    else if (curSetPriX > curPriLocal) {
        newPri = curSetPriX - 1;
        maxQ = (Queue_Handle)((UInt8 *)(Task_module->smpReadyQ[Core_numCores]) +
        (UInt)(newPri*(2*sizeof(Ptr))));
    }

    /* remove task from its readyQ */
    curTask = Queue_get(maxQ);

    /* adjust its readyQ curset accordingly */
    if (Queue_empty(maxQ)) {
        Task_module->smpCurSet[curTask->affinity] &= ~(curTask->mask);
    }

    curTask->curCoreId = coreId;
    curTask->mode = Task_Mode_RUNNING;

    Task_module->smpCurTask[coreId] = curTask;
    Task_module->smpCurMask[coreId] = curTask->mask;

    /* we've done the core's scheduler's work */
    Task_module->workFlag &= ~(1 << coreId);

    /* Signal that we are entering task thread mode */
    BIOS_setThreadType(BIOS_ThreadType_Task);

    /* Update the Sorted RunQ */
    SORT_RUNQ(newPri, coreId);

    if (Task_checkStackFlag) {
        Task_checkStacks(NULL, curTask);
    }

    if (coreId == 0) {
        /* should be safe to enable intrs here */
        Hwi_enable();
    }

#ifdef ti_sysbios_knl_Task_ENABLE_SWITCH_HOOKS
    /* Run switch hooks for first real Task */
    for (i = 0; i < Task_hooks.length; i++) {
        if (Task_hooks.elem[i].switchFxn != NULL) {
            Task_hooks.elem[i].switchFxn(NULL, Task_module->smpCurTask[coreId]);
        }
    }
#endif

    Log_write4(Task_LM_switch, (UArg)0, (UArg)0,
               (UArg)Task_module->smpCurTask[coreId],
               (UArg)Task_module->smpCurTask[coreId]->fxn);

    /* must leave this function with ints disabled */
    Hwi_disable();

    if (coreId == 0) {
        /* inform dispatcher that we're running on task stack */
        Hwi_switchFromBootStack();

        if (Task_startupHookFunc != NULL) {
            Task_startupHookFunc();
        }
    }

    /* start first task by way of enter() */
    Task_SupportProxy_swap((Ptr)&prevTask->context,
                (Ptr)&Task_module->smpCurTask[coreId]->context);
}

/*
 *  ======== Task_unlockSched ========
 */
Void Task_unlockSched()
{
    Task_module->locked = FALSE;
}

/*
 *  ======== Task_enabled ========
 */
Bool Task_enabled()
{
    if (BIOS_swiEnabled == FALSE) {
        return (Task_module->locked == FALSE);
    }
    else {
        return ((Task_module->locked == FALSE) && Swi_enabled());
    }
}

/*
 *  ======== Task_disable ========
 */
UInt Task_disable()
{
    UInt key, hwiKey;

    hwiKey = Hwi_disable();

    key = Task_module->locked;

    Task_module->locked = TRUE;

    Core_hwiRestore(hwiKey);

    if (Task_moduleStateCheckFlag) {
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
Void Task_enable()
{
    Task_restore(0);
}

/*
 *  ======== Task_enableOtherCores ========
 *  Do NOT enable interrupts locally
 */
Void Task_enableOtherCores()
{
    Task_module->locked = FALSE; /* release the scheduler lock */
    Core_unlock();               /* release the inter-core lock */
}

/*
 *  ======== Task_restore ========
 */
Void Task_restore(UInt tskKey)
{
    if (tskKey == FALSE) {
        Core_hwiDisable();
        if (Task_module->workFlag & (1 << Core_getId())
            && (!BIOS_swiEnabled || (BIOS_swiEnabled && Swi_enabled()))) {
            Task_schedule();
        }
        Task_module->locked = FALSE;

        Hwi_enable();
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
Void Task_restoreHwi(UInt tskKey)
{
    if (tskKey == FALSE) {
        if (Task_module->workFlag & (1 << Core_getId())
            && (!BIOS_swiEnabled || (BIOS_swiEnabled && Swi_enabled()))) {
            Task_schedule();
        }
        Task_module->locked = FALSE;
    }
}

/*
 *  ======== Task_self ========
 */
Task_Handle Task_self()
{
    UInt hwiKey;
    volatile Task_Handle curTask;

    hwiKey = Core_hwiDisable();
    curTask = Task_module->smpCurTask[Core_getId()];
    Core_hwiRestore(hwiKey);

    return curTask;
}

/*
 *  ======== Task_checkStacks ========
 */
Void Task_checkStacks(Task_Handle oldTask, Task_Handle newTask)
{
    UInt32 *checkValue;
    UInt oldTaskStack; /* used to obtain current (oldTask) stack address */

    if (Task_objectCheckFlag) {
        if (oldTask != NULL) {
            checkValue = Task_SupportProxy_getCheckValueAddr(oldTask);
            if (Task_objectCheckFxn(oldTask, *checkValue) != 0) {
                Error_raise(NULL, Task_E_objectCheckFailed, oldTask, 0);
            }
        }
        checkValue = Task_SupportProxy_getCheckValueAddr(newTask);
        if (Task_objectCheckFxn(newTask, *checkValue) != 0) {
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
    if (!Task_SupportProxy_checkStack(oldTask->stack, oldTask->stackSize)) {
        Error_raise(NULL, Task_E_stackOverflow, oldTask, 0);
    }

    if (!Task_SupportProxy_checkStack(newTask->stack, newTask->stackSize)) {
        Error_raise(NULL, Task_E_stackOverflow, newTask, 0);
    }

    /* check sp's for being in bounds */
    if (((UArg)&oldTaskStack < (UArg)oldTask->stack) ||
        ((UArg)&oldTaskStack > (UArg)(oldTask->stack+oldTask->stackSize))) {
        Error_raise(NULL, Task_E_spOutOfBounds, oldTask, oldTask->context);
    }

    if ((newTask->context < (Ptr)newTask->stack) ||
        (newTask->context > (Ptr)(newTask->stack+newTask->stackSize))) {
        Error_raise(NULL, Task_E_spOutOfBounds, newTask, newTask->context);
    }
}

/*
 *  ======== Task_processVitalTasks ========
 *  Call BIOS_exit() when last vitalTask exits or is
 *  deleted.
 */
Void Task_processVitalTaskFlag(Task_Object *tsk)
{
    UInt hwiKey;
    if (tsk->vitalTaskFlag == TRUE) {
        hwiKey = Hwi_disable();
        if (--Task_module->vitalTasks == 0) {
            Hwi_restore(hwiKey);
            BIOS_exit(0);
        }
        Hwi_restore(hwiKey);
    }
}

/*
 *  ======== Task_exit ========
 */
Void Task_exit()
{
    UInt tskKey, hwiKey;
    Task_Object *tsk;
#ifndef ti_sysbios_knl_Task_DISABLE_ALL_HOOKS
    Int i;
#endif

    tsk = Task_self();

#ifndef ti_sysbios_knl_Task_DISABLE_ALL_HOOKS
    /*
     * Process Task_exit hooks.  Should be called outside the Task kernel.
     */
    for (i = 0; i < Task_hooks.length; i++) {
        if (Task_hooks.elem[i].exitFxn != NULL) {
            Task_hooks.elem[i].exitFxn(tsk);
        }
    }
#endif

    Log_write2(Task_LD_exit, (UArg)tsk, (UArg)tsk->fxn);

    hwiKey = Hwi_disable();

    tskKey = Task_disable();

    Task_blockI(tsk);

    tsk->mode = Task_Mode_TERMINATED;

    Task_processVitalTaskFlag(tsk);

    Hwi_restore(hwiKey);

    Queue_elemClear((Queue_Elem *)tsk);

    /* add to terminated task list if it was dynamically created */
    if (Task_deleteTerminatedTasks == TRUE) {
        Task_Handle dynTask;

        dynTask = Task_Object_first();

        while (dynTask) {
            if (tsk == dynTask) {
                tsk->readyQ = Task_Module_State_terminatedQ();
                Queue_put(tsk->readyQ, (Queue_Elem *)tsk);
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
Void Task_sleepTimeout(UArg arg)
{
    UInt hwiKey;
    Task_PendElem *elem = (Task_PendElem *)arg;

    hwiKey = Hwi_disable();

    /*
     * put tsk back into readyQ
     * No need for Task_disable/restore sandwich since this
     * is called within Swi (or Hwi) thread
     */
    Task_unblockI(elem->task, hwiKey);

    Hwi_restore(hwiKey);
}

/*
 *  ======== Task_sleep ========
 */
Void Task_sleep(UInt32 timeout)
{
    Task_PendElem elem;
    UInt hwiKey, tskKey;
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
    if (BIOS_clockEnabled) {
        /* add Clock event */
        Clock_addI(Clock_handle(&clockStruct), (Clock_FuncPtr)Task_sleepTimeout, timeout, (UArg)&elem);
        elem.clock = Clock_handle(&clockStruct);
    }

    /* MISRA.CAST.FUNC_PTR.2012 MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write3(Task_LM_sleep, (UArg)Task_self(), (UArg)(Task_self()->fxn),
               (UArg)timeout);

    hwiKey = Hwi_disable();

    /*
     * Verify that THIS core hasn't already disabled the scheduler
     * so that the Task_restore() call below will indeed block
     */
    Assert_isTrue((Task_enabled()),
                    Task_A_sleepTaskDisabled);

    /* lock scheduler */
    tskKey = Task_disable();

    /* get task handle and block tsk */
    elem.task = Task_self();

    Task_blockI(elem.task);

    /*
     * BIOS_clockEnabled check is here to eliminate Clock module
     * references in the custom library
     */
    if (BIOS_clockEnabled) {
        Clock_startI(elem.clock);
    }

    /* Only needed for Task_delete() */
    Queue_elemClear(&elem.qElem);

    elem.task->pendElem = (Ptr)(&elem);

    Hwi_restore(hwiKey);

    /* unlock task scheduler and block */
    Task_restore(tskKey);       /* the calling task will block here */

    /*
     * BIOS_clockEnabled check is here to eliminate Clock module
     * references in the custom library
     */
    if (BIOS_clockEnabled) {
        hwiKey = Hwi_disable();
        /* remove Clock object from Clock Q */
        Clock_removeI(elem.clock);
        elem.clock = NULL;
        Hwi_restore(hwiKey);
    }

    elem.task->pendElem = NULL;
}

/*
 *  ======== Task_sleepUntil ========
 */
Void Task_sleepUntil(UInt32 tick)
{
    if (BIOS_clockEnabled) {
        UInt32 diff;
        UInt32 now;

        /* get current Clock tick count */
        now = Clock_getTicks();

        /* determine distance to target tick */
        if (tick < now) {
            diff = 0xFFFFFFFF - now + tick + 1;
        }
        else {
            diff = tick - now;
        }

        /* now sleep for the duration */
        if (diff != 0) {
            Task_sleep(diff);
        }
    }
}

/*
 *  ======== Task_getIdleTask ========
 */
Task_Handle Task_getIdleTask()
{
    return(Task_module->idleTask[0]);
}

/*
 *  ======== Task_getIdleTaskHandle ========
 */
Task_Handle Task_getIdleTaskHandle(UInt coreId)
{
    return(Task_module->idleTask[coreId]);
}

/*
 *************************************************************************
 *                       Instance functions
 *************************************************************************
 */

/*
 *  ======== Task_Instance_init ========
 */
Int Task_Instance_init(Task_Object *tsk, Task_FuncPtr fxn,
                const Task_Params *params, Error_Block *eb)
{
    Int align;
    Int status;
    SizeT stackSize;

    Assert_isTrue((BIOS_taskEnabled == TRUE), Task_A_taskDisabled);

    Assert_isTrue(((BIOS_getThreadType() != BIOS_ThreadType_Hwi) &&
                   (BIOS_getThreadType() != BIOS_ThreadType_Swi)), Task_A_badThreadType);

    Assert_isTrue((((params->priority == -1) || (params->priority > 0)) &&
                   (params->priority < (Int)Task_numPriorities)),
                   Task_A_badPriority);

    tsk->priority = params->priority;

    /* deal with undefined Task_Params defaults */
    if (params->stackHeap == NULL) {
        tsk->stackHeap = Task_defaultStackHeap;
    }
    else {
        tsk->stackHeap = params->stackHeap;
    }

    if (params->stackSize == 0) {
        stackSize = Task_defaultStackSize;
    }
    else {
        stackSize = params->stackSize;
    }

    align = Task_SupportProxy_getStackAlignment();

    if (params->stack != NULL) {
        if (align != 0) {
            UArg stackTemp;
            /* align low address to stackAlignment */
            stackTemp = (UArg)params->stack;
            stackTemp += align - 1;
            stackTemp &= -align;
            tsk->stack = (Ptr)stackTemp;

            /* subtract what we removed from the low address from stackSize */
            tsk->stackSize = stackSize - (stackTemp - (UArg)params->stack);

            /* lower the high address as necessary */
            tsk->stackSize &= -align;
        }
        else {
            tsk->stack = params->stack;
            tsk->stackSize = stackSize;
        }
        /* tell Task_delete that stack was provided */
        tsk->stackHeap = (xdc_runtime_IHeap_Handle)(-1);
    }
    else {
        if (BIOS_runtimeCreatesEnabled) {
            if (align != 0) {
                /*
                 * round stackSize up to the nearest multiple of the alignment.
                 */
                tsk->stackSize = (stackSize + align - 1) & -align;
            }
            else {
                tsk->stackSize = stackSize;
            }

            tsk->stack = Memory_alloc(tsk->stackHeap, tsk->stackSize,
                                      align, eb);

            if (tsk->stack == NULL) {
                return (1);
            }
        }
    }

    tsk->fxn = fxn;
    tsk->arg0 = params->arg0;
    tsk->arg1 = params->arg1;

    tsk->env = params->env;

    if(params->affinity != Task_AFFINITY_NONE) {
        tsk->affinity = params->affinity;
        /* cureCoreId field gets overwriiten when task starts */
        tsk->curCoreId = params->affinity;
    }
    else {
        tsk->affinity = Core_numCores;
        /* cureCoreId field gets overwriiten when task starts */
        tsk->curCoreId = Core_numCores;
    }

    tsk->vitalTaskFlag = params->vitalTaskFlag;
    if (tsk->vitalTaskFlag == TRUE) {
        Task_module->vitalTasks += 1;
    }

#ifndef ti_sysbios_knl_Task_DISABLE_ALL_HOOKS
    if (Task_hooks.length > 0) {
        tsk->hookEnv = Memory_calloc(Task_Object_heap(),
                Task_hooks.length * sizeof (Ptr), 0, eb);

        if (tsk->hookEnv == NULL) {
            return (2);
        }
    }
#endif

    status = Task_postInit(tsk, eb);

    if (status != 0) {
        return (2 + status);
    }

    return (0);   /* no failure states */
}

/*
 *  ======== Task_postInit ========
 *  Function to be called during module startup to complete the
 *  initialization of any statically created or constructed task.
 *  Initialize stack.
 *  Build Initial stack image.
 *  Add task to corresponding ready Queue.
 *
 *  returns (0) and clean 'eb' on success
 *  returns (0) and 'eb' if Task_SupportProxy_start() fails.
 *  returns (n) and 'eb' for number of successful createFxn() calls iff
 *     one of the createFxn() calls fails
 */
Int Task_postInit(Task_Object *tsk, Error_Block *eb)
{
    UInt32 *checkValue;
    UInt tskKey, hwiKey;
#ifndef ti_sysbios_knl_Task_DISABLE_ALL_HOOKS
    Int i;
    Error_Block localEB;
    Error_Block *leb;
#endif

    tsk->context = Task_SupportProxy_start(tsk,
                (Task_SupportProxy_FuncPtr)Task_enter,
                (Task_SupportProxy_FuncPtr)Task_exit,
                eb);

    if (tsk->context == NULL) {
        return (1);
    }

    tsk->mode = Task_Mode_READY;

    tsk->pendElem = NULL;

    if (Task_objectCheckFlag) {
        checkValue = Task_SupportProxy_getCheckValueAddr(tsk);
        *checkValue = Task_objectCheckValueFxn(tsk);
    }

#ifndef ti_sysbios_knl_Task_DISABLE_ALL_HOOKS
    if (eb != Error_IGNORE) {
        leb = eb;
    }
    else {
        Error_init(&localEB);
        leb = &localEB;
    }

    for (i = 0; i < Task_hooks.length; i++) {
        tsk->hookEnv[i] = (Ptr)0;
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
        tsk->readyQ = Task_Module_State_inactiveQ();
        Queue_put(tsk->readyQ, (Queue_Elem *)tsk);
    }
    else {
        tsk->mask = 1 << tsk->priority;
        tsk->readyQ =
                (Queue_Handle)((UInt8 *)(Task_module->smpReadyQ[tsk->affinity]) +
                (UInt)(tsk->priority * (2*sizeof(Ptr))));

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
Void Task_Instance_finalize(Task_Object *tsk, Int status)
{
#ifndef ti_sysbios_knl_Task_DISABLE_ALL_HOOKS
    Int i, cnt;
#endif
    UInt taskKey, hwiKey;

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
        if ((Task_deleteTerminatedTasks == TRUE)
             && (Task_getMode(tsk) == Task_Mode_TERMINATED)
             && (tsk->readyQ == Task_Module_State_terminatedQ())) {
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
            Queue_remove((Queue_Elem *)tsk);
            /* if last task in readyQ, remove corresponding bit in curSet */
            if (Queue_empty(tsk->readyQ)) {
                Task_module->smpCurSet[tsk->affinity] &= ~tsk->mask;
            }

            /*
             * if task was made ready by a pend timeout but hasn't run yet
             * then its clock object is still on the Clock service Q.
             */
            if (tsk->pendElem != NULL) {
                if (BIOS_clockEnabled && tsk->pendElem->clock) {
                    Clock_removeI(tsk->pendElem->clock);
                }
            }
       }

        if (tsk->mode == Task_Mode_BLOCKED) {
            Assert_isTrue(tsk->pendElem != NULL, Task_A_noPendElem);

            /* Seemingly redundant test in case Asserts are disabled */
            if (tsk->pendElem != NULL) {
                Queue_remove(&(tsk->pendElem->qElem));
                if (BIOS_clockEnabled && tsk->pendElem->clock) {
                    Clock_removeI(tsk->pendElem->clock);
                }
            }
        }

        if (tsk->mode == Task_Mode_TERMINATED) {
            /* remove task from terminated task list */
            Queue_remove((Queue_Elem *)tsk);
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

    if (BIOS_runtimeCreatesEnabled) {
        /* free stack if it was allocated dynamically */
        if (tsk->stackHeap != (xdc_runtime_IHeap_Handle)(-1)) {
            Memory_free(tsk->stackHeap, tsk->stack, tsk->stackSize);
        }
    }

    /* return if failed to allocate Hook Env */
    if (status == 2) {
        return;
    }

    /* return if Task_SupportProxy_start() failed to construct a task stack image */
    if (status == 3) {
        return;
    }

    /* status == 0 or status == 4 - in both cases create hook was called */

#ifndef ti_sysbios_knl_Task_DISABLE_ALL_HOOKS
    /* free any allocated Hook Envs */
    if (Task_hooks.length > 0) {
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

        Memory_free(Task_Object_heap(), tsk->hookEnv,
                Task_hooks.length * sizeof (Ptr));
    }
#endif
}

/*
 *  ======== Task_getArg0 ========
 */
UArg Task_getArg0(Task_Object *tsk)
{
    return (tsk->arg0);
}

/*
 *  ======== Task_getArg1 ========
 */
UArg Task_getArg1(Task_Object *tsk)
{
    return (tsk->arg1);
}

/*
 *  ======== Task_getEnv ========
 */
Ptr Task_getEnv(Task_Object *tsk)
{
    return (tsk->env);
}

/*
 *  ======== Task_FuncPtr ========
 */
Task_FuncPtr Task_getFunc(Task_Object *task, UArg *arg0, UArg *arg1)
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
Ptr Task_getHookContext(Task_Object *tsk, Int id)
{
    return tsk->hookEnv[id];
}

/*
 *  ======== Task_setHookContext ========
 */
Void Task_setHookContext(Task_Object *tsk, Int id, Ptr hookContext)
{
    tsk->hookEnv[id] = hookContext;
}

/*
 *  ======== Task_getPri ========
 */
Int Task_getPri(Task_Object *tsk)
{
   return tsk->priority;
}

/*
 *  ======== Task_setArg0 ========
 */
Void Task_setArg0(Task_Object *tsk, UArg arg)
{
    tsk->arg0 = arg;
}

/*
 *  ======== Task_setArg1 ========
 */
Void Task_setArg1(Task_Object *tsk, UArg arg)
{
    tsk->arg1 = arg;
}

/*
 *  ======== Task_setEnv ========
 */
Void Task_setEnv(Task_Object *tsk, Ptr env)
{
    tsk->env = env;
}

/*
 *  ======== Task_getAffinity ========
 */
UInt Task_getAffinity(Task_Object *tsk)
{
    return ((tsk->affinity == Core_numCores) ? Task_AFFINITY_NONE : tsk->affinity);
}

/*
 *  ======== Task_getMode ========
 */
Task_Mode Task_getMode(Task_Object *tsk)
{
    UInt hwiKey;
    Task_Mode mode;
    UInt tskCoreId;

    if (tsk->priority == -1) {
        return (Task_Mode_INACTIVE);
    }
    else {
        hwiKey = Hwi_disable();
        mode = tsk->mode;
        tskCoreId = tsk->curCoreId;
        if (tskCoreId != Core_numCores) {
            /*
             * Under certain transient conditions (ie within Task_exit()),
             * a running task's mode may not be RUNNING.
             * Always return RUNNING if the task is currently
             * running on it's respective core.
             */
            if (Task_module->smpCurTask[tskCoreId] == tsk) {
                mode = Task_Mode_RUNNING;
            }
        }
        Hwi_restore(hwiKey);
        return (mode);
    }
}

/*
 *  ======== Task_stat ========
 */
Void Task_stat(Task_Object *tsk, Task_Stat *statbuf)
{
    UInt hwiKey;

    /* collect a coherent set */    
    hwiKey = Hwi_disable();

    statbuf->priority = tsk->priority;
    statbuf->stack = tsk->stack;
    statbuf->stackSize = tsk->stackSize;
    statbuf->stackHeap = tsk->stackHeap;
    statbuf->env = tsk->env;
    statbuf->mode = Task_getMode(tsk);
    statbuf->sp = tsk->context;

    Hwi_restore(hwiKey);

    /*
     * allow stack used to be non-coherent with other stats
     * to avoid latency hit
     */
    statbuf->used = Task_SupportProxy_stackUsed((Char *)tsk->stack,
        tsk->stackSize);
}

/*
 *  ======== Task_block ========
 *
 *  No check is made to verify that task is NOT already blocked
 */
Void Task_block(Task_Object *tsk)
{
    UInt curset, hwiKey, mask;
    Queue_Object *readyQ;

    hwiKey = Hwi_disable();

    readyQ = tsk->readyQ;
    curset = Task_module->smpCurSet[tsk->affinity];
    mask = tsk->mask;

    /*
     * Can be used by Task_setAffinity() to move a blocked task
     */

    /*
     * if task is RUNNING, then it is NOT already on a queue
     */
    if (tsk->mode != Task_Mode_RUNNING) {
        Queue_remove((Queue_Elem *)tsk);

        /* if last task is in a readyQ, remove corresponding bit in curSet */
        if (Queue_empty(readyQ)) {
            Task_module->smpCurSet[tsk->affinity] = curset & ~mask;
        }
    }

    Task_blockI(tsk);

    Hwi_restore(hwiKey);

}

/*
 *  ======== Task_unblock ========
 */
Void Task_unblock(Task_Object *tsk)
{
    UInt hwiKey;

    hwiKey = Hwi_disable();

    Task_unblockI(tsk, hwiKey);

    Hwi_restore(hwiKey);
}

/*
 *  ======== Task_allBlockedFunction ========
 */
Void Task_allBlockedFunction()
{
    volatile UInt delay;

    if (Task_allBlockedFunc == Idle_run) {
        Hwi_enable();
        Idle_run();
        Hwi_disable();
    }
    else if (Task_allBlockedFunc == NULL) {
        Hwi_enable();
        /* Guarantee that interrupts are enabled briefly */
        for (delay = 0; delay < 1; delay++) {
           ;
        }
        Hwi_disable();
    }
    else {
        Task_allBlockedFunc();
        /*
         * disable ints just in case the
         * allBlockedFunc left them enabled
         */
        Hwi_disable();
    }
}

/*
 *  ======== Task_deleteTerminatedTasksFunc ========
 */
Void Task_deleteTerminatedTasksFunc()
{
    UInt hwiKey, taskKey;
    Task_Handle tsk;

    taskKey = Task_disable();

    hwiKey = Hwi_disable();

    if (!Queue_empty(Task_Module_State_terminatedQ())) {
        tsk = Queue_head(Task_Module_State_terminatedQ());
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
Int Task_moduleStateCheck(Task_Module_State *moduleState, UInt32 checkValue)
{
    UInt32 newCheckValue;

    newCheckValue = Task_moduleStateCheckValueFxn(moduleState);
    if (newCheckValue != checkValue) {
        return (-1);
    }

    return (0);
}

/*
 *  ======== Task_getModuleStateCheckValue ========
 */
UInt32 Task_getModuleStateCheckValue(Task_Module_State *moduleState)
{
    UInt64 checksum;

    checksum = (uintptr_t)moduleState->readyQ +
               (uintptr_t)moduleState->smpCurSet +
               (uintptr_t)moduleState->smpCurMask +
               (uintptr_t)moduleState->smpCurTask +
               (uintptr_t)moduleState->smpReadyQ +
               (uintptr_t)moduleState->idleTask +
               (uintptr_t)moduleState->constructedTasks +
               (uintptr_t)Task_moduleSmp->sortedRunQ +
               (uintptr_t)Task_moduleSmp->smpRunQ;
    checksum = (checksum >> 32) + (checksum & 0xFFFFFFFF);
    checksum = checksum + (checksum >> 32);

    return ((UInt32)(~checksum));
}

/*
 *  ======== Task_objectCheck ========
 */
Int Task_objectCheck(Task_Handle handle, UInt32 checkValue)
{
    UInt32 newCheckValue;

    newCheckValue = Task_objectCheckValueFxn(handle);
    if (newCheckValue != checkValue) {
        return (-1);
    }

    return (0);
}

/*
 *  ======== Task_getObjectCheckValue ========
 */
UInt32 Task_getObjectCheckValue(Task_Handle taskHandle)
{
    UInt64 checksum;

    checksum = taskHandle->stackSize +
               (uintptr_t)taskHandle->stack +
               (uintptr_t)taskHandle->stackHeap +
#if defined(__IAR_SYSTEMS_ICC__)
               (UInt64)taskHandle->fxn +
#else
               (uintptr_t)taskHandle->fxn +
#endif
               taskHandle->arg0 +
               taskHandle->arg1 +
               (uintptr_t)taskHandle->hookEnv +
               taskHandle->vitalTaskFlag;
    checksum = (checksum >> 32) + (checksum & 0xFFFFFFFF);
    checksum = checksum + (checksum >> 32);

    return ((UInt32)(~checksum));
}
