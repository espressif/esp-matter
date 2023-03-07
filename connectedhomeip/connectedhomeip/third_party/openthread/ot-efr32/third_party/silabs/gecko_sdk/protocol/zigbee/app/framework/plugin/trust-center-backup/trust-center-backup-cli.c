/***************************************************************************//**
 * @file
 * @brief User interface for backing up and restoring a trust center.
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
#include "app/framework/util/common.h"
#include "app/framework/util/util.h"
#include "app/util/serial/sl_zigbee_command_interpreter.h"
#include "app/framework/plugin/trust-center-backup/trust-center-backup.h"

#include "app/framework/util/af-main.h"

#if defined(EMBER_TEST)
  #include "app/framework/test/test-framework-security.h"
#endif

#ifndef UC_BUILD
// *****************************************************************************
// Forward Declarations

void printExportDataCommand(void);
void printImportDataCommand(void);
void setExtendedPanIdCommand(void);
void restoreFromBackupCommand(void);
void importKeyCommand(void);
void importClearCommand(void);

// *****************************************************************************
// Globals

#ifndef EMBER_AF_GENERATE_CLI
EmberCommandEntry emberAfPluginTrustCenterBackupCommands[] = {
#if defined(EMBER_AF_PRINT_SECURITY)
  emberCommandEntryAction("print-export", printExportDataCommand, "",
                          "Print the set of export data that a TC must backup"),
  emberCommandEntryAction("import-key", importKeyCommand, "ubb",
                          "Set a key in the import data set"),
  emberCommandEntryAction("set-ext-pan", setExtendedPanIdCommand, "b",
                          "Set the extended PAN ID in the import data set."),
  emberCommandEntryAction("print-import", printImportDataCommand, "",
                          "Print the import data set"),
  emberCommandEntryAction("clear-import", importClearCommand, "",
                          "Clear the import data set"),
  emberCommandEntryAction("restore", restoreFromBackupCommand, "",
                          "Use the import data set in a restore operation to bring the TC back online"),
#if defined(POSIX_FILE_BACKUP_SUPPORT)
  emberCommandEntryAction("file-export", emAfTcExportCommand, "b",
                          "Write the TC backup data to a file"),
  emberCommandEntryAction("file-import", emAfTcImportCommand, "b",
                          "Read the TC Backup data from a file and bring the TC back online"),
#endif

#endif
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI
#endif //UC_BUILD

#if defined(EMBER_SCRIPTED_TEST)
  #define EMBER_LINK_KEY_TABLE_SIZE                               TEST_FRAMEWORK_KEY_TABLE_SIZE
  #define EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_MAX_CLI_BACKUP_SIZE TEST_FRAMEWORK_KEY_TABLE_SIZE
#endif

#if EMBER_LINK_KEY_TABLE_SIZE > EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_MAX_CLI_BACKUP_SIZE
  #error EMBER_LINK_KEY_TABLE_SIZE > EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_MAX_CLI_BACKUP_SIZE
#endif

// A shorter more manageable name.
#define MAX_CLI_SIZE EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_MAX_CLI_BACKUP_SIZE

static EmberAfLinkKeyBackupData importKeyList[MAX_CLI_SIZE];
static EmberAfTrustCenterBackupData importData = {
  { 0, 0, 0, 0, 0, 0, 0, 0 }, // extended PAN ID (filled in later)
  0,                          // key list length (will be filled in later)
  MAX_CLI_SIZE,
  importKeyList
};

// *****************************************************************************
// Functions

void printBackup(const EmberAfTrustCenterBackupData* backup)
{
  uint8_t i = 0;

  emberAfSecurityPrint("Extended PAN ID: ");
  emberAfPrintIeeeLine(backup->extendedPanId);

  emberAfSecurityPrintln("Index  EUI64                Hashed Key");
  //123456 (>)0123456789ABCDEF

  for (i = 0; i < backup->keyListLength; i++) {
    emberAfSecurityFlush();
    emberAfSecurityPrint("%d      ", i);
    emberAfPrintBigEndianEui64(backup->keyList[i].deviceId);
    emberAfSecurityPrint("  ");
    emberAfPrintZigbeeKey(emberKeyContents(&(backup->keyList[i].key)));
  }
  emberAfSecurityFlush();
  emberAfSecurityPrintln("\n%d keys in backup", i);
}
#ifdef UC_BUILD

void printExportDataCommand(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  EmberAfLinkKeyBackupData exportKeyList[MAX_CLI_SIZE];
  EmberAfTrustCenterBackupData export;
  EmberStatus status;

  export.maxKeyListLength = MAX_CLI_SIZE;
  export.keyList = exportKeyList;
  status = emberTrustCenterExportBackupData(&export);

  if (status != EMBER_SUCCESS) {
    emberAfSecurityPrintln("%p: Failed to get TC backup data.", "Error");
    return;
  }

  printBackup(&export);
}

void printImportDataCommand(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  printBackup(&importData);
}

void importClearCommand(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  MEMSET(&importData, 0, sizeof(EmberAfTrustCenterBackupData));
  MEMSET(importKeyList,
         0,
         sizeof(EmberAfLinkKeyBackupData) * MAX_CLI_SIZE);
  importData.keyList = importKeyList;
  importData.maxKeyListLength = MAX_CLI_SIZE;
}

void importKeyCommand(sl_cli_command_arg_t *arguments)
{
  EmberEUI64 partnerEUI64;
  EmberKeyData newKey;
  uint8_t index = (uint8_t)sl_cli_get_argument_uint8(arguments, 0);
  sl_zigbee_copy_eui64_arg(arguments, 1, partnerEUI64, true);
  sl_zigbee_copy_key_arg((arguments), 2, &newKey);

  if (0 == MEMCOMPARE(emberAfNullEui64, partnerEUI64, EUI64_SIZE)) {
    emberAfSecurityPrintln("%p: EUI64 cannot be all zeroes.",
                           "Error");
    return;
  }

  if (index > MAX_CLI_SIZE) {
    emberAfSecurityPrintln("%p: index %d is greater than max import limit of %d.",
                           "Error",
                           index,
                           MAX_CLI_SIZE);
    return;
  }

  MEMMOVE(importKeyList[index].deviceId, partnerEUI64, EUI64_SIZE);
  MEMMOVE(emberKeyContents(&(importKeyList[index].key)),
          emberKeyContents(&newKey),
          EMBER_ENCRYPTION_KEY_SIZE);
  if (importData.keyListLength <= index) {
    importData.keyListLength = index + 1;
  }
}

void setExtendedPanIdCommand(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_copy_eui64_arg(arguments, 0, importData.extendedPanId, true);
}

void restoreFromBackupCommand(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  EmberStatus status;

  if (0 == MEMCOMPARE(importData.extendedPanId,
                      emberAfNullEui64,
                      EUI64_SIZE)) {
    emberAfSecurityPrintln("%p: Import contains NULL extended PAN ID",
                           "Error");
    return;
  }

  status = emberTrustCenterImportBackupAndStartNetwork(&importData);
  if (status != EMBER_SUCCESS) {
    emberAfSecurityPrintln("%p: Failed to import backup data and form network.",
                           "Error");
    return;
  }
  emberAfSecurityPrintln("Import successful.");
}

#else
void printExportDataCommand(void)
{
  EmberAfLinkKeyBackupData exportKeyList[MAX_CLI_SIZE];
  EmberAfTrustCenterBackupData export;
  EmberStatus status;

  export.maxKeyListLength = MAX_CLI_SIZE;
  export.keyList = exportKeyList;
  status = emberTrustCenterExportBackupData(&export);

  if (status != EMBER_SUCCESS) {
    emberAfSecurityPrintln("%p: Failed to get TC backup data.", "Error");
    return;
  }

  printBackup(&export);
}

void printImportDataCommand(void)
{
  printBackup(&importData);
}

void importClearCommand(void)
{
  MEMSET(&importData, 0, sizeof(EmberAfTrustCenterBackupData));
  MEMSET(importKeyList,
         0,
         sizeof(EmberAfLinkKeyBackupData) * MAX_CLI_SIZE);
  importData.keyList = importKeyList;
  importData.maxKeyListLength = MAX_CLI_SIZE;
}

void importKeyCommand(void)
{
  EmberEUI64 partnerEUI64;
  EmberKeyData newKey;
  uint8_t index = (uint8_t)emberUnsignedCommandArgument(0);
  emberCopyBigEndianEui64Argument(1, partnerEUI64);
  emberCopyKeyArgument(2, &newKey);

  if (0 == MEMCOMPARE(emberAfNullEui64, partnerEUI64, EUI64_SIZE)) {
    emberAfSecurityPrintln("%p: EUI64 cannot be all zeroes.",
                           "Error");
    return;
  }

  if (index > MAX_CLI_SIZE) {
    emberAfSecurityPrintln("%p: index %d is greater than max import limit of %d.",
                           "Error",
                           index,
                           MAX_CLI_SIZE);
    return;
  }

  MEMMOVE(importKeyList[index].deviceId, partnerEUI64, EUI64_SIZE);
  MEMMOVE(emberKeyContents(&(importKeyList[index].key)),
          emberKeyContents(&newKey),
          EMBER_ENCRYPTION_KEY_SIZE);
  if (importData.keyListLength <= index) {
    importData.keyListLength = index + 1;
  }
}

void setExtendedPanIdCommand(void)
{
  emberCopyBigEndianEui64Argument(0, importData.extendedPanId);
}

void restoreFromBackupCommand(void)
{
  EmberStatus status;

  if (0 == MEMCOMPARE(importData.extendedPanId,
                      emberAfNullEui64,
                      EUI64_SIZE)) {
    emberAfSecurityPrintln("%p: Import contains NULL extended PAN ID",
                           "Error");
    return;
  }

  status = emberTrustCenterImportBackupAndStartNetwork(&importData);
  if (status != EMBER_SUCCESS) {
    emberAfSecurityPrintln("%p: Failed to import backup data and form network.",
                           "Error");
    return;
  }
  emberAfSecurityPrintln("Import successful.");
}
#endif //UC_BUILD
