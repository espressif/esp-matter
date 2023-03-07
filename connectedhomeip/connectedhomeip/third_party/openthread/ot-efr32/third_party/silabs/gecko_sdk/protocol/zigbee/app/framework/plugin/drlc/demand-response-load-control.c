/***************************************************************************//**
 * @file
 * @brief Routines for the DRLC plugin.
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

#include "../../include/af.h"
#include "../../util/common.h"
#include "demand-response-load-control.h"

#ifdef UC_BUILD
#include "drlc-config.h"
#include "zap-cluster-command-parser.h"
#endif // UC_BUILD

#include "app/framework/plugin/esi-management/esi-management.h"

//------------------------------------------------------------------------------

/**
 * Checks to see if the passed UEG OR deviceClass match those stored in
 * the cluster attributes.
 *
 * Returns true if either the device class or the UEG match. This
 * is the assumed interpretation of the spec..
 **/
static bool afMatchUegOrDeviceClass(uint8_t endpoint,
                                    uint8_t utilityEnrollmentGroup,
                                    uint16_t deviceClass)
{
  uint8_t       ueg      = 0;
  uint16_t      dc;
  uint8_t       dcArray[2];
  uint8_t       dataType = 0;
  uint8_t       status;

  status =
    emberAfReadAttribute(endpoint,
                         ZCL_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_ID,
                         ZCL_UTILITY_ENROLLMENT_GROUP_ATTRIBUTE_ID,
                         CLUSTER_MASK_CLIENT,
                         (uint8_t*)&ueg,
                         1,
                         &dataType);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  dataType = 0;
  status =
    emberAfReadAttribute(endpoint,
                         ZCL_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_ID,
                         ZCL_DEVICE_CLASS_VALUE_ATTRIBUTE_ID,
                         CLUSTER_MASK_CLIENT,
                         (uint8_t*)dcArray,
                         2,
                         &dataType);
  dc = (uint16_t)dcArray[0] | ((uint16_t)dcArray[1] << 8);
  emberAfDemandResponseLoadControlClusterPrint("Device Class: 0x%2x, status: 0x%x\r\n",
                                               dc,
                                               status);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }
  // Our thinking is that
  // device class should always match otherwise there would be no way to
  // narrow the group and there would be no way to address specific devices
  // that are not in a specific group. - WEH
  else if (!(dc & deviceClass)) {
    return false;
  }
  if (ueg == 0 || utilityEnrollmentGroup == 0 || (utilityEnrollmentGroup == ueg)) {
    return true;
  }
  return false;
}

static void addSignature(void)
{
  // The requirement of signing DRLC messages was removed from SE 1.1, however
  // we still must include the signature type and a dummy signature in the
  // message for backwards compatibility.
  appResponseData[appResponseLength] = EMBER_ZCL_SIGNATURE_TYPE_RESERVED;
  appResponseLength++;
  MEMSET(&(appResponseData[appResponseLength]), 0xFF, EMBER_SIGNATURE_SIZE);
  appResponseLength += EMBER_SIGNATURE_SIZE;
}

