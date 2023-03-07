/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#ifndef __SLOT_MANAGER_CLI_H__
#define __SLOT_MANAGER_CLI_H__

#include "sl_cli.h"

#define MAX_FLASH_READ_BYTES    100

void emAfPluginSlotManagerCliBootloadSlot(sl_cli_command_arg_t *arguments);
void emAfPluginSlotManagerCliEraseSlot(sl_cli_command_arg_t *arguments);
void emAfPluginSlotManagerCliPrintExternalFlashInfo(sl_cli_command_arg_t *arguments);
void emAfPluginSlotManagerCliReadExtFlash(sl_cli_command_arg_t *arguments);
void emAfPluginSlotManagerCliPrintSlotsInfo(sl_cli_command_arg_t *arguments);

#endif // __SLOT_MANAGER_CLI_H__
