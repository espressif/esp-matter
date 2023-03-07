/***************************************************************************//**
 * @file
 * @brief CLI for the DRLC plugin.
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
#include "app/framework/plugin/drlc/demand-response-load-control.h"
#include "app/framework/plugin/drlc/load-control-event-table.h"

#ifdef UC_BUILD
// plugin drlc opt <in | out> <endpoint:1> <id:4>
void emberAfPluginDrlcOptCommand(sl_cli_command_arg_t *arguments)
{
  emAfLoadControlEventOptInOrOut(sl_cli_get_argument_uint8(arguments, 0),
                                 sl_cli_get_argument_uint32(arguments, 1),
                                 memcmp(arguments->argv[arguments->arg_ofs - 1], "in", strlen("in")) == 0);
}

// plugin drlc print <endpoint:1>
void emberAfPluginDrlcPrintCommand(sl_cli_command_arg_t *arguments)
{
  emAfLoadControlEventTablePrint(sl_cli_get_argument_uint8(arguments, 0));
}

// plugin drlc clear <endpoint:1>
void emberAfPluginDrlcClearCommand(sl_cli_command_arg_t *arguments)
{
  emAfLoadControlEventTableClear(sl_cli_get_argument_uint8(arguments, 0));

  emberAfCorePrintln("Events cleared");
}

#else

#if !defined(EMBER_AF_GENERATE_CLI)

void emberAfPluginDrlcOptCommand(void);
void emberAfPluginDrlcPrintCommand(void);
void emberAfPluginDrlcClearCommand(void);

EmberCommandEntry emberAfPluginDrlcOptCommands[] = {
  emberCommandEntryAction("in", emberAfPluginDrlcOptCommand, "uw", ""),
  emberCommandEntryAction("out", emberAfPluginDrlcOptCommand, "uw", ""),
  emberCommandEntryTerminator(),
};

EmberCommandEntry emberAfPluginDrlcCommands[] = {
  emberCommandEntryAction("opt", NULL, (const char *)emberAfPluginDrlcOptCommands, ""),
  emberCommandEntryAction("print", emberAfPluginDrlcPrintCommand, "u", ""),
  emberCommandEntryAction("clear", emberAfPluginDrlcClearCommand, "u", ""),
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI

// plugin drlc opt <in | out> <endpoint:1> <id:4>
void emberAfPluginDrlcOptCommand(void)
{
  emAfLoadControlEventOptInOrOut((uint8_t)emberUnsignedCommandArgument(0),
                                 emberUnsignedCommandArgument(1),
                                 emberCurrentCommand->name[0] == 'i');
}

// plugin drlc print <endpoint:1>
void emberAfPluginDrlcPrintCommand(void)
{
  emAfLoadControlEventTablePrint((uint8_t)emberUnsignedCommandArgument(0));
}

// plugin drlc clear <endpoint:1>
void emberAfPluginDrlcClearCommand(void)
{
  emAfLoadControlEventTableClear((uint8_t)emberUnsignedCommandArgument(0));

  emberAfCorePrintln("Events cleared");
}
#endif
