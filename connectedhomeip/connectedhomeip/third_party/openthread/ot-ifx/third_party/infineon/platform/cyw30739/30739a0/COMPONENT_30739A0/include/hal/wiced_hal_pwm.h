/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
*
* This file lists the API's and structs required to access the
* Pulse-Width Modulation (PWM) driver.
*
*/

#ifndef __WICED_PWM_H__
#define __WICED_PWM_H__

#include "wiced.h"

/**  \addtogroup PwmDriver Pulse Width Modulation (PWM)
* \ingroup HardwareDrivers
* @{
* Defines a driver to facilitate interfacing with the Pulse-Width
* Modulation (PWM) driver.
*
* Use this driver to output a PWM signal to a GPIO pin for external use. There
* are six, 16-bit hardware PWM channels avaliable (0-5). Typical use-cases
* include fine-control over small devices such as LEDs. Please reference the
* Kit Guide or HW User Manual for your device for more information.
*
*/

/******************************************************************************
 * Global Data Structure definitions                                          *
 ******************************************************************************/

/**
* \addtogroup group_pwm_data_structures Structures
* PWM Structures
* \{
*/

/// PWM HW block has 6 PWM channels each with its own 16 bit counter.
/// The first PWM channel is PWM0.
typedef enum
{
    PWM0  = 0,
    PWM1  = 1,
    PWM2  = 2,
#if defined(BCM20739) || defined(BCM20735) || defined(BCM20819)
    PWM4  = 4,
    PWM5  = 5,
    MAX_PWMS = 6
#else
    MAX_PWMS = 4
#endif
} PwmChannels;

/// Clock used for PWM. When LHL_CLK is set, 128 KHz is used.
/// PMU_CLK requires aclk to be configured first.
typedef enum
{
    LHL_CLK,
    PMU_CLK
} PwmClockType;

/// init count and toggle count for the PWM counters
typedef struct{
    uint32_t init_count;
    uint32_t toggle_count;
} wiced_pwm_config_t;

/** \} group_pwm_data_structures */

/******************************************************************************
*** Global functions .
******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


/**
* \addtogroup group_pwm_functions Functions
* PWM Functions
* \{
*/
///////////////////////////////////////////////////////////////////////////////
/// Configures, enables, and starts the PWM to be active on a
/// preconfigured GPIO pin.
///
/// (!) Note that the desired GPIO pin must have already been configured
/// as output. See the HW muxing section in the Kit Guide or HW User Manual for
/// your device for more information.
///
/// \param channel     - Desired PWM channel to use [0-5].
/// \param clk         - PMU_CLK or LHL_CLK
/// \param toggleCount - The number of ticks to wait before toggling the signal.
/// \param initCount   - Initial value for the counter.
/// \param invert      - 1 to invert the signal.
///
/// \return 1 if PWM was successfully started, 0 otherwise.
///
/// The following example outputs a signal on P28 using the PMU clock.
///
/// (!) Note that the maximum width or period of the PWM is 0xFFFF (16-bits).
///
/// (!) Note that if you use PMU_CLK instead of LHL_CLK, a call to
///     wiced_hal_aclk_enable() is required. When configuring aclk,
///     ACLK0 is not avaliable for use with PWM--only ACLK1.
///
/// (!) Note that each PWM<#> channel corresponds to a specific GPIO pin,
///     and said pin must be configured for output before PWM may use it.
///     Please reference the User Documentation for more information on which
///     pins support PWM on your platform.
///
/// \param channel     - Desired PWM channel to use [0-5].
/// \param clk         - PMU_CLK or LHL_CLK
/// \param toggleCount - The number of ticks to wait before toggling the signal.
/// \param initCount   - Initial value for the counter.
/// \param invert      - 1 to invert the signal.
///
/// \return 1 if PWM was successfully started, 0 otherwise.
///////////////////////////////////////////////////////////////////////////////
wiced_bool_t wiced_hal_pwm_start(uint8_t        channel,
                                         PwmClockType clk,
                                         uint32_t       toggleCount,
                                         uint32_t       initCount,
                                         wiced_bool_t       invert);

///////////////////////////////////////////////////////////////////////////////
/// Changes the PWM settings after the PWM HW has already been started.
///
/// (!) Note that the maximum width or period of the PWM is 0xFFFF (16-bits).
///
/// \param channel     - Desired PWM channel to set [0-5].
/// \param toggleCount - The number of ticks to wait before toggling the signal.
/// \param initCount   - Initial value for the counter.
///
/// \return 1 if PWM was successfully changed, 0 otherwise.
///////////////////////////////////////////////////////////////////////////////
wiced_bool_t wiced_hal_pwm_change_values(uint8_t channel,
                                                 uint32_t toggleCount,
                                                 uint32_t initCount);

///////////////////////////////////////////////////////////////////////////////
/// Returns the current toggle count setting for the corresponding PWM channel.
///
/// \param channel - Desired PWM channel from which to obtain the toggle count.
///
/// \return The value at which the PWM is going to toggle.
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_pwm_get_toggle_count(uint8_t channel);


///////////////////////////////////////////////////////////////////////////////
/// Returns the current initial count setting for the corresponding PWM channel.
///
/// \param channel - Desired PWM channel from which to obtain the initial count.
///
/// \return The initial count value of the PWM.
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_pwm_get_init_value(uint8_t channel);

///////////////////////////////////////////////////////////////////////////////
/// Disables the PWM channel.
///
/// \param channel - Desired PWM channel to stop/disable.
///
/// \return VOID
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_pwm_disable(uint8_t channel);

///////////////////////////////////////////////////////////////////////////////
/// Enables the PWM channel which is already preconfigured.
///
/// \param channel - Desired PWM channel to enable.
///
/// \return VOID
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_pwm_enable(uint8_t channel);

///////////////////////////////////////////////////////////////////////////////
/// Configure any GPIO as any PWM port
///
/// \param pin :   pin to configure as PWM
/// \param PWM:   which PWM channel to set the pin to (0 - 5)
///
/// \return VOID
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_pwm_configure_pin( uint8_t  pin, uint8_t PWM);

///////////////////////////////////////////////////////////////////////////////
/// Calculate PWM parameters
///
/// \param[in]   clock_frequency_in  - in clock frequency
/// \param[in]   duty_cycle          - duty cycle in percentage (0 to 100)
/// \param[in]   pwm_frequency_out   - Desire PWM output frequency
/// \param[out]  params_out          - PWM params
///
/// \return wiced_result_t
///////////////////////////////////////////////////////////////////////////////
wiced_result_t wiced_hal_pwm_get_params( uint32_t clock_frequency_in, uint32_t duty_cycle, uint32_t pwm_frequency_out, wiced_pwm_config_t * params_out);

/** \} group_pwm_functions */
/* @} */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __WICED_PWM_H__
