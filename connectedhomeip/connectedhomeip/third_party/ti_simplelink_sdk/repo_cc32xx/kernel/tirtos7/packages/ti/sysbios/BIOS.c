/*
 * Copyright (c) 2013-2020, Texas Instruments Incorporated
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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__ti__)
#include <_lock.h>
#endif

#include <ti/sysbios/runtime/System.h>
#include <ti/sysbios/runtime/Startup.h>
#include <ti/sysbios/runtime/Types.h>
#include <ti/sysbios/runtime/Assert.h>
#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/Types.h>

//#include <ti/sysbios/gates/GateHwi.h>
//#include <ti/sysbios/gates/GateSwi.h>
#include <ti/sysbios/gates/GateMutex.h>
//#include <ti/sysbios/gates/GateMutexPri.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Idle.h>

/* runtimeCreatesEnabled */
const bool BIOS_runtimeCreatesEnabled = BIOS_runtimeCreatesEnabled_D;

/* taskEnabled */
const bool BIOS_taskEnabled = BIOS_taskEnabled_D;

/* swiEnabled */
const bool BIOS_swiEnabled = BIOS_swiEnabled_D;

/* clockEnabled */
const bool BIOS_clockEnabled = BIOS_clockEnabled_D;

/* heapSize */
const size_t BIOS_heapSize = BIOS_heapSize_D;

BIOS_Module_State BIOS_Module_state = {
    .cpuFreq.lo = BIOS_cpuFrequency_D,
    .threadType = BIOS_ThreadType_Main,
    .startFunc = BIOS_startFunc,
    .exitFunc = BIOS_exitFunc,
    .rtsGateType = BIOS_rtsLockType_D,
    .enables = (BIOS_taskEnabled_D << 0) |
               (BIOS_swiEnabled_D << 1) |
               (BIOS_clockEnabled_D << 2) |
               (BIOS_runtimeCreatesEnabled_D << 5),
    .initDone = 0
};

const Types_FreqHz BIOS_cpuFreq = {
    .hi = 0,
    .lo = BIOS_cpuFrequency_D
};

BIOS_RTS_GATE_STRUCT BIOS_rtsGate;

/*
 *  ======== BIOS_linkedWithIncorrectBootLibrary ========
 *  Need to exclude the function declaration from LCOV due to following bug:
 *  https://github.com/bp-lang/oplex/commit/585571667c01b182c0fe7b7c90b75be7fd9e91a6
 */
/* LCOV_EXCL_START */
__attribute__((noinline)) __attribute((noreturn))
void BIOS_linkedWithIncorrectBootLibrary(void)
{
/* LCOV_EXCL_STOP */
    /*
     *  If execution reaches this function, it indicates that the wrong
     *  boot library was linked with and the XDC runtime startup functions
     *  were not called. This can happen if the code gen tool's RTS library
     *  was before SYS/BIOS's generated linker cmd file on the link line.
     */
    for (;;) {
        Startup_done;  /* dummy statement to work around CODEGEN-6288 */
    }
}

/*
 *  ======== BIOS_getThreadType ========
 */
BIOS_ThreadType BIOS_getThreadType(void)
{
    return (BIOS_module->threadType);
}

/*
 *  ======== BIOS_setThreadType ========
 */
BIOS_ThreadType BIOS_setThreadType(BIOS_ThreadType ttype)
{
    BIOS_ThreadType prevThreadType;

    prevThreadType = BIOS_module->threadType;
    BIOS_module->threadType = ttype;

    return (prevThreadType);
}

/*
 *  ======== BIOS_setCpuFreq ========
 */
void BIOS_setCpuFreq(Types_FreqHz *freq)
{
    BIOS_module->cpuFreq = *freq;
}

/*
 *  ======== BIOS_getCpuFreq ========
 */
void BIOS_getCpuFreq(Types_FreqHz *freq)
{
   *freq = BIOS_module->cpuFreq;
}

/*
 *  ======== BIOS_start ========
 *  Exclude the exit of this function from LCOV because it will never be reached.
 */
void BIOS_start(void)
{
    BIOS_module->startFunc();
/* LCOV_EXCL_START */
}
/* LCOV_EXCL_STOP */

