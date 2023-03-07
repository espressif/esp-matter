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
 *  ======== GateTask.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/gates/GateTask.h>
#include <ti/sysbios/knl/Task.h>

/* internally share a common object for GateTask_create()'d instances */
static GateTask_Object GateTask_object;

static const struct GateTask_Params GateTask_Params_default = {
    .dummy = 0
};

/*
 *  ======== GateTask_canBePreempted ========
 */
bool GateTask_canBePreempted()
{
    return (true);
}

/*
 *  ======== GateTask_canBlock ========
 */
bool GateTask_canBlock()
{
    return (false);
}

/*
 *  ======== GateTask_construct ========
 */
GateTask_Handle GateTask_construct(GateTask_Object *obj,
                               const GateTask_Params *params)
{
    return (obj);
}

/*
 *  ======== GateTask_create ========
 */
GateTask_Handle GateTask_create(
                const GateTask_Params *params, Error_Block *eb)
{
    return (&GateTask_object);
}

/*
 *  ======== GateTask_destruct ========
 */
void GateTask_destruct(GateTask_Object *obj)
{
}

/*
 *  ======== GateTask_delete ========
 */
void GateTask_delete(GateTask_Handle *gate)
{
    *gate = NULL;
}

/*
 *  ======== GateTask_enter ========
 */
intptr_t GateTask_enter(GateTask_Object *obj)
{
    /* Ensure that GateTask is entered within a Task or Main */
    Assert_isTrue(((BIOS_getThreadType() == BIOS_ThreadType_Task) ||
                   (BIOS_getThreadType() == BIOS_ThreadType_Main)),
                   GateTask_A_badContext);

    return (Task_disable());
}

/*
 *  ======== GateTask_leave ========
 */
void GateTask_leave(GateTask_Object *obj, intptr_t key)
{
    Task_restore(key);
}

/*
 * ======== GateTask_Params_init ========
 */
void GateTask_Params_init(GateTask_Params *params)
{
    *params = GateTask_Params_default;
}
