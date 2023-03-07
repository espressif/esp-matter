/*
 * Copyright (c) 2013, Texas Instruments Incorporated
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
 *  ======== HeapCallback.c ========
 */

#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Startup.h>

#include "package/internal/HeapCallback.xdc.h"

/*
 * ======== HeapCallback_Module_startup ========
 */
Int HeapCallback_Module_startup(Int state)
{
    Int i;
    HeapCallback_Object *obj;

    /*
     *  Wait till all xdc runtime modules are initialized. This will allow
     *  the user defined createInstFxn to use xdc runtime modules (for e.g.
     *  heap modules).
     */
    if (!Startup_rtsDone()) {
        return (Startup_NOTDONE);
    }

    /*
     *  Loop through all statically configured instances
     *  Call the initInstFxn and set the context for each instance.
     */
    for (i = 0; i < HeapCallback_Object_count(); i++) {
        obj = HeapCallback_Object_get(NULL, i);
        obj->context = HeapCallback_initInstFxn(obj->arg);
    }

    return (Startup_DONE);
}

/*
 *  ======== HeapCallback_Instance_finalize ========
 */
Void HeapCallback_Instance_finalize(HeapCallback_Object *obj)
{
    HeapCallback_deleteInstFxn(obj->context);
}

/*
 *  ======== HeapCallback_Instance_init ========
 */
Void HeapCallback_Instance_init(HeapCallback_Object *obj,
        const HeapCallback_Params *params)
{
    obj->arg     = params->arg;
    obj->context = HeapCallback_createInstFxn(obj->arg);
}

/*
 *  ======== HeapCallback_alloc ========
 */
Ptr HeapCallback_alloc(HeapCallback_Object *obj, SizeT size,
        SizeT align, Error_Block *eb)
{
    Ptr allocAddr;

    allocAddr = HeapCallback_allocInstFxn(obj->context, size, align);

    return (allocAddr);
}

/*
 *  ======== HeapCallback_free ========
 */
Void HeapCallback_free(HeapCallback_Object *obj, Ptr addr, SizeT size)
{
    HeapCallback_freeInstFxn(obj->context, addr, size);
}

/*
 *  ======== HeapCallback_getContext ========
 */
UArg HeapCallback_getContext(HeapCallback_Object *obj)
{
    return (obj->context);
}

/*
 *  ======== HeapCallback_getStats ========
 */
Void HeapCallback_getStats(HeapCallback_Object *obj, Memory_Stats *stats)
{
    HeapCallback_getStatsInstFxn(obj->context, stats);
}

/*
 *  ======== HeapCallback_isBlocking ========
 */
Bool HeapCallback_isBlocking(HeapCallback_Object *obj)
{
    return (HeapCallback_isBlockingInstFxn(obj->context));
}

/*  ======== Default instance functions ======== */
/*
 *  ======== HeapCallback_defaultAlloc ========
 */
Ptr ti_sysbios_heaps_HeapCallback_defaultAlloc(UArg context,
        SizeT size, SizeT align)
{
    return (NULL);
}

/*
 *  ======== HeapCallback_defaultCreate  ========
 *  Default implementation of create callback function
 */
UArg ti_sysbios_heaps_HeapCallback_defaultCreate(UArg arg)
{
    return (0);
}

/*
 *  ======== HeapCallback_defaultDelete  ========
 *  Default implementation of delete callback function
 */
Void ti_sysbios_heaps_HeapCallback_defaultDelete(UArg context)
{
}

/*
 *  ======== HeapCallback_defaultFree ========
 */
Void ti_sysbios_heaps_HeapCallback_defaultFree(UArg context, Ptr addr,
        SizeT size)
{
}

/*
 *  ======== HeapCallback_defaultGetStats ========
 */
Void ti_sysbios_heaps_HeapCallback_defaultGetStats(UArg context,
        Memory_Stats *stats)
{
}

/*
 *  ======== HeapCallback_defaultInit ========
 */
UArg ti_sysbios_heaps_HeapCallback_defaultInit(UArg arg)
{
    return (0);
}

/*
 *  ======== HeapCallback_defaultIsBlocking ========
 */
Bool ti_sysbios_heaps_HeapCallback_defaultIsBlocking(UArg context)
{
    return (FALSE);
}
