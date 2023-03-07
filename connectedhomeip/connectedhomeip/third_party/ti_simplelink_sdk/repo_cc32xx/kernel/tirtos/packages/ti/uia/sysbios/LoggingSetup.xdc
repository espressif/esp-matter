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
 *
 /

/*
 *  ======== LoggingSetup.xdc ========
 */

package ti.uia.sysbios;

/*!
 *  ======== LoggingSetup ========
 *  Module to aid in configuring TI-RTOS logging using UIA and System Analyzer.
 *
 *  @p
 *  The LoggingSetup module automates the process of configuring an application
 *  to use UIA events, and configures TI-RTOS modules to capture user-specified
 *  information such as CPU Load, Task Load and Task Execution so that it can
 *  be displayed by System Analyzer.  It also automates the creation of
 *  infrastructure modules such as loggers, the ServiceManager and RTA modules
 *  to enable the capture and upload of the events over a user-specified
 *  transport.
 *  Both JTAG and Non-JTAG transports are supported.
 *  @p
 *  The following configuration script demonstrates the use of the LoggingSetup
 *  module in the XDC configuration file for the application:
 *
 * @a(Example)
 * Example 1: Configuring an application to use the default settings provided
 * by LoggingSetup.  The following default settings are automatically applied:
 * @p(blist)
 *    - Logging UIA events from user-provided C code.  User provided C code is
 *    treated as part of the xdc.runtime.Main module.  A circular buffer of
 *    size {@link #mainLoggerSize} Bytes is enabled by default to support
 *    this.
 *   - Event logging is enabled for the TI-RTOS Load and Task modules in order
 *   to allow System Analyzer to display CPU Load, and Task
 *   Execution information.  Logging of SWI and HWI events is disabled by
 *   default. In order to optimize event capture and minimize event loss, three
 *   loggers are created: one to store events from the TI-RTOS Load module
 *   another to store events from other TI-RTOS modules, and the third to store user
 *   Log events.
 *   - The events will be logged to {@link ti.uia.loggers.LoggerStopMode LoggerStopMode}
 *     loggers, and will be uploaded over JTAG when the target halts.
 *  @p
 *  @p(code)
 *  // the Log module provides logging APIs for use by the user's software
 *  var Log = xdc.useModule('xdc.runtime.Log');
 *  // the LoggingSetup module's default settings configure much of the UIA infrastructure.
 *  var LoggingSetup = xdc.useModule('ti.uia.sysbios.LoggingSetup');
 *  @p
 *  @p(html)
 *  <hr />
 *  @p
 * @a(Example)
 * Example 2: Use all the default settings of LoggingSetup, except use the
 * minimal footprint logger, LoggerMin. The following settings will be applied:
 * @p(blist)
 *   - Logging UIA events from user-provided C code.
 *   - Event logging is enabled for the TI-RTOS Load and Task modules, and
 *     disabled for SWI and HWI.
 *   - The events will be logged via {@link ti.uia.loggers.LoggerMin LoggerMin},
 *     and will be uploaded over JTAG when the target halts.
 *   - All events will be logged into one buffer whose default sise is
 *     {@link ti.uia.loggers.LoggerMin#bufSize}
 */
