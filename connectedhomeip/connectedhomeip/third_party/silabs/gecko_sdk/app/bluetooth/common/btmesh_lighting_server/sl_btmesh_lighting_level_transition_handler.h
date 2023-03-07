/***************************************************************************//**
 * @file
 * @brief sl_btmesh_lighting_level_transition_handler.h
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

#ifndef SL_BTMESH_LIGHTING_LEVEL_TRANSITION_H
#define SL_BTMESH_LIGHTING_LEVEL_TRANSITION_H

#define LED_STATE_OFF    0   /**< light off (both LEDs turned off) */
#define LED_STATE_ON     1   /**< light on (both LEDs turned on) */
#define LED_STATE_PROV   2   /**< provisioning (LEDs blinking) */

/*******************************************************************************
 * Set LED lightness level in given transition time.
 *
 * @param[in] level          Lightness level.
 * @param[in] transition_ms  Transition time in milliseconds.
 ******************************************************************************/
void sl_btmesh_lighting_set_level(uint16_t level, uint32_t transition_ms);

/*******************************************************************************
 * Set LEDs state. Possible states are defined in macros.
 *
 * @param[in] state  State to set.
 ******************************************************************************/
void  sl_btmesh_set_state(int state);

/*******************************************************************************
 * Function for retrieving actual lightness level.
 *
 * @return Actual lightness level.
 ******************************************************************************/
uint16_t sl_btmesh_get_level(void);

/*******************************************************************************
 * Callback for setting Light Lightness by PWM level (0x0001 - FFFE)
 *
 * @return Desired lightness PWM level.
 ******************************************************************************/
void sl_btmesh_lighting_level_pwm_cb(uint16_t level);

/***************************************************************************//**
 * Called when the UI shall be updated with the changed state of
 * lightning server during a transition. The rate of this callback can be
 * controlled by changing the SL_BTMESH_LIGHTING_SERVER_UI_UPDATE_PERIOD_CFG_VAL macro.
 *
 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 *
 * @param[in] lightness_level lightness level (0x0001 - FFFE)
 ******************************************************************************/
void sl_btmesh_lighting_server_on_ui_update(uint16_t lightness_level);

#endif // SL_BTMESH_LIGHTING_LEVEL_TRANSITION_H
