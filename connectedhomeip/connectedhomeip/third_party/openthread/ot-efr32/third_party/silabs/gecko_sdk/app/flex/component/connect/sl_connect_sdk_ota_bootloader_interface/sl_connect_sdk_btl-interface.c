/***************************************************************************//**
 * @file
 * @brief Set of APIs for the sl_connect_ota_bootloader_interface component.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "bootloader-interface-app.h"
#include "sl_connect_sdk_btl-interface.h"
#include "api/btl_interface.h"

static bool isBootloaderInitialized = false;
static uint32_t storageSlotStartAddress = 0UL;

#define STORAGE_SLOT_0 (0)

//------------------------------------------------------------------------------
// APIs

bool emberAfPluginBootloaderInterfaceIsBootloaderInitialized(void)
{
  return isBootloaderInitialized;
}

void emberAfPluginBootloaderInterfaceGetVersion(uint16_t *blVersion)
{
  *blVersion = halAppBootloaderGetVersion();
}

// HACK: Currently, bootloaderIsCommonBootloader() is not exposed.
// According to Jerome, info->type == SL_BOOTLOADER will guarantee a Gecko bootloader.
static bool isGeckoBootloader()
{
  BootloaderInformation_t bootloaderInfo;
  MEMSET(&bootloaderInfo, 0, sizeof(BootloaderInformation_t));
  bootloader_getInfo(&bootloaderInfo);

  return bootloaderInfo.type == SL_BOOTLOADER;
}

bool emberAfPluginBootloaderInterfaceInit(void)
{
  BootloaderStorageInformation_t storageInfo;
  BootloaderStorageSlot_t storageSlotInfo;
  const HalEepromInformationType *eepromInfo;
  uint8_t result;

  result = halAppBootloaderInit();

  if (result != EEPROM_SUCCESS) {
    return false;
  }

  eepromInfo = halAppBootloaderInfo();

  if (eepromInfo != NULL) {
    #if 0
    emberAfCorePrintln("eeprom info: \r\ninfoVer %l\r\npartDesc %s\r\ncapabilities %l",
                       eepromInfo->version,
                       eepromInfo->partDescription,
                       eepromInfo->capabilitiesMask);
    // responsePrintf can't handle 32 bit decimals
    emberAfCorePrintln("{partSize:%l} {pageSize:%l}",
                       eepromInfo->partSize, eepromInfo->pageSize);
    if ((eepromInfo->capabilitiesMask & EEPROM_CAPABILITIES_PART_ERASE_SECONDS) == EEPROM_CAPABILITIES_PART_ERASE_SECONDS) {
      emberAfCorePrintln("partEraseTime(s) %d pageEraseMs %d",
                         eepromInfo->partEraseTime,
                         eepromInfo->pageEraseMs);
    } else {
      emberAfCorePrintln("partEraseTime(ms) %d pageEraseMs %d",
                         eepromInfo->partEraseTime,
                         eepromInfo->pageEraseMs);
    }
    #endif
  } else {
    emberAfCorePrintln("eeprom info not available");
  }

  bootloader_getStorageInfo(&storageInfo);

  // Single image space
  if (storageInfo.numStorageSlots > 0) {
    if ( bootloader_getStorageSlotInfo(STORAGE_SLOT_0, &storageSlotInfo)
         == BOOTLOADER_OK ) {
      storageSlotStartAddress = storageSlotInfo.address;
    }
  }

  isBootloaderInitialized = true;
  return true;
}

void emberAfPluginBootloaderInterfaceSleep(void)
{
  // USART need to be init'ed to send proper shutdown
  // command to EEPROM.
  if (!isBootloaderInitialized) {
    emberAfPluginBootloaderInterfaceInit();
  }
  halAppBootloaderShutdown();
  isBootloaderInitialized = false;
}

static bool legacyBootloaderChipErase(void)
{
  bool result = true;
  uint16_t delayUs = 50000;
  uint32_t timeoutUs;
  const HalEepromInformationType *eepromInfo;

  eepromInfo = halAppBootloaderInfo();
  if (eepromInfo == NULL) {
    result = false;
    goto DONE;
  }

  if ((eepromInfo->capabilitiesMask & EEPROM_CAPABILITIES_PART_ERASE_SECONDS) == EEPROM_CAPABILITIES_PART_ERASE_SECONDS) {
    timeoutUs = (4 * eepromInfo->partEraseTime) * 1000 * 1024; // partEraseTime in seconds (multiplied by 1024 b/c spiflash
                                                               // erase times in ms are based on 1024Hz)
  } else {
    timeoutUs = (4 * eepromInfo->partEraseTime) * 1000; //partEraseTime in milliseconds
  }

  halResetWatchdog();

  if ( EEPROM_SUCCESS != halAppBootloaderEraseRawStorage(0, eepromInfo->partSize) ) {
    result = false;
    goto DONE;
  }

  while (halAppBootloaderStorageBusy()) {
    halResetWatchdog();
    emberAfCorePrint(".");
    halCommonDelayMicroseconds(delayUs);

    // Exit if timeoutUs exeeded
    timeoutUs -= delayUs;
    if (timeoutUs < delayUs) {
      result = false;
      goto DONE;
    }
  }

  emberAfCorePrintln("");

  DONE:
  if (result) {
    emberAfCorePrintln("flash erase successful!");
  } else {
    emberAfCorePrintln("flash erase failed!");
  }

  return result;
}

static bool geckoBootloaderChipEraseSlot(uint32_t slot)
{
  BootloaderStorageInformation_t storageInfo;
  BootloaderStorageSlot_t storageSlotInfo;
  int32_t retVal = BOOTLOADER_OK;
  uint32_t address;
  uint32_t bytesToErase;

  bootloader_getStorageInfo(&storageInfo);
  if (slot >= storageInfo.numStorageSlots) {
    emberAfCorePrintln("ERROR: invalid erase slot number(%d)!", slot);
    return false;
  }

  // Get information on Slot
  retVal = bootloader_getStorageSlotInfo(slot, &storageSlotInfo);
  if (BOOTLOADER_OK != retVal) {
    return false;
  }

  bytesToErase = (storageSlotInfo.length / storageInfo.info->pageSize)
                 * storageInfo.info->pageSize;

  // Check for a misaligned slot
  // This shouldn't happen unless the user configures something improperly, and
  // even then, the bootloader may complain when being compiled/run
  if (storageSlotInfo.length % storageInfo.info->pageSize) {
    emberAfCorePrintln("Erase: slot length (%d) not aligned "
                       "to page size (%d). The entire slot will not be erased.",
                       storageSlotInfo.length,
                       storageInfo.info->pageSize);
  }
  address = storageSlotInfo.address;

  // Erase the slot in page chunks
  while ((BOOTLOADER_OK == retVal)
         && ((address - storageSlotInfo.address) < bytesToErase)) {
    halResetWatchdog();
    emberAfCorePrint(".");
    retVal = bootloader_eraseRawStorage(address, storageInfo.info->pageSize);
    address += storageInfo.info->pageSize;
  }

  emberAfCorePrintln("");

  if (BOOTLOADER_OK != retVal) {
    emberAfCorePrintln("Erase: failed to erase %d bytes in slot at "
                       "address 0x%4x (error 0x%x)",
                       storageInfo.info->pageSize,
                       address - storageInfo.info->pageSize,
                       retVal);
  }
  return (BOOTLOADER_OK == retVal);
}

// Erase all Gecko bootloader storage slots
static bool geckoBootloaderChipEraseSlotAll(void)
{
  bool retVal = true;
  bool result = true;
  BootloaderStorageInformation_t storageInfo;
  MEMSET(&storageInfo, 0, sizeof(BootloaderStorageInformation_t));
  bootloader_getStorageInfo(&storageInfo);
  for (uint32_t index = 0; index < storageInfo.numStorageSlots; index++) {
    emberAfCorePrintln("flash erasing slot %d started", index);
    result = geckoBootloaderChipEraseSlot(index);

    if (result) {
      emberAfCorePrintln("flash erase successful!");
    } else {
      emberAfCorePrintln("flash erase failed!");
    }

    retVal &= result;
  }

  return retVal;
}

bool emberAfPluginBootloaderInterfaceChipErase(void)
{
  bool result = true;
  if (isGeckoBootloader()) {
    result = geckoBootloaderChipEraseSlotAll();
  } else {
    result = legacyBootloaderChipErase();
  }

  return result;
}

bool emberAfPluginBootloaderInterfaceChipEraseSlot(uint32_t slot)
{
  return geckoBootloaderChipEraseSlot(slot);
}

uint16_t emberAfPluginBootloaderInterfaceValidateImage(void)
{
  uint16_t result;

  emberAfCorePrint("Verifying image");

  halResetWatchdog();
  halAppBootloaderImageIsValidReset();

  do {
    halResetWatchdog();
    result = halAppBootloaderImageIsValid();
    emberAfCorePrint(".");
  } while (result == BL_IMAGE_IS_VALID_CONTINUE);

  if (result == 0) {
    emberAfCorePrintln("failed!");
  } else {
    emberAfCorePrintln("done!");
  }

  return result;
}

void emberAfPluginBootloaderInterfaceBootload(void)
{
  EmberStatus result;

  if (!emberAfPluginBootloaderInterfaceValidateImage()) {
    return;
  }

  emberAfCorePrintln("Installing new image and rebooting...");
  halCommonDelayMilliseconds(500);

  result = halAppBootloaderInstallNewImage();

  // We should not get here if bootload is succeeded.
  emberAfCorePrintln("New image installation ");
  if (result == EMBER_SUCCESS) {
    emberAfCorePrintln("done!");
  } else {
    emberAfCorePrintln("failed (error 0x%x)!", result);
  }
  return;
}

bool emberAfPluginBootloaderInterfaceRead(uint32_t startAddress,
                                          uint32_t length,
                                          uint8_t *buffer)
{
  uint32_t address;
  uint32_t remainingLength;
  uint8_t result;
  uint8_t len;

  address = startAddress;
  remainingLength = length;

  if (isBootloaderInitialized) {
    // Implement block read so we can take care of the watchdog reset.
    while (remainingLength) {
      halResetWatchdog();
      len = (remainingLength > 255) ? 255 : (uint8_t)remainingLength;
      result = halAppBootloaderReadRawStorage((storageSlotStartAddress + address), buffer + address - startAddress, len);

      if (EEPROM_SUCCESS != result) {
        return false;
      }

      remainingLength -= len;
      address += len;
    }
    return true;
  }

  return false;
}

bool emberAfPluginBootloaderInterfaceWrite(uint32_t startAddress,
                                           uint32_t length,
                                           const uint8_t *buffer)
{
  uint32_t address;
  uint32_t remainingLength;
  uint8_t result;
  uint8_t len;

  address = startAddress;
  remainingLength = length;

  if (isBootloaderInitialized) {
    // Implement block write so we can take care of the watchdog reset.
    while (remainingLength) {
      halResetWatchdog();
      len = (remainingLength > 255) ? 255 : (uint8_t)remainingLength;
      result = halAppBootloaderWriteRawStorage((storageSlotStartAddress + address), buffer + address - startAddress, len);

      if (EEPROM_SUCCESS != result) {
        return false;
      }

      remainingLength -= len;
      address += len;
    }

    return true;
  }

  return false;
}
