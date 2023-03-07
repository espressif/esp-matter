/***************************************************************************//**
 * @file
 * @brief Storage component for the Gecko Bootloader.
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
#include "btl_storage.h"
#ifndef BTL_CONFIG_FILE
#include "btl_storage_slot_cfg.h"
#include "btl_storage_cfg.h"
#endif

#include "core/btl_reset.h"
#include "core/btl_parse.h"
#include "core/btl_bootload.h"
#include "debug/btl_debug.h"

#if defined(CRYPTOACC_PRESENT)
#include "core/btl_util.h"
MISRAC_DISABLE
#include "em_se.h"
MISRAC_ENABLE
#endif

// --------------------------------
// Prototypes

static int32_t installImageFromSlot(int32_t slotId);

// --------------------------------
// Function implementations

uint32_t storage_getBaseAddress(void)
{
#ifdef BTL_STORAGE_BASE_ADDRESS
  return BTL_STORAGE_BASE_ADDRESS;
#else
  return 0;
#endif
}

int32_t storage_main(void)
{
#if BTL_STORAGE_BOOTLOAD_LIST_LENGTH == 1
  return installImageFromSlot(0);
#else
  int32_t ret;

  int32_t slotIds[BTL_STORAGE_BOOTLOAD_LIST_LENGTH] = { -1 };

  ret = storage_getBootloadList(slotIds, BTL_STORAGE_BOOTLOAD_LIST_LENGTH);
  if (ret != BOOTLOADER_OK) {
    BTL_DEBUG_PRINTLN("BI err");
    return ret;
  }

  // Attempt to bootload given images in sequence
  for (size_t id = 0; id < BTL_STORAGE_BOOTLOAD_LIST_LENGTH; id++) {
    if (slotIds[id] == -1) {
      // Invalid slot ID; try the next one
      continue;
    }

    ret = installImageFromSlot(slotIds[id]);
    if (ret == BOOTLOADER_OK) {
      break;
    }
  }

  return ret;
#endif
}

static int32_t installImageFromSlot(int32_t slotId)
{
  BootloaderParserContext_t parseContext;
  int32_t ret;

  BTL_DEBUG_PRINT("Slot: ");
  BTL_DEBUG_PRINT_WORD_HEX(slotId);
  BTL_DEBUG_PRINT_LF();

  // Get info about the image marked for bootloading
  storage_initParseSlot(slotId,
                        &parseContext,
                        sizeof(BootloaderParserContext_t));

  do {
    ret = storage_verifySlot(&parseContext, NULL);
  } while (ret == BOOTLOADER_ERROR_PARSE_CONTINUE);

  if (ret != BOOTLOADER_ERROR_PARSE_SUCCESS) {
    // Image in slot is non-existant or corrupt.
    // Continue to next image
    BTL_DEBUG_PRINTLN("Verify fail");
    return BOOTLOADER_ERROR_STORAGE_BOOTLOAD;
  }

#if (_SILICON_LABS_32B_SERIES == 1)
  // The upgrade address can be placed to not overlap with application images on Series-2.
  if ((parseContext.imageProperties.contents & BTL_IMAGE_CONTENT_BOOTLOADER)
      && !(parseContext.imageProperties.contents & BTL_IMAGE_CONTENT_APPLICATION)) {
    BTL_DEBUG_PRINTLN("BL upg with no app");
    return BOOTLOADER_ERROR_STORAGE_BOOTLOAD;
  }
#endif

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
  if ((parseContext.imageProperties.contents & BTL_IMAGE_CONTENT_SE)
      && bootload_checkSeUpgradeVersion(parseContext.imageProperties.seUpgradeVersion)) {
#if defined(CRYPTOACC_PRESENT)
    // Check whether the SE handled the SE_COMMAND_APPLY_SE_IMAGE command during boot
    // and see if the SE image was corrupted and verification failed.
    // The only reponse message of interest is SE_RESPONSE_INVALID_SIGNATURE.
    // All the other cases, bootloader will try to apply the upgrade image.
    if (SE_readExecutedCommand() == SE_COMMAND_APPLY_SE_IMAGE) {
      SE_Command_t seCommand = SE_COMMAND_DEFAULT(SE_COMMAND_APPLY_SE_IMAGE);
      SE_Response_t response = SE_ackCommand(&seCommand);
      BTL_DEBUG_PRINT("SE response: ");
      BTL_DEBUG_PRINT_WORD_HEX(response);
      BTL_DEBUG_PRINT_LF();
      if (response == SE_RESPONSE_INVALID_SIGNATURE) {
        return BOOTLOADER_ERROR_STORAGE_BOOTLOAD;
      }
    }
#endif
    if (storage_upgradeSeFromSlot(parseContext.slotId)) {
      // SE upgrade should be applied
#if defined(BOOTLOADER_SE_UPGRADE_NO_STAGING) \
      && (BOOTLOADER_SE_UPGRADE_NO_STAGING == 1)
      const BootloaderStorageSlot_t storageSlots[] = BTL_STORAGE_SLOTS;
      const uint32_t upgradeAddress = storageSlots[slotId].address
                                      + parseContext.parserContext.offsetOfSeUpgradeTag;
#else
      const uint32_t upgradeAddress = BTL_UPGRADE_LOCATION;
#endif
      if (!bootload_commitSeUpgrade(upgradeAddress)) {
        BTL_DEBUG_PRINTLN("SE upgrade commit fail");
        return BOOTLOADER_ERROR_STORAGE_BOOTLOAD;
      }
    } else {
      BTL_DEBUG_PRINTLN("SE upgrade parse fail");
    }
  }
#endif // SEMAILBOX_PRESENT || CRYPTOACC_PRESENT

  if ((parseContext.imageProperties.contents & BTL_IMAGE_CONTENT_BOOTLOADER)) {
    BTL_DEBUG_PRINT("BL upg ");
    BTL_DEBUG_PRINT_WORD_HEX(mainBootloaderTable->header.version);
    BTL_DEBUG_PRINT(" -> ");
    BTL_DEBUG_PRINT_WORD_HEX(parseContext.imageProperties.bootloaderVersion);
    BTL_DEBUG_PRINT_LF();
  }

  if ((parseContext.imageProperties.contents & BTL_IMAGE_CONTENT_BOOTLOADER)
      && (parseContext.imageProperties.bootloaderVersion
          > mainBootloaderTable->header.version)) {
    // This is a bootloader upgrade, and we also have an application
    // available for after the bootloader upgrade is complete
    if (storage_bootloadBootloaderFromSlot(
          parseContext.slotId,
          parseContext.imageProperties.bootloaderVersion)) {
      if (!bootload_commitBootloaderUpgrade(BTL_UPGRADE_LOCATION, parseContext.imageProperties.bootloaderUpgradeSize)) {
        // Bootloader upgrade failed; not a valid image
        BTL_DEBUG_PRINTLN("Btl upgrade commit fail");
      }
    } else {
      // Bootloader upgrade failed; not a valid image
      BTL_DEBUG_PRINTLN("Btl upgrade fail");
    }
    return BOOTLOADER_ERROR_STORAGE_BOOTLOAD;
  } else {
#if (defined(BOOTLOADER_SE_UPGRADE_NO_STAGING)  \
    && (BOOTLOADER_SE_UPGRADE_NO_STAGING == 0)) \
    || !defined(BOOTLOADER_SE_UPGRADE_NO_STAGING)
    // This should be an application upgrade
    if (!(parseContext.imageProperties.contents & BTL_IMAGE_CONTENT_APPLICATION)) {
      // ...but there is no app in the GBL
      BTL_DEBUG_PRINTLN("No app in slot");
      // Continue to next image
      return BOOTLOADER_ERROR_STORAGE_BOOTLOAD;
    }
#endif // BOOTLOADER_SE_UPGRADE_NO_STAGING

    if ((parseContext.imageProperties.contents & BTL_IMAGE_CONTENT_APPLICATION)
        && !storage_bootloadApplicationFromSlot(
          parseContext.slotId,
          parseContext.imageProperties.application.version)) {
      // App upgrade failed.
      BTL_DEBUG_PRINTLN("App upgrade fail");
      // Continue to next image
      return BOOTLOADER_ERROR_STORAGE_BOOTLOAD;
    }
    // Application was updated. Reboot into new image.
  }

  return BOOTLOADER_OK;
}
