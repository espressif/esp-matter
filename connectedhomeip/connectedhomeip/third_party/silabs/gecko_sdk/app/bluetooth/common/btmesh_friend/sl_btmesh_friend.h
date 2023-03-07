/***************************************************************************//**
 * @file
 * @brief Friend feature header
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

#ifndef SL_BTMESH_FRIEND_H
#define SL_BTMESH_FRIEND_H

#include "sl_btmesh_api.h"

/***************************************************************************//**
 * @defgroup friend Friend Component
 * @brief Friend feature Implementation
 * This component implements Friend feature.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup friend
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Initialize FRIEND functionality. This function is called automatically by the
 * FRIEND component.
 *
 ******************************************************************************/
void sl_btmesh_friend_feature_init(void);

/***************************************************************************//**
 *  Handling of mesh friend events.
 *
 *  @param[in] evt  Pointer to incoming friend event.
 ******************************************************************************/
void sl_btmesh_friend_on_event(sl_btmesh_msg_t *evt);

/***************************************************************************//**
 * Called when the Friend Node establishes friendship with another node.
 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 *
 * @param[in] netkey_index Index of the network key used in friendship
 * @param[in] lpn_address Low Power Node address
 ******************************************************************************/
void sl_btmesh_friend_on_friendship_established(uint16_t netkey_index,
                                                uint16_t lpn_address);

/***************************************************************************//**
 * Called when the friendship that was successfully established with a Low Power
 * Node has been terminated.
 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 *
 * @param[in] netkey_index Index of the network key used in friendship
 * @param[in] lpn_address Low Power Node address
 * @param[in] reason Reason for friendship termination
 ******************************************************************************/
void sl_btmesh_friend_on_friendship_terminated(uint16_t netkey_index,
                                               uint16_t lpn_address,
                                               uint16_t reason);

/** @} (end addtogroup friend) */

#endif /* SL_BTMESH_FRIEND_H */
