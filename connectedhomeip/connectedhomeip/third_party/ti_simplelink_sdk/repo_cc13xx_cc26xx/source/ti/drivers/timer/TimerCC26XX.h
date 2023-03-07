/*
 * Copyright (c) 2019-2020, Texas Instruments Incorporated
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
 */
/*!*****************************************************************************
 *  @file       TimerCC26XX.h
 *  @brief      Timer driver interface for CC26XX devices
 *
 *  # Operation #
 *  This driver implements a 32-bit or 16-bit general purpose timer for the
 *  CC26XX device.
 *
 *  The timer always operates in count up mode.
 *
 *  ============================================================================
 */

#ifndef ti_drivers_timer_TimerCC26XX__include
#define ti_drivers_timer_TimerCC26XX__include

#include <stdbool.h>
#include <stdint.h>

#include <ti/drivers/Timer.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/timer/GPTimerCC26XX.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 *  @def    TimerCC26XX_SubTimer
 *
 *  @brief  Sub-timers on the CC26XX
 *
 *  The timer peripheral supports full width and half width timer operation.
 *  Use the definitions in this enumerated type to specify a full width timer
 *  (32-bit) or half width timer (16-bit) in the hardware attributes. There are
 *  two half width timers per single timer peripheral. A 16-bit timer on this
 *  device has an 8-bit prescaler.
 */
typedef enum {
    TimerCC26XX_timer16A = 0x0001,    /*!< Half width timer A */
    TimerCC26XX_timer16B = 0x0002,    /*!< Half width timer B */
    TimerCC26XX_timer32  = 0x0003,    /*!< Full width timer   */
} TimerCC26XX_SubTimer;

/*!
 *  @brief TimerCC26XX Hardware Attributes
 *
 *  Timer hardware attributes that tell the TimerCC26XX driver specific GPTimer
 *  hardware configurations.
 *
 *  A sample structure is shown below:
 *  @code
 *  const TimerCC26XX_HWAttrs timerCC26XXHWAttrs[] =
 *  {
 *      {
 *          .gpTimerUnit = GPTIMER0A,
            .subTimer    = TimerCC26XX_timer16A
 *      },
 *      {
 *          .gpTimerUnit = GPTIMER0B,
            .subTimer    = TimerCC26XX_timer16B
 *      },
 *      {
 *          .gpTimerUnit = GPTIMER1A,
            .subTimer    = TimerCC26XX_timer32
 *      }
 *  };
 *  @endcode
 */
typedef struct {
    TIMER_BASE_HWATTRS

    /*! #GPTimerCC26XX_Config unit index (0A, 0B, 1A..) */
    uint8_t              gpTimerUnit;

    /*! Specifies a full-width timer or half-width timer. */
    TimerCC26XX_SubTimer subTimer;

} TimerCC26XX_HWAttrs;

/*!
 *  @brief TimerCC26XX_Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    TIMER_BASE_OBJECT

    /* GPTimer handle used by Timer interface */
    GPTimerCC26XX_Handle   gptHandle;

    /* Flag to show GPTimer is open */
    bool                   isOpen;

} TimerCC26XX_Object;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_timer_TimerCC26XX__include */
