/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __TFM_THREAD_H__
#define __TFM_THREAD_H__

#include <stdint.h>
#include <stddef.h>
#include "tfm_arch.h"
#include "cmsis_compiler.h"

/* State code */
#define THRD_STATE_CREATING       0
#define THRD_STATE_RUNNING        1
#define THRD_STATE_BLOCK          2
#define THRD_STATE_DETACH         3
#define THRD_STATE_INVALID        4

/* Security attribute - default as security */
#define THRD_ATTR_SECURE_OFFSET   16
#define THRD_ATTR_SECURE          (0)
#define THRD_ATTR_NON_SECURE      (1 << THRD_ATTR_SECURE_OFFSET)

/* Lower value has higher priority */
#define THRD_PRIOR_MASK           0xFF
#define THRD_PRIOR_HIGHEST        0x0
#define THRD_PRIOR_MEDIUM         0x7F
#define THRD_PRIOR_LOWEST         0xFF

/* Error code */
#define THRD_SUCCESS              0
#define THRD_ERR_INVALID_PARAM    1

/* Thread entry function type */
typedef void *(*tfm_core_thrd_entry_t)(void *);

/* Thread context */
struct tfm_core_thread_t {
    tfm_core_thrd_entry_t pfn;          /* entry function               */
    void            *param;             /* entry parameter              */
    uintptr_t       stk_btm;            /* stack bottom (lower address) */
    uintptr_t       stk_top;            /* stack top    (higher address)*/
    uint32_t        prior;              /* priority                     */
    uint32_t        state;              /* state                        */

    struct tfm_arch_ctx_t    arch_ctx;  /* State context                */
    struct tfm_core_thread_t *next;     /* next thread in list          */
};

/*
 * Initialize a thread context with the necessary info.
 *
 * Parameters :
 *  pth         -    pointer of caller provided thread context
 *  pfn         -    thread entry function
 *  param       -    thread entry function parameter
 *  stk_top     -    stack pointer top (higher address)
 *  stk_btm     -    stack pointer bottom (lower address)
 *
 * Notes :
 *  Thread contex rely on caller allocated memory; initialize members in
 *  context. This function does not insert thread into schedulable list.
 */
void tfm_core_thrd_init(struct tfm_core_thread_t *pth,
                        tfm_core_thrd_entry_t pfn, void *param,
                        uintptr_t stk_top, uintptr_t stk_btm);

/*
 * Set thread priority.
 *
 * Parameters :
 *  pth         -     pointer of thread context
 *  prior       -     priority value (0~255)
 *
 * Notes :
 *  Set thread priority. Priority is set to THRD_PRIOR_MEDIUM in
 *  tfm_core_thrd_init().
 */
void __STATIC_INLINE tfm_core_thrd_set_priority(struct tfm_core_thread_t *pth,
                                                uint32_t prior)
{
    pth->prior &= ~THRD_PRIOR_MASK;
    pth->prior |= prior & THRD_PRIOR_MASK;
}

/*
 * Set thread security attribute.
 *
 * Parameters :
 *  pth         -     pointer of thread context
 *  attr_secure -     THRD_ATTR_SECURE or THRD_ATTR_NON_SECURE
 *
 * Notes
 *  Reuse prior of thread context to shift down non-secure thread priority.
 */
void __STATIC_INLINE tfm_core_thrd_set_secure(struct tfm_core_thread_t *pth,
                                              uint32_t attr_secure)
{
    pth->prior &= ~THRD_ATTR_NON_SECURE;
    pth->prior |= attr_secure;
}

/*
 * Set thread state.
 *
 * Parameters :
 *  pth         -     pointer of thread context
 *  new_state   -     new state of thread
 *
 * Return :
 *  None
 *
 * Notes :
 *  Thread state is not changed if invalid state value inputed.
 */
void tfm_core_thrd_set_state(struct tfm_core_thread_t *pth, uint32_t new_state);

/*
 * Get thread state.
 *
 * Parameters :
 *  pth         -     pointer of thread context
 *
 * Return :
 *  State of thread
 */
uint32_t __STATIC_INLINE tfm_core_thrd_get_state(struct tfm_core_thread_t *pth)
{
    return pth->state;
}

/*
 * Set thread state return value.
 *
 * Parameters :
 *  pth         -     pointer of thread context
 *  retval      -     return value to be set for thread state
 *
 * Notes :
 *  This API is useful for blocked syscall blocking thread. Syscall
 *  could set its return value to the caller before caller goes.
 */
void __STATIC_INLINE tfm_core_thrd_set_retval(struct tfm_core_thread_t *pth,
                                              uint32_t retval)
{
    TFM_STATE_RET_VAL(&pth->arch_ctx) = retval;
}

/*
 * Validate thread context and insert it into schedulable list.
 *
 * Parameters :
 *  pth         -     pointer of thread context
 *
 * Return :
 *  THRD_SUCCESS for success. Or an error is returned.
 *
 * Notes :
 *  This function validates thread info. It returns error if thread info
 *  is not correct. Thread is avaliable after successful tfm_core_thrd_start().
 */
uint32_t tfm_core_thrd_start(struct tfm_core_thread_t *pth);

/*
 * Get current running thread.
 *
 * Return :
 *  Current running thread context pointer.
 */
struct tfm_core_thread_t *tfm_core_thrd_get_curr_thread(void);

/*
 * Get next running thread in list.
 *
 * Return :
 *  Pointer of next thread to be run.
 */
struct tfm_core_thread_t *tfm_core_thrd_get_next_thread(void);

/*
 * Start scheduler for existing threads
 *
 * Parameters:
 *  pth         -     pointer of the caller context collecting thread
 *
 * Notes :
 *  This function should be called only ONCE to start the scheduler.
 *  Caller needs to provide a thread object to collect current context.
 *  The usage of the collected context is caller defined.
 */
void tfm_core_thrd_start_scheduler(struct tfm_core_thread_t *pth);

/*
 * Activate a scheduling action after exception.
 *
 * Notes :
 *  This function could be called multiple times before scheduling.
 */
void tfm_core_thrd_activate_schedule(void);

/*
 * Save current architecture context into 'prev' thread and switch to 'next'.
 *
 * Parameters :
 *  p_actx      -     latest caller context
 *  prev        -     previous thread to be switched out
 *  next        -     thread to be run
 *
 * Notes :
 *  This function could be called multiple times before scheduling.
 */
void tfm_core_thrd_switch_context(struct tfm_arch_ctx_t *p_actx,
                                  struct tfm_core_thread_t *prev,
                                  struct tfm_core_thread_t *next);

#endif
