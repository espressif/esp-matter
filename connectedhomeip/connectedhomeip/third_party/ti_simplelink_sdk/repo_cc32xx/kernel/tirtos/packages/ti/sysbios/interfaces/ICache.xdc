/*
 * Copyright (c) 2013, Texas Instruments Incorporated
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
 *  ======== ICache.xdc ========
 *
 *
 */

/*!
 *  ======== ICache ========
 *  Cache Interface
 */

@DirectCall
interface ICache
{
    /*! Lists of bitmask cache types */
    enum Type {
        Type_L1P = 0x1,         /*! Level 1 Program cache */
        Type_L1D = 0x2,         /*! Level 1 Data cache */
        Type_L1  = 0x3,         /*! Level 1 caches */
        Type_L2P = 0x4,         /*! Level 2 Program cache */
        Type_L2D = 0x8,         /*! Level 2 Data cache */
        Type_L2  = 0xC,         /*! Level 2 caches */
        Type_ALLP = 0x5,        /*! All Program caches */
        Type_ALLD = 0xA,        /*! All Data caches */
        Type_ALL = 0x7fff       /*! All caches */
    };

    /*!
     *  ======== enable ========
     *  Enables all cache(s)
     *
     *  @param(type)    bit mask of Cache type
     */
    Void enable(Bits16 type);

    /*!
     *  ======== disable ========
     *  Disables the 'type' cache(s)
     *
     *  @param(type)    bit mask of Cache type
     */
    Void disable(Bits16 type);

    /*!
     *  ======== inv ========
     *  Invalidate the range of memory within the specified starting
     *  address and byte count.  The range of addresses operated on
     *  gets quantized to whole cache lines in each cache.  All lines
     *  in range are invalidated for all the 'type' caches.
     *
     *  @param(blockPtr) start address of range to be invalidated
     *  @param(byteCnt)  number of bytes to be invalidated
     *  @param(type)     bit mask of Cache type
     *  @param(wait)     wait until the operation is completed
     */
    Void inv(Ptr blockPtr, SizeT byteCnt, Bits16 type, Bool wait);

    /*!
     *  ======== wb ========
     *  Writes back a range of memory from all cache(s)
     *
     *  Writes back the range of memory within the specified starting
     *  address and byte count.  The range of addresses operated on
     *  gets quantized to whole cache lines in each cache.  All lines
     *  within the range are left valid in the 'type' caches and the data
     *  within the range will be written back to the source memory.
     *
     *  @param(blockPtr) start address of range to be invalidated
     *  @param(byteCnt)  number of bytes to be invalidated
     *  @param(type)     bit mask of Cache type
     *  @param(wait)     wait until the operation is completed
     */
    Void wb(Ptr blockPtr, SizeT byteCnt, Bits16 type, Bool wait);

    /*!
     *  ======== wbInv ========
     *  Writes back and invalidates the range of memory within the
     *  specified starting address and byte count.  The range of
     *  addresses operated on gets quantized to whole cache lines in
     *  each cache.  All lines within the range are written back to the
     *  source memory and then invalidated for all 'type' caches.
     *
     *  @param(blockPtr) start address of range to be invalidated
     *  @param(byteCnt)  number of bytes to be invalidated
     *  @param(type)     bit mask of Cache type
     *  @param(wait)     wait until the operation is completed
     */
    Void wbInv(Ptr blockPtr, SizeT byteCnt, Bits16 type, Bool wait);

    /*!
     *  ======== wbAll ========
     *  Write back all caches
     *
     *  Perform a global write back.  There is no effect on program cache.
     *  All data cache lines are left valid.
     */
    Void wbAll();

    /*!
     *  ======== wbInvAll ========
     *  Write back invalidate all caches
     *
     *  Performs a global write back and invalidate.  All cache lines
     *  are written out to physical memory and then invalidated.  
     */
    Void wbInvAll();

    /*!
     *  ======== wait ========
     *  Wait for a previous cache operation to complete
     *
     *  Wait for the cache wb/wbInv/inv operation to complete.  A cache
     *  operation is not truly complete until it has worked its way
     *  through all buffering and all memory writes have landed in the
     *  source memory.
     */
    Void wait();

}

