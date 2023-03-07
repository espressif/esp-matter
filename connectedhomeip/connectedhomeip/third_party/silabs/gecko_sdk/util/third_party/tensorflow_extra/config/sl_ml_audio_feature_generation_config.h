/***************************************************************************//**
 * @file
 * @brief Configuration file for Audio Frontend
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
#ifndef SL_ML_AUDIO_FEATURE_GENERATION_CONFIG_H
#define SL_ML_AUDIO_FEATURE_GENERATION_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Audio Frontend Configuration

// <o SL_ML_AUDIO_FEATURE_GENERATION_AUDIO_BUFFER_SIZE> Audio Buffer Size <1024-16384:512>
// <i> Sets the length of the audio buffer in bytes. The audio buffer must
// <i> be large enough to store all new data between calls to
// <i> sl_ml_audio_feature_generation_update_features().
// <i> Note: Length must be a multiple of 512
// <i> Default: 1024
#define SL_ML_AUDIO_FEATURE_GENERATION_AUDIO_BUFFER_SIZE               1024


// <o SL_ML_AUDIO_FEATURE_GENERATION_AUDIO_GAIN> Audio Gain
// <i> Scale the raw audio data from microphone by given factor
// <i> Default: 1
#define SL_ML_AUDIO_FEATURE_GENERATION_AUDIO_GAIN                      1

// <q SL_ML_AUDIO_FEATURE_GENERATION_MANUAL_CONFIG_ENABLE> Enable Manual Frontend Configurations
// <i> Enable manual configuration of the Frontend
// <i> This may overwrite settings that are already embedded in
// <i> a .tflite file.
// <i> Default: 0
#define SL_ML_AUDIO_FEATURE_GENERATION_MANUAL_CONFIG_ENABLE            0
// </h>


// <<< end of configuration section >>>
/*******************************************************************************
 *******************   MANUAL FRONTEND CONFIGURATIONS   ************************
 ******************************************************************************/
#if (SL_ML_AUDIO_FEATURE_GENERATION_MANUAL_CONFIG_ENABLE == 0)

#if defined __has_include
#if !(__has_include ("sl_tflite_micro_model_parameters.h"))
  #error No frontend configurations were found in the model, enable manual configurations in "sl_ml_audio_feature_generation_config.h"
#endif 
#endif

#include "sl_tflite_micro_model_parameters.h"

#ifdef SL_TFLITE_MODEL_FE_SAMPLE_RATE_HZ
// Use frontend configurations from tflite model file in sl_tflite_micro_model_parameters.h
#define SL_ML_FRONTEND_SAMPLE_RATE_HZ                         SL_TFLITE_MODEL_FE_SAMPLE_RATE_HZ
#define SL_ML_FRONTEND_SAMPLE_LENGTH_MS                       SL_TFLITE_MODEL_FE_SAMPLE_LENGTH_MS
#define SL_ML_FRONTEND_WINDOW_SIZE_MS                         SL_TFLITE_MODEL_FE_WINDOW_SIZE_MS
#define SL_ML_FRONTEND_WINDOW_STEP_MS                         SL_TFLITE_MODEL_FE_WINDOW_STEP_MS
#define SL_ML_FRONTEND_FFT_LENGTH                             SL_TFLITE_MODEL_FE_FFT_LENGTH
#define SL_ML_FRONTEND_FILTERBANK_N_CHANNELS                  SL_TFLITE_MODEL_FE_FILTERBANK_N_CHANNELS
#define SL_ML_FRONTEND_FILTERBANK_LOWER_BAND_LIMIT            SL_TFLITE_MODEL_FE_FILTERBANK_LOWER_BAND_LIMIT
#define SL_ML_FRONTEND_FILTERBANK_UPPER_BAND_LIMIT            SL_TFLITE_MODEL_FE_FILTERBANK_UPPER_BAND_LIMIT
#define SL_ML_FRONTEND_NOISE_REDUCTION_ENABLE                 SL_TFLITE_MODEL_FE_NOISE_REDUCTION_ENABLE
#define SL_ML_FRONTEND_NOISE_REDUCTION_SMOOTHING_BITS         SL_TFLITE_MODEL_FE_NOISE_REDUCTION_SMOOTHING_BITS
#define SL_ML_FRONTEND_NOISE_REDUCTION_EVEN_SMOOTHING         SL_TFLITE_MODEL_FE_NOISE_REDUCTION_EVEN_SMOOTHING
#define SL_ML_FRONTEND_NOISE_REDUCTION_ODD_SMOOTHING          SL_TFLITE_MODEL_FE_NOISE_REDUCTION_ODD_SMOOTHING
#define SL_ML_FRONTEND_NOISE_REDUCTION_MIN_SIGNAL_REMAINING   SL_TFLITE_MODEL_FE_NOISE_REDUCTION_MIN_SIGNAL_REMAINING
#define SL_ML_FRONTEND_PCAN_ENABLE                            SL_TFLITE_MODEL_FE_PCAN_ENABLE
#define SL_ML_FRONTEND_PCAN_STRENGTH                          SL_TFLITE_MODEL_FE_PCAN_STRENGTH
#define SL_ML_FRONTEND_PCAN_OFFSET                            SL_TFLITE_MODEL_FE_PCAN_OFFSET
#define SL_ML_FRONTEND_PCAN_GAIN_BITS                         SL_TFLITE_MODEL_FE_PCAN_GAIN_BITS
#define SL_ML_FRONTEND_LOG_SCALE_ENABLE                       SL_TFLITE_MODEL_FE_LOG_SCALE_ENABLE
#define SL_ML_FRONTEND_LOG_SCALE_SHIFT                        SL_TFLITE_MODEL_FE_LOG_SCALE_SHIFT
#else
#error No frontend configurations were found in the model, enable manual configurations in "sl_ml_audio_feature_generation_config.h"
#endif // SL_TFLITE_MODEL_FE_SAMPLE_RATE

