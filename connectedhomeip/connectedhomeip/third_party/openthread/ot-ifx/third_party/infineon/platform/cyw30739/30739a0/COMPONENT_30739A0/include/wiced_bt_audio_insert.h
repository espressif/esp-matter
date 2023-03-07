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
 *  @addtogroup  audio_insert   Audio Insertion
 *  @ingroup     wicedbt_a2dp
 *
 * Bluetooth Audio Insert Application Programming Interface
 *
 * The Audio Insertion library provides API to allow the Wiced application to Play/Insert sound.
 * If A2DP Stream is already active, the application can change the PCM samples going to the I2S
 * interface. The application receives the PCM samples which 'should' be played. This allow to
 * either Replace or Mix (e.g. add) the Inserted Audio.
 * If A2DP Stream is not active, the I2S interface will be automatically started.
 * Note that this API cannot be used if a SCO (e.g. HFP Call) is already ongoing. In this case
 * the Audio must be inserted using the SCO Hook library.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include <stdint.h>
#include "wiced.h"
#include "wiced_bt_sco_hook.h"

/**
 * @brief Number of Max Required PCM Samples for Audio Insertion in Audio Stream
 *
 * The user application shall provide number of samples * 2 * 2 bytes(each sample is 16-bit and stereo)
 * The number of samples is based on I2S_AUD_INJECT_EVT_AUDIO_INFO information reported by i2s aud inject.
 * The max value will be WICED_BT_AUDIO_INSERT_PCM_SAMPLE_NB_AUDIO samples(1024 * 2 * 2 bytes for AAC case)
 */
#define WICED_BT_AUDIO_INSERT_PCM_SAMPLE_NB_AUDIO   1024

/**
 * @brief Number of PCM Samples for Audio Insertion in Voice Stream
 *
 * The controller will ask the application to provide 120 samples data (120 * 16 bits) in every
 * WICED_BT_SCO_HOOK_EVT_SPK_SAMPLES event.
 */
#define WICED_BT_AUDIO_INSERT_PCM_SAMPLE_NB_SCO     120

/**
 * @brief   Audio Insert Type
 *
 * Type of audio insertion
 */
typedef enum
{
    WICED_BT_AUDIO_INSERT_TYPE_SCO_MIC, /* Insertion data will be added in the Microphone data. */
    WICED_BT_AUDIO_INSERT_TYPE_SCO_SPK, /* Insertion data will be added in the Voice Call Speaker data. */
    WICED_BT_AUDIO_INSERT_TYPE_AUDIO,   /* Insertion data will be added in the audio data (streaming or playback). */
} wiced_bt_audio_insert_type_t;

/**
 * @brief Audio Insert Events.
 *
 * Audio Insert events received by the Audio Insert callback (see wiced_bt_audio_insert_callback_t)
 */
typedef enum
{
    WICED_BT_AUDIO_INSERT_EVT_DATA_REQ = 3, /* Request to fill hardware FIFO with audio data */
    WICED_BT_AUDIO_INSERT_EVT_AUDIO_INFO,   /* Indicates the sample rate for the pending audio injection */
    WICED_BT_AUDIO_INSERT_EVT_SCO,          /* indicates start SCO audio injection */
} wiced_bt_audio_insert_event_t;

/**
 * Callback when the insertion data is exhausted.
 *
 * @param[in] type
 */
typedef void (wiced_bt_audio_insert_source_data_exhausted_callback_t)(wiced_bt_audio_insert_type_t type);

typedef void (wiced_bt_audio_insert_source_data_pre_handler_t)(wiced_bt_sco_hook_event_data_t *p_data);

/* Utility for advanced audio insert (audio insert over multiple devices) control. */
typedef void (wiced_bt_audio_insert_advanced_control_enable_t)(void);
typedef void (wiced_bt_audio_insert_advanced_control_disable_t)(void);

typedef struct wiced_bt_audio_insert_advanced_control_config_sco
{
    wiced_bt_audio_insert_advanced_control_enable_t     *p_enable;
    wiced_bt_audio_insert_advanced_control_disable_t    *p_disable;
} wiced_bt_audio_insert_advanced_control_config_sco_t;

