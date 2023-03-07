/***************************************************************************//**
 * @file
 * @brief AoA configuration parser for CTE module.
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

#include <strings.h>
#include "aoa_parse_cte.h"

static const char *cte_mode_str[] = {
  "CONN",
  "CONNLESS",
  "CONN_LESS", // Alias
  "SILABS"
};

static const aoa_cte_type_t cte_mode_enum[] = {
  AOA_CTE_TYPE_CONN,
  AOA_CTE_TYPE_CONN_LESS,
  AOA_CTE_TYPE_CONN_LESS,
  AOA_CTE_TYPE_SILABS
};

#define CTE_MODE_COUNT    (sizeof(cte_mode_str) / sizeof(cte_mode_str[0]))

sl_status_t aoa_parse_cte_mode_from_string(char *str, aoa_cte_type_t *cte_mode)
{
  if ((str == NULL) || (cte_mode == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }
  for (size_t i = 0; i < CTE_MODE_COUNT; i++) {
    if (strcasecmp(str, cte_mode_str[i]) == 0) {
      *cte_mode = cte_mode_enum[i];
      return SL_STATUS_OK;
    }
  }
  return SL_STATUS_INVALID_PARAMETER;
}

sl_status_t aoa_parse_cte_mode_to_string(aoa_cte_type_t cte_mode, char **str)
{
  if (str == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  for (size_t i = 0; i < CTE_MODE_COUNT; i++) {
    if (cte_mode == cte_mode_enum[i]) {
      *str = (char *)cte_mode_str[i];
      return SL_STATUS_OK;
    }
  }
  return SL_STATUS_INVALID_PARAMETER;
}
