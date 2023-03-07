/***************************************************************************//**
 * @file
 * @brief Voice transmission header
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
#ifndef VOICE_H
#define VOICE_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  sr_8k = 8,
  sr_16k = 16,
} sample_rate_t;

/***************************************************************************//**
 * Initialize internal variables.
 ******************************************************************************/
void voice_init(void);

/***************************************************************************//**
 * Start voice transmission.
 ******************************************************************************/
void voice_start(void);

/***************************************************************************//**
 * Stop voice transmission.
 ******************************************************************************/
void voice_stop(void);

/***************************************************************************//**
 * Transmit voice buffer.
 * @param[in] buffer Transmit buffer containing voice data.
 * @param[in] size Size of the transmit buffer.
 * @note To be implemented in user code.
 ******************************************************************************/
void voice_transmit(uint8_t *buffer, uint32_t size);

/***************************************************************************//**
 * Voice event handler.
 ******************************************************************************/
void voice_process_action(void);

/***************************************************************************//**
 * Setter for configuration setting sample rate.
 *
 * @param[in] sample_rate Sample rate to be used, see \ref sample_rate_t.
 ******************************************************************************/
void voice_set_sample_rate(sample_rate_t sample_rate);

/***************************************************************************//**
 * Setter for configuration setting channels.
 *
 * @param[in] channels Number of audio channels to use.
 ******************************************************************************/
void voice_set_channels(uint8_t channels);

/***************************************************************************//**
 * Setter for configuration setting filter status.
 *
 * @param[in] status Enable (true) or disable (false) the biquad filter.
 ******************************************************************************/
void voice_set_filter_enable(bool status);

/***************************************************************************//**
 * Setter for configuration setting encoding status.
 *
 * @param[in] status Enable (true) or disable (false) the ADPCM encoder.
 ******************************************************************************/
void voice_set_encoding_enable(bool status);

#endif // VOICE_H
