/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *   Hardware Abstraction Layer for the part of HAL that needs WB.
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _HAL_WB_H_
#define _HAL_WB_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "hal_RomCode_WB.h"
#else //defined(GP_DIVERSITY_ROM_CODE)


#include "gpHal_reg.h"
#include "gpAssert.h"
#include "gpHal_kx_defs.h"

#ifndef GP_COMPONENT_ID
#define GP_COMPONENT_ID GP_COMPONENT_ID_HALCORTEXM4
#define GP_COMPONENT_ID_DEFINED_IN_HEADER
#endif //GP_COMPONENT_ID

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 *                    TIMER
 *****************************************************************************/

static INLINE UInt32 hal_timer_get_current_time_us(void)
{
#if ! defined(GP_DIVERSITY_JUMPTABLES) || !defined(GP_DIVERSITY_ROM_CODE)
    GP_ASSERT_SYSTEM(GP_WB_READ_ES_INIT_TIME_REFERENCE_BUSY() == 0);
#endif //! defined(GP_DIVERSITY_JUMPTABLES) || !defined(GP_DIVERSITY_ROM_CODE)
    return GP_WB_READ_ES_AUTO_SAMPLED_SYMBOL_COUNTER();
}

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "hal_CodeJumpTableFlash_Defs_WB.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

// Not a true 32-bit/32us timer - overflow will happen at 27-bit
#define HAL_TIMER_GET_CURRENT_TIME(time)                \
    do {                                                \
        (time) = hal_timer_get_current_time_us() >> 5;  \
    } while (false)


#define HAL_TIMER_GET_CURRENT_TIME_1US(time)            \
    do {                                                \
        (time) = hal_timer_get_current_time_us();       \
    } while (false)

#define HAL_TIMER_START(x)
#define HAL_TIMER_STOP()
#define HAL_TIMER_RESTART()

#ifdef GP_COMPONENT_ID_DEFINED_IN_HEADER
#undef GP_COMPONENT_ID
#undef GP_COMPONENT_ID_DEFINED_IN_HEADER
#endif //GP_COMPONENT_ID_DEFINED_IN_HEADER

#ifdef __cplusplus
}
#endif

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif //_HAL_WB_H_
