/***************************************************************************//**
 * @file
 * @brief Cortex M3 application bootloader interface.
 *              Provides routines used by applications to access and verify
 *              the bootload image.
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

#include PLATFORM_HEADER
#include "hal/hal.h"

#include "stack/include/ember-types.h"
#include "stack/include/error.h"

#include "bootloader-common.h"
#include "bootloader-eeprom.h"
#include "bootloader-interface.h"
#include "bootloader-interface-app.h"

#include "api/btl_interface.h"
#include "api/btl_interface_storage.h"

// Default to using storage slot 0
static int32_t storageSlot = 0;

#ifdef _SILICON_LABS_32B_SERIES_1_CONFIG_1

static void verifyAppBlVersion(uint16_t version)
{
  assert(halBootloaderAddressTable->baseTable.type == BOOTLOADER_ADDRESS_TABLE_TYPE
         && BOOTLOADER_BASE_TYPE(halBootloaderAddressTable->bootloaderType) == BL_TYPE_APPLICATION
         && halBootloaderAddressTable->baseTable.version >= version);
}

static bool bootloaderIsCommonBootloader(void)
{
  if (halBootloaderAddressTable->baseTable.type == BOOTLOADER_ADDRESS_TABLE_TYPE) {
    return false;
  } else {
    return true;
  }
}

#else

static bool bootloaderIsCommonBootloader(void)
{
  return true;
}

#endif

static void verifyMainBootloaderVersion(uint32_t version)
{
  // Assert that the main bootloader table pointer points to main flash or bootloader flash
  assert(((uint32_t)mainBootloaderTable & 0xFFFF0000U) == FLASH_MEM_BASE
         || ((uint32_t)mainBootloaderTable & 0xFFFF0000U) == 0x0FE10000U);
  // Assert that the main bootloader table pointer points inside the bootloader
  assert(((uint32_t)mainBootloaderTable & 0x0000FFFFU) < 0x4000U);

  assert(mainBootloaderTable->header.version >= version);
}

uint8_t halAppBootloaderInit(void)
{
  if (bootloaderIsCommonBootloader()) {
    verifyMainBootloaderVersion(0x00000000);

    if (bootloader_init() == BOOTLOADER_OK) {
      return EEPROM_SUCCESS;
    } else {
      return EEPROM_ERR_INVALID_CHIP;
    }
  } else {
#ifdef _SILICON_LABS_32B_SERIES_1_CONFIG_1
    // version 0x0109 is the first bootloader built for an Energy Micro part
    verifyAppBlVersion(0x0109);

    return halBootloaderAddressTable->eepromInit();
#else
    return EEPROM_ERR_INVALID_CHIP;
#endif
  }
}

HalEepromInformationType fixedEepromInfo;

const HalEepromInformationType *halAppBootloaderInfo(void)
{
  if (bootloaderIsCommonBootloader()) {
    if (!(mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_STORAGE)) {
      return NULL;
    }
    BootloaderStorageInformation_t info;
    bootloader_getStorageInfo(&info);

    if (info.info == NULL) {
      return NULL;
    }

    // if partEraseMs fits into 16 bits don't change it
    if (info.info->partEraseMs <= 65535) {
      fixedEepromInfo.partEraseTime    = info.info->partEraseMs;
      fixedEepromInfo.capabilitiesMask = info.info->capabilitiesMask;
    }
    // if partEraseMs is too big to fit into 16 bits, convert to seconds (using 1024 because the
    // partEraseMs units are 1024Hz based) and set capabilities mask bit to indicate the value
    // is in seconds instead of milliseconds
    else {
      fixedEepromInfo.partEraseTime    = ((info.info->partEraseMs) / 1024);
      fixedEepromInfo.capabilitiesMask = info.info->capabilitiesMask | EEPROM_CAPABILITIES_PART_ERASE_SECONDS;
    }
    fixedEepromInfo.version           = info.info->version;
    fixedEepromInfo.pageEraseMs       = info.info->pageEraseMs;
    fixedEepromInfo.pageSize          = info.info->pageSize;
    fixedEepromInfo.partSize          = info.info->partSize;
    MEMCOPY((void*)&fixedEepromInfo.partDescription,
            (void*)&info.info->partDescription,
            sizeof(fixedEepromInfo.partDescription));
    fixedEepromInfo.wordSizeBytes     = info.info->wordSizeBytes;

    return &fixedEepromInfo;
  } else {
#ifdef _SILICON_LABS_32B_SERIES_1_CONFIG_1
    // For internal flash bootloaders we need to ask the app for the size of
    // internal storage since the bootloader doesn't know at build time, but only
    // if we have actually set an internal storage bottom in this app. If not, then
    // we return the default struct which has a size of 0.
    if ((halBootloaderAddressTable->bootloaderType == BL_EXT_TYPE_APP_LOCAL_STORAGE)
        && ((uint32_t)halAppAddressTable.internalStorageBottom > MFB_BOTTOM)) {
      HalEepromInformationType *temp = (HalEepromInformationType*)halBootloaderAddressTable->eepromInfo();
      MEMCOPY(&fixedEepromInfo, temp, sizeof(fixedEepromInfo));
      fixedEepromInfo.partSize = (MFB_TOP - (uint32_t)halAppAddressTable.internalStorageBottom + 1);
      return &fixedEepromInfo;
    } else {
      return (HalEepromInformationType *)halBootloaderAddressTable->eepromInfo();
    }
#else
    return NULL;
#endif
  }
}

void halAppBootloaderShutdown(void)
{
  if (bootloaderIsCommonBootloader()) {
    bootloader_deinit();
  } else {
#ifdef _SILICON_LABS_32B_SERIES_1_CONFIG_1
    halBootloaderAddressTable->eepromShutdown();
#endif
  }
}

EepromStateType eepromState;

#if EEPROM_PAGE_SIZE < EBL_MIN_TAG_SIZE
  #error EEPROM_PAGE_SIZE smaller than EBL_MIN_TAG_SIZE
#endif
#ifdef _SILICON_LABS_32B_SERIES_1_CONFIG_1
static uint8_t buff[EEPROM_PAGE_SIZE];
EblConfigType eblConfig;
#endif

__ALIGNED(4) uint8_t bootloaderValidationContext[BOOTLOADER_STORAGE_VERIFICATION_CONTEXT_SIZE];

void halAppBootloaderImageIsValidReset(void)
{
  if (bootloaderIsCommonBootloader()) {
    int32_t ret;

    // The bootloader needs to have storage in order to perform validation
    assert(mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_STORAGE);

    ret = bootloader_initVerifyImage(storageSlot,
                                     bootloaderValidationContext,
                                     BOOTLOADER_STORAGE_VERIFICATION_CONTEXT_SIZE);
    assert(ret == BOOTLOADER_OK);
  } else {
#ifdef _SILICON_LABS_32B_SERIES_1_CONFIG_1
    eepromState.address = EEPROM_IMAGE_START;
    eepromState.pages = 0;
    eepromState.pageBufFinger = 0;
    eepromState.pageBufLen = 0;
    halBootloaderAddressTable->eblProcessInit(&eblConfig,
                                              &eepromState,
                                              buff,
                                              EBL_MIN_TAG_SIZE,
                                              true);
#endif
  }
}

uint16_t halAppBootloaderImageIsValid(void)
{
  if (bootloaderIsCommonBootloader()) {
    uint32_t ret;

    if (!(mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_STORAGE)) {
      return 0;
    }

    ret = bootloader_continueVerifyImage(bootloaderValidationContext, NULL);

    if (ret == BOOTLOADER_ERROR_PARSE_CONTINUE) {
      return BL_IMAGE_IS_VALID_CONTINUE;
    } else if (ret == BOOTLOADER_ERROR_PARSE_SUCCESS) {
      // Cannot return number of pages, since we don't know.
      // It's at least 1 page, though.
      return 1;
    } else {
      // Error
      return 0;
    }
  } else {
#ifdef _SILICON_LABS_32B_SERIES_1_CONFIG_1
    BL_Status status;

    status = halBootloaderAddressTable->eblProcess(halBootloaderAddressTable->eblDataFuncs,
                                                   &eblConfig,
                                                   NULL);
    if (status == BL_EBL_CONTINUE) {
      return BL_IMAGE_IS_VALID_CONTINUE;
    } else if (status == BL_SUCCESS) {
      return eepromState.pages;
    } else {
      // error, return invalid
      return 0;
    }
#else
    return 0;
#endif
  }
}

EmberStatus halAppBootloaderInstallNewImage(void)
{
  if (bootloaderIsCommonBootloader()) {
    if (!(mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_STORAGE)) {
      return EMBER_ERR_FATAL;
    }

    if (bootloader_setImagesToBootload(&storageSlot, 1) == BOOTLOADER_OK) {
      // should not return
      bootloader_rebootAndInstall();
    }
  } else {
#ifdef _SILICON_LABS_32B_SERIES_1_CONFIG_1
    verifyAppBlVersion(0x0109);
    // should not return
    halInternalSysReset(RESET_BOOTLOADER_BOOTLOAD);
#endif
  }

  return EMBER_ERR_FATAL;
}

uint8_t halAppBootloaderWriteRawStorage(uint32_t address,
                                        const uint8_t *data,
                                        uint16_t len)
{
  if (bootloaderIsCommonBootloader()) {
    if (mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_STORAGE) {
      if (bootloader_writeRawStorage(address, (uint8_t *)data, len) == BOOTLOADER_OK) {
        return EEPROM_SUCCESS;
      }
    }
    return EEPROM_ERR;
  } else {
#ifdef _SILICON_LABS_32B_SERIES_1_CONFIG_1
    return halBootloaderAddressTable->eepromWrite(address, data, len);
#else
    return EEPROM_ERR;
#endif
  }
}

uint8_t halAppBootloaderReadRawStorage(uint32_t address, uint8_t *data, uint16_t len)
{
  if (bootloaderIsCommonBootloader()) {
    if (mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_STORAGE) {
      if (bootloader_readRawStorage(address, data, len) == BOOTLOADER_OK) {
        return EEPROM_SUCCESS;
      }
    }
    return EEPROM_ERR;
  } else {
#ifdef _SILICON_LABS_32B_SERIES_1_CONFIG_1
    return halBootloaderAddressTable->eepromRead(address, data, len);
#else
    return EEPROM_ERR;
#endif
  }
}

uint8_t halAppBootloaderEraseRawStorage(uint32_t address, uint32_t len)
{
  if (bootloaderIsCommonBootloader()) {
    if (mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_STORAGE) {
      if (bootloader_eraseRawStorage(address, len) == BOOTLOADER_OK) {
        return EEPROM_SUCCESS;
      }
    }
    return EEPROM_ERR;
  } else {
#ifdef _SILICON_LABS_32B_SERIES_1_CONFIG_1
    return halBootloaderAddressTable->eepromErase(address, len);
#else
    return EEPROM_ERR;
#endif
  }
}

bool halAppBootloaderStorageBusy(void)
{
  if (bootloaderIsCommonBootloader()) {
    if (mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_STORAGE) {
      return bootloader_storageIsBusy();
    } else {
      return true;
    }
  } else {
#ifdef _SILICON_LABS_32B_SERIES_1_CONFIG_1
    return halBootloaderAddressTable->eepromBusy();
#else
    return true;
#endif
  }
}

// halAppBootloaderGetVersion
//
// Returns the application bootloader version
//
uint16_t halAppBootloaderGetVersion(void)
{
  if (bootloaderIsCommonBootloader()) {
    return mainBootloaderTable->header.version >> 16;
  } else {
#ifdef _SILICON_LABS_32B_SERIES_1_CONFIG_1
    verifyAppBlVersion(0x0109);
    return halBootloaderAddressTable->bootloaderVersion;
#else
    return BOOTLOADER_INVALID_VERSION;
#endif
  }
}

// halAppBootloaderSupportsIbr
//
// Returns whether the bootloader suppoerts IBRs
//
bool halAppBootloaderSupportsIbr(void)
{
  if (bootloaderIsCommonBootloader()) {
    return false;
  } else {
#ifdef _SILICON_LABS_32B_SERIES_1_CONFIG_1
    return halBootloaderAddressTable->baseTable.type == BOOTLOADER_ADDRESS_TABLE_TYPE
           && BOOTLOADER_BASE_TYPE(halBootloaderAddressTable->bootloaderType) == BL_TYPE_APPLICATION
           && halBootloaderAddressTable->baseTable.version >= BAT_MIN_IBR_VERSION;
#else
    return false;
#endif
  }
}
