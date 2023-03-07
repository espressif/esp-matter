/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Door Lock Server plugin.
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

/**
 * @defgroup door-lock-server Door Lock Server
 * @ingroup component cluster
 * @brief API and Callbacks for the Door Lock Cluster Server Component
 *
 * Silicon Labs implementation of the Door Lock server cluster.
 * This is an incomplete component implementation. It does all
 * of the mandatory and optional behavior in the Door Lock cluster
 * needed to pass the Door Lock cluster tests. It does not
 * store user and schedule tables in persistent memory, check user
 * pin activation against user status or user schedule, or offer
 * callbacks and component options to the application for further
 * customization of functionality.
 *
 */

/**
 * @addtogroup door-lock-server
 * @{
 */

// ------------------------------------------------------------------------------
// Core

#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct {
  EmberAfAttributeId id;
  uint16_t value;
} EmAfPluginDoorLockServerAttributeData;

/** @brief Activate Door Lock
 *
 *
 * @param activate True if the lock should move to the locked position, false
 * if it should move to the unlocked position  Ver.: always
 */
bool emberAfPluginDoorLockServerActivateDoorLockCallback(bool activate);

// For each of the provided attribute ID/value pairs, write the attribute to the
// value on the DOOR_LOCK_SERVER_ENDPOINT. If this function encounters a failure,
// it will print something out. The description parameter is a string that
// describes the type of attributes that are being written.
void emAfPluginDoorLockServerWriteAttributes(const EmAfPluginDoorLockServerAttributeData *data,
                                             uint8_t dataLength,
                                             const char *description);

// This function should be called when the door state has changed. A status
// describing the success or failure of the update will be returned.
EmberAfStatus emAfPluginDoorLockServerNoteDoorStateChanged(EmberAfDoorState state);
#endif

// At boot, the NumberOfPINUsersSupported attribute will be written to this
// value.
// Note: the DOOR_LOCK_USER_TABLE_SIZE symbol is respected because it
// was used originally as a configuration value for this plugin.
#ifdef DOOR_LOCK_USER_TABLE_SIZE
  #define EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_PIN_USER_TABLE_SIZE DOOR_LOCK_USER_TABLE_SIZE
#else
  #define EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_PIN_USER_TABLE_SIZE 8
#endif

// At boot, the NumberOfRFIDUsersSupported attribute will be written to this
// value.
#define EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_RFID_USER_TABLE_SIZE 8

// This value should reflect the value of the MaxPINCodeLength attribute.
// Note: the DOOR_LOCK_MAX_PIN_LENGTH symbol is respected because it was used
// originally as a configuration value for this plugin.
#ifdef DOOR_LOCK_MAX_PIN_LENGTH
  #define EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_MAX_PIN_LENGTH DOOR_LOCK_MAX_PIN_LENGTH
#else
  #define EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_MAX_PIN_LENGTH 8
#endif

// This value should reflect the value of the MaxRFIDCodeLength attribute.
#define EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_MAX_RFID_LENGTH 8

// At boot, the NumberOfWeekDaySchedulesSupportedPerUser attribute will be writen
// to this value.
// Note: the DOOR_LOCK_SCHEDULE_TABLE_SIZE symbol is respected because it was
// used originally as a configuration value for this plugin.
// Also note: technically, this is the _total_ number of weekday schedules that
// can be stored across all users.
#ifdef DOOR_LOCK_SCHEDULE_TABLE_SIZE
  #define EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_WEEKDAY_SCHEDULE_TABLE_SIZE DOOR_LOCK_SCHEDULE_TABLE_SIZE
#else
  #define EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_WEEKDAY_SCHEDULE_TABLE_SIZE 4
#endif

// At boot, the NumberOfYearDaySchedulesSupportedPerUser attribute will be
// written to this value.
// Note: technically, this is the _total_ number of yearday schedules that can be
// stored across all users.
#define EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_YEARDAY_SCHEDULE_TABLE_SIZE 8

// At boot, the NumberOfHolidaySchedulesSupported attribute will be written to
// this value.
#define EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_HOLIDAY_SCHEDULE_TABLE_SIZE 8

#ifndef DOOR_LOCK_SERVER_ENDPOINT
  #define DOOR_LOCK_SERVER_ENDPOINT 1
#endif

// ------------------------------------------------------------------------------
// Logging

// This value should reflect the value of the NumberOfLogRecordsSupported
// attribute.
#define EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_MAX_LOG_ENTRIES 16

typedef struct {
  uint16_t logEntryId;
  uint32_t timestamp;
  EmberAfDoorLockEventType eventType;
  EmberAfDoorLockEventSource source;

  // This field is either a EmberAfDoorLockOperationEventCode or a
  // EmberAfDoorLockProgrammingEventCode.
  uint8_t eventId;

  uint16_t userId;

  // This field is a ZCL string representing the PIN code (i.e., the first byte
  // of the buffer is the length of the total buffer).
  uint8_t pin[EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_MAX_PIN_LENGTH + 1];
} EmberAfPluginDoorLockServerLogEntry;

/**
 * @name API
 * @{
 */

/** @brief Add a log entry. Returns true if the entry was added. Note that the eventId
 * parameter should be of type EmberAfDoorLockOperationEventCode or
 * EmberAfDoorLockProgrammingEventCode.
 *
 * @param eventType Event type Ver.: always
 * @param source Event source Ver.: always
 * @param eventID Ver.: always
 * @param userId Ver.: always
 * @param pinLength Ver.: always
 * @param pin Ver.: always
 *
 * @return bool true if success
 */
