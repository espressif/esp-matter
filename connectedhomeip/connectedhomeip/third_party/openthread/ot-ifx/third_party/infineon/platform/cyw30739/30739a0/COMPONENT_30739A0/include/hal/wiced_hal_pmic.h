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
* List of parameters and defined functions needed to utilize the PMIC API.
*
*/
#pragma once

#include <stdint.h>

/**  \addtogroup PMIC Power Management Integrated Circuit
*   \ingroup HardwareDrivers
*
* @{
*
* Defines a driver to facilitate interfacing with a PMIC.
*
*/

/**
 * PMIC Voltage Callback function type wiced_hal_pmic_voltage_callback_t
 *
 *                  PMIC Voltage Callback function (registered with
 *                  wiced_hal_pmic_voltage_callback_register)
 *
 * @param[in]       milli_volt: Voltage (in millivolts)
 *
 * @return          NONE
 */
typedef void (wiced_hal_pmic_voltage_callback_t)(uint16_t milli_volt);


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Low Level external FW function definition.
 *
 */
void pmu_ldo_trim_v2_set_pmic_fp(wiced_hal_pmic_voltage_callback_t *p_voltage_cb);

/**
 *
 * Function         wiced_bt_sco_hook_init
 *
 *                  This function is called for SCO Hook Initialization.
 *
 * @param[in]       p_callback: Pointer to application SCO Hook callback function
 *
 * @return          NONE
 *
 */
inline void wiced_hal_pmic_voltage_callback_register(wiced_hal_pmic_voltage_callback_t *p_cback)
{
    pmu_ldo_trim_v2_set_pmic_fp(p_cback);
}


#ifdef __cplusplus
} /* extern "C" */
#endif

/** @} */
