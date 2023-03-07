/***************************************************************************//**
 * @file
 * @brief Storage component for Silicon Labs Bootloader (library part).
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

#include "api/btl_interface.h"
#include "core/btl_bootload.h"
#include "core/btl_parse.h"
#include "debug/btl_debug.h"
#include "parser/gbl/btl_gbl_parser.h"
#include "btl_storage.h"
#include "btl_storage_internal.h"
#include "bootloadinfo/btl_storage_bootloadinfo.h"
#include <string.h>

const BootloaderStorageFunctions_t storageFunctions = {
  // Version number
  BOOTLOADER_STORAGE_FUNCTIONS_VERSION,
  // Get information about the storage -- capabilities, configuration
  &storage_getInfo,
  // Get information about a storage slot -- size, location
  &storage_getSlotInfo,
  // Read bytes from slot into buffer
  &storage_readSlot,
  // Write bytes from buffer into slot
  &storage_writeSlot,
  // Erase an entire slot
  &storage_eraseSlot,
  // Mark a list of slots for bootload
  &storage_setBootloadList,
  // Get list of slots marked for bootload
  &storage_getBootloadList,
  // Append slot to list of slots marked for bootload
  &storage_appendBootloadList,
  // Start verification of image in a storage slot
  .initParseImage = &storage_initParseSlot,
  // Continue verification of image in a storage slot
  .verifyImage = &storage_verifySlot,
  // Get application and bootloader upgrade metadata from storage slot
  .getImageInfo = &storage_getSlotMetadata,
  // Whether storage is busy
  .isBusy = &storage_isBusy,

  // Read raw bytes from storage
  .readRaw = &storage_readRaw,
  // Write bytes to raw storage
  .writeRaw = &storage_writeRaw,
  // Erase storage
  .eraseRaw = &storage_eraseRaw,
  // Get configured DMA channel
  .getDMAchannel = &storage_getDMAchannel,
};

// --------------------------------
// Internal prototypes

static void advanceParser(BootloaderParserContext_t         *ctx,
                          const BootloaderParserCallbacks_t *callbacks);
static bool bootloadFromSlot(BootloaderParserContext_t         *context,
                             const BootloaderParserCallbacks_t *callbacks);
static void dummyCallback(uint32_t address,
                          uint8_t  *data,
                          size_t   length,
                          void     *context);

// --------------------------------
// Storage Info

void storage_getInfo(BootloaderStorageInformation_t *info)
{
  if (info == NULL) {
    return;
  }
  info->version = BOOTLOADER_STORAGE_INFO_VERSION;
  info->capabilities = 0UL;
  info->storageType = storageLayout.storageType;
  info->numStorageSlots = storageLayout.numSlots;

  // Querying detailed information about attached flash device
  info->flashInfo = getDeviceInfo();
  // Update pointer to flash info
  info->info = &(info->flashInfo);
}

// --------------------------------
// Internal parsing routine

static void advanceParser(BootloaderParserContext_t         *ctx,
                          const BootloaderParserCallbacks_t *callbacks)
{
  #if defined(BOOTLOADER_SUPPORT_INTERNAL_STORAGE) && defined(_SILICON_LABS_32B_SERIES_2)
  // Only activate the check if we have a bootloader blob that will be parsed
  if (callbacks->bootloaderCallback == bootload_bootloaderCallback) {
    uint32_t upgradeLocation = bootload_getUpgradeLocation();
    // Perform conservative check with the "worst" case upgrade size.
    uint32_t startAddr = storageLayout.slot[ctx->slotId].address + ctx->slotOffset;
    uint32_t endAddr = storageLayout.slot[ctx->slotId].address + ctx->slotOffset + BTL_STORAGE_READ_BUFFER_SIZE;
    if ((upgradeLocation >= startAddr)
        && (upgradeLocation < endAddr)) {
      ctx->errorCode = BOOTLOADER_ERROR_PARSER_OVERLAP;
      return;
    }

    if ((upgradeLocation < startAddr)
        && ((upgradeLocation + MINIMUM_REQUIRED_UPGRADE_SIZE) > startAddr)) {
      ctx->errorCode = BOOTLOADER_ERROR_PARSER_OVERLAP;
      return;
    }
  }
  #endif // BOOTLOADER_SUPPORT_INTERNAL_STORAGE

  uint8_t readBuffer[BTL_STORAGE_READ_BUFFER_SIZE];

  storage_readSlot(ctx->slotId,
                   ctx->slotOffset,
                   readBuffer,
                   BTL_STORAGE_READ_BUFFER_SIZE);
  ctx->errorCode = parser_parse(&(ctx->parserContext),
                                &(ctx->imageProperties),
                                readBuffer,
                                BTL_STORAGE_READ_BUFFER_SIZE,
                                callbacks);
  ctx->slotOffset += BTL_STORAGE_READ_BUFFER_SIZE;
}

// --------------------------------
// Slot API

int32_t storage_getSlotInfo(uint32_t slotId, BootloaderStorageSlot_t *slot)
{
  if (slot == NULL) {
    return BOOTLOADER_ERROR_STORAGE_INVALID_SLOT;
  }
  if (slotId >= storageLayout.numSlots) {
    slot->address = 0UL;
    slot->length = 0UL;
    return BOOTLOADER_ERROR_STORAGE_INVALID_SLOT;
  }

  slot->address = storageLayout.slot[slotId].address;
  slot->length = storageLayout.slot[slotId].length;

  return BOOTLOADER_OK;
}

int32_t storage_readSlot(uint32_t slotId,
                         uint32_t offset,
                         uint8_t  *buffer,
                         size_t   numBytes)
{
  // Ensure slot is valid
  if (slotId >= storageLayout.numSlots) {
    return BOOTLOADER_ERROR_STORAGE_INVALID_SLOT;
  }

  // Ensure address is within slot
  if (offset + numBytes > storageLayout.slot[slotId].length) {
    return BOOTLOADER_ERROR_STORAGE_INVALID_ADDRESS;
  }

  // Address range is valid; read data
  return storage_readRaw(storageLayout.slot[slotId].address + offset,
                         buffer,
                         numBytes);
}

int32_t storage_writeSlot(uint32_t slotId,
                          uint32_t offset,
                          uint8_t  *data,
                          size_t   numBytes)
{
  // Ensure slot is valid
  if (slotId >= storageLayout.numSlots) {
    return BOOTLOADER_ERROR_STORAGE_INVALID_SLOT;
  }

  // Ensure address is within slot
  if (offset + numBytes > storageLayout.slot[slotId].length) {
    return BOOTLOADER_ERROR_STORAGE_INVALID_ADDRESS;
  }

  return storage_writeRaw(storageLayout.slot[slotId].address + offset,
                          data,
                          numBytes);
}

int32_t storage_eraseSlot(uint32_t slotId)
{
  // Ensure slot is valid
  if (slotId >= storageLayout.numSlots) {
    return BOOTLOADER_ERROR_STORAGE_INVALID_SLOT;
  }

  return storage_eraseRaw(storageLayout.slot[slotId].address,
                          storageLayout.slot[slotId].length);
}

// --------------------------------
// Image Verification

int32_t storage_initParseSlot(uint32_t                  slotId,
                              BootloaderParserContext_t *context,
                              size_t                    contextSize
                              )
{
  int32_t retval;
  retval = core_initParser(context, contextSize);
  if (retval != BOOTLOADER_OK) {
    return retval;
  }

  BootloaderStorageSlot_t slot;
  retval = storage_getSlotInfo(slotId, &slot);
  if (retval != BOOTLOADER_OK) {
    return retval;
  }

  context->slotId = slotId;
  context->slotSize = slot.length;

  return BOOTLOADER_OK;
}

int32_t storage_verifySlot(BootloaderParserContext_t  *context,
                           BootloaderParserCallback_t metadataCallback)
{
  const BootloaderParserCallbacks_t parseCb = {
    NULL,
    NULL,
    metadataCallback,
    NULL
  };

  if ((context->errorCode == 0) && (context->slotOffset < context->slotSize)) {
    // There is still data left to parse
    advanceParser(context, &parseCb);

    if ((context->errorCode != BOOTLOADER_OK)
        && (context->errorCode != BOOTLOADER_ERROR_PARSER_EOF)) {
      return context->errorCode;
    } else {
      return BOOTLOADER_ERROR_PARSE_CONTINUE;
    }
  } else {
    // Parsing is complete, perform verification
    if ((context->imageProperties.imageCompleted)
        && (context->imageProperties.imageVerified)) {
      return BOOTLOADER_ERROR_PARSE_SUCCESS;
    } else {
      return BOOTLOADER_ERROR_PARSE_FAILED;
    }
  }
}

static void dummyCallback(uint32_t       address,
                          uint8_t        *data,
                          size_t         length,
                          void           *context)
{
  (void) address;
#ifdef __ICCARM__
// Suppress MISRA error that casting pointer to void is disallowed
#pragma diag_suppress=Pm139
#endif
  (void) data;
  (void) length;
#ifdef __ICCARM__
#pragma diag_default=Pm139
#endif

  bool *receivedData = (bool *)context;
  *receivedData = true;
}

int32_t storage_getSlotMetadata(BootloaderParserContext_t *context,
                                ApplicationData_t         *appInfo,
                                uint32_t                  *bootloaderVersion)
{
  bool receivedData = false;

  const BootloaderParserCallbacks_t parseCb = {
    .context = &receivedData,
    .applicationCallback = &dummyCallback,
    .metadataCallback = &dummyCallback,
    .bootloaderCallback = &dummyCallback,
  };

  // Default versions to zero
  *bootloaderVersion = 0UL;
  memset(appInfo, 0, sizeof(ApplicationData_t));

  while ((!receivedData)
         && (context->parserContext.internalState != GblParserStateCustomTag)
         && (context->errorCode == BOOTLOADER_OK)
         && (context->slotOffset < context->slotSize)) {
    // There is still data left to parse
    advanceParser(context, &parseCb);
  }

  if (context->imageProperties.contents & BTL_IMAGE_CONTENT_BOOTLOADER) {
    *bootloaderVersion = context->imageProperties.bootloaderVersion;
  }
  if (context->imageProperties.contents & BTL_IMAGE_CONTENT_APPLICATION) {
    memcpy(appInfo,
           &context->imageProperties.application,
           sizeof(ApplicationData_t));
  }

  if (receivedData || (context->imageProperties.contents)) {
    return BOOTLOADER_OK;
  } else {
    return BOOTLOADER_ERROR_STORAGE_NO_IMAGE;
  }
}

// --------------------------------
// Bootloading Functions

// Generic implementation of bootload from slot
static bool bootloadFromSlot(BootloaderParserContext_t         *context,
                             const BootloaderParserCallbacks_t *callbacks)
{
  parser_init(&(context->parserContext),
              &(context->decryptContext),
              &(context->authContext),
              PARSER_FLAG_PARSE_CUSTOM_TAGS);

  // Run through the image and flash it
  while ((0 == context->errorCode)
         && (context->slotOffset < context->slotSize)) {
    advanceParser(context, callbacks);
  }

  if (!context->imageProperties.imageCompleted) {
    BTL_DEBUG_PRINT("Err ");
    BTL_DEBUG_PRINT_WORD_HEX((uint32_t)context->errorCode);
    BTL_DEBUG_PRINT_LF();
  }

  if ((context->imageProperties.imageCompleted)
      && (context->imageProperties.imageVerified)) {
    return true;
  } else {
    return false;
  }
}

bool storage_upgradeSeFromSlot(uint32_t slotId)
{
  BootloaderParserContext_t parseCtx;
  int32_t ret = storage_initParseSlot(slotId,
                                      &parseCtx,
                                      sizeof(BootloaderParserContext_t));

  if (ret != BOOTLOADER_OK) {
    return false;
  }

  // Only apply SE
  parseCtx.imageProperties.instructions = BTL_IMAGE_INSTRUCTION_SE;

  const BootloaderParserCallbacks_t parseCb = {
    .context = NULL,
    .applicationCallback = NULL,
    .metadataCallback = NULL,
    .bootloaderCallback = bootload_bootloaderCallback
  };

  return bootloadFromSlot(&parseCtx, &parseCb);
}

// Bootload a bootloader image from slot
bool storage_bootloadBootloaderFromSlot(uint32_t slotId, uint32_t version)
{
  if (version <= mainBootloaderTable->header.version) {
    return false;
  }

  BootloaderParserContext_t parseCtx;
  int32_t ret = storage_initParseSlot(slotId,
                                      &parseCtx,
                                      sizeof(BootloaderParserContext_t));

  if (ret != BOOTLOADER_OK) {
    return false;
  }

  // Only apply bootloader
  parseCtx.imageProperties.instructions = BTL_IMAGE_INSTRUCTION_BOOTLOADER;

  const BootloaderParserCallbacks_t parseCb = {
    .context = NULL,
    .applicationCallback = NULL,
    .metadataCallback = NULL,
    .bootloaderCallback = bootload_bootloaderCallback
  };

  return bootloadFromSlot(&parseCtx, &parseCb);
}

// Bootload an application image from slot
bool storage_bootloadApplicationFromSlot(uint32_t slotId, uint32_t version)
{
  (void)version;
  BootloaderParserContext_t parseCtx;

  int32_t ret = storage_initParseSlot(slotId,
                                      &parseCtx,
                                      sizeof(BootloaderParserContext_t));

  if (ret != BOOTLOADER_OK) {
    return false;
  }

  // Only apply application
  parseCtx.imageProperties.instructions = BTL_IMAGE_INSTRUCTION_APPLICATION;

  const BootloaderParserCallbacks_t parseCb = {
    .context = NULL,
    .applicationCallback = bootload_applicationCallback,
    .metadataCallback = NULL, // Metadata callback not used when bootloading app
    .bootloaderCallback = NULL
  };

  return bootloadFromSlot(&parseCtx, &parseCb);
}
