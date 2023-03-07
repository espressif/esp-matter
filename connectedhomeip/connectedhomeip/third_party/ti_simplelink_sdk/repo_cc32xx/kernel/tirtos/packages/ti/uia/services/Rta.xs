/*
 * Copyright (c) 2012-2016, Texas Instruments Incorporated
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
 *  ======== Rta.xs ========
 */

var Rta = null;

var IUIATransfer = null;
var ILogger = null;
var Log = null;
var ServiceMgr = null;
var UIAPacket = null;
var Timestamp = null;
var Error = null;
var UIAMetaData = null;
var LogSync = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    Rta = this;

    /* Bring in all modules used directly by the Rta. */
    IUIATransfer = xdc.useModule('ti.uia.runtime.IUIATransfer');
    ILogger      = xdc.useModule('xdc.runtime.ILogger');
    Log          = xdc.useModule('xdc.runtime.Log');
    ServiceMgr   = xdc.useModule('ti.uia.runtime.ServiceMgr');
    UIAPacket    = xdc.useModule('ti.uia.runtime.UIAPacket');
    Timestamp    = xdc.useModule('xdc.runtime.Timestamp');
    Error        = xdc.useModule('xdc.runtime.Error');
    UIAMetaData  = xdc.useModule('ti.uia.runtime.UIAMetaData');
    LogSync = xdc.useModule('ti.uia.runtime.LogSync');
    LogSync.isUsedByRta();
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    if (xdc.om.$name != "cfg") {
        return;
    }

    /* Register all of the logger instances with the module */
    registerLoggers(mod);

    /*
     * Initialize totalPacketsSent, which simply tracks how many UIA packets
     * have been sent up for debugging purposes.
     */
    mod.totalPacketsSent = 0;

    /* Initialize the starting event sequence number */
    mod.seq = 0;

    /*
     *  If there is support control, wait for a start command.
     *  If there is no support control, start sending...unless
     *  the period is zero. Give a remark to the user for this case.
     */
    if (ServiceMgr.supportControl == true) {
       mod.txData = false;
    }
    else if (params.periodInMs != 0) {
        mod.txData = true;
    }
    else {
        mod.txData = false;
        print("\nremark: The Rta.periodInMs is zero and the " +
              "ServiceMgr.supportControl is false. Therefore Log events " +
              "cannot be sent to an instrumentation host.");
    }

    mod.period = params.periodInMs;
    mod.snapshot = false;

    /* Plug in process function */
    ServiceMgr.register(Rta.SERVICEID, '&ti_uia_services_Rta_processCallback',
                        params.periodInMs);
 }

/*
 *  ======== registerLoggers ========
 *  Register all logger instances with the Agent so that it can service
 *  them on the target.
 */
function registerLoggers(modObj)
{
    /* Add all instances of modules which implement IUIATransfer. */
    for (var i = 0; i < xdc.om.$modules.length; i++) {
        var mod = xdc.om.$modules[i];

        /* Only handle IUIATransfer modules. */
        if (!(mod instanceof IUIATransfer.Module)) {
            /*
             * Warn the user if the application is still using LoggerBuf
            */
            if ((mod.$name == "xdc.runtime.LoggerBuf") &&
               ((mod.$instances.length + mod.$objects.length) != 0)) {
                print("\nremark: UIA's Rta Service does not support LoggerBuf\n");
            }
            continue;
        }

        /* Add each of the module's instances. */
        for (var j = 0; j < mod.$instances.length; j++) {
            addLogger(mod.$instances[j], modObj.loggers);
        }

        /* get embedded loggers too */
        for (var j = 0; j < mod.$objects.length; j++) {
            addLogger(mod.$objects[j], modObj.loggers);
        }
    }

    /* Set numLoggers so the target can iterate over them. */
    modObj.numLoggers = modObj.loggers.length;
}

/*
 *  ======== addLogger ========
 *  Add logger to array if not already present.
 */
function addLogger(logger, loggers)
{
    // TODO SDOCM00076661 get the logger's id

    for (var i = 0; i < loggers.length; i++) {
        if (logger.$module.getLoggerInstanceId(logger) ==
            loggers[i].$module.getLoggerInstanceId(loggers[i])) {
            return;
        }
    }
    loggers.$add(logger);

}

/*
 *  ======== validate ========
 */
function validate()
{
    registerLoggers(Rta.$object);
}

/*
 *  ======== viewInitModule ========
 *  Initializes the Module view in ROV.
 */
function viewInitModule(view, mod)
{
    var Rta  = xdc.useModule("ti.uia.services.Rta");

    var rtaModConfig = Program.getModuleConfig(Rta.$name);

    /* Display whether ServiceId */
    view.serviceId = rtaModConfig.SERVICEID.val;

    /* Display whether Rta is enabled to transmit */
    view.enabled = mod.txData;

    /* Display whether Rta is in snapshot waiting mode */
    view.snapshotMode = mod.snapshot;

    /* Display the current period */
    view.period = mod.period;

    /* Display the current sequence number */
    view.sequence = mod.seq;

    /* Display the count of all records sent by the . */
    view.totalPacketsSent = mod.totalPacketsSent;

    /* Display the number of loggers serviced by the Transfer Agent. */
    view.numLoggers = mod.numLoggers;

    /*
     * Display an array of the UIATransfer instances that the Rta is servicing.
     * Also show the logger id (the index in the array).
     */
    try {
        /* Get the array of LoggerBuf handles. */
        var loggerHandles = Program.fetchArray(mod.loggers$fetchDesc,
                                               mod.loggers, mod.numLoggers);

        /* For each handle, get its view, then get its label or address. */
        // TODO support all UIATransfer loggers SDOCM00077586
        for (var i = 0; i < loggerHandles.length; i++) {
            var loggerView = Program.scanHandleView(
                'ti.uia.loggers.LoggerRunMode',
                loggerHandles[i], 'Instances');

            /* If a label is defined, display that. */
            if ((loggerView.label != undefined) && (loggerView.label != "")) {
                view.loggers.$add('[' + i + '] Label: ' + loggerView.label);
            }
            /* Otherwise display the address. */
            else {
                view.loggers.$add('[' + i + '] Handle: ' + loggerView.address);
            }
        }
    }
    catch (e) {
        view.$status["loggers"] = "Caught exception while processing " +
                                  "loggers: " + e.toString();
    }
}
