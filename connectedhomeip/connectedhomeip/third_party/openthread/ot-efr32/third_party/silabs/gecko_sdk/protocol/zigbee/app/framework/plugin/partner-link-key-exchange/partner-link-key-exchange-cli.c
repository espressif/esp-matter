/***************************************************************************//**
 * @file
 * @brief CLI for the Partner Link Key Exchange plugin.
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
#include "app/framework/plugin/partner-link-key-exchange/partner-link-key-exchange.h"

#if defined(EZSP_HOST)
extern EzspStatus emberAfSetEzspPolicy(EzspPolicyId policyId,
                                       EzspDecisionId decisionId,
                                       const char * policyName,
                                       const char * decisionName);
#endif // EZSP_HOST

#ifdef UC_BUILD
#include "sl_cli.h"

void cbkeAllowPartner(sl_cli_command_arg_t *arguments)
{
  emAfAllowPartnerLinkKey = sl_cli_get_argument_uint8(arguments, 0);

#if defined(EZSP_HOST)
  emberAfSetEzspPolicy(EZSP_APP_KEY_REQUEST_POLICY,
                       (emAfAllowPartnerLinkKey
                        ? EZSP_ALLOW_APP_KEY_REQUESTS
                        : EZSP_DENY_APP_KEY_REQUESTS),
                       "App Link Key Request Policy",
                       (emAfAllowPartnerLinkKey
                        ? "Allow"
                        : "Deny"));

#endif
}

void cbkePartnerCommand(sl_cli_command_arg_t *arguments)
{
  EmberNodeId target = (EmberNodeId)sl_cli_get_argument_uint16(arguments, 0);
  uint8_t endpoint     = sl_cli_get_argument_uint8(arguments, 1);
  EmberStatus status = emberAfInitiatePartnerLinkKeyExchange(target,
                                                             endpoint,
                                                             NULL); // callback
  emberAfCoreDebugExec(emAfPrintStatus("partner link key request", status));
}

#else //UC_BUILD

#include "app/util/serial/sl_zigbee_command_interpreter.h"

#if !defined(EMBER_AF_GENERATE_CLI)
void cbkeAllowPartner(void);
void cbkePartnerCommand(void);

EmberCommandEntry emberAfPluginPartnerLinkKeyExchangeCommands[] = {
  emberCommandEntryAction("partner", cbkePartnerCommand, "vu", ""),
  emberCommandEntryAction("allow-partner", cbkeAllowPartner, "u", ""),
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI

void cbkeAllowPartner(void)
{
  emAfAllowPartnerLinkKey = (bool)emberUnsignedCommandArgument(0);

#if defined(EZSP_HOST)
  emberAfSetEzspPolicy(EZSP_APP_KEY_REQUEST_POLICY,
                       (emAfAllowPartnerLinkKey
                        ? EZSP_ALLOW_APP_KEY_REQUESTS
                        : EZSP_DENY_APP_KEY_REQUESTS),
                       "App Link Key Request Policy",
                       (emAfAllowPartnerLinkKey
                        ? "Allow"
                        : "Deny"));

#endif
}

void cbkePartnerCommand(void)
{
  EmberNodeId target = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t endpoint     = (uint8_t)emberUnsignedCommandArgument(1);
  EmberStatus status = emberAfInitiatePartnerLinkKeyExchange(target,
                                                             endpoint,
                                                             NULL); // callback
  emberAfCoreDebugExec(emAfPrintStatus("partner link key request", status));
}

#endif