// -------------------------------------------------------
// DEMAND RESPONSE LOAD CONTROL CLIENT
// -------------------------------------------------------
static void fillReportEventData(uint8_t commandStatus,
                                uint8_t sequenceNumber,
                                EmberAfLoadControlEvent* e,
                                uint32_t statusTime)
{
  //emberAfDemandResponseLoadControlClusterFlush();
  emberAfDemandResponseLoadControlClusterPrintln("= TX Report Event Status =");
  //emberAfDemandResponseLoadControlClusterFlush();
  emberAfDemandResponseLoadControlClusterPrintln("  eid: %4x",
                                                 e->eventId);
  //emberAfDemandResponseLoadControlClusterFlush();
  emberAfDemandResponseLoadControlClusterPrintln("   es: %x",
                                                 commandStatus);
  emberAfDemandResponseLoadControlClusterPrintln("   st: T%4x",
                                                 statusTime);
  //emberAfDemandResponseLoadControlClusterFlush();
  emberAfDemandResponseLoadControlClusterPrintln("  cla: %x",
                                                 e->criticalityLevel);
  emberAfDemandResponseLoadControlClusterPrintln("   ec: %x", e->eventControl);
  emberAfDemandResponseLoadControlClusterPrintln("ctspa: %2x (%d)",
                                                 e->coolingTempSetPoint,
                                                 e->coolingTempSetPoint);
  //emberAfDemandResponseLoadControlClusterFlush();
  emberAfDemandResponseLoadControlClusterPrintln("htspa: %2x (%d)",
                                                 e->heatingTempSetPoint,
                                                 e->heatingTempSetPoint);
  emberAfDemandResponseLoadControlClusterPrintln("  avg: %x", e->avgLoadPercentage);
  //emberAfDemandResponseLoadControlClusterFlush();
  emberAfDemandResponseLoadControlClusterPrintln("   dc: %x", e->dutyCycle);
  emberAfDemandResponseLoadControlClusterFlush();

  appResponseLength = 0;
  (void) emberAfPutInt8uInResp(ZCL_CLUSTER_SPECIFIC_COMMAND
                               | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER);
  (void) emberAfPutInt8uInResp(sequenceNumber);
  (void) emberAfPutInt8uInResp(ZCL_REPORT_EVENT_STATUS_COMMAND_ID);
  (void) emberAfPutInt32uInResp(e->eventId);
  (void) emberAfPutInt8uInResp(commandStatus); //cmd rx
  (void) emberAfPutInt32uInResp(statusTime);
  (void) emberAfPutInt8uInResp(e->criticalityLevel);
  (void) emberAfPutInt16uInResp(e->coolingTempSetPoint);
  (void) emberAfPutInt16uInResp(e->heatingTempSetPoint);
  (void) emberAfPutInt8uInResp(e->avgLoadPercentage);
  (void) emberAfPutInt8uInResp(e->dutyCycle);
  (void) emberAfPutInt8uInResp(e->eventControl);
  addSignature();
}

/**
 * A mechanism for retrieving the right amount of randomization
 * if startRand is true, the table is requesting the amount of
 * start randomization, otherwise it is requesting the amount of
 * duration randomization. The value returned represents seconds of
 * randomization.
 */
static uint16_t afGetRandomizationTime(uint8_t endpoint,
                                       bool startRand)
{
  uint8_t maxRandMin, dataType;
  EmberStatus status =
    emberAfReadAttribute(endpoint,
                         ZCL_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_ID,
                         (startRand
                          ? ZCL_START_RANDOMIZATION_MINUTES_ATTRIBUTE_ID
                          : ZCL_DURATION_RANDOMIZATION_MINUTES_ATTRIBUTE_ID),
                         CLUSTER_MASK_CLIENT,
                         (uint8_t*)&maxRandMin,
                         1,
                         &dataType);
  if ( (status != EMBER_ZCL_STATUS_SUCCESS) || (maxRandMin == 0) ) {
    return 0; //not performed
  } else {
    uint16_t randNum, maxRandSec, randSec;
    randNum = emberGetPseudoRandomNumber();
    maxRandSec = maxRandMin * 60;
    randSec = randNum % maxRandSec;
    return randSec; //return value rep in seconds.
  }
}

void emberAfDemandResponseLoadControlClusterClientInitCallback(uint8_t endpoint)
{
  uint16_t deviceClass = EMBER_AF_PLUGIN_DRLC_DEVICE_CLASS;
  // Initialize device class
  (void) emberAfWriteAttribute(endpoint,
                               ZCL_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_ID,
                               ZCL_DEVICE_CLASS_VALUE_ATTRIBUTE_ID,
                               CLUSTER_MASK_CLIENT,
                               (uint8_t*)(&deviceClass),
                               ZCL_INT16U_ATTRIBUTE_TYPE);

  emAfLoadControlEventTableInit(endpoint);

  // Start tick
  slxu_zigbee_zcl_schedule_client_tick(endpoint,
                                       ZCL_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_ID,
                                       MILLISECOND_TICKS_PER_SECOND);
}

