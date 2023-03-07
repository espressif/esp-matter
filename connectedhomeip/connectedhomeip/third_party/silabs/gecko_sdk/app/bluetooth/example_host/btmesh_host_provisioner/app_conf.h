/***************************************************************************//**
 * @file
 * @brief BT Mesh Host Configurator Application Code
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

#ifndef APP_CONF_H
#define APP_CONF_H

#include <stdint.h>
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup btmesh_host_provisioner BT Mesh Host Provisioner Example
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup btmesh_host_provisioner_conf \
 * BT Mesh Host Provisioner Example - Node configuration
 * @{
 ******************************************************************************/

/// Default application key index
#define APP_CONF_APPKEY_INDEX     0

/***************************************************************************//**
 * Start node configuration procedure
 *
 * @param[in] netkey_index Network key used to encrypt the config requests
 * @param[in] server_address Destination node primary element address
 * @returns Status of node configuration start.
 * @retval SL_STATUS_OK If node configuration is started.
 * @retval SL_STATUS_INVALID_STATE If BT Mesh configurator is not initialized
 *   or node configuration is already in progress.
 * @retval SL_STATUS_ALLOCATION_FAILED If allocation of job or task is failed.
 ******************************************************************************/
sl_status_t app_conf_start_node_configuration(uint16_t netkey_index,
                                              uint16_t server_address);

/***************************************************************************//**
 * Print node information by server address
 *
 * @param[in] server_address Primary element address of the node
 *
 * If DCD of the node is not available in the BT Mesh database then the DCD is
 * queried first and it is printed afterwards otherwise the node information is
 * printed based on the cached data.
 *
 * The @ref app_on_nodeinfo_end and @ref app_ui_on_nodeinfo_end functions are
 * called when the node information procedure ends to notify the main application
 * logic.
 ******************************************************************************/
void app_conf_print_nodeinfo_by_addr(uint16_t server_address);

/***************************************************************************//**
 * Print node information by uuid
 *
 * @param[in] uuid UUID of the node
 *
 * If DCD of the node is not available in the BT Mesh database then the DCD is
 * queried first and it is printed afterwards otherwise the node information is
 * printed based on the cached data.
 *
 * The @ref app_on_nodeinfo_end and @ref app_ui_on_nodeinfo_end functions are
 * called when the node information procedure ends to notify the main application
 * logic.
 ******************************************************************************/
void app_conf_print_nodeinfo_by_uuid(uuid_128 uuid);

/***************************************************************************//**
 * Print node information by list identifier
 *
 * @param[in] id List ID of the node
 *
 * If DCD of the node is not available in the BT Mesh database then the DCD is
 * queried first and it is printed afterwards otherwise the node information is
 * printed based on the cached data.
 *
 * The @ref app_on_nodeinfo_end and @ref app_ui_on_nodeinfo_end functions are
 * called when the node information procedure ends to notify the main application
 * logic.
 ******************************************************************************/
void app_conf_print_nodeinfo_by_id(uint16_t id);

/** @} (end addtogroup btmesh_host_provisioner_conf) */
/** @} (end addtogroup btmesh_host_provisioner) */

#ifdef __cplusplus
};
#endif

#endif // APP_CONF_H
