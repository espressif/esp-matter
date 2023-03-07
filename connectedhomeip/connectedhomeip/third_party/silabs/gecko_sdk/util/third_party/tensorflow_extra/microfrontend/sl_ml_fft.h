/***************************************************************************//**
 * @file
 * @brief SL_ML_FFT 
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

#ifndef SL_ML_FFT_H
#define SL_ML_FFT_H

#include <stdint.h>
#include <stdlib.h>

#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Complex int16 type
struct complex_int16_t {
  int16_t real;
  int16_t imag;
};

/// FFT state
struct sli_ml_fft_state {
  int16_t* input;                   ///< Pointer to the input buffer
  struct complex_int16_t* output;   ///< Pointer to the output buffer
  size_t input_size;                ///< Size of input buffer
  size_t fft_size;                  ///< Size of FFT 
  void *scratch;                    ///< Pointer to scratch buffer
  size_t scratch_size;              ///< Size of scratch buffer
};

/**************************************************************************//**
 * Compute real FFT 
 *
 * @param state The FFT state
 * @param input Pointer to the input buffer, not modified
 * @param input_scale_shift Number of bits to upscale the values of the input 
 *  buffer prior to calculating the FFT. 
 *
 * @note Uses ARM RFFT to compute the real FFT
 *****************************************************************************/
void sli_ml_fft_compute(struct sli_ml_fft_state* state, const int16_t* input,
                int input_scale_shift);

/**************************************************************************//**
 * Initialize the FFT state struct
 *
 * @param state The FFT state
 * @param input_size Size of the input buffer
 *
 * @note The buffer input size does not need to be equal to the FFT size, but 
 * it can not be larger. 
 *****************************************************************************/
sl_status_t sli_ml_fft_init(struct sli_ml_fft_state* state, size_t input_size);

/**************************************************************************//**
 * Clear FFT buffers
 *
 * @param state The FFT state
 *
 * Clears the input and output buffers
 *****************************************************************************/
void sli_ml_fft_reset(struct sli_ml_fft_state* state);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //SL_ML_FFT_H