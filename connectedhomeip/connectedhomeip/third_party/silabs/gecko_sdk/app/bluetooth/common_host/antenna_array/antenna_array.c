/***************************************************************************//**
 * @file
 * @brief Antenna array handler for Silicon Labs boards.
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

#include <stdlib.h>
#include "antenna_array.h"
#include "antenna_array_brd4191a.h"

static uint8_t get_array_variant(uint8_t array_type, uint8_t *pattern);

static const uint8_t antenna_array_4x4_ura[] = {
  0, 1, 2, 3,
  4, 5, 6, 7,
  8, 9, 10, 11,
  12, 13, 14, 15
};

static const uint8_t antenna_array_3x3_ura[] = {
  // Variant 0
  0, 1, 2,
  4, 5, 6,
  8, 9, 10,
  // Variant 1
  1, 2, 3,
  5, 6, 7,
  9, 10, 11,
  // Variant 2
  4, 5, 6,
  8, 9, 10,
  12, 13, 14,
  // Variant 3
  5, 6, 7,
  9, 10, 11,
  13, 14, 15
};

static const uint8_t antenna_array_1x4_ula[] = {
  // Variant 0
  0, 1, 2, 3,
  // Variant 1
  4, 5, 6, 7,
  // Variant 2
  8, 9, 10, 11,
  // Variant 3
  12, 13, 14, 15,
  // Variant 4
  0,
  4,
  8,
  12,
  // Variant 5
  1,
  5,
  9,
  13,
  // Variant 6
  2,
  6,
  10,
  14,
  // Variant 7
  3,
  7,
  11,
  15
};

static const uint8_t array_size[ANTENNA_ARRAY_TYPE_LAST] = {
  16, // ANTENNA_ARRAY_TYPE_4x4_URA
  9,  // ANTENNA_ARRAY_TYPE_3x3_URA
  4,  // ANTENNA_ARRAY_TYPE_1x4_ULA
  16  // ANTENNA_ARRAY_TYPE_4x4_DP_URA
};

static const uint8_t array_variants[ANTENNA_ARRAY_TYPE_LAST] = {
  1, // ANTENNA_ARRAY_TYPE_4x4_URA
  4, // ANTENNA_ARRAY_TYPE_3x3_URA
  8, // ANTENNA_ARRAY_TYPE_1x4_ULA
  1  // ANTENNA_ARRAY_TYPE_4x4_DP_URA
};

static const uint8_t *array_list[ANTENNA_ARRAY_TYPE_LAST] = {
  antenna_array_4x4_ura,
  antenna_array_3x3_ura,
  antenna_array_1x4_ula,
  antenna_array_4x4_ura  // 4x4 DP is the same as 4x4 CP.
};

static const uint8_t antenna_array_dp_vertical[] = ANT_VERTICAL;
static const uint8_t antenna_array_dp_horizontal[] = ANT_HORIZONTAL;
static const uint8_t reference_antenna = ANT_6_CP;

/***************************************************************************//**
 * Initialize antenna switch pattern with default values.
 ******************************************************************************/
