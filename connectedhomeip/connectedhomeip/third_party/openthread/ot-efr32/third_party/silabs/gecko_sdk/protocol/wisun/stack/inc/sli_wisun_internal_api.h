/***************************************************************************//**
 * @file sl_wisun_internal_api.h
 * @brief Wi-SUN internal API
 * @warning This file is not intended for application use and may change
 *          without notice
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

#ifndef SL_WISUN_INTERNAL_API_H
#define SL_WISUN_INTERNAL_API_H

#include "sl_status.h"
#include "rail.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************//**
 * @addtogroup SL_WISUN_INTERNAL_API Wi-SUN internal API
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * @brief Get the initialized rail instance.
 * @param[out] handle rail handle
 * @return SL_STATUS_OK if successful, an error code otherwise
 *****************************************************************************/
sl_status_t sli_wisun_get_rail_handle(RAIL_Handle_t *handle);

/** @} (end SL_WISUN_INTERNAL_API) */

#ifdef __cplusplus
}
#endif

#endif  // SL_WISUN_INTERNAL_API_H
