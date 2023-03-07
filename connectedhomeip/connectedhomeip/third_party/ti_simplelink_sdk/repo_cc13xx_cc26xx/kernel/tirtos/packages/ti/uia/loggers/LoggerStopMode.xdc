/*
 * Copyright (c) 2013-2018, Texas Instruments Incorporated
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
 *
 */


/*
 *  ======== LoggerStopMode.xdc ========
 */
import xdc.runtime.ILogger;
import ti.uia.runtime.ILoggerSnapshot;
import xdc.rov.ViewInfo;
import xdc.runtime.Log;
import xdc.runtime.Diags;
import xdc.runtime.Types;
import xdc.runtime.Log;
import xdc.runtime.Error;

import ti.uia.runtime.EventHdr;

/*!
 *  ======== LoggerStopMode ========
 *  General purpose logger enabling applications to log data that
 *  can be read by an instrumentation host over JTAG, when the
 *  target is halted.
 *
 *  Each LoggerStopMode instance has its own buffer for events logged to that
 *  instance. By including the header file, ti/uia/runtime/LogUC.h, you
 *  can specify the LoggerStopMode instance that you want the event logged
 *  to.
 *
 *  The size of the buffer includes a {@link ti.uia.runtime.UIAPacket#Hdr UIAPacket_Hdr}.
 *  LoggerStopMode treats the buffer as a UInt32 array, so the application
 *  must guarantee that buffers are aligned on word addresses.  Alignment on
 *  cache line boundaries is recommended for best performance.
 *
 *  When the buffer is filled, LoggerStopMode will update the QueueDescriptor
 *  data structure (which provides the interface needed by the host's JTAG
 *  transport handler to read the logged event data) and wrap around the buffer's
 *  write pointer.
 *
 *  LoggerStopMode was designed to have as minimal impact as possible on an
 *  application  when calling a Log function. There are several configuration
 *  parameters that allow an application to get the optimal performance in
 *  exchange for certain restrictions.
 *
 *  Interrupts are disabled during the duration of the log call.
 *
 *  NOTE:  It is recommended that you use {@link ti.uia.sysbios.LoggingSetup LoggingSetup}
 *  to configure the Logger instances. Set
 *  {@link ti.uia.sysbios.LoggingSetup#loggerType LoggingSetup.loggerType}
 *  to {@link ti.uia.sysbios.LoggingSetup#LoggerType_STOPMODE LoggingSetup.LoggerType_STOPMODE}
 *  to specify that the Logger instances are based on LoggerStopMode.
 *
 *  @a(Examples)
 *  The following XDC configuration statements
 *  create a logger module, and assign it as the default logger for all
 *  modules.
 *
 *  @p(code)
 *  var Defaults = xdc.useModule('xdc.runtime.Defaults');
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var LoggerStopMode = xdc.useModule('ti.uia.loggers.LoggerStopMode');
 *
 *  LoggerStopMode.isTimestampEnabled = true;
 *
 *  var loggerParams = new LoggerStopMode.Params();
 *  Program.global.logger0 = LoggerStopMode.create(loggerParams);
 *  Defaults.common$.logger = Program.global.logger0;
 *
 *  Program.global.logger1 = LoggerStopMode.create(loggerParams);
 *
 *  @p
 *
 *  @a(Examples)
 *  The following XDC configuration statements show how to use LoggingSetup
 *  with LoggerStopMode.  Note that the second line is unnecessary, since
 *  LoggerStopMode is the default logger for LoggingSetup.
 *
 *  @p(code)
 *
 *  var LoggingSetup = xdc.useModule('ti.uia.sysbios.LoggingSetup');
 *  LoggingSetup.loggerType = LoggingSetup.LoggerType_STOPMODE;
 *
 *  @p
 */

@ModuleStartup
@Template("./LoggerStopMode.xdt")
@CustomHeader
module LoggerStopMode inherits ILoggerSnapshot {


    /*!
     *  @_nodoc
     *  ======== ModuleView ========
     */
    metaonly struct ModuleView {
        Bool       isEnabled;
        Bool       isTimestampEnabled;
    }

    /*!
     *  @_nodoc
     *  ======== InstanceView ========
     */
    metaonly struct InstanceView {
        String    label;
        Bool      enabled;
        Int       bufferSize;
        SizeT     maxEventSize;
    }

