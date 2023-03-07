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
#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)

#include "sl_se_manager.h"
#include "sli_se_manager_internal.h"
#include "em_se.h"
#include "em_system.h"
#include <string.h>

/// @addtogroup sl_se_manager
/// @{

// -----------------------------------------------------------------------------
// Global Functions

/***************************************************************************//**
 * Get random data from hardware TRNG.
 ******************************************************************************/
sl_status_t sl_se_get_random(sl_se_command_context_t *cmd_ctx,
                             void * data,
                             uint32_t num_bytes)
{
  SE_Command_t *se_cmd;
  sl_status_t ret;
  uint32_t surplus_bytes, i;
  uint32_t surplus_word = 0;

  if (cmd_ctx == NULL || (num_bytes != 0 && data == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  se_cmd = &cmd_ctx->command;
  surplus_bytes = num_bytes & 0x3U;
  num_bytes &= ~0x3U;

  if (num_bytes > 0U) {
    sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_TRNG_GET_RANDOM);
    SE_DataTransfer_t data_out = SE_DATATRANSFER_DEFAULT(data, num_bytes);

    SE_addDataOutput(se_cmd, &data_out);
    SE_addParameter(se_cmd, num_bytes);

    // Execute and wait
    if ((ret = sli_se_execute_and_wait(cmd_ctx)) != SL_STATUS_OK) {
      memset(data, 0, num_bytes);
      return ret;
    }
  }

  if (surplus_bytes > 0) {
    sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_TRNG_GET_RANDOM);
    SE_DataTransfer_t data_out = SE_DATATRANSFER_DEFAULT(&surplus_word, 4);

    SE_addDataOutput(se_cmd, &data_out);
    SE_addParameter(se_cmd, 4);

    // Execute and wait
    if ((ret = sli_se_execute_and_wait(cmd_ctx)) != SL_STATUS_OK) {
      memset(data, 0, num_bytes + surplus_bytes);
      return ret;
    }

    uint8_t *output = (uint8_t*)data + num_bytes;
    for (i = 0; i < surplus_bytes; i++) {
      output[i] = (surplus_word >> (i * 8U)) & 0xFFU;
    }
  }

  return SL_STATUS_OK;
}

/** @} (end addtogroup sl_se) */

#endif // defined(SEMAILBOX_PRESENT)
