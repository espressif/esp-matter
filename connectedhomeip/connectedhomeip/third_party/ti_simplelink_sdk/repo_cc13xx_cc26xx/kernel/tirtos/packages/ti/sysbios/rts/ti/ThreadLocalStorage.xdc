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
 *  ======== ThreadLocalStorage.xdc ========
 */

import xdc.runtime.Types;
import xdc.runtime.IHeap;
import xdc.runtime.Error;
import ti.sysbios.knl.Task;

/*!
 *  ======== ThreadLocalStorage ========
 *  Thread-local RTS data support module
 *
 *  The ThreadLocalStorage module allows SYS/BIOS Tasks to maintain
 *  RTS data that is private to each Task.  RTS data includes, among
 *  other things, the errno variable from the RTS library and the C++
 *  exception stack.
 *
 *  This support is achieved by way of per-Task "TLS" data areas and
 *  a Task switch hook function that tracks the current Task's TLS
 *  data area on behalf of the RTS library code.
 *
 *  Currently requires use of the --multithreaded option for the CG tools
 *  compiler.
 */

@DirectCall

module ThreadLocalStorage
{
    /*!
     *  ======== heapHandle ========
     *  Heap to use for TLS allocation. If null, uses the default Heap
     */
    config IHeap.Handle heapHandle = null;

    /*!
     *  ======== enableTLSSupport ========
     *  Enable TI TLS support
     */
    config Bool enableTLSSupport = false;

    /*!
     *  ======== TItlsSectMemory ========
     *  Memory section in which to place .TI.tls output data section
     */
    config String TItlsSectMemory = null;

    /*!
     *  ======== TItls_initSectMemory ========
     *  Memory section in which to place .TI.tls_init output data section
     */
    config String TItls_initSectMemory = null;

internal:
    /*!
     *  ======== createFxn ========
     */
    Void createFxn(Task.Handle tsk, Error.Block *eb);

    /*!
     *  ======== switchFxn ========
     */
    Void switchFxn(Task.Handle from, Task.Handle to);

    /*!
     *  ======== deleteFxn ========
     */
    Void deleteFxn(Task.Handle tsk);

    /*!
     *  ======== registerFxn ========
     */
    Void registerFxn(Int id);

    struct Module_State {
        Ptr          currentTP;
        UInt         contextId;
        IHeap.Handle heapHandle;
    };
}
