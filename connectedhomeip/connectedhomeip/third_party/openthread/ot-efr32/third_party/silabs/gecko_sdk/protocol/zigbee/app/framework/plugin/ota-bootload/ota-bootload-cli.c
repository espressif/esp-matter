/***************************************************************************//**
 * @file
 * @brief Bootload specific commands
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
#include "app/util/serial/sl_zigbee_command_interpreter.h"
#include "app/framework/plugin/ota-common/ota.h"

#ifdef UC_BUILD

#include "bootloader-interface.h"
void printBootloaderInfoCommand(sl_cli_command_arg_t *arguments)
{
#if !defined(EZSP_HOST)
  UNUSED BlExtendedType blExtendedType = halBootloaderGetInstalledType();
  uint32_t getEmberVersion;
  uint32_t customVersion;
  uint8_t keyData[EMBER_ENCRYPTION_KEY_SIZE];
  halGetExtendedBootloaderVersion(&getEmberVersion, &customVersion);
  otaPrintln("Installed Type (Base):      0x%X", halBootloaderGetType());
  otaPrintln("Installed Type (Extended):  0x%2X", blExtendedType);
  otaPrintln("Bootloader Version:         0x%2X", halGetBootloaderVersion());
  otaPrintln("Bootloader Version, Ember:  0x%4X", getEmberVersion);
  otaPrintln("Bootloader Version, Custom: 0x%4X", customVersion);

#if defined(EMBER_TEST)
  MEMSET(keyData, 0xFF, EMBER_ENCRYPTION_KEY_SIZE);
#else
  halCommonGetToken(keyData, TOKEN_MFG_SECURE_BOOTLOADER_KEY);
#endif

  otaPrint("Secure Bootloader Key:      ");
  emberAfPrintZigbeeKey(keyData);
  otaPrintln("");

#else
  otaPrintln("Unsupported on EZSP Host");
#endif
}

#else
#include "hal/micro/bootloader-interface.h"
//------------------------------------------------------------------------------
// Forward Declarations

void printBootloaderInfoCommand(void);

#ifndef EMBER_AF_GENERATE_CLI
EmberCommandEntry emberAfPluginOtaBootloadCommands[] = {
  emberCommandEntryAction("info", printBootloaderInfoCommand, "",
                          "Print information about the installed bootloader."),
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI

//------------------------------------------------------------------------------

void printBootloaderInfoCommand(void)
{
#if !defined(EZSP_HOST)
  UNUSED BlExtendedType blExtendedType = halBootloaderGetInstalledType();
  uint32_t getEmberVersion;
  uint32_t customVersion;
  uint8_t keyData[EMBER_ENCRYPTION_KEY_SIZE];
  halGetExtendedBootloaderVersion(&getEmberVersion, &customVersion);
  otaPrintln("Installed Type (Base):      0x%X", halBootloaderGetType());
  otaPrintln("Installed Type (Extended):  0x%2X", blExtendedType);
  otaPrintln("Bootloader Version:         0x%2X", halGetBootloaderVersion());
  otaPrintln("Bootloader Version, Ember:  0x%4X", getEmberVersion);
  otaPrintln("Bootloader Version, Custom: 0x%4X", customVersion);

#if defined(EMBER_TEST)
  MEMSET(keyData, 0xFF, EMBER_ENCRYPTION_KEY_SIZE);
#else
  halCommonGetToken(keyData, TOKEN_MFG_SECURE_BOOTLOADER_KEY);
#endif

  otaPrint("Secure Bootloader Key:      ");
  emberAfPrintZigbeeKey(keyData);
  otaPrintln("");

#else
  otaPrintln("Unsupported on EZSP Host");
#endif
}
#endif
