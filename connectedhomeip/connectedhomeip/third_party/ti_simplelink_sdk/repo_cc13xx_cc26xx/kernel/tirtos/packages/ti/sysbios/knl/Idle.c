/*
 * Copyright (c) 2012-2017, Texas Instruments Incorporated
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
 *  ======== Idle.c ========
 *  Implementation of Idle_loop.
 */

#include <xdc/std.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/hal/Core.h>
#include <ti/sysbios/hal/Hwi.h>

#include "package/internal/Idle.xdc.h"

/*
 *  ======== Idle_loop ========
 */
/* MISRA.FUNC.UNUSEDPAR.2012 */
Void Idle_loop(UArg arg1, UArg arg2)
{
    /* INFINITE_LOOP.LOCAL */
    while (TRUE) {
        Idle_run();
    }
}

/*
 *  ======== Idle_run ========
 */
Void Idle_run(Void)
{
    Int i;

    /* CWARN.CONSTCOND.IF */
    if (BIOS_smpEnabled == TRUE) {
        /* UNREACH.GEN */
        UInt coreId = Core_getId();
        for (i = 0; i < Idle_funcList.length; i++) {
            if (Idle_coreList.elem[i] == coreId) {
                Idle_funcList.elem[i]();
            }
        }
    }
    else {
        for (i = 0; i < Idle_funcList.length; i++) {
            Idle_funcList.elem[i]();
        }
    }
}
