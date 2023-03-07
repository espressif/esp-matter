/***************************************************************************//**
 * @file
 * @brief Application interface to the storage component of the bootloader.
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

#include "btl_interface.h"

// -----------------------------------------------------------------------------
// Defines

// Make assert no-op if not configured
#ifndef BTL_ASSERT
#define BTL_ASSERT(x)
#endif

// -----------------------------------------------------------------------------
// Static variables

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
static Bootloader_PPUSATDnCLKENnState_t blPPUSATDnCLKENnState = { 0 };
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

// -----------------------------------------------------------------------------
// Functions

void bootloader_getStorageInfo(BootloaderStorageInformation_t *info)
{
  if (!bootloader_pointerValid(mainBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable->storage)) {
    return;
  }
#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  mainBootloaderTable->storage->getInfo(info);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE
}

int32_t bootloader_getStorageSlotInfo(uint32_t                slotId,
                                      BootloaderStorageSlot_t *slot)
{
  if (!bootloader_pointerValid(mainBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable->storage)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }
  return mainBootloaderTable->storage->getSlotInfo(slotId, slot);
}

int32_t bootloader_readStorage(uint32_t slotId,
                               uint32_t offset,
                               uint8_t  *buffer,
                               size_t   length)
{
  if (!bootloader_pointerValid(mainBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable->storage)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  int32_t retVal = mainBootloaderTable->storage->read(slotId, offset, buffer, length);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif

  return retVal;
}

int32_t bootloader_writeStorage(uint32_t slotId,
                                uint32_t offset,
                                uint8_t  *buffer,
                                size_t   length)
{
  if (!bootloader_pointerValid(mainBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable->storage)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  int32_t retVal = mainBootloaderTable->storage->write(slotId, offset, buffer, length);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  return retVal;
}

int32_t bootloader_eraseWriteStorage(uint32_t slotId,
                                     uint32_t offset,
                                     uint8_t  *buffer,
                                     size_t   length)
{
  int32_t retVal;
  uint16_t flashPageSize;
  uint32_t storageStartAddr;
  uint32_t eraseOffset;
  uint32_t eraseLength;
  BootloaderStorageSlot_t storageSlot;
  BootloaderStorageInformation_t storageInfo;

  if (!bootloader_pointerValid(mainBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable->storage)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }

  bootloader_getStorageInfo(&storageInfo);
  flashPageSize = storageInfo.info->pageSize;
  if (flashPageSize == 0) {
    return BOOTLOADER_ERROR_STORAGE_INVALID_SLOT;
  }

  retVal = bootloader_getStorageSlotInfo(slotId, &storageSlot);
  if (retVal != BOOTLOADER_OK) {
    return retVal;
  }
  storageStartAddr = storageSlot.address;

  if (offset + length > storageSlot.length) {
    return BOOTLOADER_ERROR_STORAGE_INVALID_ADDRESS;
  }

  if (offset % flashPageSize) {
    // Erase from next page:
    eraseOffset = (offset & ~(flashPageSize - 1)) + flashPageSize;

    if ((offset + length) % flashPageSize) {
      // Example case for this if/else section:
      // 0    1    2    3
      // |----|----|----|
      //   ^          ^
      //   O          L
      eraseLength = ((offset + length) & ~(flashPageSize - 1)) + flashPageSize - eraseOffset;
    } else {
      // Example case for this if/else section:
      // 0    1    2    3
      // |----|----|----|
      //   ^            ^
      //   O            L
      eraseLength = length - (flashPageSize - (offset % flashPageSize));
    }
    eraseOffset = storageStartAddr + eraseOffset;
  } else {
    eraseOffset = storageStartAddr + offset;
    if (length % flashPageSize) {
      // Example case for this if/else section:
      // 0    1    2    3
      // |----|----|----|
      //      ^       ^
      //      O       L
      eraseLength = (length & ~(flashPageSize - 1)) + flashPageSize;
    } else {
      // Example case for this if/else section:
      // 0    1    2    3
      // |----|----|----|
      //      ^         ^
      //      O         L
      eraseLength = length;
    }
  }
  if (eraseLength != 0) {
    retVal = bootloader_eraseRawStorage(eraseOffset, eraseLength);
    if (retVal != BOOTLOADER_OK) {
      return retVal;
    }
  }

  retVal = bootloader_writeRawStorage(storageStartAddr + offset, buffer, length);
  if (retVal != BOOTLOADER_OK) {
    return retVal;
  }

  return BOOTLOADER_OK;
}

int32_t bootloader_eraseStorageSlot(uint32_t slotId)
{
  if (!bootloader_pointerValid(mainBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable->storage)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  int32_t retVal = mainBootloaderTable->storage->erase(slotId);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  return retVal;
}

int32_t bootloader_initChunkedEraseStorageSlot(uint32_t                slotId,
                                               BootloaderEraseStatus_t *eraseStat)
{
  int32_t retVal;
  BootloaderStorageInformation_t storageInfo;
  bootloader_getStorageInfo(&storageInfo);

  retVal = bootloader_getStorageSlotInfo(slotId, &eraseStat->storageSlotInfo);
  if (retVal != BOOTLOADER_OK) {
    return retVal;
  }

  eraseStat->currentPageAddr = eraseStat->storageSlotInfo.address;
  eraseStat->pageSize = storageInfo.info->pageSize;

  return BOOTLOADER_OK;
}

int32_t bootloader_chunkedEraseStorageSlot(BootloaderEraseStatus_t *eraseStat)
{
  int32_t retVal;
  if (eraseStat->currentPageAddr
      == (eraseStat->storageSlotInfo.address + eraseStat->storageSlotInfo.length)) {
    return BOOTLOADER_OK;
  }

  retVal = bootloader_eraseRawStorage(eraseStat->currentPageAddr, eraseStat->pageSize);
  if (retVal != BOOTLOADER_OK) {
    return retVal;
  }

  eraseStat->currentPageAddr += eraseStat->pageSize;
  if (eraseStat->currentPageAddr
      == (eraseStat->storageSlotInfo.address + eraseStat->storageSlotInfo.length)) {
    return BOOTLOADER_OK;
  }

  return BOOTLOADER_ERROR_STORAGE_CONTINUE;
}

int32_t bootloader_setImageToBootload(int32_t slotId)
{
  if (!bootloader_pointerValid(mainBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable->storage)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  int32_t retVal = mainBootloaderTable->storage->setImagesToBootload(&slotId, 1);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  return retVal;
}

int32_t bootloader_setImagesToBootload(int32_t *slotIds, size_t length)
{
  if (!bootloader_pointerValid(mainBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable->storage)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  int32_t retVal = mainBootloaderTable->storage->setImagesToBootload(slotIds, length);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  return retVal;
}

int32_t bootloader_getImagesToBootload(int32_t *slotIds, size_t length)
{
  if (!bootloader_pointerValid(mainBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable->storage)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  int32_t retVal = mainBootloaderTable->storage->getImagesToBootload(slotIds, length);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  return retVal;
}

int32_t bootloader_appendImageToBootloadList(int32_t slotId)
{
  if (!bootloader_pointerValid(mainBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable->storage)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  int32_t retVal = mainBootloaderTable->storage->appendImageToBootloadList(slotId);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  return retVal;
}

int32_t bootloader_initVerifyImage(uint32_t slotId,
                                   void     *context,
                                   size_t   contextSize)
{
  int32_t retVal;
  if (!bootloader_pointerValid(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_PARSE_STORAGE;
  }

  // Check that the bootloader has image verification capability
  if (mainBootloaderTable->storage == NULL) {
    return BOOTLOADER_ERROR_PARSE_STORAGE;
  }

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  retVal = mainBootloaderTable->storage->initParseImage(
    slotId,
    (BootloaderParserContext_t*)context,
    contextSize);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  return retVal;
}

int32_t bootloader_continueVerifyImage(void                       *context,
                                       BootloaderParserCallback_t metadataCallback)
{
  if (!bootloader_pointerValid(mainBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable->storage)) {
    return BOOTLOADER_ERROR_PARSE_STORAGE;
  }

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  int32_t retVal = mainBootloaderTable->storage->verifyImage(
    (BootloaderParserContext_t *)context,
    metadataCallback);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  return retVal;
}

int32_t bootloader_verifyImage(uint32_t                   slotId,
                               BootloaderParserCallback_t metadataCallback)
{
  int32_t retval;
  uint8_t context[BOOTLOADER_STORAGE_VERIFICATION_CONTEXT_SIZE];

  if (!bootloader_pointerValid(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_PARSE_STORAGE;
  }

  retval = bootloader_initVerifyImage(slotId,
                                      context,
                                      BOOTLOADER_STORAGE_VERIFICATION_CONTEXT_SIZE);

  if (retval != BOOTLOADER_OK) {
    return retval;
  }

  do {
    retval = bootloader_continueVerifyImage(context, metadataCallback);
  } while (retval == BOOTLOADER_ERROR_PARSE_CONTINUE);

  if (retval == BOOTLOADER_ERROR_PARSE_SUCCESS) {
    return BOOTLOADER_OK;
  } else {
    return retval;
  }
}

int32_t bootloader_getImageInfo(uint32_t          slotId,
                                ApplicationData_t *appInfo,
                                uint32_t          *bootloaderVersion)
{
  int32_t retval;
  uint8_t context[BOOTLOADER_STORAGE_VERIFICATION_CONTEXT_SIZE];

  if (!bootloader_pointerValid(mainBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable->storage)) {
    return BOOTLOADER_ERROR_PARSE_STORAGE;
  }

  // Check that the bootloader has image verification capability
  BTL_ASSERT(mainBootloaderTable->storage != NULL);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  retval = mainBootloaderTable->storage->initParseImage(
    slotId,
    (BootloaderParserContext_t *)context,
    BOOTLOADER_STORAGE_VERIFICATION_CONTEXT_SIZE);
  if (retval != BOOTLOADER_OK) {
#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
    bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE
    return retval;
  }
  retval = mainBootloaderTable->storage->getImageInfo(
    (BootloaderParserContext_t *)context,
    appInfo,
    bootloaderVersion);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  return retval;
}

bool bootloader_storageIsBusy(void)
{
  bool isBusy = false;

  if (!bootloader_pointerValid(mainBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable->storage)) {
    return true;
  }
#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  isBusy = mainBootloaderTable->storage->isBusy();

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  return isBusy;
}

int32_t bootloader_readRawStorage(uint32_t address,
                                  uint8_t  *buffer,
                                  size_t   length)
{
  if (!bootloader_pointerValid(mainBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable->storage)) {
    return BOOTLOADER_ERROR_INIT_STORAGE;
  }
#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  int32_t retVal = mainBootloaderTable->storage->readRaw(address, buffer, length);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  return retVal;
}

int32_t bootloader_writeRawStorage(uint32_t address,
                                   uint8_t  *buffer,
                                   size_t   length)
{
  if (!bootloader_pointerValid(mainBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable->storage)) {
    return BOOTLOADER_ERROR_INIT_STORAGE;
  }

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  int32_t retVal = mainBootloaderTable->storage->writeRaw(address, buffer, length);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  return retVal;
}

int32_t bootloader_getAllocatedDMAChannel(void)
{
  if (!bootloader_pointerValid(mainBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable->storage)) {
    return BOOTLOADER_ERROR_INIT_STORAGE;
  }

  BootloaderInformation_t info = { .type = SL_BOOTLOADER, .version = 0U, .capabilities = 0U };
  bootloader_getInfo(&info);

  if ((info.capabilities & BOOTLOADER_CAPABILITY_STORAGE) == 0u) {
    return BOOTLOADER_ERROR_INIT_STORAGE;
  }

  uint32_t blMajorVersion = ((info.version & BOOTLOADER_VERSION_MAJOR_MASK)
                             >> BOOTLOADER_VERSION_MAJOR_SHIFT);
  uint32_t blMinorVersion = ((info.version & BOOTLOADER_VERSION_MINOR_MASK)
                             >> BOOTLOADER_VERSION_MINOR_SHIFT);

  if ((blMajorVersion < 1UL) || (blMajorVersion == 1UL && blMinorVersion < 11UL)) {
    return BOOTLOADER_ERROR_INIT_STORAGE;
  }

  return mainBootloaderTable->storage->getDMAchannel();
}

int32_t bootloader_eraseRawStorage(uint32_t address,
                                   size_t   length)
{
  if (!bootloader_pointerValid(mainBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable->storage)) {
    return BOOTLOADER_ERROR_INIT_STORAGE;
  }

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  int32_t retVal = mainBootloaderTable->storage->eraseRaw(address, length);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  return retVal;
}
