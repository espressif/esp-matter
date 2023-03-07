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
 * ======== UIASnapshot.xdc ========
 */

import xdc.runtime.Diags;
import xdc.runtime.Types;
import ti.uia.events.DvtTypes;

/*!
 * UIA Snapshot Events
 *
 * The UIASnapshot module defines events that allow
 * collection of dynamic information from the heap
 * such as memory ranges, strings, dynamically assigned names, etc.
 * Snapshot events can be aggregated together using a common snapshot ID
 * as an event parameter in order to build up a multi-event description of the
 * target state.  They are intended for use solely with the methods provided by
 * the {@link ti.uia.runtime.LogSnapshot ti.uia.runtime.LogSnapshot} module.
 *
 *  The generation of UIASnapshot events is controlled by a module's diagnostics
 *  mask, which is described in details in `{@link xdc.runtime.Diags}`.
 * `UIASnapshot` events are generated only when the Diags.ANALYSIS bit is set
 *  in the module's diagnostics mask.
 *
 * The following special formatting specifiers are used in the definitions of the
 * msg fields of the UIASnapshot events:
 * %$S - a string parameter that can provide additional formatting specifiers
 *       Note that $S use in strings passed in as a paramter is not supported.
 *@p
 * %$F - a specifier for a string parameter containing the file name (__FILE__) and
 *       an integer parameter containing the line number (__LINE__).
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
 * (Note that the UIASnapshot module is automatically included by the
 * LogSnapshot.xs script, and so does not need to be referenced in the
 * application's .cfg file)
 *  @p(code)
 *  var LogSnapshot = xdc.useModule('ti.uia.runtime.LogSnapshot');
 *  var LoggerCircBuf = xdc.useModule('ti.uia.runtime.LoggerCircBuf');
 *  var LoggerCircBufParams = new LoggerCircBuf.Params;
 *  // set the logger buffer size in bytes
 *  LoggerCircBufParams.transferBufSize = 32768;
 *  var logger = LoggerCircBuf.create(LoggerCircBufParams);
 *
 *  // Configure all modules to always log Analysis events, including
 *  // UIASnapshot events
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var Defaults = xdc.useModule('xdc.runtime.Defaults');
 *  Defaults.common$.diags_ANALYSIS = Diags.ALWAYS_ON;
 *  Defaults.common$.logger = logger;
 *
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
 *  This is part of the XDC configuration file for the application:
 *  @p(code)
 *  var LogSnapshot = xdc.useModule('ti.uia.runtime.LogSnapshot');
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
module UIASnapshot inherits IUIAEvent {

    /*!
     *  ======== memoryRange ========
     *  Analysis event posted when a memoryRange snapshot is logged.
     *
     *  This event is used internally by the
     *  {@link ti.uia.runtime.LogSnapshot#writeMemoryBlock LogSnapshot.writeMemoryBlock}
     *  API.
     *
     * @a(Examples)
     * Example: The following C code shows how to log a snapshot event to
     *   capture a block of memory.
     *
     *  @p(code)
     *  #include <ti/uia/runtime/LogSnapshot.h>
     *  ...
     *  UInt32* pIntArray = (UInt32 *)malloc(sizeof(UInt32) * 200);
     *  ...
     *  LogSnapshot_writeMemoryBlock(0,"pIntArray ptr=0x%x, numBytes=%d",(UInt32)pIntArray,200);
     *  ...
     *  @p
     *  This event prints the Log call site (%$F) and a format string (%$S)
     *  which describes what information the event is logging.
     *  The following text will be displayed for the event, if it was logged
     *  from file demo.c at line 1234 and all 200 bytes were logged in the
     *  same event.
     *  @p(code)
     *  Memory Snapshot at [demo.c:1234] [snapshotID=0,adrs=0x80002000,
     *    numMAUsDataInEvent=200,numMAUsDataInRecord=200] ptr=0x80002000, numBytes=200
     *  @p
     *  If the 200 bytes were spread across multiple events,
     *  the numMAUsDataInRecord would indicate how many bytes were in the
     *  memory block, and numMAUsDataInEvent would indicate how many bytes
     *  were stored in that particular event.
     *  @p
     *  @param(__FILE__)   The file that the LogSnapshot call site was in (used by %$F)
     *  @param(__LINE__)   The line of code of the LogSnapshot call site (used by %$F)
     *  @param(snapshotID) ID used to identify snapshot events taken at the same
     *                     time. Set to 0 for first in series, set rest to return
     *                     value of LogSnapshot API.
     *  @param(startAdrs)  the start address of the range of memory
     *  @param(numMAUsDataInEvent) the number of MAUs of data payload for this event
     *  @param(numMAUsDataInRecord) the total number of MAUs of data payload for the
     *                     multi-event data record
     *  @param(fmt)        a constant string that provides a user-readable description
     *                     of what information the event is capturing
     */
    config xdc.runtime.Log.Event memoryRange = {
        mask: Diags.ANALYSIS,
        msg: "Memory Snapshot at %$F% [snapshotID=%d,adrs=0x%x,numMAUsDataInEvent=%hd,numMAUsDataInRecord=%hd] %$S"
    };

    /*!
     *  ======== metaEventMemoryRange ========
     *  Metadata description of the memoryRange event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventMemoryRange = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_MEMORYSNAPSHOT,
        displayText: "Memory Snapshot",
        tooltipText: "Memory Snapshot",
        numParameters: 8,
        paramInfo: [
        {   name: 'filename',
            dataDesc: DvtTypes.DvtDataDesc_FILENAMESTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: 'linenum',
            dataDesc: DvtTypes.DvtDataDesc_LINENUM,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'snapshotID',
            dataDesc: DvtTypes.DvtDataDesc_SNAPSHOTID,
            dataTypeName: 'UInt32',
            units: 'none',
            isHidden: false
        },
        {   name: 'startAdrs',
            dataDesc: DvtTypes.DvtDataDesc_DATAADRS,
            dataTypeName: 'Ptr',
            units: 'none',
            isHidden: false
        },
        {   name: 'numMAUsDataInEvent',
            dataDesc: DvtTypes.DvtDataDesc_LENGTHINMAUS,
            dataTypeName: 'Int16',
            units: 'none',
            isHidden: false,
            lsb: 16
        },
        {   name: 'numMAUsDataInRecord',
            dataDesc: DvtTypes.DvtDataDesc_LENGTHINMAUS,
            dataTypeName: 'Int16',
            units: 'none',
            isHidden: false,
            lsb: 0
        },
        {   name: 'fmt',
            dataDesc: DvtTypes.DvtDataDesc_FMTSTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: 'data',
            dataDesc: DvtTypes.DvtDataDesc_DATAARRAY,
            dataTypeName: 'Int32',
            units: 'none',
            isHidden: false
        }
        ]
    };
    /*!
     *  ======== stringOnHeap ========
     *  Analysis event posted when a string snapshot is logged
     *
     *  This event is used internally by the
     *  {@link ti.uia.runtime.LogSnapshot#writeString LogSnapshot.writeString}
     *  API.
     *  @a(Example)
     *   The following C code shows how to log a snapshot event to capture a
     *   block of memory.
     *
     *  @p(code)
     *  #include <ti/uia/runtime/LogSnapshot.h>
     *  #include <string.h> // for strlen
     *  ...
     *  Void myFunc(String name){
     *     ...
     *     //Upload the memory contents of the dynamically allocated string 'name'
     *     LogSnapshot_stringOnHeap(0,"name",name, strlen(name));
     *     //Now that the string memory contents have been uploaded,
     *     //subsequent events that reference the string will be properly
     *     //rendered.
     *     Log_info1("User-defined name=%s.",name);
     *  }
     *  @p
     *  The following text will be displayed for the event, if LogSnapshot was called
     *  from file demo.c at line 1234 and the value of "name" was "aUserDefinedName".
     *  @p(code)
     *  String Snapshot at [../demo.c:1234] [snapshotID=0,adrs=0x80001234,40,40] name.
     *  "demo.c", line 1235: User-defined name=aUserDefinedName.
     *  @p
     *
     *  @param(__FILE__)   The file that the LogSnapshot call site was in (used by %$F)
     *  @param(__LINE__)   The line of code of the LogSnapshot call site (used by %$F)
     *  @param(snapshotID) ID used to identify snapshot events taken at the same
     *                     time. Set to 0 for first in series, set rest to return
     *                     value of LogSnapshot API.
     *  @param(adrs)       the start address of the string in memory
     *  @param(numMAUsDataInEvent) the number of MAUs of data payload for this event
     *  @param(numMAUsDataInRecord) the total number of MAUs of data payload for the
     *                     multi-event data record
     *  @param(fmt)        a constant string that provides a user-readable description
     *                     of what information the event is capturing
     */
    config xdc.runtime.Log.Event stringOnHeap = {
        mask: Diags.ANALYSIS,
        msg: "String Snapshot at %$F [snapshotID=%d,adrs=0x%x,numMAUsDataInEvent=%hd,numMAUsDataInRecord=%hd] %$S"
    };

    /*!
     *  ======== metaEventStringOnHeap ========
     *  Metadata description of the stringOnHeap event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventStringOnHeap = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_STRINGSNAPSHOT,
        displayText: "String Snapshot",
        tooltipText: "String Snapshot",
        numParameters: 8,
        paramInfo: [
        {   name: 'filename',
            dataDesc: DvtTypes.DvtDataDesc_FILENAMESTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: 'linenum',
            dataDesc: DvtTypes.DvtDataDesc_LINENUM,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'snapshotID',
            dataDesc: DvtTypes.DvtDataDesc_SNAPSHOTID,
            dataTypeName: 'UInt32',
            units: 'none',
            isHidden: false
        },
        {   name: 'startAdrs',
            dataDesc: DvtTypes.DvtDataDesc_DATAADRS,
            dataTypeName: 'Ptr',
            units: 'none',
            isHidden: false
        },
        {   name: 'numMAUsDataInEvent',
            dataDesc: DvtTypes.DvtDataDesc_LENGTHINMAUS,
            dataTypeName: 'Int16',
            units: 'none',
            isHidden: false,
            lsb: 16
        },
        {   name: 'numMAUsDataInRecord',
            dataDesc: DvtTypes.DvtDataDesc_LENGTHINMAUS,
            dataTypeName: 'Int16',
            units: 'none',
            isHidden: false,
            lsb: 0
        },
        {   name: 'fmt',
            dataDesc: DvtTypes.DvtDataDesc_FMTSTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: 'data',
            dataDesc: DvtTypes.DvtDataDesc_DATAARRAY,
            dataTypeName: 'Int32',
            units: 'none',
            isHidden: false
        }
        ]
    };
    /*!
     *  ======== nameOfReference ========
     *  Used to log the contents of a dynamic string on the heap so that host-side
     *  tooling can display this string as the name of handle / reference ID
     *
     *  This event is used internally by the
     *  {@link ti.uia.runtime.LogSnapshot#nameOfReference LogSnapshot.nameOfReference}
     *  API.
     *
     *  @a(Example)
     *   The following C code shows how to log a task name for use by task
     *   execution graphs etc.
     *
     *  @p(code)
     *  #include <ti/uia/runtime/LogSnapshot.h>
     *  #include <ti/sysbios/BIOS.h>
     *  #include <ti/sysbios/knl/Task.h>
     *  ...
     *  // Task create hook function that logs the task name.
     *  // Notes: Task name is not trequired when creating a BIOS task. Please \
     *  // make sure a name is provided in order for the host side analysis tool
     *  // to work properly.
     *  Void  tskCreateHook(Task_Handle hTask, Error_Block *eb) {
     *          String name;
     *          name = Task_Handle_name(hTask);
     *          LogSnapshot_writeNameOfReference(hTask,"Task_create name=%s",
     *            name,strlen(name)+1);
     *  }
     *  @p
     *  This event prints the Log call site (%$F) and a format string (%$S)
     *  which describes what information the event is logging.
     *  The following text will be displayed for the event:
     *  @p(code)
     *  nameOfReference at [demo.c:line 1234] [refID=0x80002000,adrs=0x80001234,40,40] Task_create: name=10msThread.
     *  @param(__FILE__)   The file that the LogSnapshot call site was in (used by %$F)
     *  @param(__LINE__)   The line of code of the LogSnapshot call site (used by %$F)
     *  @param(refID)      reference ID (e.g. task handle) that the name is
     *                       associated with
     *  @param(adrs)       the start address of the string in memory
     *  @param(numMAUsDataInEvent) the number of MAUs of data payload for this event
     *  @param(numMAUsDataInRecord) the total number of MAUs of data payload for the
     *                     multi-event data record
     *  @param(fmt)        a constant string that provides a user-readable description
     *                     of what information the event is capturing
     */
    config xdc.runtime.Log.Event nameOfReference = {
        mask: Diags.ANALYSIS,
        msg: "nameOfReference at %$F [refID=0x%x,adrs=0x%x,numMAUsDataInEvent=%hd numMAUsDataInRecord=%hd] %$S"
    };

    /*!
     *  ======== metaEventNameOfReference ========
     *  Metadata description of the NameOfReference event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventNameOfReference = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_NAMESNAPSHOT,
        displayText: "Name Of Reference ID",
        tooltipText: "Name Of Reference ID",
        numParameters: 8,
        paramInfo: [
        {   name: 'filename',
            dataDesc: DvtTypes.DvtDataDesc_FILENAMESTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: 'linenum',
            dataDesc: DvtTypes.DvtDataDesc_LINENUM,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'referenceID',
            dataDesc: DvtTypes.DvtDataDesc_REFERENCEID,
            dataTypeName: 'UInt32',
            units: 'none',
            isHidden: false
        },
        {   name: 'startAdrs',
            dataDesc: DvtTypes.DvtDataDesc_DATAADRS,
            dataTypeName: 'Ptr',
            units: 'none',
            isHidden: false
        },
        {   name: 'numMAUsDataInEvent',
            dataDesc: DvtTypes.DvtDataDesc_LENGTHINMAUS,
            dataTypeName: 'Int16',
            units: 'none',
            isHidden: false,
            lsb: 16
        },
        {   name: 'numMAUsDataInRecord',
            dataDesc: DvtTypes.DvtDataDesc_LENGTHINMAUS,
            dataTypeName: 'Int16',
            units: 'none',
            isHidden: false,
            lsb: 0
        },
        {   name: 'fmt',
            dataDesc: DvtTypes.DvtDataDesc_FMTSTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: 'data',
            dataDesc: DvtTypes.DvtDataDesc_DATAARRAY,
            dataTypeName: 'Int32',
            units: 'none',
            isHidden: false
        }
        ]
    };
}
