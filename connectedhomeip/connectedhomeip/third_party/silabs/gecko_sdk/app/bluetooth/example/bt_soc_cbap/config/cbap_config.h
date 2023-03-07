/***************************************************************************//**
 * @file
 * @brief SoC Certificate Based Authentication and Pairing Configuration
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

#ifndef CBAP_CONFIG_H
#define CBAP_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <o ROLE> Role
//   <SL_BT_CBAP_ROLE_PERIPHERAL=> Peripheral
//   <SL_BT_CBAP_ROLE_CENTRAL=> Central
// <i> Default: Peripheral
#define ROLE        SL_BT_CBAP_ROLE_PERIPHERAL

// <e ADDR_ENABLE> Scanning configuration
// <i> Default: 0
#define ADDR_ENABLE 0

// <s.17 ADDR> Target Address
// <i> Can only take effect if the central role is selected.
// <i> Default: "00:00:00:00:00:00"
#define ADDR        "00:00:00:00:00:00"

// </e>
// <<< end of configuration section >>>

#endif // CBAP_CONFIG_H
