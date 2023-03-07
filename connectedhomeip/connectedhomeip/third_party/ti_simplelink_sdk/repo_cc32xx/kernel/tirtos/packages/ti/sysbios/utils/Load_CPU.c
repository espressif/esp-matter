/*
 * Copyright (c) 2013-2018, Texas Instruments Incorporated
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
 *  ======== Load_CPU.c ========
 *  Functions for CPU Load logging with Power idling enabled.
 *  Different idle and CPU load update functions are used when power
 *  management is enabled.
 */

#include <xdc/std.h>

#include <xdc/runtime/Startup.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Core.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Idle.h>

#include "package/internal/Load.xdc.h"


/*
 *************************************************************************
 *                       Internal functions
 *************************************************************************
 */


/*
 *  ======== Load_idleFxnPwr ========
 *  Idle function when Power idling is enabled.
 */
Void Load_idleFxnPwr(Void)
{
    Types_FreqHz freq;
    UInt32 window;
    UInt32 t1;
    UInt32 delta;
    UInt key;

    key = Hwi_disable();

    t1 = Timestamp_get32();

    if (Load_module->busyStartTime == 0) {
        /* No interrupt occurred since previous call to this function */
        /*
         *  This code should not be executed since only one iteration of the
         *  the idle loop can execute after an interrupt. This could be used
         *  in case Power idling was disabled, but then user idle functions
         *  would be counted in idle time instead of load time. (Although if
         *  an interrupt occurs in the middle of the idle function, the
         *  remaining part to execute is counted as load time). The code is
         *  here to show how CPU load calculation could be simplified if the
         *  user didn't have Idle functions.
         */

        /* An interrupt has not occurred */
        delta = t1 - Load_module->idleStartTime;

        /* If first call to this function add delta to the busy time */
        if (Load_module->idleStartTime == 0) {
            Load_module->busyTime += delta;
        }
        Load_module->timeElapsed += delta;
    }
    else {
        /*
         *  Load_module->busyTime != 0 means an interrupt has occurred since
         *  the last call to the idle function (since the Hwi begin hook
         *  would have set it).  Accumulate times.
         */
        delta = Load_module->busyStartTime - Load_module->idleStartTime;

        /* For first call to the idle function, delta is busy time. */
        if (Load_module->idleStartTime == 0) {
            Load_module->busyTime += delta;
        }
        Load_module->timeElapsed += delta;

        delta = t1 - Load_module->busyStartTime;

        Load_module->busyTime += delta;
        Load_module->timeElapsed += delta;

        Load_module->busyStartTime = 0;
    }
    Load_module->idleStartTime = t1;

    Hwi_restore(key);

    /*
     *  We do this initialization in the idle loop in case the timestamp
     *  frequency changes (e.g due to frequency scaling), in which case the
     *  window would be automatically adjusted.
     */
    Timestamp_getFreq(&freq);
    window = (freq.lo / 1000) * Load_windowInMs;

    /* Compute the load if the time window has expired. */
    if ((Load_module->timeElapsed >= window) && (Load_updateInIdle)) {
        Load_update();
    }
}

/*
 *************************************************************************
 *                      Module wide functions
 *************************************************************************
 */

/*
 *  ======== Load_updateCPULoad ========
 */
Void Load_updateCPULoad(Void)
{
    UInt key;
    Load_FuncPtr postUpdate;

    postUpdate = Load_postUpdate; /* from config */

    /* Update Global CPU load */
    key = Hwi_disable();

    if (Load_module->timeElapsed > ((UInt32)1 << 24)) {
        /* if total's too big, we scale it down to avoid overflow */
        Load_module->cpuLoad = ((100 * ((Load_module->busyTime) >> 7)) +
                (Load_module->timeElapsed >> 8))
            / (Load_module->timeElapsed >> 7);
    }
    else if (Load_module->timeElapsed != 0) {
        Load_module->cpuLoad = ((100 *
                Load_module->busyTime  + (Load_module->timeElapsed >> 1))
               / Load_module->timeElapsed);
    }
    else {
        /* If timeElapsed = 0, idle function has not had a chance to run yet */
        Load_module->cpuLoad = 100;
    }

    Load_module->busyTime = 0;
    Load_module->timeElapsed = 0;

    Hwi_restore(key);

    if (Diags_query(Diags_USER4)) {
        Load_logCPULoad();
    }

    if (postUpdate != NULL) {
        postUpdate();   /* Call user-defined postUpdate fxn */
    }
}
