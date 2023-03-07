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
 * @addtogroup  wicedbt_audio_utils     AIROC Audio Utilities
 * @ingroup     wicedbt_av
 *
 * @{
 */
#pragma once

#include "wiced.h"

/*****************************************************************************
**           Constants
*****************************************************************************/

/*****************************************************************************
 *          Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *          Function Prototypes
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialize the MP3 decoder.
 *
 * @param p_mem[in] : pointer to the allocated buffer used for decoding
 *
 * @return  WICED_TRUE  : Success
 *          WICED_FALSE : Fail
 */
wiced_bool_t wiced_mp3_codec_dec_init(void *p_mem);

/**
 * wiced_mp3_codec_dec_audio_frame_dec
 *
 * Decode the MP3 Audio Frame into PCM samples.
 * A MP3 Audio Frame includes a Audio Frame Header and a Audio Frame Body.
 * The MP3 Audio Frame shall start with the Audio Frame Header.
 *
 * @param[in]   p_in    : point to the input MP3 Audio Frame
 * @param[out]  p_out   : point to the output PCM samples
 * @param[in]   in_size :   size of the input MP3 Audio Frame in bytes
 *
 * @return      Size in bytes of the PCM data in output buffer
 */
uint32_t wiced_mp3_codec_dec_audio_frame_dec(void *p_in, void *p_out, uint32_t in_size);

/** @} wicedbt_audio_utils */


#ifdef __cplusplus
} /* extern "C" */
#endif
