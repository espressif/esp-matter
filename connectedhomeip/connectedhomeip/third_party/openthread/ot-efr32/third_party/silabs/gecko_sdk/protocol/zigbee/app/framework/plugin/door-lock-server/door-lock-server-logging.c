/***************************************************************************//**
 * @file
 * @brief Logging routines for the Door Lock Server plugin.
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

#include "af.h"
#include "door-lock-server.h"

#ifdef UC_BUILD
#include "zap-cluster-command-parser.h"
#endif

static EmberAfPluginDoorLockServerLogEntry entries[EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_MAX_LOG_ENTRIES];
static uint8_t nextEntryId = 1;

#define ENTRY_ID_TO_INDEX(entryId) ((entryId) - 1)
#define ENTRY_ID_IS_VALID(entryId) ((entryId) > 0 && (entryId) < nextEntryId)
#define MOST_RECENT_ENTRY_ID() (nextEntryId - 1)
#define LOG_IS_EMPTY() (nextEntryId == 1)

static bool loggingIsEnabled(void)
{
  // This is hardcoded to endpoint 1 because...we need to add endpoint support...
  uint8_t endpoint = 1;
  bool logging = false;
  EmberAfStatus status
    = emberAfReadServerAttribute(endpoint,
                                 ZCL_DOOR_LOCK_CLUSTER_ID,
                                 ZCL_ENABLE_LOGGING_ATTRIBUTE_ID,
                                 (uint8_t *)&logging,
                                 sizeof(logging));
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfDoorLockClusterPrintln("Could not read EnableLogging attribute: 0x%X",
                                  status);
  }
  return logging;
}

bool emberAfPluginDoorLockServerAddLogEntry(EmberAfDoorLockEventType eventType,
                                            EmberAfDoorLockEventSource source,
                                            uint8_t eventId,
                                            uint16_t userId,
                                            uint8_t pinLength,
                                            uint8_t *pin)
{
  if (!loggingIsEnabled()
      || ENTRY_ID_TO_INDEX(nextEntryId) >= (uint8_t) COUNTOF(entries)) {
    return false;
  }

  EmberAfPluginDoorLockServerLogEntry *nextEntry
    = &entries[ENTRY_ID_TO_INDEX(nextEntryId)];
  nextEntry->logEntryId = nextEntryId;
  nextEntry->timestamp = emberAfGetCurrentTimeCallback();
  nextEntry->eventType = eventType;
  nextEntry->source = source;
  nextEntry->eventId = eventId;
  nextEntry->userId = userId;
  // Truncate logged PIN if larger than log entry capacity.
  uint8_t moveLength = (pinLength > EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_MAX_PIN_LENGTH
                        ? EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_MAX_PIN_LENGTH
                        : pinLength);
  nextEntry->pin[0] = moveLength;
  MEMMOVE(nextEntry->pin + 1, pin, moveLength);

  nextEntryId++;

  return true;
}

bool emberAfPluginDoorLockServerGetLogEntry(uint16_t *entryId,
                                            EmberAfPluginDoorLockServerLogEntry *entry)
{
  if (LOG_IS_EMPTY()) {
    return false;
  }

  if (!ENTRY_ID_IS_VALID(*entryId)) {
    *entryId = MOST_RECENT_ENTRY_ID();
  }
  assert(ENTRY_ID_IS_VALID(*entryId));

  *entry = entries[ENTRY_ID_TO_INDEX(*entryId)];

  return true;
}

#ifdef UC_BUILD

bool emberAfDoorLockClusterGetLogRecordCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_door_lock_cluster_get_log_record_command_t cmd_data;
  EmberStatus status;
  EmberAfPluginDoorLockServerLogEntry entry;

  if (zcl_decode_door_lock_cluster_get_log_record_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if (LOG_IS_EMPTY() || !emberAfPluginDoorLockServerGetLogEntry(&cmd_data.logIndex, &entry)) {
    status = emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_INVALID_VALUE);
    if (status != EMBER_ZCL_STATUS_SUCCESS) {
      emberAfDoorLockClusterPrintln("Failed to send default response: 0x%X",
                                    status);
    }
  } else {
    emberAfFillCommandDoorLockClusterGetLogRecordResponse(entry.logEntryId,
                                                          entry.timestamp,
                                                          entry.eventType,
                                                          entry.source,
                                                          entry.eventId,
                                                          entry.userId,
                                                          entry.pin);
    status = emberAfSendResponse();
    if (status != EMBER_SUCCESS) {
      emberAfDoorLockClusterPrintln("Failed to send GetLogRecordResponse: 0x%X",
                                    status);
    }
  }
  return true;
}

#else // !UC_BUILD

bool emberAfDoorLockClusterGetLogRecordCallback(uint16_t entryId)
{
  EmberStatus status;
  EmberAfPluginDoorLockServerLogEntry entry;
  if (LOG_IS_EMPTY() || !emberAfPluginDoorLockServerGetLogEntry(&entryId, &entry)) {
    status = emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_INVALID_VALUE);
    if (status != EMBER_ZCL_STATUS_SUCCESS) {
      emberAfDoorLockClusterPrintln("Failed to send default response: 0x%X",
                                    status);
    }
  } else {
    emberAfFillCommandDoorLockClusterGetLogRecordResponse(entry.logEntryId,
                                                          entry.timestamp,
                                                          entry.eventType,
                                                          entry.source,
                                                          entry.eventId,
                                                          entry.userId,
                                                          entry.pin);
    status = emberAfSendResponse();
    if (status != EMBER_SUCCESS) {
      emberAfDoorLockClusterPrintln("Failed to send GetLogRecordResponse: 0x%X",
                                    status);
    }
  }
  return true;
}

#endif // UC_BUILD
