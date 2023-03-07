/***************************************************************************//**
 * @file
 * @brief
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
#ifndef SL_BTMESH_PROVISIONING_DECORATOR_CONFIG_H
#define SL_BTMESH_PROVISIONING_DECORATOR_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Provisioning decorator configuration

// <e SL_BTMESH_PROVISIONING_DECORATOR_LOGGING_CFG_VAL> Enable Logging
// <i> Default: 1
// <i> Enable or disable Logging for Provisioning Decorator specific messages for this component.
#define SL_BTMESH_PROVISIONING_DECORATOR_LOGGING_CFG_VAL   (1)

// </e>

// <o SL_BTMESH_PROVISIONING_DECORATOR_RESTART_TIMER_TIMEOUT_CFG_VAL> Timeout for system restart after provisioning fails
#define SL_BTMESH_PROVISIONING_DECORATOR_RESTART_TIMER_TIMEOUT_CFG_VAL          (2000)

// </h>

// <<< end of configuration section >>>

#endif // SL_BTMESH_PROVISIONING_DECORATOR_CONFIG_H
