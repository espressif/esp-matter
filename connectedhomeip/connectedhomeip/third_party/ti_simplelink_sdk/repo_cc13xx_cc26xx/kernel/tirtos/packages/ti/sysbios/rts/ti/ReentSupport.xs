/*
 * Copyright (c) 2017-2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== ReentSupport.xs ========
 */

var BIOS = null;
var ReentSupport = null;
var Task = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    BIOS = xdc.useModule("ti.sysbios.BIOS");
    Task = xdc.useModule("ti.sysbios.knl.Task");

    xdc.useModule('xdc.runtime.Assert');

    if (!BIOS.taskEnabled) {
        this.$logError("BIOS task must be enabled to use this module.", this);
    }

    /*  When building with ROM image of SYS/BIOS, cannot use kernel hooks.
     *  Use ROM version of getReent() which takes ownership of unused task
     *  object field (curCoreId) for errno memory storage. Otherwise, use
     *  hook version of getReent() which uses hook pointer for errno memory.
     */
    if ("ti.sysbios.rom.ROM" in xdc.om) {
        this.$private.ROM = true;
    }
    else {
        this.$private.ROM = false;

        /* add run-time hooks for re-entrancy support */
        Task.addHookSet({
            createFxn:   null,
            deleteFxn:   null,
            exitFxn:     null,
            readyFxn:    null,
            registerFxn: this.taskRegHook,
            switchFxn:   null
        });
    }
}

/*
 *  ======== getCFiles ========
 *  Return array of C language files associated
 *  with targetName (ie Program.build.target.$name).
 */
function getCFiles(targetName)
{
    return (["ReentSupport.c", "aeabi_portable.c"]);
}

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    ReentSupport = this;

    /* provide getCFiles() for Build.getCFiles() */
    this.$private.getCFiles = getCFiles;
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(state, mod)
{
    state.taskHId = -1;
}
