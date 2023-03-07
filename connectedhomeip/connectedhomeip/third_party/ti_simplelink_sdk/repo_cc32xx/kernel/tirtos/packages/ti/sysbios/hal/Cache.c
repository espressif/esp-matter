/*
 * Copyright (c) 2012, Texas Instruments Incorporated
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
 *  ======== Cache.c ========
 */

#include <xdc/std.h>
#include "package/internal/Cache.xdc.h"

/*
 *  ======== Cache_enable ========
 */
Void Cache_enable(Bits16 type)
{
    Cache_CacheProxy_enable(type);
}

/*
 *  ======== Cache_disable ========
 */
Void Cache_disable(Bits16 type)
{
    Cache_CacheProxy_disable(type);
}

/*
 *  ======== Cache_inv ========
 *  Invalidate the range of memory within the specified starting address and
 *  byte count.  The range of addresses operated on gets quantized to whole
 *  cache lines in each cache.  All lines in range are invalidated for all
 *  caches specified in the 'type' parameter.
 */
Void Cache_inv(Ptr blockPtr, SizeT byteCnt, Bits16 type, Bool wait)
{
    Cache_CacheProxy_inv(blockPtr, byteCnt, type, wait);
}

/*
 *  ======== Cache_wb ========
 *  Writes back the range of memory within the specified starting address
 *  and byte count.  The range of addresses operated on gets quantized to
 *  whole cache lines in each cache.  All lines within the range are left
 *  valid for all caches specified in the 'type' parameter and the data
 *  within the range will be written back to the source memory.
 */
Void Cache_wb(Ptr blockPtr, SizeT byteCnt, Bits16 type, Bool wait)
{
    Cache_CacheProxy_wb(blockPtr, byteCnt, type, wait);
}

/*
 *  ======== Cache_wbInv ========
 *  Writes back and invalidates the range of memory within the specified
 *  starting address and byte count.  The range of addresses operated on gets
 *  quantized to whole cache lines in each cache.  All lines within range are
 *  written back to the source memory and then invalidated for all caches
 *  specified in the 'type' parameter.
 */
Void Cache_wbInv(Ptr blockPtr, SizeT byteCnt, Bits16 type, Bool wait)
{
    Cache_CacheProxy_wbInv(blockPtr, byteCnt, type, wait);
}

/*
 *  ======== Cache_wbAll ========
 */
Void Cache_wbAll()
{
    Cache_CacheProxy_wbAll();
}

/*
 *  ======== Cache_wbInvAll ========
 */
Void Cache_wbInvAll()
{
    Cache_CacheProxy_wbInvAll();
}

/*
 *  ======== Cache_wait ========
 *  Wait for the cache wb/wbInv/inv operation to complete.  A cache
 *  operation is not truly complete until it has worked its way through
 *  all buffering and all memory writes have landed in the source memory.
 */
Void Cache_wait()
{
    Cache_CacheProxy_wait();
}
