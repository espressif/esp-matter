/***************************************************************************//**
 * @file
 * @brief Implemented routines for prepayment server.
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
#include "app/framework/util/util.h"
#include "prepayment-server.h"
#include "prepayment-debt-log.h"
#include "prepayment-debt-schedule.h"
#include "prepayment-modes-table.h"

#ifdef UC_BUILD
#include "prepayment-server-config.h"
#include "sl_component_catalog.h"
#include "zap-cluster-command-parser.h"
#else // !UC_BUILD
#define SL_CATALOG_ZIGBEE_CALENDAR_CLIENT_PRESENT
#ifdef EMBER_AF_PLUGIN_GBCS_COMPATIBILITY
#define SL_CATALOG_ZIGBEE_GBCS_COMPATIBILITY_PRESENT
#endif
extern void emberAfPluginPrepaymentSnapshotStorageInitCallback(void);
#endif // UC_BUILD
#include "../calendar-client/calendar-client.h"

typedef uint16_t PaymentControlConfiguration;
typedef uint8_t  FriendlyCredit;

extern uint8_t emberAfPluginPrepaymentServerGetSnapshotCallback(EmberNodeId nodeId, uint8_t srcEndpoint, uint8_t dstEndpoint,
                                                                uint32_t startTime,
                                                                uint32_t endTime,
                                                                uint8_t  snapshotOffset,
                                                                uint32_t snapshotCause);

void emberAfPrepaymentClusterServerInitCallback(uint8_t endpoint)
{
  uint8_t ep = emberAfFindClusterServerEndpointIndex(endpoint, ZCL_PREPAYMENT_CLUSTER_ID);

#ifndef UC_BUILD
  emberAfPluginPrepaymentSnapshotStorageInitCallback();
#endif
  emInitPrepaymentModesTable();
  emberAfPluginPrepaymentServerInitDebtLog();
  emberAfPluginPrepaymentServerInitDebtSchedule();

  if ( ep == 0xFF ) {
    return;
  }
}

//-----------------------
// ZCL commands callbacks

#ifdef UC_BUILD

bool emberAfPrepaymentClusterSelectAvailableEmergencyCreditCallback(EmberAfClusterCommand *cmd)
{
  (void)cmd;

  emberAfPrepaymentClusterPrintln("Rx: Select Available Emergency Credit");
  return true;
}

#define CUTOFF_UNCHANGED 0xFFFFFFFF
bool emberAfPrepaymentClusterChangePaymentModeCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_prepayment_cluster_change_payment_mode_command_t cmd_data;
  // The requester can be obtained with emberAfResponseDestination;
  EmberNodeId nodeId;
  uint8_t endpoint;
  uint8_t srcEndpoint, dstEndpoint;
  FriendlyCredit friendlyCredit;
  uint32_t friendlyCreditCalendarId;
  uint32_t emergencyCreditLimit;
  uint32_t emergencyCreditThreshold;
  uint8_t  dataType;
  uint8_t  i;

  if (zcl_decode_prepayment_cluster_change_payment_mode_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfPrepaymentClusterPrintln("RX: ChangePaymentMode, pid=0x%4x, eid=0x%4x, cfg=0x%2x", cmd_data.providerId, cmd_data.issuerEventId, cmd_data.proposedPaymentControlConfiguration);
  endpoint = emberAfCurrentEndpoint();

  if ( cmd_data.cutOffValue != CUTOFF_UNCHANGED ) {
#ifdef ZCL_USING_PREPAYMENT_CLUSTER_CUT_OFF_VALUE_ATTRIBUTE
    (void) emberAfWriteAttribute(endpoint, ZCL_PREPAYMENT_CLUSTER_ID,
                                 ZCL_CUT_OFF_VALUE_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                                 (uint8_t *)&cmd_data.cutOffValue, ZCL_INT32S_ATTRIBUTE_TYPE);
#endif
  }

  emberAfPrepaymentSchedulePrepaymentMode(emberAfCurrentEndpoint(), cmd_data.providerId, cmd_data.issuerEventId, cmd_data.implementationDateTime,
                                          cmd_data.proposedPaymentControlConfiguration);

  // Setup the friendly credit & emergency credit limit attributes.
#ifdef SL_CATALOG_ZIGBEE_CALENDAR_CLIENT_PRESENT
  i = emberAfPluginCalendarClientGetCalendarIndexByType(endpoint, EMBER_ZCL_CALENDAR_TYPE_FRIENDLY_CREDIT_CALENDAR);
  friendlyCredit = (i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS) ? 0x01 : 0x00;
  friendlyCreditCalendarId = emberAfPluginCalendarClientGetCalendarId(endpoint, i);
#else // !SL_CATALOG_ZIGBEE_CALENDAR_CLIENT_PRESENT
  friendlyCredit = 0x00;
  friendlyCreditCalendarId = EMBER_AF_PLUGIN_CALENDAR_CLIENT_INVALID_CALENDAR_ID;
#endif // SL_CATALOG_ZIGBEE_CALENDAR_CLIENT_PRESENT

#if (!defined ZCL_USING_PREPAYMENT_CLUSTER_EMERGENCY_CREDIT_LIMIT_ALLOWANCE_ATTRIBUTE) \
  || (!defined ZCL_USING_PREPAYMENT_CLUSTER_EMERGENCY_CREDIT_THRESHOLD_ATTRIBUTE)
#error "Prepayment Emergency Credit Limit/Allowance and Threshold attributes required for this plugin!"
#endif
  emberAfReadAttribute(emberAfCurrentEndpoint(), ZCL_PREPAYMENT_CLUSTER_ID,
                       ZCL_EMERGENCY_CREDIT_LIMIT_ALLOWANCE_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                       (uint8_t *)&emergencyCreditLimit, 4, &dataType);
  emberAfReadAttribute(emberAfCurrentEndpoint(), ZCL_PREPAYMENT_CLUSTER_ID,
                       ZCL_EMERGENCY_CREDIT_THRESHOLD_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                       (uint8_t *)&emergencyCreditThreshold, 4, &dataType);
  nodeId = emberAfCurrentCommand()->source;
  srcEndpoint = emberAfGetCommandApsFrame()->destinationEndpoint;
  dstEndpoint = emberAfGetCommandApsFrame()->sourceEndpoint;
  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);

#ifdef SL_CATALOG_ZIGBEE_GBCS_COMPATIBILITY_PRESENT
  // GBCS explicitly lists some commands that need to be sent with "disable
  // default response" flag set. This is one of them.
  // We make it conditional on GBCS so it does not affect standard SE apps.
  emberAfSetDisableDefaultResponse(EMBER_AF_DISABLE_DEFAULT_RESPONSE_ONE_SHOT);
#endif

  emberAfFillCommandPrepaymentClusterChangePaymentModeResponse(friendlyCredit, friendlyCreditCalendarId,
                                                               emergencyCreditLimit, emergencyCreditThreshold);
  emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, nodeId);
  return true;
}

bool emberAfPrepaymentClusterEmergencyCreditSetupCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_prepayment_cluster_emergency_credit_setup_command_t cmd_data;

  if (zcl_decode_prepayment_cluster_emergency_credit_setup_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfPrepaymentClusterPrintln("Rx: Emergency Credit Setup");
#ifdef ZCL_USING_PREPAYMENT_CLUSTER_EMERGENCY_CREDIT_LIMIT_ALLOWANCE_ATTRIBUTE
  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(), ZCL_PREPAYMENT_CLUSTER_ID,
                               ZCL_EMERGENCY_CREDIT_LIMIT_ALLOWANCE_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                               (uint8_t *)&cmd_data.emergencyCreditLimit, ZCL_INT32U_ATTRIBUTE_TYPE);
#else
  #error "Prepayment Emergency Credit Limit Allowance attribute is required for this plugin."
#endif

#ifdef ZCL_USING_PREPAYMENT_CLUSTER_EMERGENCY_CREDIT_THRESHOLD_ATTRIBUTE
  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(), ZCL_PREPAYMENT_CLUSTER_ID,
                               ZCL_EMERGENCY_CREDIT_THRESHOLD_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                               (uint8_t *)&cmd_data.emergencyCreditThreshold, ZCL_INT32U_ATTRIBUTE_TYPE);
#else
  #error "Prepayment Emergency Credit Threshold attribute is required for this plugin."
#endif
  return true;
}

enum {
  CREDIT_ADJUSTMENT_TYPE_INCREMENTAL = 0x00,
  CREDIT_ADJUSTMENT_TYPE_ABSOLUTE    = 0x01,
};

bool emberAfPrepaymentClusterCreditAdjustmentCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_prepayment_cluster_credit_adjustment_command_t cmd_data;

  if (zcl_decode_prepayment_cluster_credit_adjustment_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

#ifdef ZCL_USING_PREPAYMENT_CLUSTER_CREDIT_REMAINING_TIMESTAMP_ATTRIBUTE
  uint32_t currTimeUtc;
#endif
  int32_t currCreditAdjustmentValue;
  uint8_t  dataType;

  emberAfPrepaymentClusterPrintln("Rx: Credit Adjustment");
#ifdef ZCL_USING_PREPAYMENT_CLUSTER_CREDIT_REMAINING_ATTRIBUTE
  if ( cmd_data.creditAdjustmentType == CREDIT_ADJUSTMENT_TYPE_INCREMENTAL ) {
    // Read current value, then add it to the adjustment.
    emberAfReadAttribute(emberAfCurrentEndpoint(), ZCL_PREPAYMENT_CLUSTER_ID,
                         ZCL_CREDIT_REMAINING_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                         (uint8_t *)&currCreditAdjustmentValue, 4, &dataType);
    currCreditAdjustmentValue += cmd_data.creditAdjustmentValue;
  }

  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(), ZCL_PREPAYMENT_CLUSTER_ID,
                               ZCL_CREDIT_REMAINING_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                               (uint8_t *)&cmd_data.creditAdjustmentValue, ZCL_INT32S_ATTRIBUTE_TYPE);
#else
  #error "Prepayment Credit Adjustment attribute is required for this plugin."
#endif

#ifdef ZCL_USING_PREPAYMENT_CLUSTER_CREDIT_REMAINING_TIMESTAMP_ATTRIBUTE
  // This one is optional - we'll track it if supported.
  currTimeUtc = emberAfGetCurrentTime();
  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(), ZCL_PREPAYMENT_CLUSTER_ID,
                               ZCL_CREDIT_REMAINING_TIMESTAMP_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                               (uint8_t *)&currTimeUtc, ZCL_UTC_TIME_ATTRIBUTE_TYPE);
#endif

  return true;
}

#define MAX_SNAPSHOT_PAYLOAD_LEN  24
bool emberAfPrepaymentClusterGetPrepaySnapshotCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_prepayment_cluster_get_prepay_snapshot_command_t cmd_data;
  EmberNodeId nodeId;
  uint8_t srcEndpoint, dstEndpoint;

  if (zcl_decode_prepayment_cluster_get_prepay_snapshot_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfPrepaymentClusterPrintln("RX: GetPrepaySnapshot, st=0x%4x, offset=%d, cause=%d",
                                  cmd_data.earliestStartTime, cmd_data.snapshotOffset, cmd_data.snapshotCause);
  nodeId = emberAfCurrentCommand()->source;
  srcEndpoint = emberAfGetCommandApsFrame()->destinationEndpoint;
  dstEndpoint = emberAfGetCommandApsFrame()->sourceEndpoint;
  emberAfPrepaymentClusterPrintln("... from 0x%2x, ep=%d", nodeId, dstEndpoint);

  emberAfPluginPrepaymentServerGetSnapshotCallback(nodeId, srcEndpoint, dstEndpoint,
                                                   cmd_data.earliestStartTime, cmd_data.latestEndTime, cmd_data.snapshotOffset, cmd_data.snapshotCause);
  return true;
}

#else // !UC_BUILD

bool emberAfPrepaymentClusterSelectAvailableEmergencyCreditCallback(uint32_t commandIssueDateTime,
                                                                    uint8_t originatingDevice)
{
  emberAfPrepaymentClusterPrintln("Rx: Select Available Emergency Credit");
  return true;
}

#define CUTOFF_UNCHANGED 0xFFFFFFFF
bool emberAfPrepaymentClusterChangePaymentModeCallback(uint32_t providerId,
                                                       uint32_t issuerEventId,
                                                       uint32_t implementationDateTime,
                                                       PaymentControlConfiguration proposedPaymentControlConfiguration,
                                                       uint32_t cutOffValue)
{
  // The requester can be obtained with emberAfResponseDestination;
  EmberNodeId nodeId;
  uint8_t endpoint;
  uint8_t srcEndpoint, dstEndpoint;
  FriendlyCredit friendlyCredit;
  uint32_t friendlyCreditCalendarId;
  uint32_t emergencyCreditLimit;
  uint32_t emergencyCreditThreshold;
  uint8_t  dataType;
  uint8_t  i;

  emberAfPrepaymentClusterPrintln("RX: ChangePaymentMode, pid=0x%4x, eid=0x%4x, cfg=0x%2x", providerId, issuerEventId, proposedPaymentControlConfiguration);
  endpoint = emberAfCurrentEndpoint();

  if ( cutOffValue != CUTOFF_UNCHANGED ) {
#ifdef ZCL_USING_PREPAYMENT_CLUSTER_CUT_OFF_VALUE_ATTRIBUTE
    (void) emberAfWriteAttribute(endpoint, ZCL_PREPAYMENT_CLUSTER_ID,
                                 ZCL_CUT_OFF_VALUE_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                                 (uint8_t *)&cutOffValue, ZCL_INT32S_ATTRIBUTE_TYPE);
#endif
  }

  emberAfPrepaymentSchedulePrepaymentMode(emberAfCurrentEndpoint(), providerId, issuerEventId, implementationDateTime,
                                          proposedPaymentControlConfiguration);

  // Setup the friendly credit & emergency credit limit attributes.
#ifdef SL_CATALOG_ZIGBEE_CALENDAR_CLIENT_PRESENT
  i = emberAfPluginCalendarClientGetCalendarIndexByType(endpoint, EMBER_ZCL_CALENDAR_TYPE_FRIENDLY_CREDIT_CALENDAR);
  friendlyCredit = (i < EMBER_AF_PLUGIN_CALENDAR_CLIENT_CALENDARS) ? 0x01 : 0x00;
  friendlyCreditCalendarId = emberAfPluginCalendarClientGetCalendarId(endpoint, i);
#else // !SL_CATALOG_ZIGBEE_CALENDAR_CLIENT_PRESENT
  friendlyCredit = 0x00;
  friendlyCreditCalendarId = EMBER_AF_PLUGIN_CALENDAR_CLIENT_INVALID_CALENDAR_ID;
#endif // SL_CATALOG_ZIGBEE_CALENDAR_CLIENT_PRESENT

#if (!defined ZCL_USING_PREPAYMENT_CLUSTER_EMERGENCY_CREDIT_LIMIT_ALLOWANCE_ATTRIBUTE) \
  || (!defined ZCL_USING_PREPAYMENT_CLUSTER_EMERGENCY_CREDIT_THRESHOLD_ATTRIBUTE)
#error "Prepayment Emergency Credit Limit/Allowance and Threshold attributes required for this plugin!"
#endif
  emberAfReadAttribute(emberAfCurrentEndpoint(), ZCL_PREPAYMENT_CLUSTER_ID,
                       ZCL_EMERGENCY_CREDIT_LIMIT_ALLOWANCE_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                       (uint8_t *)&emergencyCreditLimit, 4, &dataType);
  emberAfReadAttribute(emberAfCurrentEndpoint(), ZCL_PREPAYMENT_CLUSTER_ID,
                       ZCL_EMERGENCY_CREDIT_THRESHOLD_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                       (uint8_t *)&emergencyCreditThreshold, 4, &dataType);
  nodeId = emberAfCurrentCommand()->source;
  srcEndpoint = emberAfGetCommandApsFrame()->destinationEndpoint;
  dstEndpoint = emberAfGetCommandApsFrame()->sourceEndpoint;
  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);

#ifdef SL_CATALOG_ZIGBEE_GBCS_COMPATIBILITY_PRESENT
  // GBCS explicitly lists some commands that need to be sent with "disable
  // default response" flag set. This is one of them.
  // We make it conditional on GBCS so it does not affect standard SE apps.
  emberAfSetDisableDefaultResponse(EMBER_AF_DISABLE_DEFAULT_RESPONSE_ONE_SHOT);
#endif

  emberAfFillCommandPrepaymentClusterChangePaymentModeResponse(friendlyCredit, friendlyCreditCalendarId,
                                                               emergencyCreditLimit, emergencyCreditThreshold);
  emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, nodeId);
  return true;
}

bool emberAfPrepaymentClusterEmergencyCreditSetupCallback(uint32_t issuerEventId,
                                                          uint32_t startTime,
                                                          uint32_t emergencyCreditLimit,
                                                          uint32_t emergencyCreditThreshold)
{
  emberAfPrepaymentClusterPrintln("Rx: Emergency Credit Setup");
#ifdef ZCL_USING_PREPAYMENT_CLUSTER_EMERGENCY_CREDIT_LIMIT_ALLOWANCE_ATTRIBUTE
  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(), ZCL_PREPAYMENT_CLUSTER_ID,
                               ZCL_EMERGENCY_CREDIT_LIMIT_ALLOWANCE_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                               (uint8_t *)&emergencyCreditLimit, ZCL_INT32U_ATTRIBUTE_TYPE);
#else
  #error "Prepayment Emergency Credit Limit Allowance attribute is required for this plugin."
#endif

#ifdef ZCL_USING_PREPAYMENT_CLUSTER_EMERGENCY_CREDIT_THRESHOLD_ATTRIBUTE
  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(), ZCL_PREPAYMENT_CLUSTER_ID,
                               ZCL_EMERGENCY_CREDIT_THRESHOLD_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                               (uint8_t *)&emergencyCreditThreshold, ZCL_INT32U_ATTRIBUTE_TYPE);
#else
  #error "Prepayment Emergency Credit Threshold attribute is required for this plugin."
#endif
  return true;
}

enum {
  CREDIT_ADJUSTMENT_TYPE_INCREMENTAL = 0x00,
  CREDIT_ADJUSTMENT_TYPE_ABSOLUTE    = 0x01,
};

bool emberAfPrepaymentClusterCreditAdjustmentCallback(uint32_t issuerEventId,
                                                      uint32_t startTime,
                                                      uint8_t creditAdjustmentType,
                                                      uint32_t creditAdjustmentValue)
{
#ifdef ZCL_USING_PREPAYMENT_CLUSTER_CREDIT_REMAINING_TIMESTAMP_ATTRIBUTE
  uint32_t currTimeUtc;
#endif
  int32_t currCreditAdjustmentValue;
  uint8_t  dataType;

  emberAfPrepaymentClusterPrintln("Rx: Credit Adjustment");
#ifdef ZCL_USING_PREPAYMENT_CLUSTER_CREDIT_REMAINING_ATTRIBUTE
  if ( creditAdjustmentType == CREDIT_ADJUSTMENT_TYPE_INCREMENTAL ) {
    // Read current value, then add it to the adjustment.
    emberAfReadAttribute(emberAfCurrentEndpoint(), ZCL_PREPAYMENT_CLUSTER_ID,
                         ZCL_CREDIT_REMAINING_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                         (uint8_t *)&currCreditAdjustmentValue, 4, &dataType);
    currCreditAdjustmentValue += creditAdjustmentValue;
  }

  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(), ZCL_PREPAYMENT_CLUSTER_ID,
                               ZCL_CREDIT_REMAINING_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                               (uint8_t *)&creditAdjustmentValue, ZCL_INT32S_ATTRIBUTE_TYPE);
#else
  #error "Prepayment Credit Adjustment attribute is required for this plugin."
#endif

#ifdef ZCL_USING_PREPAYMENT_CLUSTER_CREDIT_REMAINING_TIMESTAMP_ATTRIBUTE
  // This one is optional - we'll track it if supported.
  currTimeUtc = emberAfGetCurrentTime();
  (void) emberAfWriteAttribute(emberAfCurrentEndpoint(), ZCL_PREPAYMENT_CLUSTER_ID,
                               ZCL_CREDIT_REMAINING_TIMESTAMP_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                               (uint8_t *)&currTimeUtc, ZCL_UTC_TIME_ATTRIBUTE_TYPE);
#endif

  return true;
}

#define MAX_SNAPSHOT_PAYLOAD_LEN  24
bool emberAfPrepaymentClusterGetPrepaySnapshotCallback(uint32_t earliestStartTime, uint32_t latestEndTime,
                                                       uint8_t snapshotOffset, uint32_t snapshotCause)
{
  EmberNodeId nodeId;
  uint8_t srcEndpoint, dstEndpoint;

  emberAfPrepaymentClusterPrintln("RX: GetPrepaySnapshot, st=0x%4x, offset=%d, cause=%d", earliestStartTime, snapshotOffset, snapshotCause);
  nodeId = emberAfCurrentCommand()->source;
  srcEndpoint = emberAfGetCommandApsFrame()->destinationEndpoint;
  dstEndpoint = emberAfGetCommandApsFrame()->sourceEndpoint;
  emberAfPrepaymentClusterPrintln("... from 0x%2x, ep=%d", nodeId, dstEndpoint);

  emberAfPluginPrepaymentServerGetSnapshotCallback(nodeId, srcEndpoint, dstEndpoint,
                                                   earliestStartTime, latestEndTime, snapshotOffset, snapshotCause);
  return true;
}

#endif // UC_BUILD

#ifdef UC_BUILD

bool emberAfPrepaymentClusterChangeDebtCallback(EmberAfClusterCommand *cmd);
bool emberAfPrepaymentClusterGetDebtRepaymentLogCallback(EmberAfClusterCommand *cmd);
bool emberAfPrepaymentClusterConsumerTopUpCallback(EmberAfClusterCommand *cmd);
bool emberAfPrepaymentClusterGetTopUpLogCallback(EmberAfClusterCommand *cmd);

uint32_t emberAfPrepaymentClusterServerCommandParse(sl_service_opcode_t opcode,
                                                    sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  switch (cmd->commandId) {
    case ZCL_SELECT_AVAILABLE_EMERGENCY_CREDIT_COMMAND_ID:
    {
      wasHandled = emberAfPrepaymentClusterSelectAvailableEmergencyCreditCallback(cmd);
      break;
    }
    case ZCL_CHANGE_DEBT_COMMAND_ID:
    {
      wasHandled = emberAfPrepaymentClusterChangeDebtCallback(cmd);
      break;
    }
    case ZCL_EMERGENCY_CREDIT_SETUP_COMMAND_ID:
    {
      wasHandled = emberAfPrepaymentClusterEmergencyCreditSetupCallback(cmd);
      break;
    }
    case ZCL_CONSUMER_TOP_UP_COMMAND_ID:
    {
      wasHandled = emberAfPrepaymentClusterConsumerTopUpCallback(cmd);
      break;
    }
    case ZCL_CREDIT_ADJUSTMENT_COMMAND_ID:
    {
      wasHandled = emberAfPrepaymentClusterCreditAdjustmentCallback(cmd);
      break;
    }
    case ZCL_CHANGE_PAYMENT_MODE_COMMAND_ID:
    {
      wasHandled = emberAfPrepaymentClusterChangePaymentModeCallback(cmd);
      break;
    }
    case ZCL_GET_PREPAY_SNAPSHOT_COMMAND_ID:
    {
      wasHandled = emberAfPrepaymentClusterGetPrepaySnapshotCallback(cmd);
      break;
    }
    case ZCL_GET_TOP_UP_LOG_COMMAND_ID:
    {
      wasHandled = emberAfPrepaymentClusterGetTopUpLogCallback(cmd);
      break;
    }
    case ZCL_GET_DEBT_REPAYMENT_LOG_COMMAND_ID:
    {
      wasHandled = emberAfPrepaymentClusterGetDebtRepaymentLogCallback(cmd);
      break;
    }
    default:
    {
      // Unrecognized command ID, error status will apply.
      break;
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
