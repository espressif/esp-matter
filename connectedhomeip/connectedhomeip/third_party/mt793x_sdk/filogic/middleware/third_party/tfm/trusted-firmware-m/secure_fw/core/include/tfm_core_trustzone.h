/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_CORE_TRUSTZONE_H__
#define __TFM_CORE_TRUSTZONE_H__

/* This file holds specification or design defined TrustZone settings. */

/*
 * The non-secure entry stack guard words. To avoid fuzzing jump to
 * FNC_RETURN in NSPE, the non-secure entry address is stacked into
 * bottom (high end address) of secure stack. This would lead the
 * fuzzing jump of FNC_RETURN to the non-secure entry. Define a size
 * here for code reference.
 */
#define TFM_VENEER_STACK_GUARD_SIZE     8

/*
 * The numbers in 32bit words while basic FP involved in preempted context:
 * S0 - S15, FPSCR, Reserved
 */
#define TFM_BASIC_FP_CONTEXT_WORDS      18

/*
 * The numbers in 32bit words while addtional FP involved in preempted context:
 * S16 - S31
 */
#define TFM_ADDTIONAL_FP_CONTEXT_WORDS  16

/*
 * SG sets LR[0] to ZERO indicates a non-secure to secure transition.
 * Make sure to apply this mask on a 'LR' just after 'SG' (in veneer).
 */
#define TFM_VENEER_LR_BIT0_MASK         1

#endif /* __TFM_CORE_TRUSTZONE_H__ */
