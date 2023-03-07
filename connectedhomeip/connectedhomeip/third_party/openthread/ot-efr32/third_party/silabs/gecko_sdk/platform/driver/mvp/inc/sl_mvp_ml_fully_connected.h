/***************************************************************************//**
 * @file
 * @brief MVP fully connected kernel driver.
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

#ifndef SL_MVP_ML_FULLY_CONNECTED_H
#define SL_MVP_ML_FULLY_CONNECTED_H

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

/** Fully connected data structure. */
typedef struct {
  const int8_t    *input;             ///< Input data pointer.
  sli_shape_t     input_shape;        ///< Input shape.
  int             input_offset;       ///< Input offset. This is negative input zero point.
  const int8_t    *weight;            ///< Weight data pointer.
  sli_shape_t     weight_shape;       ///< Weight shape.
  int             weight_offset;      ///< Weight zero point. This is negative weight zero point.
  const float16_t *bias;              ///< Bias data pointer.
  size_t          bias_length;        ///< Bias length.
  int8_t          *output;            ///< Output data pointer.
  sli_shape_t     output_shape;       ///< Output shape.
  int             output_offset;      ///< Output offset. This is the same as output zero point.
  float16_t       output_multiplier;  ///< Output multiplier, see @ref sli_mvp_ml_fully_connected_output_multiplier.
  int8_t          activation_min;     ///< Minimum activation/output value.
  int8_t          activation_max;     ///< Maximum activation/output value.
} sli_mvp_ml_fully_connected_s8_params_t;

/***************************************************************************//**
 * @brief
 *   Perform fully connected operation.
 *
 * @details
 *   Fully connected is performed on 8-bit signed integer data with quantization
 *   parameters.
 *
 * @param[in] params
 *   Pointer to a data structure containing information on input data, refer
 *   to @ref sli_mvp_ml_fully_connected_s8_params_t.
 *
 * @return
 *   @ref SL_STATUS_OK on success. On failure, an appropriate sl_status_t
 *   errorcode is returned.
 ******************************************************************************/
sl_status_t sli_mvp_ml_fully_connected_s8(const sli_mvp_ml_fully_connected_s8_params_t *params);

/***************************************************************************//**
 * @brief
 *   Convert a 32 bit integer bias to the float16_t bias used by fully connected.
 *
 * @details
 *   The fully connected function needs to operate on a bias represented by a
 *   16 bit floating point number. In the conversion process the bias value is
 *   also scaled down in order to perform calculations withing the 16 bit
 *   floating point range.
 *
 * @param[in] bias
 *   Pointer to the original 32 bit integer bias vector.
 *
 * @param[out] dst
 *   Pointer to the memory area for the 16 bit floating point bias vector.
 *
 * @param[in] len
 *   Number of elements in the bias vector.
 *
 * @return
 *   @ref SL_STATUS_OK on success. On failure, an appropriate sl_status_t
 *   errorcode is returned.
 ******************************************************************************/
sl_status_t sli_mvp_ml_fully_connected_bias_convert(const int32_t *bias, float16_t *dst, size_t len);

/***************************************************************************//**
 * @brief
 *   Calculate the 16 bit output multiplier based on a double multiplier.
 *
 * @details
 *   The fully connected function operates on scaled down 16 bit floating point
 *   values and needs to use a 16 bit output multiplier value based on the full
 *   double precision output multiplier.
 *
 * @param[in] multiplier
 *   Full double precision output multiplier.
 *
 * @return
 *   16 bit floating point output multiplier which is used when later calling
 *   the @ref sli_mvp_ml_fully_connected_s8 function. This multiplier takes
 *   into account the internal scaling factor.
 ******************************************************************************/
float16_t sli_mvp_ml_fully_connected_output_multiplier(double multiplier);

/***************************************************************************//**
 * @brief
 *   Check if fully connected operation is supported on the given parameters.
 *
 * @details
 *   Check if the MVP hardware is capable of performing the fully connected
 *   operation on the given parameters.
 *
 * @param[in] params
 *   Pointer to a data structure containing information on parameter data,
 *   @ref sli_mvp_ml_fully_connected_s8_params_t. Note that not all members
 *   need to be filled out, as this operation is typically called before the
 *   input content is known.
 *
 * @return
 *    true if operation is supported on the supplied parameters,
 *    false otherwise.
 ******************************************************************************/
bool sli_mvp_ml_fully_connected_s8_is_supported(const sli_mvp_ml_fully_connected_s8_params_t *params);

/** @} (end addtogroup mvp) */
/// @endcond

#ifdef __cplusplus
}
#endif

#endif // SL_MVP_ML_FULLY_CONNECTED_H
