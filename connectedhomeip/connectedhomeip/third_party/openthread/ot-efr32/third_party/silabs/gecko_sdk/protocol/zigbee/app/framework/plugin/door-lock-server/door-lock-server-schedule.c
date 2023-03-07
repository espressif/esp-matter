/***************************************************************************//**
 * @file
 * @brief Routines for the Door Lock Server plugin.
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

static EmberAfPluginDoorLockServerWeekdayScheduleEntry weekdayScheduleTable[EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_WEEKDAY_SCHEDULE_TABLE_SIZE];
static EmberAfPluginDoorLockServerYeardayScheduleEntry yeardayScheduleTable[EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_YEARDAY_SCHEDULE_TABLE_SIZE];
static EmberAfPluginDoorLockServerHolidayScheduleEntry holidayScheduleTable[EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_HOLIDAY_SCHEDULE_TABLE_SIZE];
static EmberAfPluginDoorLockServerDisposableScheduleEntry disposableScheduleTable[EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_PIN_USER_TABLE_SIZE];

static void printWeekdayScheduleTable(void)
{
  uint8_t i;
  emberAfDoorLockClusterPrintln("id uid dm strth strtm stph stpm");
  for (i = 0; i < EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_WEEKDAY_SCHEDULE_TABLE_SIZE; i++ ) {
    EmberAfDoorLockScheduleEntry *entry = &weekdayScheduleTable[i];
    if (entry->inUse) {
      emberAfDoorLockClusterPrintln("%x %2x  %x %4x   %4x   %4x  %4x",
                                    i,
                                    entry->userId,
                                    entry->daysMask,
                                    entry->startHour,
                                    entry->stopHour,
                                    entry->stopMinute);
    }
  }
}

static void clearWeekdayScheduleTable(void)
{
  for (uint8_t i = 0;
       i < EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_WEEKDAY_SCHEDULE_TABLE_SIZE;
       i++) {
    weekdayScheduleTable[i].inUse = false;
  }
}

static void clearYeardayScheduleTable(void)
{
  for (uint8_t i = 0;
       i < EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_YEARDAY_SCHEDULE_TABLE_SIZE;
       i++) {
    yeardayScheduleTable[i].inUse = false;
  }
}

static void clearHolidayScheduleTable(void)
{
  for (uint8_t i = 0;
       i < EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_HOLIDAY_SCHEDULE_TABLE_SIZE;
       i++) {
    holidayScheduleTable[i].inUse = false;
  }
}

static void clearDisposableScheduleTable(void)
{
  for (uint8_t i = 0;
       i < EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_PIN_USER_TABLE_SIZE;
       i++) {
    disposableScheduleTable[i].inUse = false;
  }
}

void emAfPluginDoorLockServerInitSchedule(void)
{
  clearWeekdayScheduleTable();
  clearYeardayScheduleTable();
  clearHolidayScheduleTable();
  clearDisposableScheduleTable();

#if defined(ZCL_USING_DOOR_LOCK_CLUSTER_NUM_WEEKDAY_SCHEDULES_SUPPORTED_PER_USER_ATTRIBUTE)  \
  || defined(ZCL_USING_DOOR_LOCK_CLUSTER_NUM_YEARDAY_SCHEDULES_SUPPORTED_PER_USER_ATTRIBUTE) \
  || defined(ZCL_USING_DOOR_LOCK_CLUSTER_NUM_HOLIDAY_SCHEDULES_SUPPORTED_PER_USER_ATTRIBUTE)
  const EmAfPluginDoorLockServerAttributeData data[] = {
#ifdef ZCL_USING_DOOR_LOCK_CLUSTER_NUM_WEEKDAY_SCHEDULES_SUPPORTED_PER_USER_ATTRIBUTE
    { ZCL_NUM_WEEKDAY_SCHEDULES_SUPPORTED_PER_USER_ATTRIBUTE_ID,
      EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_WEEKDAY_SCHEDULE_TABLE_SIZE },
#endif

#ifdef ZCL_USING_DOOR_LOCK_CLUSTER_NUM_YEARDAY_SCHEDULES_SUPPORTED_PER_USER_ATTRIBUTE
    { ZCL_NUM_YEARDAY_SCHEDULES_SUPPORTED_PER_USER_ATTRIBUTE_ID,
      EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_YEARDAY_SCHEDULE_TABLE_SIZE },
#endif

#ifdef ZCL_USING_DOOR_LOCK_CLUSTER_NUM_HOLIDAY_SCHEDULES_SUPPORTED_PER_USER_ATTRIBUTE
    { ZCL_NUM_HOLIDAY_SCHEDULES_SUPPORTED_PER_USER_ATTRIBUTE_ID,
      EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_HOLIDAY_SCHEDULE_TABLE_SIZE },
#endif
  };
  emAfPluginDoorLockServerWriteAttributes(data, COUNTOF(data), "schedule table");
#endif
}

static void sendResponse(const char *responseName)
{
  EmberStatus status = emberAfSendResponse();
  if (status != EMBER_SUCCESS) {
    emberAfDoorLockClusterPrintln("Failed to send %s: 0x%X",
                                  responseName,
                                  status);
  }
}

#ifdef UC_BUILD

bool emberAfDoorLockClusterSetWeekdayScheduleCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_door_lock_cluster_set_weekday_schedule_command_t cmd_data;
  uint8_t status = 0x00;
  uint8_t userPin = 0x00;
  uint16_t rfProgrammingEventMask = 0xffff; //event sent by default

  if (zcl_decode_door_lock_cluster_set_weekday_schedule_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if (!emAfPluginDoorLockServerCheckForSufficientSpace(cmd_data.scheduleId,
                                                       EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_WEEKDAY_SCHEDULE_TABLE_SIZE)
      || !emAfPluginDoorLockServerCheckForSufficientSpace(cmd_data.userId,
                                                          EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_PIN_USER_TABLE_SIZE)) {
    status = 0x01;
  }
  if (!status) {
    EmberAfDoorLockScheduleEntry *entry = &weekdayScheduleTable[cmd_data.scheduleId];
    entry->inUse = true;
    entry->userId = cmd_data.userId;
    entry->daysMask = cmd_data.daysMask;
    entry->startHour = cmd_data.startHour;
    entry->startMinute = cmd_data.startMinute;
    entry->stopHour = cmd_data.endHour;
    entry->stopMinute = cmd_data.endMinute;
    emberAfDoorLockClusterPrintln("***RX SET WEEKDAY SCHEDULE***");
    printWeekdayScheduleTable();
  }
  emberAfFillCommandDoorLockClusterSetWeekdayScheduleResponse(status);
  emberAfSendResponse();

  //get bitmask so we can check if we should send event notification
  emberAfReadServerAttribute(DOOR_LOCK_SERVER_ENDPOINT,
                             ZCL_DOOR_LOCK_CLUSTER_ID,
                             ZCL_RF_PROGRAMMING_EVENT_MASK_ATTRIBUTE_ID,
                             (uint8_t*)&rfProgrammingEventMask,
                             sizeof(rfProgrammingEventMask));

  if (rfProgrammingEventMask & BIT(0)) {
    emberAfFillCommandDoorLockClusterProgrammingEventNotification(0x01, 0x00, cmd_data.userId, &userPin, 0x00, 0x00, 0x00, &userPin);
    SEND_COMMAND_UNICAST_TO_BINDINGS();
  }

  return true;
}

bool emberAfDoorLockClusterGetWeekdayScheduleCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_door_lock_cluster_get_weekday_schedule_command_t cmd_data;
  EmberAfStatus zclStatus;
  EmberAfDoorLockScheduleEntry *entry;

  if (zcl_decode_door_lock_cluster_get_weekday_schedule_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  zclStatus = ((cmd_data.scheduleId > EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_WEEKDAY_SCHEDULE_TABLE_SIZE)
               ? EMBER_ZCL_STATUS_INVALID_FIELD
               : EMBER_ZCL_STATUS_SUCCESS);
  entry = &weekdayScheduleTable[0];
  if (zclStatus == EMBER_ZCL_STATUS_SUCCESS) {
    entry = &weekdayScheduleTable[cmd_data.scheduleId];
    zclStatus = (!entry->inUse
                 ? EMBER_ZCL_STATUS_NOT_FOUND
                 : (entry->userId != cmd_data.userId
                    ? EMBER_ZCL_STATUS_NOT_FOUND
                    : EMBER_ZCL_STATUS_SUCCESS));
  }

  if (zclStatus == EMBER_ZCL_STATUS_SUCCESS) {
    emberAfFillCommandDoorLockClusterGetWeekdayScheduleResponse(cmd_data.scheduleId,
                                                                cmd_data.userId,
                                                                zclStatus,
                                                                entry->daysMask,
                                                                entry->startHour,
                                                                entry->startMinute,
                                                                entry->stopHour,
                                                                entry->stopMinute);
  } else {
    // For error status, the schedule detail fields are omitted from the response.
    emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                               | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT
                               | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
                              ZCL_DOOR_LOCK_CLUSTER_ID,
                              ZCL_GET_WEEKDAY_SCHEDULE_RESPONSE_COMMAND_ID,
                              "uvu",
                              cmd_data.scheduleId,
                              cmd_data.userId,
                              zclStatus);
  }

  sendResponse("GetWeekdayScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterClearWeekdayScheduleCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_door_lock_cluster_clear_weekday_schedule_command_t cmd_data;
  EmberAfStatus zclStatus;

  if (zcl_decode_door_lock_cluster_clear_weekday_schedule_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  zclStatus = ((cmd_data.scheduleId > EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_WEEKDAY_SCHEDULE_TABLE_SIZE)
               ? EMBER_ZCL_STATUS_INVALID_FIELD
               : EMBER_ZCL_STATUS_SUCCESS);
  if (zclStatus == EMBER_ZCL_STATUS_SUCCESS) {
    weekdayScheduleTable[cmd_data.scheduleId].inUse = false;
    emAfPluginDoorLockServerSetPinUserType(cmd_data.userId,
                                           EMBER_ZCL_DOOR_LOCK_USER_TYPE_UNRESTRICTED);
  }

  emberAfFillCommandDoorLockClusterClearWeekdayScheduleResponse(zclStatus);

  sendResponse("ClearWeekdayScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterSetYeardayScheduleCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_door_lock_cluster_set_yearday_schedule_command_t cmd_data;
  uint8_t status;

  if (zcl_decode_door_lock_cluster_set_yearday_schedule_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if (cmd_data.scheduleId >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_YEARDAY_SCHEDULE_TABLE_SIZE) {
    status = 0x01; // failure (per 7.3.2.17.15)
  } else {
    yeardayScheduleTable[cmd_data.scheduleId].userId = cmd_data.userId;
    yeardayScheduleTable[cmd_data.scheduleId].localStartTime = cmd_data.localStartTime;
    yeardayScheduleTable[cmd_data.scheduleId].localEndTime = cmd_data.localEndTime;
    yeardayScheduleTable[cmd_data.scheduleId].inUse = true;
    status = 0x00; // success (per 7.3.2.17.15)
  }
  emberAfFillCommandDoorLockClusterSetYeardayScheduleResponse(status);

  sendResponse("SetYeardayScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterGetYeardayScheduleCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_door_lock_cluster_get_yearday_schedule_command_t cmd_data;
  EmberAfPluginDoorLockServerYeardayScheduleEntry *entry = &yeardayScheduleTable[0];
  EmberAfStatus zclStatus;

  if (zcl_decode_door_lock_cluster_get_yearday_schedule_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if (cmd_data.scheduleId >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_YEARDAY_SCHEDULE_TABLE_SIZE
      || cmd_data.userId >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_PIN_USER_TABLE_SIZE) {
    zclStatus = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else {
    entry = &yeardayScheduleTable[cmd_data.scheduleId];
    if (!entry->inUse || entry->userId != cmd_data.userId) {
      zclStatus = EMBER_ZCL_STATUS_NOT_FOUND;
    } else {
      zclStatus = EMBER_ZCL_STATUS_SUCCESS;
    }
  }

  if (zclStatus == EMBER_ZCL_STATUS_SUCCESS) {
    emberAfFillCommandDoorLockClusterGetYeardayScheduleResponse(cmd_data.scheduleId,
                                                                cmd_data.userId,
                                                                zclStatus,
                                                                entry->localStartTime,
                                                                entry->localEndTime);
  } else {
    // For error status, the schedule detail fields are omitted from the response.
    emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                               | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT
                               | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
                              ZCL_DOOR_LOCK_CLUSTER_ID,
                              ZCL_GET_YEARDAY_SCHEDULE_RESPONSE_COMMAND_ID,
                              "uvu",
                              cmd_data.scheduleId,
                              cmd_data.userId,
                              zclStatus);
  }

  sendResponse("GetYeardayScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterClearYeardayScheduleCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_door_lock_cluster_clear_yearday_schedule_command_t cmd_data;
  uint8_t status;

  if (zcl_decode_door_lock_cluster_clear_yearday_schedule_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if (cmd_data.scheduleId >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_YEARDAY_SCHEDULE_TABLE_SIZE) {
    status = 0x01; // failure (per 7.3.2.17.17)
  } else {
    yeardayScheduleTable[cmd_data.scheduleId].inUse = false;
    emAfPluginDoorLockServerSetPinUserType(cmd_data.userId,
                                           EMBER_ZCL_DOOR_LOCK_USER_TYPE_UNRESTRICTED);
    status = 0x00; // success (per 7.3.2.17.17)
  }
  emberAfFillCommandDoorLockClusterClearYeardayScheduleResponse(status);

  sendResponse("ClearYeardayScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterSetHolidayScheduleCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_door_lock_cluster_set_holiday_schedule_command_t cmd_data;
  uint8_t status;

  if (zcl_decode_door_lock_cluster_set_holiday_schedule_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if (cmd_data.scheduleId
      >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_HOLIDAY_SCHEDULE_TABLE_SIZE) {
    status = 0x01; // failure (per 7.3.2.17.18)
  } else {
    holidayScheduleTable[cmd_data.scheduleId].localStartTime
      = cmd_data.localStartTime;
    holidayScheduleTable[cmd_data.scheduleId].localEndTime
      = cmd_data.localEndTime;
    holidayScheduleTable[cmd_data.scheduleId].operatingModeDuringHoliday
      = cmd_data.operatingModeDuringHoliday;
    holidayScheduleTable[cmd_data.scheduleId].inUse
      = true;
    status = 0x00; // success (per 7.3.2.17.18)
  }
  emberAfFillCommandDoorLockClusterSetHolidayScheduleResponse(status);

  sendResponse("SetHolidayScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterGetHolidayScheduleCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_door_lock_cluster_get_holiday_schedule_command_t cmd_data;
  EmberAfPluginDoorLockServerHolidayScheduleEntry *entry = &holidayScheduleTable[0];
  EmberAfStatus zclStatus;

  if (zcl_decode_door_lock_cluster_get_holiday_schedule_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if (cmd_data.scheduleId >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_HOLIDAY_SCHEDULE_TABLE_SIZE) {
    zclStatus = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else {
    entry = &holidayScheduleTable[cmd_data.scheduleId];
    if (!entry->inUse) {
      zclStatus = EMBER_ZCL_STATUS_NOT_FOUND;
    } else {
      zclStatus = EMBER_ZCL_STATUS_SUCCESS;
    }
  }

  if (zclStatus == EMBER_ZCL_STATUS_SUCCESS) {
    emberAfFillCommandDoorLockClusterGetHolidayScheduleResponse(cmd_data.scheduleId,
                                                                zclStatus,
                                                                entry->localStartTime,
                                                                entry->localEndTime,
                                                                entry->operatingModeDuringHoliday);
  } else {
    // For error status, the schedule detail fields are omitted from the response.
    emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                               | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT
                               | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
                              ZCL_DOOR_LOCK_CLUSTER_ID,
                              ZCL_GET_HOLIDAY_SCHEDULE_RESPONSE_COMMAND_ID,
                              "uu",
                              cmd_data.scheduleId,
                              zclStatus);
  }

  sendResponse("GetHolidayScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterClearHolidayScheduleCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_door_lock_cluster_clear_holiday_schedule_command_t cmd_data;
  uint8_t status;

  if (zcl_decode_door_lock_cluster_clear_holiday_schedule_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if (cmd_data.scheduleId >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_YEARDAY_SCHEDULE_TABLE_SIZE) {
    status = 0x01; // failure (per 7.3.2.17.20)
  } else {
    holidayScheduleTable[cmd_data.scheduleId].inUse = false;
    status = 0x00; // success (per 7.3.2.17.20)
  }
  emberAfFillCommandDoorLockClusterClearHolidayScheduleResponse(status);

  sendResponse("ClearYeardayScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterSetDisposableScheduleCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_door_lock_cluster_set_disposable_schedule_command_t cmd_data;
  uint8_t status = 0;

  if (zcl_decode_door_lock_cluster_set_disposable_schedule_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfDoorLockClusterPrintln("Set Disposable Schedule ");

  if (cmd_data.userId >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_PIN_USER_TABLE_SIZE) {
    status = 0x01;
  } else {
    disposableScheduleTable[cmd_data.userId].localStartTime = cmd_data.localStartTime;
    disposableScheduleTable[cmd_data.userId].localEndTime = cmd_data.localEndTime;
    disposableScheduleTable[cmd_data.userId].inUse = true;
    status = 0x00;
  }

  emberAfFillCommandDoorLockClusterSetDisposableScheduleResponse(status);

  EmberStatus emberStatus = emberAfSendResponse();
  if (emberStatus != EMBER_SUCCESS) {
    emberAfDoorLockClusterPrintln("Failed to send SetDisposableScheduleResponse: 0x%X",
                                  emberStatus);
  }

  return true;
}

bool emberAfDoorLockClusterGetDisposableScheduleCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_door_lock_cluster_get_disposable_schedule_command_t cmd_data;
  EmberAfPluginDoorLockServerDisposableScheduleEntry *entry = &disposableScheduleTable[0];
  EmberAfStatus zclStatus;

  if (zcl_decode_door_lock_cluster_get_disposable_schedule_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfDoorLockClusterPrintln("Get Disposable Schedule ");

  if (cmd_data.userId >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_PIN_USER_TABLE_SIZE) {
    zclStatus = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else {
    entry = &disposableScheduleTable[cmd_data.userId];
    if (!entry->inUse) {
      zclStatus = EMBER_ZCL_STATUS_NOT_FOUND;
    } else {
      zclStatus = EMBER_ZCL_STATUS_SUCCESS;
    }
  }

  if (zclStatus == EMBER_ZCL_STATUS_SUCCESS) {
    emberAfFillCommandDoorLockClusterGetDisposableScheduleResponse(cmd_data.userId,
                                                                   zclStatus,
                                                                   entry->localStartTime,
                                                                   entry->localEndTime);
  } else {
    // For error status, the schedule detail fields are omitted from the response.
    emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                               | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT
                               | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
                              ZCL_DOOR_LOCK_CLUSTER_ID,
                              ZCL_GET_DISPOSABLE_SCHEDULE_RESPONSE_COMMAND_ID,
                              "vu",
                              cmd_data.userId,
                              zclStatus);
  }

  sendResponse("GetDisposableScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterClearDisposableScheduleCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_door_lock_cluster_clear_disposable_schedule_command_t cmd_data;
  uint8_t status = 0;

  if (zcl_decode_door_lock_cluster_clear_disposable_schedule_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfDoorLockClusterPrintln("Clear Disposable Schedule ");

  if (cmd_data.userId >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_PIN_USER_TABLE_SIZE) {
    status = 0x01;
  } else {
    disposableScheduleTable[cmd_data.userId].inUse = false;
    emAfPluginDoorLockServerSetPinUserType(cmd_data.userId,
                                           EMBER_ZCL_DOOR_LOCK_USER_TYPE_UNRESTRICTED);
    status = 0x00;
  }

  emberAfFillCommandDoorLockClusterClearDisposableScheduleResponse(status);

  sendResponse("ClearDisposableScheduleResponse");

  return true;
}

#else // !UC_BUILD

bool emberAfDoorLockClusterSetWeekdayScheduleCallback(uint8_t scheduleId,
                                                      uint16_t userId,
                                                      uint8_t daysMask,
                                                      uint8_t startHour,
                                                      uint8_t startMinute,
                                                      uint8_t stopHour,
                                                      uint8_t stopMinute)
{
  uint8_t status = 0x00;
  uint8_t userPin = 0x00;
  uint16_t rfProgrammingEventMask = 0xffff; //event sent by default
  if (!emAfPluginDoorLockServerCheckForSufficientSpace(scheduleId,
                                                       EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_WEEKDAY_SCHEDULE_TABLE_SIZE)
      || !emAfPluginDoorLockServerCheckForSufficientSpace(userId,
                                                          EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_PIN_USER_TABLE_SIZE)) {
    status = 0x01;
  }
  if (!status) {
    EmberAfDoorLockScheduleEntry *entry = &weekdayScheduleTable[scheduleId];
    entry->inUse = true;
    entry->userId = userId;
    entry->daysMask = daysMask;
    entry->startHour = startHour;
    entry->startMinute = startMinute;
    entry->stopHour = stopHour;
    entry->stopMinute = stopMinute;
    emberAfDoorLockClusterPrintln("***RX SET WEEKDAY SCHEDULE***");
    printWeekdayScheduleTable();
  }
  emberAfFillCommandDoorLockClusterSetWeekdayScheduleResponse(status);
  emberAfSendResponse();

  //get bitmask so we can check if we should send event notification
  emberAfReadServerAttribute(DOOR_LOCK_SERVER_ENDPOINT,
                             ZCL_DOOR_LOCK_CLUSTER_ID,
                             ZCL_RF_PROGRAMMING_EVENT_MASK_ATTRIBUTE_ID,
                             (uint8_t*)&rfProgrammingEventMask,
                             sizeof(rfProgrammingEventMask));

  if (rfProgrammingEventMask & BIT(0)) {
    emberAfFillCommandDoorLockClusterProgrammingEventNotification(0x01, 0x00, userId, &userPin, 0x00, 0x00, 0x00, &userPin);
    SEND_COMMAND_UNICAST_TO_BINDINGS();
  }

  return true;
}

bool emberAfDoorLockClusterGetWeekdayScheduleCallback(uint8_t scheduleId,
                                                      uint16_t userId)
{
  EmberAfStatus zclStatus
    = ((scheduleId
        > EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_WEEKDAY_SCHEDULE_TABLE_SIZE)
       ? EMBER_ZCL_STATUS_INVALID_FIELD
       : EMBER_ZCL_STATUS_SUCCESS);
  EmberAfDoorLockScheduleEntry *entry = &weekdayScheduleTable[0];
  if (zclStatus == EMBER_ZCL_STATUS_SUCCESS) {
    entry = &weekdayScheduleTable[scheduleId];
    zclStatus = (!entry->inUse
                 ? EMBER_ZCL_STATUS_NOT_FOUND
                 : (entry->userId != userId
                    ? EMBER_ZCL_STATUS_NOT_FOUND
                    : EMBER_ZCL_STATUS_SUCCESS));
  }

  if (zclStatus == EMBER_ZCL_STATUS_SUCCESS) {
    emberAfFillCommandDoorLockClusterGetWeekdayScheduleResponse(scheduleId,
                                                                userId,
                                                                zclStatus,
                                                                entry->daysMask,
                                                                entry->startHour,
                                                                entry->startMinute,
                                                                entry->stopHour,
                                                                entry->stopMinute);
  } else {
    // For error status, the schedule detail fields are omitted from the response.
    emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                               | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT
                               | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
                              ZCL_DOOR_LOCK_CLUSTER_ID,
                              ZCL_GET_WEEKDAY_SCHEDULE_RESPONSE_COMMAND_ID,
                              "uvu",
                              scheduleId,
                              userId,
                              zclStatus);
  }

  sendResponse("GetWeekdayScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterClearWeekdayScheduleCallback(uint8_t scheduleId,
                                                        uint16_t userId)
{
  EmberAfStatus zclStatus
    = ((scheduleId
        > EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_WEEKDAY_SCHEDULE_TABLE_SIZE)
       ? EMBER_ZCL_STATUS_INVALID_FIELD
       : EMBER_ZCL_STATUS_SUCCESS);
  if (zclStatus == EMBER_ZCL_STATUS_SUCCESS) {
    weekdayScheduleTable[scheduleId].inUse = false;
    emAfPluginDoorLockServerSetPinUserType(userId,
                                           EMBER_ZCL_DOOR_LOCK_USER_TYPE_UNRESTRICTED);
  }

  emberAfFillCommandDoorLockClusterClearWeekdayScheduleResponse(zclStatus);

  sendResponse("ClearWeekdayScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterSetYeardayScheduleCallback(uint8_t scheduleId,
                                                      uint16_t userId,
                                                      uint32_t localStartTime,
                                                      uint32_t localEndTime)
{
  uint8_t status;
  if (scheduleId
      >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_YEARDAY_SCHEDULE_TABLE_SIZE) {
    status = 0x01; // failure (per 7.3.2.17.15)
  } else {
    yeardayScheduleTable[scheduleId].userId = userId;
    yeardayScheduleTable[scheduleId].localStartTime = localStartTime;
    yeardayScheduleTable[scheduleId].localEndTime = localEndTime;
    yeardayScheduleTable[scheduleId].inUse = true;
    status = 0x00; // success (per 7.3.2.17.15)
  }
  emberAfFillCommandDoorLockClusterSetYeardayScheduleResponse(status);

  sendResponse("SetYeardayScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterGetYeardayScheduleCallback(uint8_t scheduleId,
                                                      uint16_t userId)
{
  EmberAfPluginDoorLockServerYeardayScheduleEntry *entry
    = &yeardayScheduleTable[0];
  EmberAfStatus zclStatus;
  if (scheduleId >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_YEARDAY_SCHEDULE_TABLE_SIZE
      || userId >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_PIN_USER_TABLE_SIZE) {
    zclStatus = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else {
    entry = &yeardayScheduleTable[scheduleId];
    if (!entry->inUse || entry->userId != userId) {
      zclStatus = EMBER_ZCL_STATUS_NOT_FOUND;
    } else {
      zclStatus = EMBER_ZCL_STATUS_SUCCESS;
    }
  }

  if (zclStatus == EMBER_ZCL_STATUS_SUCCESS) {
    emberAfFillCommandDoorLockClusterGetYeardayScheduleResponse(scheduleId,
                                                                userId,
                                                                zclStatus,
                                                                entry->localStartTime,
                                                                entry->localEndTime);
  } else {
    // For error status, the schedule detail fields are omitted from the response.
    emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                               | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT
                               | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
                              ZCL_DOOR_LOCK_CLUSTER_ID,
                              ZCL_GET_YEARDAY_SCHEDULE_RESPONSE_COMMAND_ID,
                              "uvu",
                              scheduleId,
                              userId,
                              zclStatus);
  }

  sendResponse("GetYeardayScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterClearYeardayScheduleCallback(uint8_t scheduleId,
                                                        uint16_t userId)
{
  uint8_t status;
  if (scheduleId
      >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_YEARDAY_SCHEDULE_TABLE_SIZE) {
    status = 0x01; // failure (per 7.3.2.17.17)
  } else {
    yeardayScheduleTable[scheduleId].inUse = false;
    emAfPluginDoorLockServerSetPinUserType(userId,
                                           EMBER_ZCL_DOOR_LOCK_USER_TYPE_UNRESTRICTED);
    status = 0x00; // success (per 7.3.2.17.17)
  }
  emberAfFillCommandDoorLockClusterClearYeardayScheduleResponse(status);

  sendResponse("ClearYeardayScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterSetHolidayScheduleCallback(uint8_t holidayScheduleId,
                                                      uint32_t localStartTime,
                                                      uint32_t localEndTime,
                                                      uint8_t operatingModeDuringHoliday)
{
  uint8_t status;
  if (holidayScheduleId
      >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_HOLIDAY_SCHEDULE_TABLE_SIZE) {
    status = 0x01; // failure (per 7.3.2.17.18)
  } else {
    holidayScheduleTable[holidayScheduleId].localStartTime
      = localStartTime;
    holidayScheduleTable[holidayScheduleId].localEndTime
      = localEndTime;
    holidayScheduleTable[holidayScheduleId].operatingModeDuringHoliday
      = operatingModeDuringHoliday;
    holidayScheduleTable[holidayScheduleId].inUse
      = true;
    status = 0x00; // success (per 7.3.2.17.18)
  }
  emberAfFillCommandDoorLockClusterSetHolidayScheduleResponse(status);

  sendResponse("SetHolidayScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterGetHolidayScheduleCallback(uint8_t holidayScheduleId)
{
  EmberAfPluginDoorLockServerHolidayScheduleEntry *entry
    = &holidayScheduleTable[0];
  EmberAfStatus zclStatus;
  if (holidayScheduleId
      >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_HOLIDAY_SCHEDULE_TABLE_SIZE) {
    zclStatus = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else {
    entry = &holidayScheduleTable[holidayScheduleId];
    if (!entry->inUse) {
      zclStatus = EMBER_ZCL_STATUS_NOT_FOUND;
    } else {
      zclStatus = EMBER_ZCL_STATUS_SUCCESS;
    }
  }

  if (zclStatus == EMBER_ZCL_STATUS_SUCCESS) {
    emberAfFillCommandDoorLockClusterGetHolidayScheduleResponse(holidayScheduleId,
                                                                zclStatus,
                                                                entry->localStartTime,
                                                                entry->localEndTime,
                                                                entry->operatingModeDuringHoliday);
  } else {
    // For error status, the schedule detail fields are omitted from the response.
    emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                               | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT
                               | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
                              ZCL_DOOR_LOCK_CLUSTER_ID,
                              ZCL_GET_HOLIDAY_SCHEDULE_RESPONSE_COMMAND_ID,
                              "uu",
                              holidayScheduleId,
                              zclStatus);
  }

  sendResponse("GetHolidayScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterClearHolidayScheduleCallback(uint8_t holidayScheduleId)
{
  uint8_t status;
  if (holidayScheduleId
      >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_YEARDAY_SCHEDULE_TABLE_SIZE) {
    status = 0x01; // failure (per 7.3.2.17.20)
  } else {
    holidayScheduleTable[holidayScheduleId].inUse = false;
    status = 0x00; // success (per 7.3.2.17.20)
  }
  emberAfFillCommandDoorLockClusterClearHolidayScheduleResponse(status);

  sendResponse("ClearYeardayScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterSetDisposableScheduleCallback(uint16_t userId,
                                                         uint32_t localStartTime,
                                                         uint32_t localEndTime)
{
  uint8_t status = 0;

  emberAfDoorLockClusterPrintln("Set Disposable Schedule ");

  if (userId >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_PIN_USER_TABLE_SIZE) {
    status = 0x01;
  } else {
    disposableScheduleTable[userId].localStartTime = localStartTime;
    disposableScheduleTable[userId].localEndTime = localEndTime;
    disposableScheduleTable[userId].inUse = true;
    status = 0x00;
  }

  emberAfFillCommandDoorLockClusterSetDisposableScheduleResponse(status);

  EmberStatus emberStatus = emberAfSendResponse();
  if (emberStatus != EMBER_SUCCESS) {
    emberAfDoorLockClusterPrintln("Failed to send SetDisposableScheduleResponse: 0x%X",
                                  emberStatus);
  }

  return true;
}

bool emberAfDoorLockClusterGetDisposableScheduleCallback(uint16_t userId)
{
  EmberAfPluginDoorLockServerDisposableScheduleEntry *entry = &disposableScheduleTable[0];
  EmberAfStatus zclStatus;

  emberAfDoorLockClusterPrintln("Get Disposable Schedule ");

  if (userId >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_PIN_USER_TABLE_SIZE) {
    zclStatus = EMBER_ZCL_STATUS_INVALID_FIELD;
  } else {
    entry = &disposableScheduleTable[userId];
    if (!entry->inUse) {
      zclStatus = EMBER_ZCL_STATUS_NOT_FOUND;
    } else {
      zclStatus = EMBER_ZCL_STATUS_SUCCESS;
    }
  }

  if (zclStatus == EMBER_ZCL_STATUS_SUCCESS) {
    emberAfFillCommandDoorLockClusterGetDisposableScheduleResponse(userId,
                                                                   zclStatus,
                                                                   entry->localStartTime,
                                                                   entry->localEndTime);
  } else {
    // For error status, the schedule detail fields are omitted from the response.
    emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                               | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT
                               | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
                              ZCL_DOOR_LOCK_CLUSTER_ID,
                              ZCL_GET_DISPOSABLE_SCHEDULE_RESPONSE_COMMAND_ID,
                              "vu",
                              userId,
                              zclStatus);
  }

  sendResponse("GetDisposableScheduleResponse");

  return true;
}

bool emberAfDoorLockClusterClearDisposableScheduleCallback(uint16_t userId)
{
  uint8_t status = 0;

  emberAfDoorLockClusterPrintln("Clear Disposable Schedule ");

  if (userId >= EMBER_AF_PLUGIN_DOOR_LOCK_SERVER_PIN_USER_TABLE_SIZE) {
    status = 0x01;
  } else {
    disposableScheduleTable[userId].inUse = false;
    emAfPluginDoorLockServerSetPinUserType(userId,
                                           EMBER_ZCL_DOOR_LOCK_USER_TYPE_UNRESTRICTED);
    status = 0x00;
  }

  emberAfFillCommandDoorLockClusterClearDisposableScheduleResponse(status);

  sendResponse("ClearDisposableScheduleResponse");

  return true;
}

#endif // UC_BUILD
