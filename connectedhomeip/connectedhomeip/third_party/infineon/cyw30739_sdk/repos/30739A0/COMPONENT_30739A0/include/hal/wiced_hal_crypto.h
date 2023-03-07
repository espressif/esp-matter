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
* Definitions of the crypto functions: sha2_hmac.
*
*/

#ifndef __WICED_HAL_CRYPTO_H__
#define __WICED_HAL_CRYPTO_H__

/******************************************************************************
*** Function prototypes and defines.
******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
/// Calculated HMAC-SHA. Output = HMAC-SHA-256(224)( hmac key, input buffer )
///
/// \param key     - HMAC secret key
/// \param keylen  - length of the HMAC key
/// \param input   - buffer holding the  data
/// \param ilen    - length of the input data
/// \param output  - HMAC-SHA-224/256 result
/// \param is224   - 0 = use SHA256, 1 = use SHA224
////////////////////////////////////////////////////////////////////////////////
void wiced_sha2_hmac( const unsigned char *key, uint32_t keylen,
                      const unsigned char *input, uint32_t ilen,
                      unsigned char output[32], int32_t is224 );

#endif // __WICED_HAL_CRYPTO_H__
