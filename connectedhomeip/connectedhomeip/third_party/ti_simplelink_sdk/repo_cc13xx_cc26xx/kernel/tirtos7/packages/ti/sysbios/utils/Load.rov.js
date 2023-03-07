/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
 *  ======== Load.rov.js ========
 */

var moduleName = "ti.sysbios.utils::Load";

var viewMap = [
    {name: "CPU Load", fxn: "viewLoadModule", structName: "ModuleInfo"}
];

function ModuleInfo()
{
    this.cpuLoad  = null;
    this.swiLoad  = null;
    this.hwiLoad  = null;
    this.idleError  = null;
}

/*
 *  ======== viewLoadModule ========
 */
function viewLoadModule()
{
    var modView = new ModuleInfo();

    try {
        var modState = Program.fetchVariable("ti_sysbios_utils_Load_Module_state");
    }
    catch (e) {
        return (modView);
    }

    var cpuLoadValue = modState.cpuLoad;
    if ((cpuLoadValue > 100) || (cpuLoadValue < 0)) {
        Program.displayError(modView, "cpuLoad",
            "Error: CPU load computation resulted in out of range number: " +
            cpuLoadValue);
    }
    modView.cpuLoad = cpuLoadValue.toFixed(1);

    var swiFunc = Program.lookupSymbolValue("Load_swiBeginHook");
    var swiEnv = modState.swiEnv;
    if (swiFunc == -1) {
        modView.swiLoad = "disabled";
    }
    else {
        if (swiEnv.totalTimeElapsed == 0) {
            modView.swiLoad = "undefined";
        }
        else {
            var swiLoadValue =
                (swiEnv.totalTime / swiEnv.totalTimeElapsed) * 100;
            if ((swiLoadValue > 100) || (swiLoadValue < 0)) {
                Program.displayError(modView, "swiLoad", "Error: Swi load " +
                    "computation resulted in out of range number: " +
                    swiLoadValue);
            }
            modView.swiLoad = swiLoadValue.toFixed(1);
        }
    }

    var hwiFunc = Program.lookupSymbolValue("Load_hwiBeginHook");
    var hwiEnv = modState.hwiEnv;
    if (hwiFunc == -1) {
        modView.hwiLoad = "disabled";
    }
    else {
        if (hwiEnv.totalTimeElapsed == 0) {
            modView.hwiLoad = "undefined";
        }
        else {
            var hwiLoadValue =
                (hwiEnv.totalTime / hwiEnv.totalTimeElapsed) * 100;
            if ((hwiLoadValue > 100) || (hwiLoadValue < 0)) {
                Program.displayError(modView, "hwiLoad", "Error: Hwi load " +
                    "computation resulted in out of range number: " +
                    hwiLoadValue);
            }
            modView.hwiLoad = hwiLoadValue.toFixed(1);
        }
    }

    var minIdle = modState.minLoop > modState.minIdle ?
                      modState.minLoop : modState.minIdle;
    if (minIdle > 0) {
        var err = 1 - ((minIdle - 1) / minIdle);
        modView.idleError = (err * 100).toFixed(1) + "%";
        err = err * (1 - modState.cpuLoad / 100);
        //if ((err >= 0.15) && !LoadCfg.powerEnabled) {
        //    view.$status["idleError"] =
        //        "Warning: estimated error in total CPU load may be off by as much as "
        //        + (err * 100).toFixed(1);
        //}
    }
    else {
        modView.idleError = "unknown";
    }

    return (modView);
}
