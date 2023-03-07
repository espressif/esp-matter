/*
 * Copyright (c) 2016, Texas Instruments Incorporated
 * http://www.ti.com
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
 * --/COPYRIGHT--*/
/*
 *  ======== ThreadLocalStorage.xs ========
 */

var BIOS = null;
var Task = null;
var ThreadLocalStorage = null;

/*
 * ======== getAsmFiles ========
 * return the array of assembly language files associated
 * with targetName (ie Program.build.target.$name)
 */
function getAsmFiles(targetName)
{
    switch (targetName) {
      case "ti.targets.elf.C66":
      case "ti.targets.elf.C674":
        return (["tls_get_tp.asm"]);
        break;

      default:
        return (null);
        break;
    }
}

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    /* provide getAsmFiles() for Build.getAsmFiles() */
    this.$private.getAsmFiles = getAsmFiles;
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    ThreadLocalStorage = this;

    if (!Program.build.target.isa.match(/66/) &&
        !Program.build.target.isa.match(/674/)) {
        ThreadLocalStorage.$logFatal("ThreadLocalStorage module is supported" +
                                     " only on C66 & C674 targets",
                                     ThreadLocalStorage);
    }

    BIOS = xdc.useModule("ti.sysbios.BIOS");
    if (BIOS.taskEnabled == true) {
        Task = xdc.useModule("ti.sysbios.knl.Task");
    }
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    mod.currentTP     = '&__TI_TLS_MAIN_THREAD_Base';
    mod.contextId     = -1;
    mod.heapHandle = ThreadLocalStorage.heapHandle;

    if ((BIOS.taskEnabled == true) &&
        (ThreadLocalStorage.enableTLSSupport == true)) {

        Task.addHookSet({
            createFxn:   ThreadLocalStorage.createFxn,
            registerFxn: ThreadLocalStorage.registerFxn,
            switchFxn:   ThreadLocalStorage.switchFxn,
            deleteFxn:   ThreadLocalStorage.deleteFxn,
        });

        var TItlsSegment = ThreadLocalStorage.TItlsSectMemory;
        var TItls_initSegment = ThreadLocalStorage.TItls_initSectMemory;

        if (TItlsSegment == null) {
            TItlsSegment = Program.platform.stackMemory;
        }
        if (TItls_initSegment == null) {
            TItls_initSegment = Program.platform.stackMemory;
        }

        /* .TI.tls is a grouped section containing TLS' .tdata and .tbss */
        var sect = '.TI.tls';
        Program.sectMap[sect] = new Program.SectionSpec();
        Program.sectMap[sect].loadSegment = TItlsSegment;

        var sect = '.TI.tls_init';
        Program.sectMap[sect] = new Program.SectionSpec();
        Program.sectMap[sect].loadSegment = TItls_initSegment;
    }
}
