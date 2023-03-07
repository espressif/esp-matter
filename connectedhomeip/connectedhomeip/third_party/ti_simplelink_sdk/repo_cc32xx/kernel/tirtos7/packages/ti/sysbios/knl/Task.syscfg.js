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
 * ======== Task.syscfg.js ========
 */

/* global exports, system */

/* use bigInt for zero padding hex numbers */
let bigInt = system.utils.bigInt;

/* get device specific Settings */
let Settings = system.getScript("/ti/sysbios/family/Settings.js");

/*
 * ======== moduleInstances ========
 */
function moduleInstances (mod)
{
    let modInstances = new Array();

    modInstances.push({
        name: "taskHooks",
        displayName: "Task Hooks",
        moduleName: "ti/sysbios/knl/TaskHooks.syscfg.js",
        useArray: true
    });

    modInstances.push({
        name: "taskInit",
        displayName: "Task_init()",
        moduleName: "ti/sysbios/runtime/Startup",
        hidden: true,
        readOnly: true,
        requiredArgs: {
            startupFxn: "Task_init",
            functionType: "Module Init Function",
            externRequired: false,
            priority: 2,
            $name: "Task_init"
        }
    });

    if (mod.deleteTerminatedTasks) {
        modInstances.push(
            {
                name: "deleteTerminatedTask",
                displayName: "Idle Function that Deletes Terminated Tasks",
                moduleName: "ti/sysbios/knl/Idle",
                hidden: true,
                readOnly: true,
                requiredArgs: {
                    idleFxn: "Task_deleteTerminatedTasksFunc",
                    externRequired: false,
                    $name: "Task_deleteTerminatedTasksFunc"
                }
            }
        );
    }

    return (modInstances);
}

/*
 * ======== modules ========
 * Pull in the appropiate dependent modules
 */
function modules(mod)
{
    let modArray = new Array();

    if (mod.enableIdleTask) {
        modArray.push({
            name      : "Idle",
            moduleName: "ti/sysbios/knl/Idle",
            hidden    : true
        });
    }
    
    modArray.push({
        name      : "TaskSupport",
        moduleName: Settings.taskSupportModule,
        hidden    : true
    });

    modArray.push({
        name      : "TaskHooks",
        moduleName: "ti/sysbios/knl/TaskHooks",
        hidden    : true
    });

    modArray.push({
        name      : "Queue",
        moduleName: "ti/sysbios/knl/Queue",
        hidden    : true
    });

    return modArray;
}

