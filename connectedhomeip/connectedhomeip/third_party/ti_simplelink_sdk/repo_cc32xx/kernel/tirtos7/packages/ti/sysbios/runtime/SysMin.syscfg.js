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
 *  ======== SysMin.syscfg.js ========
 */

"use strict";

/*
 * ======== moduule_validate ========
 */
function module_validate(mod, validation)
{
    let System = system.modules["/ti/sysbios/runtime/System"];

    if (System.$static.supportModule != "SysMin") {
        validation.logError("SysMin can't be used while System.suppportModule == \"SysCallback\"", mod);
    }
}

/*
 *  ======== base ========
 *  Define the base NVS properties and methods
 */
let base = {
    staticOnly: true,
    displayName: "SysMin",
    moduleStatic: {
        name: "moduleGlobal",
        validate: module_validate,
        config: [ 
            {
                name: "outputFunc",
                description: `User supplied character output function`,
                longDescription: `
If this parameter is set to a non-null value, the specified function will
be called by System_flush() to output any characters buffered
within SysMin.
 
For example, if you define a function named "myOutputFxn", the following
configuration fragment will cause SysMin to call "myOutputFxn" whenever the
character buffer is flushed.
@p(code)
var SysMin = scripting.addModule("/ti/sysbios/runtime/SysMin");
SysMin.outputFxn = "myOutputFxn";
@p

If this parameter is not set, a default function will be used which uses the
ANSI C Standard Library function fwrite() (or HOSTwrite in the TI C Run
Time Support library) to output accumulated output characters.
                `,
                displayName: "Character Output function callback",
                default: "SysMin_output"
            },
            {
                name: "bufSize",
                description: `Size (in MAUs) of the output.`,
                longDescription: `
An internal buffer of this size is allocated. All output is stored in this
internal buffer.

If 0 is specified for the size, no buffer is created, all output is dropped,
and SysMin_ready() always returns false.
                `,
                displayName: "Output Buffer Size",
                default: 1024
            },
            {
                name: "flushAtExit",
                description: `Flush the internal buffer during exit or
                abort.`,
                longDescription: `
If the application's target is a TI target, the internal buffer is flushed
via the HOSTwrite function in the TI C Run Time Support (RTS) library.

If the application's target is not a TI target, the internal buffer is
flushed to stdout via fwrite(..., stdout).

Setting this parameter to false reduces the footprint of the application
at the expense of not getting output when the application ends via a
System_exit(), System_abort(), exit() or abort().
                `,
                displayName: "Flush At Exit",
                default: true
            }
        ]
    },
    templates: {
        bios_config_h: "/ti/sysbios/runtime/SysMin_config.h.xdt",
        "/ti/utils/rov/syscfg_c.rov.xs.xdt": "/ti/sysbios/runtime/SysMin.rov.js"
    }
};

exports = base;
