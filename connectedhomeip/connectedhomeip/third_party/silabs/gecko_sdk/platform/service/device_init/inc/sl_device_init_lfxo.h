/***************************************************************************//**
 * @file
 * @brief Device initialization for LFXO.
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
#ifndef SL_DEVICE_INIT_LFXO_H
#define SL_DEVICE_INIT_LFXO_H

#include "sl_status.h"

/**
 * @addtogroup device_init
 * @{
 * @addtogroup device_init_lfxo LFXO Initialization
 * @brief Initialize the LFXO oscillator.
 * @details
 * Configure the low frequency crystal oscillator using settings in the
 * configuration header `sl_device_init_lfxo_config.h`.
 *
 * The precision of the oscillator can be indicated using the configuration
 * option `SL_DEVICE_INIT_LFXO_PRECISION`. This is used by certain wireless
 * stacks to optimize wakeup time for scheduled events (such as Bluetooth
 * advertising).
 *
 * If using Simplicity Studio, this configuration header is also configurable
 * through the Project Configurator by selecting the "Device Init: LFXO"
 * configuration component.
 * @{
 */

/**
 * Initialize LFXO
 *
 * @details
 * Configure the low frequency crystal oscillator using settings in the
 * configuration header `sl_device_init_lfxo_config.h`.
 *
 * The precision of the oscillator can be indicated using the configuration
 * option `SL_DEVICE_INIT_LFXO_PRECISION`. This is used by certain wireless
 * stacks to optimize wakeup time for scheduled events (such as Bluetooth
 * advertising).
 *
 * @return Status code
 * @retval SL_STATUS_OK LFXO initialized successfully
 */
sl_status_t sl_device_init_lfxo(void);

/**
 * @} device_init_lfxo
 * @} device_init
 */

#endif // SL_DEVICE_INIT_LFXO_H
