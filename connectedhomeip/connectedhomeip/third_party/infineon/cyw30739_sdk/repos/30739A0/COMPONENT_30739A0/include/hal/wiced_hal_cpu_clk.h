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
* List of parameters and defined functions needed to access the
* CPU Clock.
*
*/

#ifndef __WICED_CPU_CLK_H__
#define __WICED_CPU_CLK_H__


/**  \addtogroup CPU Clock
* \ingroup HardwareDrivers
* Defines a driver to facilitate interfacing with the CPU clock.
*
* Use this to update the CPU clock frequency.
* Care should be taken by an App that it doesn't disable a frequency not set by it.
*
* @{ */

/******************************************************************************
*** Parameters.
***
*** The following parameters are used to configure the driver or define
*** return status. They are not modifiable.
******************************************************************************/
/** Clock Frequency*/
typedef enum
{
    WICED_CPU_CLK_24MHZ,    /**< 24Mhz clock frequency */
    WICED_CPU_CLK_32MHZ,    /**< 32Mhz clock frequency */
    WICED_CPU_CLK_48MHZ,    /**< 48Mhz clock frequency */
    WICED_CPU_CLK_96MHZ,    /**< 96Mhz clock frequency */
} wiced_cpu_clk_freq_t;

/******************************************************************************
*** Function prototypes and defines.
******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


/**
 * Function         wiced_update_cpu_clock
 *
 * Enables/Disables the given cpu clock frequency.
 * the actual clock frequency is selected appropriately within the driver.
 *
 * @param[in]    enable           :enable or disable clock frequency.
 * @param[in]    freq           :clock frequency to enable/disable.
 *
 *  Usage:The application should call the API with argument enable set to WICED_TRUE to request the FW to set the clock speed to the desired value.
 *        When the application is done with its work at this clock speed and wants to return to the previous speed,
 *        it shall call the API with the argument enable set to WICED_FALSE and set the argument freq to the same frequency that it earlier requested to be set.
 *
 * @return     : wiced_bool_t
 */
wiced_bool_t wiced_update_cpu_clock( wiced_bool_t enable, wiced_cpu_clk_freq_t freq);

/**
 * Function         wiced_hal_cpu_clk_ldo_voltage_force_high
 *
 * Force LDO voltage to higher one to prevent the I/O corruption.
 * If application is using a mechanism to increase CPU clock to 96 MHz dynamically,
 * user shall force the LDO voltage to higher one before start the mechanism.
 * Or the background I/O (baseband TX/RX, HCI UART) might be corrupted.
 *
 * @note
 * This function will set LDO voltage. User shall guarantee no background I/O
 * is executing or it might be corrupted as well. For example, user needs to flow HCI
 * UART off before calling this function then flow it on.
 */
void wiced_hal_cpu_clk_ldo_voltage_force_high(void);

/**
 * Function         wiced_hal_cpu_clk_ldo_voltage_release
 *
 * Release the force of LDO voltage by wiced_hal_cpu_clk_ldo_voltage_force_high.
 * User shall release the LDO voltage after the mechanism to increase CPU clock is
 * finished.
 *
 * @note
 * This function will set LDO voltage. User shall guarantee no background I/O
 * is executing or it might be corrupted as well. For example, user needs to flow HCI
 * UART off before calling this function then flow it on.
 */
void wiced_hal_cpu_clk_ldo_voltage_release(void);

/** @} */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __WICED_CPU_CLOCK_H__
