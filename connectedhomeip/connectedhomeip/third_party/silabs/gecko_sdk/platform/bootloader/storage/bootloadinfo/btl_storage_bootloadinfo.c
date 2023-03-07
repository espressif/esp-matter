/***************************************************************************//**
 * @file
 * @brief Bootload Info for Silicon Labs Bootloader.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "config/btl_config.h"
#include "btl_storage_bootloadinfo.h"
#include "storage/btl_storage_internal.h"

#include "storage/btl_storage.h"
#include "security/btl_crc32.h"

#include "debug/btl_debug.h"

int32_t storage_getBootloadList(int32_t slotIds[], size_t length)
{
  BootloadInfo_t btlInfo = { 0 };
  BootloaderStorageInformation_t storageInfo = { 0 };
  int32_t retval;

  // Get storage info
  storage_getInfo(&storageInfo);

  // If there is only one slot, don't use the BootloadInfo_t struct
  if (storageInfo.numStorageSlots <= 1UL) {
    slotIds[0] = 0;
    for (size_t i = 1UL; i < length; i++) {
      slotIds[i] = -1;
    }
    // Return early
    return BOOTLOADER_OK;
  } else if (storageInfo.numStorageSlots < length) {
    return BOOTLOADER_ERROR_BOOTLOAD_LIST_OVERFLOW;
  } else {
    // continue to multi-slot handling
  }

  uint32_t btlInfoAddress0 = storage_getBaseAddress();
  uint32_t btlInfoAddress1 = btlInfoAddress0 + storageInfo.info->pageSize;

  // Try reading from first page
  retval = storage_readRaw(btlInfoAddress0, (uint8_t *)&btlInfo, 12UL);
  if (retval != BOOTLOADER_OK) {
    return retval;
  }

  if (btlInfo.magic != BTL_STORAGE_BOOTLOADINFO_MAGIC) {
    // Page 0 is corrupt; read from page 1
    retval = storage_readRaw(btlInfoAddress1, (uint8_t *)&btlInfo, 12UL);
    if (retval != BOOTLOADER_OK) {
      return retval;
    }

    if (btlInfo.magic != BTL_STORAGE_BOOTLOADINFO_MAGIC) {
      // Page 1 is also corrupt; cannot do anything but bail
      for (size_t i = 0UL; i < length; i++) {
        slotIds[i] = -1;
      }
      BTL_DEBUG_PRINTLN("No magic");
      return BOOTLOADER_ERROR_BOOTLOAD_LIST_NO_LIST;
    }

    // Header looks good; read rest of struct
    storage_readRaw(btlInfoAddress1, (uint8_t *)&btlInfo, btlInfo.length);

    // Recover page 0 from page 1
    BTL_DEBUG_PRINTLN("BI pg0 recover");
    retval = storage_eraseRaw(btlInfoAddress0, storageInfo.info->pageSize);
    if (retval != BOOTLOADER_OK) {
      return retval;
    }
    retval = storage_writeRaw(btlInfoAddress0,
                              (uint8_t *)&btlInfo,
                              btlInfo.length);
    if (retval != BOOTLOADER_OK) {
      return retval;
    }
  }

  // Header looks good; read rest of struct
  storage_readRaw(btlInfoAddress0, (uint8_t *)&btlInfo, btlInfo.length);

  // Test CRC to ensure struct is valid
  if (btl_crc32Stream((uint8_t *)&btlInfo, btlInfo.length, BTL_CRC32_START)
      != BTL_CRC32_END) {
    for (size_t i = 0UL; i < length; i++) {
      slotIds[i] = -1;
    }
    BTL_DEBUG_PRINTLN("CRC ERR");
    return BOOTLOADER_ERROR_BOOTLOAD_LIST_NO_LIST;
  }

  size_t entriesInBtlInfo = (btlInfo.length - 4UL * sizeof(uint32_t))
                            / sizeof(uint32_t);
  size_t minLength = (length > entriesInBtlInfo) ? entriesInBtlInfo : length;
  for (size_t i = 0UL; i < minLength; i++) {
    slotIds[i] = btlInfo.bootloadList[i];
  }
  // If user-supplied list is bigger than Bootload Info list, pad with -1
  for (size_t i = minLength; i < length; i++) {
    slotIds[i] = -1;
  }

  if (slotIds[0] == -1) {
    BTL_DEBUG_PRINTLN("No slots");
  }

  return BOOTLOADER_OK;
}

int32_t storage_setBootloadList(int32_t slotIds[], size_t length)
{
  BootloadInfo_t btlInfo = { 0 };
  btlInfo.magic = BTL_STORAGE_BOOTLOADINFO_MAGIC;
  btlInfo.structVersion = BTL_STORAGE_BOOTLOADINFO_VERSION;
  btlInfo.length = sizeof(BootloadInfo_t);

  BootloaderStorageInformation_t storageInfo = { 0 };
  int32_t retval;

  storage_getInfo(&storageInfo);

  // It makes no sense to try setting more slots to boot than there are slots
  if (storageInfo.numStorageSlots < length) {
    return BOOTLOADER_ERROR_BOOTLOAD_LIST_OVERFLOW;
  }

  // If there is only one slot, don't use the BootloadInfo_t struct
  if (storageInfo.numStorageSlots <= 1UL) {
    // Allow setting slot 0, ignore setting -1
    if ((slotIds[0] != 0L) && (slotIds[0] != -1L)) {
      return BOOTLOADER_ERROR_BOOTLOAD_LIST_OVERFLOW;
    }

    // Return early
    return BOOTLOADER_OK;
  }

  uint32_t btlInfoAddress0 = storage_getBaseAddress();
  uint32_t btlInfoAddress1 = btlInfoAddress0 + storageInfo.info->pageSize;

  size_t minLength = (length > BTL_STORAGE_BOOTLOAD_LIST_MAX_LENGTH)
                     ? BTL_STORAGE_BOOTLOAD_LIST_MAX_LENGTH
                     : length;
  for (size_t i = 0UL; i < minLength; i++) {
    btlInfo.bootloadList[i] = slotIds[i];
  }
  // If user-supplied list is shorter than Bootload Info list, pad with -1
  for (size_t i = minLength; i < BTL_STORAGE_BOOTLOAD_LIST_MAX_LENGTH; i++) {
    btlInfo.bootloadList[i] = -1;
  }

  btlInfo.crc32 = ~btl_crc32Stream((uint8_t *)&btlInfo,
                                   sizeof(btlInfo) - 4UL,
                                   BTL_CRC32_START);

  // Erase & write page 1
  retval = storage_eraseRaw(btlInfoAddress1, storageInfo.info->pageSize);
  if (retval != BOOTLOADER_OK) {
    return retval;
  }
  retval = storage_writeRaw(btlInfoAddress1,
                            (uint8_t *)&btlInfo,
                            sizeof(btlInfo));
  if (retval != BOOTLOADER_OK) {
    return retval;
  }

  // Erase & write page 0
  retval = storage_eraseRaw(btlInfoAddress0, storageInfo.info->pageSize);
  if (retval != BOOTLOADER_OK) {
    return retval;
  }
  retval = storage_writeRaw(btlInfoAddress0,
                            (uint8_t *)&btlInfo,
                            sizeof(btlInfo));
  if (retval != BOOTLOADER_OK) {
    return retval;
  }
  return BOOTLOADER_OK;
}

int32_t storage_appendBootloadList(int32_t slotId)
{
  int32_t slotIds[BTL_STORAGE_BOOTLOAD_LIST_MAX_LENGTH];
  int32_t ret;
  bool inserted = false;

  BootloaderStorageInformation_t storageInfo = { 0 };
  storage_getInfo(&storageInfo);

  uint32_t maxEntries = storageInfo.numStorageSlots;

  // If there is only one slot, don't use the BootloadInfo_t struct
  if (storageInfo.numStorageSlots <= 1UL) {
    // Return early
    return BOOTLOADER_ERROR_BOOTLOAD_LIST_FULL;
  }

  ret = storage_getBootloadList(slotIds, maxEntries);
  if ((ret != BOOTLOADER_OK)
      && (ret != BOOTLOADER_ERROR_BOOTLOAD_LIST_NO_LIST)) {
    return ret;
  }
  for (size_t i = 0UL; i < maxEntries; i++) {
    if (slotIds[i] == -1) {
      slotIds[i] = slotId;
      inserted = true;
      break;
    } else if (slotIds[i] == slotId) {
      return BOOTLOADER_ERROR_BOOTLOAD_LIST_ENTRY_EXISTS;
    } else {
      // continue
    }
  }
  if (inserted) {
    return storage_setBootloadList(slotIds, maxEntries);
  } else {
    return BOOTLOADER_ERROR_BOOTLOAD_LIST_FULL;
  }
}
