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
 *  ======== GateMutex.c ========
 *  Implementation of functions specified in GateMutex.xdc.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/gates/GateMutex.h>

#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/runtime/Assert.h>

/* Constant used to denote first level of nesting */
#define FIRST_ENTER   0
#define NESTED_ENTER  1

GateMutex_Module_State GateMutex_Module_state = {
    .objQ.next = &GateMutex_Module_state.objQ,
    .objQ.prev = &GateMutex_Module_state.objQ
};

/* Params */
static const struct GateMutex_Params GateMutex_Params_default = {
    .dummy = 0
};

/*
 *  ======== GateMutex_canBePreempted ========
 */
bool GateMutex_canBePreempted()
{
    return (true);
}

/*
 *  ======== GateMutex_canBlock ========
 */
bool GateMutex_canBlock()
{
    return (true);
}

/*
 *  ======== GateMutex_construct ========
 */
GateMutex_Handle GateMutex_construct(GateMutex_Object *obj,
                               const GateMutex_Params *params)
{
    if (params == NULL) {
        params = &GateMutex_Params_default;
    }

    GateMutex_Instance_init(obj, params);

    return (obj);
}

/*
 *  ======== GateMutex_create ========
 */
GateMutex_Handle GateMutex_create(
                const GateMutex_Params *params, Error_Block *eb)
{
    GateMutex_Handle gate;

    gate = Memory_alloc(NULL, sizeof(GateMutex_Object), 0, eb);

    if (gate != NULL) {
        gate = GateMutex_construct(gate, params);
    }

    return (gate);
}

/*
 *  ======== GateMutex_Instance_init ========
 */
void  GateMutex_Instance_init(GateMutex_Object *obj,
                               const GateMutex_Params *params)
{
    Semaphore_construct(&obj->sem, 1, NULL);
    obj->owner = NULL;

    /* put GateMutex object on global GateMutex Object list (Queue_put is atomic) */
    Queue_put(&GateMutex_module->objQ, &obj->objElem);
}

/*
 *  ======== GateMutex_destruct ========
 */
void GateMutex_destruct(GateMutex_Object *obj )
{
    unsigned int hwiKey;

    GateMutex_Instance_finalize(obj);

    /* remove GateMutex object from global GateMutex object list (Queue_remove is not atomic) */
    hwiKey = Hwi_disable();
    Queue_remove(&obj->objElem);
    Hwi_restore(hwiKey);
}

/*
 *  ======== GateMutex_delete ========
 */
void GateMutex_delete(GateMutex_Handle *gate)
{
    GateMutex_destruct(*gate);

    Memory_free(NULL, *gate, sizeof(GateMutex_Object));

    *gate = NULL;
}

/*
 *  ======== GateMutex_Instance_finalize ========
 */
void GateMutex_Instance_finalize(GateMutex_Object *obj )
{
    Semaphore_destruct(&obj->sem);
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
intptr_t GateMutex_enter(GateMutex_Object *obj)
{
    /* make sure we're not calling from Hwi or Swi context */
    Assert_isTrue(((BIOS_getThreadType() == BIOS_ThreadType_Task) ||
                   (BIOS_getThreadType() == BIOS_ThreadType_Main)),
                   GateMutex_A_badContext);

    if (obj->owner != Task_self()) {
        Semaphore_pend(&obj->sem, BIOS_WAIT_FOREVER);

        obj->owner = Task_self();

        return (FIRST_ENTER);
    }

    return (NESTED_ENTER);
}

/*
 *  ======== GateMutex_leave ========
 *  Only releases the gate if key == FIRST_ENTER.
 */
void GateMutex_leave(GateMutex_Object *obj, intptr_t key)
{
    /* If this is the outermost call to leave, then post the semaphore. */
    if (key == FIRST_ENTER) {
        obj->owner = NULL;
        Semaphore_post(&obj->sem);
    }
}

/*
 *  ======== query ========
 */
bool GateMutex_query(int qual)
{
    return (true);
}

/*
 * ======== Params_init ========
 */
void GateMutex_Params_init(GateMutex_Params *params)
{
    *params = GateMutex_Params_default;
}

/*
 *  ======== GateMutex_getGateMutexFromObjElem ========
 */
GateMutex_Handle GateMutex_getGateMutexFromObjElem(Queue_Elem *gateQelem)
{
    if (gateQelem == (Queue_Elem *)&GateMutex_module->objQ) {
        return (NULL);
    }

    return ((GateMutex_Handle)((char *)gateQelem -
               offsetof(GateMutex_Struct, objElem)));
}

/*
 *  ======== GateMutex_Object_first ========
 */
GateMutex_Handle GateMutex_Object_first()
{
    return (GateMutex_getGateMutexFromObjElem(Queue_head(&(GateMutex_module->objQ))));
}

/*
 *  ======== GateMutex_Object_next ========
 */
GateMutex_Handle GateMutex_Object_next(GateMutex_Handle handle)
{
    return (GateMutex_getGateMutexFromObjElem(Queue_next(&handle->objElem)));
}
