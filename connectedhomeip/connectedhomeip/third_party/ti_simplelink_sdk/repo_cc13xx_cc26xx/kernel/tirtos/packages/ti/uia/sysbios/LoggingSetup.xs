/*
 * Copyright (c) 2013-2017, Texas Instruments Incorporated
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
 */

/*
 *  ======== LoggingSetup.xs ========
 */

var LoggingSetup = null;

var Logger = null;
var Load = null;

var Diags = null;
var Main = null;
var Registry = null;
var Timestamp = null;

var BIOS = null;
var Load = null;
var Clock = null;
var Event = null;
var Semaphore = null;
var Task = null;

var UIAMetaData = null;
var LogSnapshot = null;
var UIABenchmark = null;
var UIAProfile = null;
var UIAEvt = null;
var UIAErr = null;
var LogSnapshot = null;
var LogSyncModule = null;
var LogSync = null;

var isEnableDebugPrintf = false;
var listOfLoggersCreated = [];

/*
 *  ======== dbgPrint ========
 */
function dbgPrint(str)
{
    if (isEnableDebugPrintf) {
        print(str);
    }
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    LoggingSetup = this;
    BIOS = xdc.useModule('ti.sysbios.BIOS');

    if (BIOS.logsEnabled == false) {
        LoggingSetup.$logWarning("BIOS.logsEnabled must be set to true " +
                "when using RTOS Analyzer.  Please edit " +
                "the .cfg file and set BIOS.logsEnabled to true.",
                BIOS, "logsEnabled");
    }

    /* Check for deprecated config */
    if (LoggingSetup.$written("eventUploadMode")) {
        LoggingSetup.$logWarning("The LoggingSetup.eventUploadMode configuration " +
                "parameter has been replaced with LoggingSetup.loggerType.  Will " +
                "attempt to set LoggingSetup.loggerType based on eventUploadMode.",
                LoggingSetup, "eventUploadMode");

        switch (LoggingSetup.eventUploadMode) {
            case LoggingSetup.UploadMode_SIMULATOR:
            case LoggingSetup.UploadMode_PROBEPOINT:
            case LoggingSetup.UploadMode_JTAGSTOPMODE:
                LoggingSetup.loggerType = LoggingSetup.LoggerType_STOPMODE;
                break;

            case LoggingSetup.UploadMode_JTAGRUNMODE:
                LoggingSetup.loggerType = LoggingSetup.LoggerType_JTAGRUNMODE;
                break;

            case LoggingSetup.UploadMode_NONJTAGTRANSPORT:
            case LoggingSetup.UploadMode_NONJTAG_AND_JTAGSTOPMODE:
                LoggingSetup.loggerType = LoggingSetup.LoggerType_RUNMODE;
                break;

            case LoggingSetup.UploadMode_IDLE:
                LoggingSetup.loggerType = LoggingSetup.LoggerType_IDLE;
                break;

            case LoggingSetup.UploadMode_STREAMER2:
                LoggingSetup.loggerType = LoggingSetup.LoggerType_STREAMER2;
                break;

            case LoggingSetup.UploadMode_STREAMER:
                LoggingSetup.$logError("LoggingSetup no longer supports LoggerStreamer",
                        LoggingSetup, "eventUploadMode");
                break;

            default:
                LoggingSetup.$logError("LoggingSetup: Unsupported upload mode ",
                        LoggingSetup, "eventUploadMode");
                break;

        }
    }

    if (LoggingSetup.$written("disableMulticoreEventCorrelation")) {
        LoggingSetup.$logWarning("The LoggingSetup.disableMulticoreEventCorrelation " +
                "configuration parameter " +
            "has been deprecated.  Use LoggingSetup.multicoreEventCorrelation.",
                LoggingSetup, "disableMulticoreEventCorrelation");
        if (LoggingSetup.disableMulticoreEventCorrelation == false) {
            LoggingSetup.multicoreEventCorrelation = true;
        }
    }

    if (LoggingSetup.memorySectionName != null) {
        Program.sectMap[".uiaLoggerBuffers"] = new Program.SectionSpec();
        Program.sectMap[".uiaLoggerBuffers"].loadSegment = LoggingSetup.memorySectionName;
        dbgPrint("LoggingSetup: .bufSection = " + LoggingSetup.memorySectionName);
    }

    UIAMetaData = xdc.useModule('ti.uia.runtime.UIAMetaData');

    if (LoggingSetup.timestampSize != LoggingSetup.TimestampSize_NONE){
        Timestamp = xdc.module('xdc.runtime.Timestamp');
    }
    Diags = xdc.useModule('xdc.runtime.Diags');

    if (LoggingSetup.snapshotLogging) {
        LogSnapshot = xdc.useModule('ti.uia.runtime.LogSnapshot');
    }

    if (LoggingSetup.benchmarkLogging) {
        UIABenchmark = xdc.useModule('ti.uia.events.UIABenchmark');
    }

    if (LoggingSetup.profileLogging || LoggingSetup.enableContextAwareFunctionProfiler) {
        UIAProfile = xdc.useModule('ti.uia.events.UIAProfile');
        /*  Ensure that UIAProfile generates the hook functions if the user has enabled
         *  LoggingSetup.enableContextAwareFunctionProfiler, otherwise allow user to
         *  configure whether they want the hook functions via the UIAProfile config
         *  parameters
         */
        if (LoggingSetup.enableContextAwareFunctionProfiler) {
            UIAProfile.isContextAwareProfilingEnabled = true;
        }
    }

    if ((LoggingSetup.mainLogging) || (LoggingSetup.countingAndGraphingLogging)) {
        /* Use the most common event defintion modules */
        UIAEvt = xdc.useModule('ti.uia.events.UIAEvt');
        UIAErr = xdc.useModule('ti.uia.events.UIAErr');
    }

    UIAMetaData.setLoggingSetupConfigured();

    if (Program.cpu.catalogName.match(/6000/)) {
        LoggingSetup.showNumCoresOption = true;
    } else {
        LoggingSetup.showNumCoresOption = false;
    }

    dbgPrint("LoggingSetup: Program.cpu.catalogName = " + Program.cpu.catalogName);
    dbgPrint("LoggingSetup: Program.cpu.id = " + Program.cpu.id);

    /* TODO: come up with a better way to determine if the platform is a multicore device */
    if ((Program.cpu.catalogName.match(/6000/)) ||
            (Program.cpu.catalogName.match(/cortexa8/)) ||
            (Program.cpu.catalogName.match(/cortexa9/)) ||
            (Program.cpu.catalogName.match(/cortexa15/)) ||
            (Program.cpu.catalogName.match(/cortexm3/)) ||
            (Program.cpu.catalogName.match(/c2800/)) ||
            //      (getNumCPUs() > 1) ||
            (Program.cpu.id > 1)) {
        LoggingSetup.showMulticoreEventCorrelationOption = true;
    }

    // Do not allow JTAG RUNMODE loggerType for anything other than
    // C6x until it is fully supported.
    if (LoggingSetup.loggerType == LoggingSetup.LoggerType_JTAGRUNMODE) {
        if (!Program.cpu.catalogName.match(/6000/)) {
            // Not supported
            LoggingSetup.$logError("LoggingSetup.loggerType = " +
                    "LoggingSetup.LoggerType_JTAGRUNMODE is only " +
                    "supported on C6000 targets",
                    LoggingSetup, "loggerType");
        }
    }

    /* provide enough info to the LogSync module so that it can
     * use the correct type of logger to log the sync points with
     */
    LogSyncModule = xdc.module('ti.uia.runtime.LogSync');  /* don't use this module yet */
    var typeOfLogger = LogSyncModule.LoggerType_NONE;

    switch (LoggingSetup.loggerType) {
        case LoggingSetup.LoggerType_STOPMODE:
            xdc.useModule('ti.uia.loggers.LoggerStopMode');
            typeOfLogger = LogSyncModule.LoggerType_STOPMODE;
            break;

        case LoggingSetup.LoggerType_RUNMODE:
        case LoggingSetup.LoggerType_JTAGRUNMODE:
            xdc.useModule('ti.uia.loggers.LoggerRunMode');
            typeOfLogger = LogSyncModule.LoggerType_RUNMODE;
            break;

        case LoggingSetup.LoggerType_IDLE:
            /* App must configure LoggerIdle.transportType */
            xdc.useModule('ti.uia.sysbios.LoggerIdle');
            typeOfLogger = LogSyncModule.LoggerType_IDLE;
            break;

        case LoggingSetup.LoggerType_MIN:
            xdc.useModule('ti.uia.loggers.LoggerMin');
            typeOfLogger = LogSyncModule.LoggerType_MIN;
            break;
    }

    if  (LoggingSetup.multicoreEventCorrelation &&
            (typeOfLogger != LogSyncModule.LoggerType_NONE)) {
        LogSync = xdc.useModule('ti.uia.runtime.LogSync');
        dbgPrint("LoggingSetup: setting LogSync.loggerType = " + typeOfLogger);
        LogSync.loggerType = typeOfLogger;
    }

    var logger;

    /* Setup the Load modules loggers and diags mask */
    if (LoggingSetup.loadLogging || LoggingSetup.loadLoggingRuntimeControl) {

        Load = xdc.useModule('ti.sysbios.utils.Load');
        Load.taskEnabled = LoggingSetup.loadTaskLogging;
        Load.swiEnabled = LoggingSetup.loadSwiLogging;
        Load.hwiEnabled = LoggingSetup.loadHwiLogging;

        var setting = getDiagsSetting(LoggingSetup.loadLogging,
                LoggingSetup.loadLoggingRuntimeControl);

        /* Turn on the Load module diags mask */
        if (Load.common$.diags_USER4 === undefined) {
            Load.common$.diags_USER4 = setting;
        }

        /* If no logger supplied to this module, create one */
        if (LoggingSetup.loadLogger == null) {
            if (LoggingSetup.loggerType ==
                    LoggingSetup.LoggerType_STREAMER2) {
                LoggingSetup.$logError("LoggingSetup.loggerType = " +
                        "LoggingSetup.LoggerType_STREAMER2 requires the user to create a " +
                        "LoggerStreamer2 instance for LoggingSetup.loadLogger ",
                        LoggingSetup, "loadLogger");
            }

            /* Create the logger instance */
            logger = createLogger(LoggingSetup.loadLoggerSize, "Load Logger", null);
        }
        else {
            logger = LoggingSetup.loadLogger;
        }

        /* Plug it*/
        setLogger(Load, logger);
    }

    /* Setup Main and non-xdc modules loggers and diags mask */
    if (LoggingSetup.mainLogging || LoggingSetup.mainLoggingRuntimeControl ||
        LoggingSetup.countingAndGraphingLogging ||  LoggingSetup.snapshotLogging ||
            LoggingSetup.benchmarkLogging ||
            LoggingSetup.enableContextAwareFunctionProfiler) {

        var setting = getDiagsSetting(LoggingSetup.mainLogging,
                LoggingSetup.mainLoggingRuntimeControl);

        /* Turn on the Main module diags mask */
        var Main = xdc.useModule('xdc.runtime.Main');

        if (Main.common$.diags_STATUS === undefined) {
            Main.common$.diags_STATUS = setting;
        }
        if (Main.common$.diags_ANALYSIS === undefined) {
            Main.common$.diags_ANALYSIS = setting;
        }
        if (Main.common$.diags_INFO === undefined) {
            Main.common$.diags_INFO = setting;
        }
        if (Main.common$.diags_USER1 === undefined) {
            Main.common$.diags_USER1 = setting;
        }
        if (Main.common$.diags_USER2 === undefined) {
            Main.common$.diags_USER2 = setting;
        }
        if (Main.common$.diags_USER3 === undefined) {
            Main.common$.diags_USER3 = setting;
        }
        if (Main.common$.diags_USER4 === undefined) {
            Main.common$.diags_USER4 = setting;
        }
        if (Main.common$.diags_USER5 === undefined) {
            Main.common$.diags_USER5 = setting;
        }
        if (Main.common$.diags_USER6 === undefined) {
            Main.common$.diags_USER6 = setting;
        }

        /* If no logger supplied to this module, create one */
        if (LoggingSetup.mainLogger == null) {
            if (LoggingSetup.loggerType ==
                    LoggingSetup.LoggerType_STREAMER2) {
                LoggingSetup.$logError("LoggingSetup.loggerType = " +
                        "LoggingSetup.LoggerType_STREAMER2 requires the user to create a " +
                        "LoggerStreamer2 instance for LoggingSetup.mainLogger ",
                        LoggingSetup, "mainLogger");
            }
            logger = createLogger(LoggingSetup.mainLoggerSize, "Main Logger", null);
        }
        else {
            logger = LoggingSetup.mainLogger;
        }

        /* Plug it*/
        setLogger(Main, logger);
        if (LoggingSetup.snapshotLogging) {
            setLogger(LogSnapshot, logger);
        }

        /* Plug Registry's logger also if not defined */
        Registry = xdc.useModule('xdc.runtime.Registry');
        setLogger(Registry, logger);
    }

    /* Setup SYSBIOS modules loggers and diags mask */
    var createHwiLogger = false;
    var createSwiLogger = false;
    var createSemLogger = false;
    var createTaskLogger = false;

    /* Setup the SYSBIOS Hwi and Clock diags masks */
    if (LoggingSetup.sysbiosHwiLogging ||
            LoggingSetup.enableTaskProfiler ||
            LoggingSetup.enableContextAwareFunctionProfiler ||
            LoggingSetup.sysbiosHwiLoggingRuntimeControl) {

        var setting = getDiagsSetting(LoggingSetup.sysbiosHwiLogging,
                LoggingSetup.sysbiosHwiLoggingRuntimeControl);

        dbgPrint("LoggingSetup: useModule(ti.sysbios.hal.Hwi') because " +
                "LoggingSetup.sysbiosHwiLogging = " +
                LoggingSetup.sysbiosHwiLogging +
                "||LoggingSetup.sysbiosHwiLoggingRuntimeControl = " +
                LoggingSetup.sysbiosHwiLoggingRuntimeControl + "\n");

        var Hwi = xdc.useModule('ti.sysbios.hal.Hwi');
        var HwiDelegate = Hwi.HwiProxy.delegate$;

        if (HwiDelegate.common$.diags_USER1 === undefined) {
            HwiDelegate.common$.diags_USER1 = setting;
        }
        if (HwiDelegate.common$.diags_USER2 === undefined) {
            HwiDelegate.common$.diags_USER2 = setting;
        }
        if (Hwi.common$.diags_USER1 === undefined) {
            Hwi.common$.diags_USER1 = setting;
        }
        if (Hwi.common$.diags_USER2 === undefined) {
            Hwi.common$.diags_USER2 = setting;
        }

        Clock = xdc.useModule('ti.sysbios.knl.Clock');
        if (Clock.common$.diags_USER1 === undefined) {
            Clock.common$.diags_USER1 = setting;
        }
        if (Clock.common$.diags_USER2 === undefined) {
            Clock.common$.diags_USER2 = setting;
        }
        if (Clock.common$.diags_USER3 === undefined) {
            Clock.common$.diags_USER3 = setting;
        }

        /* If no logger supplied to this module, create one */
        if (LoggingSetup.sysbiosLogger == null) {
            if (LoggingSetup.loggerType ==
                    LoggingSetup.LoggerType_STREAMER2) {
                LoggingSetup.$logError("LoggingSetup.loggerType = " +
                        "LoggingSetup.LoggerType_STREAMER2 requires the user to create a " +
                        "LoggerStreamer2 instance for LoggingSetup.sysbiosLogger ",
                        LoggingSetup, "sysbiosLogger");
            }
            createHwiLogger = true;
        }
        else {
            Hwi.common$.logger = LoggingSetup.sysbiosLogger;
            HwiDelegate.common$.logger = LoggingSetup.sysbiosLogger;
            Clock.common$.logger = LoggingSetup.sysbiosLogger;
        }
    }

    /* Setup SYSBIOS Swi diags mask */
    if (LoggingSetup.sysbiosSwiLogging ||
            LoggingSetup.enableTaskProfiler ||
            LoggingSetup.enableContextAwareFunctionProfiler ||
            LoggingSetup.sysbiosSwiLoggingRuntimeControl) {

        var setting = getDiagsSetting(LoggingSetup.sysbiosSwiLogging,
                LoggingSetup.sysbiosSwiLoggingRuntimeControl);

        var Swi = xdc.useModule('ti.sysbios.knl.Swi');

        if (Swi.common$.diags_USER1 === undefined) {
            Swi.common$.diags_USER1 = setting;
        }
        if (Swi.common$.diags_USER2 === undefined) {
            Swi.common$.diags_USER2 = setting;
        }

        /* If no logger supplied to this module, create one */
        if (LoggingSetup.sysbiosLogger == null) {
            if (LoggingSetup.loggerType ==
                    LoggingSetup.LoggerType_STREAMER2) {
                LoggingSetup.$logError("LoggingSetup.loggerType = " +
                        "LoggingSetup.LoggerType_STREAMER2 requires the user to create a " +
                        "LoggerStreamer2 instance for LoggingSetup.sysbiosLogger ",
                        LoggingSetup, "sysbiosLogger");
            }
            createSwiLogger = true;
        }
        else {
            Swi.common$.logger = LoggingSetup.sysbiosLogger;
        }
    }

    /*
     *  Setup SYSBIOS Semaphore diags mask.
     *  Note: Semaphore events do not have to be enabled for
     *  Task Profiling or Function Profiling.
     */
    if (LoggingSetup.sysbiosSemaphoreLogging ||
            LoggingSetup.sysbiosSemaphoreLoggingRuntimeControl) {
        var setting = getDiagsSetting(LoggingSetup.sysbiosSemaphoreLogging,
                LoggingSetup.sysbiosSemaphoreLoggingRuntimeControl);

        Semaphore = xdc.useModule('ti.sysbios.knl.Semaphore');
        if (Semaphore.common$.diags_USER1 === undefined) {
            Semaphore.common$.diags_USER1 = setting;
        }
        if (Semaphore.common$.diags_USER2 === undefined) {
            Semaphore.common$.diags_USER2 = setting;
        }
        /* If no logger supplied to this module, create one */
        if (LoggingSetup.sysbiosLogger == null) {
            if (LoggingSetup.loggerType ==
                    LoggingSetup.LoggerType_STREAMER2) {
                LoggingSetup.$logError("LoggingSetup.loggerType = " +
                        "LoggingSetup.LoggerType_STREAMER2 requires the user to create a " +
                        "LoggerStreamer2 instance for LoggingSetup.sysbiosLogger ",
                        LoggingSetup, "sysbiosLogger");
            }
            createSemLogger = true;
        }
        else {
            Semaphore.common$.logger = LoggingSetup.sysbiosLogger;
        }
    }

    /* Setup SYSBIOS Task and related modules diags mask */
    if (LoggingSetup.sysbiosTaskLogging ||
            LoggingSetup.enableTaskProfiler ||
            LoggingSetup.enableContextAwareFunctionProfiler ||
            LoggingSetup.sysbiosTaskLoggingRuntimeControl) {

        var setting = getDiagsSetting(LoggingSetup.sysbiosTaskLogging,
                LoggingSetup.sysbiosTaskLoggingRuntimeControl);

        Task = xdc.useModule('ti.sysbios.knl.Task');
        if (Task.common$.diags_USER1 === undefined) {
            Task.common$.diags_USER1 = setting;
        }
        if (Task.common$.diags_USER2 === undefined) {
            Task.common$.diags_USER2 = setting;
        }

        Event = xdc.useModule('ti.sysbios.knl.Event');
        if (Event.common$.diags_USER1 === undefined) {
            Event.common$.diags_USER1 = setting;
        }
        if (Event.common$.diags_USER2 === undefined) {
            Event.common$.diags_USER2 = setting;
        }

        /* If no logger supplied to this module, create one */
        if (LoggingSetup.sysbiosLogger == null) {
            if (LoggingSetup.loggerType ==
                    LoggingSetup.LoggerType_STREAMER2) {
                LoggingSetup.$logError("LoggingSetup.loggerType = " +
                        "LoggingSetup.LoggerType_STREAMER2 requires the user to create a " +
                        "LoggerStreamer2 instance for LoggingSetup.sysbiosLogger ",
                        LoggingSetup, "sysbiosLogger");
            }
            createTaskLogger = true;
        }
        else {
            Task.common$.logger = LoggingSetup.sysbiosLogger;
            Event.common$.logger = LoggingSetup.sysbiosLogger;
        }
    }

    /* Setup SYSBIOS Task and related modules loggers */
    if ((createHwiLogger == true) || (createSwiLogger == true) ||
            (createTaskLogger == true) || (createSemLogger == true)) {

        /* Create the logger for all SYSBIOS modules */
        logger = createLogger(LoggingSetup.sysbiosLoggerSize,
                "SYSBIOS System Logger", null);
        sysbiosLoggerAutoCreated = true;

    }

    if (createHwiLogger == true) {
        setLogger(Hwi, logger);
        setLogger(HwiDelegate, logger);
        setLogger(Clock, logger);
    }

    if (createSwiLogger == true) {
        setLogger(Swi, logger);
    }

    if (createTaskLogger == true) {
        setLogger(Task, logger);
        setLogger(Event, logger);
    }

    if (createSemLogger == true) {
        setLogger(Semaphore, logger);
    }

    /* Make sure to add Rta in when we need it */
    if (LoggingSetup.loggerType == LoggingSetup.LoggerType_RUNMODE) {
        var LoggerRunMode = xdc.module('ti.uia.loggers.LoggerRunMode');
        if (LoggerRunMode.transportType == LoggerRunMode.TransportType_ETHERNET) {
            var Rta = xdc.useModule('ti.uia.services.Rta');
        }
    }
}

