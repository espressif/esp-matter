/*
 * Copyright (c) 2013-2014, Texas Instruments Incorporated
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
 *  ======== LoggerIdle.xdc ========
 */
import xdc.runtime.ILogger;
import xdc.runtime.Log;
import xdc.rov.ViewInfo;

/*!
 *  ======== LoggerIdle ========
 *  A logger which routes `Log` events to a users transport function.
 *
 *  This logger processes log events as they are generated, stores them in
 *  a buffer and during idle sends a section of the buffer to the user's
 *  transport function.  If you are seeing no log events or dropping too
 *  many events check that you are not logging too often and have enough idle
 *  time to send. LoggerIdle is compatable with StellarisWare and MWare
 *  devices. Example transports for UART (B92 and F28M35x) and USB (F28M35x)
 *  as well as initialization functions are included in the evmF28M35x.c files
 *  under the device folder in the ti.examples directory.
 *
 *  @a(Examples)
 *  Configuration example: The following XDC configuration statements
 *  create a logger module, and assign it as the default logger for all
 *  modules.
 *
 *  @p(code)
 *  var Defaults = xdc.useModule('xdc.runtime.Defaults');
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var LoggerIdle = xdc.useModule('ti.uia.sysbios.LoggerIdle');
 *
 *  LoggerIdle.bufferSize = 60;
 *  LoggerIdle.timestamp = false;
 *  LoggerIdle.transportType = LoggerIdle.TransportType_UART;
 *  LoggerIdle.transportFxn = '&LoggerIdle_uartSend';
 *  var LoggerIdleParams = new LoggerIdle.Params();
 *  Defaults.common$.logger = LoggerIdle.create(LoggerIdleParams);
 *  @p
 */

@Template("./LoggerIdle.xdt")
module LoggerIdle inherits ILogger {

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
        Int        bufferSize;
        UInt       sequenceNumber;
        String     transportType;
        String     customTransport;
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

    /*!
     *  ======== LoggerFxn ========
     *  Typedef for the transport function pointer.
     */
    typedef Int (*LoggerFxn)(UChar *, Int);

    /*!
     *  ======== bufferSize ========
     *  LoggerIdle buffer size in 32-bit words.
     */
    config SizeT bufferSize = 256;

    /*!
     *  ======== isTimestampEnabled ========
     *  Enable or disable logging the 64b local CPU timestamp
     *  at the start of each event
     *
     *  Having a timestamp allows an instrumentation host (e.g.
     *  System Analyzer) to display events with the correct system time.
     */
    config Bool isTimestampEnabled = true;

    /*!
     *  ======== transportType ========
     *  Transport used to send the records to an instrumentation host
     *
     *  This parameter is used to specify the transport that the
     *  `{@link #transportFxn}` function will use to send the buffer to
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
    metaonly config TransportType transportType = TransportType_UART;

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
     *  ======== transportFxn ========
     *  User defined transport function responsible for transmitting the records
     */
    config LoggerFxn transportFxn = null;

    /*!
     *  @_nodoc
     *  ======== writeWhenFull ========
     *
     *  This configuration parameter has been deprecated.  The behavior
     *  is now to always allow wtites when the LoggerIdle buffer is full.
     *
     *  Allow Log writes to succeed even if the the LoggerIdle buffer is
     *  full.
     *
     *  LoggerIdle maintains a circular buffer where the Log events are
     *  written.  A write pointer indicates the location in the buffer
     *  where the next Log event can be written to, and a read pointer
     *  indicates the location of the next Log event to be sent to the
     *  user's transport function.  Log write calls cause the write pointer
     *  to advance, and when Log data is passed to the user's transport
     *  function in the idle loop, the read pointer advances. If the
     *  read pointer catches up the the write pointer, the buffer is
     *  'empty', and if the write pointer catches up the the read pointer,
     *  the buffer is full.
     *
     *  The LoggerIdle buffer will fill up, if the idle function to output
     *  the Log data cannot keep up with the Log writes. When this happens,
     *  if writeWhenFull is false, Log writes will not put any new data
     *  into the buffer until the LoggerIdle transportFxn has been called
     *  to empty some of the buffer.  As a result, the most recent Log
     *  events could be lost. This is a simple solution to dealing with a
     *  full buffer. Since Log event sizes vary, it avoids having to
     *  determine how much the read pointer must be adjusted to fit a new
     *  Log event. It also allows you to send a large chunk of the buffer
     *  to the transport function in one shot, since the data will not
     *  be overwritten by Log writes during the transfer. If Log events
     *  are infrequent or the idle time is sufficient to get the Log
     *  data out, then disabling writeWhenFull may be appropriate.
     *
     *  When this flag is set to true, if the LoggerIdle buffer is full,
     *  new Log data will be written over the oldest Log record(s) in the
     *  buffer.  The oldest Log records in the buffer will be lost when
     *  this happens.
     *
     *  The cost of enabling writeWhenFull is an increase in Log write
     *  times when the buffer is full, as the buffer's read pointer will
     *  need adjusting. There is also more overhead to get the Log data
     *  out through the transport function. When writeWhenFull is enabled,
     *  LoggerIdle's idle function will copy one Log record at a time into
     *  a temporary buffer, and send the temporary buffer to the user's
     *  transport function. This is to minimize interrupt latency, as the
     *  buffer's read pointer can now be modified by both the idle function
     *  and Log writes, and must be protected. The advantage, though, is
     *  that you will not lose the most recent Log data when the buffer is
     *  full.  If Log events are frequent and the idle time is insufficient
     *  to get the Log data out, then enabling writeWhenFull may be
     *  appropriate.
     */
    config Bool writeWhenFull = false;

