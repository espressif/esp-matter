/***************************************************************************//**
 * @file
 * @brief Routines for the Identify plugin, which implements the Identify
 *        cluster.
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

// *******************************************************************
// * identify.c
// *
// *
// * Copyright 2007 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

// this file contains all the common includes for clusters in the util
#include "app/framework/include/af.h"
#include "app/framework/util/common.h"
#ifdef UC_BUILD
#include "zap-cluster-command-parser.h"
#endif

#include "identify.h"

typedef struct {
  bool identifying;
  uint16_t identifyTime;
} EmAfIdentifyState;

static EmAfIdentifyState stateTable[EMBER_AF_IDENTIFY_CLUSTER_SERVER_ENDPOINT_COUNT];

static EmberAfStatus readIdentifyTime(uint8_t endpoint, uint16_t *identifyTime);
static EmberAfStatus writeIdentifyTime(uint8_t endpoint, uint16_t identifyTime);
static EmberStatus scheduleIdentifyTick(uint8_t endpoint);

static EmAfIdentifyState *getIdentifyState(uint8_t endpoint);

static EmAfIdentifyState *getIdentifyState(uint8_t endpoint)
{
  uint8_t ep = emberAfFindClusterServerEndpointIndex(endpoint, ZCL_IDENTIFY_CLUSTER_ID);
  return (ep == 0xFF ? NULL : &stateTable[ep]);
}

void emberAfIdentifyClusterServerInitCallback(uint8_t endpoint)
{
  scheduleIdentifyTick(endpoint);
}

void emberAfIdentifyClusterServerTickCallback(uint8_t endpoint)
{
  uint16_t identifyTime;
  if (readIdentifyTime(endpoint, &identifyTime) == EMBER_ZCL_STATUS_SUCCESS) {
    // This tick writes the new attribute, which will trigger the Attribute
    // Changed callback below, which in turn will schedule or cancel the tick.
    // Because of this, the tick does not have to be scheduled here.
    writeIdentifyTime(endpoint, (identifyTime == 0 ? 0 : identifyTime - 1));
  }
}

void emberAfIdentifyClusterServerAttributeChangedCallback(uint8_t endpoint,
                                                          EmberAfAttributeId attributeId)
{
  if (attributeId == ZCL_IDENTIFY_TIME_ATTRIBUTE_ID) {
    scheduleIdentifyTick(endpoint);
  }
}

#ifdef UC_BUILD

bool emberAfIdentifyClusterIdentifyCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_identify_cluster_identify_command_t cmd_data;
  EmberStatus sendStatus;

  if (zcl_decode_identify_cluster_identify_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS ) {
    return false;
  }

  // This Identify callback writes the new attribute, which will trigger the
  // Attribute Changed callback above, which in turn will schedule or cancel the
  // tick.  Because of this, the tick does not have to be scheduled here.
  emberAfIdentifyClusterPrintln("RX identify:IDENTIFY 0x%2x", cmd_data.identifyTime);
  sendStatus = emberAfSendImmediateDefaultResponse(
    writeIdentifyTime(emberAfCurrentEndpoint(), cmd_data.identifyTime));
  if (EMBER_SUCCESS != sendStatus) {
    emberAfIdentifyClusterPrintln("Identify: failed to send %s response: "
                                  "0x%x",
                                  "default",
                                  sendStatus);
  }
  return true;
}

bool emberAfIdentifyClusterIdentifyQueryCallback(void)
{
  EmberAfStatus status;
  EmberStatus sendStatus;
  uint16_t identifyTime;

  emberAfIdentifyClusterPrintln("RX identify:QUERY");

  // According to the 075123r02ZB, a device shall not send an Identify Query
  // Response if it is not currently identifying.  Instead, or if reading the
  // Identify Time attribute fails, send a Default Response.
  status = readIdentifyTime(emberAfCurrentEndpoint(), &identifyTime);
  if (status != EMBER_ZCL_STATUS_SUCCESS || identifyTime == 0) {
    sendStatus = emberAfSendImmediateDefaultResponse(status);
    if (EMBER_SUCCESS != sendStatus) {
      emberAfIdentifyClusterPrintln("Identify: failed to send %s response: "
                                    "0x%x",
                                    "default",
                                    sendStatus);
    }
    return true;
  }

  emberAfFillCommandIdentifyClusterIdentifyQueryResponse(identifyTime);
  sendStatus = emberAfSendResponse();
  if (EMBER_SUCCESS != sendStatus) {
    emberAfIdentifyClusterPrintln("Identify: failed to send %s response: 0x%x",
                                  "query",
                                  sendStatus);
  }
  return true;
}

#else // !UC_BUILD

bool emberAfIdentifyClusterIdentifyCallback(uint16_t identifyTime)
{
  EmberStatus sendStatus;
  // This Identify callback writes the new attribute, which will trigger the
  // Attribute Changed callback above, which in turn will schedule or cancel the
  // tick.  Because of this, the tick does not have to be scheduled here.
  emberAfIdentifyClusterPrintln("RX identify:IDENTIFY 0x%2x", identifyTime);
  sendStatus = emberAfSendImmediateDefaultResponse(
    writeIdentifyTime(emberAfCurrentEndpoint(), identifyTime));
  if (EMBER_SUCCESS != sendStatus) {
    emberAfIdentifyClusterPrintln("Identify: failed to send %s response: "
                                  "0x%x",
                                  "default",
                                  sendStatus);
  }
  return true;
}
bool emberAfIdentifyClusterIdentifyQueryCallback(void)
{
  EmberAfStatus status;
  EmberStatus sendStatus;
  uint16_t identifyTime;

  emberAfIdentifyClusterPrintln("RX identify:QUERY");

  // According to the 075123r02ZB, a device shall not send an Identify Query
  // Response if it is not currently identifying.  Instead, or if reading the
  // Identify Time attribute fails, send a Default Response.
  status = readIdentifyTime(emberAfCurrentEndpoint(), &identifyTime);
  if (status != EMBER_ZCL_STATUS_SUCCESS || identifyTime == 0) {
    sendStatus = emberAfSendImmediateDefaultResponse(status);
    if (EMBER_SUCCESS != sendStatus) {
      emberAfIdentifyClusterPrintln("Identify: failed to send %s response: "
                                    "0x%x",
                                    "default",
                                    sendStatus);
    }
    return true;
  }

  emberAfFillCommandIdentifyClusterIdentifyQueryResponse(identifyTime);
  sendStatus = emberAfSendResponse();
  if (EMBER_SUCCESS != sendStatus) {
    emberAfIdentifyClusterPrintln("Identify: failed to send %s response: 0x%x",
                                  "query",
                                  sendStatus);
  }
  return true;
}

#endif // UC_BUILD

EmberAfStatus readIdentifyTime(uint8_t endpoint,
                               uint16_t *identifyTime)
{
  EmberAfStatus status = emberAfReadAttribute(endpoint,
                                              ZCL_IDENTIFY_CLUSTER_ID,
                                              ZCL_IDENTIFY_TIME_ATTRIBUTE_ID,
                                              CLUSTER_MASK_SERVER,
                                              (uint8_t *)identifyTime,
                                              sizeof(*identifyTime),
                                              NULL); // data type
#if ((defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_IDENTIFY_CLUSTER)) || defined(UC_BUILD))
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfIdentifyClusterPrintln("ERR: reading identify time %x", status);
  }
#endif //defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_IDENTIFY_CLUSTER)
  return status;
}

static EmberAfStatus writeIdentifyTime(uint8_t endpoint, uint16_t identifyTime)
{
  EmberAfStatus status = emberAfWriteAttribute(endpoint,
                                               ZCL_IDENTIFY_CLUSTER_ID,
                                               ZCL_IDENTIFY_TIME_ATTRIBUTE_ID,
                                               CLUSTER_MASK_SERVER,
                                               (uint8_t *)&identifyTime,
                                               ZCL_INT16U_ATTRIBUTE_TYPE);
#if ((defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_IDENTIFY_CLUSTER)) || defined(UC_BUILD))
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfIdentifyClusterPrintln("ERR: writing identify time %x", status);
  }
#endif //defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_IDENTIFY_CLUSTER)
  return status;
}

static EmberStatus scheduleIdentifyTick(uint8_t endpoint)
{
  EmberAfStatus status;
  EmAfIdentifyState *state = getIdentifyState(endpoint);
  uint16_t identifyTime;

  if (state == NULL) {
    return EMBER_BAD_ARGUMENT;
  }

  status = readIdentifyTime(endpoint, &identifyTime);
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    if (!state->identifying) {
      state->identifying = true;
      state->identifyTime = identifyTime;
      emberAfPluginIdentifyStartFeedbackCallback(endpoint,
                                                 identifyTime);
    }
    if (identifyTime > 0) {
      return slxu_zigbee_zcl_schedule_server_tick(endpoint,
                                                  ZCL_IDENTIFY_CLUSTER_ID,
                                                  MILLISECOND_TICKS_PER_SECOND);
    }
  }

  state->identifying = false;
  emberAfPluginIdentifyStopFeedbackCallback(endpoint);

  return slxu_zigbee_zcl_deactivate_server_tick(endpoint, ZCL_IDENTIFY_CLUSTER_ID);
}

#ifdef UC_BUILD

uint32_t emberAfIdentifyClusterServerCommandParse(sl_service_opcode_t opcode,
                                                  sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_IDENTIFY_COMMAND_ID:
      {
        wasHandled = emberAfIdentifyClusterIdentifyCallback(cmd);
        break;
      }
      case ZCL_IDENTIFY_QUERY_COMMAND_ID:
      {
        wasHandled = emberAfIdentifyClusterIdentifyQueryCallback();
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
