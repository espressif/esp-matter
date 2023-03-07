/*
 * Copyright (c) 2012-2017, Texas Instruments Incorporated
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
 *  ======== ThreadSupport.c ========
 */

#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Memory.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

#include <xdc/runtime/knl/IThreadSupport.h>

#include "package/internal/ThreadSupport.xdc.h"

/*
 *  ======== ThreadSupport_Instance_init ========
 */
Int ThreadSupport_Instance_init(ThreadSupport_Handle obj, 
    ThreadSupport_RunFxn fxn, const ThreadSupport_Params* params, 
    Error_Block* eb)
{
    Task_Params tpars;
    Semaphore_Handle bios6sem;
 
    bios6sem = ThreadSupport_Instance_State_join_sem(obj);

    Task_Params_init(&tpars);
    tpars.arg0 = (UArg)obj;
    if (params->stackSize != 0) {
        tpars.stackSize = params->stackSize;
    }
    tpars.env = obj;

    tpars.instance->name = params->instance->name;

    if (params->osPriority != ThreadSupport_INVALID_OS_PRIORITY) {
        tpars.priority = params->osPriority;
    }
    else {
        if (params->priority == ThreadSupport_Priority_LOWEST) {
            tpars.priority = ThreadSupport_lowestPriority;
        }
        else if (params->priority == ThreadSupport_Priority_BELOW_NORMAL) {
            tpars.priority = ThreadSupport_belowNormalPriority;
        }
        else if (params->priority == ThreadSupport_Priority_NORMAL) {
            tpars.priority = ThreadSupport_normalPriority;
        }
        else if (params->priority == ThreadSupport_Priority_ABOVE_NORMAL) {
            tpars.priority = ThreadSupport_aboveNormalPriority;
        }
        else if (params->priority == ThreadSupport_Priority_HIGHEST) {
            tpars.priority = ThreadSupport_highestPriority;
        }
        else {
            Error_raise(eb, ThreadSupport_E_priority, params->priority, 0);
            return (ThreadSupport_PRI_FAILURE);
        }
    }

    obj->tls = params->tls;
    obj->startFxn = fxn;
    obj->startFxnArg = params->arg;

    Semaphore_construct(Semaphore_struct(bios6sem), 0, NULL);

    obj->task = Task_create(&ThreadSupport_runStub, &tpars, eb);
    if (obj->task == NULL) {
        return (ThreadSupport_TASK_FAILURE);
    }

    return (0);
}

/*
 *  ======== ThreadSupport_runStub ========
 */
Void ThreadSupport_runStub(UArg input, UArg discard)
{
    ThreadSupport_Handle obj = (ThreadSupport_Handle)xdc_uargToPtr(input);
    ti_sysbios_knl_Semaphore_Handle bios6sem;

    bios6sem = ThreadSupport_Instance_State_join_sem(obj);

    Log_write1(ThreadSupport_L_start, (IArg)obj);
    obj->startFxn(obj->startFxnArg);
    Log_write1(ThreadSupport_L_finish, (IArg)obj);

    Semaphore_post(bios6sem);
}

/*
 *  ======== ThreadSupport_Instance_finalize ========
 */
Void ThreadSupport_Instance_finalize(ThreadSupport_Handle obj, Int status)
{
    ti_sysbios_knl_Semaphore_Handle bios6sem;
 
    bios6sem = ThreadSupport_Instance_State_join_sem(obj);

    /* status is equal to the return code from Instance_init */
    switch (status) {
        case 0:
            Task_delete(&(obj->task));

            /* OK to fall through */

        case ThreadSupport_TASK_FAILURE:
            Semaphore_destruct(Semaphore_struct(bios6sem));

            /* OK to fall through */

        case ThreadSupport_PRI_FAILURE:
        default:
            break;
    }
}

/*
 *  ======== ThreadSupport_self ========
 */
IThreadSupport_Handle ThreadSupport_self(Error_Block* eb)
{
    /* thread handls is stored in Task's env during create */
    return (IThreadSupport_Handle)Task_getEnv(Task_self());
}

/*
 *  ======== ThreadSupport_start ========
 */
Bool ThreadSupport_start(Error_Block* eb)
{
    /*
     *  Nothing to do here. The user must call BIOS_start(), which will
     *  cause statically created threads to run.
     */

    return (TRUE);
}

/*
 *  ======== ThreadSupport_yield ========
 */
Bool ThreadSupport_yield(Error_Block* eb)
{
    Task_yield();
    return (TRUE);
}

/*
 *  ======== ThreadSupport_compareOsPriorities ========
 */
Int ThreadSupport_compareOsPriorities(Int p1, Int p2, Error_Block* eb)
{
    if (p1 < p2) {
        return (IThreadSupport_CompStatus_LOWER);
    }
    else if (p1 > p2) {
        return (IThreadSupport_CompStatus_HIGHER);
    }
    else {
        return (IThreadSupport_CompStatus_EQUAL);
    }
}

