/***************************************************************************//**
 * @file mic_i2s.h
 * @brief Driver for stereo I2S microphones
 * @version 5.4.0
 *******************************************************************************
 * # License
 * <b>Copyright 2017 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef MIC_PDM_H
#define MIC_PDM_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "sl_status.h"
#include "mic_config.h"

/***************************************************************************//**
 * @addtogroup Mic
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @defgroup Mic_Functions MEMS Microphone Functions
 * @{
 * @brief MEMS microphone driver and support functions
 ******************************************************************************/

// -----------------------------------------------------------------------------
// typedef

// -----------------------------------------------------------------------------
// Global Function Prototypes
/***************************************************************************//**
 * @brief
 *    Initializes the microphone
 *
 * @param[in] channels
 *    Number of audio channels (1 or 2)
 *
 * @param[in] sample_rate
 *    The desired sample rate in Hz
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_mic_init(uint32_t sample_rate, uint8_t channels);

/***************************************************************************//**
 * @brief
 *    De-initialize the microphone
 *
 * @retval SL_STATUS_OK
 ******************************************************************************/
sl_status_t sl_mic_deinit(void);

/***************************************************************************//**
 * @brief
 *    Read samples from the microphone into a sample buffer
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
 * @retval SL_STATUS_OK
 ******************************************************************************/
sl_status_t sl_mic_get_n_samples(void *buffer, uint32_t n_frames);

/***************************************************************************//**
 * @brief
 *    Starts the microphone
 *
 * @retval SL_STATUS_OK
 ******************************************************************************/
sl_status_t sl_mic_start(void);

/***************************************************************************//**
 * @brief
 *    Stops the microphone
 *
 * @retval SL_STATUS_OK
 ******************************************************************************/
sl_status_t sl_mic_stop(void);

/***************************************************************************//**
 * @brief
 *    Checks if the sample buffer is ready
 *
 * @return
 *    Returns true if all samples requested from microphone are ready
 ******************************************************************************/
bool sl_mic_sample_buffer_ready(void);

/***************************************************************************//**
 * @brief
 *    Calculates the dBSPL value for a channel from a sample buffer
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
 * @retval SL_STATUS_BUSY Sample buffer is currently filling with samples
 ******************************************************************************/
sl_status_t sl_mic_calculate_sound_level(float *sound_level, const int16_t *buffer, uint32_t n_frames, uint8_t channel);

/** @} {end defgroup Mic_Functions}*/

/** @} {end addtogroup Mic} */

#endif // MIC_PDM_H
