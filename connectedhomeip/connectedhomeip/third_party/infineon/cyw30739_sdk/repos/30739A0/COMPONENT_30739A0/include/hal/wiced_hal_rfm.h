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
* List of parameters and defined functions needed to enable auto
* eLNA switch.
*
*/

#ifndef __WICED_HAL_RFM_H__
#define __WICED_HAL_RFM_H__

#ifdef __cplusplus
extern "C" {
#endif


///////////////////////////////////////////////////////////////////////////////
/// Configures, enables, disables auto eLNA switch
///
/// \param enable - 1 enables auto eLNA switch, 0 disables auto eLNA switch
/// \param rx_pu_pin - GPIO number of RX_PU
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_rfm_auto_elna_enable(BOOL8 enabled, wiced_bt_gpio_numbers_t rx_pu_pin);

///////////////////////////////////////////////////////////////////////////////
/// Configures, enables, disables auto ePA switch
///
/// \param enable - 1 enables auto ePA switch, 0 disables auto ePA switch
/// \param tx_pu_pin - GPIO number of TX_PU
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_rfm_auto_epa_enable(BOOL8 enabled, wiced_bt_gpio_numbers_t tx_pu_pin);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
