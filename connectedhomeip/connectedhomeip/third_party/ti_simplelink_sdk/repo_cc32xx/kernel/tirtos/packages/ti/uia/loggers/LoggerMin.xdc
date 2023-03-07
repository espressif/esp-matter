/*
 * Copyright (c) 2013-2015, Texas Instruments Incorporated
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
 *  ======== LoggerMin.xdc ========
 */
import xdc.runtime.ILogger;
import xdc.rov.ViewInfo;
import xdc.runtime.Log;
import xdc.runtime.Diags;
import xdc.runtime.Types;
import xdc.runtime.Log;

import ti.uia.runtime.EventHdr;

/*!
 *  ======== LoggerMin ========
 *  This general purpose logger is useful in situations where a very small
 *  memory overhead is required.
 *
 *  The logger stores all events in a single buffer with a compact
 *  UIAPacket event packet structure that allows them to be sent directly
 *  to System Analyzer via JTAG.
 *
 *  LoggerMin was designed to have as minimal impact as possible on an
 *  application when calling a Log function. There are several configuration
 *  parameters that allow an application to get the optimal performance in
 *  exchange for certain restrictions.
 *
 *  Interrupts are disabled during the duration of the log call.
 *
 *  NOTE:  It is recommended that you use {@link ti.uia.sysbios.LoggingSetup LoggingSetup}
 *  to configure the Logger. Set
 *  {@link ti.uia.sysbios.LoggingSetup#loggerType LoggingSetup.loggerType}
 *  to {@link ti.uia.sysbios.LoggingSetup#LoggerType_MIN LoggingSetup.LoggerType_MIN}
 *  to specify that the Logger is based on LoggerMin.
 *
 *  @a(Examples)
 *  The following XDC configuration statements
 *  create a logger module, and assign it as the default logger for all
 *  modules.
 *
 *  @p(code)
 *  var Defaults = xdc.useModule('xdc.runtime.Defaults');
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var LoggerMin = xdc.useModule('ti.uia.loggers.LoggerMin');
 *
 *  LoggerMin.bufSize = 256;
 *  LoggerMin.timestampSize = LoggerMin.TimestampSize_32b;
 *  Defaults.common$.logger = LoggerMin.create();
 *  @p
 *
 *  @a(Examples)
 *  The following XDC configuration statements show how to use LoggingSetup
 *  with LoggerMin.
 *
 *  @p(code)
 *
 *  var LoggingSetup = xdc.useModule('ti.uia.sysbios.LoggingSetup');
 *  LoggingSetup.loggerType = LoggingSetup.LoggerType_MIN;
 *
 *  @p
 */

@ModuleStartup
@Template("./LoggerMin.xdt")
@CustomHeader
module LoggerMin inherits ILogger {

    /*!
     *  ======== TimestampSize ========
     *  Enum of size of timestamps to log with events.
     */
    enum TimestampSize {
        TimestampSize_NONE = 0, /*! No timestamps will be logged with events */
        TimestampSize_32b = 1,  /*! 32-bit timestamp */
        TimestampSize_64b = 2   /*! 64-bit timestamp */
    };

