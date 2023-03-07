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
 *  ======== ThreadSupport.xdc ========
 */

import xdc.runtime.Diags;
import xdc.runtime.Log;
import xdc.runtime.Error;
import xdc.runtime.knl.IThreadSupport;
import ti.sysbios.knl.Task;

@InstanceInitError      /* because initialization can fail */
@InstanceFinalize       /* have to call Semaphore_destruct on delete */
module ThreadSupport inherits IThreadSupport
{
    /*! Error raised when thread priority is invalid */ 
    config Error.Id E_priority = {
        msg: "E_priority: Thread priority is invalid %d"
    };

    /*! Log when Thread function starts */
    config Log.Event L_start = {
        mask: Diags.LIFECYCLE,
        msg: "<-- start: (%p)"
    };

    /*! Log when Thread function finishes */
    config Log.Event L_finish = {
        mask: Diags.LIFECYCLE,
        msg: "--> finish: (%p)"
    };

    /*! Log the joined thread */
    config Log.Event L_join = {
        mask: Diags.LIFECYCLE,
        msg: "--> join: (%p)"
    };

internal:

    config UInt lowestPriority;
    config UInt belowNormalPriority;
    config UInt normalPriority;
    config UInt aboveNormalPriority;
    config UInt highestPriority;

    const Int PRI_FAILURE = 1;

    const Int TASK_FAILURE = 2;

    /*! runStub functions required to support ThreadSupport.join */
    Void runStub(UArg arg1, UArg arg2);

    struct Instance_State {
        Task.Handle task;
        Ptr tls;                /* store tls parameter */
        RunFxn startFxn;        /* store fxn parameter */
        IArg startFxnArg;       /* store arg parameter */
        ti.sysbios.knl.Semaphore.Object join_sem; /* used for Thread_join */
    }

}
