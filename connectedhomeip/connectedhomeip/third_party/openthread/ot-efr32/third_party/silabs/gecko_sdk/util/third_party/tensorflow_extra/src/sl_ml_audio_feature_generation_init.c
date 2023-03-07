/***************************************************************************//**
 * @file
 * @brief Silicon Labs Feature Generation Initialization with audio source
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_mic.h"
#include "sl_ml_audio_feature_generation_config.h"
#include "sl_ml_audio_feature_generation.h"

#define MIC_DMA_SIZE 512

static int16_t mic_sample_buffer[2 * MIC_DMA_SIZE];

/***************************************************************************//**
 *  Retrieves the features as type uint16 and copies them to the provided buffer.
 ******************************************************************************/
static void mic_buffer_ready_callback(const int16_t *buffer, uint32_t n_frames)
{
  sli_ml_audio_feature_generation_audio_buffer_write_chunk(buffer, n_frames);
}

/***************************************************************************//**
 *  Retrieves the features as type uint16 and copies them to the provided buffer.
 ******************************************************************************/
sl_status_t sl_ml_audio_feature_generation_init()
{
  // Initialize microphone as a 1-channel streaming microphone
  sl_status_t mic_status;

  mic_status = sl_mic_init(SL_ML_FRONTEND_SAMPLE_RATE_HZ, 1);
  if (mic_status != SL_STATUS_OK) {
    return mic_status;
  }

  mic_status = sl_mic_start_streaming(mic_sample_buffer, MIC_DMA_SIZE, (sl_mic_buffer_ready_callback_t)mic_buffer_ready_callback);

  if (mic_status != SL_STATUS_OK) {
    return mic_status;
  }

  sl_ml_audio_feature_generation_frontend_init();

  return SL_STATUS_OK;
}