/*
 *  ======== BIOS_exit ========
 *  Exclude the exit of this function from LCOV because it will never be reached.
 */
void BIOS_exit(int stat)
{
    BIOS_module->exitFunc(stat);
/* LCOV_EXCL_START */
}
/* LCOV_EXCL_STOP */

extern void Hwi_startup(void);

void BIOS_atExitFunc(int notused)
{
#if BIOS_swiEnabled_D
    (void)Swi_disable();
#endif
#if BIOS_taskEnabled_D
    (void)Task_disable();
#endif
    (void)BIOS_setThreadType(BIOS_ThreadType_Main);

#if defined(__ti__)
    if (BIOS_module->rtsGate != NULL) {
        _register_lock(_nop);
        _register_unlock(_nop);
    }
#endif
}

#if defined(__ti__)

/*
 *  ======== BIOS_nullFunc ========
 */
void BIOS_nullFunc(void)
{
}

/*
 *  ======== BIOS_rtsLock ========
 *  Called by rts _lock() function
 */
void BIOS_rtsLock(void)
{
    intptr_t key;

    key = BIOS_RTS_GATE_ENTER(BIOS_RTS_GATE_HANDLE(BIOS_module->rtsGate));
    if (BIOS_module->rtsGateCount == 0U) {
        BIOS_module->rtsGateKey = key;
    }
    /* Increment need not be atomic */
    BIOS_module->rtsGateCount++;
}

/*
 *  ======== BIOS_rtsUnLock ========
 *  Called by rts _unlock() function
 */
void BIOS_rtsUnlock(void)
{
    Assert_isTrue((BIOS_module->rtsGateCount), NULL);

    BIOS_module->rtsGateCount--;

    if (BIOS_module->rtsGateCount == 0U) {
        BIOS_RTS_GATE_LEAVE(BIOS_RTS_GATE_HANDLE(BIOS_module->rtsGate), BIOS_module->rtsGateKey);
    }
}

/*
 *  ======== BIOS_registerRTSLock ========
 */
void BIOS_registerRTSLock(void)
{
    if (BIOS_module->rtsGate != NULL) {
        _register_lock(BIOS_rtsLock);
        _register_unlock(BIOS_rtsUnlock);
    }
}

/*
 *  ======== BIOS_removeRTSLock ========
 */
void BIOS_removeRTSLock(void)
{
    if (BIOS_module->rtsGate != NULL) {
        _register_lock(BIOS_nullFunc);
        _register_unlock(BIOS_nullFunc);
    }
}

#endif  /* if defined(__ti__) */

/*
 *  ======== BIOS_exitFunc ========
 */
void BIOS_exitFunc(int stat)
{
#if defined(__ti__) && !defined(BIOS_RTS_GATE_NONE)
    /* remove the RTS lock */
    BIOS_removeRTSLock();
#endif

    /* force thread type to 'Main' */
    (void)BIOS_setThreadType(BIOS_ThreadType_Main);

    System_exit(stat);
}

/*
 *  ======== BIOS_startFunc ========
 */
void BIOS_startFunc(void)
{
    /*
     *  Check if XDC runtime startup functions have been called. If not, then
     *  call BIOS_linkedWithIncorrectBootLibrary() function. This function
     *  will spin forever.
     */
    if (Startup_done != true) {
        BIOS_linkedWithIncorrectBootLibrary();
    }

    (void)System_atexit(
        (System_AtexitHandler)BIOS_atExitFunc);
#if defined(__ti__) && !defined(BIOS_RTS_GATE_NONE)
    BIOS_RTS_GATE_CONSTRUCT(&BIOS_rtsGate, NULL);
    BIOS_module->rtsGate = &BIOS_rtsGate;
    BIOS_registerRTSLock();
#endif
#if BIOS_numStartUserFuncs_D
    BIOS_startUserFuncs();
#endif
    Hwi_startup();
#if BIOS_swiEnabled_D
    Swi_startup();
#endif
#if BIOS_taskEnabled_D
    Task_startup();
#else
    Idle_loop(0,0);
#endif
}
