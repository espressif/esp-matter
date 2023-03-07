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
 *  ======== LogSnapshot.xs ========
 */
var isEnableDebugPrintf = false;
var DEFAULT_LOGSNAPSHOT_BUFFERSIZE = 32768;

/*
 *  ======== module$use ========
 */
function module$use() {
    var Diags = xdc.useModule('xdc.runtime.Diags');
    var UIASnapshot = xdc.useModule('ti.uia.events.UIASnapshot');
    var IUIATraceSyncClient = xdc.useModule('ti.uia.runtime.IUIATraceSyncClient');
    var IUIATraceSyncProvider = xdc.useModule('ti.uia.runtime.IUIATraceSyncProvider');
    var UIAMetaData = xdc.useModule('ti.uia.runtime.UIAMetaData');
}

/*
 *  ======== decode ========
 *  ROV code
 */
function decode( info, evtId, args) {
    var Program = xdc.useModule('xdc.rov.Program');

    info.modName = Program.moduleIdToName(evtId & 0x0000FFFF);
    info.eventId = evtId >> 16;

    /* Look up the event name. */
    info.eventName = evtIdToName(info.eventId);

    for (var i = 0; i < cfg.NUMARGS; i++) {
        info.arg[i] = args[i];
    }

    /* Get the event's formatted message. */
    info.text = getEventMsg(info.eventId, args);
}

/*
 *  ======== evtIdToName ========
 *  In ROV, lookup an event's name given its id.
 */
function evtIdToName( eventId) {
    var Program = xdc.useModule('xdc.rov.Program');

    /* Get the LogSnapshot module's configuration object. */
    var cfg = Program.getModuleConfig('xdc.runtime.LogSnapshot');

    if (cfg.idToInfo["#" + eventId] != undefined) {
        info.eventName = cfg.idToInfo["#" + eventId].split('::')[0];
    } else if (info.eventId == 0) {
        info.eventName = "Log_print";
    } else {
        info.eventName = "Unrecognized event id: " + eventId;
    }
}

/*
 *  ======== getEventMsg ========
 *  In ROV, look up the record's message based on its event Id, then format it
 *  with the given arguments.
 */
function getEventMsg( eventId, args) {
    var Text = xdc.useModule('xdc.runtime.Text');
    var System = xdc.module('xdc.runtime.System');
    xdc.loadPackage('xdc.rta');

    var msg;

    if (isEnableDebugPrintf) {
        print("UIA: ti.uia.runtime.LogSnapshot.xs getEventMsg()");
    }

    if (eventId == 0) {
        var addr = args[0];

        /*
         * Convert address to unsigned.
         * The arguments are signed IArgs, so the address may be negative and
         * need to be converted.
         */
        if (addr < 0) {
            addr += Math.pow(2, 32);
        }

        /*
         * Shift the arguments over by one. Don't use shift since
         * 'args' may be a java array.
         */
        var printArgs = new Array();
        for (var i = 1; i < args.length; i++) {
            printArgs[i - 1] = args[i];
        }

        msg = xdc.jre.xdc.rta.Formatter.doPrint(Text.fetchAddr(addr), printArgs);
    } else {
        msg = xdc.jre.xdc.rta.Formatter.doPrint(Text.fetchId(eventId), args);
    }

    return (msg);
}

/*
 *  ======== Event$alignof ========
 *  Return alignment required for an encoded value of an EventDesc
 */
function Event$alignof() {
    return (Program.build.target.stdTypes.t_Int32.align);
}

/*
 *  ======== Event$encode ========
 *  Return a C expression that evaluates to the encoded value of an EventDesc
 */
function Event$encode( desc) {
    var encodedDesc = "0";
    if (desc) {
        encodedDesc = "(((xdc_runtime_LogSnapshot_Event)" + desc.$private.id
            + ") << 16 | " + (desc.mask & 0xFFFF) + ")";
    }
    return (encodedDesc);
}

/*
 *  ======== Event$sizeof ========
 *  Return sizeof the encoded value of an EventDesc
 */
function Event$sizeof() {
    return (Program.build.target.stdTypes.t_Int32.size);
}

/*
 *  ======== module$static$init ========
 */
