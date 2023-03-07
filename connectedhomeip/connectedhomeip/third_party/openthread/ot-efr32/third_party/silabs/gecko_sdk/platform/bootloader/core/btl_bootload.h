/***************************************************************************//**
 * @file
 * @brief Bootloading functionality for the Silicon Labs bootloader
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
#ifndef BTL_BOOTLOAD_H
#define BTL_BOOTLOAD_H

#include "em_device.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/***************************************************************************//**
 * @addtogroup Core Bootloader Core
 * @{
 * @addtogroup Bootload
 * @brief Methods to verify and bootload application images
 * @details
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Get base address of the bootloader upgrade image.
 *
 * @return Returns the base address of bootloader upgrade image.
 ******************************************************************************/
uint32_t bootload_getUpgradeLocation(void);

/***************************************************************************//**
 * Get the version of the bootloader.
 *
 * @return Returns the version of the bootloader.
 ******************************************************************************/
uint32_t bootload_getBootloaderVersion(void);

/***************************************************************************//**
 * Get the version of the application.
 *
 * @param[out] version The retrieved application version
 *
 * @return Returns true if the version was retrieved successfully
 ******************************************************************************/
bool bootload_getApplicationVersion(uint32_t *version);

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
/***************************************************************************//**
 * Get the version of the SE.
 *
 * @param[out] version The retrieved SE version
 *
 * @return Returns true if the version was retrieved successfully.
 ******************************************************************************/
bool bootload_getSeVersion(uint32_t *version);
#endif // defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)

/***************************************************************************//**
 * Check application properties magic.
 *
 * @param appProperties   Pointer to ::ApplicationProperties_t
 *
 * @return True if the application properties magic is valid.
 ******************************************************************************/
bool bootload_checkApplicationPropertiesMagic(void *appProperties);

/***************************************************************************//**
 * Check application properties struct version.
 *
 * @param appProperties   Pointer to ::ApplicationProperties_t
 *
 * @return True if the application properties struct version is compatible
 *         with the bootloader.
 ******************************************************************************/
bool bootload_checkApplicationPropertiesVersion(void *appProperties);

/***************************************************************************//**
 * Verify the application image stored in the Flash memory starting at
 * the address startAddress.
 *
 * If secure boot is enforced, the function will only return true if the
 * cryptographic signature of the application is valid. Else, the application
 * is verified according to the signature type defined in the
 * ApplicationProperties_t structure embedded in the application. Silicon Labs
 * wireless stacks declare this structure. Applications, which are not
 * using a full wireless stack may need to instantiate the structure.
 *
 * Examples of results when secure boot is not enforced:
 * - App has no signature: Valid if initial stack pointer and program counter
 *                         have reasonable values
 * - App has CRC checksum: Valid if checksum is valid
 * - App has ECDSA signature: Valid if ECDSA signature is valid.
 *
 * When secure boot is enforced, only ECDSA-signed applications with
 * a valid signature are considered valid.
 *
 * @param[in] startAddress Starting address of the application
 *
 * @return True if the image is deemed valid
 ******************************************************************************/
bool bootload_verifyApplication(uint32_t startAddress);

/***************************************************************************//**
 * Bootloader upgrade callback implementation.
 *
 * @param offset          Offset of bootloader data (byte counter
 *                        incrementing from 0)
 * @param data            Raw bootloader data
 * @param length          Size in bytes of raw bootloader data.
 * @param context         A context variable defined by the implementation that
 *                        is implementing this callback.
 ******************************************************************************/
void bootload_bootloaderCallback(uint32_t offset,
                                 uint8_t  data[],
                                 size_t   length,
                                 void     *context);

/***************************************************************************//**
 * Image data callback implementation.
 *
 * @param address         Address (inside the raw image) the data starts at
 * @param data            Raw image data
 * @param length          Size in bytes of raw image data. Always constrained to
 *                        a multiple of four.
 * @param context         A context variable defined by the implementation that
 *                        is implementing this callback.
 ******************************************************************************/
void bootload_applicationCallback(uint32_t address,
                                  uint8_t  data[],
                                  size_t   length,
                                  void     *context);

/***************************************************************************//**
 * Perform a bootloader upgrade using the upgrade image present at
 * upgradeAddress with length size.
 *
 * If the bootloader upgrade process starts successfully, this function does
 * not return and execution will resume from the reset handler of the
 * upgraded bootloader.
 *
 * @param[in] upgradeAddress  The starting address of the upgrade image
 * @param[in] size            The length of the upgrade image in bytes
 *
 * @return  False if the bootloader upgrade process didn't start
 ******************************************************************************/
bool bootload_commitBootloaderUpgrade(uint32_t upgradeAddress, uint32_t size);