void emberAfDemandResponseLoadControlClusterClientTickCallback(uint8_t endpoint)
{
  emAfLoadControlEventTableTick(endpoint);
  slxu_zigbee_zcl_schedule_client_tick(endpoint,
                                       ZCL_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_ID,
                                       MILLISECOND_TICKS_PER_SECOND);
}

#ifdef UC_BUILD

bool emberAfDemandResponseLoadControlClusterLoadControlEventCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_demand_response_and_load_control_cluster_load_control_event_command_t cmd_data;
  EmberAfLoadControlEvent e;

  if (zcl_decode_demand_response_and_load_control_cluster_load_control_event_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfDemandResponseLoadControlClusterPrintln("= RX Load Control Event =");
  emberAfDemandResponseLoadControlClusterPrintln("  eid: %4x", cmd_data.issuerEventId);
  emberAfDemandResponseLoadControlClusterPrintln(" dc/g: %2x / %x",
                                                 cmd_data.deviceClass,
                                                 cmd_data.utilityEnrollmentGroup);
  emberAfDemandResponseLoadControlClusterPrintln("   st: T%4x", cmd_data.startTime);
  emberAfDemandResponseLoadControlClusterFlush();
  emberAfDemandResponseLoadControlClusterPrintln("  dur: %2x", cmd_data.durationInMinutes);
  emberAfDemandResponseLoadControlClusterPrintln("  cla: %x", cmd_data.criticalityLevel);
  emberAfDemandResponseLoadControlClusterPrintln("co/ho: %x / %x",
                                                 cmd_data.coolingTemperatureOffset,
                                                 cmd_data.heatingTemperatureOffset);
  emberAfDemandResponseLoadControlClusterFlush();
  emberAfDemandResponseLoadControlClusterPrintln("ctspa: %2x (%d)",
                                                 cmd_data.coolingTemperatureSetPoint,
                                                 cmd_data.coolingTemperatureSetPoint);
  emberAfDemandResponseLoadControlClusterPrintln("htspa: %2x (%d)",
                                                 cmd_data.heatingTemperatureSetPoint,
                                                 cmd_data.heatingTemperatureSetPoint);
  emberAfDemandResponseLoadControlClusterFlush();
  emberAfDemandResponseLoadControlClusterPrintln("  avg: %x",
                                                 cmd_data.averageLoadAdjustmentPercentage);
  emberAfDemandResponseLoadControlClusterPrintln("   dc: %x", cmd_data.dutyCycle);
  emberAfDemandResponseLoadControlClusterPrintln("   ec: %x", cmd_data.eventControl);
  emberAfDemandResponseLoadControlClusterFlush();

  if (!afMatchUegOrDeviceClass(emberAfCurrentEndpoint(),
                               cmd_data.utilityEnrollmentGroup,
                               cmd_data.deviceClass)) {
    emberAfDemandResponseLoadControlClusterPrintln(
      "Neither UEG nor Device class matched.  Ignoring.");

    // CCB 1380: Device must send back a default response of success, or do
    // nothing. We choose to do nothing.
    //emberAfSendDefaultResponse(emberAfCurrentCommand(),
    //                           EMBER_ZCL_STATUS_NOT_FOUND);
    return true;
  }

  // Reserved criticality level
  if (cmd_data.criticalityLevel == EMBER_ZCL_AMI_CRITICALITY_LEVEL_RESERVED
      || cmd_data.criticalityLevel > EMBER_ZCL_AMI_CRITICALITY_LEVEL_UTILITY_DEFINED6) {
    emberAfDemandResponseLoadControlClusterPrintln(
      "Reserved criticality level.  Ignoring.");
    return true;
  }

  e.destinationEndpoint = cmd->apsFrame->destinationEndpoint;

  e.eventId = cmd_data.issuerEventId;
  e.deviceClass = cmd_data.deviceClass;
  e.utilityEnrollmentGroup = cmd_data.utilityEnrollmentGroup;
  if (cmd_data.startTime == 0) {
    e.startTime = emberAfGetCurrentTime();
  } else {
    e.startTime = cmd_data.startTime;
  }
  e.duration = cmd_data.durationInMinutes;
  e.criticalityLevel = cmd_data.criticalityLevel;
  e.coolingTempOffset = cmd_data.coolingTemperatureOffset;
  e.heatingTempOffset = cmd_data.heatingTemperatureOffset;
  e.coolingTempSetPoint = cmd_data.coolingTemperatureSetPoint;
  e.heatingTempSetPoint = cmd_data.heatingTemperatureSetPoint;
  e.avgLoadPercentage = cmd_data.averageLoadAdjustmentPercentage;
  e.dutyCycle = cmd_data.dutyCycle;
  e.eventControl = cmd_data.eventControl;
  e.optionControl = EVENT_OPT_IN_DEFAULT;
  e.startRand = 0;
  e.durationRand = 0;
  e.esiBitmask = 0;

  if (cmd_data.eventControl & RANDOMIZE_START_TIME_FLAG) {
    e.startRand = afGetRandomizationTime(emberAfCurrentEndpoint(), true);
  }
  if (cmd_data.eventControl & RANDOMIZE_DURATION_TIME_FLAG) {
    e.durationRand = afGetRandomizationTime(emberAfCurrentEndpoint(), false);
  }

  emberAfDemandResponseLoadControlClusterPrintln(
    "schedule -- start: %4x, start-random: %2x duration-random: %2x",
    e.startTime,
    e.startRand,
    e.durationRand);

  emAfScheduleLoadControlEvent(emberAfCurrentEndpoint(), &e);
  return true;
}

