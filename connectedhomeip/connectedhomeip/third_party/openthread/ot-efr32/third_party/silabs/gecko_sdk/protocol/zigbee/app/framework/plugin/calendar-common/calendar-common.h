/***************************************************************************//**
 * @file
 * @brief APIs for the Calendar Common plugin.
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

#ifdef UC_BUILD
#include "sl_component_catalog.h"
#include "calendar-common-config.h"
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_GBCS_COMPATIBILITY
#define SL_CATALOG_ZIGBEE_GBCS_COMPATIBILITY_PRESENT
#endif
#ifdef EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION
#define SL_CATALOG_ZIGBEE_GAS_PROXY_FUNCTION_PRESENT
#endif
#endif // UC_BUILD

#ifdef UC_BUILD
#include "calendar-common-config.h"
#endif // UC_BUILD

/**
 * @defgroup calendar-common Calendar Common
 * @ingroup component
 * @brief API and Callbacks for the Calendar Common Component
 *
 * This component provides common utility functions that
 * are used by the client and server of the Calendar Cluster.
 *
 */

/**
 * @addtogroup calendar-common
 * @{
 */

#define fieldLength(field) \
  (emberAfCurrentCommand()->bufLen - (field - emberAfCurrentCommand()->buffer));

#define EMBER_AF_PLUGIN_CALENDAR_COMMON_INVALID_SCHEDULE_ENTRY 0xFFFF
#define EMBER_AF_PLUGIN_CALENDAR_COMMON_INVALID_ID 0xFF
#define EMBER_AF_PLUGIN_CALENDAR_MAX_CALENDAR_NAME_LENGTH 12
#define EMBER_AF_PLUGIN_CALENDAR_COMMON_INVALID_INDEX 0xFF

/** @brief   The format of the actual data in the entry depends on the calendar type.
 * For calendar type 00 - 0x02, it is a rate switch time and
 * the data is a price tier enum (8-bit).
 * For calendar type 0x03, it is a friendly credit switch time and
 * the data is a bool (8-bit), which is friendly credit enabled.
 * For calendar type 0x04, it is an auxiliary load switch time and
 * the data is a bitmap (8-bit).
 */
typedef struct {
  uint16_t minutesFromMidnight;
  uint8_t data;
} EmberAfCalendarDayScheduleEntryStruct;

typedef struct {
  EmberAfCalendarDayScheduleEntryStruct scheduleEntries[EMBER_AF_PLUGIN_CALENDAR_COMMON_SCHEDULE_ENTRIES_MAX];
  uint8_t id;
  uint8_t numberOfScheduleEntries;
} EmberAfCalendarDayStruct;

typedef struct {
  EmberAfDate startDate;
  uint8_t normalDayIndex;
  uint8_t flags;
} EmberAfCalendarSpecialDayStruct;

#define EMBER_AF_PLUGIN_CALENDAR_COMMON_MONDAY_INDEX (0)
#define EMBER_AF_PLUGIN_CALENDAR_COMMON_SUNDAY_INDEX (6)
#define EMBER_AF_DAYS_IN_THE_WEEK (7)

typedef struct {
  uint8_t normalDayIndexes[EMBER_AF_DAYS_IN_THE_WEEK];
  uint8_t id;
} EmberAfCalendarWeekStruct;

typedef struct {
  EmberAfDate startDate;
  uint8_t weekIndex;
} EmberAfCalendarSeasonStruct;

#define EMBER_AF_PLUGIN_CALENDAR_COMMON_INVALID_CALENDAR_ID 0xFFFFFFFF
#define EMBER_AF_PLUGIN_CALENDAR_COMMON_WILDCARD_CALENDAR_ID 0x00000000
#define EMBER_AF_PLUGIN_CALENDAR_COMMON_WILDCARD_PROVIDER_ID 0xFFFFFFFF
#define EMBER_AF_PLUGIN_CALENDAR_COMMON_WILDCARD_ISSUER_ID 0xFFFFFFFF
#define EMBER_AF_PLUGIN_CALENDAR_COMMON_WILDCARD_CALENDAR_TYPE 0xFF

#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfPluginCalendarCommonFlags
#else
enum
#endif
{
  EMBER_AF_PLUGIN_CALENDAR_COMMON_FLAGS_SENT = 0x01,
};

