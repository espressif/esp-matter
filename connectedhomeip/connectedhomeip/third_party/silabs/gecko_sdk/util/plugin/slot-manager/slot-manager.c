/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#include PLATFORM_HEADER
#include "slot-manager.h"
#include "printf.h"
#include "hal.h"
#include "api/btl_interface.h"
#include "api/btl_interface_storage.h"

#ifndef SLOT_MANAGER_NUM_SLOTS
 #define SLOT_MANAGER_NUM_SLOTS 8
#endif

static uint32_t gBootloaderNumSlots;
__ALIGNED(4) static uint8_t  gVerificationContext[BOOTLOADER_STORAGE_VERIFICATION_CONTEXT_SIZE];
static int32_t  gSlotsToBoot[SLOT_MANAGER_NUM_SLOTS];

// Prototypes
bool    initializeBootloader(void);

void emberAfPluginSlotManagerInitCallback(void)
{
  (void)initializeBootloader();
}

bool initializeBootloader(void)
{
  BootloaderStorageInformation_t storageInfo;
  int32_t rv;
  static bool bootloaderInitialized = false;

  if (!bootloaderInitialized) {
    rv = bootloader_init();

    if (BOOTLOADER_OK == rv) {
      bootloaderInitialized = true;

      bootloader_getStorageInfo(&storageInfo);
      gBootloaderNumSlots = storageInfo.numStorageSlots;
    } else {
      // If this fires, it possibly means a legacy bootloader is on the chip
      printf("Slot Manager: failed to initialize bootloader (error 0x%4x)\n", rv);
    }
  }
  return bootloaderInitialized;
}

void emberAfPluginSlotManagerPrintExternalFlashInfo(void)
{
  BootloaderStorageInformation_t storageInfo;

  if (!initializeBootloader()) {
    return;
  }

  bootloader_getStorageInfo(&storageInfo);
  printf("Version     : %d\n", storageInfo.info->version);
  printf("Part        : %s\n", storageInfo.info->partDescription);
  printf("Capabilities: 0x%x\n", storageInfo.info->capabilitiesMask);
  printf("Part size   : %d B\n", storageInfo.info->partSize);
  printf("Page size   : %d B\n", storageInfo.info->pageSize);
  printf("Word size   : %d B\n", storageInfo.info->wordSizeBytes);
  printf("Slots       : %d\n", gBootloaderNumSlots);
}

