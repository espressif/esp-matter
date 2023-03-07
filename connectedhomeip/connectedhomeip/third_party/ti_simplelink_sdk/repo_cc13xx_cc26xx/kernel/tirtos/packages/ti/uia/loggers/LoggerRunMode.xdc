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
 *  ======== LoggerRunMode.xdc ========
 */
import xdc.runtime.ILogger;
import ti.uia.runtime.ILoggerSnapshot;
import xdc.rov.ViewInfo;
import xdc.runtime.Log;
import xdc.runtime.Diags;
import xdc.runtime.Types;
import xdc.runtime.Log;
import xdc.runtime.Error;


/*!
 *  ======== LoggerRunMode ========
 *  General purpose logger enabling applications to stream data
 *  to an instrumentation host through JTAG or ethernet.
 *
 *  LoggerRunMode events can be streamed to an instrumentation
 *  host in real-time, for targets that support either real-time
 *  JTAG or UIA Ethernet transport.  Events can be uploaded without
 *  having to halt the target.
 *
 *  Each LoggerRunMode instance has its own buffer for events logged to that
 *  instance. By including the header file, ti/uia/runtime/LogUC.h, you
 *  can specify the LoggerRunMode instance that you want the event logged
 *  to.
 *
 *  The logger's buffer is split up into three or more 'packets' for
 *  more efficient streaming to the host.  While one packet is being
 *  read by the host, the others can be written to.  When a packet
 *  is filled, the logger starts writing to the next available packet.
 *  Each packet must be large enough to hold {@link #maxEventSize}
 *  bytes, plus an additional number of bytes (64) for a header.
 *  If the configured {@link #bufSize} is not big enough for three
 *  packets of this minimum size, {@link #bufSize} will be increased
 *  automatically.
 *
 *  The logger stores the events in a {@link ti.uia.runtime.UIAPacket#Hdr UIAPacket_Hdr}
 *  structure that allows them to be sent directly to System Analyzer (e.g. via
 *  UDP), enabling
 *  efficient streaming of the data from the target to the host.  The first
 *  four 32-bit words contain a `{@link ti.uia.runtime.UIAPacket#Hdr UIAPacket_Hdr}`
 *  structure. This struct is used by the host (e.g. System Analyzer in CCS)
 *  to help decode the data (e.g. endianess, length of data, etc.).
 *
 *  The size of the buffer includes the {@link ti.uia.runtime.UIAPacket#Hdr UIAPacket_Hdr}.
 *  LoggerRunMode treats the buffer as a UInt32 array, so the application
 *  must guarantee the buffers are aligned on word addresses.  Alignment on
 *  cache line boundaries is recommended for best performance.
 *
 *  LoggerRunMode will overwrite older events if the host is not able
 *  to upload events quickly enough.
 *
 *  LoggerRunMode was designed to have as minimal impact as possible on an
 *  application  when calling a Log function. There are several configuration
 *  parameters that allow an application to get the optimal performance in
 *  exchange for certain restrictions.
 *
 *  Interrupts are disabled during the duration of the log call.
 *
 *  NOTE:  It is recommended that you use {@link ti.uia.sysbios.LoggingSetup LoggingSetup}
 *  to configure the Logger instances. For example to use LoggerRunMode with
 *  real-time JTAG transport, set
 *  {@link ti.uia.sysbios.LoggingSetup#loggerType LoggingSetup.loggerType}
 *  to {@link ti.uia.sysbios.LoggingSetup#LoggerType_JTAGRUNMODE LoggingSetup.LoggerType_JTAGRUNMODE}.
 *
 *  @a(Examples)
 *  The following XDC configuration statements
 *  create a logger module, and assign it as the default logger for all
 *  modules.
 *
 *  @p(code)
 *  var Defaults = xdc.useModule('xdc.runtime.Defaults');
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var LoggerRunMode = xdc.useModule('ti.uia.loggers.LoggerRunMode');
 *
 *  LoggerRunMode.isTimestampEnabled = true;
 *
 *  var loggerParams = new LoggerRunMode.Params();
 *
 *  Program.global.logger0 = LoggerRunMode.create(loggerParams);
 *  Defaults.common$.logger = Program.global.logger0;
 *
 *  @p
 *
 *  @a(Examples)
 *  The following C code demonstrates logging to different LoggerRunMode
 *  instances.
 *
 *  @p(code)
 *  #include <xdc/std.h>
 *  #include <xdc/runtime/Diags.h>
 *  #include <xdc/runtime/Log.h>
 *  #include <ti/uia/loggers/LoggerRunMode.h>
 *  #include <ti/uia/runtime/LogUC.h>
 *
 *  Int main(Int argc, String argv[])
 *  {
 *      Log_iwriteUC0(logger0, LoggerRunMode_L_test);
 *      Log_iwriteUC1(logger1, LoggerRunMode_L_test, 0x1000);
 *  }
 *
 *  @p
 *
 *  @a(Examples)
 *  The following XDC configuration statements show how to use LoggingSetup
 *  with LoggerRunMode.
 *
 *  @p(code)
 *
 *  var LoggingSetup = xdc.useModule('ti.uia.sysbios.LoggingSetup');
 *  LoggingSetup.loggerType = LoggingSetup.LoggingSetup_JTAGRUNMODE;
 *
 *  @p
 */

