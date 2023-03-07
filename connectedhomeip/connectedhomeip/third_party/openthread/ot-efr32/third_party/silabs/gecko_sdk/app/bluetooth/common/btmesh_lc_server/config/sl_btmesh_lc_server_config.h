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
#ifndef SL_BTMESH_LC_SERVER_CONFIG_H
#define SL_BTMESH_LC_SERVER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> LC Server configuration

// <o SL_BTMESH_LC_SERVER_NVM_SAVE_TIME_CFG_VAL> Timeout [ms] for saving States of the model to NVM.
// <i> Default: 5000
// <i> Timeout [ms] for saving States of the model to NVM.
#define SL_BTMESH_LC_SERVER_NVM_SAVE_TIME_CFG_VAL   (5000)

// <o SL_BTMESH_LC_SERVER_PS_KEY_CFG_VAL> PS Key for NVM Page where the States of the LC Model are saved.
// <i> Default: 0x4006
// <i> PS Key for NVM Page where the States of the LC Model are saved.
#define SL_BTMESH_LC_SERVER_PS_KEY_CFG_VAL   (0x4006)

// <o SL_BTMESH_LC_SERVER_PROPERTY_PS_KEY_CFG_VAL> PS Key for NVM Page where the Property State of the LC Model are saved.
// <i> Default: 0x4007
// <i> PS Key for NVM Page where the Property State of the LC Model are saved.
#define SL_BTMESH_LC_SERVER_PROPERTY_PS_KEY_CFG_VAL   (0x4007)

// <e SL_BTMESH_LC_SERVER_LOGGING_CFG_VAL> Enable Logging
// <i> Default: 1
// <i> Enable / disable Logging for LC Server model specific messages for this component.
#define SL_BTMESH_LC_SERVER_LOGGING_CFG_VAL   (1)

// </e>

// </h>

// <e SL_BTMESH_LC_SERVER_PROPERTY_STATE_DEFAULT_ENABLE_CFG_VAL> Customize LC Property states' default values
#define SL_BTMESH_LC_SERVER_PROPERTY_STATE_DEFAULT_ENABLE_CFG_VAL 0

// <h> Time

// <o SL_BTMESH_LC_SERVER_TIME_OCCUPANCY_DELAY_DEFAULT_CFG_VAL> Occupancy Delay [ms] <0x000000-0xFFFFFF>
// <i> Determines the delay for changing the LC Occupancy state upon receiving a Sensor Status message from an occupancy sensor.
// <i> LC Occupancy is a binary state that represents occupancy reported by an occupancy sensor.
#define SL_BTMESH_LC_SERVER_TIME_OCCUPANCY_DELAY_DEFAULT_CFG_VAL 0

// <o SL_BTMESH_LC_SERVER_TIME_FADE_ON_DEFAULT_CFG_VAL> Fade On [ms] <0x000000-0xFFFFFF>
// <i> Determines the time the controlled lights fade to the level determined by the LC Lightness On state.
#define SL_BTMESH_LC_SERVER_TIME_FADE_ON_DEFAULT_CFG_VAL 0

// <o SL_BTMESH_LC_SERVER_TIME_RUN_ON_DEFAULT_CFG_VAL> Run On [ms] <0x000000-0xFFFFFF>
// <i> Determines the time the controlled lights stay at the level determined by the LC Lightness On state since the occupancy input stopped detecting active occupancy information.
#define SL_BTMESH_LC_SERVER_TIME_RUN_ON_DEFAULT_CFG_VAL 0

// <o SL_BTMESH_LC_SERVER_TIME_FADE_DEFAULT_CFG_VAL> Fade [ms] <0x000000-0xFFFFFF>
// <i> Determines the time the controlled lights fade from the level determined by the LC Lightness On state to the level determined by the Lightness Prolong state.
#define SL_BTMESH_LC_SERVER_TIME_FADE_DEFAULT_CFG_VAL 0

