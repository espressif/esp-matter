/***************************************************************************//**
 * @brief Zigbee Calendar Client component configuration header.
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

// <h>Zigbee Calendar Client configuration

// <o EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS> Calendars <1-255>
// <i> Default: 2
// <i> The maximum number of calendars supported by the device.
#define EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS   2

// <o EMBER_AF_PLUGIN_CALENDAR_CLIENT_SEASONS> Seasons <4-255>
// <i> Default: 4
// <i> The maximum number of season profiles supported by the calendar.
#define EMBER_AF_PLUGIN_CALENDAR_CLIENT_SEASONS   4

// <o EMBER_AF_PLUGIN_CALENDAR_CLIENT_WEEK_PROFILES> Week profiles <4-255>
// <i> Default: 4
// <i> The maximum number of unique week profiles in the calendar.
#define EMBER_AF_PLUGIN_CALENDAR_CLIENT_WEEK_PROFILES   4

// <o EMBER_AF_PLUGIN_CALENDAR_CLIENT_DAY_PROFILES> Day profiles <1-255>
// <i> Default: 7
// <i> The maximum number of day profiles supported by the calendar.
#define EMBER_AF_PLUGIN_CALENDAR_CLIENT_DAY_PROFILES   7

// <o EMBER_AF_PLUGIN_CALENDAR_CLIENT_SCHEDULE_ENTRIES> Schedule entries <1-255>
// <i> Default: 5
// <i> The max number of price tier schedule entries in a single day.
#define EMBER_AF_PLUGIN_CALENDAR_CLIENT_SCHEDULE_ENTRIES   5

// <o EMBER_AF_PLUGIN_CALENDAR_CLIENT_SPECIAL_DAY_ENTRIES> Special day entries <1-255>
// <i> Default: 50
// <i> The maximum number of special days supported by the calendar.
#define EMBER_AF_PLUGIN_CALENDAR_CLIENT_SPECIAL_DAY_ENTRIES   50

// </h>

// <<< end of configuration section >>>
