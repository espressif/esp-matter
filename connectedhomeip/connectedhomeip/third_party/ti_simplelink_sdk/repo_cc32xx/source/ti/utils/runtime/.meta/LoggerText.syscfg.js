/*
 * Copyright (c) 2019 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== LoggerText.syscfg.js ========
 *  LoggerText configuration support
 */

"use strict";

/*
 *  ======== modules ========
 *  Express dependencies for other modules
 */
function modules(inst)
{
    let modules = new Array();

    if ("/ti/utils/build/GenLibs" in system.modules) {
        modules.push({
            name: "getlibs",
            displayName: "GetLibs",
            moduleName: "/ti/utils/runtime/GetLibs"
        });
    }

    return (modules);
}

/*
 *  ======== validate ========
 *  Validate given instance and report conflicts
 *
 *  This function is not allowed to modify the instance state.
 */
function validate(inst, vo, getRef)
{
    return;
}

/*
 *  ======== config ========
 *  Define the config params of the module instance
 */
let config = [
    {
        name: "label",
        displayName: "Label",
        default: "Logger"
    },
    {
        name: "bufType",
        displayName: "Buffer Type",
        default: "LoggerText_Type_CIRCULAR",
        options: [
            {
                name: "LoggerText_Type_CIRCULAR",
                displayName: "Circular Buffer"
            },
            {
                name: "LoggerText_Type_LINEAR",
                displayName: "Linear Buffer"
            }
        ]
    },
    {
        name: "numEntries",
        displayName: "Number of Messages",
        default: 100
    },
    {
        name: "textLen",
        displayName: "Message Size (Bytes)",
        default: 40
    }
];

/*
 *  ======== base ========
 *  Module definition object
 */
let base = {
    displayName: "LoggerText",
    description: "LoggerText module",
    config: config,
    modules: modules,
    defaultInstanceName: "CONFIG_ti_utils_runtime_LoggerText_",
    validate: validate,

    templates: {
        "/ti/utils/runtime/Config.c.xdt":
            "/ti/utils/runtime/LoggerText.Config.c.xdt",
        "/ti/utils/runtime/Config.h.xdt":
            "/ti/utils/runtime/LoggerText.Config.h.xdt",
        "/ti/utils/rov/syscfg_c.rov.xs.xdt":
            "/ti/utils/runtime/LoggerText.rov.js"
    }
};

/* export the module */
exports = base;
