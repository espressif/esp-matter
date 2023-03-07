/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "tfm_thread.h"
#include "tfm_utils.h"
#include "tfm_wait.h"

void tfm_event_wait(struct tfm_event_t *pevnt)
{
    TFM_CORE_ASSERT(pevnt && pevnt->magic == TFM_EVENT_MAGIC);

    pevnt->owner = tfm_core_thrd_get_curr_thread();
    tfm_core_thrd_set_state(pevnt->owner, THRD_STATE_BLOCK);
    tfm_core_thrd_activate_schedule();
}

void tfm_event_wake(struct tfm_event_t *pevnt, uint32_t retval)
{
    TFM_CORE_ASSERT(pevnt && pevnt->magic == TFM_EVENT_MAGIC);

    if (pevnt->owner && pevnt->owner->state == THRD_STATE_BLOCK) {
        tfm_core_thrd_set_state(pevnt->owner, THRD_STATE_RUNNING);
        tfm_core_thrd_set_retval(pevnt->owner, retval);
        pevnt->owner = NULL;
        tfm_core_thrd_activate_schedule();
    }
}