exports = {
    staticOnly: true,
    displayName: "Task",
    moduleStatic: {
        name: "moduleGlobal",
        modules: modules,
        moduleInstances: moduleInstances,
        config: [
            {
                name: "numPriorities",
                displayName: "Number of Task Priorities",
                description: `Number of Task priorities supported. Default is
                16.`,
                longDescription: `
The maximum number of priorities supported is target specific and depends on
the number of bits in a UInt data type. For 6x and ARM devices the maximum
number of priorities is therefore 32. For C28x devices, the maximum number of
priorities is 16.

                `,
                default: 16
            },
            {
                name: "defaultStackSize",
                displayName: "Default Stack Size",
                description: `Default stack size (in MAUs) used for all
                tasks.`,
                longDescription: `
Default is obtained from the device-aware Settings table (
ti/sysbios/family/Settings.js).

                `,
                default: Settings.defaultTaskStackSize
            },
            {
                name: "idleTaskStackSize",
                displayName: "Idle Task's Stack Size",
                description: `Idle task stack size in MAUs`,
                longDescription: `
Default is obtained from the device-aware Settings table (
ti/sysbios/family/Settings.js).
                `,
                default: Settings.defaultTaskStackSize
            },
            {
                name: "idleTaskVitalTaskFlag",
                displayName: "Idle Task's \"Vital Task\" flag",
                description: "Idle task's vitalTaskFlag. Default is true.",
                longDescription: `See the "vitalTaskFlag" decription in the BIOS Users' Guide`,
                default: true
            },
	        {
                name: "allBlockedFunc",
                displayName: "All Blocked Function",
                description: `Function to call while all tasks are blocked.`,
                longDescription: `

This function will be called repeatedly while no tasks are
ready to run.

Ordinarily (in applications that have tasks ready to run at startup),
the function will run in the context of the last task to block.

In an application where there are no tasks ready to run
when BIOS_start() is called, the allBlockedFunc function is
called within the BIOS_start() thread which runs on the system/ISR
stack.

By default, allBlockedFunc is initialized to point to an internal
function that simply returns.

By adding the following lines to the config script, the Idle
functions will run whenever all tasks are blocked:

~~~
Task.enableIdleTask = false;
Task.allBlockedFunc = "Idle_run";
~~~
                `,
                default: "NULL"
	        },
            {
                name: "initStackFlag",
                displayName: "Initialize Stack Flag",
                description: `Initialize stack with known value for stack
                checking at runtime (see checkStackFlag). If this flag is set
                to false, while the ti.sysbios.hal.Hwi#checkStackFlag is set
                to true, only the first byte of the stack is initialized.`,
                longDescription: `
This is also useful for inspection of stack in debugger or core dump
utilities. Default is true.

                `,
                default: true
            },
            {
                name: "checkStackFlag",
                displayName: "Check Stack Flag",
                description: `Check 'from' and 'to' task stacks before task
                context switch.`,
                longDescription: `
The check consists of testing the top of stack value against its initial
value (see initStackFlag). If it is no longer at this value, the assumption
is that the task has overrun its stack. If the test fails, then the
E_stackOverflow error is raised.

Default is true.

To enable or disable full stack checking, you should set both this flag and
the ti.sysbios.hal.Hwi#checkStackFlag.

***note:*** Enabling stack checking will add some interrupt latency because
the checks are made within the Task scheduler while interrupts are disabled.

                `,
                default: true
            },
            {
                name: "enableIdleTask",
                displayName: "Enable Idle Task",
                description: "Construct a task (of priority 0) to run the Idle functions in.",
                longDescription: `
When set to true, a task is constructed that continuously calls the
Idle_run() function, which, in turn calls each of
the configured Idle functions.

When set to false, no Idle Task is created and it is up to the
user to call the Idle_run() function if the configured Idle
functions need to be run. Or, by adding the following lines to
the config script, the Idle functions will run whenever all
tasks are blocked:
~~~
Task.enableIdleTask = false;
Task.allBlockedFunc = "Idle_run";
~~~
                `,
                default: true
            },
            {
                name: "minimizeLatency",
                displayName: "Minimize Latency",
                description: "Reduce interrupt latency by enabling interrupts within the Task scheduler.",
                longDescription: `
By default, interrupts are disabled within certain critical
sections of the task scheduler when switching to a different
task thread. This default behavior guarantees that a task stack
will only ever absorb ONE ISR context. Nested interrupts all run
on the shared Hwi stack.

While most users find this behavior desirable, the resulting
impact on interrupt latency is too great for certain applications.

By setting this parameter to 'true', the worst case interrupt latency
imposed by the kernel will be reduced but will result in task stacks
needing to be sized to accommodate one additional interrupt context.
                `,
                default: false
            },
            {
                name: "deleteTerminatedTasks",
                displayName: "Delete Terminated Tasks",
                description: `Automatically delete terminated tasks.`,
                longDescription: `
If this feature is enabled, an Idle function is installed that deletes
dynamically created Tasks that have terminated either by falling through
their task function or by explicitly calling Task_exit().

A list of terminated Tasks that were created dynmically is maintained
internally. Each invocation of the installed Idle function deletes the first
Task on this list. This one-at-a-time process continues until the list is
empty.

***note:*** This feature is disabled by default.

***warning:*** When this feature is enabled, an error will be raised if the
user's application attempts to delete a terminated task. If a terminated task
has already been automatically deleted and THEN the user's application
attempts to delete it (ie: using a stale Task handle), the results are
undefined and probably catastrophic!

                `,
                default: false
            }
        ]
    },
    templates: {
        bios_config_h: "/ti/sysbios/knl/Task_config.h.xdt",
        "/ti/utils/rov/syscfg_c.rov.xs.xdt": "/ti/sysbios/knl/Task.rov.js"
    },
    getCFiles: () => { return ["ti/sysbios/knl/Task.c"] }
};

