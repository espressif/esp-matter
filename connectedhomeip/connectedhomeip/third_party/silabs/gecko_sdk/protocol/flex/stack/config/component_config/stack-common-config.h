/***************************************************************************//**
 * @brief Connect Stack Common component configuration header.
 *
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

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Connect Stack Common configuration

// <o EMBER_HEAP_SIZE> Minimum Heap Size in bytes<250-10000:2>
// <i> Default: 4000
// <i> The minimum size in bytes of the memory heap.
#define EMBER_HEAP_SIZE             (4000)

// <o EMBER_RADIO_CCA_THRESHOLD> MAC CCA Threshold in dBm
// <i> Default: -65
// <i> MAC CCA Threshold value in dBm.
#define EMBER_RADIO_CCA_THRESHOLD   (-65)

// <o EMBER_MAC_ACK_TIMEOUT_MS> MAC ACK timeout in milliseconds <1-65>
// <i> Default: 25
// <i> This parameter should be fine-tuned to reduce energy consumption for sleepy devices and depends on the data rate of the PHY configuration used.
#define EMBER_MAC_ACK_TIMEOUT_MS    (25)

// <o EMBER_SECURITY_SHORT_TO_LONG_MAPPING_TABLE_SIZE> Size of the short-to-long address mapping table <1-64>
// <i> Default: 10
// <i> This table is only used for EMBER_MAC_MODE_DEVICE and EMBER_MAC_MODE_SLEEPY_DEVICE, but the memory will be allocated on all device types.
#define EMBER_SECURITY_SHORT_TO_LONG_MAPPING_TABLE_SIZE (10)

// </h>

// <<< end of configuration section >>>
