/***************************************************************************//**
 * @file
 * @brief CLI simple password protection.
 * @version x.y.z
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

#include "sl_cli_types.h"

#ifndef SL_CLI_SIMPLE_PASSWORD_H
#define SL_CLI_SIMPLE_PASSWORD_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup cli
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief  Initialize module
 *
 * @return  Initialization status .
 ******************************************************************************/
sl_status_t sl_cli_simple_password_init(void);

/***************************************************************************//**
 * @brief  Logout connected user from an instance.
 *
 * @param[in] handle  A handle to the CLI.
 *
 * @return  Initialization status.
 ******************************************************************************/
sl_status_t sl_cli_simple_password_logout(sl_cli_handle_t handle);

/***************************************************************************//**
 * @brief   Set a new password
 *
 * @param[in] new_password  Password string
 *
 * @return  Operation status.
 ******************************************************************************/
sl_status_t sl_cli_set_simple_password(char *new_password);

/***************************************************************************//**
 * @brief Destroy password key and force to configure a new password
 *
 * @return  Operation status.
 ******************************************************************************/
sl_status_t sl_cli_simple_password_destroy_key(void);

/***************************************************************************//**
 * @brief Reset security warning flag
 *
 * @return  Operation status.
 ******************************************************************************/
sl_status_t sl_cli_reset_security_warning_flag(void);

/** @} (end addtogroup cli) */

#ifdef __cplusplus
}
#endif

#endif // SL_CLI_SIMPLE_PASSWORD_H