typedef struct wiced_bt_audio_insert_advanced_control_config_audio
{
    wiced_bt_audio_insert_advanced_control_enable_t     *p_enable;
    wiced_bt_audio_insert_advanced_control_disable_t    *p_disable;
} wiced_bt_audio_insert_advanced_control_config_audio_t;

typedef struct wiced_bt_audio_insert_advanced_control_config
{
    wiced_bt_audio_insert_advanced_control_config_sco_t     sco;
    wiced_bt_audio_insert_advanced_control_config_audio_t   audio;
} wiced_bt_audio_insert_advanced_control_config_t;

typedef struct wiced_bt_audio_insert_data_sco
{
    int16_t        *p_source;   /* pointer to the insertion data */
    uint32_t        len;        /* length of insertion data in bytes */
    wiced_bool_t    overwrite;  /* TRUE: Overwrite existent data
                                   FALSE: Amend with insertion data. */
    wiced_bool_t    loop;       /* TRUE: loop mode
                                   FALSE: The user application shall call
                                          wiced_bt_audio_insert_start() utility again to keep the
                                          audio insertion when the data exhausted callback is
                                          called.*/
    uint8_t         volume_reduce_factor_insert;    /* Must be bigger than 0.
                                                       The insertion data will be divided by this
                                                       factor and substitutes/amends the
                                                       original data */
    uint8_t         volume_reduce_factor_original;  /* Must be bigger than 0.
                                                       The original data will be divided by this
                                                       factor first and the insertion data will be
                                                       amended if overwrite filed value is set to
                                                       0 */

    wiced_bt_audio_insert_source_data_exhausted_callback_t  *p_source_data_exhausted_callback;
    wiced_bool_t    stop_insertion_when_source_exhausted;   /* TRUE: stop audio insertion if the
                                                                     source data is exhausted. */
    wiced_bool_t    insert_data_after_target_seq_num;   /* Data will be inserted after the target
                                                           SCO data time sequence number defined in
                                                           expected_sco_time_seq_num. */
    uint32_t        expected_sco_time_seq_num;  /* Valid only when the field,
                                                   insert_data_after_target_seq_num is set. */
    wiced_bt_audio_insert_source_data_pre_handler_t  *p_source_data_pre_handler;    /* Data pre handler
                                                                                       for MIC or SPK */
} wiced_bt_audio_insert_data_sco_t;

typedef struct wiced_bt_audio_insert_data_audio
{
    int16_t        *p_source;   /* pointer to the insertion data */
    uint32_t        len;        /* length of insertion data in bytes */
    wiced_bool_t    overwrite;  /* TRUE: Overwrite existent data
                                   FALSE: Amend with insertion data. */
    wiced_bool_t    loop;       /* TRUE: loop mode
                                   FALSE: The user application shall call
                                          wiced_bt_audio_insert_start() utility again to keep the
                                          audio insertion when the data exhausted callback is
                                          called.*/
    uint8_t         volume_reduce_factor_insert;    /* Must be bigger than 0.
                                                       The insertion data will be divided by this
                                                       factor and substitutes/amends the
                                                       original data */
    uint8_t         volume_reduce_factor_original;  /* Must be bigger than 0.
                                                       The original data will be divided by this
                                                       factor first and the insertion data will be
                                                       amended if overwrite filed value is set to
                                                       0 */

    wiced_bt_audio_insert_source_data_exhausted_callback_t  *p_source_data_exhausted_callback;
    wiced_bool_t    stop_insertion_when_source_exhausted;   /* TRUE: stop audio insertion if the
                                                                     source data is exhausted. */
    wiced_bool_t    multiple;   /* TRUE if the audio insertion is for multiple devices. */
} wiced_bt_audio_insert_data_audio_t;

typedef struct wiced_bt_audio_insert_data
{
    union
    {
        wiced_bt_audio_insert_data_sco_t    sco;
        wiced_bt_audio_insert_data_audio_t  audio;
    };
} wiced_bt_audio_insert_data_t;

