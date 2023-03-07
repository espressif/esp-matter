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
 *  @addtogroup  sco_hook   SCO Hook
 *  @ingroup     wicedbt
 *
 * Bluetooth SCO Hook Application Programming Interface
 *
 * The SCO Hook library provides API to access SCO/eSCO data (both received and sent).
 * This API can be used by the application to introduce a sound in the SCO data (to the Speaker
 * or from the Microphone) or to implement Noise Reduction and Echo Cancellation.
 * Incoming voice packets (i.e. from Audio Gateway) are passed to the application which can
 * modify them before they goes out of the chip (e.g. to external Codec for rendering).
 * Outgoing voice packets (e.g. from Microphone connected to an external Codec) are passed to the
 * application which can modify them before they are send (to the Audio Gateway).
 * The Audio Algorithm (DSP code) must be provided by the application.
 * Note that the CPU clock is increased to the maximum (i.e. 96 MHz) when the callback is called.
 * Once the callback returns, the CPU clock is set back to the normal frequency.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include <stdint.h>

/**
*  @addtogroup  sco_hook_data_types        Data Types
*  @ingroup     sco_hook
*
*  <b> Data Types </b> for @b SCO Hook.
*
*  @{
*/

/**
 * @brief SCO Hook Events.
 *
 * SCO Hook events received by the SCO Hook callback (see wiced_bt_sco_hook_callback_t)
 */
typedef enum
{
    WICED_BT_SCO_HOOK_EVT_MIC_SAMPLES = 1, /**< FW delivers Mic samples for processing */
    WICED_BT_SCO_HOOK_EVT_SPK_SAMPLES = 2, /**< FW delivers Spk samples for processing */
    WICED_BT_SCO_HOOK_EVT_SPK_INSERTED_ZEROS = 3, /* FW reports underrun count */
} wiced_bt_sco_hook_event_t;

/**
 * @brief Data associated with WICED_BT_SCO_HOOK_EVT_MIC_SAMPLES.
 *
 * This event is received when Audio samples have been received from the Microphone
 *
 */
typedef struct
{
    int16_t *p_input;               /**< Pointer to Received PCM samples (16 bits, Mono) */
    int16_t *p_output;              /**< Pointer to PCM samples which will be send (16 bits, Mono) */
    uint32_t sample_count;          /**< Number of PCM samples */
    union
    {
        uint32_t value;

        union
        {
            /**< bit definition used for input SCO (PCM) data. */
            struct
            {
                uint32_t timeSeqNum : 16;   // bits 0-15
                uint32_t res        : 8;    // bits 16-23
                uint32_t bufSmpCnt  : 8;    // bits 24-31
            } in;

            /**< bit definition used for output SCO (PCM) data to PCM. */
            struct
            {
                uint32_t corr   : 9;    // bits 0-8
                uint32_t sign   : 1;    // bit 9
                uint32_t tStamp : 21;   // bits 10-30
                uint32_t reset  : 1;    // bit  31
            } out;
        } sco_info;
    } inserted_silence_len;         /**< Number of silence PCM samples (0 for for Mic) */
} wiced_bt_sco_hook_event_mic_samples_t;

/**
 * @brief Data associated with WICED_BT_SCO_HOOK_EVT_SPK_SAMPLES.
 *
 * This event is received when Audio samples will be send to the Speaker
 *
 */
typedef wiced_bt_sco_hook_event_mic_samples_t wiced_bt_sco_hook_event_spk_samples_t;

/**
 * @brief Data associated with WICED_BT_SCO_HOOK_EVT_SPK_INSERTED_ZEROS.
 *
 * This event is received when Zeros are inserted for the Speaker
 *
 */
typedef wiced_bt_sco_hook_event_mic_samples_t wiced_bt_sco_hook_event_spk_inserted_zeros_t;

/**
 * @brief Union of data associated with SCO Hook events
 *
 */
typedef union
{
    wiced_bt_sco_hook_event_mic_samples_t mic_samples;
    wiced_bt_sco_hook_event_spk_samples_t spk_samples;
    wiced_bt_sco_hook_event_spk_inserted_zeros_t spk_zeros_inserted;
} wiced_bt_sco_hook_event_data_t;

/**
 * SCO Hook Callback function type wiced_bt_sco_hook_callback_t
 *
 *                  SCO Hook Event callback (registered with wiced_bt_sco_hook_init)
 *
 * @param[in]       event: SCO Hook event received
 * @param[in]       p_data : Data (pointer on union of structure) associated with the event
 *
 * @return NONE
 */
typedef void (wiced_bt_sco_hook_callback_t)(wiced_bt_sco_hook_event_t event,
        wiced_bt_sco_hook_event_data_t *p_data);

/**
 * @brief Low Level external FW function definition.
 *
 */
extern  wiced_result_t sco_aud_init(wiced_bt_sco_hook_callback_t *p_callback);

/**
*  @addtogroup  sco_hook_api_functions       API Functions
*  @ingroup     sco_hook
*
*  <b> API Functions </b> module for @b SCO Hook.
*
*  @{
*/

/**
 *
 * Function         wiced_bt_sco_hook_init
 *
 *                  This function is called for SCO Hook Initialization.
 *
 * @param[in]       p_callback: Pointer to application SCO Hook callback function
 *
 * @return          Result code (see wiced_result_t)
 *
 */
inline wiced_result_t wiced_bt_sco_hook_init(wiced_bt_sco_hook_callback_t *p_callback)
{
    return sco_aud_init(p_callback);
}

/**@} sco_hook_api_functions */

#ifdef __cplusplus
}
#endif

/**@} sco_hook */
