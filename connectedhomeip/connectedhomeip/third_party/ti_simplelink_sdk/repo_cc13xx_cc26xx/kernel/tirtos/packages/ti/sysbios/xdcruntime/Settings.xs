/*
 * Copyright (c) 2015, Texas Instruments Incorporated
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
 *  ======== Settings.xs ========
 */

var Settings = null;
var System = null;
var systemGate = null;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    var SemThread = xdc.module('xdc.runtime.knl.SemThread');
    var SemProcess = xdc.module('xdc.runtime.knl.SemProcess');

    var GateThread = xdc.module('xdc.runtime.knl.GateThread');
    var GateProcess = xdc.module('xdc.runtime.knl.GateProcess');

    var Thread = xdc.module('xdc.runtime.knl.Thread');
    var Cache = xdc.module('xdc.runtime.knl.Cache');

    /* Initialize any undefined proxies to defaults */
    if (SemThread.Proxy == undefined) {
        SemThread.Proxy = xdc.module('ti.sysbios.xdcruntime.SemThreadSupport');
    }

    if (SemProcess.Proxy == undefined) {
        SemProcess.Proxy = xdc.module('ti.sysbios.xdcruntime.SemProcessSupport');
    }

    if (GateThread.Proxy == undefined) {
        GateThread.Proxy = xdc.module('ti.sysbios.xdcruntime.GateThreadSupport');
    }

    if (GateProcess.Proxy == undefined) {
        GateProcess.Proxy = 
            xdc.module('ti.sysbios.xdcruntime.GateProcessSupport');
    }

    if (Thread.Proxy == undefined) {
        Thread.Proxy = xdc.module('ti.sysbios.xdcruntime.ThreadSupport');
    }

    if (Cache.Proxy == undefined) {
        Cache.Proxy = xdc.module('ti.sysbios.xdcruntime.CacheSupport');
    }
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    Settings = this;
    System = xdc.useModule('xdc.runtime.System');
    var GateHwi = xdc.useModule('ti.sysbios.gates.GateHwi');

    /*
     *  Create a GateHwi to plug into xdc.runtime.System.
     *  This will be the system gate (e.g. gate used in Gate_enterSystem).
     *  Warn the user if they have already plugged this.
     */
    if (System.common$.gate != null) {
        Settings.$logWarning("User should not configure System.common$.gate. " +
                         "Forcing use of GateHwi instead.", this);
    }
    System.common$.gate = GateHwi.create();
    systemGate = System.common$.gate;
}

/*
 *  ======== module$validate ========
 *  Make sure GateHwi is the System gate.
 */
function module$validate()
{
    /* 
     *  Make sure the user did not overwrite the plugging of the
     *  system gate.  The Settings requirements are too strict to let the user
     *  configure this.
     */    
    if (systemGate != System.common$.gate) {
        Settings.$logError("System.common$.gate must be configured by Settings module.", this);
    }
}