    /*!
     *  @_nodoc
     *  ======== ModuleView ========
     */
    metaonly struct ModuleView {
        Bool       isEnabled;
        String     timestampSize;
        Int        bufferSize;
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

    /*! @_nodoc
     *  ======== getLoggerInstanceId ========
     *  returns the id of this logger instance
     *  (always 1 for LoggerMin)
     */
    metaonly function getLoggerInstanceId(inst);

    /*!
     *  @_nodoc
     *  ======== initBuffer ========
     *  Initializes the UIA packet header portion of the buffer.
     *
     *  @param(buffer)    Pointer to the buffer that LoggerMin will
     *                    fill with Log events. The first four 32-bit words
     *                    will contain the UIAPacket_Hdr structure.
     *
     *  @param(endpointId)   Used to initialize the UIA source address. For
     *                    a single core device, this will generally be 0.
     *                    For multi-core devices, it generally corresponds
     *                    to the DNUM (on C6xxxx deviecs) or the Ipc
     *                    MultiProc id. It must be unique for all cores and
     *                    match the configuration in the System Analyzer
     *                    endpoint configuration.
     */
    @Macro Void initBuffer(Ptr buffer, UInt16 endpointId);

    /*!
     *  @_nodoc
     *  ======== flush ========
     */
    @DirectCall
    Void flush();

    /*!
     *  ======== bufSize ========
     *  LoggerMin buffer size in MAUs (Minimum Addressable Units e.g.
     *  Bytes).
     *
     *  The buffer size must be less than 65536 bytes.
     *
     *  NOTE: the buffer size must contain an integer number of 32-bit words
     *  (e.g. if a MAU = 1 byte, then the buffer size must be a multiple of 4).
     *  The buffer size must also be at least maxEventSize + 64.
     */
    config SizeT bufSize = 512;

    /*!
     *  ======== bufSection ========
     *  Section name for the buffer managed by the static instance.
     *
     *  The default section is the 'dataMemory' in the platform.
     */
    metaonly config String bufSection = null;

    /*!
     * @_nodoc
     *  ======== numCores ========
     *  Number of cores running the same image with an instance in shared memory
     *
     *  A common use case is to have the same binary image (e.g. .out file)
     *  run on multiple cores of multi-core device. This causes a problem if the
     *  logger's buffer is in shared memory (e.g. DDR). Since the image is the
     *  same for all the cores, each core will attempt to write to the same
     *  buffer in the shared memory. To avoid this, either the logger's buffers
     *  must be in non-shared memory or by setting the numCores parameter to
     *  the number of cores on the device.
     *
     *  Note: the `{@link #bufSection}` along with the Program.sectMap is how
     *  a logger instance's buffer is placed into specific memory.
     *
     *  Setting numCores to a value great than 1 signals LoggerCircBuf to
     *  statically set aside additional memory ((x numCores) to allow each
     *  core to have `{@link #transferBufSize}` amount of memory.
     *
     *  Warning: setting this parameter to a value greater than one should only
     *  be done when there is a single image used on multiple cores of a
     *  multi-core device AND the logger instance's buffer is in shared memory.
     *  While functionally it will still work, memory will be wasted if both
     *  these conditions are not met.
     *
     *  The default is 1, which means do not reserve any additional memory
     *  for the logger.
     */
    config Int numCores = 1;

    /*!
     *  ======== memoryAlignmentInMAUs ========
     *  Memory Alignment in MAUs (Minimum Addressable Units)
     *
     *  Specifies alignment to use when allocating the internal packet buffer
     *  Set to 1 if no alignment is required.
     */
    metaonly config Int memoryAlignmentInMAUs = 1;

    /*!
     *  ======== timestampSize ========
     *  Configure the size of the timestamp to use.
     *  For minimum event footprint, configure as TimestampSize_32b (default).
     *
     *  Having a timestamp allows an instrumentation host (e.g.
     *  System Analyzer) to display events with the correct system time.
     */
    config TimestampSize timestampSize = TimestampSize_32b;

    /*!
     * @_nodoc
     *  ======== L_test ========
     *  Event used to benchmark write0.
     */
    config xdc.runtime.Log.Event L_test = {
        mask: xdc.runtime.Diags.USER1,
        msg: "LoggerMin Test"
    };

    /*!
     *  ======== supportLoggerDisable ========
     *  Allow Logger instances to be enabled/disabled during runtime.
     *
     *  LoggerMin footprint is smaller when supportLoggerDisable is
     *  false.
     */
    config Bool supportLoggerDisable = false;

    /*!
     *  @_nodoc
     *  ======== endpointId ========
     *  An id indicating which core in a multicore device the logger is in
     *  (For C6X devices, set this to the DNUM value)
     */
    config Bits16 endpointId = 0;

    /*!
     *  @_nodoc
     *  ======== loggerInstanceId ========
     *  An id indicating which logger this is for applications with multiple loggers
     *  (Required for metadata generation - RtaData)
     */
    config Bits16 loggerInstanceId = 0;

    /*!
     *  @_nodoc
     *  ======== write ========
     */
    @DirectCall
    Void write(xdc.runtime.Log.Event evt,
                xdc.runtime.Types.ModuleId mid,
                IArg numBytes,
                IArg a1, IArg a2, IArg a3, IArg a4,
                IArg a5, IArg a6, IArg a7, IArg a8);

    /*!
     *  @_nodoc
     *  ======== initDecoder ========
     *  Initialize the Java LoggerMinDecoder for use in the LoggerMin
     *  'Records' ROV view.
     */
    function initDecoder();

instance:

    /*!
     *  ======== create ========
     *  Create a `LoggerMin` logger
     */
    create();

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


    @DirectCall
    override Bool enable();

    @DirectCall
    override Bool disable();

internal:

    /*
     *  ======== getContents =========
     *  For testing.
     *  Fills buffer that is passed in with unread data, up to size MAUs
     *  in length.
     *
     *  The logger is responsible for ensuring that no partial event records
     *  are stored in the buffer.  Bytes are in target endianness.
     *
     *  @param(hdrBuf)   Ptr to a buffer that is at least <size> MAUs in length
     *  @param(size)     The max number of MAUs to be read into the buffer
     *  @param(cpSize)   The number of MAUs actually copied
     *
     *  @a(return)       returns false if logger has no more records to read
     */
    Bool getContents(Object *obj, Ptr hdrBuf, SizeT size, SizeT *cpSize);

    /*
     *  ======== isEmpty =========
     *  For testing.
     *
     *  returns true if the Logger buffer is empty, otherwise false.
     */
    Bool isEmpty(Object *obj);

    /*
     *  ======== genTimestamp ========
     *  Write out the timestamp, depending on whether it's 64 bit,
     *  32 bit, or disabled.
     */
    Ptr genTimestamp(Ptr writePtr);

    struct Module_State {
        Bool        enabled;          /* Enabled state */
        Bool        empty;
        UInt16      numBytesInPrevEvent;
        UInt16      droppedEvents;    /* Currently not used */
        Char        packetBuffer[];
        /*
         *  DO NOT RENAME the following elements - they are used by System
         *  Analyzer JTAG Transport
         */
        UInt32      *start;        /* Ptr to start of event log buffer */
        UInt32      *write;        /* Ptr to write location */
        UInt32      *end;          /* Ptr to end of the buffer */
        UInt16      eventSequenceNum;  /* event sequence number */
        UInt16      pktSequenceNum;
        EventHdr.HdrType eventType;
    };

    struct Instance_State {
    };
}
