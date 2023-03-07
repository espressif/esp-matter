/*
 * Copyright (c) 2013, Texas Instruments Incorporated
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
 *  ======== LogCtxChg__epilogue.h ========
 *  Implementation of the Log_* macros
 *
 *  The implementation below relies on five symbols defined by every module
 *  header.  Each of these symbols is a reference to a constant defined in a
 *  separate compilation unit.  The values of these constants are as follows:
 *
 *      Module__MID     - the module's ID (see Text.xs)
 *      LogSnapshot__LOGGEROBJ  - the module's logger object
 *      LogSnapshot__LOGMEMORYFXN - the module's logger's write0 function
 *      LogSnapshot__LOGFXN1 - the module's logger's write1 function
 *      LogCtxChg__LOGFXN2 - the module's logger's write2 function
 *      LogCtxChg__LOGFXN4 - the module's logger's write4 function
 *      LogCtxChg__LOGFXN8 - the module's logger's write8 function
 *      LogCtxChg__LOGGERDEF  - 0 if the module has a logger, non-zero otherwise
 */

/*
 *  ======== ti_uia_runtime_Log_getMask ========
 */
#define ti_uia_runtime_Log_getMask(evt) ((evt) & 0x0000ffff)

/*
 *  ======== ti_uia_runtime_Log_getRope ========
 */
#define ti_uia_runtime_Log_getRope(evt) ((xdc_runtime_Types_RopeId)((evt) >> 16))

/*
 *  ======== ti_uia_runtime_Log_getEventId ========
 */
#define ti_uia_runtime_Log_getEventId(evt) ((xdc_runtime_Log_EventId)((evt) >>16))

/*
 *  ======== putCtxChg2 ========
 *  Unconditionally put the specified `Types` event along with file name, line number, fmt string and 2 args.
 */
#define ti_uia_runtime_LogCtxChg_put2(evt, mid, a1, a2) \
    ((LogCtxChg__LOGFXN2 != NULL) ? \
    (LogCtxChg__LOGFXN2(LogCtxChg__LOGOBJ, (evt), (mid), (IArg)(a1),(IArg)(a2))) : (void)0 )

/*
 *  ======== LogCtxChg_put8 ========
 *  Unconditionally put the specified `Types` event along with file name, line number, fmt string and 2 args.
 */
 #define ti_uia_runtime_LogCtxChg_put8(evt, mid, a1, a2, a3, a4, a5, a6, a7, a8) \
    (LogCtxChg__LOGFXN8(LogCtxChg__LOGOBJ, (evt), (mid), (IArg)(a1),(IArg)(a2),(IArg)(a3),(IArg)(a4)(IArg)(a5),(IArg)(a6),(IArg)(a7),(IArg)(a8)))

/* NOTE: write2 is used for fast swi and hwi logging, and does not inject */
/* FILE and LINE info or track context change values                      */
#define ti_uia_runtime_LogCtxChg_write2(evt,fmt,arg) \
   ((ti_uia_runtime_LogCtxChg_isLoggingEnabled(xdc_runtime_Diags_ANALYSIS)) ? \
        ti_uia_runtime_LogCtxChg_put2(evt, Module__MID, \
        (IArg)fmt, (IArg)arg) : (void)0 \
    )



/*
 *  ======== ti_uia_runtime_LogCtxChg_app ========
 *  Log a context change event that can be used to enable context-aware event
 *  filtering, context-aware profiling, etc.
 */
#define ti_uia_runtime_LogCtxChg_isAppCtxEnabled(newAppId) \
        ((ti_uia_events_UIAAppCtx_isLoggingEnabledFxn__C != NULL)? \
            (ti_uia_runtime_CtxFilter_setCtxEnabled__E(ti_uia_events_UIAAppCtx_isLoggingEnabledFxn__C(newAppId))):\
			(ti_uia_runtime_CtxFilter_isCtxEnabled__E()))

#define ti_uia_runtime_LogCtxChg_app(fmt, newAppId) \
        ((ti_uia_runtime_LogCtxChg_isLoggingEnabled(ti_uia_events_UIAAppCtx_ctxChg))? \
                ((ti_uia_runtime_LogCtxChg_isAppCtxEnabled(newAppId))? \
        (LogCtxChg__LOGFXN8(LogCtxChg__LOGOBJ, ti_uia_events_UIAAppCtx_ctxChg, Module__MID, \
        (IArg)__FILE__,(IArg)__LINE__, (IArg)ti_uia_events_UIAAppCtx_setOldValue(newAppId), (IArg)fmt,  \
         (IArg)newAppId,  0,0,0)) : \
         (void)((IArg)ti_uia_events_UIAAppCtx_setOldValue(newAppId))) : (Void)0 )


