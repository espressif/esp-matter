/***************************************************************************//**
 * @file
 * @brief Device initialization for HFRCO.
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
#ifndef SL_DEVICE_INIT_HFRCO_H
#define SL_DEVICE_INIT_HFRCO_H

#include "sl_status.h"

/**
 * @addtogroup device_init
 * @{
 * @addtogroup device_init_hfrco HFRCO Initialization
 * @brief Initialize the HFRCO oscillator
 * @details
 * Configure the frequency band of the HFRCO. The configuration define
 * SL_DEVICE_INIT_HFRCO_BAND is used to configure the frequency.
 * If using Simplicity Studio, this configuration header is also configurable
 * through the Project Configurator, by selecting the "Device Init: HFRCO" configuration
 * component.
 * @{
 */

/**
 * Initialize HFRCO
 *
 * @details
 * Configure the frequency band of the HFRCO. The configuration define
 * SL_DEVICE_INIT_HFRCO_BAND is used to configure the frequency.
 *
 * @return Status code
 * @retval SL_STATUS_OK HFRCO initialized successfully
 */
sl_status_t sl_device_init_hfrco(void);

/**
 * @} device_init_hfrco
 * @} device_init
 */

#endif // SL_DEVICE_INIT_HFRCO_H
