/***************************************************************************//**
 * @file
 * @brief Routines for the Poll Control Client plugin, which implement the
 *        client side of the Poll Control cluster. The Poll Control cluster
 *        provides a means to communicate with an end device with a sleep
 *        schedule.
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
#include "poll-control-client.h"

#ifdef UC_BUILD
#include "poll-control-client-config.h"
#include "zap-cluster-command-parser.h"
#endif

static bool fastPolling = false;
static bool respondToCheckIn = true;
static uint16_t fastPollingTimeout = EMBER_AF_PLUGIN_POLL_CONTROL_CLIENT_DEFAULT_FAST_POLL_TIMEOUT;

void emAfSetFastPollingMode(bool mode)
{
  fastPolling = mode;
}

void emAfSetFastPollingTimeout(uint16_t timeout)
{
  fastPollingTimeout = timeout;
}

void emAfSetResponseMode(bool mode)
{
  respondToCheckIn = mode;
}

bool emberAfPollControlClusterCheckInCallback(void)
{
  emberAfPollControlClusterPrintln("RX: CheckIn");
  if (respondToCheckIn) {
    emberAfFillCommandPollControlClusterCheckInResponse(fastPolling,
                                                        fastPollingTimeout);
    emberAfSendResponse();
  }
  return true;
}

void emAfPollControlClientPrint(void)
{
  emberAfPollControlClusterPrintln("Poll Control Client:\n%p %p\n%p 0x%2x",
                                   "fast polling: ",
                                   fastPolling ? "on" : "off",
                                   "fast polling timeout: ",
                                   fastPollingTimeout);
}

#ifdef UC_BUILD

uint32_t emberAfPollControlClusterClientCommandParse(sl_service_opcode_t opcode,
                                                     sl_service_function_context_t *context)
{
  (void)opcode;
  bool wasHandled = false;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  if (!cmd->mfgSpecific && cmd->commandId == ZCL_CHECK_IN_COMMAND_ID) {
    wasHandled = emberAfPollControlClusterCheckInCallback();
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
