/***************************************************************************//**
 * @file
 * @brief MVP add kernel driver.
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

#ifndef SL_MVP_ML_ADD_H
#define SL_MVP_ML_ADD_H

#include "sl_mvp_types.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
/***************************************************************************//**
 * @addtogroup mvp MVP API
 * @{
 ******************************************************************************/

/** Add operation data structure. */
typedef struct {
  const int8_t    *input1;            ///< Input 1 data pointer.
  int             input1_offset;      ///< Input 1 offset (negative input1 zero point).
  float           input1_multiplier;  ///< Input 1 multiplier.
  const int8_t    *input2;            ///< Input 2 data pointer.
  int             input2_offset;      ///< Input 2 offset (negative input2 zero point).
  float           input2_multiplier;  ///< Input 2 multiplier.
  int8_t          *output;            ///< Output data pointer.
  int             output_offset;      ///< Output offset (same as output zero point).
  float           output_multiplier;  ///< Output multiplier (1/output_scale).
  size_t          length;             ///< Length of inputs and output
  int8_t          activation_min;     ///< Minimum activation/output value.
  int8_t          activation_max;     ///< Maximum activation/output value.
} sli_mvp_ml_add_s8_params_t;

/***************************************************************************//**
 * @brief
 *   Perform add operation on quantized input data.
 *
 * @details
 *   The add operation is performed on 8-bit signed integer data with quantization
 *   parameters. Both of the inputs and the output vector can have different
 *   quantization parameters.
 *
 * @param[in] params
 *   Pointer to a data structure containing information on input data, refer
 *   to @ref sli_mvp_ml_add_s8_params_t.
 *
 * @return
 *   @ref SL_STATUS_OK on success. On failure, an appropriate sl_status_t
 *   errorcode is returned.
 ******************************************************************************/
sl_status_t sli_mvp_ml_add_s8(const sli_mvp_ml_add_s8_params_t *params);

/***************************************************************************//**
 * @brief
 *   Check if add operation is supported on the given parameters.
 *
 * @details
 *   Check if the MVP hardware is capable of performing the add
 *   operation on the given parameters.
 *
 * @param[in] params
 *   Pointer to a data structure containing information on parameter data,
 *   @ref sli_mvp_ml_add_s8_params_t. Note that not all members need to be filled
 *   out, as this operation is typically called before the input content is
 *   known.
 *
 * @return
 *    true if operation is supported on the supplied parameters,
 *    false otherwise.
 ******************************************************************************/
bool sli_mvp_ml_add_s8_is_supported(const sli_mvp_ml_add_s8_params_t *params);

/** @} (end addtogroup mvp) */
/// @endcond

#ifdef __cplusplus
}
#endif

#endif // SL_MVP_ML_FULLY_CONNECTED_H