/*
 * ======== module$static$init ========
 */
function module$static$init(mod, params)
{
}

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    var GUI = this;

    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    dbgPrint("UIA: LoggingSetup.module$meta$init: GetSet operations");
    var GetSet = xdc.module("xdc.services.getset.GetSet");
    GetSet.init(GUI);

    /* ======== _setLoadAnalysisMasterFlag =========
     * Local scope function.
     * Ensures that, if the 'top level' loadLogging flag is set to false, all types of
     * load logging are also set to false in the .cfg file.
     */
     function _setLoadAnalysisMasterFlag(field, value)
     {
         dbgPrint("UIA: LoggingSetup._setLoadAnalysisMasterFlag field="+field+", value = "+value);

	 GUI.loadLogging = value;
	 if (value == false) {
             GUI.loadTaskLogging = false;
	     GUI.loadSwiLogging = false;
	     GUI.loadHwiLogging = false;
	 }
     }

     /* ======== _setExecutionAnalysisMasterFlag =========
      * Local scope function.
      * Ensures that, if the 'top level' sysbiosTaskLogging flag is set to false, all types of
      * sysbios execution logging are also set to false in the .cfg file.
      */
    function _setExecutionAnalysisMasterFlag(field, value)
    {
        dbgPrint("UIA: LoggingSetup._setExecutionAnalysisMasterFlag field="+field+", value = "+value);
        GUI.sysbiosTaskLogging = value;
	if (value == false) {
	    GUI.sysbiosSwiLogging = false;
	    GUI.sysbiosHwiLogging = false;
	    GUI.sysbiosSemaphoreLogging = false;
	}
    }

    /* The following code will cause the _setLoadAnalysisMasterFlag local function to be called
     * whenever the user clicks on the RTOS Load Analysis checkbox in the XGConf LoggingSetup.xml GUI.
     */
     GetSet.onSet(this, "loadLogging", _setLoadAnalysisMasterFlag);

     /* The following code will cause the _setExecutionAnalysisMasterFlag local function to be called
      * whenever the user clicks on the RTOS Execution Analysis checkbox in the XGConf LoggingSetup.xml GUI.
      */
      GetSet.onSet(this, "sysbiosTaskLogging", _setExecutionAnalysisMasterFlag);
}

