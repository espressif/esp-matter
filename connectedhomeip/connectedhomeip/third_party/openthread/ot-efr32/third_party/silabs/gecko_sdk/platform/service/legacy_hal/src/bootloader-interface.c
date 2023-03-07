/***************************************************************************//**
 * @file
 * @brief EFR32 common bootloader HAL functions
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

#include "stack/include/ember-types.h"

#include "bootloader-common.h"
#include "bootloader-interface.h"

#include "api/btl_interface.h"

#define LONG_TOKEN_BASE NULL

NO_STRIPPING const ApplicationProperties_t appProperties = {
  .magic = APPLICATION_PROPERTIES_MAGIC,
  .structVersion = APPLICATION_PROPERTIES_VERSION,
  .signatureType = APPLICATION_SIGNATURE_NONE,
  .signatureLocation = 0xFFFFFFFFU,
  .app = {
#if defined(EMBER_STACK_CONNECT)
    .type = APPLICATION_TYPE_FLEX,
#else
    .type = APPLICATION_TYPE_ZIGBEE,
#endif
    .version = CUSTOMER_APPLICATION_VERSION,
    .capabilities = APPLICATION_PROPERTIES_CAPABILITIES,
    .productId = CUSTOMER_APPLICATION_PRODUCT_ID
  },
  .longTokenSectionAddress = (uint8_t *)LONG_TOKEN_BASE,
};

#if !defined _SILICON_LABS_32B_SERIES_1_CONFIG_1
#define NO_BAT

static bool bootloaderIsCommonBootloader(void)
{
  return true;
}

#else

static bool bootloaderIsCommonBootloader(void)
{
  if (halBootloaderAddressTable->baseTable.type == BOOTLOADER_ADDRESS_TABLE_TYPE) {
    return false;
  } else {
    return true;
  }
}

#endif

/** @description Returns the bootloader type the application was
 *  built for.
 *
 * @return BL_TYPE_NULL, BL_TYPE_STANDALONE or BL_TYPE_APPLICATION
 */
BlBaseType halBootloaderGetType(void)
{
  // Important distinction:
  //   this returns what bootloader the app was built for
  #ifdef NULL_BTL
  BLDEBUG_PRINT("built for NULL bootloader\r\n");
  return BL_TYPE_NULL;
  #elif defined APP_BTL
  // Deprecated
  BLDEBUG_PRINT("built for APP bootloader\r\n");
  return BL_TYPE_APPLICATION;
  #elif defined SERIAL_UART_BTL
  // Deprecated
  BLDEBUG_PRINT("built for SERIAL UART standalone bootloader\r\n");
  return BL_TYPE_STANDALONE;
  #elif defined SERIAL_USB_BTL
  // Deprecated
  BLDEBUG_PRINT("built for SERIAL USB standalone bootloader\r\n");
  return BL_TYPE_STANDALONE;
  #elif defined SERIAL_OTA_BTL
  // Deprecated
  BLDEBUG_PRINT("built for SERIAL OTA standalone bootloader\r\n");
  return BL_TYPE_STANDALONE;
  #elif !defined _SILICON_LABS_32B_SERIES_1_CONFIG_1
  BLDEBUG_PRINT("built for Gecko info page bootloader\r\n");
  return BL_TYPE_STANDALONE;
  #else
  BLDEBUG_PRINT("built for generic bootloader\r\n");
  return BL_TYPE_BOOTLOADER;
  #endif
}

/** @description Returns the bootloader type present on the device.
 *
 * @return BL_TYPE_NULL, BL_TYPE_STANDALONE, BL_TYPE_APPLICATION or
 *         BL_TYPE_COMMON
 */
BlExtendedType halBootloaderGetInstalledType(void)
{
  // Important distinction:
  //   this returns what bootloader is actually present on the chip
  if (bootloaderIsCommonBootloader()) {
    // TODO: Assert that mainBootloaderTable pointer is sane
    if (mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_STORAGE) {
      return BL_EXT_TYPE_APP_UNKNOWN;
    } else if (mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_COMMUNICATION) {
      return BL_EXT_TYPE_STANDALONE_UNKNOWN;
    } else {
      return BL_EXT_TYPE_NULL;
    }
  } else {
#ifndef NO_BAT
    return halBootloaderAddressTable->bootloaderType;
#else
    return BL_EXT_TYPE_NULL;
#endif
  }
}

uint16_t halGetBootloaderVersion(void)
{
  if (bootloaderIsCommonBootloader()) {
    return mainBootloaderTable->header.version >> 16;
  } else {
#ifndef NO_BAT
    return halBootloaderAddressTable->bootloaderVersion;
#else
    return BOOTLOADER_INVALID_VERSION;
#endif
  }
}

void halGetExtendedBootloaderVersion(uint32_t* getEmberVersion, uint32_t* customerVersion)
{
  uint32_t ember = 0xFFFFFFFFU, customer = 0xFFFFFFFFU;

  if (bootloaderIsCommonBootloader()) {
    ember = mainBootloaderTable->header.version;
    customer = mainBootloaderTable->header.version;
  } else {
#ifndef NO_BAT
    // check BAT version to figure out how to create the emberVersion
    if (halBootloaderAddressTable->baseTable.version >= 0x0109) {
      // Newer BATs have a bootloaderVersion and bootloaderBuild that need to be combined
      ember = ((uint32_t)halBootloaderAddressTable->bootloaderVersion) << 16 | halBootloaderAddressTable->bootloaderBuild;
      customer = halBootloaderAddressTable->customerBootloaderVersion;
    } else {
      // Older BATs had these fields grouped together. Reformat them to be in the new format
      ember  = (((uint32_t)halBootloaderAddressTable->bootloaderVersion) << 16) & 0xFF000000;
      ember |= halBootloaderAddressTable->bootloaderVersion & 0x000000FF;
    }
#endif
  }

  // If the pointers aren't NULL copy the results over
  if (getEmberVersion != NULL) {
    *getEmberVersion = ember;
  }
  if (customerVersion != NULL) {
    *customerVersion = customer;
  }
}
