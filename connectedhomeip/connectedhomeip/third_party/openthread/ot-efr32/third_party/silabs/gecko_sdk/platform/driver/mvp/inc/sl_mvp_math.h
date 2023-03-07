/***************************************************************************//**
 * @file
 * @brief MVP Math functions.
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
#ifndef SL_MVP_MATH_H
#define SL_MVP_MATH_H

#include "sl_mvp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
/***************************************************************************//**
 * @addtogroup mvp MVP API
 * @{
 ******************************************************************************/

/**
 * @brief
 *   Add two vectors of signed 8 bit integers.
 *
 * @details
 *   All vectors must be of the same length. This function will perform the
 *   following operation: Z = A + B. The add operation is performing a saturation
 *   add, which means that the operation will never overflow or underflow.
 *   When adding two elements would overflow (>127) then the result
 *   will be 127. When adding two elements would underflow (<-128)
 *   then the result will be -128.
 *
 * @param[in] input_a First input vector, input A.
 * @param[in] input_b Second input vector, input B.
 * @param[out] output Output vector, output Z.
 * @param[in] len Length of all input and output vectors.
 */
void sli_mvp_math_vector_add_i8(const int8_t *input_a, const int8_t *input_b, int8_t *output, size_t len);

/**
 * @brief
 *   Clamp all signed 8 bit integers in a vector to a certain range.
 *
 * @details
 *   Given a min/max value, this function will make sure that none of the element
 *   in the input vector will be < min or > max. If any elements are < min then
 *   the value will be modified to min. If any elements are > max then value will
 *   be modified to max.
 *
 * @param[inout] data Vector with data values.
 * @param[in] len Length of vector.
 * @param[in] min Minimum value, after operation no elements will be < min.
 * @param[in] max Maximum value, after operation no elements will be > max.
 */
void sli_mvp_math_clamp_i8(int8_t *data, size_t len, int8_t min, int8_t max);

/** @} (end addtogroup mvp) */
/// @endcond

#ifdef __cplusplus
}
#endif

#endif // SL_MVP_MATH_H