bool emberAfDemandResponseLoadControlClusterCancelLoadControlEventCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_demand_response_and_load_control_cluster_cancel_load_control_event_command_t cmd_data;

  if (zcl_decode_demand_response_and_load_control_cluster_cancel_load_control_event_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  // FILTER ON DEVICE CLASS AND UTILITY ENROLLMENT GROUP
  if (!afMatchUegOrDeviceClass(emberAfCurrentEndpoint(),
                               cmd_data.utilityEnrollmentGroup,
                               cmd_data.deviceClass)) {
    // CCB 1380: Device must send back a default response of success, or do
    // nothing. We choose to do nothing.
    //emberAfSendDefaultResponse(emberAfCurrentCommand(), EMBER_ZCL_STATUS_NOT_FOUND);
    return true;
  }
  emAfCancelLoadControlEvent(emberAfCurrentEndpoint(),
                             cmd_data.issuerEventId,
                             cmd_data.cancelControl,
                             cmd_data.effectiveTime);
  return true;
}

bool emberAfDemandResponseLoadControlClusterCancelAllLoadControlEventsCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_demand_response_and_load_control_cluster_cancel_all_load_control_events_command_t cmd_data;

  if (zcl_decode_demand_response_and_load_control_cluster_cancel_all_load_control_events_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if (!emAfCancelAllLoadControlEvents(emberAfCurrentEndpoint(), cmd_data.cancelControl)) {
    // We didn't find any events to cancel but we were still successful in trying
    // So we return a default response of success.
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  }
  return true;
}

#else // !UC_BUILD

