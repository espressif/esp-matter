/*
 * Copyright (c) 2017, Qorvo Inc
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

#ifndef _HAL_DEFS_H_
#define _HAL_DEFS_H_

#include "global.h"

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
//#define GP_LOCAL_LOG

#define GP_COMPONENT_ID GP_COMPONENT_ID_HALCORTEXM4

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
 #define HAL_GPIO_MAX_INTERRUPT_SOURCES              8       /* only 8 GPIOs can be mapped as interrupt sources */

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

extern volatile Bool hal_PolledInterruptPending;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static INLINE void hal_SetUnpolledInterruptBeforeSleep(void)
{
    hal_PolledInterruptPending = true;
}

static INLINE void hal_ClrUnpolledInterruptBeforeSleep(void)
{
    hal_PolledInterruptPending = false;
}

static INLINE Bool hal_HasUnpolledInterruptBeforeSleep(void)
{
    return hal_PolledInterruptPending;
}

ALWAYS_INLINE void hal_reset_always(void)
{
    /* Do the early overrides immediatly after reset or wakeup */
    rap_sys_reset_early();
}

/*****************************************************************************
 *                    Public Function Declarations
 *****************************************************************************/

// Sleep related
void hal_InitSleep(void);

void hal_resume(void);

void hal_set_crc_mode(UInt8 smram, UInt8 ucram);
void hal_sleep(void);
NORETURN void hal_go_to_sleep(UInt8 clk_mode);

// hal_UART.c
void hal_UartBeforeSleep(void);
void hal_UartAfterSleep(void);

// hal_wait_xxx.S
void hal_wait_loop(UInt32 loops);

#endif /*__HAL_DEFS_H__*/

