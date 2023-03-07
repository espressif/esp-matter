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
 * ======== CtxFilter.xdc ========
 */

import xdc.runtime.Diags;
import xdc.rov.ViewInfo;

/*!
 * Context-aware Instrumentation Event Filtering infrastructure
 */
@CustomHeader
module CtxFilter {

    /*!
     *  @_nodoc
     *  ======== ModuleView ========
     */
    metaonly struct ModuleView {
        Bits16 mFlags;
        Bits16 mEnableMask;
	Bool mIsLoggingEnabled;
    }

    /*!
     *  @_nodoc
     *  ======== rovViewInfo ========
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [
                ['Module',   {type: ViewInfo.MODULE, viewInitFxn: 'viewInitModule', structName: 'ModuleView'}]
	    ]
        });

    /*! Type used to specify bits in the context mask. */
    typedef Bits16 Mask;

    const Mask ALWAYS_ENABLED = 0x0000; /*! Event logging is not qualified by context */

    const Mask CONTEXT_ENABLED = true; /*! Enable logging at the call site (subject to Diags.Mask) */
    const Mask CONTEXT_DISABLED = false; /*! Disable logging at the call site */

    /*
     * ======== isCtxEnabled ========
     * Read the mIsLoggingEnabled flag
     *
     * Returns true if the context filter has been configured
     * to enable logging in the current execution context.
     *
     * @see(xdc.runtime.Diags#Mask)
     */
    @DirectCall
    Bool isCtxEnabled();

    /*
     * ======== setCtxEnabled ========
     * Set the Context Filter's isLoggingEnabled flag.
     * Returns the new value to simplify use in macros.
     */
    @DirectCall
    Bool setCtxEnabled(Bool value);

    /*!
     *  ======== setContextFilterFlags ========
     *  sets the context filter flags
     *
     * @param(flags) bitfield of flags, one per context filter type
     */
    @DirectCall
    Void setContextFilterFlags(Bits16 flags);

    /*!
     * ======== isLoggingEnabledForAppCtx ========
     * optional function to enable context-aware filtering based on application context
     *
     * Configure the ti.uia.events.UIAAppCtx.IsLoggingEnabledFxn with the
     * address of this function in order to enable context-aware filtering based on
     * application context.  Alternatively, you can write your own isLoggingEnabled function
     * that provides additional filtering and logging capabilities (e.g. it can log
     * sync points whenever the context changes).
     *
     * @param(newAppCtx)  the new application context id that has just been switched to
     * @a(return)         true if logging is enabled for this user context.
     */
    @DirectCall
    Bool isLoggingEnabledForAppCtx(Int newAppCtx);

    /*!
     * ======== isLoggingEnabledForChanCtx ========
     * optional function to enable context-aware filtering based on channel context
     *
     * Configure the ti.uia.events.UIAChanCtx.IsLoggingEnabledFxn with the
     * address of this function in order to enable context-aware filtering based on
     * channel context.  Alternatively, you can write your own isLoggingEnabled function
     * that provides additional filtering and logging capabilities (e.g. it can log
     * sync points whenever the context changes).
     *
     * @param(newChanId)  the new channel ID that has just been switched to
     * @a(return)          true if logging is enabled for this user context.
     */
    @DirectCall
    Bool isLoggingEnabledForChanCtx(Int newChanId);


    /*!
     * ======== isLoggingEnabledForFrameCtx ========
     * optional function to enable context-aware filtering based on frame context
     *
     * Configure the ti.uia.events.UIAFrameCtx.IsLoggingEnabledFxn with the
     * address of this function in order to enable context-aware filtering based on
     * frame context.  Alternatively, you can write your own isLoggingEnabled function
     * that provides additional filtering and logging capabilities (e.g. it can log
     * sync points whenever the context changes).
     *
     * @param(newFrameId)  the new Frame ID that has just been switched to
     * @a(return)          true if logging is enabled for this user context.
     */
    @DirectCall
    Bool isLoggingEnabledForFrameCtx(Int newFrameId);


    /*!
     * ======== isLoggingEnabledForThreadCtx ========
     * optional function to enable context-aware filtering based on user context
     *
     * Configure the ti.uia.events.UIAThreadCtx.IsLoggingEnabledFxn with the
     * address of this function in order to enable context-aware filtering based on
     * thread context.  Alternatively, you can write your own isLoggingEnabled function
     * that provides additional filtering and logging capabilities (e.g. it can log
     * sync points whenever the context changes).
     *
     * @param(newThreadId)  the new thread ID that has just been switched to
     * @a(return)          true if logging is enabled for this user context.
     */
     @DirectCall
     Bool isLoggingEnabledForThreadCtx(Int newThreadId);

    /*!
     * ======== isLoggingEnabledForUserCtx ========
     * optional function to enable context-aware filtering based on user context
     *
     * Configure the ti.uia.events.UIAUserCtx.IsLoggingEnabledFxn with the
     * address of this function in order to enable context-aware filtering based on
     * user context.  Alternatively, you can write your own isLoggingEnabled function
     * that provides additional filtering and logging capabilities (e.g. it can log
     * sync points whenever the context changes).
     *
     * @param(newUserCtx)  the new user context that has just been switched to
     * @a(return)          true if logging is enabled for this user context.
     */
     @DirectCall
     Bool isLoggingEnabledForUserCtx(Int newUserCtx);

internal:

    struct Module_State {
        Bits16 mFlags;
        Bits16 mEnableMask;
        Bool mIsLoggingEnabled;
    };
}
