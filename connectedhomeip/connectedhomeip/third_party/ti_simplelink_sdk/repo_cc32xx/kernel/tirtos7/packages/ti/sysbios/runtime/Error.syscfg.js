/*
 * Copyright (c) 2020-2021, Texas Instruments Incorporated - http://www.ti.com
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
 * ======== Error.syscfg.js ========
 */

/* global exports, system */

exports = {
    staticOnly: true,
    displayName: "Error Handling",
    moduleStatic: {
        name: "moduleGlobal",
        config: [
            {
                name: "policy",
                description: `System-wide error handling policy`,
                longDescription: `
You can use this parameter to decide at the configuration time what happens
when an error is raised. The program can either return back to the caller,
call \`System_abort()\`, or spin indefinitely.
                `,
                displayName: "Error policy",
                default: "Error_UNWIND",
                options: [
                    { name: "Error_UNWIND", displayName: "Error_UNWIND" },
                    { name: "Error_TERMINATE", displayName: "Error_TERMINATE" },
                    { name: "Error_SPIN", displayName: "Error_SPIN" },
                ]
            },
            {
                name: "raiseHook",
                description: `The function to call whenever an error is
                raised`,
                longDescription: `
If set to a non-\`null\` value, the referenced function is always called when
an error is raised, even if the \`Error\` policy is TERMINATE or SPIN.

By default, this function is set to Error_print which causes the error to be
formatted and output via System_printf.  Setting this configuration parameter
to \`null\` indicates that no function hook should be called.

@see #HookFxn @see #print
                `,
                displayName: "optional function to call when an error is raised",
                default: "NULL",
            },
            {
                name: "printDetails",
                description: "use System_printf() to print error details",
                longDescription: `
If set to true, error details will be printed using System_printf().
System_printf() is a fairly big function that uses a lot of flash. You
can set this parameter to false to avoid pulling in System_printf().
                `,
                displayName: "use System_printf() to print details",
                default: true
            },
            {
                name: "retainStrings",
                description: "keep strings in target memory (flash)",
                longDescription: `
If set to true, error strings will be stored in the .const section
which will be loaded and take up space on the target. If set to false,
the error strings will be placed in a separate section .error_str
section which can be be placed in a no load section that won't be
loaded on the target.
                `,
                displayName: "Leave strings in memory (flash)",
                default: true
            },
            {
                name: "addFileLine",
                description: "keep strings in target memory (flash)",
                longDescription: `
Use the C pre-processor's __FILE__ and __LINE__ macros to add file name and line
number to the output string. You can set this parameter to 'false' to save flash
space on the target.
                `,
                displayName: "Add file and line info to error messages",
                default: true
            }
        ]
    },
    templates: {
        bios_config_h: "/ti/sysbios/runtime/Error_config.h.xdt"
    },
    getCFiles: () => { return ["ti/sysbios/runtime/Error.c"] }
};
