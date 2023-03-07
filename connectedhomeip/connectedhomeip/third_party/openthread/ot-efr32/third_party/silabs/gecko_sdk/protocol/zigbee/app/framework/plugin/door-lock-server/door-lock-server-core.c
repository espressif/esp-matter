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

static void setActuatorEnable(void)
{
  // The Door Lock cluster test spec expects this attribute set to be true by
  // default...
  bool troo = true;
  EmberAfStatus status
    = emberAfWriteServerAttribute(DOOR_LOCK_SERVER_ENDPOINT,
                                  ZCL_DOOR_LOCK_CLUSTER_ID,
                                  ZCL_ACTUATOR_ENABLED_ATTRIBUTE_ID,
                                  (uint8_t *)&troo,
                                  ZCL_BOOLEAN_ATTRIBUTE_TYPE);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfDoorLockClusterPrintln("Failed to write ActuatorEnabled attribute: 0x%X",
                                  status);
  }
}

static void setDoorState(void)
{
  uint8_t state = EMBER_ZCL_DOOR_STATE_ERROR_UNSPECIFIED;
  EmberAfStatus status = emberAfWriteServerAttribute(DOOR_LOCK_SERVER_ENDPOINT,
                                                     ZCL_DOOR_LOCK_CLUSTER_ID,
                                                     ZCL_DOOR_STATE_ATTRIBUTE_ID,
                                                     (uint8_t *)&state,
                                                     ZCL_ENUM8_ATTRIBUTE_TYPE);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfDoorLockClusterPrintln("Failed to write DoorState attribute: 0x%X",
                                  status);
  }
}

static void setLanguage(void)
{
  uint8_t englishString[] = { 0x02, 'e', 'n' };
  EmberAfStatus status
    = emberAfWriteServerAttribute(DOOR_LOCK_SERVER_ENDPOINT,
                                  ZCL_DOOR_LOCK_CLUSTER_ID,
                                  ZCL_LANGUAGE_ATTRIBUTE_ID,
                                  englishString,
                                  ZCL_CHAR_STRING_ATTRIBUTE_TYPE);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfDoorLockClusterPrintln("Failed to write Language attribute: 0x%X",
                                  status);
  }
}

void emberAfPluginDoorLockServerInitCallback(SLXU_INIT_ARG)
{
  SLXU_INIT_UNUSED_ARG;

#ifndef UC_BUILD
  // In UC this function is template-contributed directly in the event_init
  // context.
  emAfPluginDoorLockServerInitEvents();
#endif
  emAfPluginDoorLockServerInitUser();
  emAfPluginDoorLockServerInitSchedule();

  setActuatorEnable();
  setDoorState();
  setLanguage();
}

void emAfPluginDoorLockServerWriteAttributes(const EmAfPluginDoorLockServerAttributeData *data,
                                             uint8_t dataLength,
                                             const char *description)
{
  for (uint8_t i = 0; i < dataLength; i++) {
    EmberAfStatus status
      = emberAfWriteServerAttribute(DOOR_LOCK_SERVER_ENDPOINT,
                                    ZCL_DOOR_LOCK_CLUSTER_ID,
                                    data[i].id,
                                    (uint8_t *)&data[i].value,
                                    ZCL_INT16U_ATTRIBUTE_TYPE);
    if (status != EMBER_ZCL_STATUS_SUCCESS) {
      emberAfDoorLockClusterPrintln("Failed to write %s attribute 0x%2X: 0x%X",
                                    data[i].id,
                                    status,
                                    description);
    }
  }
}

EmberAfStatus emAfPluginDoorLockServerNoteDoorStateChanged(EmberAfDoorState state)
{
  EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;

#ifdef ZCL_USING_DOOR_LOCK_CLUSTER_DOOR_STATE_ATTRIBUTE
  status = emberAfWriteServerAttribute(DOOR_LOCK_SERVER_ENDPOINT,
                                       ZCL_DOOR_LOCK_CLUSTER_ID,
                                       ZCL_DOOR_STATE_ATTRIBUTE_ID,
                                       (uint8_t *)&state,
                                       ZCL_ENUM8_ATTRIBUTE_TYPE);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    return status;
  }
#endif

#if defined(ZCL_USING_DOOR_LOCK_CLUSTER_DOOR_OPEN_EVENTS_ATTRIBUTE) \
  || defined(ZCL_USING_DOOR_LOCK_CLUSTER_DOOR_CLOSED_EVENTS_ATTRIBUTE)
  if (state == EMBER_ZCL_DOOR_STATE_OPEN
      || state == EMBER_ZCL_DOOR_STATE_CLOSED) {
    EmberAfAttributeId attributeId = (state == EMBER_ZCL_DOOR_STATE_OPEN
                                      ? ZCL_DOOR_OPEN_EVENTS_ATTRIBUTE_ID
                                      : ZCL_DOOR_CLOSED_EVENTS_ATTRIBUTE_ID);
    uint32_t events;
    status = emberAfReadServerAttribute(DOOR_LOCK_SERVER_ENDPOINT,
                                        ZCL_DOOR_LOCK_CLUSTER_ID,
                                        attributeId,
                                        (uint8_t *)&events,
                                        sizeof(events));
    if (status == EMBER_ZCL_STATUS_SUCCESS) {
      events++;
      status = emberAfWriteServerAttribute(DOOR_LOCK_SERVER_ENDPOINT,
                                           ZCL_DOOR_LOCK_CLUSTER_ID,
                                           attributeId,
                                           (uint8_t *)&events,
                                           ZCL_INT32U_ATTRIBUTE_TYPE);
    }
  }
