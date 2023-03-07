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
 * */

/*
 *  ======== LogCtxChg__prologue.h ========
 */

#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Main.h>
#include <xdc/runtime/Diags.h>
#include <ti/uia/runtime/LoggerTypes.h>
#include <ti/uia/runtime/CtxFilter.h>
#include <xdc/runtime/ILogger.h>
#include <ti/uia/events/UIAChanCtx.h>
#include <ti/uia/events/UIAFrameCtx.h>
#include <ti/uia/events/UIASWICtx.h>
#include <ti/uia/events/UIAHWICtx.h>
#include <ti/uia/events/UIAThreadCtx.h>
#include <ti/uia/events/UIAUserCtx.h>
#include <ti/uia/events/UIAAppCtx.h>

#undef LogCtxChg__LOGDEF
#define LogCtxChg__LOGDEF (ti_uia_runtime_LogCtxChg_loggerDefined__C && (ti_uia_runtime_LogCtxChg_loggerFxn8__C != NULL))
#undef LogCtxChg__LOGOBJ
#define LogCtxChg__LOGOBJ ti_uia_runtime_LogCtxChg_loggerObj__C
#undef LogCtxChg__LOGFXN2
#define LogCtxChg__LOGFXN2 ti_uia_runtime_LogCtxChg_loggerFxn2__C
#undef LogCtxChg__LOGFXN8
#define LogCtxChg__LOGFXN8 ti_uia_runtime_LogCtxChg_loggerFxn8__C

#undef LogCtxChg_CTXFILTERENABLED
#define LogCtxChg_CTXFILTERENABLED ti_uia_runtime_LogCtxChg_ctxFilterEnabled__C

#undef Module__MID
#define Module__MID xdc_runtime_Main_Module__id__C

#undef LogCtxChg_ISCONTEXTFILTERENABLED
#define LogCtxChg_ISCONTEXTFILTERENABLED (LogCtxChg__LOGDEF && LogCtxChg_CTXFILTERENABLED)

#undef ti_uia_runtime_LogCtxChg_isLoggingEnabled
#if LogCtxChg_ISCONTEXTFILTERENABLED
#define ti_uia_runtime_LogCtxChg_isLoggingEnabled(evt) \
    (LogCtxChg__LOGDEF && xdc_runtime_Diags_query(evt))
#else
#define ti_uia_runtime_LogCtxChg_isLoggingEnabled(evt) \
    (LogCtxChg__LOGDEF && xdc_runtime_Diags_query(evt))
#endif

#undef ti_uia_runtime_LogCtxChg_isLoggingEnabled
#if CONTEXTENABLED
#define ti_uia_runtime_LogCtxChg_isLoggingEnabled(evt) \
    (LogCtxChg__LOGDEF && xdc_runtime_Diags_query(evt))
#else
#define ti_uia_runtime_LogCtxChg_isLoggingEnabled(evt) \
    (LogCtxChg__LOGDEF && xdc_runtime_Diags_query(evt))
#endif