    /*!
     *  @_nodoc
     *  ======== L_test ========
     *  Event used for benchmark tests
     */
    config xdc.runtime.Log.Event L_test = {
        mask: xdc.runtime.Diags.USER1,
        msg: "Test Event"
    };

    /*!
     *  ======== flush ========
     *  Call the transport function to empty out the LoggerIdle buffer.
     *
     *  This API is not intended for general use, but could be used for
     *  example, in an exception handler to recover the most recent Log
     *  data that was written after the last run of the idle loop.
     *
     *  NOTE:  Calling LoggerIdle_flush() when the idle task was in the
     *  middle of outputting data can result in lost data.  Since the idle
     *  function only outputs one Log record at a time, so at most one record
     *  could be lost.
     */
    Void flush();

    /*!
     *  @_nodoc
     *  ======== initDecoder ========
     *  Initialize the Java LoggerIdleDecoder for use in the LoggerIdle
     *  'Records' ROV view.
     */
    function initDecoder();

instance:
    /*!
     *  ======== create ========
     *  Create a `LoggerIdle` logger
     *
     *  The logger instance will route all log events it receives to
     *  the Uart.
     */
    create();

    @DirectCall
    override Void write0(xdc.runtime.Log.Event evt, xdc.runtime.Types.ModuleId mid);

    @DirectCall
    override Void write1(xdc.runtime.Log.Event evt, xdc.runtime.Types.ModuleId mid,
                            IArg a1);

    @DirectCall
    override Void write2(xdc.runtime.Log.Event evt, xdc.runtime.Types.ModuleId mid,
                            IArg a1, IArg a2);

    @DirectCall
    override Void write4(xdc.runtime.Log.Event evt, xdc.runtime.Types.ModuleId mid,
                            IArg a1, IArg a2, IArg a3, IArg a4);

    @DirectCall
    override Void write8(xdc.runtime.Log.Event evt, xdc.runtime.Types.ModuleId mid,
                            IArg a1, IArg a2, IArg a3, IArg a4,
                            IArg a5, IArg a6, IArg a7, IArg a8);

internal:

    /*!
     *  ======== idleWrite =========
     *  Idle function that calls the transport function.
     */
    Void idleWrite();

    /*!
     *  ======== idleWriteEvent =========
     *  Idle function that calls the transport function to write one
     *  Log event.
     */
    Void idleWriteEvent();

    Void write(Log.Event evt, xdc.runtime.Types.ModuleId mid,
            IArg numEventWords,
            IArg a1, IArg a2, IArg a3, IArg a4,
            IArg a5, IArg a6, IArg a7, IArg a8);

    struct Module_State {
        LoggerFxn loggerFxn;
        Bool enabled;          /* If the logger is enabled or not */
        Bool empty;         /* True if there is data in the buffer */
        UInt bufferSize;       /* Size of the buffer in words */
        UInt32 idleBuffer[];   /* Stores log events to be sent */
        UInt32 tempBuffer[];   /* For copying Event records into. */
        UInt32 *bufferRead;    /* Pointer to the first word to be read */
        UInt32 *bufferWrite;   /* Pointer to the next word to write to */
        UInt32 *bufferPad;     /* Pointer to the last word in the buffer when
                                  the buffer overflows into the 10 word pad */
        UInt32 *bufferEnd;     /* Pointer to begining of the buffer pad */
        UInt16  eventSequenceNum;  /* event sequence number */
    };

    struct Instance_State {
    };
}
