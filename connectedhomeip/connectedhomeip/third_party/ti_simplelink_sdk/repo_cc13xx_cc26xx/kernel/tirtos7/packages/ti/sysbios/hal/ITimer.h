/*
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
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
 *
 */
/*!
 * @file ti/sysbios/hal/ITimer.h
 * @brief Interface for Timer Peripherals Manager.
 */

/*
 * This file does not have an include guard because the long/short name
 * mapping requires careful coordination of multiple Timer.h files. We need 
 * to carefully map the Timer_ short names to different Timer_ long names.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This file can be included by multiple Timer.h files. We have to protect these
 * macros from multiple definition.
 */
#ifndef Timer_E_invalidTimer
#define Timer_E_invalidTimer  "Invalid Timer Id %d"
#define Timer_E_notAvailable  "Timer not available %d"
#define Timer_E_cannotSupport "Timer cannot support requested period %d"
#endif

/*!
 * @brief Timer tick function prototype
 */
typedef void (*Timer_FuncPtr)(uintptr_t arg1);

/* ANY */
#define ti_sysbios_hal_Timer_ANY (~0U)

/*!
 * @brief Timer Start Modes
 *
 * @c(StartMode_AUTO) Statically created/constructed Timers will be started in
 * BIOS_start(). Dynamically created Timers will start at create() time. This
 * includes timers created before BIOS_start().
 *
 * @c(StartMode_USER) Timer will be started by the user using start().
 */
enum Timer_StartMode {
    Timer_StartMode_AUTO,
    Timer_StartMode_USER
};
/*!
 * @brief Timer Start Modes
 *
 * @c(StartMode_AUTO) Statically created/constructed Timers will be started in
 * BIOS_start(). Dynamically created Timers will start at create() time. This
 * includes timers created before BIOS_start().
 *
 * @c(StartMode_USER) Timer will be started by the user using start().
 */
typedef enum Timer_StartMode Timer_StartMode;

/*!
 * @brief Timer Run Modes
 *
 * @c(RunMode_CONTINUOUS) Timer is periodic and runs continuously.
 *
 * @c(RunMode_ONESHOT) Timer runs for a single period value and stops.
 *
 * @c(RunMode_DYNAMIC) Timer is dynamically reprogrammed for the next required
 * tick. This mode is intended only for use by the Clock module when it is
 * operating in TickMode_DYNAMIC; it is not applicable for user-created Timer
 * instances.
 */
enum Timer_RunMode {
    Timer_RunMode_CONTINUOUS,
    Timer_RunMode_ONESHOT,
    /*!
     * @brief dynamically reprogrammed (available on subset of devices)
     */
    Timer_RunMode_DYNAMIC
};
/*!
 * @brief Timer Run Modes
 *
 * @c(RunMode_CONTINUOUS) Timer is periodic and runs continuously.
 *
 * @c(RunMode_ONESHOT) Timer runs for a single period value and stops.
 *
 * @c(RunMode_DYNAMIC) Timer is dynamically reprogrammed for the next required
 * tick. This mode is intended only for use by the Clock module when it is
 * operating in TickMode_DYNAMIC; it is not applicable for user-created Timer
 * instances.
 */
typedef enum Timer_RunMode Timer_RunMode;

/*!
 * @brief Timer Status
 *
 * @c(Status_INUSE) Timer is in use. A timer is marked in use from the time it
 * gets created to the time it gets deleted.
 *
 * @c(Status_FREE) Timer is free and can be acquired using create.
 */
enum Timer_Status {
    Timer_Status_INUSE,
    Timer_Status_FREE
};
/*!
 * @brief Timer Status
 *
 * @c(Status_INUSE) Timer is in use. A timer is marked in use from the time it
 * gets created to the time it gets deleted.
 *
 * @c(Status_FREE) Timer is free and can be acquired using create.
 */
typedef enum Timer_Status Timer_Status;

/*!
 * @brief Timer period units
 *
 * @c(PeriodType_MICROSECS) Period value is in microseconds.
 *
 * @c(PeriodType_COUNTS) Period value is in counts.
 */
enum Timer_PeriodType {
    Timer_PeriodType_MICROSECS,
    Timer_PeriodType_COUNTS
};
/*!
 * @brief Timer period units
 *
 * @c(PeriodType_MICROSECS) Period value is in microseconds.
 *
 * @c(PeriodType_COUNTS) Period value is in counts.
 */
typedef enum Timer_PeriodType Timer_PeriodType;

#ifdef __cplusplus
}
#endif
