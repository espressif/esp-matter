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
 *  ======== package.xdc ========
 *
 */

requires ti.sysbios.interfaces;

/*!
 *  ======== ti.sysbios.heaps ========
 *  Contains modules that inherit from IHeap.
 *
 *  A "Heap" is a module that implements the IHeap interface. Heaps are 
 *  dynamic memory managers: they manage a specific piece of memory and 
 *  support allocating and freeing pieces ("blocks") of that memory.
 *
 *  The {@link xdc.runtime.Memory} module is the common interface for all 
 *  memory operations. Actual memory management is performed by a Heap 
 *  instance, such as an instance of HeapMem or HeapBuf. For example, 
 *  Memory_alloc() is used at runtime to dynamically allocate memory. 
 *  All of the Memory APIs take a Heap instance as one of their parameters. 
 *  Internally, the Memory module calls into the heap’s interface functions.
 *  
 *  The xdc.runtime.Memory module is documented in the XDCtools online help. 
 *  Implementations of Heaps provided by SYS/BIOS are discussed here.
 *  
 *  Memory allocation sizes are measured in "Minimum Addressable Units" 
 *  (MAUs) of memory. An MAU is the smallest unit of data storage that can 
 *  be read or written by the CPU. For the C5000, this is a 16-bit word. 
 *  For the C28x, this is an 16-bit word. For the all other currently 
 *  supported target families—including C6000 and ARM—this is an
 *  8-bit byte.
 *
 */
package ti.sysbios.heaps [2,0,0,0] {
    module HeapBuf;
    module HeapMem;
    module HeapMultiBuf;
    module HeapNull;
    module HeapTrack;
    module HeapCallback;
}
