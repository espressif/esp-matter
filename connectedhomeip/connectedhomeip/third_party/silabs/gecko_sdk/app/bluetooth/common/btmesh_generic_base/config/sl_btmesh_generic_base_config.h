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
#ifndef SL_BTMESH_GENERIC_BASE_CONFIG_H
#define SL_BTMESH_GENERIC_BASE_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Generic Base configuration

// <o SL_BTMESH_GENERIC_BASE_INCREMENT_CFG_VAL> Register size increment <0-10>
// <i> Default: 3
// <i> The dynamically reallocated array will grow in size by this value.
// <i> Setting this value to 0 will disable reallocation.
#define SL_BTMESH_GENERIC_BASE_INCREMENT_CFG_VAL                       3

// </h>

// <h> Generic Models Initialization configuration

// <e SL_BTMESH_GENERIC_BASE_SERVER_CFG_VAL> Enable Generic Server Models
// <i> Default: 0
// <i> Enable Generic Server functionality.
#define SL_BTMESH_GENERIC_BASE_SERVER_CFG_VAL                          0

// <q SL_BTMESH_GENERIC_ON_OFF_SERVER_INIT_CFG_VAL> Generic On/Off Server
// <i> Default: 0
// <i> Initialize Generic On/Off Server.
#define SL_BTMESH_GENERIC_ON_OFF_SERVER_INIT_CFG_VAL                   0

// <q SL_BTMESH_GENERIC_LEVEL_SERVER_INIT_CFG_VAL> Generic Level Server
// <i> Default: 0
// <i> Initialize Generic Level Server.
#define SL_BTMESH_GENERIC_LEVEL_SERVER_INIT_CFG_VAL                    0

// <q SL_BTMESH_GENERIC_DEFAULT_TRANSITION_TIME_SERVER_INIT_CFG_VAL> Generic Default Transition Time Server
// <i> Default: 0
// <i> Initialize Generic Default Transition Time Server.
#define SL_BTMESH_GENERIC_DEFAULT_TRANSITION_TIME_SERVER_INIT_CFG_VAL  0

// <q SL_BTMESH_GENERIC_POWER_ON_OFF_SERVER_INIT_CFG_VAL> Generic Power On/Off Server
// <i> Default: 0
// <i> Initialize Generic Power On/Off Server.
#define SL_BTMESH_GENERIC_POWER_ON_OFF_SERVER_INIT_CFG_VAL             0

// <q SL_BTMESH_GENERIC_POWER_LEVEL_SERVER_INIT_CFG_VAL> Generic Power Level Server
// <i> Default: 0
// <i> Initialize Generic Power Level Server.
#define SL_BTMESH_GENERIC_POWER_LEVEL_SERVER_INIT_CFG_VAL              0

// <q SL_BTMESH_GENERIC_BATTERY_SERVER_INIT_CFG_VAL> Generic Battery Server
// <i> Default: 0
// <i> Initialize Generic Battery Server.
#define SL_BTMESH_GENERIC_BATTERY_SERVER_INIT_CFG_VAL                  0

// <q SL_BTMESH_GENERIC_LOCATION_SERVER_INIT_CFG_VAL> Generic Location Server
// <i> Default: 0
// <i> Initialize Generic Location Server.
#define SL_BTMESH_GENERIC_LOCATION_SERVER_INIT_CFG_VAL                 0

// <q SL_BTMESH_GENERIC_PROPERTY_SERVER_INIT_CFG_VAL> Generic Property Server
// <i> Default: 0
// <i> Initialize Generic Property Server.
#define SL_BTMESH_GENERIC_PROPERTY_SERVER_INIT_CFG_VAL                 0

// <q SL_BTMESH_GENERIC_LIGHTNESS_SERVER_INIT_CFG_VAL> Light Lightness Server
// <i> Default: 0
// <i> Initialize Light Lightness Server.
#define SL_BTMESH_GENERIC_LIGHTNESS_SERVER_INIT_CFG_VAL                0

// <q SL_BTMESH_GENERIC_CTL_SERVER_INIT_CFG_VAL> Light CTL Server
// <i> Default: 0
// <i> Initialize Light CTL Server.
#define SL_BTMESH_GENERIC_CTL_SERVER_INIT_CFG_VAL                      0

