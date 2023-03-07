/***************************************************************************//**
 * @file
 * @brief Configuration header of Bootloader Core for device_sdid_205
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

// <q BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES> Use symmetric key stored in Application Properties Struct
// <i> Default: 0
// <i> Use the symmetric key stored in Application Properties Struct for encryption and decryption.
#define BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES                0

// <e BOOTLOADER_ENFORCE_SECURE_BOOT> Enable secure boot
// <i> Default: 0
// <i> Enforce signature verification on the application image in internal flash before every boot.
#define BOOTLOADER_ENFORCE_SECURE_BOOT                    0

// <q APPLICATION_WRITE_DISABLE> Prevent write/erase of verified application
// <i> Default: 0
// <i> Lock the application area in flash after the signature verification is passed. This option is only valid if secure boot is
// <i> enabled. If end address of the signature does not touch a page boundary, the remaining flash memory in the page becomes unavailable.
#define APPLICATION_WRITE_DISABLE                    0

// <e BOOTLOADER_ROLLBACK_PROTECTION> Enable application rollback protection
// <i> Default: 0
// <i> Prevent applications from being downgraded. The application version can remain the same for upgrades. The
// <i> bootloader will only allow applications to increment its version x times. Bootloader can be upgraded thenceforth
// <i> to allow applications to increment its version again. This option is not applicable on the devices with Secure
// <i> Element configured to perform full page lock.
#define BOOTLOADER_ROLLBACK_PROTECTION                    0

// <o BOOTLOADER_ROLLBACK_PROTECTION_MINIMUM_VERSION> Minimum application version allowed
// <i> Default: 0
// <i> The minimum version of the applications allowed for boot and upgrade. Prevent applications from being downgraded
// <i> to the version below this.
#define BOOTLOADER_ROLLBACK_PROTECTION_MINIMUM_VERSION                    0
// </e>

// <e BOOTLOADER_SUPPORT_CERTIFICATES> Enable certificate support
// <i> Default: 0
// <i> Enforce signature verification on the application image using the certificate of the bootloader image.
// <i> To utilize certificate secure boot authentication, secure Element should be configured to authenticate
// <i> the bootloader image by configuring (certificate based) secure boot option in Secure Element OTP. This
// <i> option will also allow certificate based authentication of the GBL files.
#define BOOTLOADER_SUPPORT_CERTIFICATES                    0

// <q BOOTLOADER_REJECT_DIRECT_SIGNED_IMG> Reject direct signed images
// <i> Default: 0
// <i> On every boot, look for a certificate on application images and only accept the application images with a certificate.
#define BOOTLOADER_REJECT_DIRECT_SIGNED_IMG                    0
// </e>

// </e>

// BOOTLOADER_FALLBACK_LEGACY_KEY: Allow use of public key from manufacturing token storage
// Default: 1
// Allow use of the public key from the manufacturing token storage for signature verification if the public key is not stored in Secure Element.
#define BOOTLOADER_FALLBACK_LEGACY_KEY                    1

// <q BOOTLOADER_WRITE_DISABLE> Prevent bootloader write/erase
// <i> Default: 0
// <i> Write lock bootloader area before entering application. The bootloader area in flash will be locked until the next reboot.
// <i> This does not affect bootloader upgrades, but prevents the application running in main flash from disturbing the bootloader.
// <i> On Series-1 devices this is only applicable to devices that use the bootloader area of flash (EFR32xG12 and later).
#define BOOTLOADER_WRITE_DISABLE                    0

// <o BTL_UPGRADE_LOCATION_BASE> Base address of bootloader upgrade image
// <i> Default: 0x8000
// <i> At the upgrade stage of the bootloader, the running main bootloader extracts the upgrade image from the GBL file,
// <i> and stores it in internal flash at the address chosen. The address need to be a multiple of the page size.
#define BTL_UPGRADE_LOCATION_BASE  0x18000UL

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
