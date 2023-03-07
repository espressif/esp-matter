/***************************************************************************//**
 * @brief Zigbee Calendar Common component configuration header.
 *\n*******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Zigbee Calendar Common configuration

// <o EMBER_AF_PLUGIN_CALENDAR_COMMON_TOTAL_CALENDARS> Total Calendars <1-255>
// <i> Default: 2
// <i> The maximum number of calendars supported by the device.
#define EMBER_AF_PLUGIN_CALENDAR_COMMON_TOTAL_CALENDARS   2

// <o EMBER_AF_PLUGIN_CALENDAR_COMMON_SPECIAL_DAY_PROFILE_MAX> Special Day Profile Max <1-255>
// <i> Default: 50
// <i> The maximum number of special days supported by the calendar.
#define EMBER_AF_PLUGIN_CALENDAR_COMMON_SPECIAL_DAY_PROFILE_MAX   50

// <o EMBER_AF_PLUGIN_CALENDAR_COMMON_WEEK_PROFILE_MAX> Week Profile Max <4-255>
// <i> Default: 4
// <i> The maximum number of unique week profiles in the calendar.
#define EMBER_AF_PLUGIN_CALENDAR_COMMON_WEEK_PROFILE_MAX   4

// <o EMBER_AF_PLUGIN_CALENDAR_COMMON_SCHEDULE_ENTRIES_MAX> Schedule Entries Max <1-255>
// <i> Default: 5
// <i> The max number of price tier schedule entries in a single day.
#define EMBER_AF_PLUGIN_CALENDAR_COMMON_SCHEDULE_ENTRIES_MAX   5

// <o EMBER_AF_PLUGIN_CALENDAR_COMMON_SEASON_PROFILE_MAX> Season Profile Max <4-255>
// <i> Default: 4
// <i> The maximum number of season profiles supported by the calendar.
#define EMBER_AF_PLUGIN_CALENDAR_COMMON_SEASON_PROFILE_MAX   4

// <o EMBER_AF_PLUGIN_CALENDAR_COMMON_DAY_PROFILE_MAX> Day Profile Max <1-255>
// <i> Default: 7
// <i> The maximum number of day profiles supported by the calendar.
#define EMBER_AF_PLUGIN_CALENDAR_COMMON_DAY_PROFILE_MAX   7

// </h>

// <<< end of configuration section >>>
