/***************************************************************************//**
 * @file
 * @brief MVP pooling kernel driver.
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

#ifndef SL_MVP_ML_POOLING_H
#define SL_MVP_ML_POOLING_H

#include "sl_status.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
/***************************************************************************//**
 * @addtogroup mvp MVP API
 * @{
 ******************************************************************************/

/** Pooling data structure. */
typedef struct {
  int             batches;                /**< Number of batches.           */
  int             channels;               /**< Number of channels.          */
  const int8_t    *input;                 /**< Input data pointer.          */
  int             input_height;           /**< Input matrix height.         */
  int             input_width;            /**< Input matrix width.          */
  int8_t          *output;                /**< Output data pointer.         */
  int             output_height;          /**< Output matrix height.        */
  int             output_width;           /**< Output matrix width.         */
  int             output_activation_min;  /**< Minimum activation value.    */
  int             output_activation_max;  /**< Maximum activation value.    */
  int             filter_height;          /**< Filter/kernel height.        */
  int             filter_width;           /**< Filter/kernel width.         */
  int             stride_height;          /**< Filter/kernel stride height. */
  int             stride_width;           /**< Filter/kernel stride width.  */
  bool            padding;                /**< Padding mode, true=SAME, false=VALID. */
  int             pad_height;             /**< Padding height (pad top).    */
  int             pad_width;              /**< Padding width (pad left).    */
} sli_mvp_ml_pooling_s8_params_t;

/***************************************************************************//**
 * @brief
 *    Perform average pooling.
 *
 * @details
 *    Pooling is performed on 8-bit integer data on N channels of two
 *    dimensional matrices.
 *
 * @param[in] params Pointer to a data structure containing information on
 *                   input data, refer to @ref sli_mvp_ml_pooling_s8_params_t.
 *
 * @return
 *    @ref SL_STATUS_OK on success. On failure, an appropriate sl_status_t
 *    errorcode is returned.
 ******************************************************************************/
sl_status_t sli_mvp_ml_average_pooling_s8(const sli_mvp_ml_pooling_s8_params_t *params);

/***************************************************************************//**
 * @brief
 *    Perform average pooling. Only intended for use during unit testing.
 *
 * @details
 *    Pooling is performed on 8-bit integer data on N channels of two
 *    dimensional matrices.
 *
 * @param[in] params Pointer to a data structure containing information on
 *                   input data, refer to @ref sli_mvp_ml_pooling_s8_params_t.
 * @param[in] force_optimized Select optimized algorithm.
 *
 * @return
 *    @ref SL_STATUS_OK on success. On failure, an appropriate sl_status_t
 *    errorcode is returned.
 ******************************************************************************/
sl_status_t sli_mvp_ml_average_pooling_s8_test(const sli_mvp_ml_pooling_s8_params_t *params,
                                               bool force_optimized);

/***************************************************************************//**
 * @brief
 *    Perform max pooling.
 *
 * @details
 *    Pooling is performed on 8-bit integer data on N channels of two
 *    dimensional matrices.
 *
 * @param[in] params Pointer to a data structure containing information on
 *                   input data, refer to @ref sli_mvp_ml_pooling_s8_params_t.
 *
 * @return
 *    @ref SL_STATUS_OK on success. On failure, an appropriate sl_status_t
 *    errorcode is returned.
 ******************************************************************************/
sl_status_t sli_mvp_ml_max_pooling_s8(const sli_mvp_ml_pooling_s8_params_t *params);

/***************************************************************************//**
 * @brief
 *    Check if average pooling is supported.
 *
 * @details
 *    Check if the MVP hardware is capable of average pooling with given matrix
 *    parameters.
 *
 * @param[in] params Pointer to a data structure containing information on
 *                   input data, refer to @ref sli_mvp_ml_pooling_s8_params_t.
 *
 * @return
 *    True if average pooling is supported, false otherwise.
 ******************************************************************************/
bool sli_mvp_ml_average_pooling_s8_is_supported(const sli_mvp_ml_pooling_s8_params_t *params);

/***************************************************************************//**
 * @brief
 *    Check if max pooling is supported.
 *
 * @details
 *    Check if the MVP hardware is capable of max pooling with given matrix
 *    parameters.
 *
 * @param[in] params Pointer to a data structure containing information on
 *                   input data, refer to @ref sli_mvp_ml_pooling_s8_params_t.
 *
 * @return
 *    True if max pooling is supported, false otherwise.
 ******************************************************************************/
bool sli_mvp_ml_max_pooling_s8_is_supported(const sli_mvp_ml_pooling_s8_params_t *params);

/** @} (end addtogroup mvp) */
/// @endcond

#ifdef __cplusplus
}
#endif

#endif // SL_MVP_ML_POOLING_H
