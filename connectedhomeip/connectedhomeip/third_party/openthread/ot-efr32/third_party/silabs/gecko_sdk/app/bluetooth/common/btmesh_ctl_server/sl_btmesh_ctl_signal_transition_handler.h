/***************************************************************************//**
 * @file
 * @brief sl_btmesh_ctl_signal_transition_handler.h
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

#ifndef SL_BTMESH_CTL_SIGNAL_TRANSITION_H
#define SL_BTMESH_CTL_SIGNAL_TRANSITION_H

/*******************************************************************************
 * Function for retrieving actual temperature.
 *
 * @return Actual temperature level.
 ******************************************************************************/
uint16_t sl_btmesh_get_temperature(void);

/*******************************************************************************
 * Function for retrieving actual delta UV.
 *
 * @return Actual delta UV level.
 ******************************************************************************/
uint16_t sl_btmesh_get_deltauv(void);

/*******************************************************************************
 * Set LED temperature and delta UV in given transition time.
 *
 * @param[in] temperature    Temperature of color.
 * @param[in] deltauv        Delta UV value.
 * @param[in] transition_ms  Transition time in milliseconds.
 ******************************************************************************/
void sl_btmesh_ctl_set_temperature_deltauv_level(uint16_t temperature,
                                                 int16_t deltauv,
                                                 uint32_t transition_ms);

/**************************************************************************//**
 * Utility function to print the delta UV raw value into the passed character
 * buffer in the <sign>X.XX format.
 *
 * @param[out] buffer      Character buffer where the formatted delta UV
 *                         is stored
 * @param[in] buffer_size  Size of the character buffer
 * @param[in] deltauv_raw  Raw value of the Delta UV CTL parameter
 *
 * @return same as snprintf
 *****************************************************************************/
int sl_btmesh_ctl_server_snprint_deltauv(char *buffer,
                                         uint16_t buffer_size,
                                         uint16_t deltauv_raw);

/*******************************************************************************
 * Callback for setting Light Color by PWM level (0x0001 - FFFE)
 *
 * @param[in] Desired light color PWM level.
 ******************************************************************************/
void sl_btmesh_lighting_color_pwm_cb(uint16_t color);

/***************************************************************************//**
 * Called when the UI shall be updated with the changed CTL Model state during
 * a transition. The rate of this callback can be controlled by changing the
 * SL_BTMESH_CTL_SERVER_UI_UPDATE_PERIOD_CFG_VAL macro.
 *
 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 *
 * @param[in] temperature Temperature of color.
 * @param[in] deltauv     Delta UV value.
 ******************************************************************************/
void sl_btmesh_ctl_on_ui_update(uint16_t temperature,
                                uint16_t deltauv);

#endif // SL_BTMESH_CTL_SIGNAL_TRANSITION_H