typedef struct {
  EmberAfCalendarWeekStruct weeks[EMBER_AF_PLUGIN_CALENDAR_COMMON_WEEK_PROFILE_MAX];
  EmberAfCalendarDayStruct normalDays[EMBER_AF_PLUGIN_CALENDAR_COMMON_DAY_PROFILE_MAX];
  EmberAfCalendarSpecialDayStruct specialDays[EMBER_AF_PLUGIN_CALENDAR_COMMON_SPECIAL_DAY_PROFILE_MAX];
  EmberAfCalendarSeasonStruct seasons[EMBER_AF_PLUGIN_CALENDAR_COMMON_SEASON_PROFILE_MAX];
  uint32_t providerId;
  uint32_t issuerEventId;
  uint32_t calendarId;
  uint32_t startTimeUtc;
  uint8_t name[EMBER_AF_PLUGIN_CALENDAR_MAX_CALENDAR_NAME_LENGTH + 1];
  uint8_t calendarType;
  uint8_t numberOfSeasons;
  uint8_t numberOfWeekProfiles;
  uint8_t numberOfDayProfiles;
  uint8_t numberOfSpecialDayProfiles;

  /* These "received" counters do not belong here. They are here to help with
   * replaying TOM messages correctly, which will serve as the destination index
   * for the next publish command.
   */
  uint8_t numberOfReceivedSeasons;
  uint8_t numberOfReceivedWeekProfiles;
  uint8_t numberOfReceivedDayProfiles;
  uint8_t flags;
} EmberAfCalendarStruct;

extern EmberAfCalendarStruct calendars[];

#ifdef SL_CATALOG_ZIGBEE_GAS_PROXY_FUNCTION_PRESENT
#define GBCS_TARIFF_SWITCHING_CALENDAR_ID 0xFFFFFFFF
#define GBCS_NON_DISABLEMENT_CALENDAR_ID  0xFFFFFFFE
extern uint32_t tariffSwitchingCalendarId;
extern uint32_t nonDisablementCalendarId;
#endif // SL_CATALOG_ZIGBEE_GAS_PROXY_FUNCTION_PRESENT

/**
 * @name API
 * @{
 */

/** @brief Gets the calender by calender ID
 *
 * @param calenderId Ver.: always
 * @param providerId Ver.: always
 *
 * @return uint8_t Calender index
 *
 */
uint8_t emberAfPluginCalendarCommonGetCalendarById(uint32_t calendarId,
                                                   uint32_t providerId);

/** @brief Get the calender end time (UTC).
 *
 * @param calender pointer to the calender struct Ver.: always
 *
 * @return uint32_t end UTC time in seconds
 *
 */
uint32_t emberAfPluginCalendarCommonEndTimeUtc(const EmberAfCalendarStruct *calendar);

/** @brief Sets the calender info
 *
 * @param index Ver.: always
 * @param providerId Ver.: always
 * @param isserEventId Ver.: always
 * @param issuerCalenderId Ver.: always
 * @param startTimeUtc Ver.: always
 * @param calanederType Ver.: always
 * @param calenderName Ver.: always
 * @param numberOfSeasons Ver.: always
 * @param numberOfWeekProfiles Ver.: always
 * @param numberOfDayProfiles Ver.: always
 *
 * @return bool true if success
 */
bool emberAfCalendarCommonSetCalInfo(uint8_t index,
                                     uint32_t providerId,
                                     uint32_t issuerEventId,
                                     uint32_t issuerCalendarId,
                                     uint32_t startTimeUtc,
                                     uint8_t calendarType,
                                     uint8_t *calendarName,
                                     uint8_t numberOfSeasons,
                                     uint8_t numberOfWeekProfiles,
                                     uint8_t numberOfDayProfiles);

/** @brief Adds a new entry corresponding to the PublishCalendar command.
 * Tries to handle the new entry in the following method:
 * 1) Try to apply new data to a matching existing entry.
 * 2) Fields such as providerId, issuerEventId, and startTime, will be used.
 * 3) Overwrite the oldest entry (one with smallest event ID) with new information.
 *
 * @param providerId Ver.: always
 * @param issuerEventId Ver.: always
 * @param issuerCalenderId Ver.: always
 * @param startTimeUtc Ver.: always
 * @param calenderType Ver.: always
 * @param calenderName Ver.: always
 * @param numberOfSeasons Ver.: always
 * @param numberOfWeekProfiles Ver.: always
 * @param numberOfDayProfiles Ver.: always
 *
 * @return bool true if success
 *
 */
bool emberAfCalendarCommonAddCalInfo(uint32_t providerId,
                                     uint32_t issuerEventId,
                                     uint32_t issuerCalendarId,
                                     uint32_t startTimeUtc,
                                     uint8_t calendarType,
                                     uint8_t *calendarName,
                                     uint8_t numberOfSeasons,
                                     uint8_t numberOfWeekProfiles,
                                     uint8_t numberOfDayProfiles);

/** @brief Set calender server season info
 *
 * @param index Ver.: always
 * @param seasonId Ver.: always
 * @param startDate Ver.: always
 * @param weekIndex Ver.: always
 *
 * @return bool true if success
 *
 */
