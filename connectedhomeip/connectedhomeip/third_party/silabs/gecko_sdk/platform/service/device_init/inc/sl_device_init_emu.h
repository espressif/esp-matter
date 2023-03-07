/***************************************************************************//**
 * @file
 * @brief Device initialization for EMU.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SL_DEVICE_INIT_EMU_H
#define SL_DEVICE_INIT_EMU_H

#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup device_init
 * @{
 * @addtogroup device_init_emu EMU Initialization
 * @brief Initialize the Energy Management Unit.
 * @details
 * Initializes the Energy Management Unit by configuring Energy Mode 4 (EM4)
 * according to settings in the configuration header
 * `sl_device_init_emu_config.h`.
 *
 * On Series 2 devices, debugging in EM2 is enabled by default by ensuring
 * that power to the debug power domain is sustained on EM2 entry.
 * Debugging in EM2 can be disabled through the configuration header. If
 * using Simplicity Studio, this header can also be configured using the
 * Project Configurator by selecting the "Device Init: EMU" configuration
 * component.
 *
 * @note Voltage scaling for Energy Modes 0 to 3 are configured by the
 * @ref power_manager.
 * @{
 */

/**
 * Initialize EMU
 *
 * @details
 * Initializes the Energy Management Unit by configuring Energy Mode 4 (EM4)
 * according to settings in the configuration header
 * `sl_device_init_emu_config.h`.
 *
 * On Series 2 devices, debugging in EM2 is enabled by default by ensuring
 * that power to the debug power domain is sustained on EM2 entry.
 *
 * @note Voltage scaling for Energy Modes 0 to 3 are configured by the
 * @ref power_manager.
 *
 * @return Status code
 * @retval SL_STATUS_OK EMU initialized successfully
 */
sl_status_t sl_device_init_emu(void);

/**
 * @} device_init_emu
 * @} device_init
 */

#ifdef __cplusplus
}
#endif

#endif // SL_DEVICE_INIT_EMU_H
