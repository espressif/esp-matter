/***************************************************************************//**
 * @file
 * @brief CLI for the Price Common plugin.
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
#include "price-common.h"

#ifdef UC_BUILD
#include "sl_cli.h"
//=============================================================================
// Functions

void emAfPluginPriceCommonClusterGetAdjustedStartTimeCli(sl_cli_command_arg_t *arguments)
{
  uint32_t startTimeUTc = sl_cli_get_argument_uint32(arguments, 0);
  uint8_t durationType = sl_cli_get_argument_uint8(arguments, 1);
  uint32_t adjustedStartTime;
  adjustedStartTime = emberAfPluginPriceCommonClusterGetAdjustedStartTime(startTimeUTc,
                                                                          durationType);
  UNUSED_VAR(adjustedStartTime);
  emberAfPriceClusterPrintln("adjustedStartTime: 0x%4X", adjustedStartTime);
}

void emAfPluginPriceCommonClusterConvertDurationToSecondsCli(sl_cli_command_arg_t *arguments)
{
  uint32_t startTimeUtc = sl_cli_get_argument_uint32(arguments, 0);
  uint32_t duration = sl_cli_get_argument_uint32(arguments, 1);
  uint8_t durationType = sl_cli_get_argument_uint8(arguments, 2);
  emberAfPriceClusterPrintln("seconds: %d",
                             emberAfPluginPriceCommonClusterConvertDurationToSeconds(startTimeUtc,
                                                                                     duration,
                                                                                     durationType));
}

#else
#include "app/util/serial/sl_zigbee_command_interpreter.h"
//=============================================================================
// Functions

void emAfPluginPriceCommonClusterGetAdjustedStartTimeCli(void)
{
  uint32_t startTimeUTc = (uint32_t)emberUnsignedCommandArgument(0);
  uint8_t durationType = (uint8_t)emberUnsignedCommandArgument(1);
  uint32_t adjustedStartTime;
  adjustedStartTime = emberAfPluginPriceCommonClusterGetAdjustedStartTime(startTimeUTc,
                                                                          durationType);
  UNUSED_VAR(adjustedStartTime);
  emberAfPriceClusterPrintln("adjustedStartTime: 0x%4X", adjustedStartTime);
}

void emAfPluginPriceCommonClusterConvertDurationToSecondsCli(void)
{
  uint32_t startTimeUtc = (uint32_t)emberUnsignedCommandArgument(0);
  uint32_t duration = (uint32_t)emberUnsignedCommandArgument(1);
  uint8_t durationType = (uint8_t)emberUnsignedCommandArgument(2);
  UNUSED_VAR(startTimeUtc);
  UNUSED_VAR(duration);
  UNUSED_VAR(durationType);
  emberAfPriceClusterPrintln("seconds: %d",
                             emberAfPluginPriceCommonClusterConvertDurationToSeconds(startTimeUtc,
                                                                                     duration,
                                                                                     durationType));
}

#if !defined(EMBER_AF_GENERATE_CLI)
EmberCommandEntry emberAfPluginPriceCommonCommands[] = {
  emberCommandEntryAction("adj-st-t",
                          emAfPluginPriceCommonClusterGetAdjustedStartTimeCli,
                          "wu",
                          "Calculates a new UTC start time value based on the duration type parameter."),
  emberCommandEntryAction("cnvrt-durn-to-sec",
                          emAfPluginPriceCommonClusterConvertDurationToSecondsCli,
                          "wwu",
                          "Converts the duration to a number of seconds based on the duration type parameter."),
  emberCommandEntryTerminator(),
};
#endif

#endif