#ifdef SL_TFLITE_MODEL_FE_QUANTIZE_DYNAMIC_SCALE_ENABLE
#define SL_ML_AUDIO_FEATURE_GENERATION_QUANTIZE_DYNAMIC_SCALE_ENABLE 	SL_TFLITE_MODEL_FE_QUANTIZE_DYNAMIC_SCALE_ENABLE
#endif

#else // SL_ML_AUDIO_FEATURE_GENERATION_MANUAL_CONFIG_ENABLE == 1

// <o> Audio Sample Rate
// <i> Default: 8000
#define SL_ML_FRONTEND_SAMPLE_RATE_HZ                            8000

// <o> Length of an audio sample in milliseconds
// <i> This determines how much audio is used to generate a
// <i> single feature buffer.
// <i> Default: 1000
#define SL_ML_FRONTEND_SAMPLE_LENGTH_MS                          1000

// <o> Window Size
// <i> Length of window in ms
// <i> Default: 30
#define SL_ML_FRONTEND_WINDOW_SIZE_MS                            30

// <o> Window Step
// <i> Step size in ms for next window
// <i> Default: 20
#define SL_ML_FRONTEND_WINDOW_STEP_MS                            20

// <o> FFT length
//   <32U=> 32
//   <64U=> 64
//   <128U=> 128
//   <256U=> 256
//   <512U=> 512
//   <1024U=> 1024
//   <2048U=> 2048
//   <4096U=> 4096
//   <8192U=> 8192
//   <i> Specifies the length of the RFFT
//   <i> Only supports lengths of a power of 2
//   <i> Default: 256U
#define SL_ML_FRONTEND_FFT_LENGTH                                256U

// <o> Number of Filterbank Channels
// <i> Default: 32
#define SL_ML_FRONTEND_FILTERBANK_N_CHANNELS                     32

// <o> Lower Frequency Limit
// <i> The lowest frequency to invclude in the filterbanks
// <i> Default: 0.0
#define SL_ML_FRONTEND_FILTERBANK_LOWER_BAND_LIMIT               0.0

// <o> Upper Frequency Limit
// <i> The highest frequency inlcuded in the filterbanks
// <i> Note: Must be less than or equal to Sample Rate/2
// <i> Default: 4000.0
#define SL_ML_FRONTEND_FILTERBANK_UPPER_BAND_LIMIT               4000.0

// <e> Enable Noise Reduction Module
// <i> Default: 0
#define SL_ML_FRONTEND_NOISE_REDUCTION_ENABLE                    0

// <o> Smoothing Bits
// <i> Scale up signal by 2^(smoothing_bits) before reduction
// <i> Default: 5
#define SL_ML_FRONTEND_NOISE_REDUCTION_SMOOTHING_BITS            5

// <o> Even Smoothing
// <i> Smoothing coefficient for even-numbered channels
// <i> Default: 0.004
#define SL_ML_FRONTEND_NOISE_REDUCTION_EVEN_SMOOTHING            0.004

// <o> Odd Smoothing
// <i> Smoothing coefficient for odd-numbered channels
// <i> Default: 0.004
#define SL_ML_FRONTEND_NOISE_REDUCTION_ODD_SMOOTHING             0.004

// <o> Minimum Signal Remaining
// <i> Fraction of signal to preserve in smoothing
// <i> Default: 0.05
#define SL_ML_FRONTEND_NOISE_REDUCTION_MIN_SIGNAL_REMAINING      0.05
// </e>

// <e> Enable PCAN auto gain control
// <i> Default: 0
#define SL_ML_FRONTEND_PCAN_ENABLE                               0

// <o> PCAN Strength
// <i> Gain Normalization Exponent
// <i> Default: 0.95
#define SL_ML_FRONTEND_PCAN_STRENGTH                             0.95

// <o> PCAN Offset
// <i> Positive value added in the normalization denominator
// <i> Default: 80.0
#define SL_ML_FRONTEND_PCAN_OFFSET                               80.0

// <o> Gain bits
// <i> Number of fractional bits in the gain
// <i> Default: 21
#define SL_ML_FRONTEND_PCAN_GAIN_BITS                            21
// </e>

// <e> Enable Log Scaling
// <i> Default: 1
#define SL_ML_FRONTEND_LOG_SCALE_ENABLE                          1

// <o> Scale Shift
// <i> Scale filterbanks by 2^(scale shift)
// <i> Default: 6
#define SL_ML_FRONTEND_LOG_SCALE_SHIFT                           6
// </e>

// <q> Enable dynamic quantization when filling tensor with features
// <i> When using sl_ml_audio_feature_generation_fill_tensor(), this
// <i> will enable dynamic scaling of the microfrontend output before
// <i> quantizing the values to int8.
// <i> When not enabled, the fill tensor function performs a static
// <i> quantization.
// <i> Default: 0
#define SL_ML_AUDIO_FEATURE_GENERATION_QUANTIZE_DYNAMIC_SCALE_ENABLE   0

#endif // SL_ML_AUDIO_FEATURE_GENERATION_MANUAL_CONFIG_ENABLE == 0

#endif // SL_ML_AUDIO_FEATURE_GENERATION_CONFIG_H
