/***************************************************************************//**
 * @file
 * @brief Routines for the Prepayment Client plugin, which implements the client
 *        side of the Prepayment cluster.
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

// *****************************************************************************
// * prepayment-client.c
// *
// * Implemented routines for prepayment client.
// *
// * Copyright 2014 by Silicon Laboratories, Inc.
// *****************************************************************************

#include "app/framework/include/af.h"
#include "prepayment-client.h"

#ifdef UC_BUILD
#include "zap-cluster-command-parser.h"
#endif

void emberAfPluginPrepaymentClientChangePaymentMode(EmberNodeId nodeId, uint8_t srcEndpoint, uint8_t dstEndpoint, uint32_t providerId, uint32_t issuerEventId, uint32_t implementationDateTime, uint16_t proposedPaymentControlConfiguration, uint32_t cutOffValue)
{
  EmberStatus status;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(srcEndpoint, ZCL_PREPAYMENT_CLUSTER_ID);
  if ( ep == 0xFF ) {
    emberAfAppPrintln("==== NO PREPAYMENT CLIENT ENDPOINT");
    return;
  }

  emberAfFillCommandPrepaymentClusterChangePaymentMode(providerId, issuerEventId, implementationDateTime,
                                                       proposedPaymentControlConfiguration, cutOffValue);
  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);
  emberAfGetCommandApsFrame()->options |= EMBER_APS_OPTION_SOURCE_EUI64;
  status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, nodeId);
  emberAfAppPrintln("=====   SEND PAYMENT MODE stat=0x%x", status);
}

//-----------------------
// ZCL commands callbacks

#ifdef UC_BUILD

bool emberAfPrepaymentClusterChangePaymentModeResponseCallback(EmberAfClusterCommand *cmd)
{
  (void)cmd;

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  emberAfAppPrintln("RX: Change Payment Mode Response Callback");
  return true;
}

bool emberAfPrepaymentClusterPublishPrepaySnapshotCallback(EmberAfClusterCommand *cmd)
{
  (void)cmd;

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  emberAfAppPrintln("RX: Publish Prepay Snapshot Callback");
  return true;
}

bool emberAfPrepaymentClusterPublishTopUpLogCallback(EmberAfClusterCommand *cmd)
{
  (void)cmd;

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  emberAfAppPrintln("RX: Publish TopUp Log Callback");
  return true;
}

bool emberAfPrepaymentClusterPublishDebtLogCallback(EmberAfClusterCommand *cmd)
{
  (void)cmd;

#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
  sl_zcl_prepayment_cluster_publish_debt_log_command_t cmd_data;

  if (zcl_decode_prepayment_cluster_publish_debt_log_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfAppPrintln("RX: Publish Debt Log Callback");
  emberAfPrepaymentClusterPrintln("  commandIndex=%d", cmd_data.commandIndex);
  emberAfPrepaymentClusterPrintln("  totalNumberOfCommands=%d", cmd_data.totalNumberOfCommands);
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#else // !UC_BUILD

bool emberAfPrepaymentClusterChangePaymentModeResponseCallback(uint8_t friendlyCredit, uint32_t friendlyCreditCalendarId,
                                                               uint32_t emergencyCreditLimit, uint32_t emergencyCreditThreshold)
{
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  emberAfAppPrintln("RX: Change Payment Mode Response Callback");
  return true;
}

bool emberAfPrepaymentClusterPublishPrepaySnapshotCallback(uint32_t snapshotId, uint32_t snapshotTime, uint8_t totalSnapshotsFound,
                                                           uint8_t commandIndex, uint8_t totalNumberOfCommands,
                                                           uint32_t snapshotCause, uint8_t snapshotPayloadType, uint8_t *snapshotPayload)
{
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  emberAfAppPrintln("RX: Publish Prepay Snapshot Callback");
  return true;
}

bool emberAfPrepaymentClusterPublishTopUpLogCallback(uint8_t commandIndex, uint8_t totalNumberOfCommands, uint8_t *topUpPayload)
{
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  emberAfAppPrintln("RX: Publish TopUp Log Callback");
  return true;
}

bool emberAfPrepaymentClusterPublishDebtLogCallback(uint8_t commandIndex, uint8_t totalNumberOfCommands, uint8_t *debtPayload)
{
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  emberAfAppPrintln("RX: Publish Debt Log Callback");
  emberAfPrepaymentClusterPrintln("  commandIndex=%d", commandIndex);
  emberAfPrepaymentClusterPrintln("  totalNumberOfCommands=%d", totalNumberOfCommands);
  return true;
}

#endif // UC_BUILD

#ifdef UC_BUILD

uint32_t emberAfPrepaymentClusterClientCommandParse(sl_service_opcode_t opcode,
                                                    sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_PUBLISH_PREPAY_SNAPSHOT_COMMAND_ID:
      {
        wasHandled = emberAfPrepaymentClusterPublishPrepaySnapshotCallback(cmd);
        break;
      }
      case ZCL_CHANGE_PAYMENT_MODE_RESPONSE_COMMAND_ID:
      {
        wasHandled = emberAfPrepaymentClusterChangePaymentModeResponseCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_TOP_UP_LOG_COMMAND_ID:
      {
        wasHandled = emberAfPrepaymentClusterPublishTopUpLogCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_DEBT_LOG_COMMAND_ID:
      {
        wasHandled = emberAfPrepaymentClusterPublishDebtLogCallback(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
