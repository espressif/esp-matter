/***************************************************************************//**
 * @file
 * @brief Configuration header of Bootloader Core for device_sdid_80
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
#ifndef BTL_CORE_CONFIG_H
#define BTL_CORE_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>
// <h> Bootloader Core Configuration

// <q BOOTLOADER_ENFORCE_SIGNED_UPGRADE> Require signed firmware upgrade files
// <i> Default: 0
// <i> Require that firmware upgrade files are authenticated using asymmetric signature verification.
#define BOOTLOADER_ENFORCE_SIGNED_UPGRADE                    0

// <q BOOTLOADER_ENFORCE_ENCRYPTED_UPGRADE> Require encrypted firmware upgrade files
// <i> Default: 0
// <i> Require that firmware upgrade files are encrypted.
#define BOOTLOADER_ENFORCE_ENCRYPTED_UPGRADE                    0

// BOOTLOADER_USE_SYMMETRIC_KEY_FROM_SE_STORAGE: Use symmetric key stored in Secure Element storage
// Default: 0
// Use the symmetric key stored in Secure Element storage for encryption and decryption.
#define BOOTLOADER_USE_SYMMETRIC_KEY_FROM_SE_STORAGE                    0

// <e BOOTLOADER_ENFORCE_SECURE_BOOT> Enable secure boot
// <i> Default: 0
// <i> Enforce signature verification on the application image in internal flash before every boot.
#define BOOTLOADER_ENFORCE_SECURE_BOOT                    0

// APPLICATION_WRITE_DISABLE: Prevent write/erase of verified application
// Default: 0
// Lock the application area in flash after the signature verification is passed. This option is only valid if secure boot is
// enabled. If end address of the signature does not touch a page boundary, the remaining flash memory in the page becomes unavailable.
#define APPLICATION_WRITE_DISABLE                    0

// BOOTLOADER_ROLLBACK_PROTECTION: Enable application rollback protection
// Default: 0
// Prevent applications from being downgraded. The application version can remain the same for upgrades. The
// bootloader will only allow applications to increment its version x times. Bootloader can be upgraded thenceforth
// to allow applications to increment its version again. This option is not applicable on the devices with Secure
// Element configured to perform full page lock.
#define BOOTLOADER_ROLLBACK_PROTECTION                    0

// BOOTLOADER_ROLLBACK_PROTECTION_MINIMUM_VERSION: Minimum application version allowed
// Default: 0
// The minimum version of the applications allowed for boot and upgrade. Prevent applications from being downgraded
// to the version below this.
#define BOOTLOADER_ROLLBACK_PROTECTION_MINIMUM_VERSION                    0

// BOOTLOADER_SUPPORT_CERTIFICATES: Enable certificate support
// Default: 0
// Enforce signature verification on the application image using the certificate of the bootloader image.
// To utilize certificate secure boot authentication, secure Element should be configured to authenticate
// the bootloader image by configuring (certificate based) secure boot option in Secure Element OTP. This
// option will also allow certificate based authentication of the GBL files.
#define BOOTLOADER_SUPPORT_CERTIFICATES                    0

// BOOTLOADER_REJECT_DIRECT_SIGNED_IMG: Reject direct signed images
// Default: 0
// On every boot, look for a certificate on application images and only accept the application images with a certificate.
#define BOOTLOADER_REJECT_DIRECT_SIGNED_IMG                    0

// </e>

// BOOTLOADER_WRITE_DISABLE: Prevent bootloader write/erase
// Default: 0
// Write lock bootloader area before entering application. The bootloader area in flash will be locked until the next reboot.
// This does not affect bootloader upgrades, but prevents the application running in main flash from disturbing the bootloader.
// On Series-1 devices this is only applicable to devices that use the bootloader area of flash (EFR32xG12 and later).
#define BOOTLOADER_WRITE_DISABLE                    0

// <q APPLICATION_VERIFICATION_SKIP_EM4_RST> Skip verification of application on EM4 reset
// <i> Default: 0
// <i> Skip verification of the application when the device wakes up from EM4 sleep.
#define APPLICATION_VERIFICATION_SKIP_EM4_RST                    0

// <o BOOTLOADER_VERSION_MAIN_CUSTOMER> Bootloader Version Main Customer
// <i> Default: 0
// <i> Bootloader Version Main Customer
#define BOOTLOADER_VERSION_MAIN_CUSTOMER                    1

// <e USE_CUSTOM_APP_SIZE> Use custom Bootloader Application Size
// <i> Default: 0
#define USE_CUSTOM_APP_SIZE                                     0

// <o CUSTOM_BTL_APP_SPACE_SIZE> Enter Bootloader App Space Size
// <i> Default: 0
// <i> Bootloader App Space Size
#define CUSTOM_BTL_APP_SPACE_SIZE                                  0
// </e>

#if USE_CUSTOM_APP_SIZE
#define BTL_APP_SPACE_SIZE                    CUSTOM_BTL_APP_SPACE_SIZE
#else
#define BTL_APP_SPACE_SIZE                    (FLASH_BASE + FLASH_SIZE) - BTL_APPLICATION_BASE
#endif

// </h>

// <<< end of configuration section >>>

#endif // BTL_CORE_CONFIG_H