bool emberAfPluginDoorLockServerAddLogEntry(EmberAfDoorLockEventType eventType,
                                            EmberAfDoorLockEventSource source,
                                            uint8_t eventId,
                                            uint16_t userId,
                                            uint8_t pinLength,
                                            uint8_t *pin);

/** @brief Get a log entry associated with the entry ID. If the entry ID does not exist,
 * the most recent entry is returned and the entryId parameter is updated. The
 * entryId is a 1-based index into an array of log entries in order to match
 * GetLogRecord ZCL command. This will return true if the entry was successfully
 * returned.
 *
 * @param entryId Ver.: always
 * @param entry pointer to log entry struct Ver.: always
 *
 * @return bool true if success
 */

bool emberAfPluginDoorLockServerGetLogEntry(uint16_t *entryId,
                                            EmberAfPluginDoorLockServerLogEntry *entry);

// ------------------------------------------------------------------------------
// Users

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Initialize the Door Lock users.
void emAfPluginDoorLockServerInitUser(void);

// Initialize events.
void emAfPluginDoorLockServerInitEvents(void);

// Set the user type associated with the provided user ID (userId) and return
// true if successful.
bool emAfPluginDoorLockServerSetPinUserType(uint16_t userId,
                                            EmberAfDoorLockUserType type);
#endif

typedef struct {
  EmberAfDoorLockUserStatus status;
  EmberAfDoorLockUserType type;

  // This field is a Zigbee string, so the first byte is the length of the
  // remaining bytes.
  union {
    uint8_t pin[EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_MAX_PIN_LENGTH + 1];
    uint8_t rfid[EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_MAX_RFID_LENGTH + 1];
  } code;
} EmberAfPluginDoorLockServerUser;

/** @brief Unlocks the door with a pin
 *
 * @param pin Ver.: always
 * @param pinLength Ver.: always
 *
 * @return EmberAfStatus status code
 *
 */
EmberAfStatus emberAfPluginDoorLockServerApplyPin(uint8_t *pin,
                                                  uint8_t pinLength);

/** @brief Unlocks the door with RFID
 *
 * @param rfid Ver.: always
 * @param rfidLength Ver.: always
 *
 * @return EmberAfStatus status code
 *
 */
EmberAfStatus emberAfPluginDoorLockServerApplyRfid(uint8_t *rfid,
                                                   uint8_t rfidLength);

/** @} */ // end of name API

// ------------------------------------------------------------------------------
// Schedule

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Initialize the Door Lock schedules.
void emAfPluginDoorLockServerInitSchedule(void);
#endif

typedef struct {
  uint16_t userId;
  uint8_t daysMask;
  uint8_t startHour;
  uint8_t startMinute;
  uint8_t stopHour;
  uint8_t stopMinute;
  bool inUse;
} EmberAfPluginDoorLockServerWeekdayScheduleEntry;

typedef struct {
  uint16_t userId;
  uint32_t localStartTime;
  uint32_t localEndTime;
  bool inUse;
} EmberAfPluginDoorLockServerYeardayScheduleEntry;

typedef struct {
  uint32_t localStartTime;
  uint32_t localEndTime;
  EmberAfDoorLockOperatingMode operatingModeDuringHoliday;
  bool inUse;
} EmberAfPluginDoorLockServerHolidayScheduleEntry;

typedef struct {
  uint32_t localStartTime;
  uint32_t localEndTime;
  bool     inUse;
} EmberAfPluginDoorLockServerDisposableScheduleEntry;

// ------------------------------------------------------------------------------
// Utilities

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// This function will see if the space required (spaceReq) is greater than the
// space available (spaceAvail) and if so it will send a DefaultResponse
// command with the status of EMBER_ZCL_STATUS_INSUFFICIENT_SPACE and return
// false. Otherwise, it will return true.
bool emAfPluginDoorLockServerCheckForSufficientSpace(uint8_t spaceReq, uint8_t spaceAvail);
#endif

// Critical Message Queue
// If the Critical Message Queue Plugin is available, use it for event notifications
#ifdef UC_BUILD
#include "sl_component_catalog.h"
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_CRITICAL_MESSAGE_QUEUE
#define SL_CATALOG_ZIGBEE_CRITICAL_MESSAGE_QUEUE_PRESENT
#endif
#endif // UC_BUILD

#ifdef SL_CATALOG_ZIGBEE_CRITICAL_MESSAGE_QUEUE_PRESENT
#include "../critical-message-queue/critical-message-queue.h"
#define SEND_COMMAND_UNICAST_TO_BINDINGS() emberAfSendCommandUnicastToBindingsWithCallback(emberAfPluginCriticalMessageQueueEnqueueCallback)
#else
#define SEND_COMMAND_UNICAST_TO_BINDINGS() emberAfSendCommandUnicastToBindings()
#endif
// ------------------------------------------------------------------------------
// Legacy

// The following preprocessor logic serves as a conversion layer from the old
// names for the symbols used by this plugin.

#define EmberAfDoorLockScheduleEntry EmberAfPluginDoorLockServerWeekdayScheduleEntry
#define EmberAfDoorLockUser EmberAfPluginDoorLockServerUser

/** @} */ // end of door-lock-server
