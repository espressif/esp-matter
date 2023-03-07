/***************************************************************************//**
 * @file
 * @brief ADPCM encoder
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <string.h>
#include "adpcm.h"

/***************************************************************************//**
 * @defgroup ADPCM ADPCM - Encoder
 * @{
 * @brief ADPCM IMA encoder
 ******************************************************************************/

/***************************************************************************//**
 * @defgroup ADPCM_Locals ADPCM encoder Local Variables
 * @{
 * @brief ADPCM encoder local variables
 ******************************************************************************/

static const int8_t adpcmIndexAdjust[16] = {
  -1, -1, -1, -1, 2, 4, 6, 8, /* +0 - +3 => decrease, +4 - +7 => increase step */
  -1, -1, -1, -1, 2, 4, 6, 8  /* -0 - -3 => decrease, -4 - -7 => increase step */
};

static const int16_t adpcmStepTable[89] = {
  7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
  50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130, 143, 157, 173, 190, 209, 230,
  253, 279, 307, 337, 371, 408, 449, 494, 544, 598, 658, 724, 796, 876, 963,
  1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024,
  3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493,
  10442, 11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086,
  29794, 32767
};

/** @} {end defgroup ADPCM_Locals} */
/** @endcond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @defgroup ADPCM_Functions ADPCM code Functions
 * @{
 * @brief ADPCM encoder functions
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
static uint8_t imaEncodeSample(int16_t sample, adpcm_state_t *adpcmState);

/** @endcond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Initialize ADPCM state.
 *
 * @param[in] ctx
 *    Pointer to the ADPCM context.
 *
 * @return
 *    None.
 ******************************************************************************/
void ADPCM_init(adpcm_context_t *ctx)
{
  for (uint8_t ch = 0; ch < ctx->ch_count; ch++) {
    adpcm_t *adpcm = &ctx->adpcm_list[ch];
    adpcm->comprStateBegin.predictedSample = 0;
    adpcm->comprStateBegin.step = 0;
    adpcm->comprStateCurrent.predictedSample = 0;
    adpcm->comprStateCurrent.step = 0;
  }
}

/***************************************************************************//**
 * @brief
 *    Encode buffer.
 *
 * @param[in] ctx
 *    Pointer to the ADPCM context.
 *
 * @param[in] in
 *    Audio samples to be encoded.
 *
 * @param[in] out
 *    Encoded audio samples.
 *
 * @param[in] n_frames
 *     Number of samples to process per channel.
 *
 * @return
 *    None.
 ******************************************************************************/
void ADPCM_encode(adpcm_context_t *ctx, int16_t *in, uint8_t* out, uint32_t n_frames)
{
  for (uint8_t ch = 0; ch < ctx->ch_count; ch++) {
    adpcm_t *adpcm = &ctx->adpcm_list[ch];
    uint8_t compressedNibble = 0;
    uint8_t compressedNibbleCount = 0;
    uint32_t idx_in, idx_out;

    /* Copy IMA ADPCM state */
    memcpy(&adpcm->comprStateBegin, &adpcm->comprStateCurrent, sizeof(adpcm_state_t));
    idx_out = ch;

    for (uint32_t i = 0; i < n_frames; i++) {
      idx_in = (i * ctx->ch_count) + ch;
      /* Use IMA_ADPCM compression from 16-bit to 4-bit nibble */
      compressedNibble <<= 4;              /* Prepare for next nibble */
      compressedNibble |= imaEncodeSample(in[idx_in], &adpcm->comprStateCurrent);

      compressedNibbleCount++;             /* Update nibble counter */
      if (compressedNibbleCount < 2) {     /* Both nibbles in byte */
        continue;                          /* No, get next nibble */
      }
      compressedNibbleCount = 0;           /* Reset nibble counter */

      /* Save IDM_ADPCM byte (two compressed samples) to audio buffer */
      out[idx_out] = compressedNibble;
      idx_out += ctx->ch_count;
    }
  }
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Encode sample IMA.
 *
 * @param[in] sample
 *    Sample to be encoded.
 *
 * @param[in] adpcmState
 *     ADPCM state.
 *
 * @return
 *    Compressed
 ******************************************************************************/
static uint8_t imaEncodeSample(int16_t sample, adpcm_state_t *adpcmState)
{
  uint8_t compressedCode = 0;
  /* Promote the current step size and predicted sample to int */
  int32_t currentStep = (int32_t)adpcmStepTable[adpcmState->step];
  int32_t predictedSample = (int32_t)adpcmState->predictedSample;
  int32_t diff;
  int32_t predictedDiff;

  /* Calculate the current difference */
  diff = (int32_t)sample - predictedSample;

  /* Set compressed code sign bit */
  if (diff < 0) {
    compressedCode = 8;
    diff = -diff;
  }

  /* Calculate the compressed code and predicted difference */
  predictedDiff = currentStep >> 3;
  if (diff >= currentStep) {
    compressedCode |= 4;
    diff -= currentStep;
    predictedDiff += currentStep;
  }
  currentStep >>= 1;
  if (diff >= currentStep) {
    compressedCode |= 2;
    diff -= currentStep;
    predictedDiff += currentStep;
  }
  currentStep >>= 1;
  if (diff >= currentStep) {
    compressedCode |= 1;
    predictedDiff += currentStep;
  }

  /* Calculate the new predicted sample with saturation logic from int32_t->int16_t */
  predictedSample += (compressedCode & 8) ? -predictedDiff : predictedDiff;
  if (predictedSample > 32767) {
    predictedSample = 32767;
  } else if (predictedSample < -32768) {
    predictedSample = -32768;
  }
  adpcmState->predictedSample = (int16_t)predictedSample;

  /* Update the step index for the next sample */
  adpcmState->step += adpcmIndexAdjust[compressedCode];
  if (adpcmState->step < 0) {
    adpcmState->step = 0;
  } else if (adpcmState->step > 88) {
    adpcmState->step = 88;
  }

  return compressedCode;
}

/** @endcond DO_NOT_INCLUDE_WITH_DOXYGEN */
