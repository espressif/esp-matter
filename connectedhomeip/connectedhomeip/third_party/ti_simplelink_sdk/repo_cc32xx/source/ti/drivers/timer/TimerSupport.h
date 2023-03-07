/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
/** ============================================================================
 *  @file       TimerSupport.h
 *
 *  @brief      Holder of common helper functions for the Timer driver
 *
 *  ============================================================================
 */

#ifndef ti_drivers_TimerSupport__include
#define ti_drivers_TimerSupport__include

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/Timer.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @brief  Function to disable the timer peripheral
 *
 *  @param[in]  handle          A Timer_Handle returned from Timer_open()
 */
extern void TimerSupport_timerDisable(Timer_Handle handle);

/*!
 *  @brief  Function to enable the timer peripheral
 *
 *  @param[in]  handle          A Timer_Handle returned from Timer_open()
 */
extern void TimerSupport_timerEnable(Timer_Handle handle);

/*!
 *  @brief  Function to determine if timer is full-width
 *
 *  @param[in]  handle          A Timer_Handle returned from Timer_open()
 *  @return Returns a bool indicating use of full-width timer
 */
extern bool TimerSupport_timerFullWidth(Timer_Handle handle);

/*!
 *  @brief  Function to load timer peripheral
 *
 *  @param[in]  handle          A Timer_Handle returned from Timer_open()
 */
extern void TimerSupport_timerLoad(Timer_Handle handle);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_TimerSupport__include */
