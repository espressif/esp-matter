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
 *  ======== LogSnapshot.xdc ========
 */
package ti.uia.runtime;
import xdc.runtime.Types;
import ti.uia.runtime.LoggerTypes;
import xdc.runtime.ILogger;
import ti.uia.runtime.CtxFilter;
import xdc.runtime.ILogger;
import xdc.runtime.Diags;
import xdc.runtime.Text;
import ti.uia.events.UIASnapshot;
import ti.uia.runtime.IUIATraceSyncClient;

/*!
 *  ======== LogSnapshot ========
 *  Snapshot Event logging manager for logging blocks of memory, strings in memory
 *     and names of dynamically created objects
 *
 *  {@link ti.uia.events.UIASnapshot Snapshot events} are used to log dynamic
 *  target state information in order to
 *  capture the execution context of the application at a particular moment in
 *  time.  Types of information that can be logged include:
 *  @p(blist)
 *      - Blocks of memory (using the {@link #writeMemoryBlock LogSnapshot_writeMemoryBlock} API)
 *      - Strings that reside in memory (using the {@link #writeString LogSnapshot_writeString} API)
 *      - Names of dynamically created objects (using the {@link #writeNameOfReference LogSnapshot_writeNameOfReference} API)
 *  @p
 *  The host-side tooling can be instructed to treat a series of LogSnapshot
 *  events as representing the state of the target at the same moment in time by
 *  using the same non-zero value in the {@link #getSnapshotId snapshot ID} parameter
 *  of each of the LogSnapshot events in the series.
 *
 *  Snapshot events are logged by a logger that implements the
 *  {@link ti.uia.runtime.ILoggerSnapshot ILoggerSnapshot}
 *  interface (e.g. {@link ti.uia.loggers.LoggerStopMode LoggerStopMode},
 *  {@link ti.uia.loggers.LoggerRunMode LoggerRunMode}).
 *  Rather than invoking the logger's APIs directly, the APIs are
 *  called indirectly via the LogSnapshot module's APIs so that different types
 *  of loggers can be used without having to recompile the source code that is
 *  logging the snapshot events.
 * @a(Examples)
 *  @p(html)
 *  <B>Example 1: A simple 2-line configuration script</B>
 *  @p
 *  The following configuration script shows the simplest way
 * to configure an application to use log snapshot events:
 * @p(code)
 * var LoggingSetup = xdc.useModule('ti.uia.sysbios.LoggingSetup');
 * var LogSnapshot   = xdc.useModule('ti.uia.runtime.LogSnapshot');
 * @p
 * The first line causes the {@link ti.uia.sysbios.LoggingSetup LoggingSetup}
 * module's .xs script to automatically create a 32K Byte logger and assign it to
 * xdc.runtime.Main.common$.logger.  It also sets any unconfigured Diags masks
 * for the Main module to Diags.RUNTIME_ON, enabling the events to be logged.
 * @p
 * The second line causes the LogSnapshot .xs script to run during the configuration
 * process when building the application. The script detects that a logger has
 * not been assigned to the LogSnapshot module, so it checks if
 * a logger has been configured for either the Main module or the Defaults module.
 * Since there is a logger for the Main module, the script configures the LogSnapshot
 * module to log events to the same logger instance.
 *
 *  @p(html)
 *  <hr />
 *  <B>Example 3: Using Diags masks to control snapshot events</B>
 *  @p
 *  The generation of a 'Snapshot' event is controlled by a module's diagnostics
 *  mask, which is described in details in the CDoc for xdc.runtime.Diags. Each
 *  {@link ti.uia.events.UIASnapshot snapshot event} is controlled using the
 *  Diags.ANALYSIS mask, and will only be logged when the diagnostics mask for
 *  the module that is logging the code has the Diags.ANALYSIS bit
 *  configured as either ALWAYS_ON or RUNTIME_ON.
 *  @p
 *  The `LogSnapshot` function call sites are implemented in such a way that an
 *  optimizer can completely eliminate `LogSnapshot` code from the program if
 *  the module's `ANALYSIS` events have been disabled at configuration time. If
 *  the 'ANALYSIS' events are permanently turned on at configuration time,
 *  then the optimizer can eliminate all runtime conditional checking and
 *  simply invoke the 'LogSnapshot' functions directly. Runtime checking is
 *  performed only when the ANALYSIS events are configured to be runtime
 *  modifiable.
 *  @p
 * The following is an example of the configuration script used
 * to configure the default mask for modules to have Analysis events such as
 * the UIASnapshot events always on.
 * @p(code)
 *  // Configure all modules to always log Analysis events
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var Defaults = xdc.useModule('xdc.runtime.Defaults');
 *  Defaults.common$.diags_ANALYSIS = Diags.ALWAYS_ON;
 * @p
 *
 */

