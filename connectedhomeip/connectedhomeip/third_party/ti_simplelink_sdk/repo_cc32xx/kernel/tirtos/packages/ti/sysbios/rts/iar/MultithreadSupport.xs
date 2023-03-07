/*
 * Copyright (c) 2013-2018, Texas Instruments Incorporated
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
 *  ======== MultithreadSupport.xs ========
 */

var Semaphore = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    var BIOS = xdc.module("ti.sysbios.BIOS");
    var Startup = xdc.useModule('xdc.runtime.Startup');
    Semaphore = xdc.useModule('ti.sysbios.knl.Semaphore');

    /*  When building with ROM image of SYS/BIOS, cannot use kernel
     *  hooks. Use ROM version of getTlsAddr() which takes ownership
     *  of unused task object field (curCoreId) to store address of
     *  the TLS block. Otherwise, use hook version of getTlsAddr()
     *  which uses hook pointer to store the TLS address.
     */
    this.$private.ROM = ("ti.sysbios.rom.ROM" in xdc.om ? true : false);

    if (BIOS.taskEnabled && this.enableMultithreadSupport) {
        if (Program.build.target.isa.match(/v7M/)) {
            Startup.lastFxns.$add("&__iar_Initlocks");
        }
        if (!this.$private.ROM) {
            var Task = xdc.module("ti.sysbios.knl.Task");
            Task.addHookSet({
                createFxn:   null,
                deleteFxn:   this.taskDeleteHook,
                exitFxn:     null,
                readyFxn:    null,
                registerFxn: this.taskRegHook,
                switchFxn:   null
            });
        }
    }
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(state, mod)
{
    state.taskHId = -1;
    state.deletedTaskTLSPtr = null;
    state.curTaskHandle = null;

    /*
     * Create a binary semaphore. Used to protect
     * MultithreadSupport_module->taskDeleteHandle
     */
    var semParams = new Semaphore.Params();
    semParams.mode = Semaphore.Mode_BINARY;
    state.lock = Semaphore.create(1, semParams);
}

/*
 *  ======== module$validate ========
 */
function module$validate()
{
    if (!Program.build.target.$name.match(/iar/)) {
        this.$logError("This module only supports IAR target: "
            + Program.build.target.$name + " is not supported.", this);
    }
}
