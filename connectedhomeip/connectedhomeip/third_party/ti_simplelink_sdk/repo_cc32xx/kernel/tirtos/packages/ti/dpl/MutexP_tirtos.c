/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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
 *  ======== MutexP_tirtos.c ========
 */

#include <ti/drivers/dpl/MutexP.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/gates/GateMutex.h>


/*
 *  ======== MutexP_construct ========
 */
MutexP_Handle MutexP_construct(MutexP_Struct *handle, MutexP_Params *params)
{
    GateMutex_Handle gate;

    GateMutex_construct((GateMutex_Struct *)handle, NULL);

    gate = GateMutex_handle((GateMutex_Struct *)handle);

    return ((MutexP_Handle)gate);
}

/*
 *  ======== MutexP_create ========
 */
MutexP_Handle MutexP_create(MutexP_Params *params)
{
    GateMutex_Handle handle;

    handle = GateMutex_create(NULL, Error_IGNORE);

    return ((MutexP_Handle)handle);
}

/*
 *  ======== MutexP_delete ========
 */
void MutexP_delete(MutexP_Handle handle)
{
    GateMutex_Handle gateMutex = (GateMutex_Handle)handle;

    GateMutex_delete(&gateMutex);
}

/*
 *  ======== MutexP_destruct ========
 */
void MutexP_destruct(MutexP_Struct *mutexP)
{
    GateMutex_destruct((GateMutex_Struct *)mutexP);
}

/*
 *  ======== MutexP_lock ========
 */
uintptr_t MutexP_lock(MutexP_Handle handle)
{
    unsigned int key;

    key = GateMutex_enter((GateMutex_Handle)handle);

    return ((uintptr_t)key);
}

/*
 *  ======== MutexP_Params_init ========
 */
void MutexP_Params_init(MutexP_Params *params)
{
    params->callback = NULL;
}

/*
 *  ======== MutexP_staticObjectSize ========
 */
size_t MutexP_staticObjectSize(void)
{
    return (sizeof(GateMutex_Struct));
}

/*
 *  ======== MutexP_unlock ========
 */
void MutexP_unlock(MutexP_Handle handle, uintptr_t key)
{
    GateMutex_leave((GateMutex_Handle)handle, (unsigned int)key);
}
