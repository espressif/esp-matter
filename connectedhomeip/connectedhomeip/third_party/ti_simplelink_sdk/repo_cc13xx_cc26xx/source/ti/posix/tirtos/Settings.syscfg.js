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
 * ======== Settings.syscfg.js ========
 * This module's main purpose is to provide a list of source files to
 * include in the BIOS 7.x's ti_sysbios_config.c file. The BIOS module
 * queries all modules in the system for a getCFiles function which provides
 * an array of files to include in the ti_sysbios_config.c file. This could
 * be generalized and move to ti.utils.build.GetCFiles or some such module.
 */

var posixFiles = [
    "ti/posix/tirtos/clock.c",
    "ti/posix/tirtos/mqueue.c",
    "ti/posix/tirtos/pthread_barrier.c",
    "ti/posix/tirtos/pthread.c",
    "ti/posix/tirtos/pthread_cond.c",
    "ti/posix/tirtos/pthread_key.c",
    "ti/posix/tirtos/pthread_mutex.c",
    "ti/posix/tirtos/pthread_rwlock.c",
    "ti/posix/tirtos/pthread_util.c",
    "ti/posix/tirtos/sched.c",
    "ti/posix/tirtos/semaphore.c",
    "ti/posix/tirtos/sleep.c",
    "ti/posix/tirtos/timer.c"
];

function validate(mod, validation)
{
    /* confirm that TI-RTOS is selected */
    if (system.modules["/ti/utils/RTOS"]) {
        let RTOS = system.modules["/ti/utils/RTOS"];
        if (RTOS.$static.name != "TI-RTOS") {
            validation.logError("POSIX Settings should not be used when " +
                                "ti.utils.RTOS.name is not TI-RTOS.", mod);
        }
    }

    if (system.modules["/ti/drivers/RTOS"]) {
        let RTOS = system.modules["/ti/drivers/RTOS"];
        if (RTOS.$static.name != "TI-RTOS") {
            validation.logError("POSIX Settings should not be used when " +
                                "ti.drivers.RTOS.name is not TI-RTOS.", mod);
        }
    }
}

var config = [
    {
        name: "enableMutexPriority",
        description: "Enable support for pthread mutex priority protocol",
        longDescription: `
Set to true to enable pthread mutex priority protocol.
If set to true, PTHREAD_PRIO_PROTECT and PTHREAD_PRIO_INHERIT
mutexes will be available.

The default value is set to false to reduce code size.
`,
        displayName: "Enable Mutex Priority",
        default: false
    }
];

/*
 * ======== moduleInstances ========
 */
function moduleInstances(mod)
{
    let modInstances = new Array();

    modInstances.push(
        {
            name: "pthreadCleanupFxn",
            displayName: "Idle Function to Delete Terminated Threads",
            moduleName: "ti/sysbios/knl/Idle",
            hidden: true,
            readOnly: true,
            requiredArgs: {
                idleFxn: "_pthread_cleanupFxn",
                externRequired: true,
                $name: "_pthread_cleanupFxn"
            }
        }
    );

    return (modInstances);
}

function modules(inst)
{
    let modArray = [];

    modArray.push({
        name      : "Mailbox",
        moduleName: "ti/sysbios/knl/Mailbox"
    });

    modArray.push({
        name      : "Task",
        moduleName: "ti/sysbios/knl/Task"
    });

    modArray.push({
        name      : "Semaphore",
        moduleName: "ti/sysbios/knl/Semaphore"
    });

    modArray.push({
        name      : "Idle",
        moduleName: "ti/sysbios/knl/Idle"
    });

    return modArray
}

exports = {
    staticOnly: true,
    displayName: "POSIX Settings",
    moduleStatic: {
        validate: validate,
        modules: modules,
        moduleInstances: moduleInstances,
        config: config
    },
    templates: {
        bios_config_h: "/ti/posix/tirtos/Settings_config.h.xdt",
    },
    getCFiles: () => { return posixFiles }
};
