/***************************************************************************//**
 * @file
 * @brief sl_btmesh_lighting_client.h
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

#ifndef SL_BTMESH_LIGHTING_CLIENT_H
#define SL_BTMESH_LIGHTING_CLIENT_H

#define SL_BTMESH_LIGHTING_CLIENT_OFF         0   ///< Set switch state to off
#define SL_BTMESH_LIGHTING_CLIENT_ON          1   ///< Set switch state to on
#define SL_BTMESH_LIGHTING_CLIENT_TOGGLE      2   ///< Toggle switch state

/*******************************************************************************
 * This function change the lightness and send it to the server.
 *
 * @param[in] change  Defines lightness percentage change, possible values are
 *                    -100% - + 100%.
 *
 ******************************************************************************/
void sl_btmesh_change_lightness(int8_t change_percentage);

/*******************************************************************************
 * This function change the lightness and send it to the server.
 *
 * @param[in] new_lightness_percentage  Defines new lightness value as percentage
 *    Valid values 0-100 %
 *
 ******************************************************************************/
void sl_btmesh_set_lightness(uint8_t new_lightness_percentage);

/***************************************************************************//**
 * This function change the switch position and send it to the server.
 *
 * @param[in] position Defines switch position change, possible values are:
 *                       - SL_BTMESH_LIGHTING_CLIENT_OFF
 *                       - SL_BTMESH_LIGHTING_CLIENT_ON
 *                       - SL_BTMESH_LIGHTING_CLIENT_TOGGLE
 *
 ******************************************************************************/
void sl_btmesh_change_switch_position(uint8_t position);

/***************************************************************************//**
 * Get lightness.
 *
 * This function returns actual lightness to set.
 *
 ******************************************************************************/
uint16_t sl_btmesh_get_lightness(void);

#endif // SL_BTMESH_LIGHTING_CLIENT_H
