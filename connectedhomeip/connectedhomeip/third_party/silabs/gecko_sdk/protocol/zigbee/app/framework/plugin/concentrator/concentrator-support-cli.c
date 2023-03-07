/***************************************************************************//**
 * @file
 * @brief CLI interface to manage the concentrator's periodic MTORR broadcasts.
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
#include "app/framework/plugin/concentrator/concentrator-support.h"
#include "stack/include/source-route.h"
#include "app/util/serial/sl_zigbee_command_interpreter.h"
#ifdef UC_BUILD
#include "concentrator-config.h"
#endif

#define SOURCE_ROUTE_NULL_INDEX 0xFF
#ifdef UC_BUILD

void emberAfPluginConcentratorPrintSourceRouteTable(sl_cli_command_arg_t *arguments)
{
  uint8_t i;
  for (i = 0; i < emberAfGetSourceRouteTableFilledSize(); i++) {
    uint8_t index = i;
    uint8_t closerIndex;
    EmberNodeId destination;
    EmberStatus status = emberAfGetSourceRouteTableEntry(index,
                                                         &destination,
                                                         &closerIndex);
    if (status != EMBER_SUCCESS) {
      emberAfAppPrintln("0 of 0 total entries.");
      return;
    }

    emberAfAppPrint("%d: 0x%2X -> ",
                    index, destination);

    while (closerIndex != SOURCE_ROUTE_NULL_INDEX) {
      index = closerIndex;
      status = emberAfGetSourceRouteTableEntry(index,
                                               &destination,
                                               &closerIndex);
      if (status == EMBER_SUCCESS) {
        emberAfAppPrint("0x%2X -> ", destination);
      } else {
        emberAfAppPrint("NULL ");
        return;
      }
    }
    emberAfAppPrintln("0x%2X (Me)", emberAfGetNodeId());
  }
  emberAfAppPrintln("%d of %d total entries.",
                    emberAfGetSourceRouteTableFilledSize(),
                    emberAfGetSourceRouteTableTotalSize());
}

void emberAfPluginConcentratorStatus(sl_cli_command_arg_t *arguments)
{
  emberAfAppPrintln("Type:  %p RAM",
                    ((EMBER_AF_PLUGIN_CONCENTRATOR_CONCENTRATOR_TYPE
                      == EMBER_LOW_RAM_CONCENTRATOR)
                     ? "Low"
                     : "High"));

  emberAfAppPrintln("Min Time Between Broadcasts (sec): %d",
                    EMBER_AF_PLUGIN_CONCENTRATOR_MIN_TIME_BETWEEN_BROADCASTS_SECONDS);
  emberAfAppPrintln("Max Time Between Broadcasts (sec): %d",
                    EMBER_AF_PLUGIN_CONCENTRATOR_MAX_TIME_BETWEEN_BROADCASTS_SECONDS);
  emberAfAppPrintln("Max Hops: %d",
                    (EMBER_AF_PLUGIN_CONCENTRATOR_MAX_HOPS == 0
                     ? EMBER_MAX_HOPS
                     : EMBER_AF_PLUGIN_CONCENTRATOR_MAX_HOPS));
  emberAfAppPrintln("Route Error Threshold:      %d",
                    EMBER_AF_PLUGIN_CONCENTRATOR_ROUTE_ERROR_THRESHOLD);
  emberAfAppPrintln("Delivery Failure Threshold: %d",
                    EMBER_AF_PLUGIN_CONCENTRATOR_DELIVERY_FAILURE_THRESHOLD);
}

void emberAfPluginConcentratorStartDiscoveryCommand(sl_cli_command_arg_t *arguments)
{
  uint32_t qsLeft =  emberSetSourceRouteDiscoveryMode(EMBER_SOURCE_ROUTE_DISCOVERY_ON);
  emberAfAppPrintln("%d sec until next MTORR broadcast", (qsLeft >> 2));
}

void emberAfPluginConcentratorStopDiscoveryCommand(sl_cli_command_arg_t *arguments)
{
  emberSetSourceRouteDiscoveryMode(EMBER_SOURCE_ROUTE_DISCOVERY_OFF);
}

void emberAfPluginConcentratorAggregationCommand(sl_cli_command_arg_t *arguments)
{
  emberSetSourceRouteDiscoveryMode(EMBER_SOURCE_ROUTE_DISCOVERY_RESCHEDULE);
}

void emberAfPluginConcentratorSetRouterBehaviorCommand(sl_cli_command_arg_t *arguments)
{
  EmberAfPluginConcentratorRouterBehavior behavior
    = (EmberAfPluginConcentratorRouterBehavior)sl_cli_get_argument_uint8(arguments, 0);

  if (behavior > EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_MAX) {
    emberAfCorePrintln("Illegal concentrator router behavior: 0x%X", behavior);
  } else {
    emberAfCorePrintln("Illegal to change concentrator router behavior at run time."); // EZSP support required, if still needed
  }
}

#else
// *****************************************************************************
// Forward Declarations

void emberAfPluginConcentratorStatus(void);
void emberAfPluginConcentratorStartDiscovery(void);
void emberAfPluginConcentratorAggregationCommand(void);
void emberAfPluginConcentratorSetRouterBehaviorCommand(void);

void emberAfPluginConcentratorPrintSourceRouteTable(void);
void emberAfGetSourceRoute(void);
// *****************************************************************************
// Globals

#if !defined(EMBER_AF_GENERATE_CLI)
EmberCommandEntry emberAfPluginConcentratorCommands[] = {
  emberCommandEntryAction("status", emberAfPluginConcentratorStatus, "",
                          "Prints current status and configured parameters of the concentrator"),
  emberCommandEntryAction("start", emberAfPluginConcentratorStartDiscovery, "",
                          "Starts the periodic broadcast of MTORRs"),
  emberCommandEntryAction("stop", emberAfPluginConcentratorStopDiscovery, "",
                          "Stops the periodic broadcast of MTORRs"),
  emberCommandEntryAction("agg", emberAfPluginConcentratorAggregationCommand, "",
                          "Sends an MTORR broadcast now."),
  emberCommandEntryAction("print-table",
                          emberAfPluginConcentratorPrintSourceRouteTable,
                          "",
                          "Prints the source route table."),
  emberCommandEntryAction("set-router-behavior",
                          emberAfPluginConcentratorSetRouterBehaviorCommand,
                          "",
                          "This command allows the user to set the router behavior for this plugin."),
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI

// *****************************************************************************
// Functions

void emberAfPluginConcentratorPrintSourceRouteTable(void)
{
  uint8_t i;
  for (i = 0; i < emberAfGetSourceRouteTableFilledSize(); i++) {
    uint8_t index = i;
    uint8_t closerIndex;
    EmberNodeId destination;
    EmberStatus status = emberAfGetSourceRouteTableEntry(index,
                                                         &destination,
                                                         &closerIndex);
    if (status != EMBER_SUCCESS) {
      emberAfAppPrintln("0 of 0 total entries.");
      return;
    }

    emberAfAppPrint("%d: 0x%2X -> ",
                    index, destination);

    while (closerIndex != SOURCE_ROUTE_NULL_INDEX) {
      index = closerIndex;
      status = emberAfGetSourceRouteTableEntry(index,
                                               &destination,
                                               &closerIndex);
      if (status == EMBER_SUCCESS) {
        emberAfAppPrint("0x%2X -> ", destination);
      } else {
        emberAfAppPrint("NULL ");
        return;
      }
    }
    emberAfAppPrintln("0x%2X (Me)", emberAfGetNodeId());
  }
  emberAfAppPrintln("%d of %d total entries.",
                    emberAfGetSourceRouteTableFilledSize(),
                    emberAfGetSourceRouteTableTotalSize());
}

void emberAfPluginConcentratorStatus(void)
{
  emberAfAppPrintln("Type:  %p RAM",
                    ((EMBER_AF_PLUGIN_CONCENTRATOR_CONCENTRATOR_TYPE
                      == EMBER_LOW_RAM_CONCENTRATOR)
                     ? "Low"
                     : "High"));

  emberAfAppPrintln("Min Time Between Broadcasts (sec): %d",
                    EMBER_AF_PLUGIN_CONCENTRATOR_MIN_TIME_BETWEEN_BROADCASTS_SECONDS);
  emberAfAppPrintln("Max Time Between Broadcasts (sec): %d",
                    EMBER_AF_PLUGIN_CONCENTRATOR_MAX_TIME_BETWEEN_BROADCASTS_SECONDS);
  emberAfAppPrintln("Max Hops: %d",
                    (EMBER_AF_PLUGIN_CONCENTRATOR_MAX_HOPS == 0
                     ? EMBER_MAX_HOPS
                     : EMBER_AF_PLUGIN_CONCENTRATOR_MAX_HOPS));
  emberAfAppPrintln("Route Error Threshold:      %d",
                    EMBER_AF_PLUGIN_CONCENTRATOR_ROUTE_ERROR_THRESHOLD);
  emberAfAppPrintln("Delivery Failure Threshold: %d",
                    EMBER_AF_PLUGIN_CONCENTRATOR_DELIVERY_FAILURE_THRESHOLD);
}

void emberAfPluginConcentratorAggregationCommand(void)
{
  emberSetSourceRouteDiscoveryMode(EMBER_SOURCE_ROUTE_DISCOVERY_RESCHEDULE);
}

void emberAfPluginConcentratorSetRouterBehaviorCommand(void)
{
  EmberAfPluginConcentratorRouterBehavior behavior
    = (EmberAfPluginConcentratorRouterBehavior)emberUnsignedCommandArgument(0);

  if (behavior > EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_MAX) {
    emberAfCorePrintln("Illegal concentrator router behavior: 0x%X", behavior);
  } else {
    emberAfCorePrintln("Illegal to change concentrator router behavior at run time."); // EZSP support required, if still needed
  }
}
#endif

void emberAfPluginConcentratorStartDiscovery(void)
{
  uint32_t qsLeft =  emberSetSourceRouteDiscoveryMode(EMBER_SOURCE_ROUTE_DISCOVERY_ON);
  emberAfAppPrintln("%d sec until next MTORR broadcast", (qsLeft >> 2));
}

void emberAfPluginConcentratorStopDiscovery(void)
{
  emberSetSourceRouteDiscoveryMode(EMBER_SOURCE_ROUTE_DISCOVERY_OFF);
}
