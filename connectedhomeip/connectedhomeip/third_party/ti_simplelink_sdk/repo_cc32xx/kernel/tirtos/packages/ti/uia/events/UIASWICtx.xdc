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
 * ======== UIASWICtx.xdc ========
 */

import xdc.runtime.Types;
import xdc.runtime.Diags;
import ti.uia.events.IUIACtx;

/*!
 * UIA Software Interrupt Context Instrumentation
 *
 * The UIASWICtx module defines context change events
 * and methods that allow tooling to identify software interrupt context
 * switches and to enable SWI-aware filtering, trace and
 * analysis.
 *
 * Note: in order to reduce overhead, UIASWICtx does not support context-aware filtering
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
 *
 *  // BIOS specific configuration:
 *  var Swi = xdc.useModule('ti.sysbios.hal.Swi');
 *  Swi.addHookSet({
 *    beginFxn: '&swiBeginHook',
 *    endFxn: '&swiEndHook'
 *   });
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

module UIASWICtx inherits IUIACtx {

    /*!
     *  ======== start ========
     *  Software Interrupt start event
     *
     *  Used to log the start of a Software Interrupt service routine
     *
     *  @a(Example)
     *   The following C code shows how to log a Context Change
     *   event that identifies the start of a SWI.  It implements
     *   a BIOS hook function.  Alternatively, the LogCtxChg_swiStart
     *   API can be called directly from the SWI service routine.
     *
     *  @p(code)
     *  #include <ti/uia/runtime/LogCtxChg.h>
     *   ...
     *  Void swiBeginHook(Swi_Handle handle) {
         *    LogCtxChg_swiStart("Swi_Handle:0x%x",handle);
     *  }
     *  @p
     *  This event has an associated format string (%$S)
     *  which is recursively formatted with any addition arguments.
     *  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "SWI start: Swi_Handle:0x80001200"
     *
     *  @param(fmt)   a constant string that describes the SWI and provides a format specifier for the SWI handle
     *  @param(handle)   an integer which uniquely identifies the SWI
     */
    config xdc.runtime.Log.Event start = {
        mask: Diags.ANALYSIS,
        msg: "SWI start: %$S"
    };
    /*!
     *  ======== metaEventHwiStart ========
     *  Metadata description of the HWI Start event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventSwiStart = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_START,
        displayText: "SWI Start",
        tooltipText: "SWI Start",
        numParameters: 2,
        paramInfo: [
        {   name: 'fmt',
            dataDesc: DvtTypes.DvtDataDesc_FMTSTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: 'SWI ID',
            dataDesc: DvtTypes.DvtDataDesc_SWIID,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        }]

    };
    /*!
     *  ======== stop ========
     *  Software Interrupt Stop event
     *
     *  Used to log the end of a Software Interrupt service routine
     *  @a(Example)
     *   The following C code shows how to log a Context Change
     *   event that identifies the end of a SWI.  It implements
     *   a BIOS hook function.  Alternatively, the LogCtxChg_swiStop
     *   API can be called directly from the SWI service routine.
     *
     *  @p(code)
     *  #include <ti/uia/runtime/LogCtxChg.h>
     *   ...
     *  Void swiEndHook(Swi_Handle handle) {
         *    LogCtxChg_swiStop("Swi_Handle:0x%x",handle);
     *  }
     *  @p
     *  This event has an associated format string (%$S)
     *  which is recursively formatted with any addition arguments.
     *  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "SWI stop: Swi_Handle:0x80001200"
     *
     *  @param(fmt)   a constant string that describes the HWI and provides a format specifier for the HWI handle
     *  @param(handle)   an integer which uniquely identifies the HWI
     */
    config xdc.runtime.Log.Event stop = {
        mask: Diags.ANALYSIS,
        msg: "SWI stop: %$S"
    };

    /*!
     *  ======== metaEventSwiStop ========
     *  Metadata description of the SWI Stop event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventSwiStop = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_STOP,
        displayText: "SWI Exit",
        tooltipText: "SWI Exit",
        numParameters: 2,
        paramInfo: [
        {   name: 'fmt',
            dataDesc: DvtTypes.DvtDataDesc_FMTSTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: 'SWI ID',
            dataDesc: DvtTypes.DvtDataDesc_SWIID,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        }]

    };

}