@CustomHeader
module LogSnapshot inherits IUIATraceSyncClient {

    /*!
     *  ======== EventRec ========
     *  The target representation of a recorded event
     *
     *  This structure defines how events are recorded on the target.
     */
    struct EventRec {
        Types.Timestamp64 tstamp; /*! time event was written */
        Bits32 serial; /*! serial number of event */
        Types.Event evt; /*! target encoding of an Event */
        UArg snapshotId;
        IArg fmt;
        Ptr pData;
        UInt16 lengthInMAUs; /*! arguments passed via Log_write/print */
    }
    /*!
     *  ======== maxLengthInMAUs ========
     *  Maximum number of MAUs (miniumum addressable units, e.g. bytes)
     *     supported by `LogSnapshot` events.
     *
     *  Attempting to write more than the maximum length results in the
     *  multiple events being logged.  The maxLengthInMAUs must be
     *  lower than the size of the buffer that the events are being logged to.
     *  Must be less than 1400 in order to support streaming of event
     *  data over UDP.
     */
    config Int maxLengthInMAUs = 512;

    /*!
     *  ======== isTimestampEnabled ========
     *  used to enable or disable logging the 64b local CPU timestamp
     *  at the start of each event
     */
    config Bool isTimestampEnabled = true;

    /*! @_nodoc
     *  ======== loggerDefined ========
     *  set to true in the configuration script when a logger that implements
     *  ILoggerSnapshot is attached
     */
    config Bool loggerDefined = false;
    /*!
     *  ======== loggerObj ========
     *  handle of the logger that is to be used to log snapshot events
     */
    config Ptr loggerObj = null;

    /*! @_nodoc */
    config ti.uia.runtime.LoggerTypes.LogMemoryRangeFxn loggerMemoryRangeFxn = null;
    /*======================================================================*/

    /*!
     *  ======== putMemoryRange ========
     *  Unconditionally put the specified `Types` event.
     *  Supports both writeMemoryRange and writeString.
     *
     *  This method unconditionally puts the specified memoryRange`{@link Types#Event}`
     *  `evt` into the log.  This type of event is created either implicitly
     *  (and passed to an `{@link ISnapshotLogger}` implementation) or explicitly
     *  via `{@link Types#makeEvent()}`.
     *
     *  @param(evt)         the `Types` event to put into the log
     *  @param(mid)         the module ID of the caller
     *  @param(snapshotId)  unique ID that binds together a series of events used to
     *                      log a large memory range.  Upper 16b hold the ID Tag (0 for UIA)
     *  @param(fileName)    the name of the file that the event was logged from
     *  @param(lineNum)     the line number that the event was logged from
     *  @param(fmt)         a user-specified print format string
     *  @param(startAdrs)   the start address of the memory range to log
     *  @param(lengthInMAUs)   the number of minimum addressable units (e.g. bytes) to log
     *  @a(return)          value to use as snapshotId parameter for subsequent events
     */
    @Macro Void putMemoryRange(Types.Event evt, Types.ModuleId mid,
      IArg fileName, IArg lineNum, UArg snapshotID, IArg fmt, IArg startAdrs,
            IArg lengthInMAUs);

