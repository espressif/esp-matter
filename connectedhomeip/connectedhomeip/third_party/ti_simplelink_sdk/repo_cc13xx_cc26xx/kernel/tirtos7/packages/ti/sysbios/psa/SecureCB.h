/*
 * Copyright (c) 2019 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== SecureCB.h ========
 */

#ifndef ti_spe_SecureCB__include
#define ti_spe_SecureCB__include

#include <stddef.h>
#include <stdint.h>

#if defined(__GNUC__)
#ifndef __STATIC_INLINE
#define __STATIC_INLINE static inline
#endif
#elif defined(__clang__) && defined(__ti__)
#define __STATIC_INLINE static __inline
#endif

#if defined (__cplusplus)
extern "C" {
#endif

#define NVIC_ISPR_ADDRESS               0xe000e200

/*  There is no dedicated interrupt for the SecureCB module.
 *  Using arbitrary interrupt which is not in use (hopefully).
 */
#define SecureCB_INT_NUM                43

#define SecureCB_STATUS_OK              0
#define SecureCB_STATUS_ERROR           1

#define SecureCB_CRSTAT_DEFINED         1
#define SecureCB_CRSTAT_ENABLED         2

typedef uintptr_t SecureCB_Arg;
typedef void (*SecureCB_FuncPtr)(SecureCB_Arg arg);

typedef struct SecureCB_Object {
    struct SecureCB_Object *volatile next;
    struct SecureCB_Object *volatile prev;
    uint16_t intNum;
    uint16_t crstat;
    SecureCB_FuncPtr fxn;
    SecureCB_Arg arg;
} SecureCB_Object;

typedef SecureCB_Object *SecureCB_Handle;

/*
 *  ======== SecureCB_init ========
 *  Initialize SecureCB driver
 *
 *  Must be called once and prior to invoking any other SecureCB APIs.
 *
 *  This API is only available to non-secure code
 */
void SecureCB_init(void);

/*
 *  ======== SecureCB_construct ========
 *  Construct a callback object and add it from the Callback dispatcher's
 *  service Queue.
 *
 *  Returns a handle to the callback object if successful, NULL otherwise;
 *
 *  The Callback is not immediately enabled.
 *
 *  This API is only available to non-secure code
 */
SecureCB_Handle SecureCB_construct(SecureCB_Object *scb, SecureCB_FuncPtr fxn,
        SecureCB_Arg arg);

/*
 *  ======== SecureCB_destruct ========
 *  Destruct a callback object and remove it from the Callback dispatcher's
 *  service Queue.
 *
 *  Returns SecureCB_STATUS_ERROR if object appears to be invalid.
 *
 *  This API is only available to non-secure code
 */
uint32_t SecureCB_destruct(SecureCB_Object *scb);

/*
 *  ======== SecureCB_enable ========
 *  Enable a callback so that the next time the callback dispatch's
 *  interrupt occurs, the callback will be invoked.
 *
 *  Returns SecureCB_STATUS_ERROR if object appears to be invalid.
 *
 *  This API is available to both non-secure and secure code
 */
__attribute__ ((always_inline)) __STATIC_INLINE
uint32_t SecureCB_enable(SecureCB_Object *scb)
{
    if (scb->crstat & SecureCB_CRSTAT_DEFINED) {
        scb->crstat |= SecureCB_CRSTAT_ENABLED;
        return (SecureCB_STATUS_OK);
    }
    return (SecureCB_STATUS_ERROR);
}

/*
 *  ======== SecureCB_setArg ========
 *  update the arg of an existing callback
 *
 *  Returns SecureCB_STATUS_ERROR if object appears to be invalid.
 *
 *  This API is available to both non-secure and secure code
 */
__attribute__ ((always_inline)) __STATIC_INLINE
uint32_t SecureCB_setArg(SecureCB_Handle scb, SecureCB_Arg arg)
{
    if (scb->crstat & SecureCB_CRSTAT_DEFINED) {
        scb->arg = arg;
        return (SecureCB_STATUS_OK);
    }
    return (SecureCB_STATUS_ERROR);
}

/*
 *  ======== SecureCB_getArg ========
 *  fetch the arg of an existing callback
 *
 *  Returns 0 if object appears to be invalid.
 *
 *  This API is available to both non-secure and secure code
 */
__attribute__ ((always_inline)) __STATIC_INLINE
SecureCB_Arg SecureCB_getArg(SecureCB_Handle scb)
{
    if (scb->crstat & SecureCB_CRSTAT_DEFINED) {
        return (scb->arg);
    }
    return (0);
}

/*
 *  ======== SecureCB_post ========
 *  Post a callback
 *
 *  Enables the callback and triggers the NS Callback
 *  Dispatcher's interrupt.
 *
 *  Returns SecureCB_STATUS_ERROR if object appears to be invalid.
 *
 *  This API is available to both non-secure and secure code
 */
__attribute__ ((always_inline)) __STATIC_INLINE
uint32_t SecureCB_post(SecureCB_Handle scb)
{
    uint32_t *ISPR = (uint32_t *)NVIC_ISPR_ADDRESS;
    uint32_t IRQn = scb->intNum - 16;

    if (scb->crstat & SecureCB_CRSTAT_DEFINED) {
        scb->crstat |= SecureCB_CRSTAT_ENABLED;
        ISPR[IRQn >> 5UL] = 1 << (IRQn & 0x1f);
        return (SecureCB_STATUS_OK);
    }
    return (SecureCB_STATUS_ERROR);
}

#if defined (__cplusplus)
}
#endif

#endif /* ti_spe_SecureCB__include */
