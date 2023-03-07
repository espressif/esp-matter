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
 *  ======== CacheNull.c ========
 */

#include <xdc/std.h>
#include "package/internal/CacheNull.xdc.h"


/*
 *  ======== CacheNull_enable ========
 */
Void CacheNull_enable(Bits16 type)
{
}

/*
 *  ======== CacheNull_disable ========
 */
Void CacheNull_disable(Bits16 type)
{
}

/*
 *  ======== CacheNull_inv ========
 */
Void CacheNull_inv(Ptr blockPtr, size_t byteCnt, Bits16 type, Bool wait)
{
}

/*
 *  ======== CacheNull_wb ========
 */
Void CacheNull_wb(Ptr blockPtr, size_t byteCnt, Bits16 type, Bool wait)
{
}

/*
 *  ======== CacheNull_wbInv ========
 */
Void CacheNull_wbInv(Ptr blockPtr, size_t byteCnt, Bits16 type, Bool wait)
{
}

/*
 *  ======== CacheNull_wait ========
 */
Void CacheNull_wait()
{
}

/*
 *  ======== CacheNull_wbAll ========
 */
Void CacheNull_wbAll()
{
}

/*
 *  ======== CacheNull_wbInvAll ========
 */
Void CacheNull_wbInvAll()
{
}