// <o SL_BTMESH_LC_SERVER_TIME_PROLONG_DEFAULT_CFG_VAL> Prolong [ms] <0x000000-0xFFFFFF>
// <i> Determines the time the controlled lights stay at the level determined by the LC Lightness Prolong state.
#define SL_BTMESH_LC_SERVER_TIME_PROLONG_DEFAULT_CFG_VAL 0

// <o SL_BTMESH_LC_SERVER_TIME_FADE_STANDBY_AUTO_DEFAULT_CFG_VAL> Fade Standby Auto [ms] <0x000000-0xFFFFFF>
// <i> Determines the time the controlled lights fade from the level determined by the LC Lightness Prolong state to the level determined by the LC Lightness Standby state when the transition is automatic.
#define SL_BTMESH_LC_SERVER_TIME_FADE_STANDBY_AUTO_DEFAULT_CFG_VAL 0

// <o SL_BTMESH_LC_SERVER_TIME_FADE_STANDBY_MANUAL_DEFAULT_CFG_VAL> Fade Standby Manual [ms] <0x000000-0xFFFFFF>
// <i> Determines the time the controlled lights fade from the level determined by the LC Lightness Prolong state to the level determined by the LC Lightness Standby state when the transition is triggered by a change in the LC Light OnOff state.
#define SL_BTMESH_LC_SERVER_TIME_FADE_STANDBY_MANUAL_DEFAULT_CFG_VAL 0

// </h>

// <h> Lightness

// <o SL_BTMESH_LC_SERVER_LIGHTNESS_ON_DEFAULT_CFG_VAL> On <0x0000-0xFFFF>
// <i> Determines the perceptive light lightness at the Occupancy and Run internal controller states.
#define SL_BTMESH_LC_SERVER_LIGHTNESS_ON_DEFAULT_CFG_VAL 0

// <o SL_BTMESH_LC_SERVER_LIGHTNESS_PROLONG_DEFAULT_CFG_VAL> Prolong <0x0000-0xFFFF>
// <i> Determines the light lightness at the Prolong internal controller state.
#define SL_BTMESH_LC_SERVER_LIGHTNESS_PROLONG_DEFAULT_CFG_VAL 0

// <o SL_BTMESH_LC_SERVER_LIGHTNESS_STANDBY_DEFAULT_CFG_VAL> Standby <0x0000-0xFFFF>
// <i> Determines the light lightness at the Standby internal controller state.
#define SL_BTMESH_LC_SERVER_LIGHTNESS_STANDBY_DEFAULT_CFG_VAL 0

// </h>

// <h> Ambient

// <o SL_BTMESH_LC_SERVER_AMBIENT_LUX_LEVEL_ON_DEFAULT_CFG_VAL> LuxLevel On [lux] <0x0000-0xFFFF>
// <i> Represents the level that determines if the controller transitions from the Light Control Standby state.
#define SL_BTMESH_LC_SERVER_AMBIENT_LUX_LEVEL_ON_DEFAULT_CFG_VAL 0

// <o SL_BTMESH_LC_SERVER_AMBIENT_LUX_LEVEL_PROLONG_DEFAULT_CFG_VAL> LuxLevel Prolong [lux] <0x0000-0xFFFF>
// <i> Represents the required level in the Prolong state.
#define SL_BTMESH_LC_SERVER_AMBIENT_LUX_LEVEL_PROLONG_DEFAULT_CFG_VAL 0

// <o SL_BTMESH_LC_SERVER_AMBIENT_LUX_LEVEL_STANDBY_DEFAULT_CFG_VAL> LuxLevel Standby [lux] <0x0000-0xFFFF>
// <i> Represents the required level in the Standby state.
#define SL_BTMESH_LC_SERVER_AMBIENT_LUX_LEVEL_STANDBY_DEFAULT_CFG_VAL 0

// </h>

// </e>

// <<< end of configuration section >>>

#endif // SL_BTMESH_LC_SERVER_CONFIG_H
