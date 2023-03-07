/***************************************************************************//**
 * @file
 * @brief IO Stream CPC Component.
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

#ifndef SL_IOSTREAM_CPC_H
#define SL_IOSTREAM_CPC_H

#include "sl_iostream.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup iostream
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup iostream_cpc I/O Stream CPC
 * @brief I/O Stream CPC
 * @details
 * ## Overview
 *
 *   The IO Stream module can be used over a co-processor communication (CPC)
 *   link. The IO Stream CPC module will make use of the CPC endpoint dedicated
 *   to CLI.
 *
 *   The IO Stream component can be combined with the CLI component to create
 *   a command line application on your secondary device.
 *
 *   On the host side, the python script ***cpc_iostream_bridge.py*** is available
 *   under the script folder
 *   (<a href="https ://github.com/SiliconLabs/cpc-daemon/tree/main/doc/iostream_bridge.md">
 *   see Github repository</a>).
 *   This script enables you to bridge the CPC host CLI endpoint over to a network
 *   socket and thus allows you to communicate with your secondary CLI application
 *   through a telnet terminal. See host documentation for more details.
 *   **TODO Link to github doc**
 *
 * ## Initialization
 *
 *   The cpc_iostream component will open the CPC CLI endpoint during its
 *   initialization process.
 *
 *   The stream sets itself as the default stream at the end of the initialization
 *   function. You must reconfigure the default interface if you have multiple streams
 *   in your project else the last stream initialized will be set as the system default
 *   stream.
 *
 * @{
 ******************************************************************************/

extern sl_iostream_t *sl_iostream_cpc_handle;                       ///< sl_iostream_cpc_handle
extern sl_iostream_instance_info_t sl_iostream_instance_cpc_info;   ///< sl_iostream_instance_cpc_info

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * CPC Stream init.
 *
 * @return  Status result
 ******************************************************************************/
sl_status_t sl_iostream_cpc_init(void);

/** @} (end addtogroup iostream_cpc) */
/** @} (end addtogroup iostream) */

#ifdef __cplusplus
}
#endif

#endif // SL_IOSTREAM_CPC_H
