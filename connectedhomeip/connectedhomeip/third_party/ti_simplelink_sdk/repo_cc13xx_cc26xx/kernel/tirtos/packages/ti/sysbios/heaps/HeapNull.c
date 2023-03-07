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
 *  ======== HeapNull.c ========
 */

#include <xdc/std.h>

#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Memory.h>

#include "package/internal/HeapNull.xdc.h"

/*
 *  ======== HeapNull_Instance_init ========
 *  Set-up the object's fields.
 */
Void HeapNull_Instance_init(HeapNull_Object *obj, const HeapNull_Params *params)
{
}

/*
 *  ======== HeapNull_alloc ========
 *  This is a simple growth-only heap. This function returns the current
 *  location of the top of unused buffer.
 */
Ptr HeapNull_alloc(HeapNull_Object *obj, SizeT size, SizeT align,
    Error_Block *eb)
{
    return (NULL);
}

/*
 *  ======== HeapNull_free ========
 *  This is a growth-only heap, so free does nothing.
 */
Void HeapNull_free(HeapNull_Object *obj, Ptr block, SizeT size)
{
}

/*
 *  ======== HeapNull_isBlocking ======== 
 */
Bool HeapNull_isBlocking(HeapNull_Object *obj)
{
    return (FALSE);
}

/*
 *  ======== HeapNull_getStats ======== 
 *  Return the stats.
 */
Void HeapNull_getStats(HeapNull_Object *obj, Memory_Stats *stats)
{
    stats->totalSize         = 0;
    stats->totalFreeSize     = 0;
    stats->largestFreeSize   = 0;
}