/*
 * ======== createLogger =========
 * internal helper function that creates the type of logger
 * appropriate for the LoggingSetup.loggerType that has been configured.
 *
 * param loggerSize: the size of the logger in MAUs
 * param loggerInstanceName: the name to assign to the logger instance
 * param loggerPriority: the IUIATransfer.Priority to assign to the logger instance
 * returns the logger instance that was created
 */
function createLogger(loggerSize, loggerInstanceName, loggerPriority) {
    /* Create the logger instance */
    var IUIATransfer = xdc.module('ti.uia.runtime.IUIATransfer');
    var logger = null;
    var newLoggerName = loggerInstanceName;

    /*
     *  These Loggers need only one instance:
     *    LoggerIdle, LoggerMin, LoggerStreamer
     */
    if (((LoggingSetup.loggerType == LoggingSetup.LoggerType_IDLE) ||
                (LoggingSetup.loggerType == LoggingSetup.LoggerType_MIN)) &&
            (listOfLoggersCreated.length > 0)) {

        /* Return the logger that has already been created */
        return(listOfLoggersCreated[0]);
    }

    switch (LoggingSetup.loggerType) {
        case LoggingSetup.LoggerType_MIN:
            Logger = xdc.module('ti.uia.loggers.LoggerMin');
           /* Since all events are logged into a single log buffer,
              name the buffer after the logger */
            newLoggerName = "LoggerMin";
            dbgPrint("LoggingSetup: LoggerMin used for " + newLoggerName);
            break;

        case LoggingSetup.LoggerType_IDLE:
            Logger = xdc.module('ti.uia.sysbios.LoggerIdle');
           /* Since all events are logged into a single log buffer,
              name the buffer after the logger */
            newLoggerName = "LoggerIdle";
            dbgPrint("LoggingSetup: LoggerIdle used for " + newLoggerName);
            break;

        case LoggingSetup.LoggerType_STREAMER2:
            Logger = xdc.module('ti.uia.sysbios.LoggerStreamer2');
            dbgPrint("LoggingSetup: LoggerStreamer2 used for " + newLoggerName);
            break;

        case LoggingSetup.LoggerType_STOPMODE:
            Logger = xdc.module('ti.uia.loggers.LoggerStopMode');
            dbgPrint("LoggingSetup: LoggerStopMode used for " + newLoggerName);

            if (LoggingSetup.showNumCoresOption) {
                Logger.numCores = LoggingSetup.numCores;
            }
            break;

        case LoggingSetup.LoggerType_RUNMODE:
        case LoggingSetup.LoggerType_JTAGRUNMODE:
            Logger = xdc.module('ti.uia.loggers.LoggerRunMode');
            if (LoggingSetup.showNumCoresOption) {
                Logger.numCores = LoggingSetup.numCores;
            }
            dbgPrint("LoggingSetup: LoggerRunMode used for " + newLoggerName);
            break;
    }

    /* Set timestamp size */
    if ("timestampSize" in Logger) {
        switch (LoggingSetup.timestampSize) {
            case LoggingSetup.TimestampSize_NONE:
                Logger.timestampSize = Logger.TimestampSize_NONE;
                break;
            case LoggingSetup.TimestampSize_32b:
            case LoggingSetup.TimestampSize_AUTO:
                Logger.timestampSize = Logger.TimestampSize_32b;
                break;
            case LoggingSetup.TimestampSize_64b:
                Logger.timestampSize = Logger.TimestampSize_64b;
                break;
        }
    }
    else if ("isTimestampEnabled" in Logger) {
        if (LoggingSetup.timestampSize != LoggingSetup.TimestampSize_NONE) {
            Logger.isTimestampEnabled = true;
        }
        else {
            Logger.isTimestampEnabled = false;
        }
    }

    var loggerParams = new Logger.Params();
    var maxLoggerSize = 0;
    if (LoggingSetup.loggerType == LoggingSetup.LoggerType_MIN) {
        /* Configure LoggerMin module parameters */
        maxLoggerSize = getMaxLoggerSize();
        if (maxLoggerSize != 0) {
            dbgPrint("LoggingSetup: Setting LoggerMin buffer size to " + maxLoggerSize);
            Logger.bufSize = maxLoggerSize;
        }
        if (LoggingSetup.memorySectionName == null) {
            Logger.bufSection = null;
        } else {
            Logger.bufSection = ".uiaLoggerBuffers";
        }
    }
    else {
        /* Configure Logger module and instance parameters*/
        if (LoggingSetup.loggerType == LoggingSetup.LoggerType_IDLE) {
            maxLoggerSize = getMaxLoggerSize();
            if (maxLoggerSize != 0) {
                var bufSize = maxLoggerSize / Program.build.target.stdTypes.t_Int32.size;
                dbgPrint("LoggingSetup: Setting LoggerIdle buffer size to " +
                        maxLoggerSize + " / " + Program.build.target.stdTypes.t_Int32.size +
                        " = " +
                        bufSize);
                Logger.bufferSize = bufSize;
            }
        }
        else {
            if ("bufSection" in loggerParams) {
                if (LoggingSetup.memorySectionName == null) {
                    loggerParams.bufSection = null;
                } else {
                    loggerParams.bufSection = ".uiaLoggerBuffers";
                }
            }
            loggerParams.bufSize = loggerSize;

            if (!LoggingSetup.snapshotLogging) {
                /* remove buffer size constraints since snapshot logging is not enabled */
                loggerParams.maxEventSize = 0;
            } else {
                loggerParams.maxEventSize = LoggingSetup.maxEventSize;
            }
            if ("cpuId" in Logger) {
                Logger.cpuId = LoggingSetup.cpuId;
            }
        }
    }

    logger = Logger.create(loggerParams);
    logger.instance.name = newLoggerName;

    if (LoggingSetup.loggerType != LoggingSetup.LoggerType_MIN &&
            LoggingSetup.loggerType != LoggingSetup.LoggerType_IDLE) {
        logger.instanceId = listOfLoggersCreated.length + 1;
    }

    listOfLoggersCreated.push(logger);

    return (logger);
}

