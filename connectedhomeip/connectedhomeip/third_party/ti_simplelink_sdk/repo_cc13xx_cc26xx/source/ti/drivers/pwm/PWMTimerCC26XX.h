/*
 * Copyright (c) 2015-2021, Texas Instruments Incorporated
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
 *  @file       PWMTimerCC26XX.h
 *  @brief      PWM driver implementation for CC26XX/CC13XX
 *
 *  # Overview #
 * The general PWM API should be used in application code, i.e. PWM_open()
 * should be used instead of PWMTimerCC26XX_open(). The board file will define the device
 * specific config, and casting in the general API will ensure that the correct
 * device specific functions are called.
 *
 * # General Behavior #
 * Before using PWM on CC26XX:
 *   - The Timer HW is configured and system dependencies (for example IOs,
 *     power, etc.) are set by calling PWM_open().
 *
 * # Error handling #
 * If unsupported arguments are provided to an API returning an error code, the
 * PWM configuration will *not* be updated and PWM will stay in the mode it
 * was already configured to.
 *
 * # Power Management #
 * The TI-RTOS power management framework will try to put the device into the most
 * power efficient mode whenever possible. Please see the technical reference
 * manual for further details on each power mode.
 *
 *  The PWMTimerCC26XX.h driver is not explicitly setting a power constraint when the
 *  PWM is running to prevent standby as this is assumed to be done in the
 *  underlying GPTimer driver.
 *  The following statements are valid:
 *    - After PWM_open(): The device is still allowed to enter Standby. When the
 *                        device is active the underlying GPTimer peripheral will
 *                        be enabled and clocked.
 *    - After PWM_start(): The device can only go to Idle power mode since the
 *                         high-frequency clock is needed for PWM operation:
 *    - After PWM_stop():  Conditions are equal as for after PWM_open
 *    - After PWM_close(): The underlying GPTimer is turned off and the device
 *                         is allowed to go to standby.
 *
 *  # Accuracy #
 *  The PWM output period and duty cycle are limited by the underlying timer.
 *  In PWM mode the timer is effectively 24 bits which results in a minimum
 *  frequency of 48MHz / (2^24-1) = 2.86Hz (349.525ms)
 *  The driver will round off the configured duty and period to a value limited
 *  by the timer resolution and the application is responsible for selecting
 *  duty and period that works with the underlying timer if high accuracy is
 *  needed.
 *
 *  The effect of this is most visible when using high output frequencies as the
 *  available duty cycle resolution is reduced correspondingly. For a 24MHz PWM
 *  only a 0%/50%/100% duty is available as the timer uses only counts 0 and 1.
 *  Similarly for a 12MHz period the duty cycle will be limited to a 12.5%
 *  resolution.
 *
 *  @note The PWM signals are generated using the high-frequency clock as
 *  a source. The internal RC oscillator is the source of the high frequency
 *  clock, but may not be accurate enough for certain applications. If very
 *  high-accuracy outputs are needed, the application should request using
 *  the external HF crystal:
 *  @code
 *  #include <ti/drivers/Power.h>
 *  #include <ti/drivers/power/PowerCC26XX.h>
 *  Power_setDependency(PowerCC26XX_XOSC_HF);
 *  @endcode
 *
 *  # Limitations #
 *  - The PWM output can currently not be synchronized with other PWM outputs
 *  - The PWM driver does not support updating duty and period using DMA.
 *  - Changes to the timer period are applied immediately, which can cause
 *    pulses to be too long or short unless period changes are applied close
 *    to a timeout. Does not apply to duty cycle, which is applied on timeout.
 *  # PWM usage #
 *
 *  ## Basic PWM output ##
 *  The below example will output a 8MHz PWM signal with 50% duty cycle.
 *  @code
 *  PWM_Handle pwmHandle;
 *  PWM_Params params;
 *
 *  PWM_Params_init(&params);
 *  params.idleLevel      = PWM_IDLE_LOW;
 *  params.periodUnits    = PWM_PERIOD_HZ;
 *  params.periodValue    = 8e6;
 *  params.dutyUnits      = PWM_DUTY_FRACTION;
 *  params.dutyValue      = PWM_DUTY_FRACTION_MAX / 2;
 *
 *  pwmHandle = PWM_open(CONFIG_PWM0, &params);
 *  if(pwmHandle == NULL) {
 *    Log_error0("Failed to open PWM");
 *    Task_exit();
 *  }
 *  PWM_start(pwmHandle);
 *  @endcode
 *
 *
 *******************************************************************************
 */
#ifndef ti_drivers_pwm__PWMTimerCC26XX_include
#define ti_drivers_pwm__PWMTimerCC26XX_include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/PWM.h>
#include <ti/drivers/timer/GPTimerCC26XX.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @name PWMTimerCC26XX specific control commands and arguments
 *  @{
*/

/*! Timer debug stall mode (stop PWM output debugger halts CPU)
    When enabled, PWM output will be HIGH when CPU is halted
 */
#define PWMTimerCC26XX_CMD_DEBUG_STALL    PWM_CMD_RESERVED + 0  /*!< @hideinitializer */
/*!
 *  @name Arguments for PWMTimerCC26XX_CMD_DEBUG_STALL
 *  @{
 */
#define CMD_ARG_DEBUG_STALL_OFF (uint32_t)GPTimerCC26XX_DEBUG_STALL_OFF  /*!< @hideinitializer */
#define CMD_ARG_DEBUG_STALL_ON  (uint32_t)GPTimerCC26XX_DEBUG_STALL_ON   /*!< @hideinitializer */
/*  @} */

/*  @} */

/* PWM function table pointer */
extern const PWM_FxnTable PWMTimerCC26XX_fxnTable;

/*!
 *  @brief  PWMTimer26XX Hardware attributes
 *
 *  These fields are used by the driver to set up underlying GPIO and GPTimer
 *  driver statically.
 */
typedef struct PWMTimerCC26XX_HwAttrs
{
    uint8_t pwmPin;               /*!< Pin to output PWM signal on */
    uint8_t gpTimerUnit;          /*!< GPTimer unit index (0A, 0B, 1A..) */
} PWMTimerCC26XX_HwAttrs;

/*!
 *  @brief  PWMTimer26XX Object
 *
 * These fields are used by the driver to store and modify PWM configuration
 * during run-time.
 * The application must not edit any member variables of this structure.
 * Appplications should also not access member variables of this structure
 * as backwards compatibility is not guaranteed.
 */
typedef struct PWMTimerCC26XX_Object
{
    bool                 isOpen;        /*!< open flag used to check if PWM is opened */
    bool                 isRunning;     /*!< running flag, set if the output is active */
    PWM_Period_Units     periodUnit;    /*!< Current period unit */
    uint32_t             periodValue;   /*!< Current period value in unit */
    uint32_t             periodCounts;  /*!< Current period in raw timer counts */
    PWM_Duty_Units       dutyUnit;      /*!< Current duty cycle unit */
    uint32_t             dutyValue;     /*!< Current duty cycle value in unit */
    uint32_t             dutyCounts;    /*!< Current duty in raw timer counts */
    PWM_IdleLevel        idleLevel;     /*!< PWM idle level when stopped / not started */
    GPTimerCC26XX_Handle hTimer;        /*!< Handle to underlying GPTimer peripheral */
} PWMTimerCC26XX_Object;

#ifdef __cplusplus
}
#endif
#endif /* ti_driver_pwm_PWMTimerCC26XX_include */
