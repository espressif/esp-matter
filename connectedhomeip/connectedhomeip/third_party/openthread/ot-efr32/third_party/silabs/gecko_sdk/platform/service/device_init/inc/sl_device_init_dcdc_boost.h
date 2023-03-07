/***************************************************************************//**
 * @file
 * @brief Device initialization for DC/DC Boost converter.
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
#ifndef SL_DEVICE_INIT_DCDC_BOOST_H
#define SL_DEVICE_INIT_DCDC_BOOST_H

#include "sl_status.h"

/**
 * @addtogroup device_init
 * @{
 * @addtogroup device_init_dcdc_boost DCDC Boost Initialization
 * @brief Initialize the DC-DC Boost converter.
 * @details
 * Configures the DC-DC Boost converter. If the DC-DC Boost converter is not to be
 * used, the configuration option `SL_DEVICE_INIT_DCDC_BOOST_ENABLE` can be disabled,
 * and the converter will be powered off.
 *
 * See **AN0948 Power Configurations and DC-DC** for further details about DC-DC Boost
 * converter configuration and operation.
 *
 *  - Series 2: [AN0948.2](https://www.silabs.com/documents/public/application-notes/an0948.2-efr32-series-2-power-configurations-and-dcdc.pdf)
 * @{
 */

/**
 * Initialize DCDC Boost
 *
 * @details
 * Configure and start the DCDC Boost
 *
 * @return Status code
 * @retval SL_STATUS_OK DC-DC Boost converter initialized successfully
 */
sl_status_t sl_device_init_dcdc_boost(void);

/**
 * @} device_init_dcdc_boost
 * @} device_init
 */

#endif // SL_DEVICE_INIT_DCDC_BOOST_H
