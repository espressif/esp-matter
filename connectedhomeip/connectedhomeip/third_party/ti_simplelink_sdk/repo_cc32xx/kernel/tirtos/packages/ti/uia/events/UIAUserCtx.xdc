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
 * ======== UIAUserCtx.xdc ========
 */

import xdc.runtime.Types;
import xdc.runtime.Diags;
import ti.uia.events.IUIACtx;
import xdc.rov.ViewInfo;

/*!
 * UIA User Context Instrumentation
 *
 * The UIAUserCtx module provides context change events
 * and methods that allow tooling to identify user-defined
 * context switches and to enable context-aware filtering,
 * trace and analysis.
 *
 * It inherits IUIACtx, which defines a function pointer to
 * an isLoggingEnabled function which, if configured to point to
 * a function, will evaluate the function prior to logging the context
 * change event and will determine whether to log the event based on the
 * return value of the function.  If the function is not configured,
 * logging will be conditional upon ti_uia_runtime_CtxFilter_module->mIsLoggingEnabled.
 *
 *  The generation of UIAUserCtx events is also controlled by a module's diagnostics
 *  mask, which is described in details in `{@link xdc.runtime.Diags}`.
 * `UIAUserCtx` events are generated only when the Diags.ANALYSIS bit is set
 *  in the module's diagnostics mask.
 *
 *  The following configuration script demonstrates how the application might
 *  control the logging of ANALYSIS events embedded in the `Mod` module at configuration
 *  time. In this case, the configuration script arranges for the `Log`
 *  statements within modules to always generate ANALYSIS events.
 *  Without these configuration statements, no ANALYSIS events would be generated
 *  by any modules.
 *
 * @a(Examples)
 * Example 1: This is part of the XDC configuration file for the application:
 *
 *  @p(code)
 *  var LogCtxChg = xdc.useModule('ti.uia.runtime.LogCtxChg');
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var LoggerSys = xdc.useModule('xdc.runtime.LoggerSys');
 *  var Defaults = xdc.useModule('xdc.runtime.Defaults');
 *  var logger = LoggerSys.create();
 *
 *  Defaults.common$.diags_ANALYSIS = Diags.ALWAYS_ON;
 *  Defaults.common$.logger = logger;
 *  @p
 *
 *  @p(html)
 *  <hr />
 *  @p
 *
 *  Example 2: The following example configures a module to support logging
 *  of ANALYSIS events, but defers the actual activation and deactivation of the
 *  logging until runtime. See the `{@link Diags#setMask Diags_setMask()}`
 *  function for details on specifying the control string.
 *
 *  This is a part of the XDC configuration file for the application:
 *
 *  @p(code)
 *  var LogCtxChg = xdc.useModule('ti.uia.runtime.LogCtxChg');
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var Mod = xdc.useModule('my.pkg.Mod');
 *
 *  Mod.common$.diags_ANALYSIS = Diags.RUNTIME_OFF;
 *  @p
 *
 *  This is a part of the C code for the application:
 *
 *  @p(code)
 *  // turn on logging of ANALYSIS events in the module
 *  Diags_setMask("my.pkg.Mod+Z");
 *
 *  // turn off logging of ANALYSIS events in the module
 *  Diags_setMask("my.pkg.Mod-Z");
 *  @p
 */

@CustomHeader
module UIAUserCtx inherits IUIACtx {

    /*!
     *  @_nodoc
     *  ======== ModuleView ========
     */
    metaonly struct ModuleView {
        UInt mLastValue;
        UInt mEnableOnValue;
    }

    /*!
     *  @_nodoc
     *  ======== rovViewInfo ========
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [['Module',   {type: ViewInfo.MODULE,
                                    viewInitFxn: 'viewInitModule',
                                    structName: 'ModuleView'}
                      ]]
        });

    /*!
     *  ======== ctxChg ========
     *  User-defined Context Change event
     *
     *  Used to log the start of new state in the user-defined context
     *
     *  Note that this event should not be referenced directly by user code - it is used
     *  internally by the LogCtxChg_user function, which logs the previous user context Id automatically.
     *
     *  @a(Example)
     *   The following C code shows how to log a Context Change
     *   event that identifies a new user-specified context ID
     *
     *  @p(code)
     *  #include <ti/uia/runtime/LogCtxChg.h>
     *  ...
     *  LogCtxChg_user("New user context=0x%x",newUserContextId);
     *  ...
     *
     *  @p
     *  This event prints the Log call site (%$F) and a format string (%$S)
     *  which is recursively formatted with any addition arguments.
     *  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "User Ctx Change at Line 123 in appLoader.c [Prev. ctx = 0x1234] New user context=0x1235"
     *
     *  @param(fmt)   a constant string that describes the context change and provides a format specifier for newAppId
     *  @param(newUserContextId)   an integer which uniquely identifies the new user context
     */
    config xdc.runtime.Log.Event ctxChg = {
        mask: Diags.ANALYSIS,
        msg: "User Ctx Change at %$F [Prev. ctx = 0x%x] %$S"};

    /*!
     *  ======== metaEventUserCtxChg ========
     *  Metadata description of the User Context Change event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventUserCtxChg = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_CONTEXTCHANGE,
        displayText: "User Ctx Change",
        tooltipText: "User Context Change",
        numParameters: 5,
        paramInfo: [
        {   name: '__FILE__',
            dataDesc: DvtTypes.DvtDataDesc_FILENAMESTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: '__LINE__',
            dataDesc: DvtTypes.DvtDataDesc_LINENUM,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'Prev. User Context',
            dataDesc: DvtTypes.DvtDataDesc_STATEID,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'fmt',
            dataDesc: DvtTypes.DvtDataDesc_FMTSTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: 'New User Context',
            dataDesc: DvtTypes.DvtDataDesc_STATEID,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        }]

    };
    /*!
     * ======== getCtxId ========
     * Get the ID for the current user-defined context
     *
     *  @a(returns)
     *  returns the context ID logged by the last call to UIAUserCtx_logCtxChg.
     */
    @DirectCall
    UInt getCtxId();

    /*!
     * ======== getEnableOnValue ========
     * Get the EnableOn value
     *
     *  @a(returns) returns the context ID value that logging will be enabled for.
     */
    @DirectCall
    UInt getEnableOnValue();

    /*!
     * ======== setEnableOnValue ========
     * Set the EnableOn value
     *
     *  @param(value) the CtxId value that logging will be enabled for.
     */
    @DirectCall
    Void setEnableOnValue(UInt value);

    /*!
     * ======== isLoggingEnabled ========
     * returns true if the new context matches the value to enable logging with.
     *
     *  Default implementation of the IUIACtx_IsLoggingEnabledFxn for user context.
     *  To enable context-aware filtering, in the .cfg script assign
     *    UIAUserCtx_isLoggingEnabledFxn = '&UIAUserCtx_isLoggingEnabled'
     *  or assign your own implementation of this function.
     *
     *  @param(newUserCtx) the new user context
     *  @a(returns) true if logging is enabled
     */
     @DirectCall
     Bool isLoggingEnabled(UInt newUserCtx);

    /*!
     * ======== setOldValue =========
     * sets ti_uia_events_UIAUserCtx_gLastValue to the new value and returns the old value before it was updated.
     *
     * @param(newValue) the new value to save in the global variable
     * @a(return)       the original value of the global variable before it was updated.
     */
     @DirectCall
     UInt setOldValue(UInt newValue);

internal:

	struct Module_State {
        UInt mLastValue;
        UInt mEnableOnValue;
    };
}
