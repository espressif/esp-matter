/***************************************************************************//**
 * @file
 * @brief MVP Utility functions.
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
#ifndef SL_MVP_UTIL_H
#define SL_MVP_UTIL_H

#include "sl_mvp.h"

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
 *   Clear memory using MVP.
 *   Will clear 2 * batches * vecs * rows * cols bytes in memory.
 *   This function is a preliminary version of a general purpose function.
 *
 * @note
 *   The function will not wait for MVP execution to complete before exit.
 *
 * @param[in] p Pointer to an initialized program context.
 * @param[in] dst Base address of memory area to clear, must be 4-byte aligned.
 * @param[in] batches Batches.
 * @param[in] vecs Vector dimension size.
 * @param[in] rows Row dimension size.
 * @param[in] cols Column dimension size.
 */
sl_status_t sli_mvp_util_memclr_f16(sli_mvp_program_context_t *p,
                                    void *dst,
                                    unsigned short batches,
                                    unsigned short vecs,
                                    unsigned short rows,
                                    unsigned short cols);

/**
 * @brief
 *   Calculate the offset of a given element in a NHWC formated array.
 *
 * @note
 *   NHWC: N = batch, H = height, W = width, C = channel.
 *   NHWC memory ordering is "row major" within each batch, and is the default
 *   memory ordering used in TensorFlow.
 *   Input parameters n, h, w and c are all zero indexed.
 *
 * @param[in] height Matrix height (rows).
 * @param[in] width  Matrix width (cols).
 * @param[in] depth  Matrix depth (vecs).
 * @param[in] n      batch.
 * @param[in] h      height (row).
 * @param[in] w      width (col).
 * @param[in] c      channel (vec).
 */
__INLINE int sli_mvp_util_offset_nhwc(int height,
                                      int width,
                                      int depth,
                                      int n,
                                      int h,
                                      int w,
                                      int c)
{
  return (((((n * height) + h) * width) + w) * depth) + c;
}

/** @} (end addtogroup mvp) */
/// @endcond

#ifdef __cplusplus
}
#endif

#endif // SL_MVP_UTIL_H
