/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
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
 *  ======== MemAlloc.xdc ========
 */

package ti.sysbios.rts;

/*!
 *  ======== MemAlloc ========
 *  This module conditionally generates re-entrant versions of RTS memory
 *  allocation functions like malloc(), calloc(), realloc(), free(),
 *  memalign(), and aligned_alloc().
 *
 *  If BIOS.heapSize is set to a non-zero value, the generated APIs
 *  forward their arguments to the corresponding re-entrant 
 *  xdc.runtime.Memory module APIs.
 *
 *  When BIOS.heapSize is set to zero, malloc(), calloc(), realloc(),
 *  and memalign() will always return NULL, and free() will do nothing.
 *
 *  If MemAlloc.generateFunctions is set to false, these APIs are not
 *  generated. This will result in the RTS library's native versions of
 *  these APIs being used.
 *
 *  Additionally, if BIOS.heapSize is non-zero and xdc.runtime.HeapStd
 *  is used, generation of the APIs is suppressed in order to avoid
 *  infinite recursion (ie: malloc() calls Memory_alloc(), which calls 
 *  HeapStd_alloc(), which recursively calls malloc() ...).
 */

@Template ("./MemAlloc.xdt")

metaonly module MemAlloc
{
    /*!
     *  ======== generateFunctions ========
     *  By default, SYS/BIOS will generate malloc() and free() APIs that use Memory_alloc() and Memory_free().
     */
    metaonly config Bool generateFunctions = true;

    /*!
     *  @_nodoc
     *  ======== configureProgramHeap ========
     *  set Program.heap to zero if HeapStd is not used and we are generating malloc()
     *  This function is called by BIOS.module$use().
     */
    metaonly Void configureProgramHeap();
}