/*
 *  ======== setLogger ========
 *
 */
function setLogger(mod, logger)
{
    if (mod == null) {
        dbgPrint("UIA LoggingSetup.setLogger: mod == null");
    }
    if (logger == null) {
        dbgPrint("UIA LoggingSetup.setLogger: logger == null");
    }
    if ((mod != null) && (logger != null)) {
        dbgPrint("UIA LoggingSetup.setLogger: Setting " + mod.$name +
                ".logger to " + logger.$name);
    }

    /* Warn if the user has already set the logger for this module. */
    if (mod.common$.logger != undefined) {
        mod.$logWarning(mod.$name + ".common$.logger has already " +
                "been assigned, LoggingSetup reassigning logger to " +
                logger.$name, mod);
    }

    mod.common$.logger = logger;
}

/*
 *  ======== getDiagsSetting ========
 */
function getDiagsSetting(enabled, runtimeCtrl)
{
    if (!enabled && !runtimeCtrl) {
        return (Diags.ALWAYS_OFF);
    }
    else if (!enabled && runtimeCtrl) {
        return (Diags.RUNTIME_OFF);
    }
    else if (enabled && !runtimeCtrl) {
        return (Diags.ALWAYS_ON);
    }
    else if (enabled && runtimeCtrl) {
        return (Diags.RUNTIME_ON);
    }
}

