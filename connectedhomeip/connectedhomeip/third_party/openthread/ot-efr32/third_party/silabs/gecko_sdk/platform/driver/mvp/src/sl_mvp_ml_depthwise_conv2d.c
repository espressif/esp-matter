/***************************************************************************//**
 * @file
 * @brief MVP Depthwise 2D Convolution kernel driver.
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

#include "sl_mvp_ml_depthwise_conv2d.h"
#include "sl_mvp.h"
#include "sl_mvp_math.h"
#include "sl_common.h"
#include <stdbool.h>

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

sl_status_t sli_mvp_ml_depthwise_conv2d_s8_gen(const sli_mvp_ml_depthwise_conv2d_s8_params_t *params,
                                               bool execute);

sl_status_t sli_mvp_ml_depthwise_conv2d_s8_gen_opt(const sli_mvp_ml_depthwise_conv2d_s8_params_t *params,
                                                   bool execute);

/***************************************************************************//**
 *
 * Depthwise 2D Convolution.
 *
 ******************************************************************************/
sl_status_t sli_mvp_ml_depthwise_conv2d_s8(const sli_mvp_ml_depthwise_conv2d_s8_params_t *params)
{
  sl_status_t status = SL_STATUS_FAIL;
  bool opt_supported = false;

  // Check if the optimized algorithm can be used.
  // Does not support dilation or depth multiplier.
  if ((params->dilation_width == 1)
      && (params->dilation_height == 1)
      && (params->out_channels == params->in_channels)
      && (sli_mvp_ml_depthwise_conv2d_s8_gen_opt(params, false) == SL_STATUS_OK)) {
    opt_supported = true;
  }

  // Run optimized algorithm for small inputs with high depth.
  if (((params->input_width * params->input_height)
       <= (4 * params->filter_width * params->filter_height * params->out_channels))
      && opt_supported) {
    status = sli_mvp_ml_depthwise_conv2d_s8_gen_opt(params, true);
  }

  if (status != SL_STATUS_OK) {
    // Fall back to generic algorithm.
    status =  sli_mvp_ml_depthwise_conv2d_s8_gen(params, true);
    if ((status != SL_STATUS_OK) && opt_supported) {
      // Generic algorithm failed. Optimized algorithm is not optimal, but can be used.
      status = sli_mvp_ml_depthwise_conv2d_s8_gen_opt(params, true);
    }
  }

  if (status != SL_STATUS_OK) {
    EFM_ASSERT(false);
  }

  return status;
}

/***************************************************************************//**
 *
 * Check if MVP supports Depthwise Conv2D on given matrix.
 *
 ******************************************************************************/
bool sli_mvp_ml_depthwise_conv2d_s8_is_supported(const sli_mvp_ml_depthwise_conv2d_s8_params_t *params)
{
  bool supported = false;

  // Run optimized algorithm for small inputs with high depth.
  // Does not support dilation or depth multiplier.
  if ((params->dilation_width == 1)
      && (params->dilation_height == 1)
      && (params->out_channels == params->in_channels)
      && ((params->input_width * params->input_height)
          <= (4 * params->filter_width * params->filter_height * params->out_channels))) {
    supported = sli_mvp_ml_depthwise_conv2d_s8_gen_opt(params, false) == SL_STATUS_OK;
  }

  // Optimized algorithm didn't work, fall back to generic algorithm
  if (!supported) {
    supported = sli_mvp_ml_depthwise_conv2d_s8_gen(params, false) == SL_STATUS_OK;
  }

  return supported;
}

/// @endcond
