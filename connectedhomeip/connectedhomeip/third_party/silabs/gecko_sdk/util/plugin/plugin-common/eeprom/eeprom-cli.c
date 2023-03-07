/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#ifdef UC_BUILD
#include PLATFORM_HEADER
#include "hal.h"
#include "sl_cli.h"
#include "zcl-debug-print.h"
#include "eeprom-config.h"
#else // !UC_BUILD
#ifdef EMBER_AF_API_AF_HEADER // AFV2
  #include EMBER_AF_API_AF_HEADER
  #ifdef EMBER_AF_LEGACY_CLI
    #error The EEPROM plugin is not compatible with the legacy CLI.
  #endif
#else                         // AFV6
  #include PLATFORM_HEADER
  #include CONFIGURATION_HEADER
  #include EMBER_AF_API_HAL
  #include EMBER_AF_API_COMMAND_INTERPRETER2
  #ifdef EMBER_AF_API_DEBUG_PRINT
    #include EMBER_AF_API_DEBUG_PRINT
  #endif
  #include EMBER_AF_API_EEPROM_PRINT
#endif
#endif // UC_BUILD

#include "eeprom.h"

//------------------------------------------------------------------------------
// Macros

#define DATA_BLOCK 64

//------------------------------------------------------------------------------

#ifdef UC_BUILD

void emAfEepromDataPrintCommand(sl_cli_command_arg_t *arguments)
{
  uint32_t offset = sl_cli_get_argument_uint32(arguments, 0);
  uint8_t data[DATA_BLOCK];
  uint8_t status = emberAfPluginEepromRead(offset, data, DATA_BLOCK);

  if (status) {
    emberAfCorePrintln("Error:  Failed to read from EEPROM at 0x%4X, status: %d",
                       offset,
                       status);
    return;
  }

  // Divide by 8 to get the number of 8-byte blocks to print.
  emberAfPrint8ByteBlocks(DATA_BLOCK >> 3, // number of blocks
                          data,
                          true); // CR between blocks?
}

void emAfEepromStatusCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t i;
  emberAfCorePrintln("EEPROM Initialized: %c",
                     (emAfIsEepromInitialized()
                      ? 'y'
                      : 'n'));
  // NOTE:  Calling emberAfPluginEepromBusy() will actually initialize the
  // EEPROM, so we avoid triggering a side-effect during this 'status' command.
  emberAfCorePrintln("EEPROM Busy: %c",
                     (!emAfIsEepromInitialized()
                      ? '?'
                      : (emberAfPluginEepromBusy()
                         ? 'y'
                         : 'n')));
  emberAfCorePrintln("Partial Word Storage Count: %d",
                     EMBER_AF_PLUGIN_EEPROM_PARTIAL_WORD_STORAGE_COUNT);

  for (i = 0; i < EMBER_AF_PLUGIN_EEPROM_PARTIAL_WORD_STORAGE_COUNT; i++) {
    emberAfCorePrintln("Address: 0x%4X, Partial Word: 0x%X",
                       emAfEepromSavedPartialWrites[i].address,
                       emAfEepromSavedPartialWrites[i].data);
  }
}

void emAfEepromInfoCommand(sl_cli_command_arg_t *arguments)
{
  const HalEepromInformationType* part = emberAfPluginEepromInfo();
  emberAfCorePrintln("\nEEPROM Info");
  if (part == NULL) {
    emberAfCorePrintln("Not available (older bootloader)");
  } else {
    uint8_t wordSize = emberAfPluginEepromGetWordSize();
    bool confirmedWordSize = false;
    if (part->version >= EEPROM_INFO_MIN_VERSION_WITH_WORD_SIZE_SUPPORT) {
      confirmedWordSize = true;
    }
    emberAfCorePrintln("Part Description:          %p", part->partDescription);
    emberAfCorePrintln("Capabilities:              0x%2X", part->capabilitiesMask);
    emberAfCorePrintln("Page Erase time (s):      %d", part->pageEraseMs);
    if ((part->capabilitiesMask & EEPROM_CAPABILITIES_PART_ERASE_SECONDS)
        == EEPROM_CAPABILITIES_PART_ERASE_SECONDS) {
      emberAfCorePrintln("Part Erase time (s):      %d", part->partEraseTime);
    } else {
      emberAfCorePrintln("Part Erase time (ms):      %d", part->partEraseTime);
    }
    emberAfCorePrintln("Page size (bytes):         %l", part->pageSize);
    emberAfCorePrintln("Part size (bytes):         %l", part->partSize);
    emberAfCorePrintln("Word size (bytes):         %d (%p)",
                       wordSize,
                       (confirmedWordSize
                        ? "confirmed"
                        : "assumed"));
  }
}

