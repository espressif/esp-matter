/******************************************************************************

 @file  icall_platform.h

 @brief Platform specific function interfaces required for ICall implementation

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/
#ifndef ICALLPLATFORM_H
#define ICALLPLATFORM_H

#include <stdint.h>

#include "icall.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Functions in platform independent ICall module that can be used
 * by platform dependent functions.
 */
extern ICall_CSState ICall_enterCSImpl(void);
extern void ICall_leaveCSImpl(ICall_CSState key);
extern void *ICall_mallocImpl(uint_fast16_t size);
extern void ICall_freeImpl(void *ptr);

#ifdef ICALL_HOOK_ABORT_FUNC
extern void ICALL_HOOK_ABORT_FUNC();
#else /* ICALL_HOOK_ABORT_FUNC */
/* Note that customer can use their own assert handler */
#include <stdlib.h>
/**
 * Abort function definition.
 * Note that at compile time, this macro can be overridden
 * to point to another function of void fn(void) type.
 */
#if defined (__IAR_SYSTEMS_ICC__)
#pragma diag_suppress=Pe111 // Suppressed warning "statement is unreachable"
#endif // __IAR_SYSTEMS_ICC__
#define ICALL_HOOK_ABORT_FUNC() abort()
#endif /* ICALL_HOOK_ABORT */

/**
 * @internal
 * Updates power activity counter
 *
 * @param args  arguments
 * @return return value of ICall_pwrUpdateActivityCounter()
 */
extern ICall_Errno
ICallPlatform_pwrUpdActivityCounter(ICall_PwrUpdActivityCounterArgs *args);

/**
 * @internal
 * Registers power state transition notify function
 *
 * @param args  arguments
 * @return return values of ICall_pwrRegisterNotify()
 */
extern ICall_Errno
ICallPlatform_pwrRegisterNotify(ICall_PwrRegisterNotifyArgs *args);

/**
 * @internal
 * Configures power activity counter action
 *
 * @param args arguments
 * @return return value of ICall_pwrConfigACAction()
 */
extern ICall_Errno
ICallPlatform_pwrConfigACAction(ICall_PwrBitmapArgs *args);

/**
 * @internal
 * Sets power constraints and dependencies
 *
 * @param args arguments
 * @return return value of ICall_pwrRequire()
 */
extern ICall_Errno
ICallPlatform_pwrRequire(ICall_PwrBitmapArgs *args);

/**
 * @internal
 * Releases power constraints and dependencies
 *
 * @param args arguments
 * @return return value of ICall_pwrDispense()
 */
extern ICall_Errno
ICallPlatform_pwrDispense(ICall_PwrBitmapArgs *args);

/**
 * @internal
 * Checks whether HF XOSC is stable.
 *
 * @return ICALL_ERRNO_SUCCESS
 */
extern ICall_Errno
ICallPlatform_pwrIsStableXOSCHF(ICall_GetBoolArgs* args);

/**
 * @internal
 * Switches clock source to HF XOSC.
 *
 * @return ICALL_ERRNO_SUCCESS
 */
extern ICall_Errno
ICallPlatform_pwrSwitchXOSCHF(ICall_FuncArgsHdr* args);

/**
 * @internal
 * Get the estimated crystal oscillator startup time
 *
 * @return ICALL_ERRNO_SUCCESS
 */
extern ICall_Errno
ICallPlatform_pwrGetXOSCStartupTime(ICall_PwrGetXOSCStartupTimeArgs * args);

/**
 * @internal
 * Retrieves power transition state.
 *
 * @return @ref ICALL_ERRNO_SUCCESS
 */
extern ICall_Errno
ICallPlatform_pwrGetTransitionState(ICall_PwrGetTransitionStateArgs *args);

#ifdef __cplusplus
}
#endif

#endif /* ICALLPLATFORM_H */