/*
 *  ======== getNameOfLoggerInstance ========
 */
function getNameOfLoggerInstance(loggerObj){
    var moduleName = ""+loggerObj.$name;
    var instanceName = "null";

    dbgPrint("UIA: getNameOfLoggerInstance(" + loggerObj.$name + ")");

    var index = moduleName.indexOf(".Instance#");
    if (index > 0){
        moduleName = moduleName.substring(0,index);
        dbgPrint("UIA: moduleName = "+moduleName);
        var loggerMod = xdc.module(moduleName);

        for each (var cbInst in loggerMod.$instances) {
            dbgPrint("UIA: cbInst = " + cbInst);
            dbgPrint("UIA: cbInst.$name = "+cbInst.$name);
            if (cbInst.instance.name != null){
                dbgPrint("UIA: cbInst.instance.name = " + cbInst.instance.name);
                if ((cbInst != null) && (loggerObj.$name.indexOf(cbInst.$name) >= 0)) {
                    instanceName = cbInst.instance.name;
                    dbgPrint("UIA: instanceName = "+instanceName);
                    break;
                }
            }
        }
    }
    return (instanceName);
}

/*
 *  ======== writeUIAMetaDataForLogger ========
 */
function writeUIAMetaDataForLogger(strEntryName, modName,indent){
    var UIAMetaData = xdc.module('ti.uia.runtime.UIAMetaData');
    UIAMetaData.genXmlEntryOpen(indent,strEntryName);
    var IUIATransfer = xdc.module('ti.uia.runtime.IUIATransfer');
    var ILogger = xdc.module('xdc.runtime.ILogger');
    var Mod = xdc.module(modName);
    var sizeInMAUs = 0;
    var numCores = 1;
    var otherLogger = null;

    if (Mod.$used){
        var modLogger = Mod.common$.logger;
        if (modLogger instanceof IUIATransfer.Module){
            if (modLogger instanceof LoggerStreamer2.Module){
                sizeInMaus = modLogger.instance.bufSize;
                numCores = modLogger.instance.numCores
            } else if (mod instanceof LoggerRunMode.Module){
                sizeInMaus = modLogger.instance.bufSize;
                numCores = modLogger.instance.numCores
            } else {
                otherLogger = modLogger;
            }
        }
        if (modLogger instanceof ILogger.Module){
            if (modLogger.$name.lastIndexOf('LoggerStreamer2') > 0){
                sizeInMAUs = modLogger.instance.bufSize;
            } else {
                otherLogger = modLogger;
            }
        }
        if (modLogger != null){
            if (modLogger.$name != null) {
                UIAMetaData.genXmlEntry(indent+3,"instance", modLogger.$name);
            } else {
                UIAMetaData.genXmlEntry(indent+3,"instance", ""+modLogger);
            }
            var instName = getNameOfLoggerInstance(modLogger);
            UIAMetaData.genXmlEntry(indent+3,"name", instName);
            UIAMetaData.genXmlEntry(indent+3,"sizeInMAUs", ""+sizeInMAUs);
            UIAMetaData.genXmlEntry(indent+3,"numCores", ""+numCores);

        } else {
            UIAMetaData.genXmlEntry(indent+3,"instance", "null");
            UIAMetaData.genXmlEntry(indent+3,"name", "null");
        }
    } else {
        UIAMetaData.genXmlEntry(indent+3,"instance", "null");
        UIAMetaData.genXmlEntry(indent+3,"name", "null");
    }

    UIAMetaData.genXmlEntryClose(indent,strEntryName);
}

