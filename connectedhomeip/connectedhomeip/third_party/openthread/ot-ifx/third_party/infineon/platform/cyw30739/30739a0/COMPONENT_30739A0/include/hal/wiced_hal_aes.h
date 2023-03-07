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
* List of parameters and defined functions needed to calculate the
* AES with Hardware acceleration.
*
*/

#ifndef __WICED_AES_H__
#define __WICED_AES_H__

/**  \addtogroup AesCalculation
* \ingroup HardwareDrivers
* @{
*
* Defines a driver for the AES calculation with Hardware acceleration.
* It uses a 16 bytes key and 16 bytes input data to generate a 16 bytes ouput data.
* These are useful for applications such as authentication.
*
*/

/******************************************************************************
*** Function prototypes and defines.
******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
/// Calculate AES encryption by using Hardware acceleration
///
/// \param in_data  - point to input data buffer
/// \param out_data - point to output data buffer
/// \param key      - point to key data buffer
///
/// \return         - none
////////////////////////////////////////////////////////////////////////////////
void wiced_bcsulp_AES(uint8_t* key, uint8_t* in_data, uint8_t* out_data);

/* @} */

#endif // __WICED_AES_H__
