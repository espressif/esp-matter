/***************************************************************************//**
 * @file
 * @brief BT Mesh People count sensor implementation
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

#include <stdio.h>

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_CLI_PRESENT
#include "sl_cli.h"
#ifdef SL_CATALOG_APP_LOG_PRESENT
#include "app_log.h"
#define log(...) app_log(__VA_ARGS__)
#else
#define log(...)
#endif // SL_CATALOG_APP_LOG_PRESENT
#endif // SL_CATALOG_CLI_PRESENT

#include "sl_btmesh_sensor_people_count.h"
#include "sl_btmesh_sensor_people_count_config.h"

/***************************************************************************//**
 * @addtogroup Sensor
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup PeopleCount
 * @{
 ******************************************************************************/

/// People count
static count16_t people_count = SL_BTMESH_SENSOR_PEOPLE_COUNT_VALUE_IS_NOT_KNOWN;

/*******************************************************************************
 * Set the people count value. It could be used to initialize the sensor.
 *
 * @param[in] people_count  People count value to set
 ******************************************************************************/
void sl_btmesh_set_people_count(count16_t people_count_value)
{
  people_count = people_count_value;
}

/*******************************************************************************
 * Get the current people count value measured by sensor.
 *
 * @return Current value of people count.
 ******************************************************************************/
count16_t sl_btmesh_get_people_count(void)
{
  return people_count;
}

/*******************************************************************************
 * Increase people count value by one. After exceeding the maximum value it set
 * people count to value is not known.
 ******************************************************************************/
void sl_btmesh_people_count_increase(void)
{
  if (people_count < SL_BTMESH_SENSOR_PEOPLE_COUNT_VALUE_IS_NOT_KNOWN) {
    people_count += 1;
  }
}

/*******************************************************************************
 * Decrease people count value by one if value is known and greater than 0.
 ******************************************************************************/
void sl_btmesh_people_count_decrease(void)
{
  if (people_count > 0
      && people_count < SL_BTMESH_SENSOR_PEOPLE_COUNT_VALUE_IS_NOT_KNOWN) {
    people_count -= 1;
  }
}

/**************************************************************************//**
 * CLI Callbacks
 *****************************************************************************/
#ifdef SL_CATALOG_CLI_PRESENT
void people_count_increase_from_cli(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  sl_btmesh_people_count_increase();
}

void people_count_decrease_from_cli(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  sl_btmesh_people_count_decrease();
}

void people_count_set_from_cli(sl_cli_command_arg_t *arguments)
{
  uint16_t value;
  value = sl_cli_get_argument_uint16(arguments, 0);
  sl_btmesh_set_people_count(value);
}

void people_count_get_from_cli(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  log("%u\r\n", people_count);
}

#endif // SL_CATALOG_CLI_PRESENT

/** @} (end addtogroup PeopleCount) */
/** @} (end addtogroup Sensor) */