/*
 *  ======== ti_uia_runtime_LogCtxChg_channel ========
 *  Log a context change event that can be used to enable context-aware event
 *  filtering, context-aware profiling, etc.
 */
#define ti_uia_runtime_LogCtxChg_isChanCtxEnabled(newChanId) \
        ((ti_uia_events_UIAChanCtx_isLoggingEnabledFxn__C != NULL)? \
            (ti_uia_runtime_CtxFilter_setCtxEnabled__E(ti_uia_events_UIAChanCtx_isLoggingEnabledFxn__C(newChanId))):\
			(ti_uia_runtime_CtxFilter_isCtxEnabled__E()))

#define ti_uia_runtime_LogCtxChg_channel(fmt, newChanId) \
        ((ti_uia_runtime_LogCtxChg_isLoggingEnabled(ti_uia_events_UIAChanCtx_ctxChg))? \
        ((ti_uia_runtime_LogCtxChg_isChanCtxEnabled(newChanId))? \
        (LogCtxChg__LOGFXN8(LogCtxChg__LOGOBJ, ti_uia_events_UIAChanCtx_ctxChg, Module__MID,\
        (IArg)__FILE__,(IArg)__LINE__, (IArg)ti_uia_events_UIAChanCtx_setOldValue(newChanId), (IArg)fmt, \
         (IArg)newChanId,  0,0,0)) : \
         (void)((IArg)ti_uia_events_UIAChanCtx_setOldValue(newChanId))) : (Void)0 )

/*
 *  ======== ti_uia_runtime_LogCtxChg_thread ========
 *  Log a context change event that can be used to enable context-aware event
 *  filtering, context-aware profiling, etc.
 */
#define ti_uia_runtime_LogCtxChg_isThreadCtxEnabled(newThreadId) \
        ((ti_uia_events_UIAThreadCtx_isLoggingEnabledFxn__C != NULL)? \
            (ti_uia_runtime_CtxFilter_setCtxEnabled__E(ti_uia_events_UIAThreadCtx_isLoggingEnabledFxn__C(newThreadId))):\
			(ti_uia_runtime_CtxFilter_isCtxEnabled__E()))

#define ti_uia_runtime_LogCtxChg_thread(fmt, newThreadId) \
        ((ti_uia_runtime_LogCtxChg_isLoggingEnabled(ti_uia_events_UIAThreadCtx_ctxChg))? \
        ((ti_uia_runtime_LogCtxChg_isThreadCtxEnabled(newThreadId))? \
        (LogCtxChg__LOGFXN8(LogCtxChg__LOGOBJ, ti_uia_events_UIAThreadCtx_ctxChg, Module__MID,\
        (IArg)__FILE__,(IArg)__LINE__, (IArg)ti_uia_events_UIAThreadCtx_setOldValue(newThreadId), (IArg)fmt, \
         (IArg)newThreadId,  0,0,0)) : \
         (void)((IArg)ti_uia_events_UIAThreadCtx_setOldValue(newThreadId))) : (Void)0 )


/*
 *  ======== ti_uia_runtime_LogCtxChg_threadAndFunc ========
 *  Log a context change event that can be used to enable context-aware event
 *  filtering, context-aware profiling, etc.
 */

#define ti_uia_runtime_LogCtxChg_threadAndFunc(fmt, newThreadId,oldFunc,newFunc) \
        ((ti_uia_runtime_LogCtxChg_isLoggingEnabled(ti_uia_events_UIAThreadCtx_ctxChgWithFunc))? \
        ((ti_uia_runtime_LogCtxChg_isThreadCtxEnabled(newThreadId))? \
        (LogCtxChg__LOGFXN8(LogCtxChg__LOGOBJ, ti_uia_events_UIAThreadCtx_ctxChgWithFunc, Module__MID,\
        (IArg)__FILE__,(IArg)__LINE__, (IArg)ti_uia_events_UIAThreadCtx_setOldValue(newThreadId), (IArg)fmt, \
         (IArg)newThreadId, (UInt32) oldFunc,(UInt32)newFunc,0)) : \
         (void)((IArg)ti_uia_events_UIAThreadCtx_setOldValue(newThreadId))) : (Void)0 )

/*
 *  ======== ti_uia_runtime_LogCtxChg_frame ========
 *  Log a context change event that can be used to enable context-aware event
 *  filtering, context-aware profiling, etc.
 */
#define ti_uia_runtime_LogCtxChg_isFrameCtxEnabled(newFrameId) \
        ((ti_uia_events_UIAFrameCtx_isLoggingEnabledFxn__C != NULL)? \
            (ti_uia_runtime_CtxFilter_setCtxEnabled__E(ti_uia_events_UIAFrameCtx_isLoggingEnabledFxn__C(newFrameId))):\
			(ti_uia_runtime_CtxFilter_isCtxEnabled__E()))

