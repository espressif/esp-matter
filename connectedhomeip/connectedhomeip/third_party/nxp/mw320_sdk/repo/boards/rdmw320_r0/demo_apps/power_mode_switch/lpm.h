/*
 * Copyright 2020, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _LPM_H_
#define _LPM_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*!
 * @brief .
 */
typedef void (*wakeup_handler)(IRQn_Type irq);

typedef struct _lpm_config
{
    /* If system in PM2/PM3 state less than the threshold in milliseconds, mcu will not go into PM2/PM3. */
    uint32_t threshold;
    /* Milliseconds cost in enter/exit of PM2/PM3. */
    uint32_t latency;
    /* Wakeup pin select */
    bool enableWakeupPin0;
    bool enableWakeupPin1;
    /* Handler on wakeup. Can be NULL to ignore. */
    wakeup_handler handler;
} lpm_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/* Initialize the Low Power Management.
 */
void LPM_Init(const lpm_config_t *config);

/* Deinitialize the Low Power Management */
void LPM_Deinit(void);

/* Set power mode.
 */
void LPM_SetPowerMode(uint32_t mode, uint32_t excludeIo);

/* Get low power mode previously set.
 */
uint32_t LPM_GetPowerMode(void);

/* LPM_SetPowerMode() won't switch system power status immediately,
 * instead, such operation is done by LPM_WaitForInterrupt().
 * It can be callled in idle task of FreeRTOS, or main loop in bare
 * metal application. The sleep depth of this API depends
 * on current power mode set by LPM_SetPowerMode().
 * The timeoutMilliSec means if no interrupt occurs before timeout, the
 * system will be waken up by RTC.
 * Return Power mode entered.
 */
uint32_t LPM_WaitForInterrupt(uint32_t timeoutMilliSec, bool allowDeepSleep);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#endif /* _LPM_H_ */
