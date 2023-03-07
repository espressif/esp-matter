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
 * ======== Assert.syscfg.js ========
 */

/* global exports, system */

exports = {
    staticOnly: true,
    displayName: "Assertion Handling",
    moduleStatic: {
        name: "moduleGlobal",
        config: [
            {
                name: "addFileLine",
                description: "Add file and line info to assert messages",
                longDescription: `
Use the C pre-processor's __FILE__ and __LINE__ macros to add file name and line
number to the output string. You can set this parameter to 'false' to save flash
space on the target.
                `,
                displayName: "Add file and line info to assert messages",
                default: true
            },
            {
                name: "useBkpt",
                description: "Use a software breakpoint for the assert function",
                longDescription: `
If this parameter is set to true, a software breakpoint opcode will be used to
halt the processor when an Assert is raised.
                `,
                displayName: "Use BKPT for assert function",
                default: false
            }
        ]
    },
    templates: {
        bios_config_h: "/ti/sysbios/runtime/Assert_config.h.xdt"
    },
    getCFiles: () => { return ["ti/sysbios/runtime/Assert.c"] }
};
