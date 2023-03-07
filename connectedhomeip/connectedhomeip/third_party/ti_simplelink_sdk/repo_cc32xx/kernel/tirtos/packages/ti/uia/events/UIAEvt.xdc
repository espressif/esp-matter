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
 * ======== UIAEvt.xdc ========
 */
import xdc.runtime.Diags;

/*!
 * UIA Standard Events
 * @p
 * The UIAEvt module defines events that allow
 * tooling to display event information
 * and filter events based on their priority.
 * @p
 * The events in this module have one of the following event priority levels:
 *  WARNING: used to indicate an unexpected or problematic situation such as when a resource
 *  becomes dangerously low
 *  INFO: used to indicate something of interest or of use in understanding the
 *  current state of the system or behaviour of the software
 *  DETAIL: used to indicate additional information that may be of interest
 *  in troubleshooting problems or improving the software
 *@p
 * For each priority level, two predefined event codes
 * are provided: one for logging a single event code,
 * and one for logging an event code along with a
 * reference to a constant formatting string that can
 * be used to format the text displayed for the event.
 * The formatting string allows additional arguments
 * to be displayed along with the event code when the
 * event is rendered as text (e.g. by DVT).
 *@p
 * The following special formatting specifiers may be used in the
 * msg field of an event's config specification:
 * @p
 * %$S - a string parameter that can provide additional formatting specifiers
 *       Note that $S use in strings passed in as a paramter is not supported.
 *@p
 * %$F - a specifier for a string parameter containing the file name (__FILE__) and
 *       an integer parameter containing the line number (__LINE__).
 *@p
 *  The generation of UIAEvt events is controlled by a module's diagnostics
 *  mask, which is described in details in `{@link xdc.runtime.Diags}`.
 * `UIAEvt` warning events are generated only when the Diags.STATUS bit is set
 *  in the module's diagnostics mask.  The Diags.STATUS bit is set to ALWAYS_ON
 *  by default.  'UIAEvt' info and detail events are generated only when the
 *  Diags.INFO bit is set in the module's diagnostics mask.
 *
 *  The following configuration script demonstrates how to enable use of
 *  UIAEvt events within an application.  Since the Diags.STATUS bits are set
 *  to ALWAYS_ON by default, no explicit code is required to enable the
 *  Diags Masks for these events.  The Diags.INFO bitmust be explicitly set
 *  in order to enable info and detail level events.
 *
 *  This is part of the XDC configuration file for the application:
 *
 *  @p(code)
 *  var UIAEvt = xdc.useModule('ti.uia.events.UIAEvt');
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var LoggerSys = xdc.useModule('xdc.runtime.LoggerSys');
 *  var Defaults = xdc.useModule('xdc.runtime.Defaults');
 *  var logger = LoggerSys.create();
 *
 *  Defaults.common$.logger = logger;
 *  Defaults.common$.diags_INFO = Diags.ALWAYS_ON;
 *  @p
 *
 *  @p(html)
 *  <hr />
 *  @p
 *
 *  Example 2: The following example configures a module to support logging
 *  of STATUS events and INFO events, but defers the actual activation and deactivation of the
 *  logging until runtime. See the `{@link Diags#setMask Diags_setMask()}`
 *  function for details on specifying the control string.
 *
 *  This is a part of the XDC configuration file for the application:
 *
 *  @p(code)
 *  var UIAEvt = xdc.useModule('ti.uia.events.UIAEvt');
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var Mod = xdc.useModule('my.pkg.Mod');
 *
 *  Mod.common$.diags_STATUS = Diags.RUNTIME_OFF;
 *  Mod.common$.diags_INFO = Diags.RUNTIME_OFF;
 *  @p
 *
 *  This is a part of the C code for the application:
 *
 *  @p(code)
 *  // turn on logging of STATUS events (S) and INFO events (F)
 *  // in the module
 *  Diags_setMask("my.pkg.Mod+SF");
 *
 *  // turn off logging of STATUS events and INFO events in the module
 *  Diags_setMask("my.pkg.Mod-SF");
 *  @p
 */
 module UIAEvt inherits IUIAEvent {

    /*!
     *  ======== warning ========
     *  Event to use to log a Warning Event Code
     *
     *  @a(Example)
     *   The following C code shows how to log a legacy warning code as a UIA event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAEvt.h>
     *  ...
     *  Int myWarningCode = 0xBAD;
     *  ...
     *  Log_write1(UIAEvt_warning, myWarningCode);
     *  ...
     *  @p
     *  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "WARNING: EventCode:0xBAD."
     *  @p
     *  @param(eventCode)  integer that identifies the type of warning
     *  @param(fmt)        a constant string that provides format specifiers for up to 6 additional parameters
     *
     *  @see #warningWithStr
     */
    config xdc.runtime.Log.Event warning = {
            mask: Diags.STATUS,
            level: Diags.WARNING,
            msg: "WARNING: EventCode:0x%x"
    };

    /*!
     *  ======== warningWithStr ========
     *  Event to use to log a Warning Event Code and fmt string
     *
     *  @a(Example)
     *  The following C code shows how to log a legacy warning code and string as a UIA event.
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAEvt.h>
     *  ...
     *  Int myWarningCode = 0xBAD;
     *  String myWarningStr = "Legacy Warning String for warning 0xBAD";
     *  ...
     *  Log_write2(UIAEvt_warning,myWarningCode,(IArg)myWarningStr);
     *  ...
     *  @p
     *  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "WARNING: EventCode:0xBAD. Legacy Warning String for warning 0xBAD"
     *  @p
     *  @param(eventCode)  integer that identifies the type of warning event
     *  @param(fmt)        a constant string that provides format specifiers for up to 6 additional parameters
     *
     *  @see #warning
     *
     */
    config xdc.runtime.Log.Event warningWithStr = {
            mask: Diags.STATUS,
            level: Diags.WARNING,
            msg: "WARNING: EventCode:0x%x. %$S"
    };

    /*!
     *  ======== info ========
     *  Event to use to log an Informational Event Code
     *
     *  @a(Example)
     *   The following C code shows how to log an informational event code as a UIA event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAEvt.h>
     *  ...
     *  Int myInfoCode = 0xC0DE;
     *  ...
     *  Log_write1(UIAEvt_info, myInfoCode);
     *  ...
     *  @p
     *  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "INFO: EventCode:0xC0DE."
     *  @p
     *  @param(eventCode)          integer that identifies the type of info event
     *
     *  @see #infoWithStr
     *
     */
    config xdc.runtime.Log.Event info = {
            mask: Diags.INFO,
            msg: "INFO: EventCode: 0x%x"
    };

    /*!
     *  ======== infoWithStr ========
     *  Event to use to log a Informational Event Code and format string
     *
     *  @a(Example)
     *   The following C code shows how to log an informational event code
     *   and format string as a UIA event.  It also shows how additional parameters
     *   can be logged along with the event and format string.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAEvt.h>
     *  ...
     *  Int myInfoCode = 0xC0DE;
     *  Int anAdditionalParam = 0x6543;
     *  ...
     *  Log_write3(UIAEvt_infoWithStr, myInfoCode,(IArg)"Descriptive text. anAdditionalParam=0x%x.",anAdditionalParam);
     *  ...
     *  @p
     *  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "INFO: EventCode:0xC0DE. Some descriptive text.  anAdditionalParam=0x6543."
     *  @p
     *  @param(eventCode)  integer that identifies the specific info event being logged
     *  @param(fmt)        a constant string that provides format specifiers for up to 6 additional parameters
     *
     *  @see #info
     */
    config xdc.runtime.Log.Event infoWithStr = {
            mask: Diags.INFO,
            msg: "INFO: EventCode:0x%x.  %$S"
    };

    /*!
     *  ======== detail ========
     *  Event to use to log a Detail-level Event Code
     *
     *  @a(Example)
     *   The following C code shows how to log a detail-level event code as a UIA event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAEvt.h>
     *  ...
     *  Int myEventCode = 0xE1;
     *  ...
     *  Log_write1(UIAEvt_detail, myEventCode);
     *  ...
     *  @p
     *  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "DETAIL: EventCode:0xE1."
     *  @p
     *  @param(eventCode)  integer that identifies the specific detail event being logged
     *
     *  @see #detailWithStr
     */
    config xdc.runtime.Log.Event detail = {
            mask: Diags.INFO,
            level: Diags.LEVEL4,
            msg: "DETAIL: EventCode:0x%x"
    };

    /*!
     *  ======== detailWithStr ========
     *  Event to use to log a Detail-level Event Code and fmt string
     *
     *  @a(Example)
     *   The following C code shows how to log a detail-level event code
     *   and format string as a UIA event.  It also shows how additional parameters
     *   can be logged along with the event and format string.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAEvt.h>
     *  ...
     *  Int myEventCode = 0xE1;
     *  Int anAdditionalParam = 0x6543;
     *  ...
     *  Log_write3(UIAEvt_detailWithStr, myEventCode,(IArg)"Descriptive text. anAdditionalParam=0x%x.",anAdditionalParam);
     *  ...
     *  @p
     *  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "DETAIL: EventCode:0xE1. Some descriptive text.  anAdditionalParam=0x6543."
     *  @p
     *  @param(eventCode)  integer that identifies the specific detail event being logged
     *  @param(fmt)        a constant string that provides format specifiers for up to 6 additional parameters
     *
     *  @see #detail
     */
    config xdc.runtime.Log.Event detailWithStr = {
            mask: Diags.INFO ,
            level: Diags.LEVEL4,
            msg: "DETAIL: EventCode:0x%x.  %$S"
    };

    /*!
     *  ======== intWithKey ========
     *  Event to use to log values to be analyzed as Statistics and / or Graphs
     *
     *  @a(Example)
     *  Example 1:
     *   The following C code shows how to log an intWithKey event code
     *   that logs a value, a format string that defines the key, and
     *   parameters for use within the key format string.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAEvt.h>
     *  ...
     *  Int myValue = 1001;
     *  Int myInstanceId = 0x6543;
     *  ...
     *  Log_write5(UIAEvt_intWithKey, myValue,0,0,(IArg)"InstanceId=0x%x.",myInstanceId);
     *  ...
     *  @p
     *  The following text is an example of what will be displayed for the event:
     *  @p(code)
     *  "VALUE=1001 (AuxData=0,0) Key: InstanceId=0x6543."
         *
     *  @a(Example)
     *  Example 2:
     *   The following C code shows how to log an intWithKey event code
     *   that logs a value, a format string that defines the key, and
     *   parameters for use within the key format string, including
     *   the file name and line of code that the event was logged at.
     *   This example uses a special format specifier, %$F, which
     *   is used to format two parameters (__FILE__ and __LINE__)
     *   in a way that tools will be able to display the line of code
     *   that the event was logged from in a source code editor
     *   when the user clicks on the event.
     *
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAEvt.h>
     *  ...
     *  Int myValue = 1001;
     *  Int myInstanceId = 9876;
     *  ...
     *  Log_write7(UIAEvt_intWithKey, myValue,0,0,(IArg)"InstanceId=%d, at %$F.",
     *                    myInstanceId,(IArg)__FILE__,(IArg)__LINE__);
     *  ...
     *  // If you wish to log only the line number as a key, use the following:
     *  Log_write6(UIAEvt_intWithKey, myValue+1,0,0,(IArg)"InstanceId=%d, at line %d.",
     *                    myInstanceId,(IArg)__LINE__);
     *  ...
     *  // If you wish to log only the file name as a key and the line number
     *  // as auxiliary data which is logged along with the event, use the following:
     *  Log_write6(UIAEvt_intWithKey, myValue+2,(IArg)__LINE__,0,(IArg)"InstanceId=%d, in file [%s].",
     *                    myInstanceId,(IArg)__FILE__);
     *  @p
     *  The following text is an example of what will be displayed for the event,
     *  assuming it was logged from a file named demo.c at line 1234:
     *  @p(code)
     *  "VALUE=1001 (AuxData=0,0) Key: InstanceId=9876, at [../demo.c:1234] ."
     *  "VALUE=1002 (AuxData=0,0) Key: InstanceId=9876, at line 1234."
     *  "VALUE=1003 (AuxData=1234,0) Key: InstanceId=9876, in file [../demo.c]."
     *  @p
     *
     *  @param(value)      integer value that is to be analyzed
     *  @param(auxData1)   auxiliary data that is to be displayed along with the event (use 0 if none)
     *  @param(auxData2)   auxiliary data that is to be displayed along with the event (use 0 if none)
     *  @param(key)        a constant string that provides format specifiers for up to 4 key entries
     */
    config xdc.runtime.Log.Event intWithKey = {
                mask: Diags.ANALYSIS,
                msg: "VALUE=%d (AuxData=%d, %d) Key:%$S"
    };

    /*!
     *  ======== metaEventStart ========
     *  Metadata description of the start event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventIntWithKey = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_STATISTIC,
        displayText: "intWithKey",
        tooltipText: "Value with a key string",
        numParameters: 4,
        paramInfo: [
        {   name: 'value',
            dataDesc: DvtTypes.DvtDataDesc_VALUE,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'aux1',
            dataDesc: DvtTypes.DvtDataDesc_VALUE,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'aux2',
            dataDesc: DvtTypes.DvtDataDesc_VALUE,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'key',
            dataDesc: DvtTypes.DvtDataDesc_FMTSTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        }
        ]
    };

}
