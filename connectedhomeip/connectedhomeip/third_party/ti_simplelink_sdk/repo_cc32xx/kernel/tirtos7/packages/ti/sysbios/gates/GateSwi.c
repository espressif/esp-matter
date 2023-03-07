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
 *  ======== GateSwi.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/gates/GateSwi.h>
#include <ti/sysbios/knl/Swi.h>

/* internally share a common object for GateSwi_create()'d instances */
static GateSwi_Object GateSwi_object;

static const struct GateSwi_Params GateSwi_Params_default = {
    .dummy = 0
};

/*
 *  ======== GateSwi_canBePreempted ========
 */
bool GateSwi_canBePreempted()
{
    return (true);
}

/*
 *  ======== GateSwi_canBlock ========
 */
bool GateSwi_canBlock()
{
    return (false);
}

/*
 *  ======== GateSwi_construct ========
 */
GateSwi_Handle GateSwi_construct(GateSwi_Object *obj,
                               const GateSwi_Params *params)
{
    return (obj);
}

/*
 *  ======== GateSwi_create ========
 */
GateSwi_Handle GateSwi_create(
                const GateSwi_Params *params, Error_Block *eb)
{
    return (&GateSwi_object);
}

/*
 *  ======== GateSwi_destruct ========
 */
void GateSwi_destruct(GateSwi_Object *obj)
{
}

/*
 *  ======== GateSwi_delete ========
 */
void GateSwi_delete(GateSwi_Handle *gate)
{
    *gate = NULL;
}

/*
 *  ======== GateSwi_enter ========
 */
intptr_t GateSwi_enter(GateSwi_Object *obj)
{
    /* Ensure that GateSwi is entered within a Task, Swi, or Main */
    Assert_isTrue(((BIOS_getThreadType() == BIOS_ThreadType_Task) ||
                   (BIOS_getThreadType() == BIOS_ThreadType_Swi) ||
                   (BIOS_getThreadType() == BIOS_ThreadType_Main)),
                   GateSwi_A_badContext);

    return (Swi_disable());
}

/*
 *  ======== GateSwi_leave ========
 */
void GateSwi_leave(GateSwi_Object *obj, intptr_t key)
{
    Swi_restore(key);
}

/*
 * ======== GateSwi_Params_init ========
 */
void GateSwi_Params_init(GateSwi_Params *params)
{
    *params = GateSwi_Params_default;
}
