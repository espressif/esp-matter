/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _TFM_CORE_TOPOLOGY_H_
#define _TFM_CORE_TOPOLOGY_H_

#include "tfm_hal_device_header.h"
#include "cmsis_compiler.h"

#ifdef TFM_MULTI_CORE_TOPOLOGY
__STATIC_INLINE void tfm_core_topology_set_pendsv_priority(void)
{
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}
#else
__STATIC_INLINE void tfm_core_topology_set_pendsv_priority(void)
{
    /*
     * Set secure PendSV priority to the lowest in SECURE state.
     *
     * IMPORTANT NOTE:
     *
     * Although the priority of the secure PendSV must be the lowest possible
     * among other interrupts in the Secure state, it must be ensured that
     * PendSV is not preempted nor masked by Non-Secure interrupts to ensure
     * the integrity of the Secure operation.
     * When AIRCR.PRIS is set, the Non-Secure execution can act on
     * FAULTMASK_NS, PRIMASK_NS or BASEPRI_NS register to boost its priority
     * number up to the value 0x80.
     * For this reason, set the priority of the PendSV interrupt to the next
     * priority level configurable on the platform, just below 0x80.
     */
    NVIC_SetPriority(PendSV_IRQn, (1 << (__NVIC_PRIO_BITS - 1)) - 1);
}
#endif /* TFM_MULTI_CORE_TOPOLOGY */

#endif
