/*
 * Copyright (c) 2019-2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== LoggerBuf.syscfg.js ========
 */

"use strict";


/*
 *  ======== config_instance ========
 *  Define the config params of the module instance
 */
let config_instance = [
    {
        name: "label",
        displayName: "Label",
        default: ""
    },
    {
        name: "bufType",
        displayName: "Buffer Type",
        default: "LoggerBuf_Type_CIRCULAR",
        options: [
            {
                name: "LoggerBuf_Type_CIRCULAR",
                displayName: "Circular Buffer"
            },
            {
                name: "LoggerBuf_Type_LINEAR",
                displayName: "Linear Buffer"
            }
        ]
    },
    {
        name: "numEntries",
        displayName: "Number of Entries",
        default: 100
    }
];

/*
 *  ======== modules ========
 *  Express dependencies for other modules
 */
function modules(inst)
{
    let modules = new Array();

    modules.push({
        name: "interrupt",
        displayName: "Interrupt",
        moduleName: "/ti/utils/osal/Interrupt"
    });

    modules.push({
        name: "timestamp",
        displayName: "Timestamp",
        moduleName: "/ti/utils/osal/Timestamp"
    });

    modules.push({
        name: "getlibs",
        displayName: "GetLibs",
        moduleName: "/ti/loggers/utils/GetLibs"
    });

    return (modules);
}

/*
 *  ======== base ========
 *  Module definition object
 */
let base = {
    displayName: "LoggerBuf",
    description: "LoggerBuf module",
    ilogger: true, /* remove when interfaces are supported */
    config: config_instance,
    defaultInstanceName: "CONFIG_ti_loggers_utils_LoggerBuf_",
    modules: modules,

    templates: {
        "/ti/loggers/utils/Config.c.xdt":
            "/ti/loggers/utils/LoggerBuf.Config.c.xdt",
        "/ti/loggers/utils/Config.h.xdt":
            "/ti/loggers/utils/LoggerBuf.Config.h.xdt",
        "/ti/utils/rov/syscfg_c.rov.xs.xdt":
            "/ti/loggers/utils/LoggerBuf.rov.js",
        "/ti/utils/runtime/rov.js.xdt":
            "/ti/loggers/utils/LoggerBuf.rov.js.xdt"
    }
};

/* export the module */
exports = base;