// <q SL_BTMESH_GENERIC_HSL_SERVER_INIT_CFG_VAL> Light HSL Server
// <i> Default: 0
// <i> Initialize Light HSL Server.
#define SL_BTMESH_GENERIC_HSL_SERVER_INIT_CFG_VAL                      0

// </e>

// <e SL_BTMESH_GENERIC_BASE_CLIENT_CFG_VAL> Enable Generic Client Models
// <i> Default: 0
// <i> Enable Generic Client functionality.
#define SL_BTMESH_GENERIC_BASE_CLIENT_CFG_VAL                          0

// <q SL_BTMESH_GENERIC_ON_OFF_CLIENT_INIT_CFG_VAL> Generic On/Off Client
// <i> Default: 0
// <i> Initialize Generic On/Off Client.
#define SL_BTMESH_GENERIC_ON_OFF_CLIENT_INIT_CFG_VAL                   0

// <q SL_BTMESH_GENERIC_LEVEL_CLIENT_INIT_CFG_VAL> Generic Level Client
// <i> Default: 0
// <i> Initialize Generic Level Client.
#define SL_BTMESH_GENERIC_LEVEL_CLIENT_INIT_CFG_VAL                    0

// <q SL_BTMESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_INIT_CFG_VAL> Generic Default Transition Time Client
// <i> Default: 0
// <i> Initialize Generic Default Transition Time Client.
#define SL_BTMESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_INIT_CFG_VAL  0

// <q SL_BTMESH_GENERIC_POWER_ON_OFF_CLIENT_INIT_CFG_VAL> Generic Power On/Off Client
// <i> Default: 0
// <i> Initialize Generic Power On/Off Client.
#define SL_BTMESH_GENERIC_POWER_ON_OFF_CLIENT_INIT_CFG_VAL             0

// <q SL_BTMESH_GENERIC_POWER_LEVEL_CLIENT_INIT_CFG_VAL> Generic Power Level Client
// <i> Default: 0
// <i> Initialize Generic Power Level Client.
#define SL_BTMESH_GENERIC_POWER_LEVEL_CLIENT_INIT_CFG_VAL              0

// <q SL_BTMESH_GENERIC_BATTERY_CLIENT_INIT_CFG_VAL> Generic Battery Client
// <i> Default: 0
// <i> Initialize Generic Battery Client.
#define SL_BTMESH_GENERIC_BATTERY_CLIENT_INIT_CFG_VAL                  0

// <q SL_BTMESH_GENERIC_LOCATION_CLIENT_INIT_CFG_VAL> Generic Location Client
// <i> Default: 0
// <i> Initialize Generic Location Client.
#define SL_BTMESH_GENERIC_LOCATION_CLIENT_INIT_CFG_VAL                 0

// <q SL_BTMESH_GENERIC_PROPERTY_CLIENT_INIT_CFG_VAL> Generic Property Client
// <i> Default: 0
// <i> Initialize Generic Property Client.
#define SL_BTMESH_GENERIC_PROPERTY_CLIENT_INIT_CFG_VAL                 0

// <q SL_BTMESH_GENERIC_LIGHTNESS_CLIENT_INIT_CFG_VAL> Light Lightness Client
// <i> Default: 0
// <i> Initialize Lightness Client.
#define SL_BTMESH_GENERIC_LIGHTNESS_CLIENT_INIT_CFG_VAL                0

// <q SL_BTMESH_GENERIC_CTL_CLIENT_INIT_CFG_VAL> Light CTL Client
// <i> Default: 0
// <i> Initialize Light CTL Client.
#define SL_BTMESH_GENERIC_CTL_CLIENT_INIT_CFG_VAL                      0

// <q SL_BTMESH_GENERIC_HSL_CLIENT_INIT_CFG_VAL> Light HSL Client
// <i> Default: 0
// <i> Initialize Light HSL Client.
#define SL_BTMESH_GENERIC_HSL_CLIENT_INIT_CFG_VAL                      0

// </e>

// </h>

// <<< end of configuration section >>>

#endif // SL_BTMESH_GENERIC_BASE_CONFIG_H
