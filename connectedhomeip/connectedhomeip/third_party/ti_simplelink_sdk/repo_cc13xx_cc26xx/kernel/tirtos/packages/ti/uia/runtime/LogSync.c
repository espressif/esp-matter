/*
 * Copyright (c) 2012-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
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
 * */

/*
 *  ======== LogSync.c ========
 */

#include <xdc/runtime/Error.h>
#include <xdc/runtime/Gate.h>
#include <ti/uia/events/UIASync.h>
#include <xdc/runtime/ILogger.h>
#include <ti/uia/runtime/LoggerTypes.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Timestamp.h>
#include <ti/uia/runtime/IUIATraceSyncProvider.h>
#include "package/internal/LogSync.xdc.h"

#ifdef xdc_target__isaCompatible_64P
#include <c6x.h>
#endif

/* set numTimesHalted = 1 to ensure a sync point event is logged upon reset */
volatile UInt32 ti_uia_runtime_LogSync_gNumTimesHalted = 1;

/*
 *  ======== Log_Module_startup ========
 */
Int LogSync_Module_startup(Int phase) {
    LogSync_module->numTimesHalted = 0;
    LogSync_module->serialNumber = 1;
    /* Delay declaring Startup_DONE for LogSync until the timestamps have been initialized.  This
     * allows the logger that is used for sync points to wait until LogSync has finished initializing,
     * and then automatically log a sync point upon startup, with valid local and global timestamps.
     */
    if ((LogSync_GlobalTimestampProxy_Module_startupDone())&&
        (LogSync_CpuTimestampProxy_Module_startupDone())
        ) {

        /* A LogSync event needs to be written at startup,
        * with a valid CPU timestamp and a valid global timestamp
        * in order to enable Multicore event correlation from that
        * point onwards.  It can?t rely on BIOS modules, so it can?t
        * wait for them to complete, so it has to ensure that the
        * timestamps are running by starting them itself.
        */

#ifdef xdc_target__isaCompatible_64P
        /* Start the timestamp by writing a value into it.
         * Note: this does not change the value of the timestamp.
         * The first write starts the counter ? subsequent writes will
         * have no affect on the value of the timer.
         */
        TSCL = 0;
#endif
        /*
         * The following line should be enabled once the
         * Global Timebase Server has been updated to cope
         * with timeline discontinuities caused by breakpoints
         * (e.g. halting at main)
         * LogSync_writeSyncPoint();
         */
        return (Startup_DONE);
    } else {
        return(Startup_NOTDONE);
    }
}

/*!
 *  ======== enable ========
 *  Enable logging of sync point events
 *
 *  The function returns the state of the sync point event logging (`TRUE` if
 *  enabled,`FALSE` if disabled) before the call. This return value allows
 *  clients to restore the previous state.
 */
Bool LogSync_enable(){
    Bool oldValue = LogSync_module->isEnabled;
    LogSync_module->isEnabled = TRUE;
    return(oldValue);
}

/*!
 *  ======== disable ========
 *  Disable a log
 *
 *  Events written to a disabled log are silently discarded.
 *
 *  The function returns the state of the log (`TRUE` if enabled,
 *  `FALSE` if disabled) before the call. This return value allows
 *  clients to restore the previous state.
 */
Bool LogSync_disable(){
    Bool oldValue = LogSync_module->isEnabled;
    LogSync_module->isEnabled = FALSE;
    return(oldValue);
}

/*
 * ========  LogSync_isSyncEventRequired =========
 * returns true if a Sync Event needs to be logged
 */
Bool LogSync_isSyncEventRequired() {
    Bool result = FALSE;
    if (LogSync_module->numTimesHalted
            != ti_uia_runtime_LogSync_gNumTimesHalted) {
        result = TRUE;
    }
    return result;
}

/*
 *  ======== LogSync_putSyncPoint ========
 *  Log a sync point event to enable multicore event correlation.
 *
 *  Note: One of the parameters to the sync point event is a unique
 *  serial number that can be injected into the hardware trace
 *  stream by the device-specific injectIntoTraceFxn callback function.
 */
