/*******************************************************************************
* Copyright (c) 2015-2019 Cadence Design Systems, Inc.
* 
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to use this Software with Cadence processor cores only and 
* not with any other processors and platforms, subject to
* the following conditions:
* 
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************************/
/*******************************************************************************
 * xf-hal.h
 *
 * Platform-specific HAL definitions
 *******************************************************************************/

#ifndef __XF_H
#error "xf-hal.h mustn't be included directly"
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

/* ...primitive types */
#include "xf-dp_types.h"

/* ...XOS runtime */
//#include <xtensa/xtruntime.h>
#ifdef __TOOLS_RF2__
#include <xtensa/xos/xos.h>
#else   // #ifdef __TOOLS_RF2__
#include <xtensa/xos.h>
#endif  // #ifdef __TOOLS_RF2__

/*******************************************************************************
 * Auxilliary macros definitions
 ******************************************************************************/

/* ...use system-specific cache-line size */
#define XF_PROXY_ALIGNMENT              32

/* ...properly aligned shared memory structure */
#define __xf_shmem__        __attribute__((__aligned__(XF_PROXY_ALIGNMENT)))

/* ...use hifi core agnostic maximum cache-line size */
#define XF_PROXY_MAX_CACHE_ALIGNMENT    256 

/* ...properly aligned shared memory structure */
#define __xf_shmem_max_cache__          __attribute__((__aligned__(XF_PROXY_MAX_CACHE_ALIGNMENT)))

/*******************************************************************************
 * Interrupt control
 ******************************************************************************/

/* ...disable interrupts on given core */
static inline UWORD32 xf_isr_disable(UWORD32 core)
{
#if 0
    UWORD32 cur_level = xos_get_intlevel();

    /* ...no actual dependency on the core identifier */
    xos_set_intlevel(XCHAL_EXCM_LEVEL);
    
    return cur_level;
#else
    return 0;
#endif
}

/* ...enable interrupts on given core */
static inline void xf_isr_restore(UWORD32 core, UWORD32 status)
{
#if 0
    /* ...no actual dependency on the core identifier */
    xos_restore_intlevel(status);
#else
#endif
}

/*******************************************************************************
 * Auxiliary system-specific functions
 ******************************************************************************/

#if XF_CFG_CORES_NUM > 1
/* ...current core identifier (from HW) */
static inline UWORD32 xf_core_id(void)
{
    /* ...retrieve core identifier from HAL */
    return (UWORD32) xthal_get_prid();
}
#else
#define xf_core_id()        0
#endif

/*******************************************************************************
 * Atomic operations (atomicity is assured on local core only)
 ******************************************************************************/

static inline int xf_atomic_test_and_set(volatile UWORD32 *bitmap, UWORD32 mask)
{
    UWORD32     status;
    UWORD32     v;

    /* ...atomicity is assured by interrupts masking */
#ifdef __TOOLS_RF2__
    status = xos_disable_intlevel();
    v = *bitmap, *bitmap = v | mask;
    xos_restore_intlevel(status);
#else   // #ifdef __TOOLS_RF2__
    status = xos_disable_interrupts();
    v = *bitmap, *bitmap = v | mask;
    xos_restore_interrupts(status);
#endif  // #ifdef __TOOLS_RF2__

    return !(v & mask);
}

static inline int xf_atomic_test_and_clear(volatile UWORD32 *bitmap, UWORD32 mask)
{
    UWORD32     status;
    UWORD32     v;

    /* ...atomicity is assured by interrupts masking */
#ifdef __TOOLS_RF2__
    status = xos_disable_intlevel();
    v = *bitmap, *bitmap = v & ~mask;
    xos_restore_intlevel(status);
#else   // #ifdef __TOOLS_RF2__
    status = xos_disable_interrupts();
    v = *bitmap, *bitmap = v & ~mask;
    xos_restore_interrupts(status);
#endif  // #ifdef __TOOLS_RF2__
    
    return (v & mask);
}

static inline UWORD32 xf_atomic_set(volatile UWORD32 *bitmap, UWORD32 mask)
{
    UWORD32     status;
    UWORD32     v;

    /* ...atomicity is assured by interrupts masking */
#ifdef __TOOLS_RF2__
    status = xos_disable_intlevel();
    v = *bitmap, *bitmap = (v |= mask);
    xos_restore_intlevel(status);
#else   // #ifdef __TOOLS_RF2__
    status = xos_disable_interrupts();
    v = *bitmap, *bitmap = (v |= mask);
    xos_restore_interrupts(status);
#endif  // #ifdef __TOOLS_RF2__

    return v;
}

static inline UWORD32 xf_atomic_clear(volatile UWORD32 *bitmap, UWORD32 mask)
{
    UWORD32     status;
    UWORD32     v;
    
    /* ...atomicity is assured by interrupts masking */
#ifdef __TOOLS_RF2__
    status = xos_disable_intlevel();
    v = *bitmap, *bitmap = (v &= ~mask);
    xos_restore_intlevel(status);
#else   // #ifdef __TOOLS_RF2__
    status = xos_disable_interrupts();
    v = *bitmap, *bitmap = (v &= ~mask);
    xos_restore_interrupts(status);
#endif  // #ifdef __TOOLS_RF2__

    return v;
}

/*******************************************************************************
 * Abortion macro (debugger should be configured)
 ******************************************************************************/

/* ...breakpoint function */
extern void breakpoint(void);

/* ...abort execution (enter into debugger) */
#define __xf_abort()            breakpoint()
