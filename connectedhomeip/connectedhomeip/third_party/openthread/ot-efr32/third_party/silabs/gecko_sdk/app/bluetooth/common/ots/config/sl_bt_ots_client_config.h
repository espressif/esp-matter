/***************************************************************************//**
 * @file
 * @brief OTS Object Client general configuration
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

#ifndef SL_BT_OTS_CLIENT_CONFIG_H
#define SL_BT_OTS_CLIENT_CONFIG_H

#include "sl_bluetooth_connection_config.h"

// <<< Use Configuration Wizard in Context Menu >>>

// <q SL_BT_OTS_CLIENT_CONFIG_READ_REQUEST_QUEUE_SIZE> Read request queue size per connection
// <i> Size of the read request queue.
#define SL_BT_OTS_CLIENT_CONFIG_READ_REQUEST_QUEUE_SIZE                            5

// <q SL_BT_OTS_CLIENT_CONFIG_WRITE_REQUEST_QUEUE_SIZE> Write request queue size per connection
// <i> Size of the write request queue.
#define SL_BT_OTS_CLIENT_CONFIG_WRITE_REQUEST_QUEUE_SIZE                           1

// <q SL_BT_OTS_CLIENT_CONFIG_WRITE_REQUEST_DATA_SIZE> Maximum write request data length
// <i> Maximum length of the write request content.
#define SL_BT_OTS_CLIENT_CONFIG_WRITE_REQUEST_DATA_SIZE                            255

// <<< end of configuration section >>>

#endif // SL_BT_OTS_CLIENT_CONFIG_H
