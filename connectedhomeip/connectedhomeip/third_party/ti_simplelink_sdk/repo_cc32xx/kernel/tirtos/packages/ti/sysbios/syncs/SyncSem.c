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
 *  ======== SyncSem.c ========
 */

#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/knl/ISync.h>

#include <xdc/runtime/knl/ISync.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>

#include "package/internal/SyncSem.xdc.h"

/*
 *  ======== SyncSem_query ========
 */
Bool SyncSem_query(SyncSem_Object *obj, Int qual)
{
    if (qual == ISync_Q_BLOCKING) {
        return (TRUE);
    }
    else {
        return (FALSE);
    }
}

/*
 *  ======== SyncSem_Instance_init ========
 */
Int  SyncSem_Instance_init(SyncSem_Object *obj, const SyncSem_Params *params, 
    Error_Block *eb)
{
    Semaphore_Params semPrms;

    if (params->sem == NULL) {
        Semaphore_Params_init(&semPrms);
        semPrms.mode = Semaphore_Mode_BINARY;
        obj->sem = Semaphore_create(0, &semPrms, eb);
        if (obj->sem == NULL) {
            return (1);
        }
        obj->userSem = FALSE;
    }
    else {
        obj->userSem = TRUE;
        obj->sem = params->sem;
        Assert_isTrue(!Semaphore_getCount(obj->sem), NULL);
    }

    return (0);
}

/*
 *  ======== SyncSem_Instance_finalize ========
 */
Void SyncSem_Instance_finalize(SyncSem_Object *obj, Int status)
{    
    if (!(obj->userSem)) {
        Semaphore_delete(&obj->sem);
    }
}

/*
 *  ======== SyncSem_wait ========
 */
Int SyncSem_wait(SyncSem_Object *obj, UInt timeout, Error_Block *eb)
{
    UInt bios6_timeout;
    Int status;
    
    if (timeout == ISync_WAIT_FOREVER) {
        bios6_timeout = BIOS_WAIT_FOREVER;
    }
    else {
        bios6_timeout = timeout / Clock_tickPeriod;
        /*
         *  Don't let nonzero timeout round to 0 - semantically very
         *  different
         */
        if (timeout && (!bios6_timeout)) {
            bios6_timeout = 1;
        }
    }

    status = Semaphore_pend(obj->sem, bios6_timeout);

    if (status == 1) {
        return (ISync_WaitStatus_SUCCESS);
    }
    else if (status == 0) {
        return (ISync_WaitStatus_TIMEOUT);
    }
    else {
        return (ISync_WaitStatus_ERROR);
    }
}

/*
 *  ======== SyncSem_signal ========
 */
Void SyncSem_signal(SyncSem_Object *obj)
{
    Semaphore_post(obj->sem);
}


