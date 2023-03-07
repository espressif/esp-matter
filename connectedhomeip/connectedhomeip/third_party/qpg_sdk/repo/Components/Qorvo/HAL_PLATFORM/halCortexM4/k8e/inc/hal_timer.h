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

#ifndef _HAL_TIMER_H_
#define _HAL_TIMER_H_

#include "global.h"
/** @name halTimer_timerId_t */
//@{
/** @brief Specifying timer0 */
#define halTimer_timer0 0
/** @brief Specifying timer1 */
#define halTimer_timer1 1
/** @brief Specifying timer2 */
#define halTimer_timer2 2
/** @brief Specifying timer3 */
#define halTimer_timer3 3
/** @brief Specifying timer4 */
#define halTimer_timer4 4

#define MAX_NOF_TIMER 5

/** @name halTimer_clkSel_t */
//@{
/** @brief Specifying internal 16 MHz clock */
#define halTimer_clkSelIntClk 0
/** @brief Specifying timer0 as input clock */
#define halTimer_clkSelTmr0 1
/** @brief Specifying timer1 as input clock */
#define halTimer_clkSelTmr1 2
/** @brief Specifying timer2 as input clock */
#define halTimer_clkSelTmr2 3
/** @brief Specifying timer3 as input clock */
#define halTimer_clkSelTmr3 4
/** @brief Specifying timer4 as input clock */
#define halTimer_clkSelTmr4 5
//@}

/**
 * @brief Initialize  the timer HAL
 */
void halTimer_Init();

/**
 * @brief Initialize timer
 * Configures the timer
 * E.g., The callback on interrupt will be triggered at
 * ((1/16MHz) * (2**prescalerDiv) * threshold) when internal
 * clock is selected
 *
 * @param timerId       Identifies the timer
 * @param prescalerDiv  The prescaler value to scale down the input clock by a factor of 2**val
 * @param clkSel        The input clock to the timer
 * @param threshold     Threshold value at which the timer timer should wrap
 * @param Inthandler    Callback program to be called when the interrupt fires. If
 *                      this is set to NULL, interrupt mask will not be enabled for
 *                      this timer (no interrupt fires).
 * @param isPeriodic    Set state of periodicity timer interrupt
 */
void halTimer_initTimer(halTimer_timerId_t timerId,
        UInt8 prescalerDiv,
        halTimer_clkSel_t clkSel,
        UInt16 threshold,
        halTimer_cbTimerWrapInterruptHandler_t Inthandler,
        Bool isPeriodic);

/**
 * @brief Start the timer
 * Can also use halTimer_enableTimer()
 *
 * @param timerId       Identifies the timer
 */
void halTimer_startTimer(halTimer_timerId_t timerId);

/**
 * @brief Stop the timer
 * Can also use halTimer_enableTimer()
 *
 * @param timerId       Identifies the timer
 */
void halTimer_stopTimer(halTimer_timerId_t timerId);

/**
 * @brief Reset the timer
 *
 * @param timerId       Identifies the timer
 */
void halTimer_resetTimer(halTimer_timerId_t timerId);

/**
 * @brief Free the timer to be initialized and used
 * for other purposes
 *
 * @param timerId       Identifies the timer
 */
void halTimer_freeTimer(halTimer_timerId_t timerId);

/**
 * @brief Start or stop the timer
 *
 * @param timerId       Identifies the timer
 * @param val           1 - start timer, 0 - stop timer
 */
void halTimer_enableTimer(halTimer_timerId_t timerId, UInt8 val);

/**
 * @brief Check state of timer
 *
 * @param timerId       Identifies the timer
 * @param @return       1 - start timer, 0 - stop timer
 */
UInt32 halTimer_isEnabledTimer(halTimer_timerId_t timerId);

/**
 * @brief Set the prescaler value to scale down the input clock by a factor
 * of 2**val
 *
 * @param timerId       Identifies the timer
 * @param val           prescaler value
 */
void halTimer_setPrescalerDiv(halTimer_timerId_t timerId, UInt16 val);

/**
 * @brief Set the input clock for timer with timerId
 *
 * @param timerId       Identifies the timer
 * @param val           input clock of halTimer_clkSel_t
 */
void halTimer_setClkSel(halTimer_timerId_t timerId, halTimer_clkSel_t val);

/**
 * @brief Set the threshold value at which the timer should wrap
 *
 * @param timerId       Identifies the timer
 * @param val           the threshold value
 */
void halTimer_setThreshold(halTimer_timerId_t timerId, UInt16 threshold);

/**
 * @brief Get the threshold value at which the timer is wrapping
 *
 * @param timerId       Identifies the timer
 * @return              the threshold value
 */
UInt16 halTimer_getThreshold(halTimer_timerId_t timerId);

/**
 * @brief Read back the current timer value
 *
 * @param timerId       Identifies the timer
 * @return              current timer value
 */
UInt16 halTimer_getTimerValue(halTimer_timerId_t timerId);

/**
 * @brief Set the current timer value
 *
 * @param timerId       Identifies the timer
 * @param val           current timer value
 */
void halTimer_setTimerPreset(halTimer_timerId_t timerId, UInt16 val);

/**
 * @brief Set the mask of timer interrupt
 *
 * @param timerId       Identifies the timer
 * @param val           1 - enable, 0 - disable
 */
void halTimer_setMaskTimerWrapInterrupt(halTimer_timerId_t timerId, UInt8 val);
#endif // _HAL_TIMER_H_
