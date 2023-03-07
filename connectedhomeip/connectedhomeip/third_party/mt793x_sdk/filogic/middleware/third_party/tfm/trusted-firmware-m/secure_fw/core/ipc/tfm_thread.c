/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <inttypes.h>
#include "tfm_arch.h"
#include "tfm_thread.h"
#include "tfm_utils.h"
#include "tfm_memory_utils.h"
#include "core/tfm_core_svc.h"
#include "spm_api.h"
#include "tfm_core_utils.h"

/* Force ZERO in case ZI(bss) clear is missing */
static struct tfm_core_thread_t *p_thrd_head = NULL;
static struct tfm_core_thread_t *p_runn_head = NULL;
static struct tfm_core_thread_t *p_curr_thrd = NULL;

/* Define Macro to fetch global to support future expansion (PERCPU e.g.) */
#define LIST_HEAD   p_thrd_head
#define RUNN_HEAD   p_runn_head
#define CURR_THRD   p_curr_thrd

static struct tfm_core_thread_t *find_next_running_thread(
                                                struct tfm_core_thread_t *pth)
{
    while (pth && pth->state != THRD_STATE_RUNNING) {
        pth = pth->next;
    }

    return pth;
}

/* To get next running thread for scheduler */
struct tfm_core_thread_t *tfm_core_thrd_get_next_thread(void)
{
    /*
     * First RUNNING thread has highest priority since threads are sorted with
     * priority.
     */
    return find_next_running_thread(RUNN_HEAD);
}

/* To get current thread for caller */
struct tfm_core_thread_t *tfm_core_thrd_get_curr_thread(void)
{
    return CURR_THRD;
}

/* Insert a new thread into list by descending priority (Highest at head) */
static void insert_by_prior(struct tfm_core_thread_t **head,
                            struct tfm_core_thread_t *node)
{
    if (*head == NULL || (node->prior <= (*head)->prior)) {
        node->next = *head;
        *head = node;
    } else {
        struct tfm_core_thread_t *iter = *head;

        while (iter->next && (node->prior > iter->next->prior)) {
            iter = iter->next;
        }
        node->next = iter->next;
        iter->next = node;
    }
}

/*
 * Set first running thread as head to reduce enumerate
 * depth while searching for a first running thread.
 */
static void update_running_head(struct tfm_core_thread_t **runn,
                                struct tfm_core_thread_t *node)
{
    if ((node->state == THRD_STATE_RUNNING) &&
        (*runn == NULL || (node->prior < (*runn)->prior))) {
        *runn = node;
    } else {
        *runn = LIST_HEAD;
    }
}

/* Set context members only. No validation here */
void tfm_core_thrd_init(struct tfm_core_thread_t *pth,
                        tfm_core_thrd_entry_t pfn, void *param,
                        uintptr_t stk_top, uintptr_t stk_btm)
{
    pth->prior = THRD_PRIOR_MEDIUM;
    pth->state = THRD_STATE_CREATING;
    pth->pfn = pfn;
    pth->param = param;
    pth->stk_btm = stk_btm;
    pth->stk_top = stk_top;
}

uint32_t tfm_core_thrd_start(struct tfm_core_thread_t *pth)
{
    /* Validate parameters before really start */
    if ((pth->state != THRD_STATE_CREATING) ||
        (pth->pfn == NULL)                  ||
        (pth->stk_btm == 0)                 ||
        (pth->stk_top == 0)) {
        return THRD_ERR_INVALID_PARAM;
    }

    /* Thread management runs in handler mode; set context for thread mode. */
    tfm_arch_init_context(&pth->arch_ctx, pth->param, (uintptr_t)pth->pfn,
                          pth->stk_btm, pth->stk_top);

    /* Insert a new thread with priority */
    insert_by_prior(&LIST_HEAD, pth);

    /* Mark it as RUNNING after insertion */
    tfm_core_thrd_set_state(pth, THRD_STATE_RUNNING);

    return THRD_SUCCESS;
}

void tfm_core_thrd_set_state(struct tfm_core_thread_t *pth, uint32_t new_state)
{
    TFM_CORE_ASSERT(pth != NULL && new_state < THRD_STATE_INVALID);

    pth->state = new_state;
    update_running_head(&RUNN_HEAD, pth);
}

/* Scheduling won't happen immediately but after the exception returns */
void tfm_core_thrd_activate_schedule(void)
{
    tfm_arch_trigger_pendsv();
}

void tfm_core_thrd_start_scheduler(struct tfm_core_thread_t *pth)
{
    /*
     * There is no selected thread before scheduler start, assign the caller
     * provided thread as the current thread. Update the hardware PSP/PSPLIM
     * with the value in thread context to ensure they are identical.
     * This function can be called only ONCE; further calling triggers assert.
     */
    TFM_CORE_ASSERT(CURR_THRD == NULL);
    TFM_CORE_ASSERT(pth != NULL);
    TFM_CORE_ASSERT(pth->arch_ctx.sp != 0);

    tfm_arch_update_ctx(&pth->arch_ctx);

    CURR_THRD = pth;

    tfm_core_thrd_activate_schedule();
}

void tfm_core_thrd_switch_context(struct tfm_arch_ctx_t *p_actx,
                                  struct tfm_core_thread_t *prev,
                                  struct tfm_core_thread_t *next)
{
    TFM_CORE_ASSERT(prev != NULL);
    TFM_CORE_ASSERT(next != NULL);

    /*
     * First, update latest context into the current thread context.
     * Then, update background context with next thread's context.
     */
    tfm_core_util_memcpy(&prev->arch_ctx, p_actx, sizeof(*p_actx));
    tfm_core_util_memcpy(p_actx, &next->arch_ctx, sizeof(next->arch_ctx));

    /* Update current thread indicator */
    CURR_THRD = next;
}
