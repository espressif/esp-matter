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
 *  ======== package.xs ========
 */
var isEnableDebugPrintf = false;


/*
 *  ======== package.init ========
 */
function init() {
    var syncId = 0;
    var flagBit = 1;
    var IUIACtx = xdc.module('ti.uia.events.IUIACtx');

    if (xdc.om.$name != 'cfg') {
        return;
    }

    /*
    for each (var mod in this.$modules) {
        if (mod.$spec.getFunc() == 'ti.uia.events.Main'
            || mod.$name == 'ti.uia.events.Defaults') {
            continue;
        }
        mod.common$.logger = null;
        for (var dl in mod.common$) {
            if (dl.match(/^diags_/) && dl != 'diags_ASSERT') {
                mod.common$[dl] = this.Diags.ALWAYS_OFF;
            }
        }
    }
     */

    /* initialize constants used by UIA Context modules to define
     * flags used by the context filter and IDs used to
     * inject synchronization info into the trace stream
     */
    for each (var mod in this.$modules) {
        if (mod instanceof IUIACtx.Module) {
            mod.ENABLEMASK = flagBit;
            flagBit = flagBit << 1;
            mod.SYNCID = syncId;
            syncId += 1;
        }
    }
    if (isEnableDebugPrintf)
        print("UIA: ti.uia.events.Package.xs init()");

}

/*
 *  ======== package.close ========
 */
function close() {

}

/*
 *  ======== checkProxies ========
 */
function checkProxies( unit) {
    if (!('$$proxies' in unit) || unit.PROXY$ || '$$proxyCheck' in unit) {
        return;
    }

    unit.$$bind('$$proxyCheck', 1);

    for (var i = 0; i < unit.$$proxies.length; i++) {
        var p = unit.$$proxies[i];
        var pud = unit[p + '$proxy'];

        if (!pud.delegate$) {
            throw        new Error("unbound proxy: " + pud.$name);
        }

        if ('PROXY$' in pud.delegate$ && pud.delegate$.PROXY$) {
            throw new Error("proxy cannot be bound to another proxy: "
                    + pud.delegate$.$name);
        }
    }
}

/*
 *  ======== finalize ========
 *  Called by the configuration model to initialize all common module
 *  settings
 */
function finalize() {
    if (Program.system === null) {
        return;
    }
    if (isEnableDebugPrintf)
        print("UIA: ti.uia.runtime.package.xs finalize()");

    var Defaults = xdc.module('xdc.runtime.Defaults');
    var System = xdc.module('xdc.runtime.System');


    /* if System gate is defined and Defaults isn't, make the System gate
     * the default
     */
    for each (var p in ['gate', 'gateParams']) {
        if (System.common$[p] !== undefined && !Defaults.common$[p]) {
            Defaults.common$[p] = System.common$[p];
        }
    }

    /* propagate the Defaults gate to all modules needing one; if the
     * Defaults gate is null, use GateNull
     */
    for each (var mod in xdc.om.$modules) {
        if (!mod.$used || mod.$hostonly || mod.PROXY$ || mod.$$scope == -1) {
            continue;
        }
        if (!mod.$spec.attrBool('@Gated')) {
            if (mod.common$.gate !== undefined
                    && mod.$name != "xdc.runtime.Defaults") {
                mod.$logWarning("this module is non-gated but it was assigned "
                        + "a non-null gate", mod);
            }
            continue;
        }

        /* TODO: validate gateObj/gatePrms come from same module if former
         * is non-null
         */
        mod.Module__gateObj = (mod.common$.gate !== undefined) ?
                mod.common$.gate : Defaults.common$.gate;
        mod.Module__gatePrms = (mod.common$.gateParams !== undefined) ?
                mod.common$.gateParams : Defaults.common$.gateParams;
        mod.Module_GateProxy = mod.Module__gateObj ?
                mod.Module__gateObj.$module : xdc.useModule('xdc.runtime.GateNull');
    }

    /* for all used target module in the configuration ... */
    for each (var mod in xdc.om.$modules) {

        if (mod.$hostonly || !mod.$used) {
            continue;
        }

        if (mod.$spec.attrBool('@NoRuntime')) {
            mod.$used = 0;
            continue;
        }

        checkProxies(mod);

        /*  Unless otherwise specified, force some modules to be named.
         *    Main  : This improves the error messages that come from calls to
         *            Error_raise() and Log_print/write in "non module" code.
         *    Memory: This improves the error message from the call to
         *            Error_raise in Memory_alloc().
         */
        if ((mod.$name == "ti.uia.runtime.CtxFilter"
            || mod.$name == "ti.uia.runtime.Log"
            || mod.$name == "ti.uia.runtime.LogSnapshot"
            || mod.$name == "ti.uia.runtime.LogSync"
            || mod.$name == "ti.uia.runtime.LoggerTypes") && mod.$$scope != -1) {
            if (mod.common$.namedModule === undefined) {
                if (isEnableDebugPrintf)
                    print("UIA ti.uia.runtime.package.xs : found  "+
                    "%s.common$.namedModule === undefined.",mod.$name);
                mod.common$.namedModule = true;
            }
        }

        /* prevent unintended logging of loggers (usually a mistake) */
        if (mod instanceof xdc.om["xdc.runtime.ILogger"].Module) {
            if (mod.common$.logger === undefined) {
                mod.common$.logger = null;
            }
        }

        /* prevent logging of sync point events if LogSync is disabled */
        if (mod instanceof xdc.om["ti.uia.runtime.LogSync"].Module) {
            if (!mod.isEnabled) {
                mod.common$.logger = null;
                mod.common$.Module__loggerDefined = false;
                if (isEnableDebugPrintf)
                    print("UIA ti.uia.runtime.package.xs LogSync.isEnabled = "+
                    "false - setting LogSync.common$.logger = null");
                }
        }

        /* propagate Defaults to all used target modules, except the ones
         * imported from ROM assemblies. Those modules are already exported
         * in ROM asseblies and the generation code will not look at them
         * when building an application.
         */
        if (mod.$$scope != -1) {
            for (var p in Defaults.common$) {
                if (mod.common$[p] === undefined) {
                    mod.common$[p] = Defaults.common$[p];
                }
            }
        }

        if ((mod.common$.logger == null) &&
            (!(mod instanceof xdc.om["ti.uia.runtime.LogSync"].Module))) {
            if (isEnableDebugPrintf)
               print("UIA ti.uia.runtime.package.xs %s.common$.logger = ",
               mod.$name, mod.common$.logger);
        }
    }
}



/*
 *  ======== validate ========
 *  This function is called during the validation phase of configuration
 */
function validate() {
    /* for each module in this package ... */
    for each (var mod in this.$modules) {

        /* if the module's capsule has a validate function, run it */
        var cap = mod.$capsule;
        if (cap && "validate" in cap) {
            cap.validate.apply(mod, []);
        }
    }
}