    /*!
     *  ======== writeMemoryBlockWithIdTag ========
     *  Generate a `LogSnapshot` event for a block of memory
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
     *  UInt16 myCustomIdTag = 1;
     *  LogSnapshot_writeMemoryBlockWithIdTag(myCustomIdTag,0,"pIntArray ptr=0x%x, numBytes=%d",(UInt32)pIntArray,200);
     *  ...
     *  @p
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
     *  @param(idTag) 	   ID used to identify who logged the event. Set to 0 for a
     *                     standard UIA event.  Can be used to filter snapshot events on host.
     *  @param(snapshotID) ID used to identify snapshot events taken at the same
     *                     time. Set to 0 for first in series, set rest to return
     *                     value of LogSnapshot API.  {@link #getSnapshotId see getSnapshotId()}
     *  @param(fmt)        a constant string that provides a user-readable description
     *                     of what information the event is capturing
     *  @param(pMemoryRange)  the start address of the range of memory
     *  @param(lengthInMAUs) the number of MAUs of data payload for the
     *                     multi-event data record
     */
    @Macro Void writeMemoryBlockWithIdTag(UInt16 idTag, UInt32 snapshotID, IArg fmt, Ptr pMemoryRange,
      UInt16 lengthInMAUs);

    /*!
     *  ======== writeMemoryBlock ========
     *  Generate a `LogSnapshot` event for a block of memory
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
     *  @param(snapshotID) ID used to identify snapshot events taken at the same
     *                     time. Set to 0 for first in series, set rest to return
     *                     value of LogSnapshot API.  {@link #getSnapshotId see getSnapshotId()}
     *  @param(fmt)        a constant string that provides a user-readable description
     *                     of what information the event is capturing
     *  @param(pMemoryRange)  the start address of the range of memory
     *  @param(lengthInMAUs) the number of MAUs of data payload for the
     *                     multi-event data record
     */
    @Macro Void writeMemoryBlock(UInt32 snapshotID, IArg fmt, Ptr pMemoryRange,
      UInt16 lengthInMAUs);

    /*!
     *  ======== writeStringWithIdTag ========
     *  Generate a `LogSnapshot` event for a string in memory
     *
     *  @a(Example)
     *   The following C code shows how to log a snapshot event to log the
     *   contents of a string in memory.
     *
     *  @p(code)
     *  #include <ti/uia/runtime/LogSnapshot.h>
     *  ...
     *  Void myFunc(String name){
     *     ...
     *     UInt16 myCustomIdTag = 1;
     *     LogSnapshot_writeStringWithIdTag(myCustomIdTag,0,"User-defined name=%s.",name, strlen(name));
     *  }
     *  @p
     *  The following text will be displayed for the event, if it was logged
     *  from file demo.c at line 1234 and all bytes in the 40 character string
     *  was logged in the same event.
     *  @p(code)
     *  String Snapshot at [../demo.c:1234] [snapshotID=0,adrs=0x80001234,40,40] User-defined name=ValueOfParm.
     *  @p
     *  @param(idTag) 	   ID used to identify who logged the event. Set to 0 for a
     *                     standard UIA event.  Can be used to filter snapshot events on host.
     *  @param(snapshotID) ID used to identify snapshot events taken at the same
     *                     time. Set to 0 for first in series, set rest to return
     *                     value of LogSnapshot API. {@link #getSnapshotId see getSnapshotId()}
     *  @param(fmt)        a constant string that provides a user-readable description
     *                     of what information the event is capturing
     *  @param(pString)    the start address of the string in memory
     *  @param(lengthInMAUs) the number of MAUs to log (e.g. strlen(pString))
     */
    @Macro Void writeStringWithIdTag(UInt16 idTag, UInt32 snapshotID, IArg fmt, Ptr pString,
    UInt16 lengthInMAUs);

