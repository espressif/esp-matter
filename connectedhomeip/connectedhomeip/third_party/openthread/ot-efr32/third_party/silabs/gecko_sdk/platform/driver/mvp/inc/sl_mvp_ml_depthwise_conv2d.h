/***************************************************************************//**
 * @file
 * @brief MVP Depthwise Conv2d kernel driver.
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

#ifndef SL_MVP_ML_DEPTHWISE_CONV2D_H
#define SL_MVP_ML_DEPTHWISE_CONV2D_H

#include "sl_status.h"
#include "sl_mvp.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
/***************************************************************************//**
 * @addtogroup mvp MVP API
 * @{
 ******************************************************************************/

/** Depthwise Conv2D data structure. */
typedef struct {
  int             batches;                /**< Number of batches.           */
  int             in_channels;            /**< Number of input channels.    */
  const int8_t    *input;                 /**< Input data pointer.          */
  int             input_height;           /**< Input matrix height.         */
  int             input_width;            /**< Input matrix width.          */
  int             out_channels;           /**< Number of output channels.   */
  int8_t          *output;                /**< Output data pointer.         */
  int             output_height;          /**< Output matrix height.        */
  int             output_width;           /**< Output matrix width.         */
  int             output_activation_min;  /**< Minimum activation value.    */
  int             output_activation_max;  /**< Maximum activation value.    */
  const float16_t *output_scaler;         /**< Output scalers data pointer. */
  const int8_t    *filter;                /**< Filter data pointer.         */
  int             filter_height;          /**< Filter/kernel height.        */
  int             filter_width;           /**< Filter/kernel width.         */
  const float16_t *bias;                  /**< Bias (optional) data pointer.*/
  int             stride_height;          /**< Filter/kernel stride height. */
  int             stride_width;           /**< Filter/kernel stride width.  */
  bool            padding;                /**< Padding mode, true=SAME, false=VALID. */
  int             pad_height;             /**< Padding height (pad top).    */
  int             pad_width;              /**< Padding width (pad left).    */
  int             dilation_height;        /**< Dilation height factor.      */
  int             dilation_width;         /**< Dilation width factor.       */
  int             input_offset;           /**< Zero value for the input tensor. */
  int             output_offset;          /**< Zero value for the output tensor.*/
} sli_mvp_ml_depthwise_conv2d_s8_params_t;

/***************************************************************************//**
 * @brief
 *    Perform Depthwise 2D convolution.
 *
 * @details
 *    Depthwise 2D Convolution is performed on 8-bit integer input/filter
 *    with float16_t bias and output scalers. Operation output is 8-bit integer.
 *
 * @param[in] params Pointer to a data structure containing information on
 *                   all input parameters, refer to
 *                   @ref sli_mvp_ml_depthwise_conv2d_s8_params_t.
 *
 * @return
 *    @ref SL_STATUS_OK on success. On failure, an appropriate sl_status_t
 *    errorcode is returned.
 ******************************************************************************/
sl_status_t sli_mvp_ml_depthwise_conv2d_s8(const sli_mvp_ml_depthwise_conv2d_s8_params_t *params);

/***************************************************************************//**
 * @brief
 *    Check if Depthwise Conv2D is supported.
 *
 * @details
 *    Check if the MVP hardware is capable of Depthwise Conv2D with given matrix
 *    parameters.
 *
 * @param[in] params Pointer to a data structure containing information on
 *                   input data, refer to @ref sli_mvp_ml_depthwise_conv2d_s8_params_t.
 *
 * @return
 *    True if Depthwise Conv2D is supported, false otherwise.
 ******************************************************************************/
bool sli_mvp_ml_depthwise_conv2d_s8_is_supported(const sli_mvp_ml_depthwise_conv2d_s8_params_t *params);

/** @} (end addtogroup mvp) */
/// @endcond

#ifdef __cplusplus
}
#endif

#endif // SL_MVP_ML_DEPTHWISE_CONV2D_H
