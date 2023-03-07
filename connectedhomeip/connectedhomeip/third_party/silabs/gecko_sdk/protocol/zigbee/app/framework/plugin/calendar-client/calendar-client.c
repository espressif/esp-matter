/***************************************************************************//**
 * @file
 * @brief Routines for the Calendar Client plugin.
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
#include "calendar-client.h"

#ifdef UC_BUILD
#include "calendar-client-config.h"
#include "zap-cluster-command-parser.h"
#endif

#ifndef EMBER_AF_GENERATE_CLI
  #error The Calendar Client plugin is not compatible with the legacy CLI.
#endif

#define fieldLength(field) \
  (emberAfCurrentCommand()->bufLen - ((field) - emberAfCurrentCommand()->buffer))

static EmberAfCalendar tempCalendar;
static EmberAfStatus addCalendar(EmberAfCalendar *calendar);
static EmberAfCalendar *findCalendar(uint32_t providerId,
                                     uint32_t issuerCalendarId);
static void removeCalendar(EmberAfCalendar *calendar);

static EmberAfCalendar calendars[EMBER_AF_CALENDAR_CLUSTER_CLIENT_ENDPOINT_COUNT][EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS];

#ifdef UC_BUILD

bool emberAfCalendarClusterPublishCalendarCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_calendar_cluster_publish_calendar_command_t cmd_data;
  EmberAfStatus status;

  if (zcl_decode_calendar_cluster_publish_calendar_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfCalendarClusterPrint("RX: PublishCalendar 0x%4x, 0x%4x, 0x%4x, 0x%4x, 0x%x, \"",
                              cmd_data.providerId,
                              cmd_data.issuerEventId,
                              cmd_data.issuerCalendarId,
                              cmd_data.startTime,
                              cmd_data.calendarType);
  emberAfCalendarClusterPrintString(cmd_data.calendarName);
  emberAfCalendarClusterPrintln("\", %d, %d, %d",
                                cmd_data.numberOfSeasons,
                                cmd_data.numberOfWeekProfiles,
                                cmd_data.numberOfDayProfiles);

  if (findCalendar(cmd_data.providerId, cmd_data.issuerCalendarId) != NULL) {
    emberAfDebugPrintln("ERR: Duplicate calendar: 0x%4x/0x%4x",
                        cmd_data.providerId,
                        cmd_data.issuerCalendarId);
    status = EMBER_ZCL_STATUS_DUPLICATE_EXISTS;
  } else if (EMBER_ZCL_CALENDAR_TYPE_AUXILLIARY_LOAD_SWITCH_CALENDAR
             < cmd_data.calendarType) {
    emberAfDebugPrintln("ERR: Invalid calendar type: 0x%x", cmd_data.calendarType);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (EMBER_ZCL_CALENDAR_TIME_REFERENCE_UTC_TIME
             != cmd_data.calendarTimeReference) {
    emberAfDebugPrintln("ERR: Invalid calendar time reference: 0x%x", cmd_data.calendarTimeReference);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (0 < cmd_data.numberOfSeasons && cmd_data.numberOfWeekProfiles == 0) {
    emberAfDebugPrintln("ERR: Number of week profiles cannot be zero");
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_SEASONS < cmd_data.numberOfSeasons) {
    emberAfDebugPrintln("ERR: Insufficient space for seasons: %d < %d",
                        EMBER_AF_PLUGIN_CALENDAR_CLIENT_SEASONS,
                        cmd_data.numberOfSeasons);
    status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
  } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_WEEK_PROFILES
             < cmd_data.numberOfWeekProfiles) {
    emberAfDebugPrintln("ERR: Insufficient space for week profiles: %d < %d",
                        EMBER_AF_PLUGIN_CALENDAR_CLIENT_WEEK_PROFILES,
                        cmd_data.numberOfWeekProfiles);
    status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
  } else if (cmd_data.numberOfDayProfiles == 0) {
    emberAfDebugPrintln("ERR: Number of day profiles cannot be zero");
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_DAY_PROFILES
             < cmd_data.numberOfDayProfiles) {
    emberAfDebugPrintln("ERR: Insufficient space for day profiles: %d < %d",
                        EMBER_AF_PLUGIN_CALENDAR_CLIENT_DAY_PROFILES,
                        cmd_data.numberOfDayProfiles);
    status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
  } else {
    uint8_t i;
    tempCalendar.inUse = true;
    tempCalendar.providerId = cmd_data.providerId;
    tempCalendar.issuerEventId = cmd_data.issuerEventId;
    tempCalendar.issuerCalendarId = cmd_data.issuerCalendarId;
    tempCalendar.startTimeUtc = cmd_data.startTime;
    tempCalendar.calendarType = cmd_data.calendarType;
    emberAfCopyString(tempCalendar.calendarName,
                      cmd_data.calendarName,
                      EMBER_AF_CALENDAR_MAXIMUM_CALENDAR_NAME_LENGTH);
    tempCalendar.numberOfSeasons = cmd_data.numberOfSeasons;
    tempCalendar.receivedSeasons = 0;
    tempCalendar.numberOfWeekProfiles = cmd_data.numberOfWeekProfiles;
    tempCalendar.numberOfDayProfiles = cmd_data.numberOfDayProfiles;
    for (i = 0; i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_WEEK_PROFILES; i++) {
      tempCalendar.weekProfiles[i].inUse = false;
    }
    for (i = 0; i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_DAY_PROFILES; i++) {
      tempCalendar.dayProfiles[i].inUse = false;
    }
    tempCalendar.specialDayProfile.inUse = false;
    status = addCalendar(&tempCalendar);
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfCalendarClusterPublishDayProfileCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_calendar_cluster_publish_day_profile_command_t cmd_data;
  EmberAfStatus status;
  EmberAfCalendar *calendar;

  if (zcl_decode_calendar_cluster_publish_day_profile_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfCalendarClusterPrint("RX: PublishDayProfile 0x%4x, 0x%4x, 0x%4x, %d, %d, %d, %d, 0x%x, [",
                              cmd_data.providerId,
                              cmd_data.issuerEventId,
                              cmd_data.issuerCalendarId,
                              cmd_data.dayId,
                              cmd_data.totalNumberOfScheduleEntries,
                              cmd_data.commandIndex,
                              cmd_data.totalNumberOfCommands,
                              cmd_data.calendarType);
  // TODO: print dayScheduleEntries
  emberAfCalendarClusterPrintln("]");

  // The PublishDayProfile command is published in response to a GetDayProfile
  // command.  If the IssuerCalendarID does not match with one of the stored
  // calendar instances, the client shall ignore the command and respond using
  // ZCL Default Response with a status response of NOT_FOUND.

  // The Calendar server shall send only the number of Schedule Entries
  // belonging to this calendar instance.  Server and clients shall be able to
  // store at least 1 DayProfile and at least one ScheduleEntries per day
  // profile.  If the client is not able to store all ScheduleEntries, the
  // device should respond using ZCL Default Response with a status response of
  // INSUFFICIENT_SPACE.

  calendar = findCalendar(cmd_data.providerId, cmd_data.issuerCalendarId);
  if (calendar == NULL) {
    emberAfDebugPrintln("ERR: Calendar not found: 0x%4x/0x%4x",
                        cmd_data.providerId,
                        cmd_data.issuerCalendarId);
    status = EMBER_ZCL_STATUS_NOT_FOUND;
  } else if (cmd_data.dayId == 0) {
    emberAfDebugPrintln("ERR: Day id cannot be zero");
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (calendar->numberOfDayProfiles < cmd_data.dayId) {
    emberAfDebugPrintln("ERR: Invalid day: %d < %d",
                        calendar->numberOfDayProfiles,
                        cmd_data.dayId);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_DAY_PROFILES < cmd_data.dayId) {
    // This should never happen because of the previous check and the one in
    // PublishCalendar.  It is here for completeness.
    emberAfDebugPrintln("ERR: Insufficient space for day: %d < %d",
                        EMBER_AF_PLUGIN_CALENDAR_CLIENT_DAY_PROFILES,
                        cmd_data.dayId);
    status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
  } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_SCHEDULE_ENTRIES
             < cmd_data.totalNumberOfScheduleEntries) {
    emberAfDebugPrintln("ERR: Insufficient space for schedule entries: %d < %d",
                        EMBER_AF_PLUGIN_CALENDAR_CLIENT_SCHEDULE_ENTRIES,
                        cmd_data.totalNumberOfScheduleEntries);
    status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
  } else if (cmd_data.totalNumberOfCommands <= cmd_data.commandIndex) {
    emberAfDebugPrintln("ERR: Inconsistent command index: %d <= %d",
                        cmd_data.totalNumberOfCommands,
                        cmd_data.commandIndex);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (calendar->calendarType != cmd_data.calendarType) {
    emberAfDebugPrintln("ERR: Inconsistent calendar type: 0x%x != 0x%x",
                        calendar->calendarType,
                        cmd_data.calendarType);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else {
    EmberAfCalendarDayProfile *dayProfile = &calendar->dayProfiles[cmd_data.dayId - 1];
    uint16_t dayScheduleEntriesLength = fieldLength(cmd_data.dayScheduleEntries);
    uint16_t dayScheduleEntriesIndex = 0;

    status = EMBER_ZCL_STATUS_SUCCESS;
    if (!dayProfile->inUse) {
      dayProfile->inUse = true;
      dayProfile->numberOfScheduleEntries = cmd_data.totalNumberOfScheduleEntries;
    } else if (dayProfile->numberOfScheduleEntries
               != cmd_data.totalNumberOfScheduleEntries) {
      emberAfDebugPrintln("ERR: Inconsistent number of schedule entries: 0x%x != 0x%x",
                          dayProfile->numberOfScheduleEntries,
                          cmd_data.totalNumberOfScheduleEntries);
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
    }

    dayProfile->receivedScheduleEntries = 0;
    while (dayScheduleEntriesIndex < dayScheduleEntriesLength
           && status == EMBER_ZCL_STATUS_SUCCESS) {
      // The rate switch times, friendly credit switch times, and auxilliary
      // load switch times all use a two-byte start time followed by a one-
      // byte value.  These are the only valid types, so all entries must have
      // at least three bytes and, below, we just stuff the bytes into the rate
      // switch time entry in the union to simplify the code.
      if (dayScheduleEntriesLength < dayScheduleEntriesIndex + 3) {
        emberAfDebugPrintln("ERR: Malformed schedule entry");
        status = EMBER_ZCL_STATUS_MALFORMED_COMMAND;
      } else if (dayProfile->numberOfScheduleEntries
                 <= dayProfile->receivedScheduleEntries) {
        emberAfDebugPrintln("ERR: Inconsistent number of received schedule entries: %d <= %d",
                            dayProfile->numberOfScheduleEntries,
                            dayProfile->receivedScheduleEntries);
        status = EMBER_ZCL_STATUS_INVALID_FIELD;
      } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_SCHEDULE_ENTRIES
                 <= dayProfile->receivedScheduleEntries) {
        // This should never happen because of the checks above.  It is here
        // for completeness.
        emberAfDebugPrintln("ERR: Insufficient space for received schedule entries: %d <= %d",
                            EMBER_AF_PLUGIN_CALENDAR_CLIENT_SCHEDULE_ENTRIES,
                            dayProfile->receivedScheduleEntries);
        status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
      } else {
        EmberAfCalendarScheduleEntry *scheduleEntry = &dayProfile->scheduleEntries[dayProfile->receivedScheduleEntries++];
        scheduleEntry->rateSwitchTime.startTimeM = emberAfGetInt16u(cmd_data.dayScheduleEntries,
                                                                    dayScheduleEntriesIndex,
                                                                    dayScheduleEntriesLength);
        dayScheduleEntriesIndex += 2;
        scheduleEntry->rateSwitchTime.priceTier = emberAfGetInt8u(cmd_data.dayScheduleEntries,
                                                                  dayScheduleEntriesIndex,
                                                                  dayScheduleEntriesLength);
        dayScheduleEntriesIndex++;
        if (scheduleEntry != dayProfile->scheduleEntries) {
          EmberAfCalendarScheduleEntry *previous = scheduleEntry - 1;
          if (scheduleEntry->rateSwitchTime.startTimeM
              <= previous->rateSwitchTime.startTimeM) {
            emberAfDebugPrintln("ERR: Inconsistent start times: 0x%2x <= 0x%2x",
                                scheduleEntry->rateSwitchTime.startTimeM,
                                previous->rateSwitchTime.startTimeM);
            status = EMBER_ZCL_STATUS_INVALID_FIELD;
          }
        }
      }
    }
  }

  if (calendar != NULL && status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfDebugPrintln("ERR: Removing invalid calendar: 0x%4x/0x%4x",
                        calendar->providerId,
                        calendar->issuerCalendarId);
    removeCalendar(calendar);
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfCalendarClusterPublishWeekProfileCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_calendar_cluster_publish_week_profile_command_t cmd_data;
  EmberAfStatus status;
  EmberAfCalendar *calendar;

  if (zcl_decode_calendar_cluster_publish_week_profile_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfCalendarClusterPrintln("RX: PublishWeekProfile 0x%4x, 0x%4x, 0x%4x, %d, %d, %d, %d, %d, %d, %d, %d",
                                cmd_data.providerId,
                                cmd_data.issuerEventId,
                                cmd_data.issuerCalendarId,
                                cmd_data.weekId,
                                cmd_data.dayIdRefMonday,
                                cmd_data.dayIdRefTuesday,
                                cmd_data.dayIdRefWednesday,
                                cmd_data.dayIdRefThursday,
                                cmd_data.dayIdRefFriday,
                                cmd_data.dayIdRefSaturday,
                                cmd_data.dayIdRefSunday);

  // The PublishWeekProfile command is published in response to a
  // GetWeekProfile command.  If the IssuerCalendarID does not match with one
  // of the stored calendar instances, the client shall ignore the command and
  // respond using ZCL Default Response with a status response of NOT_FOUND.

  // The Price server shall send only the number of WeekProfiles belonging to
  // this calendar instance.  Server and clients shall be able to store at
  // least 4 WeekProfiles.  If the client is not able to store all entries, the
  // device should respond using ZCL Default Response with a status response of
  // INSUFFICIENT_SPACE.

  calendar = findCalendar(cmd_data.providerId, cmd_data.issuerCalendarId);
  if (calendar == NULL) {
    emberAfDebugPrintln("ERR: Calendar not found: 0x%4x/0x%4x",
                        cmd_data.providerId,
                        cmd_data.issuerCalendarId);
    status = EMBER_ZCL_STATUS_NOT_FOUND;
  } else if (cmd_data.weekId == 0) {
    emberAfDebugPrintln("ERR: Day id cannot be zero");
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (calendar->numberOfWeekProfiles < cmd_data.weekId) {
    emberAfDebugPrintln("ERR: Invalid week: %d < %d",
                        calendar->numberOfWeekProfiles,
                        cmd_data.weekId);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_WEEK_PROFILES < cmd_data.weekId) {
    // This should never happen because of the previous check and the one in
    // PublishCalendar.  It is here for completeness.
    emberAfDebugPrintln("ERR: Insufficient space for week: %d < %d",
                        EMBER_AF_PLUGIN_CALENDAR_CLIENT_WEEK_PROFILES,
                        cmd_data.weekId);
    status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
  } else {
    EmberAfCalendarWeekProfile *weekProfile = &calendar->weekProfiles[cmd_data.weekId - 1];
    weekProfile->inUse = true;
    status = EMBER_ZCL_STATUS_SUCCESS;
    if (calendar->numberOfDayProfiles < cmd_data.dayIdRefMonday) {
      emberAfDebugPrintln("ERR: Invalid %pday reference: %d < %d",
                          "Mon",
                          calendar->numberOfDayProfiles,
                          cmd_data.dayIdRefMonday);
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
    } else {
      weekProfile->dayIdRefMonday = cmd_data.dayIdRefMonday;
    }
    if (calendar->numberOfDayProfiles < cmd_data.dayIdRefTuesday) {
      emberAfDebugPrintln("ERR: Invalid %pday reference: %d < %d",
                          "Tues",
                          calendar->numberOfDayProfiles,
                          cmd_data.dayIdRefTuesday);
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
    } else {
      weekProfile->dayIdRefTuesday = cmd_data.dayIdRefTuesday;
    }
    if (calendar->numberOfDayProfiles < cmd_data.dayIdRefWednesday) {
      emberAfDebugPrintln("ERR: Invalid %pday reference: %d < %d",
                          "Wednes",
                          calendar->numberOfDayProfiles,
                          cmd_data.dayIdRefWednesday);
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
    } else {
      weekProfile->dayIdRefWednesday = cmd_data.dayIdRefWednesday;
    }
    if (calendar->numberOfDayProfiles < cmd_data.dayIdRefThursday) {
      emberAfDebugPrintln("ERR: Invalid %pday reference: %d < %d",
                          "Thurs",
                          calendar->numberOfDayProfiles,
                          cmd_data.dayIdRefThursday);
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
    } else {
      weekProfile->dayIdRefThursday = cmd_data.dayIdRefThursday;
    }
    if (calendar->numberOfDayProfiles < cmd_data.dayIdRefFriday) {
      emberAfDebugPrintln("ERR: Invalid %pday reference: %d < %d",
                          "Fri",
                          calendar->numberOfDayProfiles,
                          cmd_data.dayIdRefFriday);
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
    } else {
      weekProfile->dayIdRefFriday = cmd_data.dayIdRefFriday;
    }
    if (calendar->numberOfDayProfiles < cmd_data.dayIdRefSaturday) {
      emberAfDebugPrintln("ERR: Invalid %pday reference: %d < %d",
                          "Satur",
                          calendar->numberOfDayProfiles,
                          cmd_data.dayIdRefSaturday);
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
    } else {
      weekProfile->dayIdRefSaturday = cmd_data.dayIdRefSaturday;
    }
    if (calendar->numberOfDayProfiles < cmd_data.dayIdRefSunday) {
      emberAfDebugPrintln("ERR: Invalid %pday reference: %d < %d",
                          "Sun",
                          calendar->numberOfDayProfiles,
                          cmd_data.dayIdRefSunday);
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
    } else {
      weekProfile->dayIdRefSunday = cmd_data.dayIdRefSunday;
    }
  }

  if (calendar != NULL && status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfDebugPrintln("ERR: Removing invalid calendar: 0x%4x/0x%4x",
                        calendar->providerId,
                        calendar->issuerCalendarId);
    removeCalendar(calendar);
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfCalendarClusterPublishSeasonsCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_calendar_cluster_publish_seasons_command_t cmd_data;
  EmberAfStatus status;
  EmberAfCalendar *calendar;
  uint16_t seasonEntryCount;
  uint16_t seasonEntryIndex;

  if (zcl_decode_calendar_cluster_publish_seasons_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  seasonEntryCount = fieldLength(cmd_data.seasonEntries) / (sizeof(uint32_t) + sizeof(uint8_t));

  emberAfCalendarClusterPrint("RX: PublishSeasons 0x%4x, 0x%4x, 0x%4x, %d, %d, [",
                              cmd_data.providerId,
                              cmd_data.issuerEventId,
                              cmd_data.issuerCalendarId,
                              cmd_data.commandIndex,
                              cmd_data.totalNumberOfCommands);

  for (seasonEntryIndex = 0; seasonEntryIndex < seasonEntryCount; seasonEntryIndex++) {
    uint32_t seasonStartDate = cmd_data.seasonEntries[seasonEntryIndex * (sizeof(uint32_t) + sizeof(uint8_t))];
    uint8_t weekIdRef = cmd_data.seasonEntries[seasonEntryIndex * (sizeof(uint32_t) + sizeof(uint8_t)) + sizeof(uint32_t)];
    emberAfCalendarClusterPrint("0x%4X, 0x%X, ", seasonStartDate, weekIdRef);
  }
  emberAfCalendarClusterPrintln("]");

  // The PublishSeasons command is published in response to a GetSeason
  // command.  If the IssuerCalendarID does not match with one of the stored
  // calendar instances, the client shall ignore the command and respond using
  // ZCL Default Response with a status response of NOT_FOUND.

  // The Price server shall send only the number of SeasonEntries belonging to
  // this calendar instance.  Server and clients shall be able to store at
  // least 4 SeasonEntries.  If the client is not able to store all Season
  // Entries, the device should respond using ZCL Default Response with a
  // status response of INSUFFICIENT_SPACE.

  calendar = findCalendar(cmd_data.providerId, cmd_data.issuerCalendarId);
  if (calendar == NULL) {
    emberAfDebugPrintln("ERR: Calendar not found: 0x%4x/0x%4x",
                        cmd_data.providerId,
                        cmd_data.issuerCalendarId);
    status = EMBER_ZCL_STATUS_NOT_FOUND;
  } else if (cmd_data.totalNumberOfCommands <= cmd_data.commandIndex) {
    emberAfDebugPrintln("ERR: Inconsistent command index: %d <= %d",
                        cmd_data.totalNumberOfCommands,
                        cmd_data.commandIndex);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else {
    uint16_t seasonEntriesLength = fieldLength(cmd_data.seasonEntries);
    uint16_t seasonEntriesIndex = 0;

    status = EMBER_ZCL_STATUS_SUCCESS;
    while (seasonEntriesIndex < seasonEntriesLength
           && status == EMBER_ZCL_STATUS_SUCCESS) {
      if (seasonEntriesLength < seasonEntriesIndex + 5) {
        emberAfDebugPrintln("ERR: Malformed season");
        status = EMBER_ZCL_STATUS_MALFORMED_COMMAND;
      } else if (calendar->numberOfSeasons <= calendar->receivedSeasons) {
        emberAfDebugPrintln("ERR: Inconsistent number of received seasons: %d <= %d",
                            calendar->numberOfSeasons,
                            calendar->receivedSeasons);
        status = EMBER_ZCL_STATUS_INVALID_FIELD;
      } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_SEASONS
                 <= calendar->receivedSeasons) {
        // This should never happen because of the previous check and the one
        // in PublishCalendar.  It is here for completeness.
        emberAfDebugPrintln("ERR: Insufficient space for received seasons: %d <= %d",
                            EMBER_AF_PLUGIN_CALENDAR_CLIENT_SEASONS,
                            calendar->receivedSeasons);
        status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
      } else {
        EmberAfCalendarSeason *season = &calendar->seasons[calendar->receivedSeasons++];
        seasonEntriesIndex += emberAfGetDate(cmd_data.seasonEntries,
                                             seasonEntriesIndex,
                                             seasonEntriesLength,
                                             &season->seasonStartDate);
        season->weekIdRef = emberAfGetInt8u(cmd_data.seasonEntries,
                                            seasonEntriesIndex,
                                            seasonEntriesLength);
        seasonEntriesIndex++;
        if (season != calendar->seasons
            && emberAfCompareDates(&season->seasonStartDate, &((season - 1)->seasonStartDate)) <= 0) {
          emberAfDebugPrintln("ERR: Inconsistent start dates: 0x%4x <= 0x%4x",
                              season->seasonStartDate,
                              (season - 1)->seasonStartDate);
          status = EMBER_ZCL_STATUS_INVALID_FIELD;
        } else if (calendar->numberOfWeekProfiles < season->weekIdRef) {
          emberAfDebugPrintln("ERR: Invalid week reference: %d < %d",
                              calendar->numberOfWeekProfiles,
                              season->weekIdRef);
          status = EMBER_ZCL_STATUS_INVALID_FIELD;
        }
      }
    }
  }

  if (calendar != NULL && status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfDebugPrintln("ERR: Removing invalid calendar: 0x%4x/0x%4x",
                        calendar->providerId,
                        calendar->issuerCalendarId);
    removeCalendar(calendar);
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfCalendarClusterPublishSpecialDaysCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_calendar_cluster_publish_special_days_command_t cmd_data;
  EmberAfStatus status;
  EmberAfCalendar *calendar;

  if (zcl_decode_calendar_cluster_publish_special_days_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfCalendarClusterPrint("RX: PublishSpecialDays 0x%4x, 0x%4x, 0x%4x, 0x%4x, 0x%x, %d, %d, %d, [",
                              cmd_data.providerId,
                              cmd_data.issuerEventId,
                              cmd_data.issuerCalendarId,
                              cmd_data.startTime,
                              cmd_data.calendarType,
                              cmd_data.totalNumberOfSpecialDays,
                              cmd_data.commandIndex,
                              cmd_data.totalNumberOfCommands);
  // TODO: print specialDayEntries
  emberAfCalendarClusterPrintln("]");

  // If the Calendar Type does not match with one of the stored calendar
  // instances, the client shall ignore the command and respond using ZCL
  // Default Response with a status response of NOT_FOUND.

  // Server and clients shall be able to store at least 50 SpecialDayEntries.
  // If the client is not able to store all SpecialDayEntries, the device
  // should respond using ZCL Default Response with a status response of
  // INSUFFICIENT_SPACE.

  // If the maximum application payload is not sufficient to transfer all
  // SpecialDayEntries in one command, the ESI may send as many
  // PublishSpecialDays commands as needed.
  // Note that, in this case, it is the client's responsibility to ensure that
  // it receives all associated PublishSpecialDays commands before any of the
  // payloads can be used.

  calendar = findCalendar(cmd_data.providerId, cmd_data.issuerCalendarId);
  if (calendar == NULL) {
    emberAfDebugPrintln("ERR: Calendar not found: 0x%4x/0x%4x",
                        cmd_data.providerId,
                        cmd_data.issuerCalendarId);
    status = EMBER_ZCL_STATUS_NOT_FOUND;
  } else if (calendar->calendarType != cmd_data.calendarType) {
    emberAfDebugPrintln("ERR: Inconsistent calendar type: 0x%x != 0x%x",
                        calendar->calendarType,
                        cmd_data.calendarType);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_SPECIAL_DAY_ENTRIES
             < cmd_data.totalNumberOfSpecialDays) {
    emberAfDebugPrintln("ERR: Insufficient space for special day entries: %d < %d",
                        EMBER_AF_PLUGIN_CALENDAR_CLIENT_SPECIAL_DAY_ENTRIES,
                        cmd_data.totalNumberOfSpecialDays);
    status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
  } else if (cmd_data.totalNumberOfCommands <= cmd_data.commandIndex) {
    emberAfDebugPrintln("ERR: Inconsistent command index: %d <= %d",
                        cmd_data.totalNumberOfCommands,
                        cmd_data.commandIndex);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else {
    uint16_t specialDayEntriesLength = fieldLength(cmd_data.specialDayEntries);
    uint16_t specialDayEntriesIndex = 0;

    status = EMBER_ZCL_STATUS_SUCCESS;
    if (!calendar->specialDayProfile.inUse) {
      calendar->specialDayProfile.inUse = true;
      calendar->specialDayProfile.startTimeUtc = cmd_data.startTime;
    }
    calendar->specialDayProfile.numberOfSpecialDayEntries = cmd_data.totalNumberOfSpecialDays;
    calendar->specialDayProfile.receivedSpecialDayEntries = 0;

    while (specialDayEntriesIndex < specialDayEntriesLength
           && status == EMBER_ZCL_STATUS_SUCCESS) {
      if (specialDayEntriesLength < specialDayEntriesIndex + 5) {
        emberAfDebugPrintln("ERR: Malformed special day entry");
        status = EMBER_ZCL_STATUS_MALFORMED_COMMAND;
      } else if (calendar->specialDayProfile.numberOfSpecialDayEntries
                 <= calendar->specialDayProfile.receivedSpecialDayEntries) {
        emberAfDebugPrintln("ERR: Inconsistent number of received special day entries: %d <= %d",
                            calendar->specialDayProfile.numberOfSpecialDayEntries,
                            calendar->specialDayProfile.receivedSpecialDayEntries);
        status = EMBER_ZCL_STATUS_INVALID_FIELD;
      } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_SPECIAL_DAY_ENTRIES
                 <= calendar->specialDayProfile.receivedSpecialDayEntries) {
        // This should never happen because of the checks above.  It is here
        // for completeness.
        emberAfDebugPrintln("ERR: Insufficient space for received special day entries: %d <= %d",
                            EMBER_AF_PLUGIN_CALENDAR_CLIENT_SPECIAL_DAY_ENTRIES,
                            calendar->specialDayProfile.receivedSpecialDayEntries);
        status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
      } else {
        EmberAfCalendarSpecialDayEntry *specialDayEntry = &calendar->specialDayProfile.specialDayEntries[calendar->specialDayProfile.receivedSpecialDayEntries++];
        specialDayEntriesIndex += emberAfGetDate(cmd_data.specialDayEntries,
                                                 specialDayEntriesIndex,
                                                 specialDayEntriesLength,
                                                 &specialDayEntry->specialDayDate);;
        specialDayEntry->dayIdRef = emberAfGetInt8u(cmd_data.specialDayEntries,
                                                    specialDayEntriesIndex,
                                                    specialDayEntriesLength);
        specialDayEntriesIndex++;
        if (calendar->numberOfDayProfiles < specialDayEntry->dayIdRef) {
          emberAfDebugPrintln("ERR: Invalid day reference: %d < %d",
                              calendar->numberOfDayProfiles,
                              specialDayEntry->dayIdRef);
          status = EMBER_ZCL_STATUS_INVALID_FIELD;
        }
      }
    }
  }

  if (calendar != NULL && status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfDebugPrintln("ERR: Removing invalid calendar: 0x%4x/0x%4x",
                        calendar->providerId,
                        calendar->issuerCalendarId);
    removeCalendar(calendar);
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfCalendarClusterCancelCalendarCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_calendar_cluster_cancel_calendar_command_t cmd_data;
  EmberAfStatus status;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t index = emberAfFindClusterClientEndpointIndex(endpoint,
                                                        ZCL_CALENDAR_CLUSTER_ID);

  if (zcl_decode_calendar_cluster_cancel_calendar_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfCalendarClusterPrintln("RX: CancelCalendar 0x%4x, 0x%4x, 0x%x",
                                cmd_data.providerId,
                                cmd_data.issuerCalendarId,
                                cmd_data.calendarType);

  // The client device shall discard all instances of PublishCalendar,
  // PublishDayProfile, PublishWeekProfile, PublishSeasons, and
  // PublishSpecialDays commands associated with the stated Provider ID,
  // Calendar Type, and Issuer Calendar ID.

  if (EMBER_ZCL_CALENDAR_TYPE_AUXILLIARY_LOAD_SWITCH_CALENDAR < cmd_data.calendarType) {
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else {
    uint8_t i;
    status = EMBER_ZCL_STATUS_NOT_FOUND;
    for (i = 0; i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS; i++) {
      if (cmd_data.providerId == calendars[index][i].providerId
          && cmd_data.issuerCalendarId == calendars[index][i].issuerCalendarId
          && cmd_data.calendarType  == calendars[index][i].calendarType) {
        calendars[index][i].inUse = false;
        status = EMBER_ZCL_STATUS_SUCCESS;
      }
    }
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

#else // !UC_BUILD

bool emberAfCalendarClusterPublishCalendarCallback(uint32_t providerId,
                                                   uint32_t issuerEventId,
                                                   uint32_t issuerCalendarId,
                                                   uint32_t startTime,
                                                   uint8_t calendarType,
                                                   uint8_t calendarTimeReference,
                                                   uint8_t *calendarName,
                                                   uint8_t numberOfSeasons,
                                                   uint8_t numberOfWeekProfiles,
                                                   uint8_t numberOfDayProfiles)
{
  EmberAfStatus status;
  emberAfCalendarClusterPrint("RX: PublishCalendar 0x%4x, 0x%4x, 0x%4x, 0x%4x, 0x%x, \"",
                              providerId,
                              issuerEventId,
                              issuerCalendarId,
                              startTime,
                              calendarType);
  emberAfCalendarClusterPrintString(calendarName);
  emberAfCalendarClusterPrintln("\", %d, %d, %d",
                                numberOfSeasons,
                                numberOfWeekProfiles,
                                numberOfDayProfiles);

  if (findCalendar(providerId, issuerCalendarId) != NULL) {
    emberAfDebugPrintln("ERR: Duplicate calendar: 0x%4x/0x%4x",
                        providerId,
                        issuerCalendarId);
    status = EMBER_ZCL_STATUS_DUPLICATE_EXISTS;
  } else if (EMBER_ZCL_CALENDAR_TYPE_AUXILLIARY_LOAD_SWITCH_CALENDAR
             < calendarType) {
    emberAfDebugPrintln("ERR: Invalid calendar type: 0x%x", calendarType);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (EMBER_ZCL_CALENDAR_TIME_REFERENCE_UTC_TIME
             != calendarTimeReference) {
    emberAfDebugPrintln("ERR: Invalid calendar time reference: 0x%x", calendarTimeReference);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (0 < numberOfSeasons && numberOfWeekProfiles == 0) {
    emberAfDebugPrintln("ERR: Number of week profiles cannot be zero");
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_SEASONS < numberOfSeasons) {
    emberAfDebugPrintln("ERR: Insufficient space for seasons: %d < %d",
                        EMBER_AF_PLUGIN_CALENDAR_CLIENT_SEASONS,
                        numberOfSeasons);
    status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
  } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_WEEK_PROFILES
             < numberOfWeekProfiles) {
    emberAfDebugPrintln("ERR: Insufficient space for week profiles: %d < %d",
                        EMBER_AF_PLUGIN_CALENDAR_CLIENT_WEEK_PROFILES,
                        numberOfWeekProfiles);
    status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
  } else if (numberOfDayProfiles == 0) {
    emberAfDebugPrintln("ERR: Number of day profiles cannot be zero");
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_DAY_PROFILES
             < numberOfDayProfiles) {
    emberAfDebugPrintln("ERR: Insufficient space for day profiles: %d < %d",
                        EMBER_AF_PLUGIN_CALENDAR_CLIENT_DAY_PROFILES,
                        numberOfDayProfiles);
    status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
  } else {
    uint8_t i;
    tempCalendar.inUse = true;
    tempCalendar.providerId = providerId;
    tempCalendar.issuerEventId = issuerEventId;
    tempCalendar.issuerCalendarId = issuerCalendarId;
    tempCalendar.startTimeUtc = startTime;
    tempCalendar.calendarType = calendarType;
    emberAfCopyString(tempCalendar.calendarName,
                      calendarName,
                      EMBER_AF_CALENDAR_MAXIMUM_CALENDAR_NAME_LENGTH);
    tempCalendar.numberOfSeasons = numberOfSeasons;
    tempCalendar.receivedSeasons = 0;
    tempCalendar.numberOfWeekProfiles = numberOfWeekProfiles;
    tempCalendar.numberOfDayProfiles = numberOfDayProfiles;
    for (i = 0; i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_WEEK_PROFILES; i++) {
      tempCalendar.weekProfiles[i].inUse = false;
    }
    for (i = 0; i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_DAY_PROFILES; i++) {
      tempCalendar.dayProfiles[i].inUse = false;
    }
    tempCalendar.specialDayProfile.inUse = false;
    status = addCalendar(&tempCalendar);
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfCalendarClusterPublishDayProfileCallback(uint32_t providerId,
                                                     uint32_t issuerEventId,
                                                     uint32_t issuerCalendarId,
                                                     uint8_t dayId,
                                                     uint8_t totalNumberOfScheduleEntries,
                                                     uint8_t commandIndex,
                                                     uint8_t totalNumberOfCommands,
                                                     uint8_t calendarType,
                                                     uint8_t *dayScheduleEntries)
{
  EmberAfStatus status;
  EmberAfCalendar *calendar;

  emberAfCalendarClusterPrint("RX: PublishDayProfile 0x%4x, 0x%4x, 0x%4x, %d, %d, %d, %d, 0x%x, [",
                              providerId,
                              issuerEventId,
                              issuerCalendarId,
                              dayId,
                              totalNumberOfScheduleEntries,
                              commandIndex,
                              totalNumberOfCommands,
                              calendarType);
  // TODO: print dayScheduleEntries
  emberAfCalendarClusterPrintln("]");

  // The PublishDayProfile command is published in response to a GetDayProfile
  // command.  If the IssuerCalendarID does not match with one of the stored
  // calendar instances, the client shall ignore the command and respond using
  // ZCL Default Response with a status response of NOT_FOUND.

  // The Calendar server shall send only the number of Schedule Entries
  // belonging to this calendar instance.  Server and clients shall be able to
  // store at least 1 DayProfile and at least one ScheduleEntries per day
  // profile.  If the client is not able to store all ScheduleEntries, the
  // device should respond using ZCL Default Response with a status response of
  // INSUFFICIENT_SPACE.

  calendar = findCalendar(providerId, issuerCalendarId);
  if (calendar == NULL) {
    emberAfDebugPrintln("ERR: Calendar not found: 0x%4x/0x%4x",
                        providerId,
                        issuerCalendarId);
    status = EMBER_ZCL_STATUS_NOT_FOUND;
  } else if (dayId == 0) {
    emberAfDebugPrintln("ERR: Day id cannot be zero");
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (calendar->numberOfDayProfiles < dayId) {
    emberAfDebugPrintln("ERR: Invalid day: %d < %d",
                        calendar->numberOfDayProfiles,
                        dayId);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_DAY_PROFILES < dayId) {
    // This should never happen because of the previous check and the one in
    // PublishCalendar.  It is here for completeness.
    emberAfDebugPrintln("ERR: Insufficient space for day: %d < %d",
                        EMBER_AF_PLUGIN_CALENDAR_CLIENT_DAY_PROFILES,
                        dayId);
    status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
  } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_SCHEDULE_ENTRIES
             < totalNumberOfScheduleEntries) {
    emberAfDebugPrintln("ERR: Insufficient space for schedule entries: %d < %d",
                        EMBER_AF_PLUGIN_CALENDAR_CLIENT_SCHEDULE_ENTRIES,
                        totalNumberOfScheduleEntries);
    status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
  } else if (totalNumberOfCommands <= commandIndex) {
    emberAfDebugPrintln("ERR: Inconsistent command index: %d <= %d",
                        totalNumberOfCommands,
                        commandIndex);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (calendar->calendarType != calendarType) {
    emberAfDebugPrintln("ERR: Inconsistent calendar type: 0x%x != 0x%x",
                        calendar->calendarType,
                        calendarType);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else {
    EmberAfCalendarDayProfile *dayProfile = &calendar->dayProfiles[dayId - 1];
    uint16_t dayScheduleEntriesLength = fieldLength(dayScheduleEntries);
    uint16_t dayScheduleEntriesIndex = 0;

    status = EMBER_ZCL_STATUS_SUCCESS;
    if (!dayProfile->inUse) {
      dayProfile->inUse = true;
      dayProfile->numberOfScheduleEntries = totalNumberOfScheduleEntries;
    } else if (dayProfile->numberOfScheduleEntries
               != totalNumberOfScheduleEntries) {
      emberAfDebugPrintln("ERR: Inconsistent number of schedule entries: 0x%x != 0x%x",
                          dayProfile->numberOfScheduleEntries,
                          totalNumberOfScheduleEntries);
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
    }

    dayProfile->receivedScheduleEntries = 0;
    while (dayScheduleEntriesIndex < dayScheduleEntriesLength
           && status == EMBER_ZCL_STATUS_SUCCESS) {
      // The rate switch times, friendly credit switch times, and auxilliary
      // load switch times all use a two-byte start time followed by a one-
      // byte value.  These are the only valid types, so all entries must have
      // at least three bytes and, below, we just stuff the bytes into the rate
      // switch time entry in the union to simplify the code.
      if (dayScheduleEntriesLength < dayScheduleEntriesIndex + 3) {
        emberAfDebugPrintln("ERR: Malformed schedule entry");
        status = EMBER_ZCL_STATUS_MALFORMED_COMMAND;
      } else if (dayProfile->numberOfScheduleEntries
                 <= dayProfile->receivedScheduleEntries) {
        emberAfDebugPrintln("ERR: Inconsistent number of received schedule entries: %d <= %d",
                            dayProfile->numberOfScheduleEntries,
                            dayProfile->receivedScheduleEntries);
        status = EMBER_ZCL_STATUS_INVALID_FIELD;
      } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_SCHEDULE_ENTRIES
                 <= dayProfile->receivedScheduleEntries) {
        // This should never happen because of the checks above.  It is here
        // for completeness.
        emberAfDebugPrintln("ERR: Insufficient space for received schedule entries: %d <= %d",
                            EMBER_AF_PLUGIN_CALENDAR_CLIENT_SCHEDULE_ENTRIES,
                            dayProfile->receivedScheduleEntries);
        status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
      } else {
        EmberAfCalendarScheduleEntry *scheduleEntry = &dayProfile->scheduleEntries[dayProfile->receivedScheduleEntries++];
        scheduleEntry->rateSwitchTime.startTimeM = emberAfGetInt16u(dayScheduleEntries,
                                                                    dayScheduleEntriesIndex,
                                                                    dayScheduleEntriesLength);
        dayScheduleEntriesIndex += 2;
        scheduleEntry->rateSwitchTime.priceTier = emberAfGetInt8u(dayScheduleEntries,
                                                                  dayScheduleEntriesIndex,
                                                                  dayScheduleEntriesLength);
        dayScheduleEntriesIndex++;
        if (scheduleEntry != dayProfile->scheduleEntries) {
          EmberAfCalendarScheduleEntry *previous = scheduleEntry - 1;
          if (scheduleEntry->rateSwitchTime.startTimeM
              <= previous->rateSwitchTime.startTimeM) {
            emberAfDebugPrintln("ERR: Inconsistent start times: 0x%2x <= 0x%2x",
                                scheduleEntry->rateSwitchTime.startTimeM,
                                previous->rateSwitchTime.startTimeM);
            status = EMBER_ZCL_STATUS_INVALID_FIELD;
          }
        }
      }
    }
  }

  if (calendar != NULL && status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfDebugPrintln("ERR: Removing invalid calendar: 0x%4x/0x%4x",
                        calendar->providerId,
                        calendar->issuerCalendarId);
    removeCalendar(calendar);
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfCalendarClusterPublishWeekProfileCallback(uint32_t providerId,
                                                      uint32_t issuerEventId,
                                                      uint32_t issuerCalendarId,
                                                      uint8_t weekId,
                                                      uint8_t dayIdRefMonday,
                                                      uint8_t dayIdRefTuesday,
                                                      uint8_t dayIdRefWednesday,
                                                      uint8_t dayIdRefThursday,
                                                      uint8_t dayIdRefFriday,
                                                      uint8_t dayIdRefSaturday,
                                                      uint8_t dayIdRefSunday)
{
  EmberAfStatus status;
  EmberAfCalendar *calendar;

  emberAfCalendarClusterPrintln("RX: PublishWeekProfile 0x%4x, 0x%4x, 0x%4x, %d, %d, %d, %d, %d, %d, %d, %d",
                                providerId,
                                issuerEventId,
                                issuerCalendarId,
                                weekId,
                                dayIdRefMonday,
                                dayIdRefTuesday,
                                dayIdRefWednesday,
                                dayIdRefThursday,
                                dayIdRefFriday,
                                dayIdRefSaturday,
                                dayIdRefSunday);

  // The PublishWeekProfile command is published in response to a
  // GetWeekProfile command.  If the IssuerCalendarID does not match with one
  // of the stored calendar instances, the client shall ignore the command and
  // respond using ZCL Default Response with a status response of NOT_FOUND.

  // The Price server shall send only the number of WeekProfiles belonging to
  // this calendar instance.  Server and clients shall be able to store at
  // least 4 WeekProfiles.  If the client is not able to store all entries, the
  // device should respond using ZCL Default Response with a status response of
  // INSUFFICIENT_SPACE.

  calendar = findCalendar(providerId, issuerCalendarId);
  if (calendar == NULL) {
    emberAfDebugPrintln("ERR: Calendar not found: 0x%4x/0x%4x",
                        providerId,
                        issuerCalendarId);
    status = EMBER_ZCL_STATUS_NOT_FOUND;
  } else if (weekId == 0) {
    emberAfDebugPrintln("ERR: Day id cannot be zero");
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (calendar->numberOfWeekProfiles < weekId) {
    emberAfDebugPrintln("ERR: Invalid week: %d < %d",
                        calendar->numberOfWeekProfiles,
                        weekId);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_WEEK_PROFILES < weekId) {
    // This should never happen because of the previous check and the one in
    // PublishCalendar.  It is here for completeness.
    emberAfDebugPrintln("ERR: Insufficient space for week: %d < %d",
                        EMBER_AF_PLUGIN_CALENDAR_CLIENT_WEEK_PROFILES,
                        weekId);
    status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
  } else {
    EmberAfCalendarWeekProfile *weekProfile = &calendar->weekProfiles[weekId - 1];
    weekProfile->inUse = true;
    status = EMBER_ZCL_STATUS_SUCCESS;
    if (calendar->numberOfDayProfiles < dayIdRefMonday) {
      emberAfDebugPrintln("ERR: Invalid %pday reference: %d < %d",
                          "Mon",
                          calendar->numberOfDayProfiles,
                          dayIdRefMonday);
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
    } else {
      weekProfile->dayIdRefMonday = dayIdRefMonday;
    }
    if (calendar->numberOfDayProfiles < dayIdRefTuesday) {
      emberAfDebugPrintln("ERR: Invalid %pday reference: %d < %d",
                          "Tues",
                          calendar->numberOfDayProfiles,
                          dayIdRefTuesday);
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
    } else {
      weekProfile->dayIdRefTuesday = dayIdRefTuesday;
    }
    if (calendar->numberOfDayProfiles < dayIdRefWednesday) {
      emberAfDebugPrintln("ERR: Invalid %pday reference: %d < %d",
                          "Wednes",
                          calendar->numberOfDayProfiles,
                          dayIdRefWednesday);
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
    } else {
      weekProfile->dayIdRefWednesday = dayIdRefWednesday;
    }
    if (calendar->numberOfDayProfiles < dayIdRefThursday) {
      emberAfDebugPrintln("ERR: Invalid %pday reference: %d < %d",
                          "Thurs",
                          calendar->numberOfDayProfiles,
                          dayIdRefThursday);
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
    } else {
      weekProfile->dayIdRefThursday = dayIdRefThursday;
    }
    if (calendar->numberOfDayProfiles < dayIdRefFriday) {
      emberAfDebugPrintln("ERR: Invalid %pday reference: %d < %d",
                          "Fri",
                          calendar->numberOfDayProfiles,
                          dayIdRefFriday);
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
    } else {
      weekProfile->dayIdRefFriday = dayIdRefFriday;
    }
    if (calendar->numberOfDayProfiles < dayIdRefSaturday) {
      emberAfDebugPrintln("ERR: Invalid %pday reference: %d < %d",
                          "Satur",
                          calendar->numberOfDayProfiles,
                          dayIdRefSaturday);
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
    } else {
      weekProfile->dayIdRefSaturday = dayIdRefSaturday;
    }
    if (calendar->numberOfDayProfiles < dayIdRefSunday) {
      emberAfDebugPrintln("ERR: Invalid %pday reference: %d < %d",
                          "Sun",
                          calendar->numberOfDayProfiles,
                          dayIdRefSunday);
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
    } else {
      weekProfile->dayIdRefSunday = dayIdRefSunday;
    }
  }

  if (calendar != NULL && status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfDebugPrintln("ERR: Removing invalid calendar: 0x%4x/0x%4x",
                        calendar->providerId,
                        calendar->issuerCalendarId);
    removeCalendar(calendar);
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfCalendarClusterPublishSeasonsCallback(uint32_t providerId,
                                                  uint32_t issuerEventId,
                                                  uint32_t issuerCalendarId,
                                                  uint8_t commandIndex,
                                                  uint8_t totalNumberOfCommands,
                                                  uint8_t *seasonEntries)
{
  EmberAfStatus status;
  EmberAfCalendar *calendar;
  uint16_t seasonEntryCount = fieldLength(seasonEntries) / (sizeof(uint32_t) + sizeof(uint8_t));
  uint16_t seasonEntryIndex;

  emberAfCalendarClusterPrint("RX: PublishSeasons 0x%4x, 0x%4x, 0x%4x, %d, %d, [",
                              providerId,
                              issuerEventId,
                              issuerCalendarId,
                              commandIndex,
                              totalNumberOfCommands);

  for (seasonEntryIndex = 0; seasonEntryIndex < seasonEntryCount; seasonEntryIndex++) {
    uint32_t seasonStartDate = seasonEntries[seasonEntryIndex * (sizeof(uint32_t) + sizeof(uint8_t))];
    uint8_t weekIdRef = seasonEntries[seasonEntryIndex * (sizeof(uint32_t) + sizeof(uint8_t)) + sizeof(uint32_t)];
    emberAfCalendarClusterPrint("0x%4X, 0x%X, ", seasonStartDate, weekIdRef);
  }
  emberAfCalendarClusterPrintln("]");

  // The PublishSeasons command is published in response to a GetSeason
  // command.  If the IssuerCalendarID does not match with one of the stored
  // calendar instances, the client shall ignore the command and respond using
  // ZCL Default Response with a status response of NOT_FOUND.

  // The Price server shall send only the number of SeasonEntries belonging to
  // this calendar instance.  Server and clients shall be able to store at
  // least 4 SeasonEntries.  If the client is not able to store all Season
  // Entries, the device should respond using ZCL Default Response with a
  // status response of INSUFFICIENT_SPACE.

  calendar = findCalendar(providerId, issuerCalendarId);
  if (calendar == NULL) {
    emberAfDebugPrintln("ERR: Calendar not found: 0x%4x/0x%4x",
                        providerId,
                        issuerCalendarId);
    status = EMBER_ZCL_STATUS_NOT_FOUND;
  } else if (totalNumberOfCommands <= commandIndex) {
    emberAfDebugPrintln("ERR: Inconsistent command index: %d <= %d",
                        totalNumberOfCommands,
                        commandIndex);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else {
    uint16_t seasonEntriesLength = fieldLength(seasonEntries);
    uint16_t seasonEntriesIndex = 0;

    status = EMBER_ZCL_STATUS_SUCCESS;
    while (seasonEntriesIndex < seasonEntriesLength
           && status == EMBER_ZCL_STATUS_SUCCESS) {
      if (seasonEntriesLength < seasonEntriesIndex + 5) {
        emberAfDebugPrintln("ERR: Malformed season");
        status = EMBER_ZCL_STATUS_MALFORMED_COMMAND;
      } else if (calendar->numberOfSeasons <= calendar->receivedSeasons) {
        emberAfDebugPrintln("ERR: Inconsistent number of received seasons: %d <= %d",
                            calendar->numberOfSeasons,
                            calendar->receivedSeasons);
        status = EMBER_ZCL_STATUS_INVALID_FIELD;
      } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_SEASONS
                 <= calendar->receivedSeasons) {
        // This should never happen because of the previous check and the one
        // in PublishCalendar.  It is here for completeness.
        emberAfDebugPrintln("ERR: Insufficient space for received seasons: %d <= %d",
                            EMBER_AF_PLUGIN_CALENDAR_CLIENT_SEASONS,
                            calendar->receivedSeasons);
        status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
      } else {
        EmberAfCalendarSeason *season = &calendar->seasons[calendar->receivedSeasons++];
        seasonEntriesIndex += emberAfGetDate(seasonEntries,
                                             seasonEntriesIndex,
                                             seasonEntriesLength,
                                             &season->seasonStartDate);
        season->weekIdRef = emberAfGetInt8u(seasonEntries,
                                            seasonEntriesIndex,
                                            seasonEntriesLength);
        seasonEntriesIndex++;
        if (season != calendar->seasons
            && emberAfCompareDates(&season->seasonStartDate, &((season - 1)->seasonStartDate)) <= 0) {
          emberAfDebugPrintln("ERR: Inconsistent start dates: 0x%4x <= 0x%4x",
                              season->seasonStartDate,
                              (season - 1)->seasonStartDate);
          status = EMBER_ZCL_STATUS_INVALID_FIELD;
        } else if (calendar->numberOfWeekProfiles < season->weekIdRef) {
          emberAfDebugPrintln("ERR: Invalid week reference: %d < %d",
                              calendar->numberOfWeekProfiles,
                              season->weekIdRef);
          status = EMBER_ZCL_STATUS_INVALID_FIELD;
        }
      }
    }
  }

  if (calendar != NULL && status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfDebugPrintln("ERR: Removing invalid calendar: 0x%4x/0x%4x",
                        calendar->providerId,
                        calendar->issuerCalendarId);
    removeCalendar(calendar);
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfCalendarClusterPublishSpecialDaysCallback(uint32_t providerId,
                                                      uint32_t issuerEventId,
                                                      uint32_t issuerCalendarId,
                                                      uint32_t startTime,
                                                      uint8_t calendarType,
                                                      uint8_t totalNumberOfSpecialDays,
                                                      uint8_t commandIndex,
                                                      uint8_t totalNumberOfCommands,
                                                      uint8_t *specialDayEntries)
{
  EmberAfStatus status;
  EmberAfCalendar *calendar;

  emberAfCalendarClusterPrint("RX: PublishSpecialDays 0x%4x, 0x%4x, 0x%4x, 0x%4x, 0x%x, %d, %d, %d, [",
                              providerId,
                              issuerEventId,
                              issuerCalendarId,
                              startTime,
                              calendarType,
                              totalNumberOfSpecialDays,
                              commandIndex,
                              totalNumberOfCommands);
  // TODO: print specialDayEntries
  emberAfCalendarClusterPrintln("]");

  // If the Calendar Type does not match with one of the stored calendar
  // instances, the client shall ignore the command and respond using ZCL
  // Default Response with a status response of NOT_FOUND.

  // Server and clients shall be able to store at least 50 SpecialDayEntries.
  // If the client is not able to store all SpecialDayEntries, the device
  // should respond using ZCL Default Response with a status response of
  // INSUFFICIENT_SPACE.

  // If the maximum application payload is not sufficient to transfer all
  // SpecialDayEntries in one command, the ESI may send as many
  // PublishSpecialDays commands as needed.
  // Note that, in this case, it is the client's responsibility to ensure that
  // it receives all associated PublishSpecialDays commands before any of the
  // payloads can be used.

  calendar = findCalendar(providerId, issuerCalendarId);
  if (calendar == NULL) {
    emberAfDebugPrintln("ERR: Calendar not found: 0x%4x/0x%4x",
                        providerId,
                        issuerCalendarId);
    status = EMBER_ZCL_STATUS_NOT_FOUND;
  } else if (calendar->calendarType != calendarType) {
    emberAfDebugPrintln("ERR: Inconsistent calendar type: 0x%x != 0x%x",
                        calendar->calendarType,
                        calendarType);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_SPECIAL_DAY_ENTRIES
             < totalNumberOfSpecialDays) {
    emberAfDebugPrintln("ERR: Insufficient space for special day entries: %d < %d",
                        EMBER_AF_PLUGIN_CALENDAR_CLIENT_SPECIAL_DAY_ENTRIES,
                        totalNumberOfSpecialDays);
    status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
  } else if (totalNumberOfCommands <= commandIndex) {
    emberAfDebugPrintln("ERR: Inconsistent command index: %d <= %d",
                        totalNumberOfCommands,
                        commandIndex);
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else {
    uint16_t specialDayEntriesLength = fieldLength(specialDayEntries);
    uint16_t specialDayEntriesIndex = 0;

    status = EMBER_ZCL_STATUS_SUCCESS;
    if (!calendar->specialDayProfile.inUse) {
      calendar->specialDayProfile.inUse = true;
      calendar->specialDayProfile.startTimeUtc = startTime;
    }
    calendar->specialDayProfile.numberOfSpecialDayEntries = totalNumberOfSpecialDays;
    calendar->specialDayProfile.receivedSpecialDayEntries = 0;

    while (specialDayEntriesIndex < specialDayEntriesLength
           && status == EMBER_ZCL_STATUS_SUCCESS) {
      if (specialDayEntriesLength < specialDayEntriesIndex + 5) {
        emberAfDebugPrintln("ERR: Malformed special day entry");
        status = EMBER_ZCL_STATUS_MALFORMED_COMMAND;
      } else if (calendar->specialDayProfile.numberOfSpecialDayEntries
                 <= calendar->specialDayProfile.receivedSpecialDayEntries) {
        emberAfDebugPrintln("ERR: Inconsistent number of received special day entries: %d <= %d",
                            calendar->specialDayProfile.numberOfSpecialDayEntries,
                            calendar->specialDayProfile.receivedSpecialDayEntries);
        status = EMBER_ZCL_STATUS_INVALID_FIELD;
      } else if (EMBER_AF_PLUGIN_CALENDAR_CLIENT_SPECIAL_DAY_ENTRIES
                 <= calendar->specialDayProfile.receivedSpecialDayEntries) {
        // This should never happen because of the checks above.  It is here
        // for completeness.
        emberAfDebugPrintln("ERR: Insufficient space for received special day entries: %d <= %d",
                            EMBER_AF_PLUGIN_CALENDAR_CLIENT_SPECIAL_DAY_ENTRIES,
                            calendar->specialDayProfile.receivedSpecialDayEntries);
        status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
      } else {
        EmberAfCalendarSpecialDayEntry *specialDayEntry = &calendar->specialDayProfile.specialDayEntries[calendar->specialDayProfile.receivedSpecialDayEntries++];
        specialDayEntriesIndex += emberAfGetDate(specialDayEntries,
                                                 specialDayEntriesIndex,
                                                 specialDayEntriesLength,
                                                 &specialDayEntry->specialDayDate);;
        specialDayEntry->dayIdRef = emberAfGetInt8u(specialDayEntries,
                                                    specialDayEntriesIndex,
                                                    specialDayEntriesLength);
        specialDayEntriesIndex++;
        if (calendar->numberOfDayProfiles < specialDayEntry->dayIdRef) {
          emberAfDebugPrintln("ERR: Invalid day reference: %d < %d",
                              calendar->numberOfDayProfiles,
                              specialDayEntry->dayIdRef);
          status = EMBER_ZCL_STATUS_INVALID_FIELD;
        }
      }
    }
  }

  if (calendar != NULL && status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfDebugPrintln("ERR: Removing invalid calendar: 0x%4x/0x%4x",
                        calendar->providerId,
                        calendar->issuerCalendarId);
    removeCalendar(calendar);
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfCalendarClusterCancelCalendarCallback(uint32_t providerId,
                                                  uint32_t issuerCalendarId,
                                                  uint8_t calendarType)
{
  EmberAfStatus status;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t index = emberAfFindClusterClientEndpointIndex(endpoint,
                                                        ZCL_CALENDAR_CLUSTER_ID);

  emberAfCalendarClusterPrintln("RX: CancelCalendar 0x%4x, 0x%4x, 0x%x",
                                providerId,
                                issuerCalendarId,
                                calendarType);

  // The client device shall discard all instances of PublishCalendar,
  // PublishDayProfile, PublishWeekProfile, PublishSeasons, and
  // PublishSpecialDays commands associated with the stated Provider ID,
  // Calendar Type, and Issuer Calendar ID.

  if (EMBER_ZCL_CALENDAR_TYPE_AUXILLIARY_LOAD_SWITCH_CALENDAR < calendarType) {
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else {
    uint8_t i;
    status = EMBER_ZCL_STATUS_NOT_FOUND;
    for (i = 0; i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS; i++) {
      if (providerId       == calendars[index][i].providerId
          && issuerCalendarId == calendars[index][i].issuerCalendarId
          && calendarType  == calendars[index][i].calendarType) {
        calendars[index][i].inUse = false;
        status = EMBER_ZCL_STATUS_SUCCESS;
      }
    }
  }

  emberAfSendImmediateDefaultResponse(status);
  return true;
}

#endif // UC_BUILD

uint8_t emberAfPluginCalendarClientGetCalendarIndexByType(uint8_t endpoint, uint8_t calendarType)
{
  uint8_t i;
  uint8_t epIndex = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_CALENDAR_CLUSTER_ID);
  for ( i = 0; i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS; i++) {
    if ( calendars[epIndex][i].calendarType == calendarType ) {
      break;
    }
  }
  return i;
}

uint32_t emberAfPluginCalendarClientGetCalendarId(uint8_t endpoint, uint8_t index)
{
  uint8_t epIndex = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_CALENDAR_CLUSTER_ID);
  if ( (epIndex == 0xFF) || (index >= EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS) ) {
    return EMBER_AF_PLUGIN_CALENDAR_CLIENT_INVALID_CALENDAR_ID;
  } else {
    return calendars[epIndex][index].issuerCalendarId;
  }
}

static EmberAfStatus addCalendar(EmberAfCalendar *calendar)
{
  uint32_t nowUtc = emberAfGetCurrentTime();
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t index = emberAfFindClusterClientEndpointIndex(endpoint,
                                                        ZCL_CALENDAR_CLUSTER_ID);
  uint8_t i;

  if (calendar->startTimeUtc == 0) {
    calendar->startTimeUtc = nowUtc;
  }

  // Nested and overlapping calendars are not allowed.  In the case of
  // overlapping calendars of the same type (calendar type), the calendar with
  // the newer IssuerCalendarID takes priority over all nested and overlapping
  // calendars.  All existing calendar instances that overlap, even partially,
  // should be removed.  The only exception to this is if a calendar instance
  // with a newer Issuer Event ID overlaps with the end of the current active
  // calendar but is not yet active, then the active calendar is not deleted
  // but modified so that the active calendar ends when the new calendar
  // begins.

  // First, check that this calendar has a newer issuer calendar id than all
  // active calendars of the same type.  If it doesn't, then it overlaps, and
  // must be rejected.
  for (i = 0; i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS; i++) {
    if (calendars[index][i].inUse
        && calendar->issuerEventId < calendars[index][i].issuerEventId
        && calendar->calendarType == calendars[index][i].calendarType) {
      emberAfDebugPrintln("ERR: Overlaps with newer calendar: 0x%4x/0x%4x",
                          calendars[index][i].providerId,
                          calendars[index][i].issuerCalendarId);
      return EMBER_ZCL_STATUS_FAILURE;
    }
  }

  // We now know that this calendar is newer and that we're going to store it,
  // assuming we have room.  Before saving it, we need to check if it starts
  // before any existing calendars of the same type.  If so, it means those
  // calendars overlap with it and must be removed.
  for (i = 0; i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS; i++) {
    if (calendars[index][i].inUse
        && calendar->startTimeUtc <= calendars[index][i].startTimeUtc
        && calendar->calendarType == calendars[index][i].calendarType) {
      emberAfDebugPrintln("INFO: Overlaps with older calendar: 0x%4x/0x%4x",
                          calendars[index][i].providerId,
                          calendars[index][i].issuerCalendarId);
      removeCalendar(&calendars[index][i]);
    }
  }

  // Now, look for any calendars that started in the past and have been
  // superceded by a newer calendar.  These are stale and can be removed.
  // TODO: Maybe this should only be done if we don't have space to store the
  // new calendar?
  for (i = 0; i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS; i++) {
    if (calendars[index][i].inUse
        && calendars[index][i].startTimeUtc < nowUtc) {
      uint8_t j;
      for (j = 0; j < EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS; j++) {
        if (i != j
            && calendars[index][j].inUse
            && (calendars[index][i].startTimeUtc
                < calendars[index][j].startTimeUtc)) {
          emberAfDebugPrintln("INFO: Removing expired calendar: 0x%4x/0x%4x",
                              calendars[index][i].providerId,
                              calendars[index][i].issuerCalendarId);
          removeCalendar(&calendars[index][i]);
        }
      }
    }
  }

  // Look for an empty slot to save this calendar.  If we don't have room, we
  // have to drop it.
  for (i = 0; i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS; i++) {
    if (!calendars[index][i].inUse) {
      MEMCOPY(&calendars[index][i], calendar, sizeof(EmberAfCalendar));
      return EMBER_ZCL_STATUS_SUCCESS;
    }
  }

  emberAfDebugPrintln("ERR: Insufficient space for calendar: 0x%4x/0x%4x",
                      calendar->providerId,
                      calendar->issuerCalendarId);
  return EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
}

// This function uses emberAfCurrentEndpoint and therefore must only be called
// in the context of an incoming message.
static EmberAfCalendar *findCalendar(uint32_t providerId,
                                     uint32_t issuerCalendarId)
{
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t index = emberAfFindClusterClientEndpointIndex(endpoint,
                                                        ZCL_CALENDAR_CLUSTER_ID);
  uint8_t i;
  for (i = 0; i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS; i++) {
    if (calendars[index][i].inUse
        && calendars[index][i].providerId == providerId
        && calendars[index][i].issuerCalendarId == issuerCalendarId) {
      return &calendars[index][i];
    }
  }
  return NULL;
}

static void removeCalendar(EmberAfCalendar *calendar)
{
  emberAfDebugPrintln("INFO: Removed calendar: 0x%4x/0x%4x",
                      calendar->providerId,
                      calendar->issuerCalendarId);
  calendar->inUse = false;
}

#ifdef UC_BUILD
// plugin calendar-client clear <endpoint:1>
void emberAfPluginCalendarClientClearCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t i;
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t index = emberAfFindClusterClientEndpointIndex(endpoint,
                                                        ZCL_CALENDAR_CLUSTER_ID);
  if (index == 0xFF) {
    return;
  }

  for (i = 0; i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS; i++) {
    if (calendars[index][i].inUse) {
      removeCalendar(&calendars[index][i]);
    }
  }
}

// plugin calendar-client print <endpoint:1>
void emberAfPluginCalendarClientPrintCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t i, j, k;
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t index = emberAfFindClusterClientEndpointIndex(endpoint,
                                                        ZCL_CALENDAR_CLUSTER_ID);
  if (index == 0xFF) {
    return;
  }

  for (i = 0; i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS; i++) {
    const EmberAfCalendar *calendar = &calendars[index][i];
    if (!calendar->inUse) {
      continue;
    }

    emberAfCalendarClusterPrintln("calendar:                       %d", i);
    emberAfCalendarClusterPrintln("  providerId:                   0x%4x", calendar->providerId);
    emberAfCalendarClusterPrintln("  issuerEventId:                0x%4x", calendar->issuerEventId);
    emberAfCalendarClusterPrintln("  issuerCalendarId:             0x%4x", calendar->issuerCalendarId);
    emberAfCalendarClusterPrintln("  startTimeUtc:                 0x%4x", calendar->startTimeUtc);
    emberAfCalendarClusterPrintln("  calendarType:                 0x%x", calendar->calendarType);
    emberAfCalendarClusterPrint("  calendarName:                 \"");
    emberAfCalendarClusterPrintString(calendar->calendarName);
    emberAfCalendarClusterPrintln("\"");
    emberAfCalendarClusterPrintln("  numberOfSeasons:              %d", calendar->numberOfSeasons);
    emberAfCalendarClusterPrintln("  receivedSeasons:              %d", calendar->receivedSeasons);
    emberAfCalendarClusterPrintln("  numberOfWeekProfiles:         %d", calendar->numberOfWeekProfiles);
    emberAfCalendarClusterPrintln("  numberOfDayProfiles:          %d", calendar->numberOfDayProfiles);

    for (j = 0; j < calendar->receivedSeasons; j++) {
      const EmberAfCalendarSeason *season = &calendar->seasons[j];
      emberAfCalendarClusterPrintln("  season:                       %d", i);
      emberAfCalendarClusterPrintln("    seasonStartDate:            0x%4x", season->seasonStartDate);
      emberAfCalendarClusterPrintln("    weekIdRef:                  %d", season->weekIdRef);
    }

    for (j = 0; j < calendar->numberOfWeekProfiles; j++) {
      const EmberAfCalendarWeekProfile *weekProfile = &calendar->weekProfiles[j];
      if (weekProfile->inUse) {
        emberAfCalendarClusterPrintln("  weekProfile:                  %d", j);
        emberAfCalendarClusterPrintln("    weekId:                     %d", j + 1);
        emberAfCalendarClusterPrintln("    dayIdRefMonday:             %d", weekProfile->dayIdRefMonday);
        emberAfCalendarClusterPrintln("    dayIdRefTuesday:            %d", weekProfile->dayIdRefTuesday);
        emberAfCalendarClusterPrintln("    dayIdRefWednesday:          %d", weekProfile->dayIdRefWednesday);
        emberAfCalendarClusterPrintln("    dayIdRefThursday:           %d", weekProfile->dayIdRefThursday);
        emberAfCalendarClusterPrintln("    dayIdRefFriday:             %d", weekProfile->dayIdRefFriday);
        emberAfCalendarClusterPrintln("    dayIdRefSaturday:           %d", weekProfile->dayIdRefSaturday);
        emberAfCalendarClusterPrintln("    dayIdRefSunday:             %d", weekProfile->dayIdRefSunday);
      }
    }

    for (j = 0; j < calendar->numberOfDayProfiles; j++) {
      const EmberAfCalendarDayProfile *dayProfile = &calendar->dayProfiles[j];
      if (dayProfile->inUse) {
        emberAfCalendarClusterPrintln("  dayProfile:                   %d", j);
        emberAfCalendarClusterPrintln("    dayId:                      %d", j + 1);
        emberAfCalendarClusterPrintln("    numberOfScheduleEntries:    %d", dayProfile->numberOfScheduleEntries);
        emberAfCalendarClusterPrintln("    receivedScheduleEntries:    %d", dayProfile->receivedScheduleEntries);
        for (k = 0; k < dayProfile->receivedScheduleEntries; k++) {
          const EmberAfCalendarScheduleEntry *scheduleEntry = &dayProfile->scheduleEntries[k];
          emberAfCalendarClusterPrintln("    scheduleEntry:              %d", k);
          switch (calendar->calendarType) {
            case EMBER_ZCL_CALENDAR_TYPE_DELIVERED_CALENDAR:
            case EMBER_ZCL_CALENDAR_TYPE_RECEIVED_CALENDAR:
            case EMBER_ZCL_CALENDAR_TYPE_DELIVERED_AND_RECEIVED_CALENDAR:
              emberAfCalendarClusterPrintln("      startTimeM:               0x%2x", scheduleEntry->rateSwitchTime.startTimeM);
              emberAfCalendarClusterPrintln("      priceTier:                0x%x", scheduleEntry->rateSwitchTime.priceTier);
              break;
            case EMBER_ZCL_CALENDAR_TYPE_FRIENDLY_CREDIT_CALENDAR:
              emberAfCalendarClusterPrintln("      startTimeM:               0x%2x", scheduleEntry->friendlyCreditSwitchTime.startTimeM);
              emberAfCalendarClusterPrintln("      friendlyCreditEnable:     %p", scheduleEntry->friendlyCreditSwitchTime.friendlyCreditEnable ? "true" : "false");
              break;
            case EMBER_ZCL_CALENDAR_TYPE_AUXILLIARY_LOAD_SWITCH_CALENDAR:
              emberAfCalendarClusterPrintln("      startTimeM:               0x%2x", scheduleEntry->auxilliaryLoadSwitchTime.startTimeM);
              emberAfCalendarClusterPrintln("      auxiliaryLoadSwitchState: 0x%x", scheduleEntry->auxilliaryLoadSwitchTime.auxiliaryLoadSwitchState);
              break;
          }
        }
      }
    }

    {
      const EmberAfCalendarSpecialDayProfile *specialDayProfile = &calendar->specialDayProfile;
      if (specialDayProfile->inUse) {
        emberAfCalendarClusterPrintln("  specialDayProfile:");
        emberAfCalendarClusterPrintln("    startTimeUtc:               0x%4x", specialDayProfile->startTimeUtc);
        emberAfCalendarClusterPrintln("    numberOfSpecialDayEntries:  %d", specialDayProfile->numberOfSpecialDayEntries);
        emberAfCalendarClusterPrintln("    receivedSpecialDayEntries:  %d", specialDayProfile->receivedSpecialDayEntries);
        for (j = 0; j < specialDayProfile->receivedSpecialDayEntries; j++) {
          const EmberAfCalendarSpecialDayEntry *specialDayEntry = &specialDayProfile->specialDayEntries[j];
          emberAfCalendarClusterPrintln("    specialDayEntry:            %d", j);
          emberAfCalendarClusterPrintln("      specialDayDate:           0x%4x", specialDayEntry->specialDayDate);
          emberAfCalendarClusterPrintln("      dayIdRef:                 %d", specialDayEntry->dayIdRef);
        }
      }
    }
  }
}

#else
// plugin calendar-client clear <endpoint:1>
void emberAfPluginCalendarClientClearCommand(void)
{
  uint8_t i;
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  uint8_t index = emberAfFindClusterClientEndpointIndex(endpoint,
                                                        ZCL_CALENDAR_CLUSTER_ID);
  if (index == 0xFF) {
    return;
  }

  for (i = 0; i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS; i++) {
    if (calendars[index][i].inUse) {
      removeCalendar(&calendars[index][i]);
    }
  }
}

// plugin calendar-client print <endpoint:1>
void emberAfPluginCalendarClientPrintCommand(void)
{
  uint8_t i, j, k;
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  uint8_t index = emberAfFindClusterClientEndpointIndex(endpoint,
                                                        ZCL_CALENDAR_CLUSTER_ID);
  if (index == 0xFF) {
    return;
  }

  for (i = 0; i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS; i++) {
    const EmberAfCalendar *calendar = &calendars[index][i];
    if (!calendar->inUse) {
      continue;
    }

    emberAfCalendarClusterPrintln("calendar:                       %d", i);
    emberAfCalendarClusterPrintln("  providerId:                   0x%4x", calendar->providerId);
    emberAfCalendarClusterPrintln("  issuerEventId:                0x%4x", calendar->issuerEventId);
    emberAfCalendarClusterPrintln("  issuerCalendarId:             0x%4x", calendar->issuerCalendarId);
    emberAfCalendarClusterPrintln("  startTimeUtc:                 0x%4x", calendar->startTimeUtc);
    emberAfCalendarClusterPrintln("  calendarType:                 0x%x", calendar->calendarType);
    emberAfCalendarClusterPrint("  calendarName:                 \"");
    emberAfCalendarClusterPrintString(calendar->calendarName);
    emberAfCalendarClusterPrintln("\"");
    emberAfCalendarClusterPrintln("  numberOfSeasons:              %d", calendar->numberOfSeasons);
    emberAfCalendarClusterPrintln("  receivedSeasons:              %d", calendar->receivedSeasons);
    emberAfCalendarClusterPrintln("  numberOfWeekProfiles:         %d", calendar->numberOfWeekProfiles);
    emberAfCalendarClusterPrintln("  numberOfDayProfiles:          %d", calendar->numberOfDayProfiles);

    for (j = 0; j < calendar->receivedSeasons; j++) {
      const EmberAfCalendarSeason *season = &calendar->seasons[j];
      emberAfCalendarClusterPrintln("  season:                       %d", i);
      emberAfCalendarClusterPrintln("    seasonStartDate:            0x%4x", season->seasonStartDate);
      emberAfCalendarClusterPrintln("    weekIdRef:                  %d", season->weekIdRef);
    }

    for (j = 0; j < calendar->numberOfWeekProfiles; j++) {
      const EmberAfCalendarWeekProfile *weekProfile = &calendar->weekProfiles[j];
      if (weekProfile->inUse) {
        emberAfCalendarClusterPrintln("  weekProfile:                  %d", j);
        emberAfCalendarClusterPrintln("    weekId:                     %d", j + 1);
        emberAfCalendarClusterPrintln("    dayIdRefMonday:             %d", weekProfile->dayIdRefMonday);
        emberAfCalendarClusterPrintln("    dayIdRefTuesday:            %d", weekProfile->dayIdRefTuesday);
        emberAfCalendarClusterPrintln("    dayIdRefWednesday:          %d", weekProfile->dayIdRefWednesday);
        emberAfCalendarClusterPrintln("    dayIdRefThursday:           %d", weekProfile->dayIdRefThursday);
        emberAfCalendarClusterPrintln("    dayIdRefFriday:             %d", weekProfile->dayIdRefFriday);
        emberAfCalendarClusterPrintln("    dayIdRefSaturday:           %d", weekProfile->dayIdRefSaturday);
        emberAfCalendarClusterPrintln("    dayIdRefSunday:             %d", weekProfile->dayIdRefSunday);
      }
    }

    for (j = 0; j < calendar->numberOfDayProfiles; j++) {
      const EmberAfCalendarDayProfile *dayProfile = &calendar->dayProfiles[j];
      if (dayProfile->inUse) {
        emberAfCalendarClusterPrintln("  dayProfile:                   %d", j);
        emberAfCalendarClusterPrintln("    dayId:                      %d", j + 1);
        emberAfCalendarClusterPrintln("    numberOfScheduleEntries:    %d", dayProfile->numberOfScheduleEntries);
        emberAfCalendarClusterPrintln("    receivedScheduleEntries:    %d", dayProfile->receivedScheduleEntries);
        for (k = 0; k < dayProfile->receivedScheduleEntries; k++) {
          const EmberAfCalendarScheduleEntry *scheduleEntry = &dayProfile->scheduleEntries[k];
          emberAfCalendarClusterPrintln("    scheduleEntry:              %d", k);
          switch (calendar->calendarType) {
            case EMBER_ZCL_CALENDAR_TYPE_DELIVERED_CALENDAR:
            case EMBER_ZCL_CALENDAR_TYPE_RECEIVED_CALENDAR:
            case EMBER_ZCL_CALENDAR_TYPE_DELIVERED_AND_RECEIVED_CALENDAR:
              emberAfCalendarClusterPrintln("      startTimeM:               0x%2x", scheduleEntry->rateSwitchTime.startTimeM);
              emberAfCalendarClusterPrintln("      priceTier:                0x%x", scheduleEntry->rateSwitchTime.priceTier);
              break;
            case EMBER_ZCL_CALENDAR_TYPE_FRIENDLY_CREDIT_CALENDAR:
              emberAfCalendarClusterPrintln("      startTimeM:               0x%2x", scheduleEntry->friendlyCreditSwitchTime.startTimeM);
              emberAfCalendarClusterPrintln("      friendlyCreditEnable:     %p", scheduleEntry->friendlyCreditSwitchTime.friendlyCreditEnable ? "true" : "false");
              break;
            case EMBER_ZCL_CALENDAR_TYPE_AUXILLIARY_LOAD_SWITCH_CALENDAR:
              emberAfCalendarClusterPrintln("      startTimeM:               0x%2x", scheduleEntry->auxilliaryLoadSwitchTime.startTimeM);
              emberAfCalendarClusterPrintln("      auxiliaryLoadSwitchState: 0x%x", scheduleEntry->auxilliaryLoadSwitchTime.auxiliaryLoadSwitchState);
              break;
          }
        }
      }
    }

    {
      const EmberAfCalendarSpecialDayProfile *specialDayProfile = &calendar->specialDayProfile;
      if (specialDayProfile->inUse) {
        emberAfCalendarClusterPrintln("  specialDayProfile:");
        emberAfCalendarClusterPrintln("    startTimeUtc:               0x%4x", specialDayProfile->startTimeUtc);
        emberAfCalendarClusterPrintln("    numberOfSpecialDayEntries:  %d", specialDayProfile->numberOfSpecialDayEntries);
        emberAfCalendarClusterPrintln("    receivedSpecialDayEntries:  %d", specialDayProfile->receivedSpecialDayEntries);
        for (j = 0; j < specialDayProfile->receivedSpecialDayEntries; j++) {
          const EmberAfCalendarSpecialDayEntry *specialDayEntry = &specialDayProfile->specialDayEntries[j];
          emberAfCalendarClusterPrintln("    specialDayEntry:            %d", j);
          emberAfCalendarClusterPrintln("      specialDayDate:           0x%4x", specialDayEntry->specialDayDate);
          emberAfCalendarClusterPrintln("      dayIdRef:                 %d", specialDayEntry->dayIdRef);
        }
      }
    }
  }
}
#endif

#ifdef UC_BUILD

uint32_t emberAfCalendarClusterClientCommandParse(sl_service_opcode_t opcode,
                                                  sl_service_function_context_t *context)
{
  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_PUBLISH_CALENDAR_COMMAND_ID:
      {
        wasHandled = emberAfCalendarClusterPublishCalendarCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_DAY_PROFILE_COMMAND_ID:
      {
        wasHandled = emberAfCalendarClusterPublishDayProfileCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_WEEK_PROFILE_COMMAND_ID:
      {
        wasHandled = emberAfCalendarClusterPublishWeekProfileCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_SEASONS_COMMAND_ID:
      {
        wasHandled = emberAfCalendarClusterPublishSeasonsCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_SPECIAL_DAYS_COMMAND_ID:
      {
        wasHandled = emberAfCalendarClusterPublishSpecialDaysCallback(cmd);
        break;
      }
      case ZCL_CANCEL_CALENDAR_COMMAND_ID:
      {
        wasHandled = emberAfCalendarClusterCancelCalendarCallback(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
