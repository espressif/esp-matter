/*
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
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
 *
 */

/*
 * ======== Startup.syscfg.js ========
 */

/* global exports, system */

/* use bigInt for zero padding hex numbers */
let bigInt = system.utils.bigInt;

var instConfigs = [
    {
        name: "startupFxn",
        displayName: "Startup Function",
        placeholder: "<a Startup Function is Required>",
        default: ""
    },
    {
        name: "functionType",
        displayName: "Startup Function Type",
        default: "First Function",
        options: [
            { name: "Reset Function", displayName: "Reset Function" },
            { name: "First Function", displayName: "First Function" },
            { name: "Module Init Function", displayName: "Module Init Function" },
            { name: "Last Function", displayName: "Last Function" }
        ]
    },
    {
        name: "priority",
        displayName: "Priority",
        default: 1
    },
    {
        /*
         * funcs declared in included header files don't
         * need 'extern func' inserted in generated .h file
         */
        name: "externRequired",
        hidden: true,
        default: true
    }
];

exports = {
    displayName: "Startup",
    defaultInstanceName: "Startup",
    config: instConfigs,
    moduleStatic: {
        name: "moduleGlobal",
        config: [
            {
                name: "startupEnabled",
                displayName: "Startup Enabled",
                default: true
            }
        ]
    },
    uiAdd: "staticAndInstance",
    templates: {
        bios_config_h: "/ti/sysbios/runtime/Startup_config.h.xdt",
        bios_config_c: "/ti/sysbios/runtime/Startup_config.c.xdt"
    },
    getCFiles: () => { return ["ti/sysbios/runtime/Startup.c"] }
};

