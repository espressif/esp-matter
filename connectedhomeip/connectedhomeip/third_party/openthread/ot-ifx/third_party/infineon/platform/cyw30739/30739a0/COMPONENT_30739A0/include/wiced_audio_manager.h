/**
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
 * Audio manager is a high level interface for applications to use platform audio capabilities.
 * It supports different streams like A2DP_PLAYBACK, HFP etc.
 * Audio manager also has interface for specific stream Start, Stop and also stream specific volume control.
 */

#ifndef __WICED_AUDIO_MANAGER_API_H
#define __WICED_AUDIO_MANAGER_API_H

#include <stdint.h>
#include <wiced_result.h>
#include <wiced_bt_a2dp_defs.h>
#include <platform_audio_device.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#define DEFAULT_VOLUME 80
#define DEFAULT_BITSPSAM 16
#define DEFAULT_PLAYBACK_SR 44100
#define DEFAULT_RECORD_SR 8000
#define DEFAULT_CH 2
#define AM_VOL_LEVEL_HIGH 10
#define AM_VOL_LEVEL_LOW 0
#define AM_PLAYBACK_SR_8K 8000
#define AM_PLAYBACK_SR_11K 11025
#define AM_PLAYBACK_SR_12K 12000
#define AM_PLAYBACK_SR_16K 16000
#define AM_PLAYBACK_SR_22K 22050
#define AM_PLAYBACK_SR_32K 32000
#define AM_PLAYBACK_SR_44K 44100
#define AM_PLAYBACK_SR_48K 48000
#define AM_PLAYBACK_SR_64K 64000
#define AM_PLAYBACK_SR_88K 88200
#define AM_PLAYBACK_SR_96K 96000

#define WICED_AUDIO_MANAGER_STREAM_ID_INVALID   -1

/******************************************************
 *                   Enumerations
 ******************************************************/
/**
 * @brief Audio Streams.
 *
 */
typedef enum
{
    CAPTURE,                                /**< Audio Capture stream. */
    RENDER,                                 /**< Audio Render stream. */
    FILE_PLAYBACK,                          /**< File Play back Stream. */
    A2DP_PLAYBACK,                          /**< A2DP Play back Stream. */
    HFP,                                    /**< Hands free profile stream */
    MAX_NO_OF_STREAMS,                      /**< Used to accumulate total stream types. */
    STREAM_TYPE_INVALD = MAX_NO_OF_STREAMS, /** < Invalid stream type */
} stream_type_t;

/**
 * @brief Audio Parameter types.
 *
 */
typedef enum
{
    AM_CODEC_CONFIG,       /**< Codec Configuration.    */
    AM_AUDIO_CONFIG,       /**< Audio Configuration.    */
    AM_SAMPLE_RATE,        /**< Sample rate.            */
    AM_CHANNELS,           /**< No of channels.         */
    AM_BITS_PER_SAMPLE,    /**< No of bits per sample.  */
    AM_MIC_GAIN_LEVEL,     /**< Mic gain level.         */
    AM_SPEAKER_VOL_LEVEL,  /**< volume level            */
    AM_NREC_ENABLE,        /**< NREC enable             */
    AM_NREC_DISABLE,       /**< NREC disable            */
    AM_GAIN_CONFIG,        /**< Gain Configuration      */
    AM_NREC_CONFIG,        /**< NREC Configuration      */
    AM_IO_DEVICE,          /**< Input/Output device     */
} param_type_t;

/**
 * @brief audio output type
 */
typedef enum
{
    AM_SPEAKERS,
    AM_HEADPHONES,
    AM_UART,
} am_audio_io_device_t;

/******************************************************
 *                    Structures
 ******************************************************/
/**
 * @brief Codec configuration
 *
 */
typedef struct
{
    uint32_t  codec_type;     /**<  Type of Codec  */
} codec_config_t;

/**
 * @brief Audio configuration
 *
 */
typedef struct
{
    int32_t  sr;                /**<  Sampling rate  */
    int32_t  channels;          /**<  no of channels  */
    int32_t  bits_per_sample;   /**<  no of bits per sample  */
    int32_t  volume;            /**<  volume level  */
    int32_t  mic_gain;          /**<  mic volume gain  */
    am_audio_io_device_t sink;  /**<  audio output device  */
} audio_config_t;


