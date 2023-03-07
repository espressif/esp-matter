/***************************************************************************//**
 * @file
 * @brief Voice transmission
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
// Define module name for Power Manager debuging feature.
#define CURRENT_MODULE_NAME    "APP_BLUETOOTH_EXAMPLE_SOC_VOICE"

#include <string.h>
#include "sl_power_manager.h"
#include "sl_board_control.h"
#include "app_assert.h"
#include "circular_buff.h"
#include "filter.h"
#include "adpcm.h"
#include "sl_mic.h"
#include "voice.h"

// -----------------------------------------------------------------------------
// Private macros

#define VOICE_SAMPLE_RATE_DEFAULT sr_16k
#define VOICE_CHANNELS_DEFAULT    1
#define VOICE_FILTER_DEFAULT      true
#define VOICE_ENCODE_DEFAULT      true

#define MIC_CHANNELS_MAX        2
#define MIC_SAMPLE_SIZE         2
#define MIC_SAMPLE_BUFFER_SIZE  112
#define MIC_SEND_BUFFER_SIZE    (MIC_SAMPLE_BUFFER_SIZE * MIC_SAMPLE_SIZE)
#define ADPCM_COMPRESS_FACTOR   2         // 2 * 16 bit -> 2 * 4 bit = 1 * 8 bit
#define ADPCM_PAYLOAD_SIZE      (MIC_SAMPLE_BUFFER_SIZE / ADPCM_COMPRESS_FACTOR)
#define CIRCULAR_BUFFER_SIZE    (MIC_SAMPLE_BUFFER_SIZE * 10)

#define SR2FS(sr)               ((sr) * 1000)

// -----------------------------------------------------------------------------
// Private type definitions

typedef struct {
  sample_rate_t sampleRate;
  uint8_t channels;
  bool filter_enabled;
  bool encoding_enabled;
} voice_config_t;

// -----------------------------------------------------------------------------
// Private variables

static biquad_t biquads[MIC_CHANNELS_MAX];
static filter_context_t filter = { 0, biquads };
static bool voice_running = false;
static int16_t mic_buffer[2 * MIC_SAMPLE_BUFFER_SIZE];
static voice_config_t voice_config;
static circular_buffer_t circular_buffer;
static const int16_t *sample_buffer;
static uint32_t frames;
static bool event_process = false;
static bool event_send = false;
static adpcm_t adpcms[MIC_CHANNELS_MAX];
static adpcm_context_t adpcm = { 0, adpcms };
static uint8_t adpcm_payload[ADPCM_PAYLOAD_SIZE];

// -----------------------------------------------------------------------------
// Private function declarations

/***************************************************************************//**
 * Process data coming from microphone.
 *
 * Depending on the configuration settings data are filtered, encoded and added
 * to a circular buffer.
 ******************************************************************************/
static void voice_process_data(void);

/***************************************************************************//**
 * Send audio data from circular buffer.
 *
 * Data are sent in packages of MIC_SEND_BUFFER_SIZE size.
 * If there is less then MIC_SEND_BUFFER_SIZE in circular buffer data will be
 * sent after next DMA readout.
 ******************************************************************************/
static void voice_send_data(void);

/***************************************************************************//**
 * DMA callback indicating that the buffer is ready.
 *
 * @param buffer Microphone buffer to be processed.
 ******************************************************************************/
static void mic_buffer_ready(const void *buffer, uint32_t n_frames);

// -----------------------------------------------------------------------------
// Public function definitions

/***************************************************************************//**
 * Initialize internal variables.
 ******************************************************************************/
void voice_init(void)
{
  cb_err_code_t err;
  voice_config.sampleRate = VOICE_SAMPLE_RATE_DEFAULT;
  voice_config.channels = VOICE_CHANNELS_DEFAULT;
  voice_config.filter_enabled = VOICE_FILTER_DEFAULT;
  voice_config.encoding_enabled = VOICE_ENCODE_DEFAULT;
  err = cb_init(&circular_buffer, CIRCULAR_BUFFER_SIZE, sizeof(uint8_t));
  app_assert(err == cb_err_ok,
             "[E: 0x%04x] Circular buffer init failed\n",
             (int)err);
}

/***************************************************************************//**
 * Start voice transmission.
 ******************************************************************************/
