/***************************************************************************//**
 * @file
 * @brief This file handle how the application can configure and interact with the OTA
 * cluster.
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
#include "app/framework/plugin/ota-storage-common/ota-storage.h"
#include "app/framework/util/util.h"
#include "app/framework/util/common.h"
#ifdef HAL_CONFIG
#include "hal-config.h"
#include "ember-hal-config.h"
#endif
#include "ota-client-policy.h"

#ifdef UC_BUILD
 #if !defined(EMBER_TEST)
  #include "bootloader-interface-app.h"
  #endif
#include "sl_component_catalog.h"
#if (EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_EBL_VERIFICATION == 1)
#define EBL_VERIFICATION
#endif
#if (EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_INCLUDE_HARDWARE_VERSION == 1)
#define INCLUDE_HARDWARE_VERSION
#endif
#if (EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_DELETE_FAILED_DOWNLOADS == 1)
#define DELETE_FAILED_DOWNLOADS
#endif
#else // !UC_BUILD
#include "callback.h"
#ifdef EMBER_AF_PLUGIN_SLOT_MANAGER
#define SL_CATALOG_SLOT_MANAGER_PRESENT
#endif
#ifdef EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_EBL_VERIFICATION
#define EBL_VERIFICATION
#endif
#ifdef EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_INCLUDE_HARDWARE_VERSION
#define INCLUDE_HARDWARE_VERSION
#endif
#ifdef EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_DELETE_FAILED_DOWNLOADS
#define DELETE_FAILED_DOWNLOADS
#endif
#endif // UC_BUILD

#ifdef SL_CATALOG_SLOT_MANAGER_PRESENT
 #include "slot-manager.h"
 #define slotManagerBeginImageValidation(...) \
  (void)emberAfPluginSlotManagerImageIsValidReset(__VA_ARGS__)
 #define slotManagerBootSlot(...) (void)emberAfPluginSlotManagerBootSlot(__VA_ARGS__)
#else
 #define slotManagerBeginImageValidation(...)
 #define slotManagerBootSlot(...)
#endif // SL_CATALOG_SLOT_MANAGER_PRESENT

// Right now this sample policy only supports a single set of version
// information for the device, which will be supplied to the OTA cluster in
// order to query and download a new image when it is available.  This does not
// support multiple products with multiple download images.

#if defined(INCLUDE_HARDWARE_VERSION)
  #define HARDWARE_VERSION EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_HARDWARE_VERSION
#else
  #define HARDWARE_VERSION EMBER_AF_INVALID_HARDWARE_VERSION
#endif

//------------------------------------------------------------------------------
// Globals
EmberAfImageVerifyStatus slotManagerContinueImageValidation();

//------------------------------------------------------------------------------

void emberAfOtaClientVersionInfoCallback(EmberAfOtaImageId* currentImageInfo,
                                         uint16_t* hardwareVersion)
{
  // This callback is fired when a new query and download is initiated.
  // The application will fill in the currentImageInfo with their manufacturer
  // ID, image type ID, and current software version number to use in that
  // query. The deviceSpecificFileEui64 can be ignored.

  // It may be necessary to dynamically determine this data by talking to
  // another device, as is the case with a host talking to an NCP device.

  // The OTA client plugin will cache the data returned by this callback
  // and use it for the subsequent transaction, which could be a query
  // or a query and download.  Therefore it is possible to instruct the
  // OTA client cluster code to query about multiple images by returning
  // different values.

  MEMSET(currentImageInfo, 0, sizeof(EmberAfOtaImageId));
  currentImageInfo->manufacturerId  = EMBER_AF_MANUFACTURER_CODE;
  currentImageInfo->imageTypeId     = EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_IMAGE_TYPE_ID;
  currentImageInfo->firmwareVersion = EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_FIRMWARE_VERSION;

  if (hardwareVersion != NULL) {
    *hardwareVersion = HARDWARE_VERSION;
  }
}

EmberAfImageVerifyStatus emberAfOtaClientCustomVerifyCallback(bool newVerification,
                                                              const EmberAfOtaImageId* id)
{
  // Manufacturing specific checks can be made to the image in this function to
  // determine if it is valid.  This function is called AFTER cryptographic
  // checks have passed.  If the cryptographic checks failed, this function will
  // never be called.

  // The function shall return one of the following based on its own
  // verification process.
  // 1) EMBER_AF_IMAGE_GOOD - the image has passed all checks
  // 2) EMBER_AF_IMAGE_BAD  - the image is not valid
  // 3) EMBER_AF_IMAGE_VERIFY_IN_PROGRESS - the image is valid so far, but more
  //      checks are needed.  This callback shall be re-executed later to
  //      continue verification.  This allows other code in the framework to run.

  // Note that EBL verification is an SoC-only feature.

#if !defined(EZSP_HOST) && defined(EBL_VERIFICATION)
  uint16_t pages;
  EmberAfImageVerifyStatus status;
  uint32_t slot;

  // If we're using slots, we'll need to call a different set of APIs
  slot = emAfOtaStorageGetSlot();

  // For sleepies, we must re-initalize the EEPROM / bootloader
  // after each nap/hibernate.  This call will only re-initalize the EEPROM
  // if that is the case.
  emberAfEepromInitCallback();

  if (newVerification) {
    otaPrintln("Starting EBL verification");
    if (INVALID_SLOT != slot) {
      slotManagerBeginImageValidation(slot);
    } else {
      halAppBootloaderImageIsValidReset();
    }
  }

  if (INVALID_SLOT != slot) {
    status = slotManagerContinueImageValidation();
    if (status == EMBER_AF_IMAGE_VERIFY_IN_PROGRESS) {
      return status;
    } else if (status == EMBER_AF_IMAGE_BAD) {
      otaPrintln("EBL failed verification.");
      return status;
    }
  } else {
    pages = halAppBootloaderImageIsValid();
    if (pages == BL_IMAGE_IS_VALID_CONTINUE) {
      return EMBER_AF_IMAGE_VERIFY_IN_PROGRESS;
    } else if (pages == 0) {
      otaPrintln("EBL failed verification.");
      return EMBER_AF_IMAGE_BAD;
    }
  }
  otaPrintln("EBL passed verification.");
#endif

  return EMBER_AF_IMAGE_GOOD;
}

bool emberAfOtaClientDownloadCompleteCallback(EmberAfOtaDownloadResult result,
                                              const EmberAfOtaImageId* id)
{
  // At this point the image has been completely downloaded and cryptographic
  // checks (if applicable) have been performed.
  // Manufacturer verification callback has also been called and passed.

  if (result != EMBER_AF_OTA_DOWNLOAD_AND_VERIFY_SUCCESS) {
    emberAfOtaBootloadClusterPrintln("Download failed.");

#if defined(DELETE_FAILED_DOWNLOADS)
    if (result != EMBER_AF_OTA_ERASE_FAILED) {
      EmberAfOtaStorageStatus status = emberAfOtaStorageClearTempDataCallback();
      emberAfOtaBootloadClusterPrintln("Deleting failed download, status: 0x%X",
                                       status);
    }
#endif

    return true;   // return value is ignored
  }

  // If the client wants to abort for some reason then it can do so here
  // and return false.  Otherwise it should give the "go ahead" by returning
  // true.

  return true;
}

void emberAfOtaClientBootloadCallback(const EmberAfOtaImageId* id)
{
  // OTA Server has told us to bootload.
  // Any final preperation prior to the bootload should be done here.
  // It is assumed that the device will reset in most all cases.

  uint32_t offset;
  uint32_t endOffset;
  uint32_t slot;

  if (EMBER_AF_OTA_STORAGE_SUCCESS
      != emAfOtaStorageGetTagOffsetAndSize(id,
                                           OTA_TAG_UPGRADE_IMAGE,
                                           &offset,
                                           &endOffset)) {
    emberAfCoreFlush();
    otaPrintln("Image does not contain an Upgrade Image Tag (0x%2X). Skipping "
               "upgrade.", OTA_TAG_UPGRADE_IMAGE);
    return;
  }

  otaPrintln("Executing bootload callback.");
  (void) emberSerialWaitSend(APP_SERIAL);

  // If we're using slots, we'll need to use a different set of APIs
  slot = emAfOtaStorageGetSlot();

  // These routines will NOT return unless we failed to launch the bootloader.
  if (INVALID_SLOT != slot) {
    slotManagerBootSlot(slot);
  } else {
    emberAfOtaBootloadCallback(id, OTA_TAG_UPGRADE_IMAGE);
  }
}

EmberAfImageVerifyStatus slotManagerContinueImageValidation()
{
#ifdef SL_CATALOG_SLOT_MANAGER_PRESENT
  sl_status_t slotManagerResult = emberAfPluginSlotManagerImageIsValid();

  if (slotManagerResult == SL_STATUS_IN_PROGRESS) {
    return EMBER_AF_IMAGE_VERIFY_IN_PROGRESS;
  } else if (slotManagerResult != SL_STATUS_OK) {
    return EMBER_AF_IMAGE_BAD;
  } else {
    return EMBER_AF_IMAGE_GOOD;
  }
#else
  return EMBER_AF_IMAGE_BAD;
#endif // SL_CATALOG_SLOT_MANAGER_PRESENT
}
