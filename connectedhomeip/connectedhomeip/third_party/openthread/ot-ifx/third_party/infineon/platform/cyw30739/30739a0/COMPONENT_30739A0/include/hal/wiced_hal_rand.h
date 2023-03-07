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
* List of parameters and defined functions needed to utilize the
*
*
*/

#ifndef __WICED_HAL_RAND_H__
#define __WICED_HAL_RAND_H__

#include "wiced.h"

/**  \addtogroup RandomNumberGenerator Random Number Generator (RNG)
* \ingroup HardwareDrivers
* @{
*
* Defines a driver for the Random Number Generator (RNG). The RNG uses a
* special hardware module to generate either a single 32-bit random number or
* fill a given array with 32-bit random numbers. These are useful for
* applications such as authentication.
*
*/

/******************************************************************************
*** Function prototypes.
******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


///////////////////////////////////////////////////////////////////////////////
/// Generates and returns a random 32-bit integer. Internal functions check
/// that the generating hardware is warmed up and ready before returning
/// the random value. If the hardware is too "cold" at the time of use,
/// the function will instead use the BT clock as a "seed" and generate a
/// "soft" random number.
///
/// \param none
///
/// \return A randomly generated 32-bit integer.
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_rand_gen_num(void);


///////////////////////////////////////////////////////////////////////////////
/// Fills a given array with randomly generated 32-bit integers. Uses the
/// function wiced_hal_rand_gen_num().
///
/// \param randNumberArrayPtr - Pointer to an array to be populated with
///                             the random numbers.
/// \param length             - Length of the array pointed to by
///                             randNumberArrayPtr.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_rand_gen_num_array(uint32_t* randNumberArrayPtr, uint32_t length);

///////////////////////////////////////////////////////////////////////////////
/// This functin returns a pseudo random number
///
/// \param none
///
/// \return A randomly generated 32-bit integer.
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_get_pseudo_rand_number(void);

/* @} */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __WICED_RAND_H__
