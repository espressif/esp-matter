/***************************************************************************//**
 * @file
 * @brief This code provides support for managing the address table.
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
#include "address-table.h"
#include "app/util/serial/sl_zigbee_command_interpreter.h"

#ifdef UC_BUILD
#include "address-table-config.h"
#endif

#ifdef UC_BUILD
//[plugin address-table set 0 {0200000000000000} 0xA3D1
void emberAfPluginAddressTableAddCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t index;
  EmberEUI64 entry;
  sl_zigbee_copy_eui64_arg(arguments, 0, entry, false);// assuming there is left padding as before
  index = emberAfPluginAddressTableAddEntry(entry);

  if (index == EMBER_NULL_ADDRESS_TABLE_INDEX) {
    emberAfCorePrintln("Table full, entry not added");
  } else {
    emberAfCorePrintln("Entry added at position 0x%02X", index);
  }
}

void emberAfPluginAddressTableRemoveCommand(sl_cli_command_arg_t *arguments)
{
  EmberStatus status;
  EmberEUI64 entry;
  sl_zigbee_copy_eui64_arg(arguments, 0, entry, false);

  status = emberAfPluginAddressTableRemoveEntry(entry);

  if (status == EMBER_SUCCESS) {
    emberAfCorePrintln("Entry removed");
  } else {
    emberAfCorePrintln("Entry removal failed");
  }
}

void emberAfPluginAddressTableLookupCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t index;
  EmberEUI64 entry;
  sl_zigbee_copy_eui64_arg(arguments, 0, entry, false);
  index = emberAfPluginAddressTableLookupByEui64(entry);

  if (index == EMBER_NULL_ADDRESS_TABLE_INDEX) {
    emberAfCorePrintln("Entry not found");
  } else {
    emberAfCorePrintln("Found entry at position 0x%02X", index);
  }
}

// plugin address-table print
void emberAfPluginAddressTablePrintCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t i;
  uint8_t used = 0;
  sl_zigbee_core_debug_print("#  node   eui\n");
  for (i = 0; i < emberAfGetAddressTableSize(); i++) {
    EmberNodeId nodeId = emberGetAddressTableRemoteNodeId(i);
    if (nodeId != EMBER_TABLE_ENTRY_UNUSED_NODE_ID) {
      EmberEUI64 eui64;
      used++;
      sl_zigbee_core_debug_print("%d: 0x%04X ", i, nodeId);
      emberGetAddressTableRemoteEui64(i, eui64);
      emberAfAppDebugExec(emberAfPrintBigEndianEui64(eui64));
      sl_zigbee_core_debug_print("\n");
      emberAfAppFlush();
    }
  }
  sl_zigbee_core_debug_print("%d of %d entries used.\n",
                             used,
                             emberAfGetAddressTableSize());
}

void emberAfPluginAddressTableSetCommand(sl_cli_command_arg_t *arguments)
{
  EmberEUI64 eui64;
  EmberStatus status;
  uint8_t index = sl_cli_get_argument_uint8(arguments, 0);
  EmberNodeId nodeId = sl_cli_get_argument_uint16(arguments, 2);
  sl_zigbee_copy_eui64_arg(arguments, 1, eui64, true);
  status = emberAfSetAddressTableEntry(index, eui64, nodeId);
  UNUSED_VAR(status);
  sl_zigbee_core_debug_print("set address %d: 0x%x", index, status);
}

#else

void emberAfPluginAddressTableAddCommand(void);
void emberAfPluginAddressTableRemoveCommand(void);
void emberAfPluginAddressTableLookupCommand(void);

#if !defined(EMBER_AF_GENERATE_CLI)
EmberCommandEntry emberAfPluginAddressTableCommands[] = {
  emberCommandEntryAction("add", emberAfPluginAddressTableAddCommand, "b", "Add an entry to the address table."),
  emberCommandEntryAction("remove", emberAfPluginAddressTableRemoveCommand, "b", "Remove an entry from the address table."),
  emberCommandEntryAction("lookup", emberAfPluginAddressTableLookupCommand, "b", "Search for an entry in the address table."),
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI

void emberAfPluginAddressTableAddCommand(void)
{
  uint8_t index;
  EmberEUI64 entry;
  emberCopyEui64Argument(0, entry);

  index = emberAfPluginAddressTableAddEntry(entry);

  if (index == EMBER_NULL_ADDRESS_TABLE_INDEX) {
    emberAfCorePrintln("Table full, entry not added");
  } else {
    emberAfCorePrintln("Entry added at position 0x%x", index);
  }
}

void emberAfPluginAddressTableRemoveCommand(void)
{
  EmberStatus status;
  EmberEUI64 entry;
  emberCopyEui64Argument(0, entry);

  status = emberAfPluginAddressTableRemoveEntry(entry);

  if (status == EMBER_SUCCESS) {
    emberAfCorePrintln("Entry removed");
  } else {
    emberAfCorePrintln("Entry removal failed");
  }
}

void emberAfPluginAddressTableLookupCommand(void)
{
  uint8_t index;
  EmberEUI64 entry;
  emberCopyEui64Argument(0, entry);
  index = emberAfPluginAddressTableLookupByEui64(entry);

  if (index == EMBER_NULL_ADDRESS_TABLE_INDEX) {
    emberAfCorePrintln("Entry not found");
  } else {
    emberAfCorePrintln("Found entry at position 0x%x", index);
  }
}

// plugin address-table print
void emberAfPluginAddressTablePrintCommand(void)
{
  uint8_t i;
  uint8_t used = 0;
  emberAfAppPrintln("#  node   eui");
  for (i = 0; i < emberAfGetAddressTableSize(); i++) {
    EmberNodeId nodeId = emberGetAddressTableRemoteNodeId(i);
    if (nodeId != EMBER_TABLE_ENTRY_UNUSED_NODE_ID) {
      EmberEUI64 eui64;
      used++;
      emberAfAppPrint("%d: 0x%2x ", i, nodeId);
      emberGetAddressTableRemoteEui64(i, eui64);
      emberAfAppDebugExec(emberAfPrintBigEndianEui64(eui64));
      emberAfAppPrintln("");
      emberAfAppFlush();
    }
  }
  emberAfAppPrintln("%d of %d entries used.",
                    used,
                    emberAfGetAddressTableSize());
}
// plugin address-table set <index> <eui64> <node id>
void emberAfPluginAddressTableSetCommand(void)
{
  EmberEUI64 eui64;
  EmberStatus status;
  uint8_t index = (uint8_t)emberUnsignedCommandArgument(0);
  EmberNodeId nodeId = (EmberNodeId)emberUnsignedCommandArgument(2);
  emberCopyBigEndianEui64Argument(1, eui64);
  status = emberAfSetAddressTableEntry(index, eui64, nodeId);
  UNUSED_VAR(status);
  emberAfAppPrintln("set address %d: 0x%x", index, status);
}
#endif //!UC_BUILD
