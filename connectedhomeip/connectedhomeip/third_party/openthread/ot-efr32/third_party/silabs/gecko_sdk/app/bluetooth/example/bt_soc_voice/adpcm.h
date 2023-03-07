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

#ifndef ADPCM_H_
#define ADPCM_H_

#include <stdint.h>

/***************************************************************************//**
 * @addtogroup ADPCM
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @defgroup ADPCM_Config_Settings ADPCM coder Configuration Settings
 * @{
 * @brief ADPCM coder configuration setting macro definitions
 ******************************************************************************/

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/
typedef struct {
  int8_t step;                         /**< Index into adpcmStepTable */
  int16_t predictedSample;             /**< Last predicted sample */
} adpcm_state_t;

typedef struct {
  adpcm_state_t comprStateBegin;       /**< Compression begin state */
  adpcm_state_t comprStateCurrent;     /**< Compression current state */
} adpcm_t;

typedef struct {
  uint8_t ch_count;
  adpcm_t *adpcm_list;
} adpcm_context_t;

/***************************************************************************//**
 * @defgroup ADPCM_Functions ADPCM coder Functions
 * @{
 * @brief ADPCM coder API functions
 ******************************************************************************/

void ADPCM_init(adpcm_context_t *ctx);
void ADPCM_encode(adpcm_context_t *ctx, int16_t *in, uint8_t* out, uint32_t n_frames);

/** @} {end defgroup ADPCM_Functions}*/

/** @} {end addtogroup ADPCM} */

#endif /* ADPCM_H_ */
