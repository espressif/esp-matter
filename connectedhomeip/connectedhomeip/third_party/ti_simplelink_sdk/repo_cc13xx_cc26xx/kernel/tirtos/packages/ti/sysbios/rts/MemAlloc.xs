/*
 * Copyright (c) 2015-2017 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== MemAlloc.xs ========
 */

var BIOS = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    BIOS = xdc.module("ti.sysbios.BIOS");
    var MemAlloc = xdc.module('ti.sysbios.rts.MemAlloc');
    var HeapStd = xdc.module('xdc.runtime.HeapStd');

    if (Program.build.target.$name.match(/gnu/) &&
       (BIOS.taskEnabled == true) &&
       (BIOS.heapSize != 0) &&
       !Program.build.target.$name.match(/A53F/)) {
        xdc.useModule('ti.sysbios.rts.gnu.ReentSupport');
    }
    else if (Program.build.target.$name.match(/ti/) &&
             (Program.build.target.isa.match(/66/) ||
              Program.build.target.isa.match(/674/)) &&
             (BIOS.taskEnabled == true) &&
             (BIOS.heapSize != 0)) {
        xdc.useModule('ti.sysbios.rts.ti.ThreadLocalStorage');
    }
}

/*
 *  ======== configureProgramHeap ========
 */
function configureProgramHeap()
{
    var MemAlloc = xdc.module('ti.sysbios.rts.MemAlloc');
    var HeapStd = xdc.module('xdc.runtime.HeapStd');

    var nogenFunctions = (MemAlloc.generateFunctions == false) ||
                     ((BIOS.heapSize != 0) &&
                      (HeapStd.$used));

    if (nogenFunctions == false) {
        /*
         * If we are generating malloc() and HeapStd is not in use,
         * then there is no need for Program.heap.
         */
        if (HeapStd.$used == false) {
            Program.heap = 0;
        }
    }
}