Void LogSync_putSyncPoint()
{
    IArg key;
    UInt32 serialNum;
    Types_Timestamp64 globalTStamp;
    Types_Timestamp64 cpuTStamp;
    Types_FreqHz globalFreq;
    Types_FreqHz cpuFreq;
    xdc_runtime_Log_Event evt = UIASync_syncPoint;
    if (LogSync_module->isEnabled){
        if (LogSync_module->numTimesHalted !=
            ti_uia_runtime_LogSync_gNumTimesHalted){
            evt = UIASync_syncPointAfterHalt;
            LogSync_module->numTimesHalted =
                ti_uia_runtime_LogSync_gNumTimesHalted;
        }

        /* Lock out preemption in order to ensure that a unique serial number
         * is used and that the global timestamp is in sync with the event
         * timestamp
         */
        key = Gate_enterSystem();
        serialNum = LogSync_module->serialNumber++;
        LogSync_GlobalTimestampProxy_get64(&globalTStamp);
#ifndef xdc_target__isaCompatible_64P
        LogSync_CpuTimestampProxy_get64(&cpuTStamp);
#else
        cpuTStamp.lo = TSCL;
        cpuTStamp.hi = TSCH;
#endif
        LogSync_GlobalTimestampProxy_getFreq(&globalFreq);
        LogSync_CpuTimestampProxy_getFreq(&cpuFreq);

        /* Inject the sync point serial number into hardware trace stream
         * if a device-specific callback function has been configured for use
         * by the LogSync module */
        if (LogSync_injectIntoTraceFxn != NULL){
            LogSync_injectIntoTraceFxn(serialNum,
                    IUIATraceSyncProvider_ContextType_SyncPoint);
        }
        Gate_leaveSystem(key);

        /* Log the events directly without any filtering based on the
         * diags mask. */
        xdc_runtime_ILogger_write8(ti_uia_runtime_LogSync_Module__loggerObj__C,
                evt,ti_uia_runtime_LogSync_Module__id__C, serialNum,
                cpuTStamp.lo, cpuTStamp.hi, globalTStamp.lo,globalTStamp.hi,
                LogSync_cpuTimestampCyclesPerTick,cpuFreq.lo,cpuFreq.hi);
        xdc_runtime_ILogger_write4(ti_uia_runtime_LogSync_Module__loggerObj__C,
                UIASync_globalTimerFreq,ti_uia_runtime_LogSync_Module__id__C,
                serialNum, LogSync_globalTimestampCpuCyclesPerTick,globalFreq.lo,
                globalFreq.hi);
    }
}

/*
 *  ======== LogSync_writeSyncPointRaw ========
 */
Void LogSync_writeSyncPointRaw(const Types_Timestamp64 *cpuTS,
        const Types_Timestamp64 *globalTS, const Types_FreqHz *globalTickFreq)
{
    UInt32 serial;
    Types_FreqHz cpuFreq;

    serial = LogSync_module->serialNumber++;
    LogSync_CpuTimestampProxy_getFreq(&cpuFreq);

    xdc_runtime_ILogger_write8(ti_uia_runtime_LogSync_Module__loggerObj__C,
            UIASync_syncPointAfterHalt, ti_uia_runtime_LogSync_Module__id__C,
            serial, cpuTS->lo, cpuTS->hi, globalTS->lo, globalTS->hi,
            LogSync_cpuTimestampCyclesPerTick, cpuFreq.lo, cpuFreq.hi);

    xdc_runtime_ILogger_write4(ti_uia_runtime_LogSync_Module__loggerObj__C,
            UIASync_globalTimerFreq, ti_uia_runtime_LogSync_Module__id__C,
            serial, LogSync_globalTimestampCpuCyclesPerTick,
            globalTickFreq->lo, globalTickFreq->hi);
}

/*
 *  ======== LogSync_idleHook ========
 *  Hook function that can be called by SysBios when the Idle function.
 *  Logs a sync point event if required in order to enable multicore event correlation.
 *  Allows multicore event correlation to be re-established after the target
 *  has been halted and then resumed execution.  (e.g. after CIO operation or breakpoint)
 */
Void LogSync_idleHook(){
        if (LogSync_isSyncEventRequired()){
                LogSync_writeSyncPoint();
        }
}
/*
 *  ======== LogSync_timerHook ========
 *  Hook function that can be called periodically by SysBios to enable correlation
 *  of CPU trace, STM trace and software instrumentation events.
 */
Void LogSync_timerHook(UArg arg){
        LogSync_writeSyncPoint();
}
