/***************************************************************************//**
 * @file
 * @brief Low Power Node feature header
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

#ifndef SL_BTMESH_LPN_H
#define SL_BTMESH_LPN_H

#include "sl_btmesh_api.h"

/***************************************************************************//**
 * @defgroup lpn Low Power Node Component
 * @brief Low Power Node feature Implementation
 * This component implements Low Power Node feature.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup lpn
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Initialize LPN functionality. This function is called automatically by the
 * LPN component.
 *
 ******************************************************************************/
void sl_btmesh_lpn_feature_init(void);

/***************************************************************************//**
 * Deinitialize LPN functionality. This function is called automatically by the
 * LPN component.
 ******************************************************************************/
void sl_btmesh_lpn_feature_deinit(void);

/***************************************************************************//**
 *  Handling of mesh lpn events.
 *
 *  @param[in] pEvt  Pointer to incoming lpn event.
 ******************************************************************************/
void sl_btmesh_lpn_on_event(sl_btmesh_msg_t *pEvt);

/***************************************************************************//**
 * Called when the Low Power Node is initialized.
 *
 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 ******************************************************************************/
void sl_btmesh_lpn_on_init(void);

/***************************************************************************//**
 * Called when the Low Power Node is deinitialized.
 *
 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 ******************************************************************************/
void sl_btmesh_lpn_on_deinit(void);

/***************************************************************************//**
 * Called when the Low Power Node establishes friendship with another node
 *
 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 *
 * @param[in] friend_address Node address of the new friend
 ******************************************************************************/
void sl_btmesh_lpn_on_friendship_established(uint16_t friend_address);

/***************************************************************************//**
 * Called when the friendship establishment attempt of Low Power Node fails
 *
 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 *
 * @param[in] reason Reason for friendship establishment failure
 ******************************************************************************/
void sl_btmesh_lpn_on_friendship_failed(uint16_t reason);

/***************************************************************************//**
 * Called when friendship that was successfully established has been terminated
 *
 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 *
 * @param[in] reason Reason for friendship termination
 ******************************************************************************/
void sl_btmesh_lpn_on_friendship_terminated(uint16_t reason);

/** @} (end addtogroup lpn) */

#endif /* SL_BTMESH_LPN_H */
