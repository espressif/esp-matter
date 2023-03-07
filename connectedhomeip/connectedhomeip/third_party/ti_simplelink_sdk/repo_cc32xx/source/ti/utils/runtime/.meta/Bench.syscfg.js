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
 *  ======== Bench.syscfg.js ========
 */

"use strict";

/*
 *  ======== validate_instance ========
 *  Validate given instance and report conflicts
 *
 *  This function is not allowed to modify the instance state.
 */
function validate_instance(inst, vo, getRef)
{
    /* given instance label must not conflict with any other instance */
    let instAry = inst.$module.$instances;
    let conflict = false;

    for (let i = 0; i < instAry.length; i++) {
        if (inst.$name == instAry[i].$name) {
            continue; /* skip myself */
        }
        if (inst.mark == instAry[i].mark) {
            conflict = true;
            break;
        }
    }
    if (conflict) {
        vo["mark"].errors.push("Resource conflict");
    }
}

/*
 *  ======== validate_module ========
 *  Validate the module state and report conflicts
 *
 *  This function is not allowed to modify the module state.
 */
function validate_module(inst, vo, getRef)
{
    /* TBD */
    return;
}

/*
 *  ======== config_instance ========
 *  Define the config params of the module instance
 */
let config_instance = [
    {
        name: "mark",
        displayName: "Benchmark Label",
        default: "A0"
    },
    {
        name: "proxy",
        displayName: "Proxy Instance",
        default: false
    },
    {
        name: "section",
        displayName: "Section Name",
        default: ""
    }
];

/*
 *  ======== config_module ========
 *  Define module wide config params
 */
let config_module = [
    {
        name: "numEntries",
        displayName: "Number of Entries",
        default: 100
    },
    {
        name: "timestampHz",
        displayName: "Timestamp Ticks Per Second",
        default: 4000000
    }
];

/*
 *  ======== base ========
 *  Module definition object
 */
let base = {
    displayName: "Bench",
    description: "Bench module",
    config: config_instance,
    defaultInstanceName: "CONFIG_Bench_",
    validate: validate_instance,
    moduleStatic: {
        config: config_module,
        validate: validate_module
    },
    templates: {
        "/ti/utils/runtime/Config.c.xdt":
            "/ti/utils/runtime/Bench.Config.c.xdt",
        "/ti/utils/runtime/Config.h.xdt":
            "/ti/utils/runtime/Bench.Config.h.xdt",
        "/ti/utils/rov/syscfg_c.rov.xs.xdt":
            "/ti/utils/runtime/Bench.rov.js"
    }
};

/* export the module */
exports = base;
