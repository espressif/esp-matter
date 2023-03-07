/*
 * Copyright (c) 2018, Texas Instruments Incorporated
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
 *  =========================== thread.component.js ===========================
 *  Component definition for the SysConfig Thread module
 */

"use strict";

const deviceId = system.deviceData.deviceId;
const displayName = "TI-OpenThread";
let topModules = [];
let templates = [];

/* Only export TIOP module if device is supported */
if(deviceId.match(/CC(265[12]|2653|2654|1352|1354(R1|P1|P7|P10))|CC2674R10/))
{
    topModules = [
        {
            displayName: "RF Stacks",
            description: "RF Stacks",
            modules: ["/ti/thread/thread"]
        }
    ];
    templates = [
        {
            name: "/ti/thread/templates/tiop_config.h.xdt",
            outputPath: "tiop_config.h"
        },
        {
            name: "/ti/thread/templates/tiop_config.c.xdt",
            outputPath: "tiop_config.c"
        }
    ];
}


exports = {
    displayName: displayName,
    topModules: topModules,
    templates: templates
};
