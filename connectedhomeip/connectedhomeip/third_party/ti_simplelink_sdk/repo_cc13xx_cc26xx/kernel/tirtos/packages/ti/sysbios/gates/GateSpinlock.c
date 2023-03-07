/*
 * Copyright (c) 2014, Texas Instruments Incorporated
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
 *  ======== GateSpinlock.c ========
 *  Implementation of functions specified in GateSpinlock.xdc.
 */

#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/IGateProvider.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Core.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include "package/internal/GateSpinlock.xdc.h"

/* Constant used to denote first level of nesting */
#define FIRST_ENTER   0
#define NESTED_ENTER  1

/*
 *  ======== GateSpinlock_Instance_init ========
 */
Void  GateSpinlock_Instance_init(GateSpinlock_Object *obj,
                               const GateSpinlock_Params *params)
{
    obj->owner = NULL;
}

/*
 *  ======== GateSpinlock_enter ========
 *  Returns FIRST_ENTER when it gets the gate, returns NESTED_ENTER
 *  on nested calls.
 *
 *  During startup, Task_self returns NULL.  So all calls to the
 *  GateSpinlock_enter look like it is a nested call, so nothing done.
 *  Then the leave's will do nothing either.
 */
IArg GateSpinlock_enter(GateSpinlock_Object *obj)
{
    UInt key;

    /* make sure we're not calling from Hwi or Swi context */
    Assert_isTrue(((BIOS_getThreadType() == BIOS_ThreadType_Task) ||
                   (BIOS_getThreadType() == BIOS_ThreadType_Main)),
                   GateSpinlock_A_badContext);

    if (obj->owner == Task_self()) {
        return (NESTED_ENTER);
    }

    key = Hwi_disable();

    while (1) {
        if (obj->owner == NULL) {
            obj->owner = Task_self();
            break;
        }
        else {
            /* Re-enable interrupts and release inter-core lock */
            Hwi_restore(key);
            /* Wait for lock to be released */
            while (obj->owner != NULL);
            key = Hwi_disable();
        }
    }

    Hwi_restore(key);

    return (FIRST_ENTER);
}

/*
 *  ======== GateSpinlock_enterHwi ========
 *  Returns FIRST_ENTER when it gets the gate, returns NESTED_ENTER
 *  on nested calls. This function returns with interrupt disabled.
 *
 *  During startup, Task_self returns NULL.  So all calls to the
 *  GateSpinlock_enter look like it is a nested call, so nothing done.
 *  Then the leave's will do nothing either.
 */
IArg GateSpinlock_enterHwi(GateSpinlock_Object *obj)
{
    UInt key;

    /* make sure we're not calling from Hwi or Swi context */
    Assert_isTrue(((BIOS_getThreadType() == BIOS_ThreadType_Task) ||
                   (BIOS_getThreadType() == BIOS_ThreadType_Main)),
                   GateSpinlock_A_badContext);

    if (obj->owner == Task_self()) {
        return (NESTED_ENTER);
    }

    key = Hwi_disable();

    while (1) {
        if (obj->owner == NULL) {
            obj->owner = Task_self();
            break;
        }
        else {
            /* Re-enable interrupts and release inter-core lock */
            Hwi_restore(key);
            /* Wait for lock to be released */
            while (obj->owner != NULL);
            key = Hwi_disable();
        }
    }

    /* Store hwi key */
    obj->hwiKey = key;

    /* Release inter-core lock */
    if (BIOS_smpEnabled) {
        Core_unlock();
    }

    return (FIRST_ENTER);
}

/*
 *  ======== GateSpinlock_leave ========
 *  Only releases the gate if key == FIRST_ENTER.
 */
Void GateSpinlock_leave(GateSpinlock_Object *obj, IArg key)
{
    /* If this is the outermost call to leave, then release the gate. */
    if (key == FIRST_ENTER) {
        obj->owner = NULL;
    }
}

/*
 *  ======== GateSpinlock_leaveHwi ========
 *  Only releases the gate if key == FIRST_ENTER.
 */
Void GateSpinlock_leaveHwi(GateSpinlock_Object *obj, IArg key)
{
    volatile UInt hwiKey;

    /* If this is the outermost call to leave, then release the gate. */
    if (key == FIRST_ENTER) {
        hwiKey = obj->hwiKey;
#if defined(__GNUC__) && !defined(__ti__)
        /*
         * Use GNU compiler intrinsic to insert memory barrier. This barrier
         * is to ensure obj->hwiKey is copied to hwiKey before we release
         * the lock. With TI compilers, declaring obj->hwiKey and obj->owner
         * as volatile is enough and will guarantee the instructions are not
         * re-ordered.
         */
        __sync_synchronize();
#endif
        obj->owner = NULL;
        Core_hwiRestore(hwiKey);
    }
}

/*
 *  ======== query ========
 *
 */
Bool GateSpinlock_query(Int qual)
{
    Bool rc;

    switch (qual) {
        case IGateProvider_Q_BLOCKING:
           rc = FALSE;
           break;
        case IGateProvider_Q_PREEMPTING:
           rc = TRUE;
           break;
        default:
           Assert_isTrue(FALSE, GateSpinlock_A_invalidQuality);
           rc = FALSE;
           break;
    }
    return (rc);
}
