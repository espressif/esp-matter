/***************************************************************************//**
 * @file
 * @brief MVP program memory allocator.
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
#ifndef SL_MVP_PROGRAM_AREA_H
#define SL_MVP_PROGRAM_AREA_H

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
 *   Get MVP program buffer.
 *
 * @details
 *   This function returns a pointer to a MVP program buffer located in RAM.
 *
 * @return
 *   Pointer to MVP program buffer.
 */
sli_mvp_program_t *sli_mvp_get_program_area_single(void);

/**
 * @brief
 *   Get a double MVP program buffer.
 *
 * @details
 *   This function returns a pointer to an array of two MVP program buffers
 *   located in RAM. With two program buffers it is possible to generate the
 *   next MVP program while current program is executing (ping-pong buffering).
 *
 * @return
 *   Pointer to array of two MVP program buffers.
 */
sli_mvp_program_t *sli_mvp_get_program_area_double(void);

/**
 * @brief
 *   Get a full MVP program buffer context.
 *
 * @details
 *   This function returns a pointer to a MVP program context located in RAM.
 *   A MVP program context contains two MVP program buffers plus MVP
 *   program builder bookkeeping data. This context can be used with the
 *   the MVP program builder APIs (sli_mvp_pb_xxx()).
 *
 * @return
 *   Pointer to MVP program buffer context.
 */
sli_mvp_program_context_t *sli_mvp_get_program_area_context(void);

/** @} (end addtogroup mvp) */
/// @endcond

#ifdef __cplusplus
}
#endif

#endif // SL_MVP_PROGRAM_AREA_H