sl_status_t antenna_array_init(antenna_array_t *antenna_array,
                               uint8_t array_type)
{
  if (antenna_array == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (array_type >= ANTENNA_ARRAY_TYPE_LAST) {
    // Array type not supported or invalid.
    return SL_STATUS_INVALID_PARAMETER;
  }

  antenna_array->array_type = array_type;
  antenna_array->size = array_size[array_type];

  for (uint8_t i = 0; i < antenna_array->size; i++) {
    // Use the 1st variant as default.
    antenna_array->pattern[i] = array_list[array_type][i];
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Override antenna switch pattern.
 ******************************************************************************/
sl_status_t antenna_array_set_pattern(antenna_array_t *antenna_array,
                                      uint8_t *pattern,
                                      uint8_t size)
{
  if ((antenna_array == NULL) || (pattern == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }

  if (size != antenna_array->size) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check if pattern is valid.
  if (get_array_variant(antenna_array->array_type, pattern) == array_variants[antenna_array->array_type]) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Copy pattern.
  for (uint8_t i = 0; i < antenna_array->size; i++) {
    antenna_array->pattern[i] = pattern[i];
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Get antenna switch pin sequence.
 *
 * @param[in] antenna_array Antenna array instance to operate on.
 * @param[out] pattern Pin sequence that implements the antenna switch pattern.
 *                     The first element in the sequence represents the
 *                     reference antenna.
 * @param[in,out] size Buffer length as input, actual pattern length as output.
 ******************************************************************************/
sl_status_t antenna_array_get_pin_pattern(antenna_array_t *antenna_array,
                                          uint8_t *pattern,
                                          uint8_t *size)
{
  if ((antenna_array == NULL) || (size == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }
  if (antenna_array_type_is_dp(antenna_array->array_type)) {
    // Output buffer is optional.
    if (pattern != NULL) {
      // Check if output buffer size is large enough.
      if (*size < (1 + (2 * antenna_array->size))) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      // Fill output buffer.
      pattern[0] = reference_antenna;
      for (uint8_t i = 0; i < antenna_array->size; i++) {
        pattern[2 * i + 1] = antenna_array_dp_vertical[antenna_array->pattern[i]];
        pattern[2 * i + 2] = antenna_array_dp_horizontal[antenna_array->pattern[i]];
      }
    }
    *size = 1 + (2 * antenna_array->size);
  } else {
    // Output buffer is optional.
    if (pattern != NULL) {
      // Check if output buffer size is large enough.
      if (*size < antenna_array->size) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      // Fill output buffer.
      for (uint8_t i = 0; i < antenna_array->size; i++) {
        pattern[i] = antenna_array->pattern[i];
      }
    }
    *size = antenna_array->size;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Convert antenna switch pattern to [0..n-1] interval without gaps.
 ******************************************************************************/
sl_status_t antenna_array_get_continuous_pattern(antenna_array_t *antenna_array,
                                                 uint32_t *pattern,
                                                 uint32_t *size)
{
  if ((antenna_array == NULL) || (pattern == NULL) || (size == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }

  if (*size < antenna_array->size) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  uint8_t variant = get_array_variant(antenna_array->array_type, antenna_array->pattern);
  if (variant == array_variants[antenna_array->array_type]) {
    // Pattern corrupted.
    return SL_STATUS_FAIL;
  }

  uint8_t offset = variant * array_size[antenna_array->array_type];
  const uint8_t *reference_array = &array_list[antenna_array->array_type][offset];

  for (uint8_t i = 0; i < antenna_array->size; i++) {
    for (uint8_t j = 0; j < array_size[antenna_array->array_type]; j++) {
      if (antenna_array->pattern[i] == reference_array[j]) {
        pattern[i] = j;
        break;
      }
    }
  }
  *size = antenna_array->size;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Shuffle the current antenna switch pattern.
 ******************************************************************************/
sl_status_t antenna_array_shuffle_pattern(antenna_array_t *antenna_array,
                                          int(random)(void))
{
  uint8_t i, j, temp;

  if ((antenna_array == NULL) || (random == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }
  // Swap index i with a random index j.
  for (i = 0; i < antenna_array->size - 1; i++) {
    j = i + 1 + (random() % (antenna_array->size - i - 1));
    temp = antenna_array->pattern[j];
    antenna_array->pattern[j] = antenna_array->pattern[i];
    antenna_array->pattern[i] = temp;
  }
  return SL_STATUS_OK;
}

static uint8_t get_array_variant(uint8_t array_type, uint8_t *pattern)
{
  uint8_t variant;
  uint8_t antenna_i;
  uint8_t antenna_j;

  for (variant = 0; variant < array_variants[array_type]; variant++) {
    for (antenna_i = 0; antenna_i < array_size[array_type]; antenna_i++) {
      for (antenna_j = 0; (antenna_j < array_size[array_type]); antenna_j++) {
        if (pattern[antenna_j] == array_list[array_type][variant * array_size[array_type] + antenna_i]) {
          // Antenna found.
          break;
        }
      }
      if (antenna_j == array_size[array_type]) {
        // Antenna not found, try next variant.
        break;
      }
    }
    if (antenna_i == array_size[array_type]) {
      // All antennas found for the variant.
      break;
    }
  }
  return variant;
}
