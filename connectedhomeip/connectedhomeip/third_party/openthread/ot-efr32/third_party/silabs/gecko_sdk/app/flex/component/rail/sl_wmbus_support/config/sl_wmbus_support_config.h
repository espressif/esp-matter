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
#ifndef SL_WMBUS_SUPPORT_CONFIG_H_
#define SL_WMBUS_SUPPORT_CONFIG_H_

#include "sl_wmbus_support.h"
// <<< Use Configuration Wizard in Context Menu >>>

// <h> Available Wireless M-bus mode options for Wireless M-bus Meter app
// <o SL_WMBUS_TYPE> Default Wireless M-bus mode
// <WMBUS_MODE_S=> S mode
// <WMBUS_MODE_T_METER=> T mode, Meter to Other
// <WMBUS_MODE_T_COLLECTOR=> T mode, Other to Meter
// <WMBUS_MODE_C=> C mode
// <WMBUS_MODE_F=> F mode
// <WMBUS_MODE_R2=> R2 mode
// <WMBUS_MODE_N_24=> N mode, 2.4kbps (channel 2a and 2b)
// <WMBUS_MODE_N_48=> N mode, 4.8kbps (channel 1a, 1b, 3a and 3b)
// <WMBUS_MODE_N_192=> N mode, 19.2kbps (channel 0)
// <i> Default: WMBUS_MODE_T_METER
#define SL_WMBUS_TYPE  WMBUS_MODE_T_METER
// </h>

// <h> Available Wireless M-bus mode options for Wireless M-bus Meter app
// <o SL_WMBUS_ACCESSIBLILTY> Default Wireless M-bus Accessibility
// <WMBUS_ACCESSIBILITY_NO_ACCESS=> No access (unidirectional)
// <WMBUS_ACCESSIBILITY_TEMPORARY_NO_ACCESS=> Bidirectional, temporary no access
// <WMBUS_ACCESSIBILITY_LIMITED_ACCESS=> Bidirectional, limited access (short time after TX)
// <WMBUS_ACCESSIBILITY_UNLIMITED_ACCESS=> Bidirectional, unlimited access
// <i> Default: WMBUS_ACCESSIBILITY_LIMITED_ACCESS
#define SL_WMBUS_ACCESSIBLILTY  WMBUS_ACCESSIBILITY_LIMITED_ACCESS
// </h>

#endif /* SL_WMBUS_SUPPORT_CONFIG_H_ */