/**
 * @brief Configuration data used to enable/start audio insertion.
 */
typedef struct wiced_bt_audio_insert_config
{
    wiced_bt_audio_insert_type_t    type;
    uint32_t                        *p_sample_rate;
    wiced_bt_audio_insert_data_t    insert_data;
} wiced_bt_audio_insert_config_t;

/**
 * @brief Data associated with WICED_BT_AUDIO_INSERT_EVT_DATA_REQ.
 *
 * This event is received when FW requests audio data
 *
 */
typedef struct
{
    int16_t *p_data_in;                     /* Decoded data from A2DP stream */
    int16_t *p_data_out;                    /* Pointer to output buffer */
    uint16_t bufferSize;                    /* size of the buffer that needs to be filled */
} wiced_bt_audio_insert_event_data_req_t;

/**
 * @brief Data associated with LITE_HOST_LRAC_EVT_AUDIO_INFO.
 *
 * This event is received when the FW indicates the Sample Rate and the Buffer size to use
 * for the Audio Insertion
 *
 */
typedef struct
{
    uint16_t sample_rate;                      /* Sample rate for pending audio injection */
    uint16_t nb_samples;                       /* Number of PCM samples (16 bits, Interleaved Stereo) */
} wiced_bt_audio_insert_event_audio_info_t;

/**
 * @brief Union of data associated with NREC events
 *
 */
typedef union
{
    wiced_bt_audio_insert_event_data_req_t      data_req;   /* WICED_BT_AUDIO_INSERT_EVT_DATA_REQ */
    wiced_bt_audio_insert_event_audio_info_t    audio_info; /* WICED_BT_AUDIO_INSERT_EVT_AUDIO_INFO */
    wiced_bt_audio_insert_event_audio_info_t    sco_info;   /* WICED_BT_AUDIO_INSERT_EVT_SCO */
} wiced_bt_audio_insert_event_data_t;

/**
 * Audio Insert Callback function type wiced_bt_audio_insert_callback_t
 *
 *                  Audio Insert Event callback (registered with wiced_bt_audio_insert_init)
 *
 * @param[in]       event: Audio Insert event received
 * @param[in]       p_data : Data (pointer on union of structure) associated with the event
 *
 * @return NONE
 */
typedef void (wiced_bt_audio_insert_callback_t)(wiced_bt_audio_insert_event_t event,
        wiced_bt_audio_insert_event_data_t *p_data);

/**
*  @addtogroup  audio_insert_api_functions       API Functions
*  @ingroup     audio_insert
*
*  <b> API Functions </b> module for @b Audio Insert.
*
*  @{
*/

/**
 * wiced_bt_audio_insert_init
 *
 * Initialize the Bluetooth Audio Insertion Module.
 */
void wiced_bt_audio_insert_init(void);

/**
 * wiced_bt_audio_insert_start
 *
 * Start the audio insertion
 *
 * @param p_config - refer to wiced_bt_audio_insert_config_t
 *
 */
void wiced_bt_audio_insert_start(wiced_bt_audio_insert_config_t *p_config);

/**
 * wiced_bt_audio_insert_stop
 *
 * Stop the audio insertion
 *
 * @param type - refer to wiced_bt_audio_insert_type_t
 */
void wiced_bt_audio_insert_stop(wiced_bt_audio_insert_type_t type);

/**
 * wiced_bt_audio_insert_sco_in_data_latest_time_sequence_number_get
 *
 * Acquire the latest time sequence number of incoming SCO data.
 *
 * @return lastest sco in data time sequence number
 */
uint32_t wiced_bt_audio_insert_sco_in_data_latest_time_sequence_number_get(void);

/**
 * wiced_bt_audio_insert_advanced_control_utility_install
 *
 * Utilities used to install the advanced audio insert (multiple device audio insertion) utilities.
 *
 * @param p_config - configuration
 */
void wiced_bt_audio_insert_advanced_control_utility_install(wiced_bt_audio_insert_advanced_control_config_t *p_config);

#ifdef __cplusplus
}
#endif

/**@} audio_insert */