/**
 *  @brief NREC configuration
 */
typedef struct
{
    uint32_t  sample_rate;     /**<  Place holder for NREC configuration parameters  */
} nrec_config_t;

/**
 *  @brief Stream Interface Functions
 */
typedef struct
{
    wiced_result_t (*open)          ( );                                    /**< Stream Open                */
    wiced_result_t (*close)         ( void*);                               /**< Stream Close               */
    wiced_result_t (*config)        (audio_config_t*);                      /**< Configure the stream       */
    wiced_result_t (*start)         (void*);                                /**< Start The stream           */
    wiced_result_t (*stop)          (void*);                                /**< Stop the stream            */
    wiced_result_t (*get_buffer)    (uint8_t** pp_buf, int32_t* size);      /**< Get buffer for the stream  */
    wiced_result_t (*put_buffer)    (uint8_t* p_buf, int32_t* size);        /**< Put buffer for the stream  */
} stream_interface_t;

/**
 *  @brief Capture stream interface
 */
typedef struct
{
    stream_interface_t interface;                                               /**< common Interface structure */
    wiced_result_t (*get_ref_buffer)    (uint8_t** pp_buf, int32_t* size);      /**< Get reference buffer       */
    wiced_result_t (*put_ref_buffer)    (uint8_t* p_buf, int32_t* size);        /**< put reference buffer       */

} capture_stream_interface_t;


/**
 *  @brief Render stream interface
 */
typedef struct
{
    stream_interface_t interface;

} render_stream_interface_t;


/*****************************************************************************
 *          Function Prototypes
 *****************************************************************************/

/**
 * The application should call this function to Initialize the Audio Manager
 *
 * @param           NO parameters required.
 * @return          none
 */
void wiced_am_init(void);
/**
 * The application should call this function to open the stream
 * This will construct the graph of various components in the chain based on the stream type.
 *
 * @param           stream_type  : Type of the stream to register (stream_type_t)
 * @param           args         : Arguments to be passed if any.
 * @return          stream id
 *                  invalid if WICED_AUDIO_MANAGER_STREAM_ID_INVALID
 */
int32_t wiced_am_stream_open(uint32_t stream_type);

/**
 * The application should call this function to start the stream
 *
 * @param           stream_id  : Id of the stream.
 * @return          WICED_NOT_FOUND
 *                  WICED_SUCCESS
 */
wiced_result_t wiced_am_stream_start(int32_t stream_id);

/**
 * The application should call this function to stop the stream
 *
 * @param           stream_id  : Id of the stream.
 * @return          WICED_NOT_FOUND
 *                  WICED_SUCCESS
 */
wiced_result_t wiced_am_stream_stop(int32_t stream_id);

/**
 * The application should call this function to close the stream
 *
 * @param           stream_id  : Id of the stream.
 * @return          WICED_NOT_FOUND
 *                  WICED_SUCCESS
 */
wiced_result_t wiced_am_stream_close(int32_t stream_id);

/**
 * The application should call this function to set the parameters of the stream
 *
 * @param           stream_id  : Id of the stream.
 * @param           param_type : Type of the parameters to be set.
 * @param           param_config: Parameter configurations
 * @return          WICED_NOT_FOUND
 *                  WICED_ERROR
 *                  WICED_SUCCESS
 */
wiced_result_t wiced_am_stream_set_param(int32_t stream_id, uint32_t param_type, void *param_config);

/**
 * The application should call this function to get parameters of the stream
 *
 * @param           stream_id  : Id of the stream.
 * @param           param_type : Type of the parameters to be set.
 * @param           param_config: Parameter configurations
 * @return          result
 */
int32_t wiced_am_stream_get_param(uint32_t stream_id, uint32_t param_type, void* param_config);

/**
 * The application should call this function to get interface of the stream
 *
 * @param           stream_id  : Id of the stream.
 * @param           inf_type   : Interface type.
 * @return          result
 */
stream_interface_t* wiced_am_stream_get_intf(uint32_t stream_id, uint32_t inf_type);

/** @} wiced_bt_am_api_functions */

#ifdef __cplusplus
}
#endif

#endif
