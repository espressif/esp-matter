/*
 * Copyright (c) 2012-2018, Texas Instruments Incorporated
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
 * */

/*
 *  ======== LogSnapshot.c ========
 */
#include <xdc/std.h>

#include <xdc/runtime/System.h>
#include <xdc/runtime/Text.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Gate.h>
#include <ti/uia/runtime/IUIATraceSyncProvider.h>
#include "package/internal/LogSnapshot.xdc.h"

volatile UInt32 ti_uia_runtime_LogSnapshot_gLastUsedSnapshotId = 0;

/*
 * ======== getSnapshotId ========
 * returns a unique ID to use to group a set of snapshot event logs together
 * and, if injectIntoTraceFxn is not null, injects it into the trace stream
 * in order to support trace correlation with the snapshot data.
 */
UArg LogSnapshot_getSnapshotId() {
    IArg key;
    UInt16 newSnapshotId;

    key = Gate_enterSystem();

    newSnapshotId = (UInt16)((UInt32)LogSnapshot_SNAPSHOTID_MASK & ++ti_uia_runtime_LogSnapshot_gLastUsedSnapshotId);
    if (newSnapshotId == 0) {
        ti_uia_runtime_LogSnapshot_gLastUsedSnapshotId = 1;
        newSnapshotId = 1;
    }
    if (LogSnapshot_injectIntoTraceFxn != NULL){
        LogSnapshot_injectIntoTraceFxn(newSnapshotId,
                IUIATraceSyncProvider_ContextType_Snapshot);
    }
    Gate_leaveSystem(key);
    return((UArg)newSnapshotId);
}

/*
 *  ======== doPrint ========
 */
Void LogSnapshot_doPrint(LogSnapshot_EventRec *er) {
}
