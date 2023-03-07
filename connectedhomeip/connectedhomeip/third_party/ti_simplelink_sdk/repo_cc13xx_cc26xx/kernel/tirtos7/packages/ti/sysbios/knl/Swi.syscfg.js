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
 * ======== Swi.syscfg.js ========
 */

/* global exports, system */

/* use bigInt for zero padding hex numbers */
let bigInt = system.utils.bigInt;

/*
 * ======== moduleInstances ========
 */
function moduleInstances (mod)
{
    let modInstances = new Array();

    modInstances.push({
        name: "swiHooks",
        displayName: "Swi Hooks",
        moduleName: "ti/sysbios/knl/SwiHooks.syscfg.js",
        useArray: true
    });

    modInstances.push({
        name: "swiInit",
        displayName: "Swi_init()",
        moduleName: "ti/sysbios/runtime/Startup",
        hidden: true,
        readOnly: true,
        requiredArgs: {
            startupFxn: "Swi_init",
            functionType: "Module Init Function",
            externRequired: false,
            priority: 1,
            $name: "Swi_init"
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
//    console.log(Object.keys(mod));
    let modArray = new Array();

    modArray.push({
        name      : "SwiHooks",
        moduleName: "ti/sysbios/knl/SwiHooks",
        hidden    : true
    });

    return modArray;
}

exports = {
    staticOnly: true,
    displayName: "Swi",
    moduleStatic: {
        name: "moduleGlobal",
        moduleInstances: moduleInstances,
        modules: modules,
        config: [
            {
                name: "numPriorities",
                description: `Number of Swi priorities supported`,
                longDescription: `
The maximum number of priorities supported is target-specific and depends on
the number of bits in a UInt data type. For 6x and ARM devices the maximum
number of priorities is therefore 32. For the C28x, the maximum number of
priorities is 16.

                `,
                displayName: "Number of Swi Priorities",
                default: 16
            }
        ]
    },
    templates: {
        bios_config_h: "/ti/sysbios/knl/Swi_config.h.xdt",
        "/ti/utils/rov/syscfg_c.rov.xs.xdt": "/ti/sysbios/knl/Swi.rov.js"
    },
    getCFiles: () => { return ["ti/sysbios/knl/Swi.c"] }
};

