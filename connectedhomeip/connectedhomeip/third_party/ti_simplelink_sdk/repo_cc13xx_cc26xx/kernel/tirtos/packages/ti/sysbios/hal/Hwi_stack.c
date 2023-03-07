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
 *  ======== Hwi_stack.c ========
 */

#include <xdc/std.h>
#include <xdc/runtime/Error.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Core.h>

#include "package/internal/Hwi.xdc.h"

#if defined(xdc_target__isaCompatible_430)    \
    || defined(xdc_target__isaCompatible_430X)

/*
 *  ======== Hwi_initStack ========
 *  Initialize the Common Interrupt Stack
 */
Void ti_sysbios_hal_Hwi_initStack(Void)
{
    Hwi_StackInfo stkInfo;
    UInt *curStack;

    /* Get stack base and size */
    Hwi_getStackInfo(&stkInfo, FALSE);

    curStack = (UInt *)(stkInfo.hwiStackBase);

    while (curStack < (UInt *)(&curStack)) {
        *((volatile UInt *)curStack) = 0xbebe;
        curStack++;
    }
}

#elif defined(xdc_target__isaCompatible_v8A)

/*
 *  ======== Hwi_initStack ========
 *  Initialize the Common Interrupt Stack
 */
Void ti_sysbios_hal_Hwi_initStack(Void)
{
    Hwi_StackInfo stkInfo;
    SizeT curStack;
    register SizeT stackTop __asm__ ("sp");

    /* Get stack base and size */
    if (BIOS_smpEnabled) {
        Hwi_getCoreStackInfo(&stkInfo, FALSE, Core_getId());
    }
    else {
        Hwi_getStackInfo(&stkInfo, FALSE);
    }

    curStack = (SizeT)(stkInfo.hwiStackBase);

    while (curStack < stackTop) {
        *((volatile UInt8 *)curStack) = 0xbe;
        curStack++;
    }
}

#else

/*
 *  ======== Hwi_initStack ========
 *  Initialize the Common Interrupt Stack
 */
Void ti_sysbios_hal_Hwi_initStack(Void)
{
    Hwi_StackInfo stkInfo;
    UArg curStack;

    /* Get stack base and size */
    if (BIOS_smpEnabled) {
        Hwi_getCoreStackInfo(&stkInfo, FALSE, Core_getId());
    }
    else {
        Hwi_getStackInfo(&stkInfo, FALSE);
    }

#ifdef xdc_target__isaCompatible_28
    curStack = (UArg)(stkInfo.hwiStackBase) + (SizeT)(stkInfo.hwiStackSize);

    while (--curStack > ((UArg)(&curStack) + 1)) {
        *((volatile UInt8 *)curStack) = 0xbebe;
    }
#else
    curStack = (UArg)(stkInfo.hwiStackBase);

    while (curStack < (UArg)(&curStack)) {
        *((volatile UInt8 *)curStack) = 0xbe;
        curStack++;
    }
#endif
}

#endif

#if defined(xdc_target__isaCompatible_430)    \
    || defined(xdc_target__isaCompatible_430X)

/*
 *  ======== Hwi_initStackMin ========
 *  Initialize only the first byte/word of the Common interrupt stack
 */
Void ti_sysbios_hal_Hwi_initStackMin(Void)
{
    Hwi_StackInfo stkInfo;
    UInt *curStack;

    /* Get stack base and size */
    Hwi_getStackInfo(&stkInfo, FALSE);

    curStack = (UInt *)(stkInfo.hwiStackBase);
    *((volatile UInt *)curStack) = 0xbebe;

}

#else

/*
 *  ======== Hwi_initStackMin ========
 *  Initialize only the first byte/word of the Common interrupt stack
 */
Void ti_sysbios_hal_Hwi_initStackMin(Void)
{
    Hwi_StackInfo stkInfo;
    UArg curStack;

    /* Get stack base and size */
    if (BIOS_smpEnabled) {
        Hwi_getCoreStackInfo(&stkInfo, FALSE, Core_getId());
    }
    else {
        Hwi_getStackInfo(&stkInfo, FALSE);
    }


#ifdef xdc_target__isaCompatible_28
    curStack = (UArg)(stkInfo.hwiStackBase) + (SizeT)(stkInfo.hwiStackSize) - 1;
    *((volatile UInt8 *)curStack) = 0xbebe;

#else
    curStack = (UArg)(stkInfo.hwiStackBase);
    *((volatile UInt8 *)curStack) = 0xbe;

#endif
}

#endif

/*
 *  ======== Hwi_checkStack ========
 *  Check the Common Interrupt Stack for overflow.
 */
Void ti_sysbios_hal_Hwi_checkStack(Void)
{
    Bool overflow;
    Hwi_StackInfo stkInfo;

    if (BIOS_smpEnabled) {
        overflow = Hwi_getCoreStackInfo(&stkInfo, FALSE, Core_getId());
    }
    else {
        overflow = Hwi_getStackInfo(&stkInfo, FALSE);
    }

    if (overflow) {
        Error_raise(NULL, Hwi_E_stackOverflow, 0, 0);
    }
}
