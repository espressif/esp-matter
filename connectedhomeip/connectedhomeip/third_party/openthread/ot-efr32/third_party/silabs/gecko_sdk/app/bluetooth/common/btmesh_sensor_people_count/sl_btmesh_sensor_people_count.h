/***************************************************************************//**
 * @file
 * @brief BT Mesh People count sensor header
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

#ifndef SL_BTMESH_SENSOR_PEOPLE_COUNT_H
#define SL_BTMESH_SENSOR_PEOPLE_COUNT_H

#include "sl_btmesh_device_properties.h"

#define SL_BTMESH_SENSOR_PEOPLE_COUNT_VALUE_IS_NOT_KNOWN  (0xFFFF)

/***************************************************************************//**
 * @defgroup PeopleCount People Count Sensor Module
 * @brief People Count Sensor Module Implementation
 * This module simulate the people count sensor behavior.
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup Sensor
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup PeopleCount
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Set the people count value. It could be used to initialize the sensor.
 *
 * @param[in] people_count  People count value to set
 ******************************************************************************/
void sl_btmesh_set_people_count(count16_t people_count);

/***************************************************************************//**
 * Get the current people count value measured by sensor.
 *
 * @return Current value of people count.
 ******************************************************************************/
count16_t sl_btmesh_get_people_count(void);

/***************************************************************************//**
 * Increase people count value by one. After exceeding the maximum value it set
 * people count to value is not known.
 ******************************************************************************/
void  sl_btmesh_people_count_increase(void);

/***************************************************************************//**
 * Decrease people count value by one if value is known and greater than 0.
 ******************************************************************************/
void  sl_btmesh_people_count_decrease(void);

/** @} (end addtogroup PeopleCount) */
/** @} (end addtogroup Sensor) */

#endif /* SL_BTMESH_SENSOR_PEOPLE_COUNT_H */