metaonly module LoggingSetup
{

    /*!
     *  ======== enableTaskProfiler ========
     *  Enable System Analyzer's Task Profiler
     *
     *  Setting enableTaskProfiler to true enables task, hwi and swi
     *  entry and exit logging in order to generate profiling
     *  information
     */
    metaonly config Bool enableTaskProfiler = false;

    /*!
     *  ======== enableContextAwareFunctionProfiler ========
     *  Enable System Analyzer's Context Aware Function Profiler
     *
     *  Setting to true enables function entry and exit logging
     *  in order to generate function profiling information.
     *  This requires that the compiler's function entry and exit
     *  hook functions be enabled.
     */
    metaonly config Bool enableContextAwareFunctionProfiler = false;

    /*!
     *  ======== memorySectionName ========
     *  Memory section name for the buffer managed by the static instance
     *  (default is null).
     *
     *  By default the local 'dataMemory' in the platform is used (typically
     *  SRAM or L2SRAM).
     *  The available memory section names for your device can be determined by
     *  either looking in the MEMORY CONFIGURATION section of the .map file
     *  that is generated when you build your application or from the Platform
     *  file that your application is using.
     */
    metaonly config String memorySectionName = null;

    /*!
     *  ======== numCores ========
     *  Number of cores running the same image with an instance in shared
     *  memory.
     *
     *  A common use case is to have the same binary image (e.g. .out file)
     *  run on multiple cores of a multi-core device. This causes a problem if
     *  the logger's buffers are in shared memory (e.g. DDR). Since the image is
     *  the same for all the cores, each core will attempt to write to the same
     *  buffer in the shared memory. To avoid this, either place the logger's
     *  buffers in non-shared memory, or set the numCores parameter to
     *  the number of cores on the device.
     *
     *  Note: Use the `{@link #bufSection}` along with the Program.sectMap to
     *  place a logger instance's buffer into specific memory.
     *
     *  Setting numCores to a value greater than 1 signals the logger to
     *  statically set aside additional memory ((x numCores), allowing each
     *  core to have the logger's configured buffer size amount of memory.
     *
     *  Warning: Setting this parameter to a value greater than one should only
     *  be done when there is a single image used on multiple cores of a
     *  multi-core device AND the logger instance's buffer is in shared memory.
     *  While functionally it will still work, memory will be wasted if both
     *  these conditions are not met.
     *
     *  The default is 1, which means do not reserve any additional memory
     *  for the logger.
     */
    metaonly config Int numCores = 1;


    /*!
     *  ======== cpuId=========
     *  CPU ID to put in the packet header as the endpoint ID.
     *
     *  Not required for C6X or C7X devices (uses DNUM).
     */
    metaonly config Int cpuId = 0;

    /*
     *  ======== TimestampSize ========
     *  Enum of size of timestamps to log with events.
     */
    enum TimestampSize {
        TimestampSize_AUTO = 0,    /*! Use the logger's default timestamp size */
        TimestampSize_NONE = 1,    /*! No timestamps logged with events */
        TimestampSize_32b = 2,     /*! 32-bit timestamps */
        TimestampSize_64b = 3      /*! 64-bit timestamps */
    };

    /*!
     *  ======== timestampSize ========
     *  Size of the timestamps logged with the events.  Set to TimestampSize_NONE
     *  to disable timestamps.
     */
    metaonly config TimestampSize timestampSize = TimestampSize_AUTO;

    /*!
     *  ======== loadLogging ========
     *  Enable CPU load event logging.
     *
     *  If loadLogging is false, CPU Load events will be disabled, otherwise the events
     *  will be logged.
     *  Use the {@link #loadLoggingRuntimeControl} parameter
     *  to determine whether the state can be modified during runtime.
     */
    metaonly config Bool loadLogging = true;

    /*!
     *  ======== loadTaskLogging ========
     *  Enable Task load logging.
     *
     *  If this is false, the Task load events will be disabled.
     *  Otherwise the events will be enabled.
     *  Use the {@link #loadLoggingRuntimeControl} parameter
     *  to determine whether the state can be modified during runtime.
     */
    metaonly config Bool loadTaskLogging = false;

    /*!
     *  ======== loadSwiLogging ========
     *  Enable Software Interrupt (Swi) event logging.
     *
     *  If this is false, the Swi load events will be disabled.
     *  Otherwise the events will be enabled.
     *  Use the {@link #loadLoggingRuntimeControl} parameter
     *  to determine whether the state can be modified during runtime.
     */
    metaonly config Bool loadSwiLogging = false;

    /*!
     *  ======== loadHwiLogging ========
     *  Enable Hardware Interrupt (Hwi) event logging.
     *
     *  If this is false, the Hwi load events will be disabled.
     *  Otherwise the events will be enabled.
     *  Use the {@link #loadLoggingRuntimeControl} parameter
     *  to determine whether the state can be modified during runtime.
     */
     metaonly config Bool loadHwiLogging = false;

    /*!
     *  ======== loadLogger ========
     *  User-created logger used for the Load module Log events
     *
     *  Leave null to have LoggingSetup automatically create
     *  the loadLogger if required.
     */
    config xdc.runtime.ILogger.Handle loadLogger = null;

    /*!
     *  ======== loadLoggerSize ========
     *  Size (in MAUs) of the packet used to upload the Load module Log events.
     */
    metaonly config SizeT loadLoggerSize = 512;

    /*!
     *  ======== loadLoggingRuntimeControl ========
     *  Specify whether load logging can be enabled / disabled at runtime.
     *
     *  This determines what {@link xdc.runtime.Diags Diags} settings are
     *  applied to the module's Diags mask.
     *  If 'false', the Diags bits will be configured as
     *  ALWAYS_ON, meaning they can't be changed at runtime. If 'true', the
     *  bits will be configured as 'RUNTIME_ON'.
     *
     *  Use the {@link #loadLogging} parameter
     *  to determine whether the event is ON or OFF. For example, the
     *  following two lines set the Load module's events to 'ALWAYS_ON'.
     *
     *  @p(code)
     *  LoggingSetup.loadLogging = true;
     *  LoggingSetup.loadLoggingRuntimeControl = false;
     *  @p
     */
    metaonly config Bool loadLoggingRuntimeControl = false;

    /*!
     *  ======== mainLogging ========
     *  Enable logging of application code.
     *
     *  If mainLogging is false, application logging will be disabled, otherwise
     *  it will be enabled. Use the {@link #mainLoggingRuntimeControl} parameter
     *  to determine whether the state can be modified during runtime.
     *  Together, these two configuration parameters will be used to
     *  initialize the {@link xdc.runtime.Main} Diags mask.
     *
     *  The table below shows the initial values of the
     *  xdc.runtime.Main Diags mask settings, based on mainLogging and
     *  mainLoggingRuntimeControl configuration. These settins only
     *  apply to Diags masks that have not been set in the application's
     *  configuration file.  For example, this configuration code would
     *  cause xdc.runtime.Main's USER1 Diags mask to be unaffected by
     *  the settings of mainLogging and mainLoggingRuntimeControl:
     *
     *  @p(code)
     *  Diags.setMaskMeta('xdc.runtime.Main', Diags.USER1, Diags.RUNTIME_ON);
     *  @p
     *
     *  @p(html)
     *  <h3> Diags Mask Settings for xdc.runtime.Main </h3>
     *  <table border="1" cellpadding="3">
     *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
     *  </colgroup>
     *    <tr><th colspan="2" rowspan="2">  </th><th colspan="2">  mainLoggingRuntimeControl </th></tr>
     *    <tr><th>  true   </th><th> false </th></tr>
     *    <tr><th rowspan="2">  mainLogging  </th><th>  true  </th><td> RUNTIME_ON</td>
     *    <td> ALWAYS_ON </td></tr>
     *    <tr><th> false </th><td>  RUNTIME_OFF  </td> <td> ALWAYS_OFF </td></tr>
     * </table>
     */
    metaonly config Bool mainLogging = true;

    /*!
     *  ======== mainLogger ========
     *  Logger used for application Log events
     *
     *  Leave null to have LoggingSetup automatically create
     *  the mainLogger if required.
     */
    config xdc.runtime.ILogger.Handle mainLogger = null;

    /*!
     *  ======== mainLoggerSize ========
     *  Size (in MAUs) of packet used for application Log
     *  events.
     */
    metaonly config SizeT mainLoggerSize = 1024;

    /*!
     *  ======== mainLoggingRuntimeControl ========
     *  Specify whether application logging can be enabled / disabled
     *  at runtime.
     *
     *  This parameter, together with {@link #mainLogging}, determine the
     *  initial {@link xdc.runtime.Diags} settings applied to
     *  {@link xdc.runtime.Main}.
     *  If 'false' and {@link #mainLogging} is true, the diags bits will
     *  be configured as ALWAYS_ON, meaning they can't be changed at runtime.
     *  If 'true', the bits will be configured as 'RUNTIME_ON'.
     *
     *  Use the {@link #mainLogging} parameter
     *  to determine whether the event is ON or OFF. For example, the
     *  following two lines set the xdc.runtime.Main events to
     *  initially be 'RUNTIME_ON'.
     *
     *  @p(code)
     *  LoggingSetup.mainLogging = true;
     *  LoggingSetup.mainLoggingRuntimeControl = true;
     *  @p
     */
    metaonly config Bool mainLoggingRuntimeControl = true;

    /*!
     *  ======== sysbiosLogger ========
     *  Logger used for SYSBIOS modules' Log events
     *
     *  Leave null to have LoggingSetup automatically create
     *  the sysbiosLogger if required.
     */
    config xdc.runtime.ILogger.Handle sysbiosLogger = null;

    /*!
     *  ======== sysbiosLoggerSize ========
     *  Size (in MAUs) of the packet used for the TI-RTOS modules' Log events.
     */
    metaonly config SizeT sysbiosLoggerSize = 1024;

    /*!
     *  ======== sysbiosTaskLogging ========
     *  Enable the SYSBIOS Task module's event logging.
     *
     *  If false, Task events will be disabled, otherwise the events
     *  will be enabled.
     *  Use the {@link #sysbiosTaskLoggingRuntimeControl} parameter
     *  to determine whether the state can be modified during runtime.
     */
    metaonly config Bool sysbiosTaskLogging = true;

    /*!
     *  ======== sysbiosTaskLoggingRuntimeControl ========
     *  Specify whether Task logging can be enabled / disabled at runtime.
     *
     *  This parameter, together with {@link #sysbiosTaskLogging}, determine the
     *  initial {@link xdc.runtime.Diags Diags} settings applied to
     *  the Task module. For example, the following two
     *  lines initialize the Task module's events to
     *  'ALWAYS_ON'.
     *
     *  @p(code)
     *  LoggingSetup.sysbiosTaskLogging = true;
     *  LoggingSetup.sysbiosTaskLoggingRuntimeControl = false;
     *  @p
     */
    metaonly config Bool sysbiosTaskLoggingRuntimeControl = true;

    /*!
     *  ======== sysbiosSwiLogging ========
     *  Enable SYSBIOS Swi module's event logging.
     *
     *  If false, Swi events will be disabled, otherwise the events
     *  will be enabled.
     *  Use the {@link #sysbiosSwiLoggingRuntimeControl} parameter
     *  to determine whether the state can be modified during runtime.
     */
    metaonly config Bool sysbiosSwiLogging = false;

    /*!
     *  ======== sysbiosSwiLoggingRuntimeControl ========
     *  Specify whether Swi logging can be enabled / disabled at runtime.
     *
     *  This parameter, together with {@link #sysbiosSwiLogging}, determine the
     *  initial {@link xdc.runtime.Diags Diags} settings applied to
     *  the SYSBIOS Swi module. For example, the following two
     *  lines initialize the Swi module's events to 'RUNTIME_OFF'.
     *
     *  @p(code)
     *  LoggingSetup.sysbiosSwiLogging = false;
     *  LoggingSetup.sysbiosSwiLoggingRuntimeControl = true;
     *  @p
     */
    metaonly config Bool sysbiosSwiLoggingRuntimeControl = false;

    /*!
     *  ======== sysbiosHwiLogging ========
     *  Enable the SYSBIOS Hwi and Clock modules' event logging.
     *
     *  If false, Hwi and Clock events will be disabled, otherwise the events
     *  will be enabled.
     *  Use the {@link #sysbiosHwiLoggingRuntimeControl} parameter
     *  to determine whether the state can be modified during runtime.
     */
    metaonly config Bool sysbiosHwiLogging = false;

    /*!
     *  ======== sysbiosHwiLoggingRuntimeControl ========
     *  Specify whether Hwi and Clock logging can be enabled / disabled at
     *  runtime.
     *
     *  This parameter, together with {@link #sysbiosHwiLogging}, determine the
     *  initial {@link xdc.runtime.Diags Diags} settings applied to
     *  the SYSBIOS Hwi and Clock modules. For example, the following two
     *  lines set the Hwi and Clock modules' events to 'ALWAYS_ON'.
     *
     *  @p(code)
     *  LoggingSetup.sysbiosHwiLogging = true;
     *  LoggingSetup.sysbiosHwiLoggingRuntimeControl = false;
     *  @p
     */
    metaonly config Bool sysbiosHwiLoggingRuntimeControl = false;

    /*!
     *  ======== sysbiosSemaphoreLogging ========
     *  Enable the SYSBIOS Semaphore module's event logging.
     *
     *  If false, Semaphore events will be disabled, otherwise the events
     *  will be enabled.
     *  Use the {@link #sysbiosSemaphoreLoggingRuntimeControl} parameter
     *  to determine whether the state can be modified during runtime.
     */
    metaonly config Bool sysbiosSemaphoreLogging = false;

    /*!
     *  ======== sysbiosSemaphoreLoggingRuntimeControl ========
     *  Specify whether Semaphore logging can be enabled / disabled at runtime.
     *
     *  This parameter, together with {@link #sysbiosSemaphoreLogging}, determine the
     *  initial {@link xdc.runtime.Diags Diags} settings applied to
     *  the SYSBIOS Semaphore module. For example, the following code
     *  initializes the Semaphore module's events to 'RUNTIME_OFF'.
     *
     *  @p(code)
     *  LoggingSetup.sysbiosSemaphoreLogging= false;
     *  LoggingSetup.sysbiosSemaphoreLoggingRuntimeControl = true;
     *  @p
     */
    metaonly config Bool sysbiosSemaphoreLoggingRuntimeControl = false;

    /*!
     *  ======== profileLogging ========
     *  Enable logging of function profiling events.
     *
     *  If this is false, the events will be disabled. Otherwise the events
     *  will be enabled.
     */
    metaonly config Bool profileLogging = false;

    /*!
     *  ======== countingAndGraphingLogging ========
     *  Enable logging of counting and graphing events.
     *
     *  If this is false, the events will be disabled. Otherwise the events
     *  will be enabled.
     */
    metaonly config Bool countingAndGraphingLogging = false;

    /*!
     *  ======== benchmarkLogging ========
     *  Enable logging of benchmark events.
     *
     *  If this is false, the events will be disabled. Otherwise the events
     *  will be enabled.
     */
    metaonly config Bool benchmarkLogging = false;

    /*!
     *  ======== snapshotLogging ========
     *  Enable logging of snapshot events.
     *
     *  If this is false, the events will be disabled. Otherwise the events
     *  will be enabled.
     */
    metaonly config Bool snapshotLogging = false;

    /*!
     * @_nodoc
     *  ======== overflowLoggerSize ========
     *  Overflow logger is no longer used.
     *
     *  Size of logger used for overflow events when
     *  uploadMode is either JTAGRUNMODE or NONJTAG_AND_JTAGSTOPMODE
     */
    metaonly config SizeT overflowLoggerSize = 1024;

    /*!
     * @_nodoc
     *  ======== UploadMode ========
     *  Keep for backwards compatibility with 1.04 LoggingSetup.
     */
    enum UploadMode {
        UploadMode_SIMULATOR = 1,
        UploadMode_PROBEPOINT = 2,
        UploadMode_JTAGSTOPMODE = 3,
        UploadMode_JTAGRUNMODE = 4,
        UploadMode_NONJTAGTRANSPORT = 5,
        UploadMode_NONJTAG_AND_JTAGSTOPMODE = 6,
        UploadMode_STREAMER = 7,
        UploadMode_IDLE = 8,
        UploadMode_STREAMER2 = 9
    };

    /*!
     *  @_nodoc
     *  ========= eventUploadMode ========
     *  Keep for backwards compatibility with 1.04 LoggingSetup.
     */
    metaonly config UploadMode eventUploadMode = UploadMode_JTAGSTOPMODE;

    /*!
     *  ======== LoggerType ========
     *  Enum of the type of loggers that can be used.
     */
    metaonly enum LoggerType {
        LoggerType_MIN,         /*! LoggerMin - Minimal footprint logger
                                 * (JTAG only) */
        LoggerType_STOPMODE,    /*! LoggerStopMode (JTAG only) */
        LoggerType_JTAGRUNMODE, /*! LoggerRunMode JTAG (c6x only) */
        LoggerType_RUNMODE,     /*! LoggerRunMode ETHERNET upload */
        LoggerType_IDLE,        /*! LoggerIdle - Upload in Idle loop */

        /*
         *  LoggingSetup cannot create LoggerStreamer2 loggers, since the
         *  configuration parameters, exchangeFxn and primeFxn cannot be
         *  determined.  Therefore, we don't want this to show up in the
         *  list of choices.
         */
        LoggerType_STREAMER2    /*! LoggerStreamer2 - Application manages
                                 * logger buffers */
    };

    /*!
     * ======== loggerType ========
     * Configures the type of logger to use.
     */
    metaonly config LoggerType loggerType = LoggerType_STOPMODE;

    /*!
     * ======== multicoreEventCorrelation ========
     * Set to true for multi-core applications.
     *
     * When set to true, the LoggingSetup module will automatically
     * include the {@link ti.uia.runtime.LogSync LogSync} module.  The LogSync module is
     * required for events from multiple CPU cores to be correlated with each other.
     * @see ti.uia.runtime.LogSync
     *
     */
    metaonly config Bool multicoreEventCorrelation = false;

    /*!
     *  @_nodoc
     * ======== disableMulticoreEventCorrelation ========
     *  Keep for backwards compatibility with 1.04 LoggingSetup.
     */
    metaonly config Bool disableMulticoreEventCorrelation;

    /*!
     *  ======== maxEventSize ========
     *  For Snapshot events with loggers that support the writeMemoryRange API,
     *  this specifies the maximum event size (in Maus) that
     *  can be written with a single event. Must be less than or equal to
     *  the logger's buffer size - 64.
     *
     *  The logger's writeMemoryRange API checks to see if the event size required to
     *  write the block of memory is larger than maxEventSize.  If so, it will
     *  split the memory range up into a number of smaller blocks and log the
     *  blocks using separate events with a common snapshot ID in order to
     *  allow the events to be collated and the original memory block to be
     *  reconstructed on the host.
     */
    config SizeT maxEventSize = 128;

    /*!
     *  ======== syncLoggerSize ========
     *  Size (in MAUs) of the logger used for the {@link ti.uia.runtime.LogSync LogSync}
     *  modules' sync point events.
     */
    metaonly config SizeT syncLoggerSize = ti.uia.runtime.LogSync.defaultSyncLoggerSize;

    /*! @_nodoc
     * ======== createLogger =========
     * Internal helper function that creates the type of logger
     * appropriate for the LoggingSetup.uploadMode that has been configured.
     *
     * @param(loggerSize): the size of the logger in MAUs
     * @param(loggerInstanceName): the name to assign to the logger instance
     * @param (loggerPriority): the IUIATransfer.Priority to assign to the
     * logger instance
     *
     * @a(return) returns the logger instance that was created
     */
    metaonly function createLogger(loggerSize, loggerInstanceName,
            loggerPriority);

    /*! @_nodoc
     *  ======== writeUIAMetaData ========
     *  Writes any UIA metadata required to support the module
     *
     *  NOTE: This should only be called by ti.uia.runtime.UIAMetaData.xs in
     *  order to ensure that the UIA xml file has been properly opened and is
     *  ready for writing.
     *  @param(indent) the number of spaces to preceed the xml tag with
     */
    metaonly function writeUIAMetaData(indent);

    /*! @_nodoc
     *  ======== showMulticoreEventCorrelationOption ========
     *  READ ONLY - used by XGCONF / LoggingSetup.xml
     */
    metaonly config Bool showMulticoreEventCorrelationOption = false;

    /*! @_nodoc
     *  ======== isSysbiosLoggerAutoCreated ========
     *  READ ONLY - used by XGCONF / LoggingSetup.xml
     */
    metaonly config Bool isSysbiosLoggerAutoCreated = true;

    /*! @_nodoc
     *  ======== isLoadLoggerAutoCreated ========
     *  READ ONLY - used by XGCONF / LoggingSetup.xml
     */
    metaonly config Bool isLoadLoggerAutoCreated = true;

    /*! @_nodoc
     *  ======== isMainLoggerAutoCreated ========
     *  READ ONLY - used by XGCONF / LoggingSetup.xml
     */
    metaonly config Bool isMainLoggerAutoCreated = true;

    /*! @_nodoc
     *  ======== showNumCoresOption ========
     *  READ ONLY - used by XGCONF / LoggingSetup.xml
     */
    metaonly config Bool showNumCoresOption = false;
}
