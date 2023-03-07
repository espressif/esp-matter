/***************************************************************************//**
 * @file
 * @brief Configuration for bootloader
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
#ifndef BTL_CONFIG_H
#define BTL_CONFIG_H

//
// Bootloader Version
//
#define BOOTLOADER_VERSION_MAIN_MAJOR             2
#define BOOTLOADER_VERSION_MAIN_MINOR             1

#include "core/btl_util.h"
MISRAC_DISABLE
#include "em_device.h"
MISRAC_ENABLE

//
// Bootloader configuration
#if defined(BTL_CONFIG_FILE)
  #include BTL_CONFIG_FILE
#elif defined(BOOTLOADER_SECURE)
  #include "btl_core_s_cfg.h"
#else
  #if !defined(BOOTLOADER_NONSECURE)
    #include "btl_core_cfg.h"
  #endif
#endif

#ifndef BOOTLOADER_VERSION_MAIN_CUSTOMER
#define BOOTLOADER_VERSION_MAIN_CUSTOMER          1
#endif

#define BOOTLOADER_VERSION_MAIN (BOOTLOADER_VERSION_MAIN_MAJOR   << 24 \
                                 | BOOTLOADER_VERSION_MAIN_MINOR << 16 \
                                 | BOOTLOADER_VERSION_MAIN_CUSTOMER)

#ifdef BTL_SLOT_CONFIGURATION
  #include BTL_SLOT_CONFIGURATION
#endif

#ifndef BOOTLOADER_ENFORCE_SECURE_BOOT
#define BOOTLOADER_ENFORCE_SECURE_BOOT (true)
#endif

// Address of bootloader upgrade location
// Fixed upgrade address for Series 1 devices
#if defined(_SILICON_LABS_32B_SERIES_1) && !defined(BTL_UPGRADE_LOCATION_BASE)
#define BTL_UPGRADE_LOCATION_BASE  0x8000UL
#endif // _SILICON_LABS_32B_SERIES_1

#if !defined(BTL_UPGRADE_LOCATION) && !defined(BOOTLOADER_NONSECURE)
#define BTL_UPGRADE_LOCATION (FLASH_BASE + BTL_UPGRADE_LOCATION_BASE)
#endif

//
// Option validation
//

#if defined(APPLICATION_WRITE_DISABLE) && (APPLICATION_WRITE_DISABLE == 1)
  #if (BOOTLOADER_ENFORCE_SECURE_BOOT != 1)
    #error "Prevent write/erase of verified application is only available if the secure boot option is enabled"
  #endif
#endif // APPLICATION_WRITE_DISABLE

#if defined(BOOTLOADER_ROLLBACK_PROTECTION) && (BOOTLOADER_ROLLBACK_PROTECTION == 1)
  #if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80) || defined(MAIN_BOOTLOADER_IN_MAIN_FLASH)
    #error "Rollback protection not supported"
  #endif
#endif // defined(BOOTLOADER_ROLLBACK_PROTECTION)

#if defined(BOOTLOADER_SUPPORT_CERTIFICATES) && (BOOTLOADER_SUPPORT_CERTIFICATES == 1)
  #if !defined(_SILICON_LABS_32B_SERIES_2)
    #error "Certificate not supported"
  #endif
#endif // defined(BOOTLOADER_SUPPORT_CERTIFICATES)

#if defined(BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES) \
  && (BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES == 1)
  #if !defined(_SILICON_LABS_32B_SERIES_2)
    #error "Using the GBL decryption key stored in the Application Properties is not supported"
  #endif
#endif // defined(BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES)

// Certificate is enforced
#if defined(BOOTLOADER_SECURE) && (BOOTLOADER_ENFORCE_SECURE_BOOT == 1)
  #if defined(SEMAILBOX_PRESENT)
    #if (BOOTLOADER_FALLBACK_LEGACY_KEY == 1)
      #error "The legacy key can't be used"
    #endif
  #else
    #if (BOOTLOADER_SUPPORT_CERTIFICATES != 1)
      #error "The certificate support has to be enabled"
    #endif
  #endif // SEMAILBOX_PRESENT
#endif // BOOTLOADER_SECURE && BOOTLOADER_ENFORCE_SECURE_BOOT

#if defined(SEMAILBOX_PRESENT)
  #if (defined(BOOTLOADER_USE_SYMMETRIC_KEY_FROM_SE_STORAGE) && (BOOTLOADER_USE_SYMMETRIC_KEY_FROM_SE_STORAGE == 1)) \
  && (defined(BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES) && (BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES == 1))
    #error "Choose either to store the GBL decryption key in Secure Element or Application Properties struct"
  #endif // BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES && BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES
#endif // SEMAILBOX_PRESENT

#if defined(BOOTLOADER_SECURE)
  #if defined(SEMAILBOX_PRESENT)
    #if (BOOTLOADER_USE_SYMMETRIC_KEY_FROM_SE_STORAGE != 1) \
  && (BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES != 1)
      #error "The GBL decryption key has to be either embedded into the Application Properties struct or stored in Secure Element"
    #endif // BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES && BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES
  #else
    #if (BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES != 1)
      #error "The GBL decryption key has to be embedded into the Application Properties struct"
    #endif // BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES
  #endif // SEMAILBOX_PRESENT
#endif

#if defined(BTL_UPGRADE_LOCATION_BASE)

#if (BTL_UPGRADE_LOCATION_BASE % FLASH_PAGE_SIZE)
  #error "Invalid bootloader upgrade base address"
#endif

// BTL_UPGRADE_LOCATION_BASE is fixed on Series-1 devices.
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
// The upgrade location needs to fit upgrades of up to 48k
#define MINIMUM_REQUIRED_UPGRADE_SIZE (0x0000C000UL)

#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)
// The upgrade location needs to fit upgrades of up to 24k
#define MINIMUM_REQUIRED_UPGRADE_SIZE (0x00006000UL)

#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3)
// The upgrade location needs to fit upgrades of up to 96k
#define MINIMUM_REQUIRED_UPGRADE_SIZE (0x00018000UL)

#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_4)
// The upgrade location needs to fit upgrades of up to 96k
#define MINIMUM_REQUIRED_UPGRADE_SIZE (0x00018000UL)

#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
// The upgrade location needs to fit upgrades of up to 96k
#define MINIMUM_REQUIRED_UPGRADE_SIZE (0x00018000UL)

#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_7)
// The upgrade location needs to fit upgrades of up to 96k
#define MINIMUM_REQUIRED_UPGRADE_SIZE (0x00018000UL)

#else
  #if defined(_SILICON_LABS_32B_SERIES_2)
    #error "Undefined family"
  #endif

#endif // Device family

#if defined(_SILICON_LABS_32B_SERIES_2)

// BTL_UPGRADE_LOCATION_BASE sanity check
#if ((BTL_UPGRADE_LOCATION_BASE + MINIMUM_REQUIRED_UPGRADE_SIZE) > FLASH_SIZE)
  #error "Invalid bootloader upgrade base address"
#endif

#include "api/btl_interface.h"
#if (BTL_UPGRADE_LOCATION_BASE < BTL_MAIN_STAGE_MAX_SIZE)
  #error "Invalid bootloader upgrade base address"
#endif

#endif // defined(_SILICON_LABS_32B_SERIES_2)

#endif // defined(BTL_UPGRADE_LOCATION_BASE)

// Validate option BOOTLOADER_SE_UPGRADE_NO_STAGING
// (applicable to storage bootloaders only)
#if defined(BOOTLOADER_SE_UPGRADE_NO_STAGING) \
  && (BOOTLOADER_SE_UPGRADE_NO_STAGING == 1)
#if !defined(BOOTLOADER_SUPPORT_STORAGE)
#error "Error: BOOTLOADER_SE_UPGRADE_NO_STAGING is enabled, but no storage."
#endif // BOOTLOADER_SUPPORT_STORAGE

#if !defined(_SILICON_LABS_32B_SERIES_2)
#error "Error: BOOTLOADER_SE_UPGRADE_NO_STAGING is enabled, but this is only valid for devices with SE."
#endif
#endif // BOOTLOADER_SE_UPGRADE_NO_STAGING

// Validate option BTL_PARSER_SUPPORT_VERSION_DEPENDENCY_TAG
#if defined(BTL_PARSER_SUPPORT_VERSION_DEPENDENCY_TAG) \
  && !defined(_SILICON_LABS_32B_SERIES_2)
#error "Error: BTL_PARSER_SUPPORT_VERSION_DEPENDENCY_TAG is enabled, but this feature is only available for Series 2 devices."
#endif // BTL_PARSER_SUPPORT_VERSION_DEPENDENCY_TAG

#endif // BTL_CONFIG_H