    metaonly struct RecordView {
        Int     sequence;
        Long    timestampRaw;
        String  modName;
        String  text;
        Int     eventId;
        String  eventName;
        IArg    arg0;
        IArg    arg1;
        IArg    arg2;
        IArg    arg3;
        IArg    arg4;
        IArg    arg5;
        IArg    arg6;
        IArg    arg7;
    }

    /*!
     *  @_nodoc
     *  ======== rovViewInfo ========
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [
                ['Module',
                    {
                        type: ViewInfo.MODULE,
                        viewInitFxn: 'viewInitModule',
                        structName: 'ModuleView'
                    }
                ],
                ['Instances',
                    {
                        type: ViewInfo.INSTANCE,
                        viewInitFxn: 'viewInitInstances',
                        structName: 'InstanceView'
                    }
                ],
                ['Records',
                    {
                        type: xdc.rov.ViewInfo.INSTANCE_DATA,
                        viewInitFxn: 'viewInitRecords',
                        structName: 'RecordView'
                    }
                ]
            ]
        });

    /*!
     *  ======== RtaData ========
     *  Data added to the RTA MetaData file to support System Analyzer
     */
    @XmlDtd metaonly struct RtaData {
        Int instanceId;
    }

    /*!
     *  ======== isTimestampEnabled ========
     *  Enable or disable logging the 64-bit local CPU timestamp
     *  at the start of each event
     *
     *  Having a timestamp allows an instrumentation host (e.g.
     *  System Analyzer) to display events with the correct system time.
     */
    config Bool isTimestampEnabled = true;


    /*!
     *  ======== supportLoggerDisable ========
     *  Allow LoggerStopMode instances to be enabled/disabled during runtime.
     *
     *  Setting supportLoggerDisable to true will increase the
     *  LoggerStopMode footprint.
     */
    config Bool supportLoggerDisable = false;

    /*!
     *  @_nodoc
     *  ======== statusLogger ========
     *  For backwards compatibility with ti.uia.runtime.LoggerStopMode.
     */
    metaonly config xdc.runtime.IFilterLogger.Handle statusLogger = null;

    /*!
     *  @_nodoc
     *  ======== overflowLogger ========
     *  For backwards compatibility with ti.uia.runtime.LoggerStopMode.
     */
    metaonly config ILogger.Handle overflowLogger = null;

    /*!
     *  ======== level1Mask ========
     *  Mask of diags categories whose initial filtering level is Diags.LEVEL1
     *
     *  See '{@link #level4Mask}' for details.
     */
    config Diags.Mask level1Mask = 0;

    /*!
     *  ======== level2Mask ========
     *  Mask of diags categories whose initial filtering level is Diags.LEVEL2
     *
     *  See '{@link #level4Mask}' for details.
     */
    config Diags.Mask level2Mask = 0;

    /*!
     *  ======== level3Mask ========
     *  Mask of diags categories whose initial filtering level is Diags.LEVEL3
     *
     *  See '{@link #level4Mask}' for details.
     */
    config Diags.Mask level3Mask = 0;

    /*!
     *  ======== level4Mask ========
     *  Mask of diags categories whose initial filtering level is Diags.LEVEL4
     *
     *  If 'filterByLevel' is true, then all LoggerBuf instances will filter
     *  incoming events based on their event level.
     *
     *  The LoggerStopMode module allows for specifying a different filter level
     *  for every Diags bit. These filtering levels are module wide; LoggerBuf
     *  does not support specifying the levels on a per-instance basis.
     *
     *  The setFilterLevel API can be used to change the filtering levels at
     *  runtime.
     *
     *  The default filtering levels are assigned using the 'level1Mask' -
     *  'level4Mask' config parameters. These are used to specify, for each of
     *  the four event levels, the set of bits which should filter at that
     *  level by default.
     *
     *  The default filtering configuration sets the filter level to
     *  Diags.LEVEL4 for all logging-related diags bits so that all events are
     *  logged by default.
     */
    config Diags.Mask level4Mask = Diags.ALL_LOGGING;

    /*!
     *  @_nodoc
     *  ======== moduleToRouteToStatusLogger ========
     *  For backwards compatibility with ti.uia.runtime.LoggerStopMode.
     */
    metaonly config String moduleToRouteToStatusLogger = null;

