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
 * ======== HeapCallback.syscfg.js ========
 */

/*
 * ======== moduleInstances ========
 */
function moduleInstances (mod)
{
    let modInstances = new Array();

    modInstances.push({
        name: "heapCallbackInit",
        displayName: "HeapCallback_init()",
        moduleName: "ti/sysbios/runtime/Startup",
        hidden: true,
        readOnly: true,
        requiredArgs: {
            startupFxn: "HeapCallback_init",
            functionType: "Module Init Function",
            externRequired: false,
            priority: 100,   /* don't particularly care when init called */
            $name: "HeapCallback_init"
        }
    });

    return (modInstances);
}

exports = {
    displayName: "HeapCallback",
    moduleStatic: {
        name: "moduleGlobal",
        moduleInstances: moduleInstances,
        config: [
            {
                name: "allocInstFxn",
                description: `User supplied instance alloc function`,
                longDescription: `
This function is called when HeapCallback_alloc() is called.

This parameter is configured with a default alloc function. The default alloc
function returns NULL.

                `,
                displayName: "alloc function callback",
                default: "HeapCallback_defaultAlloc"
            },
            {
                name: "createInstFxn",
                description: `User supplied instance create function`,
                longDescription: `
This function is called when HeapCallback_create() is called.

This parameter is configured with a default create function. The default
create function returns 0.

                `,
                displayName: "create function callback",
                default: "HeapCallback_defaultCreate"
            },
            {
                name: "deleteInstFxn",
                description: `User supplied instance delete function`,
                longDescription: `
This function is called when HeapCallback_delete() is called.

This parameter is configured with a default delete function.

                `,
                displayName: "delete function callback",
                default: "HeapCallback_defaultDelete"
            },
            {
                name: "freeInstFxn",
                description: `User supplied instance free function`,
                longDescription: `
This function is called when HeapCallback_free() is called.

This parameter is configured with a default free function.

                `,
                displayName: "free function callback",
                default: "HeapCallback_defaultFree"
            },
            {
                name: "getStatsInstFxn",
                description: `User supplied getStats function`,
                longDescription: `
This function is called when HeapCallback_getStats() is called.

This parameter is configured with a default getStats function. The default
getStats function does not make any changes to stats structure.

                `,
                displayName: "getStats function callback",
                default: "HeapCallback_defaultGetStats"
            },
            {
                name: "initFxn",
                description: `User supplied init function`,
                longDescription: `
This function is called at boot time to initialize the custom heap manager.

This parameter is configured with a default init function.  The default
init function is a noop and does nothing.

                `,
                displayName: "init function callback",
                default: "HeapCallback_defaultInit"
            },
            {
                name: "isBlockingInstFxn",
                description: `User supplied isBlocking function`,
                longDescription: `
This function is called when HeapCallback_isBlocking() is called.

This parameter is configured with a default isBlocking function.  The default
isBlocking returns FALSE.

                `,
                displayName: "isBlocking function callback",
                default: "HeapCallback_defaultIsBlocking"
            }
        ]
    },
    templates: {
        bios_config_h: "/ti/sysbios/heaps/HeapCallback_config.h.xdt",
        "/ti/utils/rov/syscfg_c.rov.xs.xdt": "/ti/sysbios/heaps/HeapCallback.rov.js"
    },
    getCFiles: () => { return ["ti/sysbios/heaps/HeapCallback.c"] }
};
