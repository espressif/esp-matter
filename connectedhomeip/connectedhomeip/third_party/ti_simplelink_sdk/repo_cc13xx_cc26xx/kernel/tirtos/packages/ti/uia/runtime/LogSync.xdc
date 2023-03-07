/*
 * Copyright (c) 2013-2016, Texas Instruments Incorporated
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
 *  ======== LogSync.xdc ========
 */
package ti.uia.runtime;
import xdc.runtime.Types;
import xdc.runtime.ILogger;
import xdc.runtime.IHeap;
import xdc.runtime.ILogger;
import xdc.runtime.Diags;
import xdc.runtime.Text;
import ti.uia.events.IUIAMetaProvider;
import xdc.rov.ViewInfo;

/*!
 *  ======== LogSync ========
 *  SyncPoint Event logging module for logging sync point events.  Allows sync
 *  point events to use a different logger instance than used for other events.
 *
 * SyncPoint events are used to log timestamp values for two timebases: the
 * local CPU timestamp that is used to timestamp events from this CPU, and a
 * 'global' timestamp value that can be accessed by two or more CPUs.
 * By logging the current timestamp values from these two timebase sources,
 * the sync point events provide correlation points between the two timebases.
 *
 * In order to allow sync point information to be injected into hardware trace
 * streams, the LogSync module supports a configuration parameter named
 * injectIntoTraceFxn that allows the user to hook in a function pointer
 * to a function that handles the (ISA specific) details of injecting whatever
 * information is required into the trace stream.  For C64X+ full gem devices,
 * the address of the ti.uia.family.c64p.GemTraceSync module's
 * GemTraceSync_injectIntoTrace function should be used.
 *
 * The sync point events are defined in the ti.uia.events.UIASync module
 * (@see ti.uia.events.UIASync#syncPoint)
 *
 * A unique 'serial number' is assigned to each sync point event that is logged.
 * The same serial number is logged as a parameter for all UIASync events that are
 * used to log information related to the sync point, allowing host-side tooling to
 * treat these separate events coherently.  The serial number can optionally be injected
 * into device-specific trace streams (e.g. CPU trace, System Trace, etc.) in order
 * to enable host-side tooling to correlate these separate streams with the CPU and
 * global timestamp information logged with the sync point events.
 *
 * @a(Examples)
 * Example 1: This is part of the XDC configuration file for the application
 *  that demonstrates a standard configuration using default settings.  In this
 *  example, the Rta module internally handles the logging of the sync point
 *  events.  A timestamp module that implements the IUIATimestampProvider
 *  interface is used for the global timestamp.  Default values are used for
 *  the CPU maxCpuClockFreq (700 MHz).
 *
 *  @p(code)
 * // By including Rta, Log records will be collected and sent to the
 * // instrumentation host (once it is connected and started).  The Rta module
 * // logs sync point events upon receiving either the start or stop command,
 * // and prior to sending up a new event packet if
 * // LogSync_isSyncPointEventRequired() returns true.
 * var Rta  = xdc.useModule('ti.uia.services.Rta');
 *
 * // By default, the sync point events will be logged to a dedicated
 * // LoggerCircBuf buffer named 'SyncLog' that is assigned to the LogSync
 * // module.  Using a dedicated event logger buffer is recommended
 * // in order to ensure that sufficient timing information
 * // is captured to enable accurate multicore event correlation.
 * // Configure LogSync.defaultSyncLoggerSize to specify a custom buffer size.
 * var LogSync = xdc.useModule('ti.uia.runtime.LogSync');
 *
 * // For C64X+ and C66X devices that provide CPU trace hardware capabilities,
 * // the following line will enable injection of correlation information into
 * // the GEM CPU trace, enabling correlation of software events with the CPU
 * // trace events.
 * var GemTraceSync = xdc.useModule('ti.uia.family.c64p.GemTraceSync');
 *
 * // Configure a shared timer to act as a global time reference to enable
 * // multicore correlation.  The TimestampC6472Timer module implements the
 * // IUIATimestampProvider interface, so assigning this timer to
 * // LogSync.GlobalTimestampProxy will configure the LogSync module's global
 * // clock parameters automatically.  Exmaple 2 shows how to use other
 * // types of timers.
 * var TimestampC6472Timer =
 *    xdc.useModule('ti.uia.family.c64p.TimestampC6472Timer');
 * LogSync.GlobalTimestampProxy = TimestampC6472Timer;
 * @p(html)
 * <hr />
 * @p
 *
 * Example 2: Using a timer that does not implement the IUIATimestampProvider
 * interface as the global timestamp timer.  This example shows how to use,
 * for example, timers that are provided by DSP/BIOS as the global timer
 * source for event correlation 'sync point' timestamps.
 * @p(code)
 *
 * var LogSync = xdc.useModule('ti.uia.runtime.LogSync');
 * var BiosTimer = xdc.useModule('ti.sysbios.family.c64p.TimestampProvider');
 * LogSync.GlobalTimestampProxy = BiosTimer;
 *
 * // The following additional configuration code is required to use
 * // a timer that does not implement the IUIATimeestampProvider interface
 * // as the global timer for the LogSync module.  If the maxGlobalClockFreq
 * // config option is not initialized, the following warning message will be displayed
 * // at build time: "Warning: UIA Event correlation disabled.  Please
 * // configure LogSync.globalClkFreq (.lo,.hi) to a non-zero value to enable."
 * LogSync.maxGlobalClockFreq.lo = 700000000; // frequency in Hz - lower 32b
 * LogSync.maxGlobalClockFreq.hi = 0;         // frequency in Hz - upper 32b
 *
 * // Configure the LogSync module with CPU timestamp clock frequency info
 * // for clock frequencies other than the default (700MHz).
 * LogSync.maxCpuClockFreq.lo = 1000000000; // 1GHz CPU freq. - lower 32b
 * LogSync.maxCpuClockFreq.hi = 0;         // 1GHz CPU freq.- upper 32b
 *
 * // The globalTimestampCpuCyclesPerTick config option is optional.
 * // It is used to convert global timestamp tick counts into CPU cycle counts
 * // for devices where there is a fixed relationship between the global timer
 * // frequency and the CPU clock.
 * LogSync.globalTimestampCpuCyclesPerTick = 6;
 * @p(html)
 * <hr />
 * @p *
 * Example 3: Disabling LogSync module at configuation time
 * The logging of sync point events can be disabled by adding the following
 * to the configuration script:
 * @p(code)
 * LogSync.isEnabled = false;
 * @p(html)
 * <hr />
 * @p *
 * Example 4:  This is a part of the C code for an application that does
 *  not use the Rta module, and so needs to log the sync point events itself:
 *
 *  @p(code)
 *  #include <ti/uia/runtime/LogSync.h>
 *  ...
 *
 * // If the target has been suspended or halted
 * // since the last time an event packet was sent to the
 * // host, or the event transport has received a 'start' or
 * // 'stop' command from the host, log a new sync point event to record
 * // the current correlation info between the local
 * // timestamp and the global timestamp.
 * if ((LogSync_isSyncEventRequired())||( [starting/stopping event transport])){
 *    LogSync_writeSyncPoint();
 * }
 * @p
 * <hr />
 * @p *
 * Example 5:  The following configuration script snippet shows how to periodically
 * log a sync point event.  This allows System Analyzer to properly correlate UIA software
 * instrumentation events with C6X CPU trace and STM (System Trace) events.
 *
 *  @p(code)
 * //Configure a Timer to interrupt every 100ms and call the LogSync_timerHook
 * // function to log a sync point event
 * var Timer = xdc.useModule('ti.sysbios.hal.Timer');
 * var timerParams = new Timer.Params();
 * timerParams.startMode = Timer.StartMode_AUTO;
 * timerParams.period = 100000;        // 100,000 uSecs = 100ms
 * var timer0 = Timer.create(Timer.ANY, '&ti_uia_runtime_LogSync_timerHook', timerParams);
 * @p
 */
