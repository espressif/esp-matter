/***************************************************************************//**
 * @file
 * @brief Application logging command line interface source file
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

#include "app_log.h"
#include "app_log_cli.h"
#include <stdint.h>
#include <stdlib.h>

/**************************************************************************//**
 * Set application log options.
 *****************************************************************************/
sl_status_t app_log_set_option(char option, char *value)
{
  sl_status_t sc = SL_STATUS_OK;
  unsigned long log_lev;

  switch (option) {
    // App log level.
    case 'l':
      log_lev = strtoul(value, NULL, 0);
      if (log_lev >= APP_LOG_LEVEL_COUNT) {
        sc = SL_STATUS_INVALID_PARAMETER;
      } else {
        sc = app_log_filter_threshold_set((uint8_t)log_lev);
      }
      break;
    // Unknown option.
    default:
      sc = SL_STATUS_NOT_FOUND;
      break;
  }
  return sc;
}