@ModuleStartup
@Template("./LoggerRunMode.xdt")
@CustomHeader
module LoggerRunMode inherits ILoggerSnapshot {

    /*!
     *  ======== TransportType ========
     *  This enum is used by the instrumentation host to determine what
     *  the transport is.
     */
    enum TransportType {
        TransportType_JTAG = 0,
        TransportType_ETHERNET = 1
    };

    /*!
     *  @_nodoc
     *  ======== ModuleView ========
     */
    metaonly struct ModuleView {
        Bool       isEnabled;
        Bool       isTimestampEnabled;
        String     transportType;
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
     *  ======== transportType ========
     *  Transport used to send the records to an instrumentation host
     *
     *  This parameter is used to specify the transport that will
     *  be used to send the buffer to an instrumentation host (e.g. System
     *  Analyzer in CCS).
     *
     *  This parameter is placed into the generated UIA XML file. The
     *  instrumentation host can use the XML file to help it auto-detect as
     *  much as possible and act accordingly.
     */
    metaonly config TransportType transportType = TransportType_JTAG;

    /*!
     *  @_nodoc
     *  ======== customTransportType ========
     *  Custom transport used to send the records to an instrumentation host
     *
     *  If the desired transport is not in the `{@link #TransportType}` enum,
     *  and `{@link #transportType}` is set to `{@link #TransportType_CUSTOM}`,
     *  this parameter must be filled in with the correct transport name.
     *
     *  If `{@link #transportType}` is NOT set to
     *  `{@link #TransportType_CUSTOM}`, this parameter is ignored.
     */
    config String customTransportType = null;

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
     *  Allow LoggerRunMode instances to be enabled/disabled during runtime.
     *
     *  Setting supportLoggerDisable to true will increase the
     *  footprint of LoggerRunMode.
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
     *  The LoggerRunMode module allows for specifying a different filter level
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
        msg: "LoggerRunMode Test"
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
     *  The number of C6X cores running the same image with an instance in
     *  shared memory.
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
     *  Setting numCores to a value greater than 1 signals LoggerRunMode to
     *  statically set aside additional memory ((x numCores) to allow each
     *  core to have `{@link #transferBufSize}` amount of memory.
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
     *  Initialize the Java LoggerRunModeDecoder for use in the LoggerRunMode
     *  'Records' ROV view.
     */
    function initDecoder();


    /*!
     *  ======== cpuId=========
     *  CPU ID to put in the packet header as the endpoint ID.
     *
     *  Not required for C6X or C7X devices (uses DNUM).
     */
    metaonly config Int cpuId = 0;

    /*!
     *  @_nodoc
     *  ======== isUploadRequired ========
     *  Returns true if aprox. 1 second has elapsed since the last call to flush()
     */
    @DirectCall
    Bool isUploadRequired();

    /*!
     *  ======== idleHook ========
     *  Hook function that can be called by SysBios's Idle loop.
     *  This function ensures that events are uploaded in a timely manner even if
     *  they are logged infrequently.
     */
    @DirectCall
    Void idleHook();

    /*!
     *  ======== enableAutoConfigOfIdleHook ========
     *  If true, the ti.sysbios.knl.Idle module will automatically be
     *  configured to call LoggerRunMode\s idleHook function.
     *  This function ensures that events are uploaded in a timely manner even if
     *  they are logged infrequently.  Set to false if you do not
     *  wish to use the ti.sysbios.nk.Idle module.
     */
    metaonly config Bool enableAutoConfigOfIdleHook = true;

instance:

    /*!
     *  ======== create ========
     *  Create a `LoggerRunMode` logger
     */
    create();

    /*!
     *  @_nodoc
     *  ======== initBuffer ========
     *  Initializes the UIA packet header.
     *
     *  This API is used to initialize a buffer before it is given to
     *  LoggerRunMode (via priming or exchange). The function initializes
     *  the {@link ti.uia.runtime.UIAPacket#Hdr UIAPacket_Hdr} portion of the buffer.
     *
     *  @param(buffer)    Pointer to the buffer that LoggerRunMode will
     *                    fill with Log events. The first four 32-bit words
     *                    will contain the UIAPacket_Hdr structure.
     *
     *  @param(src)       Used to initialize the UIA source address. For
     *                    a single core device, this will generally be 0.
     *                    For multi-core devices, it generally corresponds
     *                    to the DNUM (on C6xxxx deviecs) or the Ipc
     *                    MultiProc id. It must be unique for all cores and
     *                    match the configuration in the System Analyzer
     *                    endpoint configuration.
     */
    Void initBuffer(Ptr buffer, UInt16 src);

    /*!
     *  @_nodoc
     *  ======== flush ========
     *  Force LoggerRunMode to use the next packet.
     */
    Void flush();

    /*!
     *  @_nodoc
     *  ======== prime ========
     *  Initializes the event buffer prior to event logging
     */
    @DirectCall
    Ptr prime();

    /*!
     *  @_nodoc
     *  ======== exchange ========
     *  Set the logger's write pointer to the next packet.
     */
    @DirectCall
    Ptr exchange(Ptr full, Ptr lastWritePtr);


    /*!
     *  @_nodoc
     *  ======== initQueueDescriptor ========
     *  Initialize the QueueDescriptor Header (for use
     *  with JTAGRUNMODE transport)
     *  @param(mid) the Logger's module ID
     */
    @DirectCall
    Void initQueueDescriptor(xdc.runtime.Types.ModuleId mid);

    /*!
     *  ======== instanceId ========
     *  Unique id of the LoggerRunMode instance.
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
     *  LoggerRunMode instance's buffer size in MAUs (Minimum Addressable
     *  Units e.g. Bytes)
     *
     *  NOTE: the buffer size must contain an integer number of 32-bit words
     *  (e.g. if a MAU = 1 byte, then the buffer size must be a multiple of 4).
     *  The buffer size must also be at least maxEventSize + 64.
     */
    config SizeT bufSize = 1024;

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
    metaonly config Int maxId = 0;

    /*!
     *  ======== filterOutEvent ========
     */
    @DirectCall
    Bool filterOutEvent(Diags.Mask mask);

    /*!
     * ======== writeStart ========
     */
    @DirectCall
    UArg *writeStart(Object *obj, xdc.runtime.Log.Event evt,
            xdc.runtime.Types.ModuleId mid, UInt16 numBytes);

    struct Module_State {
        Bool        enabled;       /* Enabled state */
        Diags.Mask level1;
        Diags.Mask level2;
        Diags.Mask level3;
        Bits32     lastUploadTstamp;
    };

    struct Instance_State {
        Bool        enabled;       /* Enabled state */
        Int16       instanceId;
        Bool        primeStatus;   /* Has the Log been primed? */
        UInt32      bufSize;       /* Size of the buffer */
        UArg       *buffer;        /* Ptr to buffer */
        UArg       *write;         /* Ptr to write location */
        UArg       *end;
        SizeT       maxEventSizeUArgs;    /* Max event size in # of UArgs */
        SizeT       maxEventSize;         /* Max event size in target MAUs */
        UInt16      numBytesInPrevEvent;
        /*
         *  incremented by writeMemoryRange when event is too big to log or
         *  no buffers available
         */
        Bits32  droppedEvents;
        UInt16  eventSequenceNum;
        UInt16  pktSequenceNum;

        /*
         *  QueueDescriptor header for JTAG transports.  NULL for all
         *  other transports.
         */
        Char hdr[];

        /*
         *  Packet buffers for use with JTAG transports.  NULL for other
         *  transports
         */
        Char packetArray[];
        Int numPackets;
        UInt32 packetSize;
    };
}
