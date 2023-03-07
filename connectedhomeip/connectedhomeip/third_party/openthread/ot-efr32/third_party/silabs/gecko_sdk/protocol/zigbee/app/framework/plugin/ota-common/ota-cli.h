/***************************************************************************//**
 * @file
 * @brief Zigbee Over-the-air bootload cluster for upgrading firmware and
 * downloading specific file.  This is the CLI to interact with the
 * main cluster code.
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

// Common CLI interface
EmberAfOtaImageId emAfOtaFindImageIdByIndex(uint8_t index);
#ifndef UC_BUILD
void emAfOtaPrintAllImages(void);

void emAfOtaReloadStorageDevice(void);

#define OTA_COMMON_COMMANDS                                                      \
  emberCommandEntryAction("printImages", emAfOtaPrintAllImages, "", ""),         \
  emberCommandEntryAction("delete", (CommandAction)emAfOtaImageDelete, "u", ""), \
  emberCommandEntryAction("reload", emAfOtaReloadStorageDevice, "", ""),         \
  emberCommandEntryAction("storage-info", emAfOtaStorageInfoPrint, "", ""),      \
  emberCommandEntryTerminator(),                                                 \


// Client CLI interface

#if !defined (EMBER_AF_PLUGIN_OTA_CLIENT)
  #define OTA_CLIENT_COMMANDS
#endif

void otaFindServerCommand(void);
void otaQueryServerCommand(void);
void otaUsePageRequestCommand(void);
void otaQuerySpecificFileCommand(void);
void otaSendUpgradeCommand(void);
void emAfOtaImageDelete(void);

// Server CLI interface

#if !defined (EMBER_AF_PLUGIN_OTA_SERVER)
  #define OTA_SERVER_COMMANDS
#endif

void otaImageNotifyCommand(void);
#endif
