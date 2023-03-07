/***************************************************************************//**
 * @file
 * @brief CLI for the Calendar Server plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "app/framework/include/af.h"
#include "app/framework/plugin/calendar-common/calendar-common.h"
#include "calendar-server.h"

#ifdef UC_BUILD

// plugin calendar-server publish-calendar <nodeId:2> <srcEndpoint:1> <dstEndpoint:1> <calendarIndex:1>
void emAfCalendarServerCliPublishCalendar(sl_cli_command_arg_t *arguments)
{
  EmberNodeId nodeId = (EmberNodeId)sl_cli_get_argument_uint16(arguments, 0);
  uint8_t srcEndpoint = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t dstEndpoint = sl_cli_get_argument_uint8(arguments, 2);
  uint8_t calendarIndex = sl_cli_get_argument_uint8(arguments, 3);
  emberAfCalendarServerPublishCalendarMessage(nodeId,
                                              srcEndpoint,
                                              dstEndpoint,
                                              calendarIndex);
}

// plugin calendar-server publish-day-profiles <nodeId:2> <srcEndpoint:1> <dstEndpoint:1> <calendarIndex:1> <dayIndex:1>
void emAfCalendarServerCliPublishDayProfiles(sl_cli_command_arg_t *arguments)
{
  EmberNodeId nodeId = (EmberNodeId)sl_cli_get_argument_uint16(arguments, 0);
  uint8_t srcEndpoint = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t dstEndpoint = sl_cli_get_argument_uint8(arguments, 2);
  uint8_t calendarIndex = sl_cli_get_argument_uint8(arguments, 3);
  uint8_t dayIndex = sl_cli_get_argument_uint8(arguments, 4);
  emberAfCalendarServerPublishDayProfilesMessage(nodeId,
                                                 srcEndpoint,
                                                 dstEndpoint,
                                                 calendarIndex,
                                                 dayIndex);
}

// plugin calendar-server publish-week-profile <nodeId:2> <srcEndpoint:1> <dstEndpoint:1> <calendarIndex:1> <weekIndex:1>
void emAfCalendarServerCliPublishWeekProfile(sl_cli_command_arg_t *arguments)
{
  EmberNodeId nodeId = (EmberNodeId)sl_cli_get_argument_uint16(arguments, 0);
  uint8_t srcEndpoint = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t dstEndpoint = sl_cli_get_argument_uint8(arguments, 2);
  uint8_t calendarIndex = sl_cli_get_argument_uint8(arguments, 3);
  uint8_t weekIndex = sl_cli_get_argument_uint8(arguments, 4);
  emberAfCalendarServerPublishDayProfilesMessage(nodeId,
                                                 srcEndpoint,
                                                 dstEndpoint,
                                                 calendarIndex,
                                                 weekIndex);
}

// plugin calendar-server publish-week-profile <nodeId:2> <srcEndpoint:1> <dstEndpoint:1> <calendarIndex:1>
void emAfCalendarServerCliPublishSeasons(sl_cli_command_arg_t *arguments)
{
  EmberNodeId nodeId = (EmberNodeId)sl_cli_get_argument_uint16(arguments, 0);
  uint8_t srcEndpoint = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t dstEndpoint = sl_cli_get_argument_uint8(arguments, 2);
  uint8_t calendarIndex = sl_cli_get_argument_uint8(arguments, 3);
  emberAfCalendarServerPublishSeasonsMessage(nodeId,
                                             srcEndpoint,
                                             dstEndpoint,
                                             calendarIndex);
}

// plugin calendar-server publish-special-days <nodeId:2> <srcEndpoint:1> <dstEndpoint:1> <calendarIndex:1>
void emAfCalendarServerCliPublishSpecialDays(sl_cli_command_arg_t *arguments)
{
  EmberNodeId nodeId = (EmberNodeId)sl_cli_get_argument_uint16(arguments, 0);
  uint8_t srcEndpoint = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t dstEndpoint = sl_cli_get_argument_uint8(arguments, 2);
  uint8_t calendarIndex = sl_cli_get_argument_uint8(arguments, 3);
  emberAfCalendarServerPublishSpecialDaysMessage(nodeId,
                                                 srcEndpoint,
                                                 dstEndpoint,
                                                 calendarIndex);
}

// plugin calendar-server cancel-calendar <nodeId:2> <srcEndpoint:1> <dstEndpoint:1> <calendarIndex:1>
void emberAfCalendarServerCliCancelCalendar(sl_cli_command_arg_t *arguments)
{
  EmberNodeId nodeId = (EmberNodeId)sl_cli_get_argument_uint16(arguments, 0);
  uint8_t srcEndpoint = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t dstEndpoint = sl_cli_get_argument_uint8(arguments, 2);
  uint8_t calendarIndex = sl_cli_get_argument_uint8(arguments, 3);
  emberAfCalendarServerCancelCalendarMessage(nodeId,
                                             srcEndpoint,
                                             dstEndpoint,
                                             calendarIndex);
  // now invalidate the calendar
  if (calendarIndex < EMBER_AF_PLUGIN_CALENDAR_COMMON_TOTAL_CALENDARS) {
    MEMSET(&(calendars[calendarIndex]), 0, sizeof(EmberAfCalendarStruct));
    calendars[calendarIndex].calendarId = EMBER_AF_PLUGIN_CALENDAR_COMMON_INVALID_CALENDAR_ID;
  }
}

#else

#ifndef EMBER_AF_GENERATE_CLI
  #error The Calendar Server plugin is not compatible with the legacy CLI.
#endif

// plugin calendar-server publish-calendar <nodeId:2> <srcEndpoint:1> <dstEndpoint:1> <calendarIndex:1>
void emAfCalendarServerCliPublishCalendar(void)
{
  EmberNodeId nodeId = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t srcEndpoint = (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t dstEndpoint = (uint8_t)emberUnsignedCommandArgument(2);
  uint8_t calendarIndex = (uint8_t)emberUnsignedCommandArgument(3);
  emberAfCalendarServerPublishCalendarMessage(nodeId,
                                              srcEndpoint,
                                              dstEndpoint,
                                              calendarIndex);
}

// plugin calendar-server publish-day-profiles <nodeId:2> <srcEndpoint:1> <dstEndpoint:1> <calendarIndex:1> <dayIndex:1>
void emAfCalendarServerCliPublishDayProfiles(void)
{
  EmberNodeId nodeId = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t srcEndpoint = (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t dstEndpoint = (uint8_t)emberUnsignedCommandArgument(2);
  uint8_t calendarIndex = (uint8_t)emberUnsignedCommandArgument(3);
  uint8_t dayIndex = (uint8_t)emberUnsignedCommandArgument(4);
  emberAfCalendarServerPublishDayProfilesMessage(nodeId,
                                                 srcEndpoint,
                                                 dstEndpoint,
                                                 calendarIndex,
                                                 dayIndex);
}

// plugin calendar-server publish-week-profile <nodeId:2> <srcEndpoint:1> <dstEndpoint:1> <calendarIndex:1> <weekIndex:1>
void emAfCalendarServerCliPublishWeekProfile(void)
{
  EmberNodeId nodeId = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t srcEndpoint = (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t dstEndpoint = (uint8_t)emberUnsignedCommandArgument(2);
  uint8_t calendarIndex = (uint8_t)emberUnsignedCommandArgument(3);
  uint8_t weekIndex = (uint8_t)emberUnsignedCommandArgument(4);
  emberAfCalendarServerPublishDayProfilesMessage(nodeId,
                                                 srcEndpoint,
                                                 dstEndpoint,
                                                 calendarIndex,
                                                 weekIndex);
}

// plugin calendar-server publish-week-profile <nodeId:2> <srcEndpoint:1> <dstEndpoint:1> <calendarIndex:1>
void emAfCalendarServerCliPublishSeasons(void)
{
  EmberNodeId nodeId = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t srcEndpoint = (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t dstEndpoint = (uint8_t)emberUnsignedCommandArgument(2);
  uint8_t calendarIndex = (uint8_t)emberUnsignedCommandArgument(3);
  emberAfCalendarServerPublishSeasonsMessage(nodeId,
                                             srcEndpoint,
                                             dstEndpoint,
                                             calendarIndex);
}

// plugin calendar-server publish-special-days <nodeId:2> <srcEndpoint:1> <dstEndpoint:1> <calendarIndex:1>
void emAfCalendarServerCliPublishSpecialDays(void)
{
  EmberNodeId nodeId = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t srcEndpoint = (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t dstEndpoint = (uint8_t)emberUnsignedCommandArgument(2);
  uint8_t calendarIndex = (uint8_t)emberUnsignedCommandArgument(3);
  emberAfCalendarServerPublishSpecialDaysMessage(nodeId,
                                                 srcEndpoint,
                                                 dstEndpoint,
                                                 calendarIndex);
}

// plugin calendar-server cancel-calendar <nodeId:2> <srcEndpoint:1> <dstEndpoint:1> <calendarIndex:1>
void emberAfCalendarServerCliCancelCalendar(void)
{
  EmberNodeId nodeId = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t srcEndpoint = (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t dstEndpoint = (uint8_t)emberUnsignedCommandArgument(2);
  uint8_t calendarIndex = (uint8_t)emberUnsignedCommandArgument(3);
  emberAfCalendarServerCancelCalendarMessage(nodeId,
                                             srcEndpoint,
                                             dstEndpoint,
                                             calendarIndex);
  // now invalidate the calendar
  if (calendarIndex < EMBER_AF_PLUGIN_CALENDAR_COMMON_TOTAL_CALENDARS) {
    MEMSET(&(calendars[calendarIndex]), 0, sizeof(EmberAfCalendarStruct));
    calendars[calendarIndex].calendarId = EMBER_AF_PLUGIN_CALENDAR_COMMON_INVALID_CALENDAR_ID;
  }
}
#endif
