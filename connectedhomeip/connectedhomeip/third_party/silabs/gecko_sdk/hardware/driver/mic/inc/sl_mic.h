/***************************************************************************//**
 * @file
 * @brief Sound level microphone driver
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#ifndef SL_MIC_H
#define SL_MIC_H

#include <stdint.h>
#include <stdbool.h>
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup mic Microphone
 * @brief Sound level driver for PDM and I2S microphones.
 * @details
   @n @section mic_example Microphone example code

    Basic example for looping measurement of sound level: @n @n
    @verbatim

 #include "sl_mic.h"

 #define MIC_SAMPLE_RATE            44100
 #define MIC_SAMPLE_BUFFER_SIZE     1024
 #define MIC_N_CHANNELS             2

   static int16_t buffer[MIC_SAMPLE_BUFFER_SIZE * MIC_N_CHANNELS];

   int main( void )
   {

    ...
    float sound_level_0;
    float sound_level_1;
    uint32_t n_samples = 1024;

    // Initialize microphone with sample rate and number of channels
    sl_mic_init(MIC_SAMPLE_RATE, MIC_N_CHANNELS);

    while(true){

      // Read samples from the microphone
      sl_mic_get_n_samples(buffer, n_samples);

      while (!sl_mic_sample_buffer_ready()) {
        // Wait until sample buffer ready
      }

      // Calculate sound level
      sl_mic_calculate_sound_level(&sound_level_0, buffer, n_samples, 0);
      sl_mic_calculate_sound_level(&sound_level_1, buffer, n_samples, 1);

    }

    ...

   } @endverbatim
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *    Callback function indicating that the sample buffer is ready.
 *
 * @param[in] buffer
 *    Pointer to the sample buffer.
 *
 * @param[in] n_frames
 *    Number of audio frames in the sample buffer.
 *
 * @return
 *    None.
 ******************************************************************************/
typedef void (*sl_mic_buffer_ready_callback_t)(const void *buffer, uint32_t n_frames);

/***************************************************************************//**
 * @brief
 *    Initialize the microphone.
 *
 * @param[in] sample_rate
 *    The desired sample rate in Hz
 *
 * @param[in] channels
 *    Number of audio channels (1 or 2)
 *
 * @return
 *    Returns SL_STATUS_OK on success, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_mic_init(uint32_t sample_rate, uint8_t channels);

/***************************************************************************//**
 * @brief
 *    De-initialize the microphone.
 *
 * @retval SL_STATUS_OK
 ******************************************************************************/
sl_status_t sl_mic_deinit(void);

/***************************************************************************//**
 * @brief
 *    Read samples from the microphone into a sample buffer.
 *
 * @details
 *    This function starts the microphone sampling and stops the sampling after
 *    reading the desired number of samples. Call @ref sl_mic_sample_buffer_ready
 *    to check when the samples are ready in the buffer.
 *
 * @param[in] buffer
 *    Pointer to the sample buffer to store the data.
 *    16-bit channel data is stored consecutively, starting with ch0
 *
 * @param[in] n_frames
 *    The number of the audio frames to get
 *
 * @retval SL_STATUS_OK Success.
 * @retval SL_STATUS_NOT_INITIALIZED Not initialized.
 * @retval SL_STATUS_INVALID_STATE Sampling is already in progress.
 ******************************************************************************/
sl_status_t sl_mic_get_n_samples(void *buffer, uint32_t n_frames);

/***************************************************************************//**
 * @brief
 *    Read samples from the microphone into a sample buffer continuously.
 *
 * @details
 *    This function starts the microphone sampling and stops only upon calling
 *    @ref sl_mic_stop or @ref sl_mic_deinit. The buffer is used in a "ping-pong"
 *    manner meaning that one half of the buffer is used for sampling while the
 *    other half is being processed.
 *
 * @param[in] buffer
 *    Pointer to the sample buffer to store the data.
 *    16-bit channel data is stored consecutively, starting with ch0.
 *    This buffer shall be big enough to hold twice the n_frames because of the
 *    ping-pong operation.
 *
 * @param[in] n_frames
 *    The number of audio frames to receive before the callback is called.
 *    Maximum value limited by DMADRV_MAX_XFER_COUNT.
 *
 * @param[in] callback
 *    Callback is called when n_frames in the sample buffer is ready.
 *
 * @retval SL_STATUS_OK Success.
 * @retval SL_STATUS_NOT_INITIALIZED Not initialized.
 * @retval SL_STATUS_INVALID_STATE Sampling is already in progress.
 * @retval SL_STATUS_INVALID_PARAMETER n_frames too large.
 ******************************************************************************/
sl_status_t sl_mic_start_streaming(void *buffer, uint32_t n_frames, sl_mic_buffer_ready_callback_t callback);

/***************************************************************************//**
 * @brief
 *    Start the microphone.
 *
 * @retval SL_STATUS_OK Success.
 * @retval SL_STATUS_NOT_INITIALIZED Not initialized.
 * @retval SL_STATUS_INVALID_STATE Microphone is already running.
 ******************************************************************************/
sl_status_t sl_mic_start(void);

/***************************************************************************//**
 * @brief
 *    Stop the microphone.
 *
 * @retval SL_STATUS_OK Success.
 * @retval SL_STATUS_INVALID_STATE Microphone is not running.
 ******************************************************************************/
sl_status_t sl_mic_stop(void);

/***************************************************************************//**
 * @brief
 *    Check whether the sample buffer is ready.
 *
 * @retval true All samples requested from microphone are ready.
 * @retval false Samples are still being read into the buffer.
 ******************************************************************************/
bool sl_mic_sample_buffer_ready(void);

/***************************************************************************//**
 * @brief
 *    Calculate the dBSPL value for a channel from a sample buffer.
 *
 * @param[out] sound_level
 *    The calculated sound level
 *
 * @param[in] buffer
 *    Buffer to calculate sound level from. Must contain 16-bit samples, starting
 *    with channel 0
 *
 * @param[in] n_frames
 *    Number of audio frames to use when calculating sound level
 *
 * @param[in] channel
 *    The channel to get the sound level for
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_INVALID_PARAMETER Invalid channel num
 ******************************************************************************/
sl_status_t sl_mic_calculate_sound_level(float *sound_level, const int16_t *buffer, uint32_t n_frames, uint8_t channel);

/** @} */

#ifdef __cplusplus
}
#endif

#endif //SL_MIC_H
