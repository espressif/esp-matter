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
 *  ======== LogSnapshot__prologue.h ========
 */

#include <xdc/runtime/Error.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Main.h>
#include <xdc/runtime/Diags.h>
#include <ti/uia/runtime/LoggerTypes.h>
#include <ti/uia/runtime/CtxFilter.h>
#include <ti/uia/events/UIASnapshot.h>

#undef LogSnapshot__LOGDEF
#define LogSnapshot__LOGDEF ti_uia_runtime_LogSnapshot_loggerDefined
#undef LogSnapshot__LOGOBJ
#define LogSnapshot__LOGOBJ ti_uia_runtime_LogSnapshot_loggerObj__C
#undef LogSnapshot__LOGMEMORYFXN
#define LogSnapshot__LOGMEMORYFXN ti_uia_runtime_LogSnapshot_loggerMemoryRangeFxn__C

#undef LogSnapshot_CTXFILTERENABLED
#define LogSnapshot_CTXFILTERENABLED ti_uia_runtime_LogSnapshot_ctxFilterEnabled__C

#undef Module__MID
#define Module__MID xdc_runtime_Main_Module__id__C

#undef LOGSNAPSHOT_ISCONTEXTFILTERENABLED
#define LOGSNAPSHOT_ISCONTEXTFILTERENABLED (LogSnapshot__LOGDEF && LogSnapshot_CTXFILTERENABLED)

#undef ti_uia_runtime_LogSnapshot_isSnapshotLoggingEnabled
#define ti_uia_runtime_LogSnapshot_isSnapshotLoggingEnabled(evt) \
    (LogSnapshot__LOGDEF && xdc_runtime_Diags_query(evt))