/*
 *  ======== ThreadSupport_sleep ========
 */
 Bool ThreadSupport_sleep(UInt timeout, Error_Block *eb)
{
    UInt bios6_timeout;

    bios6_timeout = timeout / Clock_tickPeriod;
    /*
     *  Don't let nonzero timeout round to 0 - semantically very
     *  different
     */
    if (timeout && (!bios6_timeout)) {
        bios6_timeout = 1;
    }

    Task_sleep(bios6_timeout);
    
    return (TRUE);   
}

/*
 *  ======== ThreadSupport_join ========
 */
Bool ThreadSupport_join(ThreadSupport_Handle obj, Error_Block* eb)
{
    ti_sysbios_knl_Semaphore_Handle bios6sem;
 
    bios6sem = ThreadSupport_Instance_State_join_sem(obj);
    Semaphore_pend(bios6sem, BIOS_WAIT_FOREVER);
    Log_write1(ThreadSupport_L_join, (IArg)obj);

    return (TRUE);
}

/*
 *  ======== ThreadSupport_getPriority ========
 */
ThreadSupport_Priority ThreadSupport_getPriority(ThreadSupport_Handle obj, Error_Block* eb)
{
    Int bios6Pri;
    ThreadSupport_Priority threadPri = ThreadSupport_Priority_INVALID;
 
    bios6Pri = Task_getPri(obj->task);
    
    if (bios6Pri == ThreadSupport_lowestPriority) {
        threadPri = ThreadSupport_Priority_LOWEST;
    }
    else if (bios6Pri == ThreadSupport_belowNormalPriority) {
        threadPri = ThreadSupport_Priority_BELOW_NORMAL;
    }
    else if (bios6Pri == ThreadSupport_normalPriority) {
        threadPri = ThreadSupport_Priority_NORMAL;
    }
    else if (bios6Pri == ThreadSupport_aboveNormalPriority) {
        threadPri = ThreadSupport_Priority_ABOVE_NORMAL;
    }
    else if (bios6Pri == ThreadSupport_highestPriority) {
        threadPri = ThreadSupport_Priority_HIGHEST;
    }

    return (threadPri);
}

/*
 *  ======== ThreadSupport_setPriority ========
 */
Bool ThreadSupport_setPriority(ThreadSupport_Handle obj, 
    ThreadSupport_Priority newPri, Error_Block* eb)
{
    Int bios6Pri;
 
    if (newPri == ThreadSupport_Priority_LOWEST) {
        bios6Pri = ThreadSupport_lowestPriority;
    }
    else if (newPri == ThreadSupport_Priority_BELOW_NORMAL) {
        bios6Pri = ThreadSupport_belowNormalPriority;
    }
    else if (newPri == ThreadSupport_Priority_NORMAL) {
        bios6Pri = ThreadSupport_normalPriority;
    }
    else if (newPri == ThreadSupport_Priority_ABOVE_NORMAL) {
        bios6Pri = ThreadSupport_aboveNormalPriority;
    }
    else if (newPri == ThreadSupport_Priority_HIGHEST) {
        bios6Pri = ThreadSupport_highestPriority;
    }
    else {
        Error_raise(NULL, ThreadSupport_E_priority, newPri, 0);
        return (FALSE);
    }

    Task_setPri(obj->task, bios6Pri);

    return (TRUE);
}

/*
 *  ======== ThreadSupport_getOsPriority ========
 */
Int ThreadSupport_getOsPriority(ThreadSupport_Handle obj, Error_Block* eb)
{
 
    return (Task_getPri(obj->task));
}

/*
 *  ======== ThreadSupport_setOsPriority ========
 */
Bool ThreadSupport_setOsPriority(ThreadSupport_Handle obj, Int newPri, 
    Error_Block* eb)
{
    Task_setPri(obj->task, newPri);

    return (TRUE);
}

/*
 *  ======== ThreadSupport_getOsHandle ========
 */
Ptr ThreadSupport_getOsHandle(ThreadSupport_Handle obj)
{
    return (obj->task);
}

/*
 *  ======== ThreadSupport_getTls ========
 */
Ptr ThreadSupport_getTls(ThreadSupport_Handle obj)
{
    return (obj->tls);
}

/*
 *  ======== ThreadSupport_setTls ========
 */
Void ThreadSupport_setTls(ThreadSupport_Handle obj, Ptr tls)
{
    obj->tls = tls;
}

/*
 *  ======== ThreadSupport_stat ========
 */
Bool ThreadSupport_stat(ThreadSupport_Handle obj, \
    ThreadSupport_Stat* buf, Error_Block* eb)
{
    Task_Stat statbuf;

    Task_stat(Task_self(), &statbuf);
    buf->stackSize = statbuf.stackSize;
    buf->stackUsed = statbuf.used;

    return (TRUE);
}
