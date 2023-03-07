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
 *  ======== LogCtxChg.xs ========
 */
var isEnableDebugPrintf = false;
var DEFAULT_LOGCTXCHG_BUFFERSIZE = 8192;

/*
 *  ======== module$use ========
 */
function module$use() {
        if (isEnableDebugPrintf) {
                print("UIA: LogoCtxChg.xs in module$use().");
        }
        var CtxFilter = xdc.useModule('ti.uia.runtime.CtxFilter');
        var UIAChanCtx = xdc.useModule('ti.uia.events.UIAChanCtx');
        var UIAFrameCtx = xdc.useModule('ti.uia.events.UIAFrameCtx');
        var UIASWICtx = xdc.useModule('ti.uia.events.UIASWICtx');
        var UIAHWICtx = xdc.useModule('ti.uia.events.UIAHWICtx');
        var UIAThreadCtx = xdc.useModule('ti.uia.events.UIAThreadCtx');
        var UIAUserCtx = xdc.useModule('ti.uia.events.UIAUserCtx');
        var UIAAppCtx = xdc.useModule('ti.uia.events.UIAAppCtx');
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

        /* Get the Log module's configuration object. */
        var cfg = Program.getModuleConfig('xdc.runtime.LogCtxChg');

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
        if (isEnableDebugPrintf)
                print("UIA: ti.uia.runtime.LogCtxChg.xs getEventMsg()");
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

                msg
                = xdc.jre.xdc.rta.Formatter.doPrint(Text.fetchAddr(addr),
                                printArgs);
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
                encodedDesc = "(((xdc_runtime_LogCtxChg_Event)" + desc.$private.id
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
        var mod = xdc.module('ti.uia.runtime.LogCtxChg');
        var IFilterLogger = xdc.module('xdc.runtime.IFilterLogger');
        var numModules = 0;

        if (isEnableDebugPrintf) {
                print("UIA: ti.uia.runtime.LogCtxChg.xs module$static$init()");
                if (mod.loggerObj != null)
                        print("UIA: LogCtxChg.xs mod.loggerObj = "+
                                        mod.loggerObj.$orig.$module.$name);
                else
                        print("UIA: LogCtxChg.xs mod.loggerObj = NULL");

                if (mod.Module__loggerObj != null)
                        print("UIA: LogCtxChg.xs mod.Module__loggerObj = "+
                                        mod.Module__loggerObj.$orig.$module.$name);
                else
                        print("UIA: LogCtxChg.xs mod.Module__loggerObj = NULL");
                print("UIA: LogCtxChg.xs mod.loggerDefined = "+mod.loggerDefined);
                print("UIA: LogCtxChg.xs mod.Module__loggerDefined = "+
                                mod.Module__loggerDefined);
        }
        if (mod.loggerObj == null) {
                var newLogger = null;

                if ((mod.common$.logger != null) &&
                                (mod.common$.logger.$orig.$module instanceof IFilterLogger.Module)) {
                        if (isEnableDebugPrintf) {
                                print("UIA: LogCtxChg - using common$.logger "+mod.common$.logger);
                        }
                        newLogger = mod.common$.logger;
                } /*else if (UIAMetaData.isLoggingSetupConfigured()){
                        var LoggingSetup = xdc.module('ti.uia.sysbios.LoggingSetup');
                        var IUIATransfer = xdc.module('ti.uia.runtime.IUIATransfer');
                        newLogger = LoggingSetup.createLogger(DEFAULT_LOGCTXCHG_BUFFERSIZE,
                                        "UIACtxChg", IUIATransfer.Priority_HIGH);
                        if (isEnableDebugPrintf) {
                                print("UIA: LogCtxChg - using dedicated logger "+newLogger);
                        }
                }*/ else {
                        var Main = xdc.module('xdc.runtime.Main');
                        var IFilterLogger = xdc.module('xdc.runtime.IFilterLogger');
                        if (isModuleUsedInBuild(Main)){
                                if ((Main.common$.logger != null) &&
                                                (Main.common$.logger.$orig.$module instanceof IFilterLogger.Module)) {
                                        if (isEnableDebugPrintf) {
                                                print("UIA: LogCtxChg - using Main.common$.logger "+
                                                                Main.common$.logger);
                                        }
                                        newLogger = Main.common$.logger;
                                }
                        }
                }
                if (newLogger != null){
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

                mod.$unseal('loggerFxn2');
                mod.$unseal('loggerFxn8');
                mod.loggerFxn2 = $externModFxn(cn + '_write2__E');
                mod.loggerFxn8 = $externModFxn(cn + '_write8__E');
                if (isEnableDebugPrintf) {
                        print("UIA: LogCtxChg.loggerFxn2 = "+mod.loggerFxn2);
                        print("UIA: LogCtxChg.loggerFxn8 = "+mod.loggerFxn8);
                }
                mod.$seal('loggerFxn2');
                mod.$seal('loggerFxn8');

                mod.Module__loggerDefined = true;
        } else {
                print("warning: ti.uia.runtime.LogCtxChg does not have a logger defined."+
                                "  Context change event logging disabled.");
        }
}

function isModuleUsedInBuild(module1){
        var result = false
                        for each (var mod in Program.targetModules()) {
                                if (mod.$name == module1.$name){
                                        result = true;
                                        break;
                                }
                        }
        if (isEnableDebugPrintf) {
                print("LogCtxChg.isModuleUsedInBuild("+module1.$name+") returned "+result);
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

}
