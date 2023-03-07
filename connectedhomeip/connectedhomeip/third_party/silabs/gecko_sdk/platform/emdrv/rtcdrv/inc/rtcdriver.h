/***************************************************************************//**
 * @file
 * @brief RTCDRV timer API definition.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef __SILICON_LABS_RTCDRV_H__
#define __SILICON_LABS_RTCDRV_H__

#ifndef SL_SUPPRESS_DEPRECATION_WARNINGS_SDK_3_2
#warning "The RTC driver is deprecated and marked for removal in a later release. Please use the sleeptimer service instead."
#endif

#include <stdint.h>
#include <stdbool.h>

#include "ecode.h"
#include "rtcdrv_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup rtcdrv
 * @{
 ******************************************************************************/

#define ECODE_EMDRV_RTCDRV_OK                   (ECODE_OK)                               ///< A successful return value.
#define ECODE_EMDRV_RTCDRV_ALL_TIMERS_USED      (ECODE_EMDRV_RTCDRV_BASE | 0x00000001)   ///< No timers available.
#define ECODE_EMDRV_RTCDRV_ILLEGAL_TIMER_ID     (ECODE_EMDRV_RTCDRV_BASE | 0x00000002)   ///< An illegal timer ID.
#define ECODE_EMDRV_RTCDRV_TIMER_NOT_ALLOCATED  (ECODE_EMDRV_RTCDRV_BASE | 0x00000003)   ///< A timer is not allocated.
#define ECODE_EMDRV_RTCDRV_PARAM_ERROR          (ECODE_EMDRV_RTCDRV_BASE | 0x00000004)   ///< An illegal input parameter.
#define ECODE_EMDRV_RTCDRV_TIMER_NOT_RUNNING    (ECODE_EMDRV_RTCDRV_BASE | 0x00000005)   ///< A timer is not running.
#define ECODE_EMDRV_RTCDRV_NOT_INITIALIZED      (ECODE_EMDRV_RTCDRV_BASE | 0x00000006)   ///< The driver is not initialized.

/// @brief Timer ID.
typedef uint32_t RTCDRV_TimerID_t;

/***************************************************************************//**
 * @brief
 *  Typedef for the user-supplied callback function which is called when
 *  a timer elapses.
 *
 * @note This callback is called from within an interrupt handler with
 *       interrupts disabled.
 *
 * @param[in] id
 *   The timer ID.
 *
 * @param[in] user
 *   An extra parameter for the user application.
 ******************************************************************************/
typedef void (*RTCDRV_Callback_t)(RTCDRV_TimerID_t id, void *user);

/// @brief Timer type enumerator.
typedef enum {
  rtcdrvTimerTypeOneshot = 0,    ///< Oneshot timer.
  rtcdrvTimerTypePeriodic = 1    ///< Periodic timer.
} RTCDRV_TimerType_t;

Ecode_t   RTCDRV_AllocateTimer(RTCDRV_TimerID_t *id);
Ecode_t   RTCDRV_DeInit(void);
Ecode_t   RTCDRV_Delay(uint32_t ms);
Ecode_t   RTCDRV_FreeTimer(RTCDRV_TimerID_t id);
Ecode_t   RTCDRV_Init(void);
Ecode_t   RTCDRV_IsRunning(RTCDRV_TimerID_t id, bool *isRunning);
Ecode_t   RTCDRV_StartTimer(RTCDRV_TimerID_t id,
                            RTCDRV_TimerType_t type,
                            uint32_t timeout,
                            RTCDRV_Callback_t callback,
                            void *user);
Ecode_t   RTCDRV_StopTimer(RTCDRV_TimerID_t id);
Ecode_t   RTCDRV_TimeRemaining(RTCDRV_TimerID_t id, uint32_t *timeRemaining);

#if defined(EMDRV_RTCDRV_WALLCLOCK_CONFIG)
uint32_t  RTCDRV_GetWallClock(void);
uint32_t  RTCDRV_GetWallClockTicks32(void);
uint64_t  RTCDRV_GetWallClockTicks64(void);
uint64_t  RTCDRV_MsecsToTicks(uint32_t ms);
uint64_t  RTCDRV_SecsToTicks(uint32_t secs);
Ecode_t   RTCDRV_SetWallClock(uint32_t secs);
uint32_t  RTCDRV_TicksToMsec(uint64_t ticks);
uint64_t  RTCDRV_TicksToMsec64(uint64_t ticks);
uint32_t  RTCDRV_TicksToSec(uint64_t ticks);
#endif

/** @} (end addtogroup rtcdrv) */

#ifdef __cplusplus
}
#endif

#endif /* __SILICON_LABS_RTCDRV_H__ */
