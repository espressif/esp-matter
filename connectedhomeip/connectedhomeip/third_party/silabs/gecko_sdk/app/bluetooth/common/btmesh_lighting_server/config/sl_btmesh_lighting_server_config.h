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
#ifndef SL_BTMESH_LIGHTING_SERVER_CONFIG_H
#define SL_BTMESH_LIGHTING_SERVER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Light Lightness Server configuration

// <o SL_BTMESH_LIGHTING_SERVER_NVM_SAVE_TIME_CFG_VAL> Timeout [ms] for saving States of the model to NVM.
// <i> Default: 5000
// <i> Timeout [ms] for saving States of the model to NVM.
#define SL_BTMESH_LIGHTING_SERVER_NVM_SAVE_TIME_CFG_VAL   (5000)

// <o SL_BTMESH_LIGHTING_SERVER_PS_KEY_CFG_VAL> PS Key for NVM Page where the States of the Lighting Model are saved.
// <i> Default: 0x4004
// <i> PS Key for NVM Page where the States of the Lighting Model are saved.
#define SL_BTMESH_LIGHTING_SERVER_PS_KEY_CFG_VAL   (0x4004)

// <o SL_BTMESH_LIGHTING_SERVER_PWM_UPDATE_PERIOD_CFG_VAL> Periodicity [ms] for updating the PWM duty cycle during a transition.
// <i> Default: 1
// <i> Periodicity [ms] for updating the PWM duty cycle during a transition.
#define SL_BTMESH_LIGHTING_SERVER_PWM_UPDATE_PERIOD_CFG_VAL   (1)

// <o SL_BTMESH_LIGHTING_SERVER_UI_UPDATE_PERIOD_CFG_VAL> for updating the UI with lightness level during a transition.
// <i> Default: 100
// <i> Periodicity [ms] for updating the UI with lightness level during a transition.
#define SL_BTMESH_LIGHTING_SERVER_UI_UPDATE_PERIOD_CFG_VAL   (100)

// <o SL_BTMESH_LIGHTING_SERVER_PWM_MINIMUM_BRIGHTNESS_CFG_VAL> Timer value for minimum PWM duty cycle.
// <i> Default: 1
// <i> Timer value for minimum PWM duty cycle.
#define SL_BTMESH_LIGHTING_SERVER_PWM_MINIMUM_BRIGHTNESS_CFG_VAL   (1)

// <o SL_BTMESH_LIGHTING_SERVER_PWM_MAXIMUM_BRIGHTNESS_CFG_VAL> Timer value for maximum PWM duty cycle.
// <i> Default: 0xFFFE
// <i> Timer value for minimum PWM duty cycle.
#define SL_BTMESH_LIGHTING_SERVER_PWM_MAXIMUM_BRIGHTNESS_CFG_VAL   (0xFFFE)

// <h> Lightness Range

// <o SL_BTMESH_LIGHTING_SERVER_LIGHTNESS_MIN_CFG_VAL> Minimum lightness value <0x0001-0xFFFF>
// <i> Determines the minimum non-zero lightness an element is configured to emit.
// <i> Default: 0x0001
#define SL_BTMESH_LIGHTING_SERVER_LIGHTNESS_MIN_CFG_VAL   0x0001

// <o SL_BTMESH_LIGHTING_SERVER_LIGHTNESS_MAX_CFG_VAL> Maximum lightness value <0x0001-0xFFFF>
// <i> Determines the maximum lightness an element is configured to emit.
// <i> The value of the Light Lightness Range Max state shall be greater than
// <i> or equal to the value of the Light Lightness Range Min state.
// <i> Default: 0xFFFF
#define SL_BTMESH_LIGHTING_SERVER_LIGHTNESS_MAX_CFG_VAL   0xFFFF

//</h>

// <e SL_BTMESH_LIGHTING_SERVER_LOGGING_CFG_VAL> Enable Logging
// <i> Default: 1
// <i> Enable / disable Logging for Lighting Server model specific messages for this component.
#define SL_BTMESH_LIGHTING_SERVER_LOGGING_CFG_VAL   (1)

// <q SL_BTMESH_LIGHTING_SERVER_DEBUG_PRINTS_FOR_STATE_CHANGE_EVENTS_CFG_VAL> Enable debug prints for each server state changed event.
// <i> Default: 0
// <i> Enable debug prints for each server state changed event.
#define SL_BTMESH_LIGHTING_SERVER_DEBUG_PRINTS_FOR_STATE_CHANGE_EVENTS_CFG_VAL   (0)

// </e>

// </h>

// <<< end of configuration section >>>

// The PWM update period shall not be greater than the UI update period
#if (SL_BTMESH_LIGHTING_SERVER_UI_UPDATE_PERIOD_CFG_VAL) < (SL_BTMESH_LIGHTING_SERVER_PWM_UPDATE_PERIOD_CFG_VAL)
#error "The SL_BTMESH_LIGHTING_SERVER_PWM_UPDATE_PERIOD_CFG_VAL shall be less than SL_BTMESH_LIGHTING_SERVER_UI_UPDATE_PERIOD_CFG_VAL."
#endif

// Lightness maximum value cannot be less than minimum value
#if (SL_BTMESH_LIGHTING_SERVER_LIGHTNESS_MAX_CFG_VAL) < (SL_BTMESH_LIGHTING_SERVER_LIGHTNESS_MIN_CFG_VAL)
#error The value of the Lightness Range Max shall be greater than or equal to \
  the value of the Lightness Range Min state.
#endif // (SL_BTMESH_LIGHTING_SERVER_LIGHTNESS_MAX_CFG_VAL) < (SL_BTMESH_LIGHTING_SERVER_LIGHTNESS_MIN_CFG_VAL)

#endif // SL_BTMESH_LIGHTING_SERVER_CONFIG_H