/*
 *  ======== writeUIAMetaData ========
 * generates XML metadata describing the clocks used for multicore correlation
 *
 *  NOTE: This should only be called by ti.uia.runtime.UIAMetaData.xs in order
 *  to ensure that the UIA xml file has been properly opened and is ready for
 *  writing.
 *  @param(inst) instance of the module to generate data for, null for module-level info
 *  @param(instNum) the instance number used in .h files
 *  @param(indent) the number of spaces to preceed the xml tag with
 */
function writeUIAMetaData(indent)
{
    var mod = xdc.module('ti.uia.sysbios.LoggingSetup');
    var UIAMetaData = xdc.module('ti.uia.runtime.UIAMetaData');
    var ILogger = xdc.module('xdc.runtime.ILogger');

    dbgPrint("UIA: LoggingSetup.writeUIAMetaData: loggerType = " +
            mod.loggerType);

    UIAMetaData.genXmlEntryOpen(indent,"LoggingSetupConfig");
    indent += 3;

    var ctr = 0;

    /* Provide information to drive System Analyzer Use Cases (CCSv6 and later) */
    /* NOTE: BE SURE TO KEEP THE CAPTIONS IN SYNC WITH LoggingSetup.xml */
    UIAMetaData.genXmlComment(indent, "System Analyzer Event Sources");
    UIAMetaData.genXmlEntryOpen(indent, "SystemAnalyzerEventSources");

    UIAMetaData.genXmlEntry(indent+3, "RTOS_Execution_Analysis",
            LoggingSetup.sysbiosTaskLogging.toString());
    if (!LoggingSetup.sysbiosTaskLogging) {
        UIAMetaData.genXmlEntry(indent+3, "RTOS_Execution_Analysis_Caption",
                "Enable &quot;RTOS Execution Analysis&quot;");
    } else {
        UIAMetaData.genXmlEntry(indent+3, "RTOS_Execution_Analysis_Caption"," ");
    }

    UIAMetaData.genXmlEntry(indent+3, "RTOS_Load_Analysis", LoggingSetup.loadLogging.toString());
    if (!LoggingSetup.loadLogging) {
        UIAMetaData.genXmlEntry(indent+3, "RTOS_Load_Analysis_Caption", "Enable &quot;RTOS Load Analysis&quot;");
    } else {
        UIAMetaData.genXmlEntry(indent+3,"RTOS_Load_Analysis_Caption"," ");
    }

    UIAMetaData.genXmlEntry(indent+3,"Context_Aware_Function_Profiler",
            LoggingSetup.enableContextAwareFunctionProfiler.toString());
    if (!LoggingSetup.enableContextAwareFunctionProfiler) {
        UIAMetaData.genXmlEntry(indent+3, "Context_Aware_Function_Profiler_Caption", "Enable &quot;Context-Aware Function Profiler&quot;");
    } else {
        UIAMetaData.genXmlEntry(indent+3, "Context_Aware_Function_Profiler_Caption"," ");
    }

    UIAMetaData.genXmlEntry(indent+3,"Task_Profiler",LoggingSetup.enableTaskProfiler.toString());
    if (!LoggingSetup.enableTaskProfiler){
        UIAMetaData.genXmlEntry(indent+3, "Task_Profiler_Caption", "Enable &quot;Task Profiler&quot;");
    } else {
        UIAMetaData.genXmlEntry(indent+3, "Task_Profiler_Caption"," ");
    }

    UIAMetaData.genXmlEntry(indent+3, "EWIP_Events", LoggingSetup.mainLogging.toString());
    if (!LoggingSetup.mainLogging) {
        UIAMetaData.genXmlEntry(indent+3,"EWIP_Events_Caption","Enable &quot;Error, Warning, Info and Print Events&quot;");
    } else {
        UIAMetaData.genXmlEntry(indent+3,"EWIP_Events_Caption"," ");
    }

    UIAMetaData.genXmlEntry(indent+3,"Benchmarking",LoggingSetup.benchmarkLogging.toString());
    if (!LoggingSetup.benchmarkLogging) {
        UIAMetaData.genXmlEntry(indent+3,"Benchmarking_Caption","Enable &quot;Benchmarking&quot;");
    } else {
        UIAMetaData.genXmlEntry(indent+3,"Benchmarking_Caption"," ");
    }

    UIAMetaData.genXmlEntry(indent+3,"Counting_and_Graphing",LoggingSetup.countingAndGraphingLogging.toString());
    if (!LoggingSetup.countingAndGraphingLogging) {
        UIAMetaData.genXmlEntry(indent+3,"Counting_and_Graphing_Caption","Enable &quot;Counting and Graphing&quot;");
    } else {
        UIAMetaData.genXmlEntry(indent+3,"Counting_and_Graphing_Caption"," ");
    }

    UIAMetaData.genXmlEntry(indent+3,"Snapshot_Events",LoggingSetup.snapshotLogging.toString());
    if (!LoggingSetup.snapshotLogging) {
        UIAMetaData.genXmlEntry(indent+3,"Snapshot_Events_Caption","Enable &quot;Snapshot Events&quot;");
    } else {
        UIAMetaData.genXmlEntry(indent+3,"Snapshot_Events_Caption"," ");
    }

    UIAMetaData.genXmlEntry(indent+3,"Multicore_Event_Correlation",(LoggingSetup.multicoreEventCorrelation).toString());
    if (!LoggingSetup.multicoreEventCorrelation){
        UIAMetaData.genXmlEntry(indent+3,"Multicore_Event_Correlation_Caption","Enable &quot;Multicore Event Correlation&quot;");
    } else {
        UIAMetaData.genXmlEntry(indent+3,"Multicore_Event_Correlation_Caption"," ");
    }

    UIAMetaData.genXmlEntry(indent+3,"Task_Context",LoggingSetup.sysbiosTaskLogging.toString());
    if (!LoggingSetup.sysbiosTaskLogging) {
        UIAMetaData.genXmlEntry(indent+3, "Task_Context_Caption",
                "Enable &quot;RTOS Execution Analysis&quot; - &quot;Task&quot;");
    } else {
        UIAMetaData.genXmlEntry(indent+3, "Task_Context_Caption"," ");
    }

    UIAMetaData.genXmlEntry(indent+3,"HWI_Context",LoggingSetup.sysbiosHwiLogging.toString());
    if ((!LoggingSetup.sysbiosTaskLogging) || (!LoggingSetup.sysbiosHwiLogging)) {
        UIAMetaData.genXmlEntry(indent+3, "HWI_Context_Caption",
                "Enable &quot;RTOS Execution Analysis&quot; - &quot;Hwi&quot;");
    } else {
        UIAMetaData.genXmlEntry(indent+3, "HWI_Context_Caption"," ");
    }

    UIAMetaData.genXmlEntry(indent+3,"SWI_Context",LoggingSetup.sysbiosSwiLogging.toString());
    if ((!LoggingSetup.sysbiosTaskLogging) || (!LoggingSetup.sysbiosSwiLogging)) {
        UIAMetaData.genXmlEntry(indent+3, "SWI_Context_Caption",
                "Enable &quot;RTOS Execution Analysis&quot; - &quot;Swi&quot;");
    } else {
        UIAMetaData.genXmlEntry(indent+3, "SWI_Context_Caption"," ");
    }

    UIAMetaData.genXmlEntry(indent+3,"Semaphores",LoggingSetup.sysbiosSemaphoreLogging.toString());
    if ((!LoggingSetup.sysbiosTaskLogging) || (!LoggingSetup.sysbiosSemaphoreLogging)) {
        UIAMetaData.genXmlEntry(indent+3, "Semaphores_Caption",
                "Enable &quot;RTOS Execution Analysis&quot; - &quot;Semaphores&quot;");
    } else {
        UIAMetaData.genXmlEntry(indent+3, "Semaphores_Caption"," ");
    }

    UIAMetaData.genXmlEntry(indent+3,"CPU_Load",LoggingSetup.loadLogging.toString());
    if (!LoggingSetup.loadLogging) {
        UIAMetaData.genXmlEntry(indent+3, "CPU_Load_Caption", "Enable &quot;RTOS Load Analysis&quot;");
    } else {
        UIAMetaData.genXmlEntry(indent+3,"CPU_Load_Caption"," ");
    }

    UIAMetaData.genXmlEntry(indent+3,"Task_Load",LoggingSetup.loadTaskLogging.toString());
    if ((!LoggingSetup.loadLogging) || (!LoggingSetup.loadTaskLogging)) {
        UIAMetaData.genXmlEntry(indent+3, "Task_Load_Caption", "Enable &quot;RTOS Load Analysis&quot; - &quot;Task Load&quot;");
    } else {
        UIAMetaData.genXmlEntry(indent+3,"Task_Load_Caption"," ");
    }

    UIAMetaData.genXmlEntry(indent+3,"HWI_Load",LoggingSetup.loadHwiLogging.toString());
    if ((!LoggingSetup.loadLogging) || (!LoggingSetup.loadHwiLogging)) {
        UIAMetaData.genXmlEntry(indent+3, "HWI_Load_Caption", "Enable &quot;RTOS Load Analysis&quot; - &quot;Hwi Load&quot;");
    } else {
        UIAMetaData.genXmlEntry(indent+3,"HWI_Load_Caption"," ");
    }

    UIAMetaData.genXmlEntry(indent+3,"SWI_Load",LoggingSetup.loadSwiLogging.toString());
    if ((!LoggingSetup.loadLogging) || (!LoggingSetup.loadSwiLogging)) {
        UIAMetaData.genXmlEntry(indent+3, "SWI_Load_Caption", "Enable &quot;RTOS Load Analysis&quot; - &quot;Swi Load&quot;");
    } else {
        UIAMetaData.genXmlEntry(indent+3,"SWI_Load_Caption"," ");
    }

    UIAMetaData.genXmlEntryClose(indent,"SystemAnalyzerEventSources");

    UIAMetaData.genXmlComment(indent,"User-friendly buffer names (aligned with LoggingSetup.xml captions)");
    UIAMetaData.genXmlEntryOpen(indent,"BufferNames");
    var strModuleId = "";
    var strModuleName ="";
    var numInstances = 0;
    var instanceName = "";
    var instanceNameLowerCase = "";
    var instanceNameWithQuotes = "";
    var index = 0;

    for each (var mod in Program.targetModules()) {
        if (mod instanceof ILogger.Module) {
            strModuleName = mod.$name;
            numInstances = mod.$instances.length;
            if (strModuleName.indexOf("LoggerIdle") >= 0){
                var LoggerIdle = xdc.module('ti.uia.sysbios.LoggerIdle');
                instanceName = LoggerIdle.$instances[0].instance.name;
                instanceNameWithQuotes = "\x22"+instanceName+"\x22";
                dbgPrint("LoggingSetup: LoggerIdle found ("+strModuleName+")");
                UIAMetaData.genXmlEntryWithKey(indent+3,"logger","name",instanceNameWithQuotes,"LoggerIdle Buffer");
            } else {
                if (strModuleName.indexOf("LoggerMin") >= 0){
                    var LoggerMin = xdc.module('ti.uia.loggers.LoggerMin');
                    instanceName = LoggerMin.$instances[0].instance.name;
                    instanceNameWithQuotes = "\x22"+instanceName+"\x22";

                    dbgPrint("LoggingSetup: LoggerMin found ("+strModuleName+")");
                    dbgPrint("LoggingSetup: logger instance name = "+instanceName+" ("+strModuleName+")");
                    UIAMetaData.genXmlEntryWithKey(indent+3,"logger","name",instanceNameWithQuotes,"LoggerMin Buffer");
                } else {
                    /*
                     *  Check that for no instances of the logger, eg if LoggingSetup
                     *  is used but all loggers are assigned to LoggerStreamer2
                     *  instances. In this case, LoggerStopMode is brought in but
                     *  there are no instances.
                     */
                    if (mod.$instances.length == 0) {
                        continue;
                    }

                    instanceName = mod.$instances[0].instance.name;

                    if (instanceName == null) {
                        continue;
                    }
                    instanceNameLowerCase = instanceName.toLowerCase();
                    instanceNameWithQuotes = "\x22" + instanceName + "\x22";

                    for (index=0; index < numInstances; index++){
                        instanceName = mod.$instances[index].instance.name;
                        instanceNameWithQuotes = "\x22" + instanceName + "\x22";
                        dbgPrint("LoggingSetup: logger instance name = " + instanceName + " ("+strModuleName+")");
                        if (instanceNameLowerCase.indexOf("load") > 0) {
                            UIAMetaData.genXmlEntryWithKey(indent+3,"logger","name",instanceNameWithQuotes,"RTOS Load Events Buffer");
                        } else {
                            if (instanceNameLowerCase.indexOf("sysbios") > 0) {
                                UIAMetaData.genXmlEntryWithKey(indent+3,"logger","name",instanceNameWithQuotes,"RTOS Execution Events Buffer");
                            } else {
                                if (instanceNameLowerCase.indexOf("main") > 0) {
                                    UIAMetaData.genXmlEntryWithKey(indent+3,"logger","name",instanceNameWithQuotes,"User-written Log Events Buffer");
                                } else {
                                    if (instanceNameLowerCase.indexOf("sync") > 0) {
                                        UIAMetaData.genXmlEntryWithKey(indent+3,"logger","name",instanceNameWithQuotes,"LogSync Sync-point Events Buffer");
                                    } else {
                                        if (instanceName != null) {
                                            UIAMetaData.genXmlEntryWithKey(indent+3,"logger","name",instanceNameWithQuotes,instanceName.toString()+" Events Buffer");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    UIAMetaData.genXmlEntryClose(indent,"BufferNames");


    writeUIAMetaDataForLogger("mainLogger", "xdc.runtime.Main",indent);
    writeUIAMetaDataForLogger("loadLogger", "ti.sysbios.utils.Load",indent);
    writeUIAMetaDataForLogger("taskLogger", "ti.sysbios.knl.Task",indent);
    writeUIAMetaDataForLogger("hwiLogger", "ti.sysbios.hal.Hwi",indent);
    writeUIAMetaDataForLogger("swiLogger", "ti.sysbios.knl.Swi",indent);
    writeUIAMetaDataForLogger("semaphoreLogger", "ti.sysbios.knl.Semaphore",indent);
    writeUIAMetaDataForLogger("eventLogger", "ti.sysbios.knl.Event",indent);
    writeUIAMetaDataForLogger("syncLogger", "ti.uia.runtime.LogSync",indent);

    UIAMetaData.genXmlComment(indent,"transportType enumeration values");
    UIAMetaData.genXmlEntryOpen(indent,"TransportTypeEnum");
    UIAMetaData.genXmlEntryWithKey(indent+3,"Name","Value","\"0\"","JTAG_STOPMODE");
    UIAMetaData.genXmlEntryWithKey(indent+3,"Name","Value","\"1\"", "JTAG_RUNMODE");
    UIAMetaData.genXmlEntryWithKey(indent+3,"Name","Value","\"3\"", "SERIAL");
    UIAMetaData.genXmlEntryWithKey(indent+3,"Name","Value","\"4\"", "ETHERNET");
    UIAMetaData.genXmlEntryWithKey(indent+3,"Name","Value","\"5\"", "USER");
    UIAMetaData.genXmlEntryWithKey(indent+3,"Name","Value","\"6\"", "NONE");
    UIAMetaData.genXmlEntryClose(indent,"TransportTypeEnum");

    indent = indent - 3;
    UIAMetaData.genXmlEntryClose(indent,"LoggingSetupConfig");
}

/*
 *  ======== getNumCPUs ========
 */
function getNumCpus()
{
    var platform = Program.platform;
    var ctr = 0;
    var result = 0;
    var platformConfigs = Program.platform.$spec.getConfigs().toArray();

    for (var ctr = 0; ctr < platformConfigs.length; ctr++) {
        var c = platformConfigs[ctr];
        dbgPrint("UIA: LoggingSetup.getNumCpus: platform config " + c.getName());

        if (c.getName().indexOf("CPU") >= 0) {
            dbgPrint("CPU = " + mod.CPU.val);
            result++;
        }
        if (c.getName().indexOf("DSP") >= 0) {
            dbgPrint("DSP = " + mod.DSP.val);
            result++;
        }
        if (c.getName().indexOf("GPP") >= 0) {
            dbgPrint("GPP = " + mod.GPP.val);
            result++;
        }
    }
}

/*
 *  ======== getMaxLoggerSize ========
 *  For LoggerIdle and LoggerMin, which have one buffer for all
 *  logs.
 */
function getMaxLoggerSize()
{
    var loggerSize = 0;

    /*
     *  Pick the biggest of the configured sizes.  If none have
     *  been configured, choose the loadLoggerSize.
     */
    if (LoggingSetup.$written("mainLoggerSize")) {
        loggerSize = LoggingSetup.mainLoggerSize;
    }

    if (LoggingSetup.$written("loadLoggerSize") &&
        (LoggingSetup.loadLoggerSize > loggerSize)) {
        loggerSize = LoggingSetup.loadLoggerSize;
    }

    if (LoggingSetup.$written("sysbiosLoggerSize") &&
        (LoggingSetup.sysbiosLoggerSize > loggerSize)) {
        loggerSize = LoggingSetup.sysbiosLoggerSize;
    }

    /*
     *  Return logger size of 0, if nothing was written.  In that
     *  case, use the default buffer size of LoggerIdle or LoggerMin.
     */

    return (loggerSize);
}
