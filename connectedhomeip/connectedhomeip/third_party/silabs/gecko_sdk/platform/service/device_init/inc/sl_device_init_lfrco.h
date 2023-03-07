/***************************************************************************//**
 * @file
 * @brief Device initialization for LFRCO.
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
#ifndef SL_DEVICE_INIT_LFRCO_H
#define SL_DEVICE_INIT_LFRCO_H

#include "sl_status.h"

/**
 * @addtogroup device_init
 * @{
 * @addtogroup device_init_lfrco LFRCO Initialization
 * @brief Initialize the LFRCO oscillator.
 * @details
 * Initialize the low frequency RC oscillator. On Series 2 devices with
 * precision mode support, the precision mode is configured in the configuration
 * header `sl_device_init_lfrco_config.h`. On other devices, there is nothing
 * to configure.
 *
 * If using Simplicity Studio, this configuration header is also configurable
 * through the Project Configurator, by selecting the "Device Init: LFRCO"
 * configuration component.
 * @{
 */

/**
 * Initialize LFRCO
 *
 * @details
 * Initialize the low frequency RC oscillator. On devices that support
 * high precision mode, this function configures the precision mode based
 * on the configuration option SL_DEVICE_INIT_LFRCO_PRECISION.
 *
 * @return Status code
 * @retval SL_STATUS_OK LFRCO initialized successfully
 */
sl_status_t sl_device_init_lfrco(void);

/**
 * @} device_init_lfrco
 * @} device_init
 */

#endif // SL_DEVICE_INIT_LFRCO_H
