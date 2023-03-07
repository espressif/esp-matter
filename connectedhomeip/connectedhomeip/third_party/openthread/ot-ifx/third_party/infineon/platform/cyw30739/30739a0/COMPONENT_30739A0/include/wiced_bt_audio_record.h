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
 *  @addtogroup  audio_record   Audio Record
 *  @ingroup     wicedbt_a2dp
 *
 * Bluetooth Audio Record Application Programming Interface
 *
 * The Audio Record library provides APIs to allow the Wiced application to receive audio input.
 * The audio record interface only works when A2DP or SCO is not ongoing. Whenever the application
 * received an event to retrieve the incoming audio data, it must do so to relief the audio buffer
 * as soon as possible.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include <stdint.h>
#include "wiced.h"

/**
*  @addtogroup  audio_record_data_types        Data Types
*  @ingroup     audio_record
*
*  <b> Data Types </b> for @b Audio Record.
*
*  @{
*/

/**
 * @brief Audio Record Events.
 *
 * Audio Record events received by the Audio Record callback (see wiced_bt_audio_record_callback_t)
 */
typedef enum
{
    WICED_BT_AUDIO_RECORD_EVT_DATA_READY = 3, /* Notification to retrieve audio data */
    WICED_BT_AUDIO_RECORD_EVT_AUDIO_INFO,     /* Indicates the sample rate for the pending audio record */
} wiced_bt_audio_record_event_t;


/**
 * @brief Data associated with WICED_BT_AUDIO_RECORD_EVT_DATA_READY.
 *
 * This event is received when FW data is ready to be retrieved
 *
 */
typedef struct
{
    int16_t *p_data;             /* Incoming data buffer */
    int16_t data_len;            /* Incoming data length */
} wiced_bt_audio_record_event_data_ready_t;

/**
 * @brief Data associated with WICED_BT_AUDIO_RECORD_EVT_AUDIO_INFO
 *
 * This event is received when FW indicates the Sample Rate and the Buffer size to use
 * for the Audio Recording
 *
 */
typedef struct
{
    uint16_t sample_rate;        /* Sample rate for audio recording */
    uint16_t buffer_size;        /* Audio buffer size */
} wiced_bt_audio_record_event_audio_info_t;

/**
 * @brief Union of data associated with NREC events
 *
 */
typedef union
{
    wiced_bt_audio_record_event_data_ready_t data_ready;
    wiced_bt_audio_record_event_audio_info_t audio_info;
} wiced_bt_audio_record_event_data_t;

/**
 * Audio Record Callback function type wiced_bt_audio_record_callback_t
 *
 *                  Audio Record Event callback (registered with wiced_bt_audio_record_init)
 *
 * @param[in]       event: Audio Record event received
 * @param[in]       p_data : Data (pointer on union of structure) associated with the event
 *
 * @return NONE
 */
typedef void (wiced_bt_audio_record_callback_t)(wiced_bt_audio_record_event_t event,
        wiced_bt_audio_record_event_data_t *p_data);

/**
*  @addtogroup  audio_record_api_functions       API Functions
*  @ingroup     audio_record
*
*  <b> API Functions </b> module for @b Audio Record.
*
*  @{
*/

/**
 *
 * Function         wiced_bt_audio_record_init
 *
 *                  This function is called for Audio Record Initialization.
 *                  This function must be called, once, before any other Audio Record functions.
 *
 * @param[in]       p_callback: Pointer to application Audio Record callback function
 *
 * @return          Result code (see wiced_result_t)
 *
 */
wiced_result_t wiced_bt_audio_record_init(wiced_bt_audio_record_callback_t *p_callback);

/**@} audio_record_api_functions */

/**
 *
 * Function         wiced_bt_audio_record_enable
 *
 *                  This function is called to Enable/disable Audio Record.
 *
 * @param[in]       enable: Enable (1) or Disable (0) Audio Recordion
 * @param[in/out]   sample_rate: IN: Sample Rate proposed by the application
 *                               OUT: Sample Rate to use (decided by HW/HW)
 *
 * @return          Result code (see wiced_result_t)
 *
 */
wiced_result_t wiced_bt_audio_record_enable(uint8_t enable, uint32_t *p_sample_rate);

#ifdef __cplusplus
}
#endif

/**@} audio_record */