function module$static$init( obj, params) {
    var Text = xdc.module('xdc.runtime.Text');
    var Diags = xdc.module('xdc.runtime.Diags');
    var mod = xdc.module('ti.uia.runtime.LogSnapshot');
    var ILoggerSnapshot = xdc.module('ti.uia.runtime.ILoggerSnapshot');

    if (isEnableDebugPrintf) {
        print("UIA: ti.uia.runtime.LogSnapshot.xs module$static$init()");
        if (mod.loggerObj != null) {
            print("UIA: LogSnapshot.xs mod.loggerObj = "+mod.loggerObj.$orig.$module.$name);
        }
        else {
            print("UIA: LogSnapshot.xs mod.loggerObj = NULL");
        }

        if (mod.Module__loggerObj != null) {
            print("UIA: LogSnapshot.xs mod.Module__loggerObj = "+mod.Module__loggerObj.$orig.$module.$name);
        }
        else {
            print("UIA: LogSnapshot.xs mod.Module__loggerObj = NULL");
        }
        print("UIA: LogSnapshot.xs mod.loggerDefined = "+mod.loggerDefined);
        print("UIA: LogSnapshot.xs mod.Module__loggerDefined = "+mod.Module__loggerDefined);
    }

    if (mod.loggerObj == null) {
        var newLogger = null;
        if ((mod.common$.logger != null) &&
                (mod.common$.logger.$orig.$module instanceof ILoggerSnapshot.Module)) {
            if (isEnableDebugPrintf) {
                print("UIA: LogSnapshot.xs - using common$.logger "+mod.common$.logger);
            }
            newLogger = mod.common$.logger;
        }
        else {
            var Main = xdc.module('xdc.runtime.Main');
            if (isModuleUsedInBuild(Main)) {
                if ((Main.common$.logger != null) &&
                        (Main.common$.logger.$orig.$module instanceof ILoggerSnapshot.Module)) {
                    if (isEnableDebugPrintf) {
                        print("UIA: LogSnapshot.xs - using Main.common$.logger "+Main.common$.logger);
                    }
                    newLogger = Main.common$.logger;
                }
            }
        }

        if (newLogger != null) {
            mod.$unseal('loggerObj');
            mod.$unseal('loggerDefined');
            mod.loggerObj = newLogger;
            mod.loggerDefined = true;
            mod.$seal('loggerObj');
            mod.$seal('loggerDefined');
        }
    }
    if (mod.loggerObj != null) {
        var cn = mod.loggerObj.$orig.$module.$name.replace(/\./g, '_');

        if ("maxEventSize" in mod.loggerObj.$orig) {
            if (mod.loggerObj.$orig.maxEventSize == 0) {
                mod.$logError(mod.loggerObj.$orig.$module.$name + " logger must have maxEventSize > 0 for LogSnapshot. " +
                        "Set LoggingSetup.snapshotLogging = true in your .cfg file to fix this error.", mod);
            }
        }

        mod.$unseal('loggerMemoryRangeFxn');
        mod.loggerMemoryRangeFxn = $externModFxn(cn + '_writeMemoryRange__E');
        if (isEnableDebugPrintf) {
            print("UIA: LogSnapshot.loggerMemoryRangeFxn = "+mod.loggerMemoryRangeFxn);
        }
        mod.$seal('loggerMemoryRangeFxn');

        mod.Module__loggerDefined = true;
    } else {
        print("warning: ti.uia.runtime.LogSnapshot does not have a logger defined.");
    }
}

/*
 *  ======== isModuleUsedInBuild ========
 */
function isModuleUsedInBuild(module1) {
    var result = false;

    for each (var mod in Program.targetModules()) {
        if (mod.$name == module1.$name){
            result = true;
            break;
        }
    }
    if (isEnableDebugPrintf) {
        print("LogSnapshot.isModuleUsedInBuild("+module1.$name+") returned "+result);
    }
    return (result);
}

/*
 *  ======== query ========
 */
function query(mod, dl)
{
    var Defaults = xdc.module('xdc.runtime.Defaults');
    var Diags = xdc.module('xdc.runtime.Diags');

    var sel = 'diags_' + dl;

    return (mod.common$[sel] === undefined
            ? (Defaults.common$[sel] != Diags.ALWAYS_OFF)
                    : (mod.common$[sel] != Diags.ALWAYS_OFF));
}

/*
 *  ======== validate ========
 */
function validate()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    var Diags = xdc.module('xdc.runtime.Diags');

    for (var i = 0; i < xdc.om.$modules.length; i++) {
        var mod = xdc.om.$modules[i];

        /* check all Event's declared in any module */
        if ('$$logEvtCfgs' in mod && mod.$$logEvtCfgs.length > 0) {
            for each (var cn in mod.$$logEvtCfgs) {
                var desc = mod[cn];

                /* check that mask is within UInt16 range */
                if (desc.mask < 0) {
                    this.$logError("value of 'mask' must be non-negative",
                            mod, cn);
                }
                else if (desc.mask > 0xFFFF) {
                    this.$logError("value of 'mask' must be at most 16 bits",
                            mod, cn);
                }
            }
        }
    }
}