bool emberAfDemandResponseLoadControlClusterLoadControlEventCallback(uint32_t eventId,
                                                                     uint16_t deviceClass,
                                                                     uint8_t utilityEnrollmentGroup,
                                                                     uint32_t startTime,
                                                                     uint16_t duration,
                                                                     uint8_t criticalityLevel,
                                                                     uint8_t coolingTempOffset,
                                                                     uint8_t heatingTempOffset,
                                                                     int16_t coolingTempSetPoint,
                                                                     int16_t heatingTempSetPoint,
                                                                     int8_t averageLoadAdjustPercent,
                                                                     uint8_t dutyCycle,
                                                                     uint8_t eventControl)
{
  EmberAfLoadControlEvent e;
  EmberAfClusterCommand *cc = emberAfCurrentCommand();

  emberAfDemandResponseLoadControlClusterPrintln("= RX Load Control Event =");
  emberAfDemandResponseLoadControlClusterPrintln("  eid: %4x", eventId);
  emberAfDemandResponseLoadControlClusterPrintln(" dc/g: %2x / %x",
                                                 deviceClass,
                                                 utilityEnrollmentGroup);
  emberAfDemandResponseLoadControlClusterPrintln("   st: T%4x", startTime);
  emberAfDemandResponseLoadControlClusterFlush();
  emberAfDemandResponseLoadControlClusterPrintln("  dur: %2x", duration);
  emberAfDemandResponseLoadControlClusterPrintln("  cla: %x", criticalityLevel);
  emberAfDemandResponseLoadControlClusterPrintln("co/ho: %x / %x",
                                                 coolingTempOffset,
                                                 heatingTempOffset);
  emberAfDemandResponseLoadControlClusterFlush();
  emberAfDemandResponseLoadControlClusterPrintln("ctspa: %2x (%d)",
                                                 coolingTempSetPoint,
                                                 coolingTempSetPoint);
  emberAfDemandResponseLoadControlClusterPrintln("htspa: %2x (%d)",
                                                 heatingTempSetPoint,
                                                 heatingTempSetPoint);
  emberAfDemandResponseLoadControlClusterFlush();
  emberAfDemandResponseLoadControlClusterPrintln("  avg: %x",
                                                 averageLoadAdjustPercent);
  emberAfDemandResponseLoadControlClusterPrintln("   dc: %x", dutyCycle);
  emberAfDemandResponseLoadControlClusterPrintln("   ec: %x", eventControl);
  emberAfDemandResponseLoadControlClusterFlush();

  if (!afMatchUegOrDeviceClass(emberAfCurrentEndpoint(),
                               utilityEnrollmentGroup,
                               deviceClass)) {
    emberAfDemandResponseLoadControlClusterPrintln(
      "Neither UEG nor Device class matched.  Ignoring.");

    // CCB 1380: Device must send back a default response of success, or do
    // nothing. We choose to do nothing.
    //emberAfSendDefaultResponse(emberAfCurrentCommand(),
    //                           EMBER_ZCL_STATUS_NOT_FOUND);
    return true;
  }

  // Reserved criticality level
  if (criticalityLevel == EMBER_ZCL_AMI_CRITICALITY_LEVEL_RESERVED
      || criticalityLevel > EMBER_ZCL_AMI_CRITICALITY_LEVEL_UTILITY_DEFINED6) {
    emberAfDemandResponseLoadControlClusterPrintln(
      "Reserved criticality level.  Ignoring.");
    return true;
  }

  e.destinationEndpoint = cc->apsFrame->destinationEndpoint;

  e.eventId = eventId;
  e.deviceClass = deviceClass;
  e.utilityEnrollmentGroup = utilityEnrollmentGroup;
  if (startTime == 0) {
    e.startTime = emberAfGetCurrentTime();
  } else {
    e.startTime = startTime;
  }
  e.duration = duration;
  e.criticalityLevel = criticalityLevel;
  e.coolingTempOffset = coolingTempOffset;
  e.heatingTempOffset = heatingTempOffset;
  e.coolingTempSetPoint = coolingTempSetPoint;
  e.heatingTempSetPoint = heatingTempSetPoint;
  e.avgLoadPercentage = averageLoadAdjustPercent;
  e.dutyCycle = dutyCycle;
  e.eventControl = eventControl;
  e.optionControl = EVENT_OPT_IN_DEFAULT;
  e.startRand = 0;
  e.durationRand = 0;
  e.esiBitmask = 0;

  if (eventControl & RANDOMIZE_START_TIME_FLAG) {
    e.startRand = afGetRandomizationTime(emberAfCurrentEndpoint(), true);
  }
  if (eventControl & RANDOMIZE_DURATION_TIME_FLAG) {
    e.durationRand = afGetRandomizationTime(emberAfCurrentEndpoint(), false);
  }

  emberAfDemandResponseLoadControlClusterPrintln(
    "schedule -- start: %4x, start-random: %2x duration-random: %2x",
    e.startTime,
    e.startRand,
    e.durationRand);

  emAfScheduleLoadControlEvent(emberAfCurrentEndpoint(), &e);
  return true;
}

