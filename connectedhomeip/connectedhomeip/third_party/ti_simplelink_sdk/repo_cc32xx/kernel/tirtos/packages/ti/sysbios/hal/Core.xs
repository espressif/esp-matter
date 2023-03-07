/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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
 *  ======== Core.xs ========
 *
 *
 */

var Core = null;
var CoreDelegate = null;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    Core = this;

    Core.common$.fxntab = false;

    if ((Core.CoreProxy === undefined) || Core.CoreProxy == null) {
        var Settings = xdc.module("ti.sysbios.family.Settings");
        CoreDelegate = Settings.getDefaultCoreDelegate();
        if (CoreDelegate == null) {
            CoreDelegate = "ti.sysbios.hal.CoreNull";
        }
        Core.CoreProxy = xdc.useModule(CoreDelegate, true);
    }

    /*
     * Push down common$ settings to the delegates
     */
    for (var dl in Core.common$) {
        if (dl.match(/^diags_/) || dl.match(/^logger/)) {
            /*
             * Extra check below to check if CoreProxy delegate is in ROM.
             * If delegate is in ROM, do NOT push down common$ settings
             */
            if (Core.CoreProxy.delegate$.$$scope != -1) {
                Core.CoreProxy.delegate$.common$[dl] = Core.common$[dl];
            }
        }
    }

    Core.numCores = Core.CoreProxy.numCores;
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    var Build = xdc.module("ti.sysbios.Build");

    /* add -D to compile line to optimize exception code */
    Build.ccArgs.$add("-Dti_sysbios_hal_Core_numCores__D=" + Core.numCores);
}
