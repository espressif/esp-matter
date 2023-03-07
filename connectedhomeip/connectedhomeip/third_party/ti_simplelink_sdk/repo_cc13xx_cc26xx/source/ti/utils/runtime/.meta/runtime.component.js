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
 *  ======== runtime.component.js ========
 *  Define the SysConfig modules in this component
 */

"use strict";

const displayName = "TI Utils Runtime (experimental)";

let description =
    "TI Utils Run-time provides foundational components for target software.";

let desc_execGraph =
    "Configuration and data connectors for the Execution Graph ROV Addon";

let desc_instrumentation =
    "Enable run-time instrumentation events";

let topModules = [
    {
        displayName: "Instrumentation",
        description: desc_instrumentation,
        expanded: false,
        modules: [
            "/ti/utils/runtime/Bench",
            "/ti/utils/runtime/LogMain",
            "/ti/utils/runtime/LogSite",
            "/ti/utils/runtime/LoggerText"
        ]
    },
    {
        displayName: "TI Utils Run-time",
        description: description,
        categories: [
            {
                displayName: "Execution Graph",
                description: desc_execGraph,
                expanded: false,
                modules: [
                    "/ti/utils/runtime/ExecGraphAdapter",
                    "/ti/utils/runtime/ExecGraphFilter",
                    "/ti/utils/runtime/ExecGraphLogger",
                    "/ti/utils/runtime/ExecGraphSettings",
                    "/ti/utils/runtime/ExGr_SYSBIOS",
                    "/ti/utils/runtime/ExGr_XDCtools",
                    "/ti/utils/runtime/ExGr_tiutils"
                ]
            }
        ]
    }
];

let templates = [
    {
        name: "/ti/utils/runtime/Config.c.xdt",
        outputPath: "ti_utils_runtime_config.c"
    },
    {
        name: "/ti/utils/runtime/Config.h.xdt",
        outputPath: "ti_utils_runtime_config.h"
    },
    {
        name: "/ti/utils/runtime/model.gv.xdt",
        outputPath: "ti_utils_runtime_model.gv",
        alwaysRun: true
    },
    {
        name: "/ti/utils/runtime/Makefile.xdt",
        outputPath: "ti_utils_runtime_Makefile",
        alwaysRun: true
    },
    {
        name: "/ti/utils/runtime/ExecGraph.js.xdt",
        outputPath: "ti_utils_runtime_execgraph.js.xs"
    },
    {
        name: "/ti/utils/runtime/rov.js.xdt",
        outputPath: "ti_utils_runtime_rov.js.xs"
    },
    {
        name: "/ti/utils/runtime/LogSite.cmd.xdt",
        outputPath: "ti_utils_runtime_LogSite.cmd.genlibs"
    }
];

exports = {
    displayName,
    topModules,
    templates
};
