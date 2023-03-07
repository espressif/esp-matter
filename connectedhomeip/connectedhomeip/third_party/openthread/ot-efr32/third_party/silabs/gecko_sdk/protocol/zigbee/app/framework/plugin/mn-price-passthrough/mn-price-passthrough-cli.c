/***************************************************************************//**
 * @file
 * @brief CLI for the MN Price Passthrough plugin.
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

#include "app/framework/plugin/mn-price-passthrough/mn-price-passthrough.h"
#include "app/framework/plugin/price-common/price-common.h"
#ifdef UC_CLI
#include "app/util/serial/sl_zigbee_command_interpreter.h"
// plugin mn-price-passthrough start
void emAfPluginMnPriceStartCommand(sl_cli_command_arg_t *arguments)
{
  emAfPluginMnPricePassthroughStartPollAndForward();
}

// plugin mn-price-passthrough start
void emAfPluginMnPriceStopCommand(sl_cli_command_arg_t *arguments)
{
  emAfPluginMnPricePassthroughStopPollAndForward();
}

// plugin mn-price-passthrough setRouting <forwardingId:2> <forwardingEndpoint:1> <proxyEsiEndpoint:1>
void emAfPluginMnPriceSetRoutingCommand(sl_cli_command_arg_t *arguments)
{
  EmberNodeId fwdId = (EmberNodeId)sl_cli_get_argument_uint16(arguments, 0);
  uint8_t fwdEndpoint = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t esiEndpoint = sl_cli_get_argument_uint8(arguments, 2);
  emAfPluginMnPricePassthroughRoutingSetup(fwdId,
                                           fwdEndpoint,
                                           esiEndpoint);
}

// plugin mn-price-passthrough print
void emAfPluginMnPricePrintCommand(sl_cli_command_arg_t *arguments)
{
  emAfPluginMnPricePassthroughPrintCurrentPrice();
}

#else
#include "app/util/serial/sl_zigbee_command_interpreter.h"
void start(void);
void stop(void);
void setRouting(void);
void print(void);

#ifndef EMBER_AF_GENERATE_CLI
EmberCommandEntry emberAfPluginMnPricePassthroughCommands[] = {
  emberCommandEntryAction("start", start, "", ""),
  emberCommandEntryAction("stop", stop, "", ""),
  emberCommandEntryAction("set-routing", setRouting, "vuu", ""),
  emberCommandEntryAction("print", print, "", ""),
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI

// plugin mn-price-passthrough start
void start(void)
{
  emAfPluginMnPricePassthroughStartPollAndForward();
}

// plugin mn-price-passthrough start
void stop(void)
{
  emAfPluginMnPricePassthroughStopPollAndForward();
}

// plugin mn-price-passthrough setRouting <forwardingId:2> <forwardingEndpoint:1> <proxyEsiEndpoint:1>
void setRouting(void)
{
  EmberNodeId fwdId = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t fwdEndpoint = (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t esiEndpoint = (uint8_t)emberUnsignedCommandArgument(2);
  emAfPluginMnPricePassthroughRoutingSetup(fwdId,
                                           fwdEndpoint,
                                           esiEndpoint);
}

// plugin mn-price-passthrough print
void print(void)
{
  emAfPluginMnPricePassthroughPrintCurrentPrice();
}
#endif
