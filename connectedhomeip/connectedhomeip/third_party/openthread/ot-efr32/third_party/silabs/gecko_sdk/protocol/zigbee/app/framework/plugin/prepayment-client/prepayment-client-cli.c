/***************************************************************************//**
 * @file
 * @brief Routines for interacting with the prepayment-server.
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
#include "app/framework/util/af-main.h"
#include "prepayment-client.h"

/******
   TEMPORARY FIX -
    According to Bob, we believe these types should be auto-generated and put into enums.h
    Currently that is not the case.  For now, define these here.
******/

typedef uint16_t PaymentControlConfiguration;
typedef uint32_t PrepaySnapshotPayloadCause;
typedef uint8_t  PrepaySnapshotPayloadType;
typedef uint8_t  FriendlyCredit;

void emAfPrepaymentClientCliChangePaymentMode(void);

void emAfPrepaymentClientCliChangePaymentMode()
{
  EmberNodeId nodeId;
  uint8_t srcEndpoint, dstEndpoint;
  uint32_t providerId, issuerEventId;
  uint32_t implementationDateTime;
  PaymentControlConfiguration proposedPaymentControlConfiguration;
  uint32_t cutOffValue;

  nodeId = (EmberNodeId)emberUnsignedCommandArgument(0);
  srcEndpoint = (uint8_t)emberUnsignedCommandArgument(1);
  dstEndpoint = (uint8_t)emberUnsignedCommandArgument(2);
  providerId = (uint32_t)emberUnsignedCommandArgument(3);
  issuerEventId = (uint32_t)emberUnsignedCommandArgument(4);

  implementationDateTime = (uint32_t)emberUnsignedCommandArgument(5);
  proposedPaymentControlConfiguration = (PaymentControlConfiguration)emberUnsignedCommandArgument(6);
  cutOffValue = (uint32_t)emberUnsignedCommandArgument(7);

  //emberAfAppPrintln("RX Publish Prepay Snapshot Cmd, varLen=%d", i );
  emberAfAppPrintln("Change Payment Mode, srcEp=%d, dstEp=%d, addr=%2x", srcEndpoint, dstEndpoint, nodeId);
  emberAfPluginPrepaymentClientChangePaymentMode(nodeId, srcEndpoint, dstEndpoint, providerId, issuerEventId,
                                                 implementationDateTime, proposedPaymentControlConfiguration,
                                                 cutOffValue);
}
