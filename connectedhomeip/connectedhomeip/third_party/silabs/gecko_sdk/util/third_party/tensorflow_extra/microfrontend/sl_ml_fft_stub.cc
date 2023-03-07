/***************************************************************************//**
 * @file
 * @brief SL_ML_FFT_STUB
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
#include "microfrontend/sl_ml_fft.h"

/**************************************************************************//**
 * Compute real FFT
 *****************************************************************************/
void sli_ml_fft_compute(struct sli_ml_fft_state* state, const int16_t* input,
                int input_scale_shift) {
  (void)state;
  (void)input;
  (void)input_scale_shift;
}

/**************************************************************************//**
 * Initialize FFT state
 *****************************************************************************/
sl_status_t sli_ml_fft_init(struct sli_ml_fft_state* state, size_t input_size) {
  (void)state;
  (void)input_size;
  return SL_STATUS_OK;
}

void sli_ml_fft_deinit(struct sli_ml_fft_state* state) {
  (void)state;
}

/**************************************************************************//**
 * Clear FFT buffers
 *****************************************************************************/
void sli_ml_fft_reset(struct sli_ml_fft_state* state){
  (void)state;
}
