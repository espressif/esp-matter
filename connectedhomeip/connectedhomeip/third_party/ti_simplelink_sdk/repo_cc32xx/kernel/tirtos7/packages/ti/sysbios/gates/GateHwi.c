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
 *  ======== GateHwi.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/gates/GateHwi.h>

#include <ti/sysbios/hal/Hwi.h>

static const struct GateHwi_Params GateHwi_Params_default = {
    .dummy = 0
};

/* internally share a common object for GateHwi_create()'d instances */
static GateHwi_Object GateHwi_object;

/*
 *  ======== GateHwi_canBePreempted ========
 */
bool GateHwi_canBePreempted()
{
    return (false);
}

/*
 *  ======== GateHwi_canBlock ========
 */
bool GateHwi_canBlock()
{
    return (false);
}

/*
 *  ======== GateHwi_construct ========
 */
GateHwi_Handle GateHwi_construct(GateHwi_Object *obj,
                               const GateHwi_Params *params)
{
    return (obj);
}

/*
 *  ======== GateHwi_create ========
 */
GateHwi_Handle GateHwi_create(
                const GateHwi_Params *params, Error_Block *eb)
{
    return (&GateHwi_object);
}

/*
 *  ======== GateHwi_destruct ========
 */
void GateHwi_destruct(GateHwi_Object *obj)
{
}

/*
 *  ======== GateHwi_delete ========
 */
void GateHwi_delete(GateHwi_Handle *gate)
{
    *gate = NULL;
}

/*
 *  ======== GateHwi_enter ========
 */
intptr_t GateHwi_enter(GateHwi_Object *obj)
{
    return ((intptr_t)Hwi_disable());
}

/*
 *  ======== GateHwi_leave ========
 */
void GateHwi_leave(GateHwi_Object *obj, intptr_t key)
{
    Hwi_restore((unsigned int)key);
}

/*
 * ======== GateHwi_Params_init ========
 */
void GateHwi_Params_init(GateHwi_Params *params)
{
    *params = GateHwi_Params_default;
}