/***************************************************************************//**
 * Verify the application version for rollback protection.
 *
 * @param[in] appVersion                  Application version to be checked.
 * @param[in] checkRemainingAppUpgrades   Check remaining application upgrades.
 *
 * @return  True if the application version is higher or equal than
 *          the application versions seen.
 *          False if the application version is lower than
 *          the application versions seen.
 *          False if no remaining application upgrades are left when
 *          \p checkRemainingAppUpgrades is true.
 ******************************************************************************/
bool bootload_verifyApplicationVersion(uint32_t appVersion,
                                       bool checkRemainingAppUpgrades);

/***************************************************************************//**
 * Store the application version.
 *
 * @note
 *   Only the version of the verified application should be stored.
 *
 * @param startAddress    Start address of application.
 *
 * @return  True if application version is successfully stored.
 ******************************************************************************/
bool bootload_storeApplicationVersion(uint32_t startAddress);

/***************************************************************************//**
 * Count the total remaining number of application upgrades.
 *
 * @return remaining number of application upgrades.
 ******************************************************************************/
uint32_t bootload_remainingApplicationUpgrades(void);

/***************************************************************************//**
 * Store application version reset magic.
 *
 * @note Store application version reset magic to ensure that application
 *       versions are cleaned after a bootloader upgrade.
 ******************************************************************************/
void bootload_storeApplicationVersionResetMagic(void);

/***************************************************************************//**
 * Clean the application versions seen.
 *
 * @note The application versions are cleaned only if this is requested with a
 *       magic and the application version storage is not already empty.
 ******************************************************************************/
void bootload_removeStoredApplicationVersions(void);

/***************************************************************************//**
 * Get the application version storage capacity.
 *
 * @return Application version storage capacity.
 ******************************************************************************/
uint32_t bootload_getApplicationVersionStorageCapacity(void);

/***************************************************************************//**
 * Get the address of the application version storage buffer.
 *
 * @param index     Index of the application version storage buffer.
 *
 * @return Address of the application version storage buffer
 *         with the given index.
 ******************************************************************************/
uint32_t* bootload_getApplicationVersionStoragePtr(uint32_t index);

/***************************************************************************//**
 * Check whether the application contains a certificate.
 *
 * @param appProp     Pointer to ::ApplicationProperties_t of application.
 *
 * @return True if application contains a certificate.
 ******************************************************************************/
bool bootload_gotCertificate(void *appProp);

/***************************************************************************//**
 * Verify a certificate with a bootloader certificate.
 *
 * @param cert     Pointer to ::ApplicationCertificate_t.
 *
 * @return True if certificate is verified.
 ******************************************************************************/
bool bootload_verifyCertificate(void *cert);

/***************************************************************************//**
 * Verify the application certificate.
 *
 * @note
 *   This function will always return true if certificate support is
 *   not enabled. Also true if \p appProp does not contain any certificate and
 *   direct signed applications can be accepted.
 *
 * @param appProp     Pointer to ::ApplicationProperties_t of application.
 * @param gotCert     Boolean to store application certificate presence.
 *
 * @return True if application certificate is verified.
 ******************************************************************************/
bool bootload_verifyApplicationCertificate(void *appProp, void *gotCert);

#if defined(_MSC_PAGELOCK0_MASK)
/***************************************************************************//**
 * Lock the application area in Flash.
 *
 * @note
 *   Lock applied on the application area in flash is irreversible until
 *   the next reboot.
 *
 * @param startAddress    Start address of application to be locked.
 * @param endAddress      End address of application to be locked.
 *
 * @return  True if flash is successfully locked.
 ******************************************************************************/
bool bootload_lockApplicationArea(uint32_t startAddress, uint32_t endAddress);
#endif

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
/***************************************************************************//**
 * Check that an SE upgrade with a given version number is allowed to be
 * installed.
 *
 * This will only be true if the upgrade version is higher than the running
 * version.
 *
 * @param[in] upgradeVersion  The version of the SE upgrade image
 *
 * @return  True if the SE upgrade image should be installed
 ******************************************************************************/
bool bootload_checkSeUpgradeVersion(uint32_t upgradeVersion);

/***************************************************************************//**
 * Perform an SE upgrade using the upgrade image present at upgradeAddress.
 *
 * If the SE upgrade process starts successfully, this function does
 * not return and execution will resume from the reset handler after the SE
 * upgrade is complete.
 *
 * @param[in] upgradeAddress  The starting address of the upgrade image.
 *
 * @return  False if the SE upgrade process didn't start.
 ******************************************************************************/
bool bootload_commitSeUpgrade(uint32_t upgradeAddress);

#endif // defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)

/** @} addtogroup bootload */
/** @} addtogroup core */

#endif // BTL_BOOTLOAD_H
