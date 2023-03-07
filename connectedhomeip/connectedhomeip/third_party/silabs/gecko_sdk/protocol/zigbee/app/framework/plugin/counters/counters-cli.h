/***************************************************************************//**
 * @file
 * @brief CLI APIs for the Counters plugin.
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

#ifndef SILABS_PLUGIN_COUNTERS_CLI_H
#define SILABS_PLUGIN_COUNTERS_CLI_H
#ifdef UC_BUILD
void emberAfPluginCountersPrintCommand(sl_cli_command_arg_t *args);
void emberAfPluginCountersSimplePrintCommand(sl_cli_command_arg_t *args);
void emberAfPluginCountersPrintThresholdsCommand(sl_cli_command_arg_t *args);
void emberAfPluginCountersSetThresholdCommand(sl_cli_command_arg_t *args);
void emberAfPluginCountersSendRequestCommand(sl_cli_command_arg_t *args);
#else
void emberAfPluginCountersPrintCommand(void);
void emberAfPluginCountersSimplePrintCommand(void);
void emberAfPluginCountersPrintThresholdsCommand(void);
void emberAfPluginCountersSetThresholdCommand(void);
void emberAfPluginCountersSendRequestCommand(void);
#endif

void emberAfPluginCounterPrintCountersResponse(EmberMessageBuffer message);

/** Args: destination id, clearCounters (bool) */
void sendCountersRequestCommand(void);

/** Utility function for printing out the OTA counters response. */
void printCountersResponse(EmberMessageBuffer message);

#endif // SILABS_PLUGIN_COUNTERS_CLI_H