#define ti_uia_runtime_LogCtxChg_frame(fmt, newFrameId) \
        ((ti_uia_runtime_LogCtxChg_isLoggingEnabled(ti_uia_events_UIAFrameCtx_ctxChg))? \
        ((ti_uia_runtime_LogCtxChg_isThreadCtxEnabled(newFrameId))? \
        (LogCtxChg__LOGFXN8(LogCtxChg__LOGOBJ, ti_uia_events_UIAFrameCtx_ctxChg, Module__MID,\
        (IArg)__FILE__,(IArg)__LINE__, (IArg)ti_uia_events_UIAFrameCtx_setOldValue(newFrameId), (IArg)fmt, \
         (IArg)newFrameId,  0,0,0)) : \
         (void)((IArg)ti_uia_events_UIAFrameCtx_setOldValue(newFrameId))) : (Void)0 )


/*
 *  ======== ti_uia_runtime_LogCtxChg_hwiStart ========
 *  Log a context change event that can be used to enable context-aware event
 *  filtering, context-aware profiling, etc.
 */
#define ti_uia_runtime_LogCtxChg_hwiStart(fmt, hwiId) \
    ((ti_uia_runtime_LogCtxChg_isLoggingEnabled(ti_uia_events_UIAHWICtx_start))? \
        ti_uia_runtime_LogCtxChg_write2(ti_uia_events_UIAHWICtx_start,fmt,hwiId) : (Void)0 )

/*
 *  ======== ti_uia_runtime_LogCtxChg_hwiStop ========
 *  Log a context change event that can be used to enable context-aware event
 *  filtering, context-aware profiling, etc.
 */
#define ti_uia_runtime_LogCtxChg_hwiStop(fmt, hwiId) \
    ((ti_uia_runtime_LogCtxChg_isLoggingEnabled(ti_uia_events_UIAHWICtx_stop))? \
        ti_uia_runtime_LogCtxChg_write2(ti_uia_events_UIAHWICtx_stop,fmt,hwiId) : (Void)0 )


/*
 *  ======== ti_uia_runtime_LogCtxChg_swiStart ========
 *  Log a context change event that can be used to enable context-aware event
 *  filtering, context-aware profiling, etc.
 */
 #define ti_uia_runtime_LogCtxChg_swiStart(fmt, swiId) \
    ((ti_uia_runtime_LogCtxChg_isLoggingEnabled(ti_uia_events_UIASWICtx_start))? \
    ti_uia_runtime_LogCtxChg_write2(ti_uia_events_UIASWICtx_start,fmt,swiId): (Void)0 )

/*
 *  ======== ti_uia_runtime_LogCtxChg_swiStop ========
 *  Log a context change event that can be used to enable context-aware event
 *  filtering, context-aware profiling, etc.
 */
#define ti_uia_runtime_LogCtxChg_swiStop(fmt, swiId) \
    ((ti_uia_runtime_LogCtxChg_isLoggingEnabled(ti_uia_events_UIASWICtx_stop))? \
        ti_uia_runtime_LogCtxChg_write2(ti_uia_events_UIASWICtx_stop,fmt,swiId) : (Void)0 )

/*
 *  ======== ti_uia_runtime_LogCtxChg_user ========
 *  Log a context change event that can be used to enable context-aware event
 *  filtering, context-aware profiling, etc.
 */
#define ti_uia_runtime_LogCtxChg_isUserCtxEnabled(newUserCtx) \
        ((ti_uia_events_UIAUserCtx_isLoggingEnabledFxn__C != NULL)? \
            (ti_uia_runtime_CtxFilter_setCtxEnabled__E(ti_uia_events_UIAUserCtx_isLoggingEnabledFxn__C(newUserCtx))):\
			(ti_uia_runtime_CtxFilter_isCtxEnabled__E()))

#define ti_uia_runtime_LogCtxChg_user(fmt, newUserCtx) \
        ((ti_uia_runtime_LogCtxChg_isLoggingEnabled(ti_uia_events_UIAUserCtx_ctxChg))? \
        ((ti_uia_runtime_LogCtxChg_isUserCtxEnabled(newUserCtx))? \
        (LogCtxChg__LOGFXN8(LogCtxChg__LOGOBJ, ti_uia_events_UIAUserCtx_ctxChg, Module__MID,\
        (IArg)__FILE__,(IArg)__LINE__, (IArg)ti_uia_events_UIAUserCtx_setOldValue(newUserCtx), (IArg)fmt,  \
         (IArg)newUserCtx,  0,0,0)) : \
         (void)((IArg)ti_uia_events_UIAUserCtx_setOldValue(newUserCtx))) : (Void)0 )
