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
 *  ======== ExecGraphLogger.syscfg.js ========
 */

"use strict";

/*
 *  ======== config ========
 *  Define the config params of the module instance
 */
let config = [
    {
        name: "label",
        displayName: "Instance Label",
        default: ""
    },
    {
        name: "module",
        displayName: "Module Name",
        default: ""
    },
    {
        name: "view",
        displayName: "View Name",
        default: ""
    },
    {
        name: "fxn",
        displayName: "Function Definition",
        multiline: true,
        default: ""
    }
];

/*
 *  ======== base ========
 *  Module definition object
 */
let base = {
    displayName: "ExecGraphLogger",
    description: "Execution Graph logger data source",
    config: config,
    defaultInstanceName: "Config_ExecGraphLogger_",
    templates: {
        "/ti/utils/runtime/ExecGraph.js.xdt":
            "/ti/utils/runtime/ExecGraphLogger.js.xdt",
        "/ti/utils/rov/syscfg_c.rov.xs.xdt":
            "/ti/utils/runtime/ExecGraphLogger.rov.js"
    }
};

/* export the module */
exports = base;
