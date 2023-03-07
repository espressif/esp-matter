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
 *  ======== LoggerStreamer2.xdc ========
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
 *  ======== LoggerStreamer2 ========
 *  General purpose logger enabling applications to stream data
 *  to an instrumentation host.
 *
 *  This logger is an enhancement of {@link LoggerStreamer}, and is useful in
 *  situations where the application wants to manage the buffers used
 *  to store and transmit events. This includes managing the sending of the
 *  buffers to an instrumentation host (e.g. System Analyzer in CCS).
 *  LoggerStreamer2 enables the application to send a stream of
 *  packets containing UIA event data to the instrumentation host.
 *
 *  The difference between LoggerStreamer2 and {@link LoggerStreamer}, is that
 *  each LoggerStreamer2 instance has its own buffer for events logged to that
 *  instance. By including the header file, ti/uia/runtime/LogUC.h, you
 *  can specify the LoggerStreamer2 instance that you want the event logged
 *  to.
 *
 *  The application is responsible for providing the buffers that
 *  LoggerStreamer2 loggers use. There are two ways to accomplish this.
 *  @p(blist)
 *      - Provide a prime callback function via the `{@link #primeFxn}`
 *        configuration parameter.
 *      - Call the `{@link #prime}` API once.
 *  @p
 *
 *  The logger stores the events in a {@link ti.uia.runtime.UIAPacket#Hdr UIAPacket_Hdr}
 *  structure that allows them to be sent directly to System Analyzer (e.g. via
 *  UDP), enabling
 *  efficient streaming of the data from the target to the host.  The first
 *  four 32-bit words contain a `{@link ti.uia.runtime.UIAPacket#Hdr UIAPacket_Hdr}`
 *  structure. This struct is used by the host (e.g. System Analyzer in CCS)
 *  to help decode the data (e.g. endianess, length of data, etc.). The
 *  `{@link ti.uia.runtime.UIAPacket#Hdr UIAPacket_Hdr}` structure is
 *  initialized via the `{@link #initBuffer}` API. All buffers
 *  given to LoggerStreamer2 loggers (via priming or exchange) must be
 *  initialized via `{@link #initBuffer}`.
 *
 *  The size of the buffer includes the {@link ti.uia.runtime.UIAPacket#Hdr UIAPacket_Hdr}.
 *  LoggerStreamer2 treats the buffer as a UInt32 array, so the application
 *  must guarantee the buffers are aligned on word addresses.  Alignment on
 *  cache line boundaries is recommended for best performance.
 *
 *  When the buffer is filled, LoggerStreamer2 will hand it off to the
 *  application using an application-provided exchange function (`{@link #exchangeFxn}`).
 *  The exchange function must be of type `{@link #ExchangeFxnType}`.  The
 *  exchange function takes the Log instance as a parameter.
 *
 *  The exchange function is called within the context of a Log call, so the
 *  exchange function should be designed to be fast. Since the exchange
 *  function is called within the context of the Log call, LoggerStreamer2
 *  guarantees no Log records are dropped (i.e. LoggerStreamer2 is lossless).
 *
 *  LoggerStreamer2 was designed to have as minimal impact as possible on an
 *  application  when calling a Log function. There are several configuration
 *  parameters that allow an application to get the optimal performance in
 *  exchange for certain restrictions.
 *
 *  Interrupts are disabled during the duration of the log call including
 *  when the exchange function is called. LoggerStreamer2 will ignore any
 *  log events generated during the exchangeFxn (e.g. posting a semaphore).
 *
 *  NOTE:  You can use {@link LoggingSetup}, but you will need to set the loggers
 *  first.  {@link LoggingSetup} cannot create LoggerStreamer2 instances, since it
 *  does not know what config parameters to use.  See example of
 *  {@link LoggingSetup} use below.
 *
 *  @a(Examples)
 *  The following XDC configuration statements
 *  create a logger module, and assign it as the default logger for all
 *  modules.
 *
 *  @p(code)
 *  var Defaults = xdc.useModule('xdc.runtime.Defaults');
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var LoggerStreamer2 = xdc.useModule('ti.uia.sysbios.LoggerStreamer2');
 *
 *  LoggerStreamer2.isTimestampEnabled = true;
 *
 *  var loggerParams = new LoggerStreamer2.Params();
 *  loggerParams.primeFxn = '&prime';
 *  loggerParams.exchangeFxn = '&exchange';
 *  loggerParams.context = 0;
 *
 *  Program.global.logger0 = LoggerStreamer2.create(loggerParams);
 *  Defaults.common$.logger = Program.global.logger0;
 *
 *  loggerParams.context = 1;
 *  Program.global.logger1 = LoggerStreamer2.create(loggerParams);
 *
 *  @p
 *
 *  @a(Examples)
 *  The following C code demonstrates basic prime and exchange functions,
 *  and logging to different LoggerStreamer2 instances.
 *  A real implementation would send the buffer to an instrumentation
 *  host (e.g. System Analyzer in CCS) via a transport such as UDP.
 *
 *  @p(code)
 *  #include <xdc/std.h>
 *  #include <xdc/runtime/Diags.h>
 *  #include <xdc/runtime/Log.h>
 *
 *  #include <ti/uia/runtime/LogUC.h>
 *
 *  UInt32 buffer_0[2][BUFSIZE_0];
 *  UInt32 buffer_1[2][BUFSIZE_1];
 *
 *  Int main(Int argc, String argv[])
 *  {
 *      Log_iwriteUC0(logger0, LoggerStreamer2_L_test);
 *      Log_iwriteUC1(logger1, LoggerStreamer2_L_test, 0x1000);
 *  }
 *
 *  Ptr prime(LoggerStreamer2_Object *log)
 *  {
 *      UInt32 id = (UInt32)LoggerStreamer2_getContext(log);
 *
 *      if (id == 0) {
 *          LoggerStreamer2_initBuffer(buffer_0[0], 0);
 *          LoggerStreamer2_initBuffer(buffer_0[1], 0);
 *          return ((Ptr)buffer_0[0]);
 *      }
 *      if (id == 1) {
 *          LoggerStreamer2_initBuffer(buffer_1[0], 0);
 *          LoggerStreamer2_initBuffer(buffer_1[1], 0);
 *          return ((Ptr)(buffer_1[0]));
 *      }
 *
 *      return (NULL); // Error
 *  }
 *
 *  Ptr exchange(LoggerStreamer2_Object *log, Ptr *full)
 *  {
 *      UInt32 id = (UInt32)LoggerStreamer2_getContext(log);
 *
 *      if (id == 0) {
 *          count_0++;
 *          // Ping-pong between the two buffers
 *          return ((Ptr*)buffer_0[count_0 & 1]);
 *      }
 *      if (id == 1) {
 *          count_1++;
 *          // Ping-pong between the two buffers
 *          return ((Ptr*)buffer_1[count_1 & 1]);
 *      }
 *      return (NULL);  // Error
 *  }
 *  @p
 *
 *  @a(Examples)
 *  The following XDC configuration statements show how to use LoggingSetup
 *  with LoggerStreamer2.
 *
 *  @p(code)
 *
 *  var LoggingSetup = xdc.useModule('ti.uia.sysbios.LoggingSetup');
 *  LoggingSetup.eventUploadMode = LoggingSetup.UploadMode_STREAMER2;
 *  LoggingSetup.mainLogger = Program.global.logger0;
 *  LoggingSetup.sysbiosLogger = Program.global.logger0;
 *
 *  @p
 */

@ModuleStartup
@Template("./LoggerStreamer2.xdt")
@CustomHeader
module LoggerStreamer2 inherits ILoggerSnapshot {

    /*!
     *  ======== TransportType ========
     *  Used to specify the type of transport to use
     *
     *  This enum is used by the instrumentation host to determine what
     *  the transport is. It is not used by the target code.
     */
    enum TransportType {
        TransportType_UART = 0,
        TransportType_USB = 1,
        TransportType_ETHERNET = 2,
        TransportType_CUSTOM = 3
    };

    /*!
     *  @_nodoc
     *  ======== ModuleView ========
     */
    metaonly struct ModuleView {
        Bool       isEnabled;
        Bool       isTimestampEnabled;
        String     transportType;
        String     customTransport;
    }

    /*!
     *  @_nodoc
     *  ======== InstanceView ========
     */
    metaonly struct InstanceView {
        String    label;
        Bool      enabled;
        Int       bufferSize;
        String    primeFunc;
        String    exchangeFunc;
        SizeT     maxEventSize;
        UArg      context;
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
                        type: xdc.rov.ViewInfo.MODULE_DATA,
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
     *  ======== ExchangeFxnType ========
     *  Typedef for the exchange function pointer.
     *
     *  The exchange function takes a LoggerStreamer2 object and a pointer to
     *  the full buffer as arguments.  If using the same exchange function
     *  for multiple LoggerStreamer2 instances, {@link #getContext()} can be
     *  called within the exchange function to determine which buffer to
     *  exchange.
     */
    typedef Ptr (*ExchangeFxnType)(Object *, Ptr);

    /*!
     *  ======== PrimeFxnType ========
     *  Typedef for the exchange function pointer.
     */
    typedef Ptr (*PrimeFxnType)(Object *);

    /*!
     *  ======== transportType ========
     *  Transport used to send the records to an instrumentation host
     *
     *  This parameter is used to specify the transport that the
     *  `{@link #exchangeFxn}` function will use to send the buffer to
     *  an instrumentation host (e.g. System Analyzer in CCS).
     *
     *  This parameter is placed into the generated UIA XML file. The
     *  instrumentation host can use the XML file to help it auto-detect as
     *  much as possible and act accordingly.
     *
     *  If the desired transport is not in the `{@link #TransportType}` enum,
     *  select `{@link #TransportType_CUSTOM}` and set the
     *  `{@link #customTransportType}` string with the desired string.
     */
    metaonly config TransportType transportType = TransportType_ETHERNET;

    /*!
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
     *  Enable or disable logging the 64b local CPU timestamp
     *  at the start of each event
     *
     *  Having a timestamp allows an instrumentation host (e.g.
     *  System Analyzer) to display events with the correct system time.
     */
    config Bool isTimestampEnabled = false;

    /*!
     * @_nodoc
     *  ======== isBadPacketDetectionEnabled ========
     *  Enable or disable checking that the event contents in the packet are
     *  properly formatted with no data errors
     *
     *  If enabled, a breakpoint can be placed in the code to detect when
     *  a bad packet is found.
     */
    config Bool isBadPacketDetectionEnabled = false;

    /*!
     *  ======== supportLoggerDisable ========
     *  Allow LoggerStreamer2 instances to be enabled/disabled during runtime.
     */
    config Bool supportLoggerDisable = false;

    /*!
     *  ======== testForNullWrPtr ========
     *  Protect against log calls during the exchange function.
     */
    config Bool testForNullWrPtr = true;

    /*!
     *  ======== statusLogger ========
     *  This configuration option is not supported by this logger and should
     *  be left null.
     */
    config xdc.runtime.IFilterLogger.Handle statusLogger = null;

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
     *  The LoggerCircBuf module allows for specifying a different filter level
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
     *  ======== moduleToRouteToStatusLogger ========
     *  This configuration option is not supported by this logger and should
     *  be left unconfigured.
     */
    metaonly config String moduleToRouteToStatusLogger;

    /*!
     *  ======== setModuleIdToRouteToStatusLogger ========
     *  This function is provided for compatibility with the ILoggerSnapshot
     *  interface only and simply returns when called.
     */
    @DirectCall
    Void setModuleIdToRouteToStatusLogger(Types.ModuleId mid);

    /*!
     * @_nodoc
     *  ======== L_test ========
     *  Event used to benchmark write0.
     */
    config xdc.runtime.Log.Event L_test = {
        mask: xdc.runtime.Diags.USER1,
        msg: "Test"
    };

    /*!
     * @_nodoc
     *  ======== E_badLevel ========
     *  Error raised if get or setFilterLevel receive a bad level value
     */
    config Error.Id E_badLevel = {
         msg: "E_badLevel: Bad filter level value: %d"
    };

    /*!
     *  ======== A_invalidBuffer ========
     *  Assert raised when the buffer parameter is NULL
     */
    config xdc.runtime.Assert.Id A_invalidBuffer =
        {msg: "LoggerStreamer2_create's buffer returned by primeFxn is NULL"};

instance:

    /*!
     *  ======== create ========
     *  Create a `LoggerStreamer2` logger
     */
    create();

    /*!
     *  ======== initBuffer ========
     *  Initializes the UIA packet header.
     *
     *  This API is used to initialize a buffer before it is given to
     *  LoggerStreamer2 (via priming or exchange). The function initializes
     *  the {@link ti.uia.runtime.UIAPacket#Hdr UIAPacket_Hdr} portion of the buffer.
     *
     *  @param(buffer)    Pointer to the buffer that LoggerStreamer2 will
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
     *  ======== flush ========
     *  Force LoggerStreamer2 to call the exchange function
     *
     *  This API makes LoggerStreamer2 call the application provided
     *  `{@link #exchangeFxn}` function if there are Log events present
     *  in the buffer.
     *
     *  The call to the `{@link #exchangeFxn}` function is called in the
     *  context of the flush call.
     */
     Void flush();

     /*!
      *  ======== prime =========
      *  If PrimeFxn is not set the user must call prime with the first buffer.
      */
     Bool prime(Ptr buffer);

    /*!
     * @_nodoc
     *  ======== validatePacket ========
     *  if isBadPacketDetectionEnabled is configured as true, this function is
     *  called prior to the exchange function being called.
     *
     *  Returns null if the packet is ok, else returns the address of a string
     *  that describes the error.
     */
    @DirectCall
    Char* validatePacket(UInt32 *writePtr, UInt32 numBytesInPacket);

    /*!
     *  ======== context ========
     *  Context that can be used in exchangeFxn and primeFxn.
     *
     *  The context can be used to identify the logger instance, when
     *  using the same {@link #exchangeFxn} or {@link #primeFxn} for
     *  multiple LoggerStreamer2 instances.  Use {@link #getContext()}
     *  to get the logger context. The context can be changed, if needed,
     *  by calling {@link #setContext()}.
     *  The context can be set to null if using a different exchangeFxn and primeFxn
     *  for each LoggerStreamer2 instance.
     */
    config UArg context = null;

    /*!
     *  ======== primeFxn ========
     *  Function pointer to the prime function.
     */
    config PrimeFxnType primeFxn = null;

    /*!
     *  ======== exchangeFxn ========
     *  Function pointer to the exchange function.
     *
     *  The exchange function must return a pointer to a buffer that is word
     *  aligned, initialized with a UIA header and the correct size.  This is
     *  called in the context of a log so generally the exchange function
     *  should be quick to execute.
     */
    config ExchangeFxnType exchangeFxn = null;

    /*!
     *  ======== instanceId ========
     *  Unique id of the LoggerStreamer2 instance.
     */
    config Int16 instanceId = 1;

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
     *  LoggerStreamer2 instance's buffer size in MAUs (Minimum Addressable
     *  Units e.g. Bytes)
     *
     *  NOTE: the buffer size must contain an integer number of 32b words
     *  (e.g. if a MAU = 1 byte, then the buffer size must be a multiple of 4).
     *  The buffer size must also be at least maxEventSize + 64.
     */
    config SizeT bufSize = 1400;

    /*!
     *  ======== write0 ========
     *  Process a log event with 0 arguments and the calling address.
     *
     *  Same as `write4` except with 0 arguments rather than 4.
     *  @see #write4()
     */
    @DirectCall
    override Void write0(xdc.runtime.Log.Event evt,
                xdc.runtime.Types.ModuleId mid);

    /*!
     *  ======== write1 ========
     *  Process a log event with 1 arguments and the calling address.
     *
     *  Same as `write4` except with 1 arguments rather than 4.
     *  @see #write4()
     */
    @DirectCall
    override Void write1(xdc.runtime.Log.Event evt,
                xdc.runtime.Types.ModuleId mid,
                IArg a1);

    /*!
     *  ======== write2 ========
     *  Process a log event with 2 arguments and the calling address.
     *
     *  Same as `write4` except with 2 arguments rather than 4.
     *  @see #write4()
     */
    @DirectCall
    override Void write2(xdc.runtime.Log.Event evt,
                xdc.runtime.Types.ModuleId mid,
                IArg a1, IArg a2);

    /*!
     *  ======== write4 ========
     *  Process a log event with 4 arguments and the calling address.
     *
     *  @see ILogger#write4()
     */@DirectCall
    override Void write4(xdc.runtime.Log.Event evt,
                xdc.runtime.Types.ModuleId mid,
                IArg a1, IArg a2, IArg a3, IArg a4);

    /*!
     *  ======== write8 ========
     *  Process a log event with 8 arguments and the calling address.
     *
     *  Same as `write4` except with 8 arguments rather than 4.
     *
     *  @see #write4()
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

    /*!
     *  ======== getContext ========
     *  Returns the Log's context
     *
     *  @b(returns)     context
     */
    @DirectCall
    UArg getContext();

    /*!
     *  ======== setContext ========
     *  Set the Log's context.
     *
     *  @param(context)     New value of Log's context
     */
    @DirectCall
    Void setContext(UArg context);

internal:
    metaonly config Int maxId = 0;

    /* Write size in bytes for generating event length */
    const Int WRITE0_SIZE_IN_BYTES = 8;
    const Int WRITE1_SIZE_IN_BYTES = 12;
    const Int WRITE2_SIZE_IN_BYTES = 16;
    const Int WRITE4_SIZE_IN_BYTES = 24;
    const Int WRITE8_SIZE_IN_BYTES = 40;
    /* Bytes added for timestamps; used for generating event length in bytes */
    const Int TIMESTAMP = 8;
    const Int NO_TIMESTAMP = 0;

    /*!
     *  ======== filterOutEvent ========
     */
    @DirectCall
    Bool filterOutEvent(Diags.Mask mask);

    /*
     *  Interesting...I moved the enabled field to the end and performance
     *  in the exchange case was slower by 4 cycles...
     */
    struct Module_State {
        Bool        enabled;       /* Enabled state */
        Diags.Mask level1;
        Diags.Mask level2;
        Diags.Mask level3;
        Types.ModuleId moduleIdToRouteToStatusLogger;
    };

    struct Instance_State {
        Bool        enabled;       /* Enabled state */
        Int16       instanceId;
        PrimeFxnType    primeFxn;
        ExchangeFxnType exchangeFxn;
        UArg        context;
        Bool        primeStatus;   /* Has the Log been primed? */
        UInt32      bufSize;       /* Size of the buffer */
        UArg       *buffer;        /* Ptr to buffer */
        UArg       *write;         /* Ptr to write location */
        UArg       *end;
        SizeT       maxEventSizeUArg; /* Max event size in UArgs */
        SizeT       maxEventSize;  /* Max event size in target MAUs */

        /*
         *  incremented by writeMemoryRange when event is too big to log or
         *  no buffers available
         */
        Int  droppedEvents;
        UInt16      seqNumber;
    };
}
