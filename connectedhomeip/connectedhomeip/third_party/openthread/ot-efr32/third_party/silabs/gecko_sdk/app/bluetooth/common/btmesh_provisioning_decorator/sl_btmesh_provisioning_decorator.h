/***************************************************************************//**
 * @file
 * @brief provisiong_decorator.h
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_BTMESH_PROVISIONING_DECORATOR_H
#define SL_BTMESH_PROVISIONING_DECORATOR_H

/***************************************************************************//**
 * Callbacks
 ******************************************************************************/

/***************************************************************************//**
 * Called at node initialization time to provide provisioning information
 *
 * @param[in] provisioned  true: provisioned, false: unprovisioned
 * @param[in] address      Unicast address of the primary element of the node.
                           Ignored if unprovisioned.
 * @param[in] iv_index     IV index for the first network of the node
                           Ignored if unprovisioned.
 *
 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 ******************************************************************************/
void sl_btmesh_on_provision_init_status(bool provisioned,
                                        uint16_t address,
                                        uint32_t iv_index);

/***************************************************************************//**
 * Called when the Provisioning starts
 *
 * @param[in] result  Result code. 0: success, non-zero: error
 *
 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 ******************************************************************************/
void sl_btmesh_on_node_provisioning_started(uint16_t result);

/***************************************************************************//**
 * Called when the Provisioning finishes successfully
 *
 * @param[in] address      Unicast address of the primary element of the node.
                           Ignored if unprovisioned.
 * @param[in] iv_index     IV index for the first network of the node
                           Ignored if unprovisioned.

 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 ******************************************************************************/
void sl_btmesh_on_node_provisioned(uint16_t address,
                                   uint32_t iv_index);

/***************************************************************************//**
 * Called when the Provisioning fails
 *
 * @param[in] result  Result code. 0: success, non-zero: error
 *
 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 ******************************************************************************/
void sl_btmesh_on_node_provisioning_failed(uint16_t result);

/***************************************************************************//**
 * Functions which are automatically called when the component is selected
 ******************************************************************************/
/***************************************************************************//**
 * Handling of Provisioning Decorator stack events.
 *
 * @param[in] evt  Pointer to the event type
 ******************************************************************************/
void sl_btmesh_handle_provisioning_decorator_event(sl_btmesh_msg_t *evt);

#endif // SL_BTMESH_PROVISIONING_DECORATOR_H
