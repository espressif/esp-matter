/*
 * Copyright (c) 2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== LogMain.syscfg.js ========
 */

"use strict";

/*
 *  ======== config ========
 */
let config = [
    {
        name: "instGrp",
        displayName: "Instrumentation",
        config: [
            {
                name: "log_INFO1",
                displayName: "Log Level INFO1",
                default: false
            },
            {
                name: "log_INFO2",
                displayName: "Log Level INFO2",
                default: false
            },
            {
                name: "log_INFO3",
                displayName: "Log Level INFO3",
                default: false
            },
            {
                name: "log_INFO4",
                displayName: "Log Level INFO4",
                default: false
            },
            {
                name: "log_INFO5",
                displayName: "Log Level INFO5",
                default: false
            },
            {
                name: "log_INFO6",
                displayName: "Log Level INFO6",
                default: false
            },
            {
                name: "log_WARN",
                displayName: "Log Level WARN",
                default: false
            },
            {
                name: "log_ERROR",
                displayName: "Log Level ERROR",
                default: false
            }
        ]
    }
];

/*
 *  ======== moduleInstances ========
 */
function moduleInstances(inst)
{
    let reqs = new Array();

    reqs.push({
        moduleName: "/ti/utils/runtime/LogSite",
        name: "LogSite",
        requiredArgs: {
            module: "ti_utils_runtime_LogMain",
            enable_INFO1: inst.log_INFO1,
            enable_INFO2: inst.log_INFO2,
            enable_INFO3: inst.log_INFO3,
            enable_INFO4: inst.log_INFO4,
            enable_INFO5: inst.log_INFO5,
            enable_INFO6: inst.log_INFO6,
            enable_WARN:  inst.log_WARN,
            enable_ERROR: inst.log_ERROR
        },
        group: "instGrp"
    });

    return (reqs);
}

/*
 *  ======== base ========
 */
let base = {
    displayName: "LogMain",
    moduleStatic: {
        config: config,
        moduleInstances: moduleInstances
    }
};

/* export the module */
exports = base;
