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
 *  ======== SysCallback.syscfg.js ========
 */

"use strict";

/*
 * ======== moduule_validate ========
 */
function module_validate(mod, validation)
{
    let System = system.modules["/ti/sysbios/runtime/System"];

    if (System.$static.supportModule != "SysCallback") {
        validation.logError("SysCallback can't be used while System.suppportModule == \"SysMin\"", mod);
    }
}

/*
 *  ======== base ========
 *  Define the base NVS properties and methods
 */
let base = {
    staticOnly: true,
    displayName: "SysCallback",
    moduleStatic: {
        name: "moduleGlobal",
        validate: module_validate,
        config: [ 
            {
                name: "abortFxn",
                description: `User supplied abort function`,
                longDescription: `
This function is called when the application calls System_abort() function.
If the user supplied funtion returns, the abort function of the ANSI C
Standard library is called. For more information see the System_abort()
documentation.

By default, this function is configured with a default abort function. This
default abort function spins forever and never returns.

                `,
                displayName: "User supplied abort function",
                default: "SysCallback_defaultAbort"
            },
            {
                name: "exitFxn",
                description: `User supplied exit function`,
                longDescription: `
This function is called when the application calls System_exit() function. If
the user supplied function returns, the ANSI C Standard Library atexit
processing will be completed. For more information see the System_exit()
documentation.

By default, this function is configured with a default exit function. The
default exit function does nothing and returns.

                `,
                displayName: "User supplied exit function",
                default: "SysCallback_defaultExit"
            },
            {
                name: "flushFxn",
                description: `User supplied flush function`,
                longDescription: `
This function is called when the application calls System_flush() function.

By default, this function is configured with a default flush function. The
default flush function does nothing and returns.

                `,
                displayName: "User supplied flush function",
                default: "SysCallback_defaultFlush"
            },
            {
                name: "initFxn",
                displayName: "User supplied init function",
                default: "SysCallback_defaultInit"
            },
            {
                name: "putchFxn",
                description: `User supplied character output function`,
                longDescription: `
This function is called whenever the \`System\` module needs to output a
character; e.g., during System_printf() or System_putch().

By default, this function is configured with a default putch function. The
default putch function drops the characters.

                `,
                displayName: "User supplied output function",
                default: "SysCallback_defaultPutch"
            },
            {
                name: "readyFxn",
                description: `User supplied ready function`,
                longDescription: `
This function is called by the \`System\` module prior to performing any
character output to check if the \`SystemSupport\` module is ready to accept
the character.

By default, this function is configured with a default ready function. The
default ready function returns \`true\` always.

                `,
                displayName: "User supplied ready function",
                default: "SysCallback_defaultReady"
            },
        ]
    },
    templates: {
        bios_config_h: "/ti/sysbios/runtime/SysCallback_config.h.xdt"
    }
};

exports = base;