bool emberAfDemandResponseLoadControlClusterCancelLoadControlEventCallback(
  uint32_t eventId,
  uint16_t deviceClass,
  uint8_t utilityEnrollmentGroup,
  uint8_t cancelControl,
  uint32_t effectiveTime)
{
  // FILTER ON DEVICE CLASS AND UTILITY ENROLLMENT GROUP
  if (!afMatchUegOrDeviceClass(emberAfCurrentEndpoint(),
                               utilityEnrollmentGroup,
                               deviceClass)) {
    // CCB 1380: Device must send back a default response of success, or do
    // nothing. We choose to do nothing.
    //emberAfSendDefaultResponse(emberAfCurrentCommand(), EMBER_ZCL_STATUS_NOT_FOUND);
    return true;
  }
  emAfCancelLoadControlEvent(emberAfCurrentEndpoint(),
                             eventId,
                             cancelControl,
                             effectiveTime);
  return true;
}

bool emberAfDemandResponseLoadControlClusterCancelAllLoadControlEventsCallback(
  uint8_t cancelControl)
{
  if (!emAfCancelAllLoadControlEvents(emberAfCurrentEndpoint(), cancelControl)) {
    // We didn't find any events to cancel but we were still successful in trying
    // So we return a default response of success.
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  }
  return true;
}

#endif // UC_BUILD

void emberAfEventAction(EmberAfLoadControlEvent *event,
                        uint8_t eventStatus,
                        uint8_t sequenceNumber,
                        uint8_t esiIndex)
{
  bool sendReport;
  uint8_t i;

  // Here we defer to custom callback which can do whatever it wants with
  // the even.
  sendReport = emberAfPluginDrlcEventActionCallback(event,
                                                    (EmberAfAmiEventStatus)eventStatus,
                                                    sequenceNumber);

  if (!sendReport) {
    return;
  }

  for (i = 0; i < EMBER_AF_PLUGIN_ESI_MANAGEMENT_ESI_TABLE_SIZE; i++) {
    EmberAfPluginEsiManagementEsiEntry* esiEntry;
    uint8_t curIndex = (esiIndex < 0xFF) ? esiIndex : i;

    // Bit associated to this ESI not set, skip to the next one. If we are
    // sending to a specific ESI (i.e., esiIndex < 0xFF) the ESI table should
    // always contain an active entry at the passed index.
    if (esiIndex == 0xFF && (event->esiBitmask & BIT(curIndex)) == 0) {
      continue;
    }

    assert(curIndex < EMBER_AF_PLUGIN_ESI_MANAGEMENT_ESI_TABLE_SIZE);

    esiEntry = emberAfPluginEsiManagementEsiLookUpByIndex(curIndex);

    if (esiEntry != NULL) {
      fillReportEventData(eventStatus,
                          sequenceNumber,
                          event,
                          emberAfGetCurrentTime());

      assert(emberLookupNodeIdByEui64(esiEntry->eui64) == esiEntry->nodeId);
      emberAfResponseDestination = esiEntry->nodeId;
      emberAfResponseApsFrame.sourceEndpoint = event->destinationEndpoint;
      emberAfResponseApsFrame.destinationEndpoint = esiEntry->endpoint;

      emberAfResponseApsFrame.clusterId =
        ZCL_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_ID;
      emberAfSendResponse();

      // If we are sending the response to a specific ESI, we brake the for cycle
      // after we are done.
      if (esiIndex < 0xFF) {
        break;
      }
    } else {
      assert(false);
    }
  }
}

#ifdef UC_BUILD

uint32_t emberAfDemandResponseLoadControlClusterClientCommandParse(sl_service_opcode_t opcode,
                                                                   sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_LOAD_CONTROL_EVENT_COMMAND_ID:
      {
        wasHandled = emberAfDemandResponseLoadControlClusterLoadControlEventCallback(cmd);
        break;
      }
      case ZCL_CANCEL_LOAD_CONTROL_EVENT_COMMAND_ID:
      {
        wasHandled = emberAfDemandResponseLoadControlClusterCancelLoadControlEventCallback(cmd);
        break;
      }
      case ZCL_CANCEL_ALL_LOAD_CONTROL_EVENTS_COMMAND_ID:
      {
        wasHandled = emberAfDemandResponseLoadControlClusterCancelAllLoadControlEventsCallback(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
