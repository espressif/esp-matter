/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _POWER_MODE_SWITCH_H_
#define _POWER_MODE_SWITCH_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef enum _app_wakeup_source
{
    kAPP_WakeupSourcePin0, /*!< Wakeup by wakeup pin0. */
    kAPP_WakeupSourcePin1, /*!< Wakeup by wakeup pin1. */
    kAPP_WakeupSourceRtc,  /*!< Wakeup by RTC.         */
    kAPP_WakeupSourceUart, /*!< Wakeup by UART.        */
} app_wakeup_source_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__cplusplus)
}
#endif

#endif /* _POWER_MODE_SWITCH_H_ */

/*******************************************************************************
 * EOF
 ******************************************************************************/