bool emberAfCalendarServerSetSeasonsInfo(uint8_t index,
                                         uint8_t seasonId,
                                         EmberAfDate startDate,
                                         uint8_t weekIndex);

/** @brief Add calender server season info
 *
 * @param issuerCalenderId Ver.: always
 * @param seasonEntries Ver.: always
 * @param seasonEnteriesLength Ver.: always
 * @param unknownWeekIdSeasonsMask Ver.: always
 *
 * @return bool true if success
 *
 */
bool emberAfCalendarServerAddSeasonsInfo(uint32_t issuerCalendarId,
                                         uint8_t * seasonsEntries,
                                         uint8_t seasonsEntriesLength,
                                         uint8_t * unknownWeekIdSeasonsMask);

/** @brief Set day profile info
 *
 * @param index Ver.: always
 * @param dayId Ver.: always
 * @param entryId Ver.: always
 * @param minutesFromMidnight Ver.: always
 * @param data Ver.: always
 *
 * @return bool true if success
 */
bool emberAfCalendarCommonSetDayProfInfo(uint8_t index,
                                         uint8_t dayId,
                                         uint8_t entryId,
                                         uint16_t minutesFromMidnight,
                                         uint8_t data);

/** @brief Add day profile info
 *
 * @param issuerCalenderId Ver.: always
 * @param dayId Ver.: always
 * @param dayScheduleEnteries Ver.: always
 * @param dayScheduleEnteriesLength Ver.: always
 *
 * @return bool true if success
 *
 */
bool emberAfCalendarCommonAddDayProfInfo(uint32_t issuerCalendarId,
                                         uint8_t dayId,
                                         uint8_t * dayScheduleEntries,
                                         uint16_t dayScheduleEntriesLength);

/** @brief Set calender server week profile info
 *
 * @param index Ver.: always
 * @param weekId Ver.: always
 * @param dayIdRefMon Ver.: always
 * @param dayIdRefTue Ver.: always
 * @param dayIdRefWed Ver.: always
 * @param dayIdRefThu Ver.: always
 * @param dayIdRefFri Ver.: always
 * @param dayIdRefSat Ver.: always
 * @param dayIdRefSun Ver.: always
 *
 * @return bool true if success
 */
bool emberAfCalendarServerSetWeekProfInfo(uint8_t index,
                                          uint8_t weekId,
                                          uint8_t dayIdRefMon,
                                          uint8_t dayIdRefTue,
                                          uint8_t dayIdRefWed,
                                          uint8_t dayIdRefThu,
                                          uint8_t dayIdRefFri,
                                          uint8_t dayIdRefSat,
                                          uint8_t dayIdRefSun);

/** @brief Add calender server week profile
 *
 * @param issuerCalenderId Ver.: always
 * @param weekId Ver.: always
 * @param dayIdRefMon Ver.: always
 * @param dayIdRefTue Ver.: always
 * @param dayIdRefWed Ver.: always
 * @param dayIdRefThu Ver.: always
 * @param dayIdRefFri Ver.: always
 * @param dayIdRefSat Ver.: always
 * @param dayIdRefSun Ver.: always
 *
 * @return bool true if success
 *
 */
bool emberAfCalendarServerAddWeekProfInfo(uint32_t issuerCalendarId,
                                          uint8_t weekId,
                                          uint8_t dayIdRefMon,
                                          uint8_t dayIdRefTue,
                                          uint8_t dayIdRefWed,
                                          uint8_t dayIdRefThu,
                                          uint8_t dayIdRefFri,
                                          uint8_t dayIdRefSat,
                                          uint8_t dayIdRefSun);

/** @brief Updating special days information of the specified calendar.
 * Assumes that the value of totalNumberOfSpecialDays will match
 * up with the information passed in between specialDaysEntries and
 * specialDaysEntriesLength.
 *
 * @param issuerCalenderId Ver.: always
 * @param totalNumberOfSpecialDays Ver.: always
 * @param specialDaysEnteries Ver.: always
 * @param specialDaysEnteriesLength Ver.: always
 * @param unknownSpecialDayMask Ver.: always
 *
 * @return bool true if success
 *
 */
bool emberAfCalendarCommonAddSpecialDaysInfo(uint32_t issuerCalendarId,
                                             uint8_t totalNumberOfSpecialDays,
                                             uint8_t * specialDaysEntries,
                                             uint16_t specialDaysEntriesLength,
                                             uint8_t * unknownSpecialDaysMask);

/** @} */ // end of name API
/** @} */ // end of calendar-common

#define SCHEDULE_ENTRY_SIZE (3)
// Season start date (4-bytes) and week ID ref (1-byte).
#define SEASON_ENTRY_SIZE (5)
// Special day date (4 bytes) and Day ID ref (1-byte).
#define SPECIAL_DAY_ENTRY_SIZE (5)
