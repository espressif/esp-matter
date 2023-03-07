/*
 * Copyright (c) 2012, Texas Instruments Incorporated
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
 *  ======== GateMutex.c ========
 *  Implementation of functions specified in GateMutex.xdc.
 */

#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include "package/internal/GateMutex.xdc.h"

/* Constant used to denote first level of nesting */
#define FIRST_ENTER   0
#define NESTED_ENTER  1

/*
 * Inheritd IGateProvider. Part of runtime. Called in first pass.
 * APIs ready because Semaphore_pend() and Semaphore_post() use only
 * unconstrained APIs and do no work before BIOS_start(). This is true 
 * even when events are enabled. Note that initial semaphore count is one,
 * timeout is always BIOS_WAIT_FOREVER.
 */

/*
 *  ======== GateMutex_Instance_init ========
 */
Void  GateMutex_Instance_init(GateMutex_Object *obj,
                               const GateMutex_Params *params)
{
    Semaphore_Handle sem;

    sem = GateMutex_Instance_State_sem(obj);
    Semaphore_construct(Semaphore_struct(sem), 1, NULL);
    obj->owner = NULL;
}

/*
 *  ======== GateMutex_Instance_finalize ========
 */
Void GateMutex_Instance_finalize(GateMutex_Object *obj )
{
    Semaphore_Handle sem;

    sem = GateMutex_Instance_State_sem(obj);
    Semaphore_destruct(Semaphore_struct(sem));
}

/*
 *  ======== GateMutex_enter ========
 *  Returns FIRST_ENTER when it gets the gate, returns NESTED_ENTER
 *  on nested calls.
 *
 *  During startup, Task_self returns NULL.  So all calls to the 
 *  GateMutex_enter look like it is a nested call, so nothing done.
 *  Then the leave's will do nothing either. 
 */
IArg GateMutex_enter(GateMutex_Object *obj)
{
    Semaphore_Handle sem;

    /* make sure we're not calling from Hwi or Swi context */
    Assert_isTrue(((BIOS_getThreadType() == BIOS_ThreadType_Task) ||
                   (BIOS_getThreadType() == BIOS_ThreadType_Main)),
                   GateMutex_A_badContext);

    if (obj->owner != Task_self()) {
        sem = GateMutex_Instance_State_sem(obj);
        Semaphore_pend(sem, BIOS_WAIT_FOREVER);

        obj->owner = Task_self();

        return (FIRST_ENTER);
    }

    return (NESTED_ENTER);
}

/*
 *  ======== GateMutex_leave ========
 *  Only releases the gate if key == FIRST_ENTER.
 */
Void GateMutex_leave(GateMutex_Object *obj, IArg key)
{
    Semaphore_Handle sem;

    /* If this is the outermost call to leave, then post the semaphore. */
    if (key == FIRST_ENTER) {
        obj->owner = NULL;
        sem = GateMutex_Instance_State_sem(obj);
        Semaphore_post(sem);
    }
}

/*
 *  ======== query ========
 *  
 */
Bool GateMutex_query(Int qual)
{
    return (TRUE);
}
