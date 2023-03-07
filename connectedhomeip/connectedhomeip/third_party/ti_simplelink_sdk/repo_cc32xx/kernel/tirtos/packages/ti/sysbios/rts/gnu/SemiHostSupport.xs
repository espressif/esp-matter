/*
 * Copyright (c) 2014, Texas Instruments Incorporated
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
 *  ======== SemiHostSupport.xs ========
 */

var Hwi = null;
var BIOS = null;
var Startup = null;
var SemiHostSupport = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    SemiHostSupport = this;

    BIOS = xdc.module('ti.sysbios.BIOS');
    Startup = xdc.useModule('xdc.runtime.Startup');

    if (Program.build.target.$name.match(/gnu/)) {
        if (Program.build.target.name == "A8F") {
            Hwi = xdc.module('ti.sysbios.family.arm.a8.intcps.Hwi');
            Hwi.swiFunc = "&SVC_Handler";
        }
        else if ((Program.build.target.name == "A9F") ||
                 (Program.build.target.name == "A15F")) {
            Hwi = xdc.module('ti.sysbios.family.arm.gic.Hwi');
            Hwi.svcFunc = "&SVC_Handler";
        }
    }

    Startup.firstFxns.$add(SemiHostSupport.startup);
}

/*
 *  ======== module$validate ========
 */
function module$validate()
{
    if (!Program.build.target.$name.match(/gnu/)) {
        SemiHostSupport.$logError(
            "This module does not support non-GNU targets.", SemiHostSupport);
    }
}
