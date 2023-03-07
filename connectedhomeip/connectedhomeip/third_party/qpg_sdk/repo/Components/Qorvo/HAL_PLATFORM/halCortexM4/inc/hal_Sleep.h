/*
 * Copyright (c) 2021, Qorvo Inc
 *
 * hal_Sleep.h
 *   Hardware Abstraction Layer Sleep for ARM devices.
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

#ifndef _HAL_SLEEP_H_
#define _HAL_SLEEP_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/** @brief Default max idle time for sleep check function*/
#define HAL_SLEEP_MAX_IDLE_TIME_US          0xFFFFFFFF

/** @brief Allow sleep only if no events are pending
*/
#define GP_SCHED_NO_EVENTS_GOTOSLEEP_THRES ((UInt32)(0xFFFFFFFF))

/** @brief Default time between events before going to sleep is considered */
#ifndef HAL_DEFAULT_GOTOSLEEP_THRES
#define HAL_DEFAULT_GOTOSLEEP_THRES GP_SCHED_NO_EVENTS_GOTOSLEEP_THRES
#endif
/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
/**
 * @brief Recursive management of the sleep state of the processor.
 *        Multiple components could call it to disable sleep, the last one will re-enable it.
 *        Enabling twice will assert
 *
 * @param Enables or disables the processor to sleep state
 *
*/
void hal_SleepSetGotoSleepEnable(Bool enable);

/**
 * @brief Sets the sleep state threshold of the processor.
 *
 * @param Sets the minimum idle time in us when the processor goes into sleep mode
 *
*/
void hal_SleepSetGotoSleepThreshold(UInt32 threshold);

/**
 * @brief Gets the sleep state threshold of the processor.
 *
 * @return Gets the minimum idle time in us when the processor goes into sleep mode
 *
*/
UInt32 hal_SleepGetGotoSleepThreshold(void);

/**
 * @brief Checks if the processor can go into sleep mode for the given sleep time
 *
 * @param Expected idle time in us
 *
 * @return Can the CPU go to sleep (true / false)
 *
*/
Bool hal_SleepCheck(UInt32 expectedIdleTime);

#endif //_HAL_SLEEP_H_
