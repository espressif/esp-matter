/*
 * Copyright (c) 2015-2018 Texas Instruments Incorporated - http://www.ti.com
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
 */

/*
 *  ======== Settings.xs ========
 */

var Idle = null;
var Task = null;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    var toolchain = "";
    var Build = xdc.module('ti.sysbios.Build');

    /* load modules which are needed by TI-POSIX */
    xdc.useModule('xdc.runtime.Assert');
    xdc.useModule('xdc.runtime.Memory');

    xdc.useModule('ti.sysbios.BIOS');
    xdc.useModule('ti.sysbios.hal.Seconds');
    xdc.useModule('ti.sysbios.knl.Clock');
    xdc.useModule('ti.sysbios.knl.Mailbox');
    xdc.useModule('ti.sysbios.knl.Queue');
    xdc.useModule('ti.sysbios.knl.Semaphore');
    xdc.useModule('ti.sysbios.gates.GateMutex');
    Task = xdc.useModule('ti.sysbios.knl.Task');

    /*
     *  BIOS posix uses the Idle task to clean up detached pthreads that
     *  have exited.  Check that the Idle task is enabled.
     */
    if (Task.enableIdleTask == false) {
        throw new xdc.global.Error("Task.enableIdleTask must be set to true");
    }

    Idle = xdc.useModule("ti.sysbios.knl.Idle");

    /* Add Idle function that will clean up terminated pthreads */
    Idle.addFunc("&_pthread_cleanupFxn");

    /* identify which toolchain is loaded */
    var match = null;
    if (Program.build.target.$name.match(/^ti\.targets\.arm\.clang/)) {
        /* wonky, but matches the syntax of .match() */
        match = ["ignore", "ticlang"];
    } else {
        /* extract the first word of the target package name */
        match = Program.build.target.$name.match(/^(\w+)\.targets/);
    }
    switch (match != null ? match[1] : "") {
        case "gnu":     toolchain = "gcc";     break;
        case "iar":     toolchain = "iar";     break;
        case "ti":      toolchain = "ccs";     break;
        case "ticlang": toolchain = "ticlang"; break;
        default:
            throw new xdc.global.Error("unknown target:"
                + Program.build.target.$name);
    }

    /* add compiler defines to the kernel build */
    Build.ccArgs.$add("-Dti_posix_tirtos_Settings_debug__D="
            + (this.debug ? "TRUE" : "FALSE"));

    if (this.enableMutexPriority) {
        Build.ccArgs.$add("-Dti_posix_tirtos_Settings_enableMutexPriority__D");
    }

    if (toolchain == "gcc") {
        Build.ccArgs.$add("-std=c99");
    }

    /* add source files to the kernel build */
    Build.annex.$add({
        repo: this.$package.packageRepository,
        incs: [
            this.$package.packageRepository + "ti/posix/" + toolchain
        ],
        files: [
            "ti/posix/tirtos/clock.c",
            "ti/posix/tirtos/mqueue.c",
            "ti/posix/tirtos/pthread.c",
            "ti/posix/tirtos/pthread_barrier.c",
            "ti/posix/tirtos/pthread_cond.c",
            "ti/posix/tirtos/pthread_key.c",
            "ti/posix/tirtos/pthread_mutex.c",
            "ti/posix/tirtos/pthread_rwlock.c",
            "ti/posix/tirtos/pthread_util.c",
            "ti/posix/tirtos/sched.c",
            "ti/posix/tirtos/semaphore.c",
            "ti/posix/tirtos/sleep.c",
            "ti/posix/tirtos/timer.c"
        ]
    });

    /* load appropriate reentrant support module */
    switch (toolchain) {

        case "ccs":
        case "ticlang":
            xdc.useModule('ti.sysbios.rts.ti.ReentSupport');
            break;

        case "iar":
            xdc.useModule('ti.sysbios.rts.iar.MultithreadSupport');
            break;

        case "gcc":
            /* none needed */
            break;

        default:
            throw new xdc.global.Error("no reentrant support module available");
    }
}