#endif

  return status;
}

#ifdef UC_BUILD

bool emberAfDoorLockClusterLockDoorCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterUnlockDoorCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterUnlockWithTimeoutCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterGetLogRecordCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterSetPinCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterGetPinCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterClearPinCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterClearAllPinsCallback(void);
bool emberAfDoorLockClusterSetWeekdayScheduleCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterGetWeekdayScheduleCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterClearWeekdayScheduleCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterSetYeardayScheduleCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterGetYeardayScheduleCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterClearYeardayScheduleCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterSetHolidayScheduleCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterGetHolidayScheduleCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterClearHolidayScheduleCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterSetUserTypeCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterGetUserTypeCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterSetRfidCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterGetRfidCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterClearRfidCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterClearAllRfidsCallback(void);
bool emberAfDoorLockClusterSetDisposableScheduleCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterGetDisposableScheduleCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterClearDisposableScheduleCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterClearBiometricCredentialCallback(EmberAfClusterCommand *cmd);
bool emberAfDoorLockClusterClearAllBiometricCredentialsCallback(void);

uint32_t emberAfDoorLockClusterServerCommandParse(sl_service_opcode_t opcode,
                                                  sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_LOCK_DOOR_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterLockDoorCallback(cmd);
        break;
      }
      case ZCL_UNLOCK_DOOR_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterUnlockDoorCallback(cmd);
        break;
      }
      case ZCL_UNLOCK_WITH_TIMEOUT_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterUnlockWithTimeoutCallback(cmd);
        break;
      }
      case ZCL_GET_LOG_RECORD_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterGetLogRecordCallback(cmd);
        break;
      }
      case ZCL_SET_PIN_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterSetPinCallback(cmd);
        break;
      }
      case ZCL_GET_PIN_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterGetPinCallback(cmd);
        break;
      }
      case ZCL_CLEAR_PIN_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterClearPinCallback(cmd);
        break;
      }
      case ZCL_CLEAR_ALL_PINS_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterClearAllPinsCallback();
        break;
      }
      case ZCL_SET_WEEKDAY_SCHEDULE_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterSetWeekdayScheduleCallback(cmd);
        break;
      }
      case ZCL_GET_WEEKDAY_SCHEDULE_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterGetWeekdayScheduleCallback(cmd);
        break;
      }
      case ZCL_CLEAR_WEEKDAY_SCHEDULE_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterClearWeekdayScheduleCallback(cmd);
        break;
      }
      case ZCL_SET_YEARDAY_SCHEDULE_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterSetYeardayScheduleCallback(cmd);
        break;
      }
      case ZCL_GET_YEARDAY_SCHEDULE_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterGetYeardayScheduleCallback(cmd);
        break;
      }
      case ZCL_CLEAR_YEARDAY_SCHEDULE_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterClearYeardayScheduleCallback(cmd);
        break;
      }
      case ZCL_SET_HOLIDAY_SCHEDULE_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterSetHolidayScheduleCallback(cmd);
        break;
      }
      case ZCL_GET_HOLIDAY_SCHEDULE_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterGetHolidayScheduleCallback(cmd);
        break;
      }
      case ZCL_CLEAR_HOLIDAY_SCHEDULE_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterClearHolidayScheduleCallback(cmd);
        break;
      }
      case ZCL_SET_USER_TYPE_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterSetUserTypeCallback(cmd);
        break;
      }
      case ZCL_GET_USER_TYPE_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterGetUserTypeCallback(cmd);
        break;
      }
      case ZCL_SET_RFID_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterSetRfidCallback(cmd);
        break;
      }
      case ZCL_GET_RFID_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterGetRfidCallback(cmd);
        break;
      }
      case ZCL_CLEAR_RFID_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterClearRfidCallback(cmd);
        break;
      }
      case ZCL_CLEAR_ALL_RFIDS_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterClearAllRfidsCallback();
        break;
      }
      case ZCL_SET_DISPOSABLE_SCHEDULE_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterSetDisposableScheduleCallback(cmd);
        break;
      }
      case ZCL_GET_DISPOSABLE_SCHEDULE_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterGetDisposableScheduleCallback(cmd);
        break;
      }
      case ZCL_CLEAR_DISPOSABLE_SCHEDULE_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterClearDisposableScheduleCallback(cmd);
        break;
      }
      case ZCL_CLEAR_BIOMETRIC_CREDENTIAL_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterClearBiometricCredentialCallback(cmd);
        break;
      }
      case ZCL_CLEAR_ALL_BIOMETRIC_CREDENTIALS_COMMAND_ID:
      {
        wasHandled = emberAfDoorLockClusterClearAllBiometricCredentialsCallback();
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
