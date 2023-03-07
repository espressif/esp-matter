/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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

/*! @cond NODOC */

#ifndef ti_sysbios_hal_Hwi__include
#define ti_sysbios_hal_Hwi__include

/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __IAR_SYSTEMS_ICC__
#include <intrinsics.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if (defined(__ARM_ARCH) && ((__ARM_ARCH == 6) && (__ARM_ARCH_PROFILE == 'M'))) || \
    (defined(__CORE__) && (__CORE__ == __ARM6M__)) || \
    defined(__ARM_ARCH_6M__)
#include <ti/sysbios/family/arm/v6m/Hwi.h>
#elif  (defined(__ARM_ARCH) && ((__ARM_ARCH == 7) && (__ARM_ARCH_PROFILE == 'M'))) || \
    (defined(__CORE__) && (__CORE__ == __ARM7M__)) || \
    defined(__ARM_ARCH_7M__)
#include <ti/sysbios/family/arm/m3/Hwi.h>
#elif (defined(__ARM_ARCH) && ((__ARM_ARCH == 8) && (__ARM_ARCH_PROFILE == 'M'))) || \
    (defined(__CORE__) && (__CORE__ == __ARM8M__)) || \
    defined(__ARM_ARCH_8M__)
#include <ti/sysbios/family/arm/v8m/Hwi.h>
#endif

#include <ti/sysbios/runtime/Types.h>

/* Target unique Hwi_switchAndRunFunc */
extern void Hwi_switchAndRunFunc(void (*func)(void));

extern const unsigned int Hwi_disablePriority;

#if (defined(__ARM_ARCH) && ((__ARM_ARCH == 6) && (__ARM_ARCH_PROFILE == 'M'))) || \
    (defined(__CORE__) && (__CORE__ == __ARM6M__)) || \
    defined(__ARM_ARCH_6M__)

#if defined(__TI_COMPILER_VERSION__) && !defined(__clang__)

static inline unsigned int Hwi_disable()
{
    return ((unsigned int)__set_PRIMASK(1));
}

/*!
 */
static inline unsigned int Hwi_enable()
{
    return ((unsigned int)__set_PRIMASK(0));
}

/*!
 */
static inline void Hwi_restore(unsigned int key)
{
    __set_PRIMASK(key);
}

#elif defined(__IAR_SYSTEMS_ICC__)


static inline unsigned int Hwi_disable()
{
    unsigned int key;

    key = __get_PRIMASK();
    __set_PRIMASK(1);

    return (key);
}

static inline unsigned int Hwi_enable()
{
    unsigned int key;
    key = __get_PRIMASK();
    __set_PRIMASK(0);

    return (key);
}

static inline void Hwi_restore(unsigned int key)
{
    __set_PRIMASK(key);
}

#else /* V6M and clang or GNU */

static inline unsigned int Hwi_disable()
{
    unsigned int key;

    __asm__ __volatile__ (
            "mrs %0, primask\n\t"
            "cpsid i"
            : "=&r" (key)
            :
            : "memory"
            );
    return (key);
}

static inline unsigned int Hwi_enable()
{
    unsigned int key;

    __asm__ __volatile__ (
            "mrs %0, primask\n\t"
            "cpsid i"
            : "=&r" (key)
            :
            : "memory"
            );
    return (key);
}

static inline void Hwi_restore(unsigned int key)
{
    __asm__ __volatile__ (
            "msr primask, %0"
            :
            : "r" (key)
            : "memory"
            );
}

#endif

/* TODO -- replace with check for V7M */
#elif (1 /* V7M */)

#if defined(__TI_COMPILER_VERSION__) && !defined(__clang__)

static inline unsigned int Hwi_disable()
{
    return (_set_interrupt_priority(Hwi_disablePriority));
}

/*!
 */
static inline unsigned int Hwi_enable()
{
    return (_set_interrupt_priority(0));
}

/*!
 */
static inline void Hwi_restore(unsigned int key)
{
    (void)_set_interrupt_priority(key);
}


#elif defined(__IAR_SYSTEMS_ICC__)

static inline unsigned int Hwi_disable()
{
    unsigned int key;

    key = __get_BASEPRI();
    __set_BASEPRI(Hwi_disablePriority);

    return (key);
}

/*!
 */
static inline unsigned int Hwi_enable()
{
    unsigned int key;
    key = __get_BASEPRI();
    __set_BASEPRI(0);

    return (key);
}

/*!
 */
static inline void Hwi_restore(unsigned int key)
{
    __set_BASEPRI(key);
}

#else  /* clang or GNU */

static inline unsigned int Hwi_disable()
{
    unsigned int key;

    __asm__ __volatile__ (
            "mrs %0, basepri\n\t"
            "msr basepri, %1"
            : "=&r" (key)
            : "r" (Hwi_disablePriority)
            : "memory"
            );
    return (key);
}

/*!
 */
static inline unsigned int Hwi_enable()
{
    unsigned int key;

    __asm__ __volatile__ (
            "movw r12, #0\n\t"
            "mrs %0, basepri\n\t"
            "msr basepri, r12"
            : "=r" (key)
            :
            : "r12", "memory"
            );
    return (key);
}

/*!
 */
static inline void Hwi_restore(unsigned int key)
{
    __asm__ __volatile__ (
            "msr basepri, %0"
            :
            : "r" (key)
            : "memory"
            );
}

#endif /* defined(__ti__) && !defined(__clang__) */

#endif /* V7M */

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_hal_Hwi__include */

/*! @endcond */
