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
#ifndef SL_BTMESH_SCENE_CLIENT_CONFIG_H
#define SL_BTMESH_SCENE_CLIENT_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Scene Client configuration

// <o SL_BTMESH_SCENE_CLIENT_RETRANSMISSION_COUNT_CFG_VAL> Scene model restransmission count
// <i> Default: 3
// <i> Scene model restransmission count (How many times Scene model messages are to be sent out for reliability).
#define SL_BTMESH_SCENE_CLIENT_RETRANSMISSION_COUNT_CFG_VAL   (3)

// <o SL_BTMESH_SCENE_CLIENT_RETRANSMISSION_TIMEOUT_CFG_VAL> Scene model restransmission timeout
// <i> Default: 50
// <i> Scene model restransmission timeout.
#define SL_BTMESH_SCENE_CLIENT_RETRANSMISSION_TIMEOUT_CFG_VAL   (50)

// <e SL_BTMESH_SCENE_CLIENT_LOGGING_CFG_VAL> Enable Logging
// <i> Default: 1
// <i> Enable / disable Logging for Scene Client model specific messages for this component.
#define SL_BTMESH_SCENE_CLIENT_LOGGING_CFG_VAL   (1)

// <s.128 SL_BTMESH_SCENE_CLIENT_LOGGING_RECALL_CFG_VAL> Log text when recalling a new scene.
// <i> Set Log text in case the a new scene is recalled
#define SL_BTMESH_SCENE_CLIENT_LOGGING_RECALL_CFG_VAL "Recall scene number %u\r\n"

// <s.128 SL_BTMESH_SCENE_CLIENT_LOGGING_RECALL_FAIL_CFG_VAL> Log text when recalling a new scene fails.
// <i> Set Log text in case the a  scene recall fails
#define SL_BTMESH_SCENE_CLIENT_LOGGING_RECALL_FAIL_CFG_VAL "Scene recall failed\r\n"

// <s.128 SL_BTMESH_SCENE_CLIENT_LOGGING_RECALL_SUCCESS_CFG_VAL> Log text when recalling a scene recall is successful.
// <i> Set Log text a scene recall is successful.
#define SL_BTMESH_SCENE_CLIENT_LOGGING_RECALL_SUCCESS_CFG_VAL "Scene request sent, trid = %u, delay = %u\r\n"

// </e>

// </h>

// <<< end of configuration section >>>

#endif // SL_BTMESH_SCENE_CLIENT_CONFIG_H
