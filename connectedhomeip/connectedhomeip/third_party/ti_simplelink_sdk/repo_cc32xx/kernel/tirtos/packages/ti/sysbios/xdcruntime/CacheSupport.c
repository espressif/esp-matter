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
 *  ======== CacheSupport.c =======
 */

#include <xdc/std.h>
#include <xdc/runtime/Error.h>

#include <ti/sysbios/hal/Cache.h>

#include "package/internal/CacheSupport.xdc.h"

/*
 *  ======== CacheSupport_inv ========
 */
Bool CacheSupport_inv(Ptr blockPtr, SizeT byteCnt, Bool wait, Error_Block *eb)
{
    Cache_inv(blockPtr, byteCnt, Cache_Type_ALL, wait);

    return (TRUE);
}

/*
 *  ======== CacheSupport_wb ========
 */
Bool CacheSupport_wb(Ptr blockPtr, SizeT byteCnt, Bool wait, Error_Block *eb)

{
    Cache_wb(blockPtr, byteCnt, Cache_Type_ALL, wait);

    return (TRUE);
}

/*
 *  ======== CacheSupport_wbInv ========
 */
Bool CacheSupport_wbInv(Ptr blockPtr, SizeT byteCnt, Bool wait, Error_Block *eb)
{
    Cache_wbInv(blockPtr, byteCnt, Cache_Type_ALL, wait);

    return (TRUE);
}

/*
 *  ======== CacheSupport_wait ========
 */
Bool CacheSupport_wait(Error_Block *eb)
{
    Cache_wait();

    return (TRUE);
}