sl_status_t emberAfPluginSlotManagerReadExtFlash(uint32_t address,
                                                 uint8_t  * data,
                                                 uint8_t  length)
{
  int32_t rv;

  if (!data || (0 == length)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (!initializeBootloader()) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  rv = bootloader_readRawStorage(address, data, length);
  if (BOOTLOADER_OK != rv) {
    printf("Failed to read from storage flash (error %d)\n", rv);
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

sl_status_t emberAfPluginSlotManagerVerifyAndBootloadSlot(uint32_t slotId)
{
  sl_status_t status = SL_STATUS_OK;
  int32_t rv;

  if (!initializeBootloader()) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (slotId >= gBootloaderNumSlots) {
    printf("Invalid slot %d (max slot %d)\n", slotId, gBootloaderNumSlots - 1);
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Verify the image
  printf("Beginning image verification...");

  halResetWatchdog();
  rv = bootloader_initVerifyImage(slotId,
                                  (void*)gVerificationContext,
                                  BOOTLOADER_STORAGE_VERIFICATION_CONTEXT_SIZE);
  halResetWatchdog();

  while (rv == BOOTLOADER_OK) {
    halResetWatchdog();
    rv = bootloader_continueVerifyImage((void*)gVerificationContext, NULL);
    if (rv == BOOTLOADER_ERROR_PARSE_SUCCESS) {
      break;
    } else if (rv == BOOTLOADER_ERROR_PARSE_CONTINUE) {
      printf(".");
      rv = BOOTLOADER_OK;
    }
  }

  // Bootload the image if it was verified with success
  if (rv != BOOTLOADER_ERROR_PARSE_SUCCESS) {
    printf(".failed! (Error 0x%4x)\n", rv);
    status = SL_STATUS_FAIL;
  } else {
    printf(".passed! Booting slot %d\n", slotId);
    status = emberAfPluginSlotManagerBootSlot(slotId);
  }

  printf("\n");

  return status;
}

sl_status_t emberAfPluginSlotManagerGetSlotInfo(uint32_t             slotId,
                                                SlotManagerSlotInfo_t* slotInfo)
{
  int32_t  rv;
  uint32_t btlVersion;

  if (!initializeBootloader()) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if ((slotId >= gBootloaderNumSlots) || (!slotInfo)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  MEMSET(slotInfo, 0, sizeof(*slotInfo));

  rv = bootloader_getStorageSlotInfo(slotId, &(slotInfo->slotStorageInfo));
  if (BOOTLOADER_OK != rv) {
    return SL_STATUS_FAIL;
  }

  rv = bootloader_getImageInfo(slotId, &(slotInfo->slotAppInfo), &btlVersion);

  return ((BOOTLOADER_OK == rv) || ((BOOTLOADER_ERROR_STORAGE_BASE | BOOTLOADER_ERROR_STORAGE_NO_IMAGE) == rv)) ? SL_STATUS_OK : SL_STATUS_FAIL;
}

sl_status_t emberAfPluginSlotManagerWriteToSlot(uint32_t slotId,
                                                uint32_t offset,
                                                uint8_t  * buffer,
                                                size_t   length)
{
  int32_t rv;
  BootloaderStorageSlot_t storageSlot;

  if (!initializeBootloader()) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (slotId >= gBootloaderNumSlots) {
    printf("Invalid slot %d (max slot %d)\n", slotId, gBootloaderNumSlots - 1);
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (!buffer || (0 == length)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  rv = bootloader_getStorageSlotInfo(slotId, &storageSlot);
  if (BOOTLOADER_OK != rv) {
    printf("Failed to get info for slot %d\n", slotId);
    return SL_STATUS_FAIL;
  }

  // Report an error if too many bytes are being written into the slot
  if ((offset + length) > storageSlot.length) {
    printf("Slot Manager: slot write failure. Address offset (%d) +"
           " write length (%d) greater than total slot length (%d)\n",
           offset, length, storageSlot.length);
    return SL_STATUS_INVALID_RANGE;
  }

  rv = bootloader_writeStorage(slotId, offset, buffer, length);

  return (BOOTLOADER_OK == rv) ? SL_STATUS_OK : SL_STATUS_FAIL;
}

sl_status_t emberAfPluginSlotManagerEraseSlot(uint32_t slotId)
{
  int32_t rv = BOOTLOADER_OK;
  BootloaderStorageInformation_t storageInfo;
  BootloaderStorageSlot_t storageSlotInfo;
  uint32_t bytesToErase;
  uint32_t address;

  if (!initializeBootloader()) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (slotId >= gBootloaderNumSlots) {
    printf("Invalid slot %d (max slot %d)\n", slotId, gBootloaderNumSlots - 1);
    return SL_STATUS_INVALID_PARAMETER;
  }

  bootloader_getStorageInfo(&storageInfo);

  rv = bootloader_getStorageSlotInfo(slotId, &storageSlotInfo);

  if (BOOTLOADER_OK != rv) {
    return SL_STATUS_FAIL;
  }

  bytesToErase = (storageSlotInfo.length / storageInfo.info->pageSize)
                 * storageInfo.info->pageSize;

  // Check for a misaligned slot
  // This shouldn't happen unless the user configures something improperly, and
  // even then, the bootloader may complain when being compiled/run
  if (storageSlotInfo.length % storageInfo.info->pageSize) {
    printf("Slot Manager warning: slot %d length (%d) not aligned "
           "to page size (%d). The entire slot will not be erased.\n",
           slotId,
           storageSlotInfo.length,
           storageInfo.info->pageSize);
  }

  address = storageSlotInfo.address;

  // Erase the slot in page chunks
  while ((BOOTLOADER_OK == rv)
         && ((address - storageSlotInfo.address) < bytesToErase)) {
    halResetWatchdog();
    rv = bootloader_eraseRawStorage(address, storageInfo.info->pageSize);
    address += storageInfo.info->pageSize;
  }

  if (BOOTLOADER_OK != rv) {
    printf("Slot Manager: failed to erase %d bytes in slot %d at "
           "address 0x%4x (error 0x%x)\n",
           storageInfo.info->pageSize,
           slotId,
           address - storageInfo.info->pageSize,
           rv);
  } else {
    printf("Erased %d bytes in slot %d\n", bytesToErase, slotId);
  }

  return (BOOTLOADER_OK == rv) ? SL_STATUS_OK : SL_STATUS_FAIL;
}

sl_status_t emberAfPluginSlotManagerGetNumberOfSlots(uint32_t *numSlots)
{
  if (!numSlots) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (!initializeBootloader()) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  *numSlots = gBootloaderNumSlots;

  return SL_STATUS_OK;
}

sl_status_t emberAfPluginSlotManagerImageIsValidReset(uint32_t slotId)
{
  int32_t rv;

  if (!initializeBootloader()) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (slotId >= gBootloaderNumSlots) {
    printf("Invalid slot %d (max slot %d)\n", slotId, gBootloaderNumSlots - 1);
    return SL_STATUS_INVALID_PARAMETER;
  }

  halResetWatchdog();

  rv = bootloader_initVerifyImage(slotId,
                                  (void*)gVerificationContext,
                                  BOOTLOADER_STORAGE_VERIFICATION_CONTEXT_SIZE);

  return (BOOTLOADER_OK == rv) ? SL_STATUS_OK : SL_STATUS_FAIL;
}

sl_status_t continueVerifyImage(BootloaderParserCallback_t callbackFunction)
{
  int32_t rv;

  if (!initializeBootloader()) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  rv = bootloader_continueVerifyImage((void*)gVerificationContext,
                                      callbackFunction);// may be NULL for no CB

  if (rv == BOOTLOADER_ERROR_PARSE_SUCCESS) {
    return SL_STATUS_OK;
  } else if (rv == BOOTLOADER_ERROR_PARSE_CONTINUE) {
    return SL_STATUS_IN_PROGRESS;
  }

  return SL_STATUS_FAIL;
}

sl_status_t emberAfPluginSlotManagerImageIsValid()
{
  if (!initializeBootloader()) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  return continueVerifyImage(NULL);
}

sl_status_t emberAfPluginSlotManagerBootSlot(uint32_t slotId)
{
  int32_t  rv;
  uint32_t index;
  uint32_t slotsToPush = gBootloaderNumSlots;

  if (!initializeBootloader()) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (slotId >= gBootloaderNumSlots) {
    printf("Invalid slot %d (max slot %d)\n", slotId, gBootloaderNumSlots - 1);
    return SL_STATUS_INVALID_PARAMETER;
  }

  // First, fetch the ordered list of slots to bootload
  rv = bootloader_getImagesToBootload(gSlotsToBoot, gBootloaderNumSlots);
  if (BOOTLOADER_OK != rv) {
    // If this fails, it means there is no bootload info metadata in flash
    // This can happen for a number of reasons (e.g. user didn't flash metadata
    // to right address, metadata is corrupt, etc)
    // Simply writing the slot-to-boot will fix this condition
    rv = bootloader_setImageToBootload(slotId);
    if (BOOTLOADER_OK != rv) {
      printf("Failed to set slot to boot (error 0x%4x)\n", rv);
    } else {
      bootloader_rebootAndInstall();
    }
    return (BOOTLOADER_OK == rv) ? SL_STATUS_OK : SL_STATUS_FAIL;
  }

  // Second, search if we are already in the slots-to-boot
  for (index = 0; index < gBootloaderNumSlots; index++) {
    if (slotId == (uint32_t)gSlotsToBoot[index]) {
      slotsToPush = index;
      break;
    }
  }

  // Third, push all the slots over
  // If we weren't in the slots-to-boot list, then we push all elements over one
  if (slotsToPush == gBootloaderNumSlots) {
    slotsToPush--;
  }

  if (slotsToPush > 0) {
    for (index = slotsToPush; index > 0; index--) {
      gSlotsToBoot[index] = gSlotsToBoot[index - 1];
    }
  }

  gSlotsToBoot[0] = slotId;

  // Fourth, set the new slots-to-boot order through the bootloader
  rv = bootloader_setImagesToBootload(gSlotsToBoot, gBootloaderNumSlots);
  if (BOOTLOADER_OK != rv) {
    printf("Failed to set list of slots to boot (error 0x%4x)\n", rv);
    return SL_STATUS_FAIL;
  }

  rv = bootloader_getImagesToBootload(gSlotsToBoot, gBootloaderNumSlots);
  if (BOOTLOADER_OK != rv) {
    printf("Failed to get new list of slots to boot (error 0x%4x)\n", rv);
    return SL_STATUS_FAIL;
  }

  // Finally, boot the new list of slots to boot. This call won't return.
  bootloader_rebootAndInstall();

  return SL_STATUS_OK;
}

sl_status_t emberAfPluginSlotManagerGetMetadataTagsFromGbl(
  uint32_t slotId,
  BootloaderParserCallback_t callbackFunction)
{
  sl_status_t status;

  if (!initializeBootloader()) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if ((slotId >= gBootloaderNumSlots) || (NULL == callbackFunction)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  halResetWatchdog();
  status = emberAfPluginSlotManagerImageIsValidReset(slotId);

  if (SL_STATUS_OK != status) {
    return status;
  }

  halResetWatchdog();
  status = continueVerifyImage(callbackFunction);

  while (SL_STATUS_IN_PROGRESS == status) {
    halResetWatchdog();
    status = continueVerifyImage(callbackFunction);
  }

  return status;
}
