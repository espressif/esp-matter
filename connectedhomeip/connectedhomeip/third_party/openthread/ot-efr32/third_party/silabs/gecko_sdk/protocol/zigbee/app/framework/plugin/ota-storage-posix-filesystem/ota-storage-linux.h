/***************************************************************************//**
 * @file
 * @brief APIs and defines for the OTA Storage POSIX Filesystem plugin.
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

/**
 * @defgroup ota-storage-linux OTA Storage Linux
 * @ingroup component
 * @brief API and Callbacks for the OTA Storage Linux Component
 *
 * Silicon Labs implementation of a multi-file storage module for Over-the-Air
 * Bootload protocols, which is used by either the Zigbee Over-the-Air
 * cluster or the legacy Ember standalone bootloader protocol. It uses a
 * POSIX filesystem as the underlying storage device, and therefore can
 * store any number of files.  It can be used by either the OTA client or
 * OTA server.
 *
 */

/**
 * @addtogroup ota-storage-linux
 * @{
 */

//This is mostly internal so nothing to document here

/** @} */ // end of ota-storage-linux

typedef void (EmAfOtaStorageFileAddedHandler)(const EmberAfOtaHeader*);

typedef struct {
  bool memoryDebug;
  bool fileDebug;
  bool fieldDebug;
  bool ignoreFilesWithUnderscorePrefix;
  bool printFileDiscoveryOrRemoval;
  EmAfOtaStorageFileAddedHandler* fileAddedHandler;
} EmAfOtaStorageLinuxConfig;

void emAfOtaStorageGetConfig(EmAfOtaStorageLinuxConfig* currentConfig);
void emAfOtaStorageSetConfig(const EmAfOtaStorageLinuxConfig* newConfig);
