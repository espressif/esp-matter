/*
 * Copyright (c) 2013-2019, Texas Instruments Incorporated
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
 *  ======== BIOS.c ========
 */

#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>

#include <ti/sysbios/hal/Core.h>

#include "package/internal/BIOS.xdc.h"

/*!
 *  ======== BIOS_linkedWithIncorrectBootLibrary ========
 *  Need to exclude the function declaration from LCOV due to following bug:
 *  https://github.com/bp-lang/oplex/commit/585571667c01b182c0fe7b7c90b75be7fd9e91a6
 */
/* LCOV_EXCL_START */
Void BIOS_linkedWithIncorrectBootLibrary(Void)
{
/* LCOV_EXCL_STOP */
    /*
     *  If execution reaches this function, it indicates that the wrong
     *  boot library was linked with and the XDC runtime startup functions
     *  were not called. This can happen if the code gen tool's RTS library
     *  was before SYS/BIOS's generated linker cmd file on the link line.
     */
    while (1);
}

/*!
 *  ======== BIOS_getThreadType ========
 */
BIOS_ThreadType BIOS_getThreadType(Void)
{
    UInt key;
    BIOS_ThreadType threadType;

    if (BIOS_smpEnabled == TRUE) {
        /*
         * Disable interrupts locally on this core before reading
         * the thread type as the thread may move to another core
         * between the time the Core id is read and the smpThreadType
         * array is indexed.
         */
        key = Core_hwiDisable();
        threadType = BIOS_module->smpThreadType[Core_getId()];
        Core_hwiRestore(key);
        return (threadType);
    }
    else {
        return (BIOS_module->threadType);
    }
}

/*!
 *  ======== BIOS_setThreadType ========
 */
BIOS_ThreadType BIOS_setThreadType(BIOS_ThreadType ttype)
{
    BIOS_ThreadType prevThreadType;

    if (BIOS_smpEnabled == TRUE) {
        UInt coreId = Core_getId();
        prevThreadType = BIOS_module->smpThreadType[coreId];
        BIOS_module->smpThreadType[coreId] = ttype;
    }
    else {
        prevThreadType = BIOS_module->threadType;
        BIOS_module->threadType = ttype;
    }

    return (prevThreadType);
}

/*
 *  ======== BIOS_setCpuFreq ========
 */
Void BIOS_setCpuFreq(Types_FreqHz *freq)
{
    BIOS_module->cpuFreq = *freq;
}

/*
 *  ======== BIOS_getCpuFreq ========
 */
Void BIOS_getCpuFreq(Types_FreqHz *freq)
{
   *freq = BIOS_module->cpuFreq;
}

/*
 *  ======== BIOS_start ========
 *  Exclude the exit of this function from LCOV because it will never be reached.
 */
Void BIOS_start(void)
{
    BIOS_module->startFunc();
/* LCOV_EXCL_START */
}
/* LCOV_EXCL_STOP */

/*
 *  ======== BIOS_exit ========
 *  Exclude the exit of this function from LCOV because it will never be reached.
 */
Void BIOS_exit(Int stat)
{
    BIOS_module->exitFunc(stat);
/* LCOV_EXCL_START */
}
/* LCOV_EXCL_STOP */
