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
 * ======== UIAThreadCtx.xdc ========
 */

import xdc.runtime.Types;
import xdc.runtime.Diags;
import ti.uia.events.IUIACtx;
import xdc.rov.ViewInfo;

/*!
 * UIA Thread Context Instrumentation
 *
 * The UIAThreadCtx module defines context change events
 * and methods that allow tooling to identify thread context
 * switches and to enable thread-aware filtering, trace and
 * analysis.
 *
 * It inherits IUIACtx, which defines a function pointer to
 * an isLoggingEnabled function which, if configured to point to
 * a function, will evaluate the function prior to logging the context
 * change event and will determine whether to log the event based on the
 * return value of the function.  If the function is not configured,
 * logging will be conditional upon ti_uia_runtime_CtxFilter_module->mIsLoggingEnabled.
 *
 * The generation of UIAThreadCtx events is also controlled by a module's diagnostics
 * mask, which is described in details in `{@link xdc.runtime.Diags}`.
 * 'UIAThreadCtx` events are generated only when the Diags.ANALYSIS bit is set
 * in the module's diagnostics mask.
 *
 * The following configuration script demonstrates how the application might
 * control the logging of ANALYSIS events embedded in the `Mod` module at configuration
 * time. In this case, the configuration script arranges for the `Log`
 * statements within modules to always generate ANALYSIS events.
 * Without these configuration statements, no ANALYSIS events would be generated
 * by any modules.
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
 * Example 2: The following example configures a module to support logging
 * of ANALYSIS events, but defers the actual activation and deactivation of the
 * logging until runtime. See the `{@link Diags#setMask Diags_setMask()}`
 * function for details on specifying the control string.
 *
 * This is a part of the XDC configuration file for the application:
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
module UIAThreadCtx inherits IUIACtx {

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
     *  threadSwitch Context Change event
     *
     *  Used to log a thread switch
     *  Note that the previous thread Id is logged automatically by the ti_uia_runtime_LogCtxChg_thread
     *  API.
     *  If ti_uia_events_UIAThreadCtx_isLoggingEnabledFxn is not NULL
     *  it is called and its return value determines whether logging is enabled or not.
     *
     *  @a(Example)
     *   The following C code shows how to log a Thread Context Change
     *   event that identifies a new thread using a BIOS task switch hook function
     *
     *  @p(code)
     *  #include <ti/uia/runtime/LogCtxChg.h>
     *  ...
     *  Void  tskSwitchHook(Task_Handle hOldTask,Task_Handle hNewTask){
     *          LogCtxChg_thread("thread: new = 0x%x",(Int)hNewTask);
     *  }
     *  @p
     *  This event prints the Log call site (%$F) and a format string (%$S)
     *  which is recursively formatted with any addition arguments.
     *  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "Thread Ctx Change at Line 123 in demo.c [Prev. thread ID=0x1234] New thread ID=0x1235"
     *
     *  @param(fmt)   a constant string that describes the context change and provides a format specifier for newThreadId
     *  @param(newThreadId)   an integer which uniquely identifies the new context
     */
    config xdc.runtime.Log.Event ctxChg = {
        mask: Diags.ANALYSIS,
        msg: "Thread Ctx Change at %$F [Prev. thread ID = 0x%x] %$S"};

    /*!
     *  ======== metaEventThreadCtxChg ========
     *  Metadata description of the Thread Context Change event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventThreadCtxChg = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_CONTEXTCHANGE,
        displayText: "Thread Ctx Change",
        tooltipText: "Thread Context Change",
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
        {   name: 'Prev. Thread ID',
            dataDesc: DvtTypes.DvtDataDesc_THREADID,
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
        {   name: 'New Thread ID',
            dataDesc: DvtTypes.DvtDataDesc_THREADID,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        }]

    };
    /*!
     *  ======== ctxChgWithFunc ========
     *  threadSwitch Context Change event with prempted function and resumed function addresses
     *
     *  Used to log a thread switch along with function info (preempted function and new function)
     *  Note that the previous thread Id is logged automatically by the ti_uia_runtime_LogCtxChg_threadAndFunc
     *  API.
     *  If ti_uia_events_UIAThreadCtx_isLoggingEnabledFxn is not NULL
     *  it is called and its return value determines whether logging is enabled or not.
     *
     *  @a(Example)
     *   The following C code shows how to log a Thread Context Change
     *   event that identifies a new thread, the preempted function and the preempting function.
     *
     *  @p(code)
     *  #include <ti/uia/runtime/LogCtxChg.h>
     *  ...
     *  LogCtxChg_threadAndFunc("New thread ID=0x%x, oldFunc=0x%x, newFunc=0x%x",(Int)hNewTask, (Int)&oldFunc,(Int)&newFunc);
     *
     *  @p
     *  This event prints the Log call site (%$F) and a format string (%$S)
     *  which is recursively formatted with any addition arguments.
     *  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "Thread CtxChgWithFunc at Line 123 in demo.c [Prev. thread ID=0x1234] New thread ID=0x1235, oldFunc=0x80001200, newFunc=0x80001080"
     *
     *  @param(fmt)   a constant string that describes the context change and provides a format specifier for newThreadId
     *  @param(newThreadId)   an integer which uniquely identifies the new context
     *  @param(oldFunc)   the address of the function that was preempted
     *  @param(newFunc)   the address of the function that is being resumed
     */
    config xdc.runtime.Log.Event ctxChgWithFunc = {
        mask: Diags.ANALYSIS,
        msg: "Thread CtxChgWithFunc at %$F [Prev. threadId = 0x%x] %$S"};

    /*!
     *  ======== metaEventThreadCtxChgWithFunc ========
     *  Metadata description of the Application Context Change event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventThreadCtxChgWithFunc = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_CONTEXTCHANGE,
        displayText: "Thread Ctx Change with function addresses",
        tooltipText: "Thread Context Change with function addresses",
        numParameters: 7,
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
        {   name: 'Prev. Thread ID',
            dataDesc: DvtTypes.DvtDataDesc_THREADID,
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
        {   name: 'New Frame ID',
            dataDesc: DvtTypes.DvtDataDesc_THREADID,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'Preempted Function Address',
            dataDesc: DvtTypes.DvtDataDesc_FUNCTIONADRS,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'Resumed Function Address',
            dataDesc: DvtTypes.DvtDataDesc_FUNCTIONADRS,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        }
        ]

    };
    /*!
     * ======== getCtxId ========
     * Get the ID for the current thread
     *
     *  @a(returns)
     *  returns the thread ID logged by the last call to UIAThreadCtx_logCtxChg.
     */
    @DirectCall
    UInt getCtxId();

    /*!
     * ======== getEnableOnValue ========
     * Get the EnableOn value
     *
     *  @a(returns) returns the thread ID value that logging will be enabled for.
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
     *    UIAThreadCtx_isLoggingEnabledFxn = '&UIAThreadCtx_isLoggingEnabled'
     *  or assign your own implementation of this function.
     *
     *  @param(newThreadId) the new thread ID
     *  @a(returns) true if logging is enabled
     */
     @DirectCall
     Bool isLoggingEnabled(UInt newThreadId);

    /*!
     * ======== setOldValue =========
     * sets ti_uia_events_UIAThreadCtx_gLastValue to the new value and returns the old value before it was updated.
     *
     * @param(newValue) the new value to save in the global variable
     * @a(return0       the original value of the global variable before it was updated.
     */
     @DirectCall
     UInt setOldValue(UInt newValue);

internal:

	struct Module_State {
        UInt mLastValue;
        UInt mEnableOnValue;
    };
}
