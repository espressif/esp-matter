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
 * ======== Load.syscfg.js ========
 */

/* global exports, system */

/*
 * ======== moduleInstances ========
 */
function moduleInstances (mod)
{
    let modInstances = new Array();

    modInstances.push({
        name: "loadInit",
        displayName: "Load_init()",
        moduleName: "ti/sysbios/runtime/Startup",
        hidden: true,
        readOnly: true,
        requiredArgs: { 
            startupFxn: "Load_init",
            functionType: "Module Init Function",
            externRequired: false,
            priority: 0,
            $name: "Load_init"
        }
    });

    /*
     *  Load_hwiBeginHook will pull in appropriate
     *  code depending on hwiEnabled.  This way, we
     *  have one Hwi hook function if both are enabled.
     */
    if (mod.hwiEnabled) {
        modInstances.push({
            name: "Load_HwiHooks",
            displayName: "Hwi hooks for Load module",
            moduleName: "ti/sysbios/hal/HwiHooks",
            hidden: true,
            readOnly: true,
            requiredArgs: {
                beginFxn: "Load_hwiBeginHook",
                endFxn: "Load_hwiEndHook",
                $name: "Load_HwiHooks"
            }
        });
    }

    if (mod.swiEnabled) { //TODO: how to check BIOS.swiEnabled?
        modInstances.push({
            name: "Load_SwiHooks",
            displayName: "Swi hooks for Load module",
            moduleName: "ti/sysbios/knl/SwiHooks",
            hidden: true,
            readOnly: true,
            requiredArgs: {
                beginFxn: "Load_swiBeginHook",
                endFxn: "Load_swiEndHook",
                $name: "Load_SwiHooks"
            }
        });
    }

    if (mod.taskEnabled) { //TODO: how to check BIOS.taskEnabled?
        modInstances.push({
            name: "Load_TaskHooks",
            displayName: "Task hooks for Load module",
            moduleName: "ti/sysbios/knl/TaskHooks",
            hidden: true,
            readOnly: true,
            requiredArgs: {
                registerFxn: "Load_taskRegHook",
                createFxn: "Load_taskCreateHook",
                switchFxn: "Load_taskSwitchHook",
                deleteFxn: "Load_taskDeleteHook",
                $name: "Load_TaskHooks"
            }
        });
    }

    modInstances.push({
            name: "Load_idleFxn",
            displayName: "Idle Function for Load module",
            moduleName: "ti/sysbios/knl/Idle",
            hidden: true,
            readOnly: true,
            requiredArgs: {
                idleFxn: "Load_idleFxn",
                externRequired: true,/* TODO: is this ok? Got build error otherwise */
                $name: "Load_idleFxn"
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

    modArray.push({
        name      : "Timestamp",
        moduleName: "ti/sysbios/runtime/Timestamp",
        hidden    : true
    });

    modArray.push({
        name      : "Queue",
        moduleName: "ti/sysbios/knl/Queue",
        hidden    : true
    });

    return (modArray);
}

exports = {
    staticOnly: true,
    displayName: "Load",
    moduleStatic: {
        name: "moduleGlobal",
        modules: modules,
        moduleInstances: moduleInstances,
        //validate: validate,
        config: [
            {
                name: "postUpdate",
                description: `User function called after each load update`,
                longDescription: `
If non-NULL, postUpdate is run immediately after each update.

                `,
                displayName: "Post Update function",
                default: "NULL",
            },
            {
                name: "updateInIdle",
                description: `Automatically call update in Idle loop`,
                longDescription: `
If this parameter is set to \`true\`, an Idle function, which computes and
updates the CPU load, is added to the Idle loop.

                `,
                displayName: "Update in Idle",
                default: true,
            },
            {
                name: "windowInMs",
                description: `Load averaging period`,
                longDescription: `
When updateInIdle is enabled, this period determines the minimum elapsed time
between updates of the load statistics in the idle loop.

                `,
                displayName: "Window size in milliseconds",
                default: 500,
            },
            {
                name: "hwiEnabled",
                description: `Enables Hwi time monitoring`,
                longDescription: `
When this parameter is set to \`true\`, Hwi processing time is accumulated
between successive calls of update and, as part of Load_update, an event with
this cumulative time is logged.  This time, together with the total elapsed
time, is used to estimate the CPU load due to any Hwi instances run between
these updates.

                `,
                displayName: "Enable Hwi load monitoring",
                default: false,
            },
            {
                name: "swiEnabled",
                description: `Enables Swi time monitoring`,
                longDescription: `
When this parameter is set to \`true\`, Swi processing time is accumulated
between successive calls of update and, as part of Load_update, an event with
this cumulative time is logged.  This time together with the total elapsed
time is used to estimate the CPU load due to any Swi instances run between
these updates.

                `,
                displayName: "Enable Swi load monitoring",
                default: false,
            },
            {
                name: "taskEnabled",
                description: `Enables Task time monitoring`,
                longDescription: `
When this parameter is set to \`true\`, each Task's processing time is
accumulated between successive calls of update and, as part of Load_update,
an event with this cumulative time is logged for each task.  This time
together with the total elapsed time is used to estimate the CPU load due to
each Task run between these updates.

                `,
                displayName: "Enable Task load monitoring",
                default: true,
            },
            {   /* no_doc */
                name: "autoAddTasks",
                description: `Automatically add all tasks`,
                displayName: "Automatically add all tasks",
                default: true,
            },
            {
                name: "enableCPULoadCalc",
                description: `Automatically update the Load module's CPU load
                value in update.`,
                longDescription: `
If this parameter is set to \`true\`, Load_update() will calculate the CPU
load for the elapsed time.  In some cases, the user may prefer to use the
statistics gathered by the Load module and do the CPU load calculation
themself.  Set this parameter to false, to disable the Load module's CPU load
calculations.  This can improve performance of the Load_update() call.

                `,
                displayName: "Enable CPU load calculation",
                default: true,
            },
            {
                name: "minIdle",
                description: `Specifies the minimum time used to compute idle
                time`,
                longDescription: `
During CPU load measurement, the minimum time observed executing the idle
loop together with the count of the number of times the idle loop runs id
used to compute the total idle time.  However, since the _actual_ time spent
in the idle loop will almost always be greater then the observed minimum, the
idle time is almost always under estimated.

This configuration parameter can be used to compensate for this underestimate
by placing a lower bound on the minimum time used in the CPU measurements.
The minimum idle time used to compute CPU load is guarenteed to be greater
than or equal to \`minIdle\`.

                `,
                displayName: "Minimum time used to compute idle time",
                default: 0,
            },
        ]
    },
    templates: {
        bios_config_h: "/ti/sysbios/utils/Load_config.h.xdt",
        "/ti/utils/rov/syscfg_c.rov.xs.xdt": "/ti/sysbios/utils/Load.rov.js"
    },
    getCFiles: () => { return ["ti/sysbios/utils/Load.c"] }
};

