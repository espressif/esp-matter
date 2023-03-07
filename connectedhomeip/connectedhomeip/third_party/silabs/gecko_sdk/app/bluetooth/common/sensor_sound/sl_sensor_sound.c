/***************************************************************************//**
 * @file
 * @brief Sound level sensor
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdbool.h>
#include "sl_board_control.h"
#include "sl_mic.h"
#include "sl_sleeptimer.h"
#include "app_assert.h"
#include "sl_sensor_sound.h"

// -----------------------------------------------------------------------------
// Private macros

#define MIC_SAMPLE_RATE            44100
#define MIC_SAMPLE_BUFFER_SIZE     1000
#define MIC_N_CHANNELS             1
#define MIC_IIR_WEIGHT             0.1f

// -----------------------------------------------------------------------------
// Private variables

static bool initialized = false;
static float sound_level = 0;
static int16_t buffer[MIC_SAMPLE_BUFFER_SIZE * MIC_N_CHANNELS];

// -----------------------------------------------------------------------------
// Public function definitions

sl_status_t sl_sensor_sound_init(void)
{
  sl_status_t sc;
  // Enable microphone and wait 50 ms power up time
  sl_board_enable_sensor(SL_BOARD_SENSOR_MICROPHONE);
  sl_sleeptimer_delay_millisecond(50);
  sc = sl_mic_init(MIC_SAMPLE_RATE, MIC_N_CHANNELS);
  if (SL_STATUS_OK == sc) {
    // Start sampling
    sl_mic_get_n_samples(buffer, MIC_SAMPLE_BUFFER_SIZE);
    initialized = true;
  } else {
    initialized = false;
  }
  return sc;
}

void sl_sensor_sound_deinit(void)
{
  initialized = false;
  sl_mic_deinit();
  sl_board_disable_sensor(SL_BOARD_SENSOR_MICROPHONE);
}

sl_status_t sl_sensor_sound_get(float *sl)
{
  *sl = sound_level;
  if (initialized) {
    return SL_STATUS_OK;
  } else {
    return SL_STATUS_NOT_INITIALIZED;
  }
}

void sl_sensor_sound_step(void)
{
  float sl;
  if (initialized) {
    if (sl_mic_sample_buffer_ready()) {
      if (SL_STATUS_OK == sl_mic_calculate_sound_level(&sl, buffer, MIC_SAMPLE_BUFFER_SIZE, 0)) {
        // Apply IIR filter on the measured value
        sound_level = (sl * MIC_IIR_WEIGHT) + (sound_level * (1 - MIC_IIR_WEIGHT));
      }
      sl_mic_get_n_samples(buffer, MIC_SAMPLE_BUFFER_SIZE);
    }
  }
}
