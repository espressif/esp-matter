/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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
 *  ======== DebugP_tirtos.c ========
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>

/*
 *  ======== _DebugP_assert ========
 */
void _DebugP_assert(int expression, const char *file, int line)
{
#ifndef ti_sysbios_runtime_Assert__include
    if (!expression) {
        xdc_runtime_Assert_raise__I(Module__MID, file, line, 0U);
    }
#endif
}
/*
 *  ======== DebugP_log0 ========
 */
void DebugP_log0(const char *format)
{
    Log_print0(Diags_USER1, format);
}

/*
 *  ======== DebugP_log1 ========
 */
void DebugP_log1(const char *format, uintptr_t p1)
{
    Log_print1(Diags_USER1, format, (xdc_IArg)p1);
}

/*
 *  ======== DebugP_log2 ========
 */
void DebugP_log2(const char *format, uintptr_t p1, uintptr_t p2)
{
    Log_print2(Diags_USER1, format, (xdc_IArg)p1, (xdc_IArg)p2);
}
/*
 *  ======== DebugP_log3 ========
 */
void DebugP_log3(const char *format, uintptr_t p1, uintptr_t p2, uintptr_t p3)
{
    Log_print3(Diags_USER1, format, (xdc_IArg)p1, (xdc_IArg)p2, (xdc_IArg)p3);
}
/*
 *  ======== DebugP_log4 ========
 */
void DebugP_log4(const char *format, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4)
{
    Log_print4(Diags_USER1, format, (xdc_IArg)p1,
              (xdc_IArg)p2, (xdc_IArg)p3, (xdc_IArg)p4);
}
