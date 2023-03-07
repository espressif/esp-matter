/*
 * Copyright (c) 2017-2020, Texas Instruments Incorporated
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
 *  ======== SwiP_tirtos.c ========
 */


#include <ti/drivers/dpl/SwiP.h>

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <xdc/std.h>
#include <xdc/runtime/Error.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Swi.h>

/*
 *  ======== SwiP_Params_init ========
 */
void SwiP_Params_init(SwiP_Params *params)
{
    params->arg0 = 0U;
    params->arg1 = 0U;
    params->trigger = 0U;
    params->priority = ~0U;
}

/*
 *  ======== SwiP_construct ========
 */
SwiP_Handle SwiP_construct(SwiP_Struct *handle, SwiP_Fxn swiFxn,
        SwiP_Params *params)
{
    Swi_Handle   swi;
    Swi_Params   swiParams;

    if (params == NULL) {
        swi = Swi_construct2((Swi_Struct2 *)handle, swiFxn, NULL);
    }
    else {
        if ((params->priority >= Swi_numPriorities) && (params->priority != ~(0U))) {
            /* Swi_construct2() will not return NULL on priority error */
            return (NULL);
        }
        Swi_Params_init(&swiParams);

        swiParams.arg0 = params->arg0;
        swiParams.arg1 = params->arg1;
        swiParams.trigger = params->trigger;
        swiParams.priority = params->priority;
        swi = Swi_construct2((Swi_Struct2 *)handle, swiFxn, &swiParams);
    }

    return ((SwiP_Handle)swi);
}

/*
 *  ======== SwiP_create ========
 */
SwiP_Handle SwiP_create(SwiP_Fxn swiFxn, SwiP_Params *params)
{
    Swi_Handle   handle;
    Swi_Params   swiParams;

    if (params == NULL) {
        handle = Swi_create(swiFxn, NULL, Error_IGNORE);
    }
    else {
        if ((params->priority >= Swi_numPriorities) && (params->priority != ~(0U))) {
            /* Swi_create() only asserts params is within range */
            return (NULL);
        }
        Swi_Params_init(&swiParams);
        swiParams.arg0 = params->arg0;
        swiParams.arg1 = params->arg1;
        swiParams.trigger = params->trigger;
        swiParams.priority = params->priority;
        handle = Swi_create(swiFxn, &swiParams, Error_IGNORE);
    }

    return ((SwiP_Handle)handle);
}

/*
 *  ======== SwiP_delete ========
 */
void SwiP_delete(SwiP_Handle handle)
{
    Swi_Handle swi = (Swi_Handle)handle;

    Swi_delete(&swi);
}

/*
 *  ======== SwiP_destruct ========
 */
void SwiP_destruct(SwiP_Struct *handle)
{
    Swi_destruct((Swi_Struct *)handle);
}

/*
 *  ======== SwiP_disable ========
 */
uintptr_t SwiP_disable(void)
{
    uintptr_t key;

    key = Swi_disable();

    return (key);
}

/*
 *  ======== SwiP_getTrigger ========
 */
uint32_t SwiP_getTrigger()
{
    uint32_t trigger;

    trigger = Swi_getTrigger();
    return (trigger);
}

/*
 *  ======== SwiP_andn ========
 */
void SwiP_andn(SwiP_Handle handle, uint32_t mask)
{
    Swi_andn((Swi_Handle)handle, mask);
}

/*
 *  ======== SwiP_dec ========
 */
void SwiP_dec(SwiP_Handle handle)
{
    Swi_dec((Swi_Handle)handle);
}

/*
 *  ======== SwiP_inc ========
 */
void SwiP_inc(SwiP_Handle handle)
{
    Swi_inc((Swi_Handle)handle);
}

/*
 *  ======== SwiP_inISR ========
 */
bool SwiP_inISR(void)
{
    BIOS_ThreadType threadType;

    threadType = BIOS_getThreadType();
    if (threadType == BIOS_ThreadType_Swi) {
        return (true);
    }

    return (false);
}

/*
 *  ======== SwiP_or ========
 */
void SwiP_or(SwiP_Handle handle, uint32_t mask)
{
    Swi_or((Swi_Handle)handle, mask);
}

/*
 *  ======== SwiP_post ========
 */
void SwiP_post(SwiP_Handle handle)
{
    Swi_post((Swi_Handle)handle);
}

/*
 *  ======== SwiP_restore ========
 */
void SwiP_restore(uintptr_t key)
{
    Swi_restore(key);
}

/*
 *  ======== SwiP_setPriority ========
 */
void SwiP_setPriority(SwiP_Handle handle, uint32_t priority)
{
    if ((priority < Swi_numPriorities) || (priority == ~(0U))) {
        Swi_setPri((Swi_Handle)handle, priority);
    }
}

/*
 *  ======== SwiP_staticObjectSize ========
 */
size_t SwiP_staticObjectSize(void)
{
    return (sizeof(Swi_Struct));
}
