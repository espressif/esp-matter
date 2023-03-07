/*
 * Copyright (c) 2013-2018, Texas Instruments Incorporated
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
 *  ======== MultithreadSupport.c ========
 */
#include <xdc/std.h>

#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Types.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include "package/internal/MultithreadSupport.xdc.h"

#include <yvals.h>

#include <DLib_Threads.h>

#define MultithreadSupport_Lock ti_sysbios_rts_iar_MultithreadSupport_Lock

/* Lock struct definition */
typedef struct {
    Ptr owner;
    Int count;
    Semaphore_Struct sem;
} MultithreadSupport_Lock;

/*
 *  ======== MultithreadSupport_taskRegHook ========
 *  The Task register hook is called once per hookset, before main and before
 *  any Task initialization has been done.
 *
 *  This function allows the MultithreadSupport module to store its hookset id,
 *  which is passed to Task_get/setHookContext. The HookContext can be an
 *  arbitrary structure. The MultithreadSupport module has defined a HookContext
 *  to be of type void _DLIB_TLS_MEMORY *.
 */
Void MultithreadSupport_taskRegHook(Int id)
{
    MultithreadSupport_module->taskHId = id;
}

/*
 *  ======== MultithreadSupport_taskDeleteHook ========
 */
Void MultithreadSupport_taskDeleteHook(Task_Handle task)
{
    UInt key;
    void *pStoredContext;

    pStoredContext = (void *)Task_getHookContext(task,
                                        MultithreadSupport_module->taskHId);

    if (pStoredContext != NULL) {
        Semaphore_pend(MultithreadSupport_module->lock, BIOS_WAIT_FOREVER);
        key = Hwi_disable();
        /*
         * __call_thread_dtors() will internally call getTLSPtr() which
         * will return the re-entrancy structure for the currect task.
         * This is a problem because dtors need to be invoked on the deleted
         * Task and not the current Task.
         *
         * To workaround this issue, getTLSPtr() is modified such that
         * it checks if a Task delete is in progress and TLS is being requested
         * by the __call_thread_dtors() invoked by the Task delete hook. If
         * yes, then it returns the deleted Task's TLS pointer. deletedTaskTLSPtr
         * and curTaskHandle fields are set by this delete hook to indicate
         * to getTLSPtr() that a delete operation is in progress.
         */
        MultithreadSupport_module->deletedTaskTLSPtr = (Ptr)pStoredContext;
        MultithreadSupport_module->curTaskHandle = Task_self();
        Hwi_restore(key);

        __call_thread_dtors();
        Memory_free(Task_Object_heap(), pStoredContext, __iar_tls_size());

        key = Hwi_disable();
        MultithreadSupport_module->deletedTaskTLSPtr = NULL;
        MultithreadSupport_module->curTaskHandle = NULL;
        Hwi_restore(key);
        Semaphore_post(MultithreadSupport_module->lock);
    }
}

/*
 *  ======== MultithreadSupport_perThreadAccess ========
 */
Void *MultithreadSupport_perThreadAccess(Void *symbp)
{
    return (NULL);
}

/*
 *  ======== MultithreadSupport_getTlsPtr ========
 */
#pragma section="__iar_tls$$DATA"
void *MultithreadSupport_getTlsPtr()
{
    UInt key;
    void *pCurTaskEnv;
    BIOS_ThreadType type;

    /* get the thread type */
    type = BIOS_getThreadType();
    Assert_isTrue(((type != BIOS_ThreadType_Hwi) &&
            (type != BIOS_ThreadType_Swi)), MultithreadSupport_A_badThreadType);

    /* if still in the main thread, use TLS block allocated by linker */
    if (type == BIOS_ThreadType_Main) {
        pCurTaskEnv = (void *)__section_begin("__iar_tls$$DATA");
    }
    else {
        key = Hwi_disable();
        /*
         * If a Task delete is in progress then read the TLS Ptr from
         * MultithreadSupport_module->deletedTaskTLSPtr and return.
         */
        if ((MultithreadSupport_module->deletedTaskTLSPtr != NULL) &&
            (MultithreadSupport_module->curTaskHandle == Task_self())) {
            Hwi_restore(key);
            return(MultithreadSupport_module->deletedTaskTLSPtr);
        }
        Hwi_restore(key);

        /*  Invoke internal function (generated by template), to get
         *  address of TLS. This function implementation will differ
         *  depending if SYS/BIOS is in ROM or not.
         */
        pCurTaskEnv = MultithreadSupport_getTlsAddr();
    }

    return (pCurTaskEnv);
}

/*
 *  ======== MultithreadSupport_initLock ========
 */
Void MultithreadSupport_initLock(Void **ptr)
{
    Semaphore_Params params;
    MultithreadSupport_Lock *p;

    /* Allocate a lock */
    p = (MultithreadSupport_Lock *)Memory_alloc(Task_Object_heap(),
                                   sizeof(MultithreadSupport_Lock), 0, NULL);
    if (p == NULL) {
        return;
    }

    /* Construct a binary semaphore */
    Semaphore_Params_init(&params);
    params.mode = Semaphore_Mode_BINARY;
    Semaphore_construct(&(p->sem), 1, &params);

    /* Initialize owner for this lock */
    p->owner = NULL;
    /* Initialize count for this lock */
    p->count = 0;

    /* Store the pointer to lock */
    *ptr = p;
}

/*
 *  ======== MultithreadSupport_destroyLock ========
 */
Void MultithreadSupport_destroyLock(Void **ptr)
{
    MultithreadSupport_Lock *p;

    /* Get the lock pointer */
    p = (MultithreadSupport_Lock *)*ptr;

    /* Destruct the semaphore */
    Semaphore_destruct(&(p->sem));

    /* Free the lock */
    Memory_free(Task_Object_heap(), p, sizeof(MultithreadSupport_Lock));
}

/*
 *  ======== MultithreadSupport_acquireLock ========
 */
Void MultithreadSupport_acquireLock(Void **ptr)
{
    MultithreadSupport_Lock *p;
    Ptr self;
    BIOS_ThreadType type;

    /* Get the lock pointer */
    p = (MultithreadSupport_Lock *)*ptr;

    /* Get the thread type */
    type = BIOS_getThreadType();
    if (type == BIOS_ThreadType_Main) {
        self = (Ptr)~(0);
    }
    else {
        self = (Ptr)Task_self();
    }

    /* If nested call is detected, increment lock count */
    if (p->owner == self) {
        (p->count)++;
    }
    else {
        /* acquire lock */
        Semaphore_pend(Semaphore_handle(&(p->sem)), BIOS_WAIT_FOREVER);
        p->count = 1;
        p->owner = self;
    }
}

/*
 *  ======== MultithreadSupport_releaseLock ========
 */
Void MultithreadSupport_releaseLock(Void **ptr)
{
    MultithreadSupport_Lock  *p;
    BIOS_ThreadType type;

    /* Get the lock pointer */
    p = (MultithreadSupport_Lock *)*ptr;

    /* Get the thread type */
    type = BIOS_getThreadType();
    /* Assert current task is the  owner */
    if (type == BIOS_ThreadType_Main) {
        Assert_isTrue((p->owner == (Ptr)~(0)),
                      MultithreadSupport_A_badLockRelease);
    }
    else {
        Assert_isTrue((p->owner == (Ptr)Task_self()),
                      MultithreadSupport_A_badLockRelease);
    }

    /* Decrement the counter */
    (p->count)--;

    /* Release lock */
    if (p->count == 0) {
        p->owner = NULL;
        Semaphore_post(Semaphore_handle(&(p->sem)));
    }

}
