/***************************************************************************//**
 * @file
 * @brief Simulation for the OTA Storage POSIX Filesystem plugin.
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
#include "app/framework/plugin/ota-common/ota.h"
#include "ota-storage-linux-config.h"

#include "app/util/serial/sl_zigbee_command_interpreter.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//------------------------------------------------------------------------------
// Globals

#define INVALID_MANUFACTURER_ID  0xFFFF
#define INVALID_DEVICE_ID        0xFFFF
#define INVALID_FIRMWARE_VERSION 0xFFFFFFFFL
#define INVALID_EUI64 { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define INVALID_SLOT (uint32_t)-1

#define INVALID_OTA_IMAGE_ID  \
  { INVALID_MANUFACTURER_ID,  \
    INVALID_DEVICE_ID,        \
    INVALID_FIRMWARE_VERSION, \
    INVALID_EUI64,            \
  }

EmberAfOtaImageId emberInvalidImageId = INVALID_OTA_IMAGE_ID;
// Dummy variable only to fix build error for simulation host target.
char defaultStorageDirectory[OTA_FILE_STORAGE_DIR_LENGTH] = "ota-files";
//------------------------------------------------------------------------------
// Forward Declarations

//------------------------------------------------------------------------------

void emAfOtaStorageClose(void)
{
}

void emAfOtaStorageInfoPrint(void)
{
}

EmberAfOtaStorageStatus emberAfOtaStorageCheckTempDataCallback(uint32_t* returnOffset,
                                                               uint32_t* returnTotalSize,
                                                               EmberAfOtaImageId* returnOtaImageId)
{
  return EMBER_AF_OTA_STORAGE_SUCCESS;
}

EmberAfOtaStorageStatus emberAfOtaStorageClearTempDataCallback(void)
{
  return EMBER_AF_OTA_STORAGE_SUCCESS;
}

void emAfOtaLoadFileCommand(void)
{
}

EmberAfOtaStorageStatus emberAfOtaStorageDeleteImageCallback(const EmberAfOtaImageId* id)
{
  return EMBER_AF_OTA_STORAGE_ERROR;
}

uint8_t emberAfOtaStorageGetCountCallback(void)
{
  return 0;
}

EmberAfOtaStorageStatus emberAfOtaStorageGetFullHeaderCallback(const EmberAfOtaImageId* id,
                                                               EmberAfOtaHeader* returnData)
{
  return EMBER_AF_OTA_STORAGE_ERROR;
}

uint32_t emberAfOtaStorageGetTotalImageSizeCallback(const EmberAfOtaImageId* id)
{
  return 0;
}

EmberAfOtaStorageStatus emberAfOtaStorageInitCallback(void)
{
  return EMBER_AF_OTA_STORAGE_SUCCESS;
}

EmberAfOtaImageId emberAfOtaStorageIteratorFirstCallback(void)
{
  return emberInvalidImageId;
}

EmberAfOtaImageId emberAfOtaStorageIteratorNextCallback(void)
{
  return emberInvalidImageId;
}

EmberAfOtaStorageStatus emberAfOtaStorageReadImageDataCallback(const EmberAfOtaImageId* id,
                                                               uint32_t offset,
                                                               uint32_t length,
                                                               uint8_t* returnData,
                                                               uint32_t* returnedLength)
{
  return EMBER_AF_OTA_STORAGE_ERROR;
}

EmberAfOtaImageId emberAfOtaStorageSearchCallback(uint16_t manufacturerId,
                                                  uint16_t manufacturerDeviceId,
                                                  const uint16_t* hardwareVersion)
{
  return emberInvalidImageId;
}

uint32_t emAfOtaStorageGetSlot(void)
{
  return INVALID_SLOT;
}

uint32_t emberAfOtaStorageDriverMaxDownloadSizeCallback(void)
{
  // In theory we are limited by the local disk space, but for now
  // assume there is no limit.
  return 0xFFFFFFFFUL;
}
