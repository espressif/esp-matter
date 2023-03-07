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
 *  ======== System.syscfg.js ========
 */

"use strict";

/*
 * ======== moduleInstances ========
 */
function moduleInstances (mod)
{
    let modInstances = new Array();

    modInstances.push({
        name: "systemInit",
        displayName: "System_init()",
        moduleName: "ti/sysbios/runtime/Startup",
        hidden: true,
        readOnly: true,
        requiredArgs: {
            startupFxn: "System_init",
            functionType: "Module Init Function",
            externRequired: false,
            priority: -1,
            $name: "System_init"
        }
    });

    return (modInstances);
}

/*
 * ======== modules ========
 * Pull in the appropiate dependent modules
 */
function modules(mod)
{
    let modArray = [];

    switch (mod.supportModule) {
        case "SysCallback":
            modArray.push({
                name      : "SysCallback",
                moduleName: "ti/sysbios/runtime/SysCallback",
            });
            break;
        case "SysMin":
            modArray.push({
                name      : "SysMin",
                moduleName: "ti/sysbios/runtime/SysMin",
            });
            break;
    }

    return modArray;
}

/*
 * ======== getCFiles ========
 */
function getCFiles()
{
    let System = system.modules["/ti/sysbios/runtime/System"];
    let cFiles = [ "ti/sysbios/runtime/System.c" ];
 
    if (System.$static.supportModule == "SysCallback") {
        cFiles.push("ti/sysbios/runtime/SysCallback.c");
    }
    else if (System.$static.supportModule == "SysMin") {
        cFiles.push("ti/sysbios/runtime/SysMin.c");
    }
    
    return (cFiles);
}

/*
 *  ======== base ========
 *  Define the base NVS properties and methods
 */
let base = {
    staticOnly: true,
    displayName: "System",
    moduleStatic: {
        name: "moduleGlobal",
        modules: modules,
        moduleInstances: moduleInstances,
        config: [
            {
                name: "supportModule",
                description: `back end support module`,
                longDescription: `
Module used to handle the bulk of System support functionality.
                `,
                displayName: "SystemSupport Module",
                default: "SysMin",
                options: [
                    { name: "SysMin", displayName: "SysMin" },
                    { name: "SysCallback", displayName: "SysCallback" }
                ]
            },
            {
                name: "maxAtexitHandlers",
                description: `Maximum number of dynamic atexit handlers
                allowed in the system.`,
                longDescription: `
Maximum number of System_atexit handlers set during runtime via the
System_atexit() function.

                `,
                displayName: "Maximum Atexit Handlers",
                default: 8
            },
            {
                name: "abortFxn",
                description: `Abort handler function`,
                longDescription: `
This configuration parameter allows user to plug in their own abort function.
By default abortStd which calls ANSI C Standard abort() is plugged in.
Alternatively abortSpin can be plugged which loops infinitely.
                `,
                displayName: "System_abort() function",
                longDescription: `
This configuration parameter allows users to plug in their own abort function.
By default, System_abortStd, which calls ANSI C Standard abort() is plugged in.
Alternatively, System_abortSpin can be plugged which loops infinitely.
                `,
                default: "System_abortStd"
            },
            {
                name: "exitFxn",
                description: `Exit handler function`,
                longDescription: `
This configuration parameter allows user to plug in their own exit function.
By default exitStd which calls ANSI C Standard exit() is plugged in.
Alternatively exitSpin can be plugged which loops infinitely.
                `,
                displayName: "System_exit(int status) function",
                longDescription: `
This configuration parameter allows users to plug in their own exit function.
By default, System_exitStd, which calls ANSI C Standard exit() is plugged in.
Alternatively, System_exitSpin can be plugged which loops infinitely.
                `,
                default: "System_exitStd"
            },
            {
                name: "extendedFormats",
                description: `Optional conversions supported by
                System_printf`,
                longDescription: `
This string specifies the set of optional argument conversion specifiers
required by the application.  By reducing the number of optional conversions
understood by the System printf methods, it is possible to significantly
reduce the code size footprint of the System module.  This configuration
parameter enables one to balance printf functionality against code size
footprint.

The format of this string is simply a concatenated list of the desired
conversion specifiers (with the leading % character).  For example, to
support %f, set extendedFormats to "%f".

To disable all optional converstions, set extendedFormats to null or
the empty string "".

For a complete list of supported extensions, see the System_printf
"Extended_Format_Specifiers" section.

***Note:*** If an optional conversion is used by some part of the application
and it is not specified in extendedFormats, the conversion character(s)
and leading % are treated as ordinary characters to be output.  As a
result, all subsequent arguments will almost certainly be converted using the
wrong conversion specifier!
                `,
                displayName: "Optional System_printf formats",
                default: ""
            },
        ]
    },
    templates: {
        bios_config_h: "/ti/sysbios/runtime/System_config.h.xdt"
    },
    getCFiles: getCFiles
};

exports = base;
