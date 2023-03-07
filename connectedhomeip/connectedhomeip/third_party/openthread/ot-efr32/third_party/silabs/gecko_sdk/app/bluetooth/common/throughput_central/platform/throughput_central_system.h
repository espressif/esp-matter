/***************************************************************************//**
 * @file
 * @brief Throughput test application central mode - platform includes
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
#ifndef THROUGHPUT_CENTRAL_INCLUDE_H
#define THROUGHPUT_CENTRAL_INCLUDE_H

#include "throughput_central_config.h"

#if defined (__arm__) || defined (__ICCARM__)

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT
#ifdef SL_CATALOG_CLI_PRESENT
#include "sl_cli.h"
#endif // SL_CATALOG_CLI_PRESENT
#ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
#include "throughput_ui.h"
#endif // SL_CATALOG_THROUGHPUT_UI_PRESENT

#else

#include "sl_bt_ncp_host.h"

/**************************************************************************//**
 * Check if RSSI timer expires
 *****************************************************************************/
void timer_step_rssi(void);

#endif

#endif