    /*!
     *  ======== writeString ========
     *  Generate a `LogSnapshot` event for a string in memory
     *
     *  @a(Example)
     *   The following C code shows how to log a snapshot event to log the
     *   contents of a string in memory.
     *
     *  @p(code)
     *  #include <ti/uia/runtime/LogSnapshot.h>
     *  ...
     *  Void myFunc(String name){
     *     ...
     *     LogSnapshot_writeString(0,"User-defined name=%s.",name, strlen(name));
     *  }
     *  @p
     *  The following text will be displayed for the event, if it was logged
     *  from file demo.c at line 1234 and all bytes in the 40 character string
     *  was logged in the same event.
     *  @p(code)
     *  String Snapshot at [../demo.c:1234] [snapshotID=0,adrs=0x80001234,40,40] User-defined name=ValueOfParm.
     *  @p
     *  @param(snapshotID) ID used to identify snapshot events taken at the same
     *                     time. Set to 0 for first in series, set rest to return
     *                     value of LogSnapshot API. {@link #getSnapshotId see getSnapshotId()}
     *  @param(fmt)        a constant string that provides a user-readable description
     *                     of what information the event is capturing
     *  @param(pString)    the start address of the string in memory
     *  @param(lengthInMAUs) the number of MAUs to log (e.g. strlen(pString))
     */
    @Macro Void writeString(UArg snapshotID, IArg fmt, Ptr pString,
            UInt16 lengthInMAUs);

    /*!
     *  ======== nameOfReference ========
     *  Used to log the contents of a dynamic string on the heap so that host-side
     *  tooling can display this string as the name of handle / reference ID
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
     *          LogSnapshot_writeNameOfReference(hTask,"Task_create: name=%s",
     *            name,strlen(name)+1);
     *  }
     *  @p
     *  This event prints the Log call site (%$F) and a format string (%$S)
     *  which describes what information the event is logging.
     *  The following text will be displayed for the event:
     *  @p(code)
     *  nameOfReference at  [demo.c:line 1234] [refID=0x80002000,adrs=0x80001234,40,40] Task_create: name=10msThread.
     *  @param(refID)        reference ID (e.g. task handle) that the name is
     *                       associated with
     *  @param(pString)      the start address of the string on the heap
     *  @param(lengthInMAUs) the number of MAUs to log (e.g. strlen(pString))
     *  @param(fmt)          a constant string that provides format specifiers
     *                       describing the string
     */
    @Macro Void writeNameOfReference(UArg refID, IArg fmt, Ptr pString,
    UInt16 lengthInMAUs);

    /*!
     * ======== getSnapshotId ========
     * returns a unique ID to use to group a set of snapshot event logs together
     *
     * Allows tooling to treat a set of consecutive event logs as a unit and
     * display all of the relevent data together as a set
     *  @a(Example)
     *   The following C code shows how to log two snapshot events that capture
     *   the target state of two different data structures, using a common unique
     *   non-zero snapshot ID provided by the getSnapshotId to inform the host-side
     *   tooling that the events represent the target state at the same point in time
     *  @p(code)
     *  #include <ti/uia/runtime/LogSnapshot.h>
     *  ...
     *  MyStruct1 myStruct1;
     *  MyStruct2 myStruct2;
     *  UInt32 snapshotId;
     *  ...
     *  snapshotId = LogSnapshot_getSnapshotId();
     *  LogSnapshot_writeMemoryBlock(snapshotId,"myStruct1 ptr=0x%x, numBytes=%d",(UInt32)&myStruct1,sizeof(MyStruct1));
     *  LogSnapshot_writeMemoryBlock(snapshotId,"myStruct2 ptr=0x%x, numBytes=%d",(UInt32)&myStruct2,sizeof(MyStruct2));
     *  ...
     *  @p
     * @a(return) a unique non-zero snapshot ID to pass in as a parameter to the
     *    LogSnapshot APIs
     */
    @DirectCall
    UArg getSnapshotId();

    /*!
     *  ======== doPrint ========
     *  Render an event as text via `{@link System#printf System_printf}`
     *
     *  This method is not currently implemented.
     *
     *  @param(evRec)   a non`NULL` pointer to an initialized `
     *     LogSnapshot_EventRec`structure to be formated via
     *     {@link System#printf System_printf}`.
     */
    Void doPrint(EventRec *er);

    /* @_nodoc
     *  ======== idToInfo ========
     *  Map event ID strings into a string of the form <eventName>::<eventMsg>
     */
    metaonly config String idToInfo[string] = [];

}
