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
 *  ======== SemThreadSupport.c ========
 */

#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/knl/ISemaphore.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Clock.h>

#include "package/internal/SemThreadSupport.xdc.h"

/*
 *  ======== SemThreadSupport_Instance_init ========
 */
Void SemThreadSupport_Instance_init(SemThreadSupport_Handle sem, Int count,
        const SemThreadSupport_Params* params)
{
    Semaphore_Handle bios6sem;
    Semaphore_Params semParams;
    
    Semaphore_Params_init(&semParams);

    if (params->mode == ISemaphore_Mode_COUNTING) {
        semParams.mode = Semaphore_Mode_COUNTING;
    }
    else {
        semParams.mode = Semaphore_Mode_BINARY;
    }

    bios6sem = SemThreadSupport_Instance_State_sem(sem);

    Semaphore_construct(Semaphore_struct(bios6sem), count, &semParams);
}


/*
 *  ======== SemThreadSupport_Instance_finalize ========
 */
Void SemThreadSupport_Instance_finalize(SemThreadSupport_Handle sem)
{
    Semaphore_Handle bios6sem;
 
    bios6sem = SemThreadSupport_Instance_State_sem(sem);
    Semaphore_destruct(Semaphore_struct(bios6sem));
}

/*
 *  ======== SemThreadSupport_pend ========
 */
Int SemThreadSupport_pend(SemThreadSupport_Handle sem, UInt timeout,
    Error_Block *eb)
{
    UInt bios6_timeout;
    Semaphore_Handle bios6sem;
    Int status;
 
    bios6sem = SemThreadSupport_Instance_State_sem(sem);

    if (SemThreadSupport_FOREVER == timeout) {
        bios6_timeout = BIOS_WAIT_FOREVER;
    }
    else if (timeout == 0) {
        bios6_timeout = 0;
    }
    else {
        bios6_timeout = timeout/Clock_tickPeriod;
        /*
         *  Don't let nonzero timeout round to 0 - semantically very
         *  different
         */
        if (timeout && !bios6_timeout) {
            bios6_timeout = 1;
        }
    }

    status = Semaphore_pend(bios6sem, bios6_timeout);

    if (status == 1) {
        return (ISemaphore_PendStatus_SUCCESS);
    }
    else if (status == 0) {
        return (ISemaphore_PendStatus_TIMEOUT);
    }
    else {
        return (ISemaphore_PendStatus_ERROR);
    }
}

/*
 *  ======== SemThreadSupport_post ========
 */
Bool SemThreadSupport_post(SemThreadSupport_Handle sem, Error_Block *eb)
{
    Semaphore_Handle bios6sem;
 
    bios6sem = SemThreadSupport_Instance_State_sem(sem);
    Semaphore_post(bios6sem);

    return (TRUE);
}
