/***************************************************************************//**
 * @file
 * @brief Silicon Labs Secure Engine Manager API.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SL_SE_MANAGER_ENTROPY_H
#define SL_SE_MANAGER_ENTROPY_H

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT) || defined(DOXYGEN)

/// @addtogroup sl_se_manager
/// @{

/***************************************************************************//**
 * @addtogroup sl_se_manager_entropy Entropy
 *
 * @brief
 *   Random number generators
 * @details
 *
 *   API for getting randomness from the Secure Engine True Random Number
 *   Generator (TRNG).
 *
 * @{
 ******************************************************************************/

#include "sl_se_manager_key_handling.h"
#include "sl_se_manager_types.h"
#include "em_se.h"
#include "sl_status.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * @brief
 *   Get random data from hardware TRNG.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[out] data
 *   Random data from TRNG.
 *
 * @param[in] num_bytes
 *   Length of data request.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_get_random(sl_se_command_context_t *cmd_ctx,
                             void *data,
                             uint32_t num_bytes);

#ifdef __cplusplus
}
#endif

/// @} (end addtogroup sl_se_manager_entropy)
/// @} (end addtogroup sl_se)

#endif // defined(SEMAILBOX_PRESENT)

#endif // SL_SE_MANAGER_ENTROPY_H