@ModuleStartup      /* Initialize static instances */
@CustomHeader
module LogSync inherits ti.uia.runtime.IUIATraceSyncClient {

    /*!
     *  @_nodoc
     *  ======== ModuleView ========
     */
    metaonly struct ModuleView {
        UInt32 numTimesHalted;
        UInt32 serialNumber;
        Bool isEnabled;
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
        ]
        ]
    });

    /*! @_nodoc
     * ======== LoggerType ========
     * Enum of the type of loggers that can be used to log sync point events.
     */
    metaonly enum LoggerType {
        LoggerType_NONE,
        LoggerType_MIN,
        LoggerType_STOPMODE,
        LoggerType_RUNMODE,
        LoggerType_IDLE,
        LoggerType_STREAMER,
        LoggerType_STREAMER2
    };

    /*! @_nodoc
     * ======== loggerType ========
     * Configures the preferred type of logger to use to log sync point events
     */
    metaonly config LoggerType loggerType = LoggerType_NONE;


    /*!
     * ======== syncLogger ========
     * Configures the logger instance to use to log sync point events
     *
     * If left null, an instance of LoggerStopMode will be created for
     * dedicated use by the LogSync module in order to log sync point events.
     * (The ti.uia.services.Rta and ti.uia.sysbios.LoggerSetup modules
     * can specify that the LoggerCircBuf module be used as the default
     * if the user has specified a non-JTAG transport for event upload.)
     */
    metaonly config xdc.runtime.ILogger.Handle syncLogger;

    /*!
     * ======== defaultSyncLoggerSize ========
     * Configures the size of the default syncLogger created by LogSync
     *
     * Only used if syncLogger is null.
     */
    metaonly config SizeT defaultSyncLoggerSize = 256;

    /*!
     * ======== isEnabled ========
     * Configures whether sync logging is enabled (true) or disabled (false)
     */
    metaonly config Bool isEnabled = true;

    /*!
     *  ======== CpuTimestampProxy ========
     *  CPU Timestamp Proxy
     *
     *  This proxy provides a timestamp server that can be different
     *  from the one provided by `{@link xdc.runtime.Timestamp}`. However, if
     *  not supplied by a user, this proxy defaults to whichever timestamp
     *  server is provided by `xdc.runtime.Timestamp`.
     *  @p
     *  Configuring the CpuTimestampProxy with a local timestamp module
     *  allows applications that change the CPU frequency to report this
     *  information to System Analyzer so that event timestamps can
     *  be adjusted to accommodate the change in frequency.
     *  @a(Examples)
     *  Example: the following configuration script shows how to configure
     *  a C66X Local Timestamp module for use as the CpuTimestampProxy
     * @p(code)
     * var TimestampC66Local = xdc.useModule('ti.uia.family.c66.TimestampC66Local');
     * TimestampC66Local.maxTimerClockFreq = {lo:1200000000,hi:0};
     * var LogSync = xdc.useModule('ti.uia.runtime.LogSync');
     * LogSync.CpuTimestampProxy = TimestampC66Local;
     * @p
     */
    proxy CpuTimestampProxy inherits xdc.runtime.ITimestampClient;
    /*!
     * ======== cpuTimestampCyclesPerTick ========
     * The number of CPU cycles each tick of the global timestamp corresponds to.
     *    0 if no relation between clocks.
     *
     * If the module configured as the CpuTimestampProxy implements
     * ti.uia.runtime.IUIATimestampProvider, the default value of this config
     * option is derived at configuration time from that module's config data.
     * Otherwise it is initialized to 0 to signify that there is no way to
     * convert a number of global timestamp tick counts into an equivalent
     * number of CPU cycles.
     */
    config UInt32 cpuTimestampCyclesPerTick = 1;

    /*!
     * ======== maxCpuClockFreq =========
     * The highest bus clock frequency used to drive the timer.
     *
     * The default ticks per second rate of the timer is calculated by dividing
     * the timer's bus clock frequency by the cpuTimestampCyclesPerTick
     * config parameter.
     *
     * Defines the 32 MSBs of the highest bus clock frequency used to drive
     * the timer.
     */
    metaonly config Types.FreqHz maxCpuClockFreq;

    /*!
     * ======== canCpuFrequencyBeChanged =========
     * Indicates whether the timer frequency can be changed or not
     *
     * Set to true if the timer's clock frequency can be changed
     */
    metaonly config Bool canCpuFrequencyBeChanged = false;


    /*!
     * ======== canCpuCyclesPerTickBeChanged =========
     * Indicates whether the CPU timer's cycles per tick divide down ratio can
     *    be changed or not
     *
     * Set to true if the timer's CPU cycles per tick can be changed
     */
    metaonly config Bool canCpuCyclesPerTickBeChanged = false;

    /*!
     *  ======== GlobalTimestampProxy ========
     *  Global Timestamp Proxy
     *
     *  This proxy provides a timestamp server that can be different
     *  from the server provided by `{@link xdc.runtime.Timestamp}`.
     *  This must be configured in order to use this module.
     */
    proxy GlobalTimestampProxy inherits xdc.runtime.ITimestampClient;

    /*!
     * ======== globalTimestampCyclesPerTick ========
     * The number of CPU cycles each tick of the global timestamp corresponds
     *    to.  0 if no relation between clocks.
     *
     * A value of 0 signifies that there is no way to convert a number of
     * global timestamp tick counts into an equivalent number of CPU cycles.
     * Note that this value will be automatically copied from the
     * GlobalTimestampProxy.cpuCyclesPerTick configuration value
     * at configuration time if GlobalTimestampProxy.cpuCyclesPerTick > 0.
     */
    config UInt32 globalTimestampCpuCyclesPerTick = 0;

    /*!
     * ======== maxGlobalClockFreq =========
     * The highest bus clock frequency used to drive the timer used for the global
     *  timestamp.
     *
     * The default ticks per second rate of the timer is calculated by dividing
     * the timer's bus clock frequency by the globalTimestampCpuCyclesPerTick
     * config parameter.
     *
     * Defines the highest bus clock frequency used to drive the shared timer used
     * for the global timestamp.
     */
    config Types.FreqHz maxGlobalClockFreq;

    /*!
     *  ======== enable ========
     *  Enables logging of sync point events
     *
     *  @a(returns)
     *  The function returns the state of the module-level enable (`TRUE`
     *  if enabled,`FALSE` if disabled) before the call. This return value
     *  allows clients to restore the previous state.
     *  Note: not thread safe.
     */

    @DirectCall
    Bool enable();

    /*!
     *  ======== disable ========
     *  Disable logging of sync point events
     *
     *  @a(returns)
     *  The function returns the state of the module-level enable (`TRUE`
     *  if enabled,`FALSE` if disabled) before the call. This return value
     *  allows clients to restore the previous state.
     *  Note: not thread safe.
     */
    @DirectCall
    Bool disable();

    /*!
     *  ======== LogSync_idleHook ========
     *  Hook function that can be called by SysBios when the Idle function.
     *  Logs a sync point event if required in order to enable multicore event correlation.
     *  Allows multicore event correlation to be re-established after the target
     *  has been halted and then resumed execution.  (e.g. after CIO operation or breakpoint)
     */
    @DirectCall
    Void idleHook();

    /*!
     *  ======== LogSync_timerHook ========
     *  Hook function that can be called periodically by SysBios to enable correlation
     *  of CPU trace, STM trace and software instrumentation events.
     */
    @DirectCall
    Void timerHook(UArg arg);

    /*!
     *  ======== putSyncPoint ========
     *  Unconditionally put the specified `Types` event.
     *
     *  This method unconditionally logs a sync point event. It is used
     *  internally by the writeSyncPoint() macro and typically should not be
     *  called directly.
     *
     */
    @DirectCall
    Void putSyncPoint();

    /*!
     *  ======== writeSyncPoint ========
     *  Log a sync point event along with global timestamp, local CPU frequency
     *     and sync point serial number.
     *
     *  This method logs a synchronization point event, local CPU timestamp and
     *  global timestamp into the log along with the fmt string a sync point
     *  serial number
     *
     *  @param(fmt)   a constant string that describes why the sync point was
     *     logged.
     */
    @Macro Void writeSyncPoint();

    /*
     *  ======== writeSyncPointRaw ========
     *  Write log sync events using given timestamps.
     */
    Void writeSyncPointRaw(const Types.Timestamp64 *cpuTS,
        const Types.Timestamp64 *globalTS, const Types.FreqHz *globalTickFreq);

    /*!
     * ======== isSyncEventRequired ========
     * Is Sync Event Required
     *
     * Checks whether the target has been halted since the
     * last sync point event and returns true if it has.
     *
     * @a(return) true if a synchronization event should be logged
     */
    @DirectCall
    Bool isSyncEventRequired();


    /*!
     * ======== enableEventCorrelationForJTAG ========
     * Enable event correlation for JTAG Transports (deprecated)
     *
     * By default, event correlation is enabled for JTAG transports.
     * In order for event correlation to work with JTAG transports,
     * it is necessary for the target program to periodically execute
     * LogSync_idleHook in order to log enough synchronization information
     * to reestablish event correlation after a breakpoint has been hit.
     * The following .cfg script snippet shows how to configure the
     * ti.uia.sysbios.LoggingSetup module to enable this:
     * @p(code)
     * .cfg script:
     * var LoggingSetup = xdc.useModule('ti.uia.sysbios.LoggingSetup');
     * @p
     * Since most JTAG debugging sessions start with a breakpoint being
     * hit at main, event correlation will only be possible once a sync
     * point event has been logged after running from main.  Calling the
     * following code snippet as part of the code that is run by main is
     * highly recommended in order to establish synchronization information
     * as early in the program's execution as possible.
     *
     * @p(code)
     * C code:
     * #include <xdc/std.h>
     * #include <ti/uia/runtime/LogSync.h>
     * ...
     *
     *    if (LogSync_isSyncEventRequired()){
     *       LogSync_writeSyncPoint();
     *    }
     * @p
     */
    metaonly config Bool enableEventCorrelationForJTAG = true;

    /*! @_nodoc
     * ======== hasMetaData ========
     * Indicates that the LogSync module generates content for the uia.xml file.
     */
    override metaonly config Bool hasMetaData = true;

    /*! @_nodoc
     * ======== finalize ========
     * get configured clock settings from timer modules and configure logger to log
     * sync events with
     */
    metaonly function finalize();

    /*! @_nodoc
     * ======== isUsedByRta ========
     * Called by the RTA module to indicate that it is in the .cfg file
     *
     * Sets an internal metaonly flag that helps select the appropriate type of logger
     */
    metaonly function isUsedByRta();

    /*========================================================================*/
    instance:

    internal:
    struct Module_State {
        UInt32 numTimesHalted;
        UInt32 serialNumber;
        Bool isEnabled;
    };

}