    /*!
     * @_nodoc
     *  ======== L_test ========
     *  Event used to benchmark write0.
     */
    config xdc.runtime.Log.Event L_test = {
        mask: xdc.runtime.Diags.USER1,
        msg: "LoggerStopMode Test"
    };

    /*!
     * @_nodoc
     *  ======== E_badLevel ========
     *  Error raised if get or setFilterLevel receive a bad level value
     */
    config Error.Id E_badLevel = {
         msg: "E_badLevel: Bad filter level value: %d"
    };

    metaonly config Int cacheLineSizeInMAUs = 128;

    metaonly Int getNumInstances();

    /*!
     *  ======== numCores ========
     *  Number of C6X cores running the same image with an instance in shared memory
     *
     *  A common use case is to have the same binary image (e.g. .out file)
     *  run on multiple cores of multi-core device. This causes a problem if the
     *  logger's buffer is in shared memory (e.g. DDR). Since the image is the
     *  same for all the cores, each core will attempt to write to the same
     *  buffer in the shared memory. To avoid this, either place the logger's
     *  buffers in non-shared memory, or set the numCores parameter to
     *  the number of cores on the device.
     *
     *  Note: Use the `{@link #bufSection}` along with the Program.sectMap to
     *  place a logger instance's buffer into specific memory.
     *
     *  Setting numCores to a value greater than 1 signals LoggerStopMode to
     *  statically set aside additional memory ((x numCores) to allow each
     *  core to have `{@link #bufSize}` amount of memory.
     *
     *  Warning: Setting this parameter to a value greater than one should only
     *  be done when there is a single image used on multiple cores of a
     *  multi-core device AND the logger instance's buffer is in shared memory.
     *  While functionally it will still work, memory will be wasted if both
     *  these conditions are not met.
     *
     *  The default is 1, which means do not reserve any additional memory
     *  for the logger.
     *
     */
    config Int numCores = 1;

    /*!
     *  @_nodoc
     *  ======== initDecoder ========
     *  Initialize the Java LoggerStopModeDecoder for use in the LoggerStopMode
     *  'Records' ROV view.
     */
    function initDecoder();

instance:

    /*!
     *  ======== create ========
     *  Create a `LoggerStopMode` logger
     */
    create();

    /*!
     *  ======== flush ========
     *  Reset the logger's write pointer back to the beginning of the buffer.
     */
     Void flush();


     /*!
      *  @_nodoc
      *  ======== initQueueDescriptor ========
      *  Initialize the QueueDescriptor Header
      *  @param(mid) the Logger's module ID
      */
     @DirectCall
     Void initQueueDescriptor(xdc.runtime.Types.ModuleId mid);

    /*!
     *  ======== instanceId ========
     *  Unique id of the LoggerStopMode instance.
     */
    config Int16 instanceId = 1;

    /*!
     *  @_nodoc
     *  ======== transferBufSize ========
     *  For backwards compatibility with ti.uia.runtime.LoggerStopMode, bufSize
     *  should be used instead.
     */
    config SizeT transferBufSize = 0;

     /*!
     *  ======== maxEventSize ========
     *  The maximum event size (in Maus) that can be written with a single
     *  event. Must be less than or equal to bufSize - 64.
     *
     *  The writeMemoryRange API checks to see if the event size required to
     *  write the block of memory is larger than maxEventSize.  If so, it will
     *  split the memory range up into a number of smaller blocks and log the
     *  blocks using separate events with a common snapshot ID in order to
     *  allow the events to be collated and the original memory block to be
     *  reconstructed on the host.
     */
    config SizeT maxEventSize = 512;

    /*!
     *  ======== bufSize ========
     *  LoggerStopMode instance's buffer size in MAUs (Minimum Addressable
     *  Units e.g. Bytes)
     *
     *  NOTE: the buffer size must contain an integer number of 32-bit words
     *  (e.g. if a MAU = 1 byte, then the buffer size must be a multiple of 4).
     *  The buffer size must also be at least maxEventSize + 64.
     */
    config SizeT bufSize = 1400;

    /*!
     *  ======== bufSection ========
     *  Section name for the buffer managed by the static instance.
     *
     *  The default section is the 'dataMemory' in the platform.
     */
    metaonly config String bufSection = null;

