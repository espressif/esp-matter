/***************************************************************************//**
 * @file
 * @brief Device initialization for clocks.
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
#ifndef SL_DEVICE_INIT_CLOCKS_H
#define SL_DEVICE_INIT_CLOCKS_H

#include "sl_status.h"

/**
 * @addtogroup device_init Device Initialization
 * @brief Device Initialization
 * @details
 * The Device Initialization functions assist in bringing the device from the
 * reset state to an initialized state by applying errata fixes, starting
 * oscillators and configuring certain power control features.
 *
 * The top-level Device Init component will automatically add required and
 * recommended initialization components to the platform init event handled
 * by the System Init component based on the selected hardware.
 *
 *  - Errata fixes are applied on all devices
 *  - EMU initialization is added on all devices
 *  - Clock tree initialization is added on all devices
 *  - NVIC initialization is added on all devices
 *  - DCDC initialization is added on devices with a DCDC converter
 *  - HFXO initialization is added on Silicon Labs modules and development
 *    boards with HFXO
 *  - LFXO initialization is added on Silicon Labs modules and development
 *    boards with LFXO
 *  - HFRCO initialization is added on EFR32xG21 devices to set the core
 *    frequency to 80 MHz (oscillator band is configurable in the component)
 *  - LFRCO initialization is added on EFR32xG22 devices when Bluetooth is
 *    present to enable high precision mode (precision mode is configurable
 *    in the component)
 *
 * If the individual initialization components are not sufficiently configurable,
 * or if any of these default actions are not desired, the top-level device
 * initialization component can be removed from the project and replaced by the
 * subset of desired initialization components.
 *
 * @{
 * @addtogroup device_init_clocks Clock Initialization
 * @brief Initialize the clock tree.
 * @details
 *
 * Automatically configures the clock tree to use the appropriate high- and
 * low-frequency clock sources depending on which other device initialization
 * components are present.
 *
 * ### High Frequency Clocks
 * If the **DPLL** or **HFRCO** device init components are present, the
 * **HFRCO** oscillator is used as the clock source for high frequency clock
 * trees. Otherwise, if the **HFXO** device init component is present, the
 * **HFXO** oscillator is used. If no high frequency oscillator device init
 * component is present, the **HFRCO** oscillator is used with its default
 * settings.
 *
 * ### Low Frequency Clocks
 * If the **LFXO** device init component is present, the **LFXO** oscillator is
 * used as the clock source for low frequency clock trees.
 * Otherwise, the **LFRCO** oscillator is used.
 *
 * @{
 */

/**
 * Configure the clock tree and enable clocks
 *
 * @details
 * Automatically configures the clock tree to use the appropriate HF and LF
 * clock sources depending on which other device initialization components are
 * present.
 *
 * @return Status code
 * @retval SL_STATUS_OK Clock tree configured successfully
 */
sl_status_t sl_device_init_clocks(void);

/**
 * @} device_init_clocks
 * @} device_init
 */

#endif // SL_DEVICE_INIT_CLOCKS_H
