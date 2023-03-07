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
 * ======== UIARoundtrip.xdc ========
 */

import xdc.runtime.Diags;
import xdc.runtime.Types;
import ti.uia.events.DvtTypes;

/*!
 * UIA Roundtrip Events
 *
 *  The UIARoundtrip module defines events that allow
 *  tooling to analyze the absolute time elapsed between
 *  a start point event and a stop point event.  It differs
 *  from UIABenchmark in that UIABenchmark supports context-awareness
 *  (i.e. exclusive time elapsed) whereas UIARoundtrip is always
 *  inclusive (i.e. the time elapsed that DVT displays includes
 *  time spent in all other threads / tasks).
 *
 *  The generation of UIARoundtrip events is controlled by a module's diagnostics
 *  mask, which is described in details in `{@link xdc.runtime.Diags}`.
 * `UIARoundtrip` events are generated only when the Diags.ANALYSIS bit is set
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
 *  var UIARoundtrip = xdc.useModule('ti.uia.events.UIARoundtrip');
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
 *  var UIARoundtrip = xdc.useModule('ti.uia.events.UIARoundtrip');
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

module UIARoundtrip inherits IUIAEvent {

    /*!
     *  ======== start ========
     *  Roundtrip event used to log the start of an operation
     *
     * @a(Example)
     *   The following C code shows how to log a simple
     *   roundtrip 'start' event along with a user-specified
     *   format string describing the event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIARoundtrip.h>
     *  ...
     *  Log_write1(UIARoundtrip_start, "My roundtrip event");
     *  @p
     *  The following text will be displayed for the event:
     *  @p(code)
     *  Roundtrip_Start: My roundtrip event
     *  @p
     *
     *  @param(fmt)  a constant string that provides format specifiers for up to 6 additional parameters
     */
    config xdc.runtime.Log.Event start = {
        mask: Diags.ANALYSIS,
        msg: "Roundtrip_Start: %$S"};

    /*!
     *  ======== metaEventStart ========
     *  Metadata description of the start event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventStart = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_START,
        displayText: "Roundtrip_Start",
        tooltipText: "Marks the start of analysis",
        numParameters: 1,
        paramInfo: [
        {   name: 'fmt',
            dataDesc: DvtTypes.DvtDataDesc_FMTSTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        }]
    };

    /*!
     *  ======== stop ========
     *  Roundtrip event used to log the end of an operation
     *
     * @a(Example)
     *   The following C code shows how to log a simple
     *   roundtrip 'stop' event along with a user-specified
     *   format string describing the event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIARoundtrip.h>
     *  ...
     *  Log_write1(UIARoundtrip_stop, "My roundtrip event");
     *  @p
     *  The following text will be displayed for the event:
     *  @p(code)
     *  Roundtrip_Stop: My roundtrip event
     *  @p
     *  @param(fmt)        a constant string that provides format specifiers for up to 6 additional parameters
     */
    config xdc.runtime.Log.Event stop = {
        mask: Diags.ANALYSIS,
        msg: "Roundtrip_Stop: %$S"};

    /*!
     *  ======== metaEventStop ========
     *  Metadata description of the stop event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventStop = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_STOP,
        displayText: "Roundtrip_Stop",
        tooltipText: "Marks the end of analysis",
        numParameters: 1,
        paramInfo: [
        {   name: 'fmt',
            dataDesc: DvtTypes.DvtDataDesc_FMTSTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        }]
    };
    /*!
     *  ======== startInstance ========
     *  Roundtrip event used to log the start of an operation instance
     *
     *  Event parameter provides instance data to differentiate
     *  between multiple instances that can run in parallel.
     *
     *  @a(Example)
     *   The following C code shows how to log a roundtrip
     *   'startInstance' event along with a user-specified
     *   instance identifier and a format string describing the event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Gate.h>
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIARoundtrip.h>
     *  static volatile int gMyGlobalInstanceId = 0;
     *  ...
     *  IArg key;
     *  int localInstanceId;
     *
     * // protect pre-increment operation from race conditions
     *  key = Gate_enterSystem();
     *  localInstanceId = ++gMyGlobalInstanceId;
     *  Gate_leaveSystem(key);
     *
     *  Log_write2(UIARoundtrip_startInstance, "My roundtrip event: instanceId=%d",localInstanceId);
     *  ...
     *  Log_write2(UIARoundtrip_stopInstance, "My roundtrip event: instanceId=%d",localInstanceId);
     *  @p
     *  The following text will be displayed for the event:
     *  @p(code)
     *  Roundtrip_StartInstance: My roundtrip event: instanceId=1
     *  Roundtrip_StopInstance: My roundtrip event: instanceId=1
     *  @p
     *  @param(fmt)                 a constant string that provides format specifiers for up to 5 additional parameters
     *  @param(instanceId)          a unique instance ID that can be used to match Roundtrip start and stop events
     */
    config xdc.runtime.Log.Event startInstance = {
        mask: Diags.ANALYSIS,
        msg: "Roundtrip_StartInstance: %$S"
    };

    /*!
     *  ======== metaEventStartInstance ========
     *  Metadata description of the startInstance event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventStartInstance = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_START,
        displayText: "Roundtrip_StartInstance",
        tooltipText: "Marks the start of analysis for a module instance",
        numParameters: 2,
        paramInfo: [
        {   name: 'fmt',
            dataDesc: DvtTypes.DvtDataDesc_FMTSTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: 'InstanceID',
            dataDesc: DvtTypes.DvtDataDesc_INSTANCE,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        }
        ]
    };

    /*!
     *  ======== stopInstance ========
     *  Roundtrip event used to log the end of an operation instance
     *
     *  Event parameter provides instance data to differentiate
     *  between multiple instances that can run in parallel
     *  @a(Example)
     *   The following C code shows how to log a roundtrip
     *   'stopInstance' event along with a user-specified
     *   instance identifier and a format string describing the event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Gate.h>
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIARoundtrip.h>
     *  static volatile int gMyGlobalInstanceId = 0;
     *  ...
     *  IArg key;
     *  int localInstanceId;
     *
     * // protect pre-increment operation from race conditions
     *  key = Gate_enterSystem();
     *  localInstanceId = ++gMyGlobalInstanceId;
     *  Gate_leaveSystem(key);
     *
     *  Log_write2(UIARoundtrip_startInstance, "My roundtrip event: instanceId=%d",localInstanceId);
     *  ...
     *  Log_write2(UIARoundtrip_stopInstance, "My roundtrip event: instanceId=%d",localInstanceId);
     *  @p
     *  The following text will be displayed for the event:
     *  @p(code)
     *  Roundtrip_StartInstance: My roundtrip event: instanceId=1
     *  Roundtrip_StopInstance: My roundtrip event: instanceId=1
     *  @p
     *  @param(fmt)                 a constant string that provides format specifiers for up to 4 additional parameters
     *  @param(instanceId)          a unique instance ID that can be used to match Roundtrip start and stop events
     */
    config xdc.runtime.Log.Event stopInstance = {
        mask: Diags.ANALYSIS,
        msg: "Roundtrip_StopInstance: %$S"
    };

    /*!
     *  ======== metaEventStopInstance ========
     *  Metadata description of the stopInstance event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventStopInstance = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_STOP,
        displayText: "Roundtrip_StopInstance",
        tooltipText: "Marks the end of analysis for a module instance",
        numParameters: 2,
        paramInfo: [
        {   name: 'fmt',
            dataDesc: DvtTypes.DvtDataDesc_FMTSTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: 'InstanceID',
            dataDesc: DvtTypes.DvtDataDesc_INSTANCE,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        }
        ]
    };


    /*!
     *  ======== startInstanceWithAdrs ========
     *  Roundtrip event used to log the start of an operation instance
     *
     *  Event parameter provides instance data to differentiate
     *  between multiple instances that can run in parallel
     *
     * @a(Example)
     *   The following C code shows how to log a roundtrip
     *   'startInstanceWithAdrs' event along with a task handle as the
     *   instance identifier, the function address and a format string
     *   describing the event.
     *
     *  @p(code)
     *  #include <ti/sysbios/knl/Task.h>
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIARoundtrip.h>
     *  ...
     * Void myFunction(){
     *   Task_Handle hTsk = Task_selfMacro( );
     *
     *   Log_write3(UIARoundtrip_startInstanceWithAdrs, "My roundtrip event: task=0x%x, fnAdrs=0x%x",(IArg)hTsk,(IArg)&myFunc);
     *   ...
     *   Log_write3(UIARoundtrip_stopInstanceWithAdrs, "My roundtrip event: task=0x%x", fnAdrs=0x%x",(IArg)hTsk,(IArg)&myFunc);
     * }
     *  @p
     *  The following text will be displayed for the event:
     *  @p(code)
     *  Roundtrip_StartInstanceWithAdrs: My roundtrip event: task=0x893230, fnAdrs=0x820060
     *  Roundtrip_StopInstanceWithAdrs: My roundtrip event: task=0x893230, fnAdrs=0x820060
     *  @p
     *  @param(fmt)                 a constant string that provides format specifiers for up to 5 additional parameters
     *  @param(instanceId)          a unique instance ID that can be used to match benchmark start and stop events
     *  @param(functionAdrs)        the address of a function that can differentiate this pair of start and stop events from others

     */
    config xdc.runtime.Log.Event startInstanceWithAdrs = {
        mask: Diags.ANALYSIS,
        msg: "Roundtrip_StartInstanceWithAdrs: %$S"
    };

    /*!
     *  ======== metaEventStartInstanceWithAdrs ========
     *  Metadata description of the startInstanceWithAdrs event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventStartInstanceWithAdrs = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_START,
        displayText: "Roundtrip_StartInstanceWithAdrs",
        tooltipText: "Marks the start of analysis for a module instance",
        numParameters: 3,
        paramInfo: [
        {   name: 'fmt',
            dataDesc: DvtTypes.DvtDataDesc_FMTSTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: 'InstanceID',
            dataDesc: DvtTypes.DvtDataDesc_INSTANCE,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'FunctionAdrs',
            dataDesc: DvtTypes.DvtDataDesc_FUNCTIONADRS,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        }
        ]
    };





    /*!
     *  ======== stopInstanceWithAdrs ========
     *  Roundtrip event used to log the end of an operation instance
     *
     * @a(Example)
     *   The following C code shows how to log a roundtrip
     *   'stopInstanceWithAdrs' event along with a task handle as the
     *   instance identifier, the function address and a format string
     *   describing the event.
     *
     *  @p(code)
     *  #include <ti/sysbios/knl/Task.h>
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIARoundtrip.h>
     *  ...
     * Void myFunction(){
     *   Task_Handle hTsk = Task_selfMacro( );
     *
     *   Log_write3(UIARoundtrip_startInstanceWithAdrs, "My roundtrip event: task=0x%x, fnAdrs=0x%x",(IArg)hTsk,(IArg)&myFunc);
     *   ...
     *   Log_write3(UIARoundtrip_stopInstanceWithAdrs, "My roundtrip event: task=0x%x", fnAdrs=0x%x",(IArg)hTsk,(IArg)&myFunc);
     * }
     *  @p
     *  The following text will be displayed for the event:
     *  @p(code)
     *  Roundtrip_StartInstanceWithAdrs: My roundtrip event: task=0x893230, fnAdrs=0x820060
     *  Roundtrip_StopInstanceWithAdrs: My roundtrip event: task=0x893230, fnAdrs=0x820060
     *  @p
     *  @param(fmt)                 a constant string that provides format specifiers for up to 4 additional parameters
     *  @param(instanceId)          a unique instance ID that can be used to match Roundtrip start and stop events
     *  @param(functionAdrs)        the address of the function
     */
    config xdc.runtime.Log.Event stopInstanceWithAdrs = {
        mask: Diags.ANALYSIS,
        msg: "Roundtrip_StopInstanceWithAdrs: %$S"
    };

    /*!
     *  ======== metaEventStopInstanceWithAdrs ========
     *  Metadata description of the stopInstanceWithAdrs event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventStopInstanceWithAdrs = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_STOP,
        displayText: "Roundtrip_StopInstanceWithAdrs",
        tooltipText: "Marks the end of analysis for a module instance",
        numParameters: 3,
        paramInfo: [
        {   name: 'fmt',
            dataDesc: DvtTypes.DvtDataDesc_FMTSTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: 'Handle',
            dataDesc: DvtTypes.DvtDataDesc_INSTANCE,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'FunctionAdrs',
            dataDesc: DvtTypes.DvtDataDesc_FUNCTIONADRS,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        }
        ]
    };


    /*!
     *  ======== startInstanceWithStr ========
     *  Roundtrip event used to log the start of an operation instance
     *
     *  Event parameter provides instance data to differentiate
     *  between multiple instances that can run in parallel
     * @a(Example)
     *   The following C code shows how to log a roundtrip
     *   'startInstanceWithStr' event along with a unique instance identifier
     *   and a string reference used only by the pair of start / stop events.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIARoundtrip.h>
     *  ...
     * Void packetHdlr(Int packetId){
     *
     *   Log_write3(UIARoundtrip_startInstanceWithStr, "My roundtrip event: packetId=0x%x",packetId,(IArg)"(routing)");
     *   ...
     *   Log_write3(UIARoundtrip_stopInstanceWithStr, "My roundtrip event: packetId=0x%x",packetId,(IArg)"(routing)");
     * }
     *  @p
     *  The following text will be displayed for the event:
     *  @p(code)
     *  Roundtrip_StartInstanceWithStr: My roundtrip event: packetId=0x3bc3 (routing)
     *  Roundtrip_StopInstanceWithStr: My roundtrip event: packetId=0x3bc3 (routing)
     *  @p
     *  Event parameter provides instance data to differentiate
     *  between multiple instances that can run in parallel
     *  @param(fmt)                 a constant string that provides format specifiers for up to 4 additional parameters
     *  @param(instanceId)          a unique instance ID that can be used to match Roundtrip start and stop events
     *  @param(str)                 a constant string reference
     */
    config xdc.runtime.Log.Event startInstanceWithStr = {
        mask: Diags.ANALYSIS,
        msg: "Roundtrip_StartInstanceWithStr: %$S"
    };

    /*!
     *  ======== metaEventStartInstanceWithStr ========
     *  Metadata description of the startInstance event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventStartInstanceWithStr = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_START,
        displayText: "Roundtrip_StartInstanceWithStr",
        tooltipText: "Marks the start of analysis for a module instance",
        numParameters: 3,
        paramInfo: [
        {   name: 'fmt',
            dataDesc: DvtTypes.DvtDataDesc_FMTSTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: 'InstanceID',
            dataDesc: DvtTypes.DvtDataDesc_INSTANCE,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'FunctionAdrs',
            dataDesc: DvtTypes.DvtDataDesc_FUNCTIONADRS,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        }
        ]
    };


    /*!
     *  ======== stopInstanceWithStr ========
     *  Roundtrip event used to log the end of an operation instance
     *
     *  Event parameter provides instance data to differentiate
     *  between multiple instances that can run in parallel
     * @a(Example)
     *   The following C code shows how to log a roundtrip
     *   'stopInstanceWithStr' event along with a unique instance identifier
     *   and a string reference used only by the pair of start / stop events.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIARoundtrip.h>
     *  ...
     * Void packetHdlr(Int packetId){
     *
     *   Log_write3(UIARoundtrip_startInstanceWithStr, "My roundtrip event: packetId=0x%x",packetId,(IArg)"(routing)");
     *   ...
     *   Log_write3(UIARoundtrip_stopInstanceWithStr, "My roundtrip event: packetId=0x%x",packetId,(IArg)"(routing)");
     * }
     *  @p
     *  The following text will be displayed for the event:
     *  @p(code)
     *  Roundtrip_StartInstanceWithStr: My roundtrip event: packetId=0x3bc3 (routing)
     *  Roundtrip_StopInstanceWithStr: My roundtrip event: packetId=0x3bc3 (routing)
     *  @p
     *  Event parameter provides instance data to differentiate
     *  between multiple instances that can run in parallel
     *  @param(fmt)                 a constant string that provides format specifiers for up to 4 additional parameters
     *  @param(instanceId)          a unique instance ID that can be used to match Roundtrip start and stop events
     *  @param(str)                 a constant string reference
     */
    config xdc.runtime.Log.Event stopInstanceWithStr = {
        mask: Diags.ANALYSIS,
        msg: "Roundtrip_StopInstanceWithStr: %$S"
    };

    /*!
     *  ======== metaEventStopInstance ========
     *  Metadata description of the stopInstance event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventStopInstanceWithStr = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_STOP,
        displayText: "Roundtrip_StopInstanceWithStr",
        tooltipText: "Marks the end of analysis for a module instance",
        numParameters: 3,
        paramInfo: [
        {   name: 'fmt',
            dataDesc: DvtTypes.DvtDataDesc_FMTSTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: 'InstanceID',
            dataDesc: DvtTypes.DvtDataDesc_INSTANCE,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'String',
            dataDesc: DvtTypes.DvtDataDesc_STRINGADRS,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        }
        ]
    };

}