    /*!
     *  @_nodoc
     *  ======== bufHeap ========
     *  For backwards compatibility with ti.uia.runtime.LoggerStopMode.
     */
    metaonly config xdc.runtime.IHeap.Handle bufHeap = null;

    /*!
     *  ======== write0 ========
     *  Process a log event with 0 arguments.
     */
    @DirectCall
    override Void write0(xdc.runtime.Log.Event evt,
                xdc.runtime.Types.ModuleId mid);

    /*!
     *  ======== write1 ========
     *  Process a log event with 1 argument.
     */
    @DirectCall
    override Void write1(xdc.runtime.Log.Event evt,
                xdc.runtime.Types.ModuleId mid,
                IArg a1);

    /*!
     *  ======== write2 ========
     *  Process a log event with 2 arguments.
     */
    @DirectCall
    override Void write2(xdc.runtime.Log.Event evt,
                xdc.runtime.Types.ModuleId mid,
                IArg a1, IArg a2);

    /*!
     *  ======== write4 ========
     *  Process a log event with 4 arguments.
     */
    @DirectCall
    override Void write4(xdc.runtime.Log.Event evt,
                xdc.runtime.Types.ModuleId mid,
                IArg a1, IArg a2, IArg a3, IArg a4);

    /*!
     *  ======== write8 ========
     *  Process a log event with 8 arguments.
     */
    @DirectCall
    override Void write8(xdc.runtime.Log.Event evt,
                xdc.runtime.Types.ModuleId mid,
                IArg a1, IArg a2, IArg a3, IArg a4,
                IArg a5, IArg a6, IArg a7, IArg a8);

    /*!
     *  ======== setFilterLevel ========
     *  Sets the level of detail that instances will log.
     *
     *  Events with the specified level or higher will be logged, events
     *  below the specified level will be dropped.
     *
     *  Events are filtered first by diags category, then by level. If an
     *  event's diags category is disabled in the module's diags mask, then it
     *  will be filtered out regardless of level. The event will not even be
     *  passed to the logger.
     *
     *  This API allows for setting the filtering level for more than one
     *  diags category at a time. The mask parameter can be a single category
     *  or multiple categories combined, and the level will be set for all of
     *  those categories.
     *
     *  @param(mask) The diags categories to set the level for
     *  @param(filterLevel) The new filtering level for the specified
     *                      categories
     */
    @DirectCall
    override Void setFilterLevel(Diags.Mask mask, Diags.EventLevel filterLevel);

    /*!
     *  ======== getFilterLevel ========
     *  Returns the mask of diags categories currently set to the specified
     *  level.
     *
     *  See '{@link #setFilterLevel}' for an explanation of level filtering.
     */
    @DirectCall
    override Diags.Mask getFilterLevel(Diags.EventLevel level);

    /*!
     *  ======== getBufSize ========
     *  Returns the Log's configured buffer size.
     *
     *  @b(returns)     Log's configured buffer size.
     */
    @DirectCall
    SizeT getBufSize();

internal:

    /*!
     * @_nodoc
     *  ======== initBuffer ========
     *  Initializes the UIA packet header.
     */
    Void initBuffer(Object *obj, Ptr buffer, UInt16 src);

    metaonly config Int maxId = 0;

    /*!
     *  ======== filterOutEvent ========
     */
    @DirectCall
    Bool filterOutEvent(Diags.Mask mask);

    struct Module_State {
        Bool        enabled;       /* Enabled state */
        Diags.Mask  level1;
        Diags.Mask  level2;
        Diags.Mask  level3;
    };

    struct Instance_State {
        Bool        enabled;       /* Enabled state */
        Int16       instanceId;
        UInt32      bufSize;       /* Size of the buffer */
        UArg       *buffer;        /* Ptr to buffer */
        UArg       *write;         /* Ptr to write location */
        UArg       *end;
        SizeT       maxEventSizeUArgs;    /* Max event size in # of UArgs */
        SizeT       maxEventSize;  /* Max event size in target MAUs */
        UInt16      numBytesInPrevEvent;
        /*
         *  incremented by writeMemoryRange when event is too big to log or
         *  no buffers available
         */
        Bits32  droppedEvents;
        UInt16  eventSequenceNum;
        UInt16  pktSequenceNum;
        Char hdr[];           /* Array of numCores QueueDescriptor headers */
        Char packetArray[];   /* Array of numCores buffers */
    };
}