#else // !UC_BUILD

void emAfEepromDataPrintCommand(void)
{
  uint32_t offset = emberUnsignedCommandArgument(0);
  uint8_t data[DATA_BLOCK];
  uint8_t status = emberAfPluginEepromRead(offset, data, DATA_BLOCK);

  if (status) {
    emberAfCorePrintln("Error:  Failed to read from EEPROM at 0x%4X, status: %d",
                       offset,
                       status);
    return;
  }

  // Divide by 8 to get the number of 8-byte blocks to print.
  emberAfPrint8ByteBlocks(DATA_BLOCK >> 3, // number of blocks
                          data,
                          true); // CR between blocks?
}

void emAfEepromStatusCommand(void)
{
  uint8_t i;
  emberAfCorePrintln("EEPROM Initialized: %c",
                     (emAfIsEepromInitialized()
                      ? 'y'
                      : 'n'));
  // NOTE:  Calling emberAfPluginEepromBusy() will actually initialize the
  // EEPROM, so we avoid triggering a side-effect during this 'status' command.
  emberAfCorePrintln("EEPROM Busy: %c",
                     (!emAfIsEepromInitialized()
                      ? '?'
                      : (emberAfPluginEepromBusy()
                         ? 'y'
                         : 'n')));
  emberAfCorePrintln("Partial Word Storage Count: %d",
                     EMBER_AF_PLUGIN_EEPROM_PARTIAL_WORD_STORAGE_COUNT);

  for (i = 0; i < EMBER_AF_PLUGIN_EEPROM_PARTIAL_WORD_STORAGE_COUNT; i++) {
    emberAfCorePrintln("Address: 0x%4X, Partial Word: 0x%X",
                       emAfEepromSavedPartialWrites[i].address,
                       emAfEepromSavedPartialWrites[i].data);
  }
}

void emAfEepromInfoCommand(void)
{
  const HalEepromInformationType* part = emberAfPluginEepromInfo();
  emberAfCorePrintln("\nEEPROM Info");
  if (part == NULL) {
    emberAfCorePrintln("Not available (older bootloader)");
  } else {
    uint8_t wordSize = emberAfPluginEepromGetWordSize();
    bool confirmedWordSize = false;
    if (part->version >= EEPROM_INFO_MIN_VERSION_WITH_WORD_SIZE_SUPPORT) {
      confirmedWordSize = true;
    }
    emberAfCorePrintln("Part Description:          %p", part->partDescription);
    emberAfCorePrintln("Capabilities:              0x%2X", part->capabilitiesMask);
    emberAfCorePrintln("Page Erase time (s):      %d", part->pageEraseMs);
    if ((part->capabilitiesMask & EEPROM_CAPABILITIES_PART_ERASE_SECONDS)
        == EEPROM_CAPABILITIES_PART_ERASE_SECONDS) {
      emberAfCorePrintln("Part Erase time (s):      %d", part->partEraseTime);
    } else {
      emberAfCorePrintln("Part Erase time (ms):      %d", part->partEraseTime);
    }
    emberAfCorePrintln("Page size (bytes):         %l", part->pageSize);
    emberAfCorePrintln("Part size (bytes):         %l", part->partSize);
    emberAfCorePrintln("Word size (bytes):         %d (%p)",
                       wordSize,
                       (confirmedWordSize
                        ? "confirmed"
                        : "assumed"));
  }
}

#endif // UC_BUILD
