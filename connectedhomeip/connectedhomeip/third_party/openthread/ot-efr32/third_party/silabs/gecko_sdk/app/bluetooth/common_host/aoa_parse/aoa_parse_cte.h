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

#ifndef AOA_PARSE_CTE_H
#define AOA_PARSE_CTE_H

#include "sl_status.h"
#include "aoa_cte.h"

/**
 * Convert string to CTE mode.
 *
 * @param[in] str Input string.
 * @param[out] cte_mode CTE mode.
 * @return Status ot the conversion.
 * @retval SL_STATUS_OK The conversion was successful.
 * @retval SL_STATUS_NULL_POINTER Invalid null pointer received as argument.
 * @retval SL_STATUS_INVALID_PARAMETER The input was invalid.
 */
sl_status_t aoa_parse_cte_mode_from_string(char *str, aoa_cte_type_t *cte_mode);

/**
 * Convert CTE mode to string.
 *
 * @param[in] cte_mode CTE mode.
 * @param[out] str Output string.
 * @return Status ot the conversion.
 * @retval SL_STATUS_OK The conversion was successful.
 * @retval SL_STATUS_NULL_POINTER Invalid null pointer received as argument.
 * @retval SL_STATUS_INVALID_PARAMETER The input was invalid.
 */
sl_status_t aoa_parse_cte_mode_to_string(aoa_cte_type_t cte_mode, char **str);

#endif // AOA_PARSE_CTE_H