void voice_start(void)
{
  sl_status_t sc;
  // Check if transfer is running
  if (voice_running) {
    return;
  }
  // Power up microphone
  sc = sl_board_enable_sensor(SL_BOARD_SENSOR_MICROPHONE);
  if ( sc != SL_STATUS_OK ) {
    return;
  }
  // Microphone initialization
  sc = sl_mic_init(SR2FS(voice_config.sampleRate), voice_config.channels);
  if ( sc != SL_STATUS_OK ) {
    return;
  }
  // Start microphone sampling
  sc = sl_mic_start_streaming(mic_buffer, MIC_SAMPLE_BUFFER_SIZE / voice_config.channels, mic_buffer_ready);
  if ( sc != SL_STATUS_OK ) {
    return;
  }
  // ADPCM encoder initialization
  if (voice_config.encoding_enabled) {
    adpcm.ch_count = voice_config.channels;
    ADPCM_init(&adpcm);
  }
  // Filter initialization
  if (voice_config.filter_enabled) {
    filter_parameters_t fp = DEFAULT_FILTER;
    fp.srate = SR2FS(voice_config.sampleRate);
    filter.ch_count = voice_config.channels;
    fil_init(&filter, &fp);
  }
  // Limit sleep level to EM1
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);

  // Audio transfer started
  voice_running = true;
}

/***************************************************************************//**
 * Stop voice transmission.
 ******************************************************************************/
void voice_stop(void)
{
  // Check if transfer is running
  if (!voice_running) {
    return;
  }

  // Microphone deinitialization
  sl_mic_deinit();

  // Power down microphone
  sl_board_disable_sensor(SL_BOARD_SENSOR_MICROPHONE);

  // Remove energy mode requirement
  sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);

  // Audio transfer stopped
  voice_running = false;
}

/***************************************************************************//**
 * Transmit voice buffer.
 ******************************************************************************/
SL_WEAK void voice_transmit(uint8_t *buffer, uint32_t size)
{
  (void)buffer;
  (void)size;
  // Dummy weak implementation
}

/***************************************************************************//**
 * Voice event handler.
 ******************************************************************************/
void voice_process_action(void)
{
  if (event_process) {
    event_process = false;
    voice_process_data();
  }
  if (event_send) {
    event_send = false;
    voice_send_data();
  }
}

/***************************************************************************//**
 * Setter for configuration setting sample rate.
 ******************************************************************************/
void voice_set_sample_rate(sample_rate_t sample_rate)
{
  if ((sample_rate == sr_16k) || (sample_rate == sr_8k)) {
    voice_config.sampleRate = sample_rate;
  }
  // Otherwise leave the sample rate unchanged.
}

/***************************************************************************//**
 * Setter for configuration setting channels.
 ******************************************************************************/
void voice_set_channels(uint8_t channels)
{
  if ((channels > 0) && (channels <= MIC_CHANNELS_MAX)) {
    voice_config.channels = channels;
  }
  // Otherwise leave the parameter unchanged.
}

/***************************************************************************//**
 * Setter for configuration setting filter status.
 ******************************************************************************/
void voice_set_filter_enable(bool status)
{
  voice_config.filter_enabled = status;
}

/***************************************************************************//**
 * Setter for configuration setting encoding status.
 ******************************************************************************/
void voice_set_encoding_enable(bool status)
{
  voice_config.encoding_enabled = status;
}

// -----------------------------------------------------------------------------
// Private function definitions

static void voice_process_data(void)
{
  cb_err_code_t err;
  int16_t buffer[MIC_SAMPLE_BUFFER_SIZE];
  uint32_t sample_count = frames * voice_config.channels;

  // Move DMA samples to local buffer.
  for (uint32_t i = 0; i < sample_count; i++ ) {
    buffer[i] = sample_buffer[i];
  }

  if (voice_config.filter_enabled) {
    // Filter samples.
    fil_filter(&filter, buffer, buffer, frames);
  }

  if (voice_config.encoding_enabled) {
    // Encode samples.
    ADPCM_encode(&adpcm, buffer, adpcm_payload, frames);
    err = cb_push_buff(&circular_buffer, adpcm_payload, sample_count / ADPCM_COMPRESS_FACTOR);
  } else {
    err = cb_push_buff(&circular_buffer, buffer, sample_count * MIC_SAMPLE_SIZE);
  }
  app_assert(err == cb_err_ok,
             "[E: 0x%04x] Circular buffer push failed\n",
             (int)err);

  event_send = true;
}

static void voice_send_data(void)
{
  cb_err_code_t cb_error;
  uint8_t buffer[MIC_SEND_BUFFER_SIZE];

  cb_error = cb_pop_buff(&circular_buffer, buffer, MIC_SEND_BUFFER_SIZE);

  if ( cb_error == cb_err_ok ) {
    voice_transmit(buffer, MIC_SEND_BUFFER_SIZE);
    event_send = true;
  }
}

static void mic_buffer_ready(const void *buffer, uint32_t n_frames)
{
  sample_buffer = (int16_t *)buffer;
  frames = n_frames;
  event_process = true;
}
