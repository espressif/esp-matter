/***************************************************************************//**
 * @file
 * @brief NCP reset module.
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

#ifndef NCP_RESET_H
#define NCP_RESET_H

#include "sl_status.h"
#include "sl_bt_api.h"

/**
 * Bluetooth stack event handler.
 *
 * @param [in] evt Event from the Bluetooth stack.
 * @retval SL_STATUS_OK NCP target is operational.
 * @retval SL_STATUS_BUSY NCP target reset is pending.
 */
sl_status_t ncp_reset_on_event(sl_bt_msg_t *evt);

/**
 * Send system reset request to ncp target.
 */
void ncp_reset(void);

#endif // NCP_RESET_H
