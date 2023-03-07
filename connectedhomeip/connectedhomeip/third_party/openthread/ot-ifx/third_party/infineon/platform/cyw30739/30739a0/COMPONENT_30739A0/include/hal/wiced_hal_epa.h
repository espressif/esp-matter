
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
* External Power Amplifier (ePA) driver.
*
*/

#ifndef __WICED_EPA_H__
#define __WICED_EPA_H__


/**  \addtogroup ExternalPowerAmplifierDriver External Power Amplifier
*   \ingroup HardwareDrivers
*
* @{
*
* Defines a driver to facilitate interfacing with a external power amplifier.
*
* Use this driver to output the CTX and CRX signals needed to drive an
* external power amplifier frontend module
*
*/

/******************************************************************************
*** Parameters.
***
*** The following parameters are used to configure the driver or define
*** return status. They are not modifiable.
******************************************************************************/

//
typedef enum
{
    EPA_VCC_1P8V,
    EPA_VCC_3P3V
} wiced_bt_epa_vcc_config_t;

/******************************************************************************
*** Function prototypes and defines.
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


///////////////////////////////////////////////////////////////////////////////
/// Configures, enables, disables external power amplifier control signals
/// CTX and CRX. CTX/CRX can be mapped to WICED_P02, WICED_P04, WICED_P10,
/// WICED_P16, WICED_P17, WICED_P26, WICED_P28, WICED_P34.
///
/// \param ctx    - Tx control signal of external power amplifier
/// \param crx    - Rx control signal of external power amplifier
/// \param enable - TRUE enables normal control, FALSE disables control and
///                 configures ctx=0 and crx =1
/// \param vbias  - EPA_VCC_1P8V or EPA_VCC_3P3V
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_epa_configure(uint8_t ctx, uint8_t crx, wiced_bool_t enable, uint8_t vbias);

/** @} */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __WICED_EPA_H__
