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
#include "config/btl_config.h"

#include "btl_bootload.h"
#include "btl_reset.h"
#include "btl_util.h"

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
MISRAC_DISABLE
#include "em_se.h"
MISRAC_ENABLE
#endif

// Interface
#include "api/btl_interface.h"
#include "api/application_properties.h"

// Image parser
#include "parser/gbl/btl_gbl_parser.h"
#include "parser/gbl/btl_gbl_format.h"

// Security algorithms
#include "security/btl_security_types.h"
#include "security/btl_security_ecdsa.h"
#include "security/btl_crc32.h"
#include "security/btl_security_tokens.h"

// Flashing
#include "core/flash/btl_internal_flash.h"

// Debug
#include "debug/btl_debug.h"

// Get memcpy
#include <string.h>

#ifdef __ICCARM__
// Silence MISRA warning disallowing statements without side effects
#pragma diag_suppress=Pm049
// Silence MISRA warning disallowing access to volatile object in right-hand operand of || operator
#pragma diag_suppress=Pm026
#endif

//
// Option validation
//
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

// --------------------------------
// Local type declarations
static bool bootload_verifySecureBoot(uint32_t startAddress);

static void flashData(uint32_t address,
                      uint8_t  data[],
                      size_t   length);

static bool getSignatureX(ApplicationProperties_t *appProperties,
                          uint32_t *appSignatureX);

#if defined(BOOTLOADER_ROLLBACK_PROTECTION) && (BOOTLOADER_ROLLBACK_PROTECTION == 1)
static bool checkResetMagic(void);
static bool checkMaxVersionMagic(void);
static uint32_t getHighestApplicationVersionSeen(void);
#endif

// --------------------------------
// Defines

#if defined(BOOTLOADER_ROLLBACK_PROTECTION) && (BOOTLOADER_ROLLBACK_PROTECTION == 1)
#define SL_GBL_APPLICATION_VERSION_STORAGE_CAPACITY 6UL
#define SL_GBL_APPLICATION_VERSION_MAX_MAGIC        0x1234DCBAUL
#define SL_GBL_APPLICATION_VERSION_RESET_MAGIC      0x5839FBACUL
#define SL_GBL_UINT32_MAX_NUMBER                    0xFFFFFFFFUL
#endif

// --------------------------------
// Local functions

#if defined(BOOTLOADER_ROLLBACK_PROTECTION) && (BOOTLOADER_ROLLBACK_PROTECTION == 1)
static bool checkMaxVersionMagic(void)
{
  uint32_t *versionMaxMagicPtr = bootload_getApplicationVersionStoragePtr(SL_GBL_APPLICATION_VERSION_STORAGE_CAPACITY);
  if (*versionMaxMagicPtr == SL_GBL_APPLICATION_VERSION_MAX_MAGIC) {
    return true;
  }
  return false;
}

static bool checkResetMagic(void)
{
  uint32_t *versionResetMagicPtr = bootload_getApplicationVersionStoragePtr(SL_GBL_APPLICATION_VERSION_STORAGE_CAPACITY + 1UL);
  if (*versionResetMagicPtr == SL_GBL_APPLICATION_VERSION_RESET_MAGIC) {
    return true;
  }
  return false;
}

static uint32_t getHighestApplicationVersionSeen(void)
{
  uint32_t *appVersionStoragePtr = bootload_getApplicationVersionStoragePtr(SL_GBL_APPLICATION_VERSION_STORAGE_CAPACITY);
  if (checkMaxVersionMagic()) {
    return SL_GBL_UINT32_MAX_NUMBER;
  }

  for (uint32_t i = 0UL; i < SL_GBL_APPLICATION_VERSION_STORAGE_CAPACITY; i++) {
    ++appVersionStoragePtr;
    if (*appVersionStoragePtr != SL_GBL_UINT32_MAX_NUMBER) {
      return *appVersionStoragePtr;
    }
  }

  return PARSER_APPLICATION_MINIMUM_VERSION_VALID;
}
#endif

static void flashData(uint32_t address,
                      uint8_t  data[],
                      size_t   length)
{
  const uint32_t pageSize = (uint32_t)FLASH_PAGE_SIZE;

  // Erase the page if write starts at a page boundary
  if (address % pageSize == 0UL) {
    flash_erasePage(address);
  }

  // Erase all pages that start inside the write range
  for (uint32_t pageAddress = (address + pageSize) & ~(pageSize - 1UL);
       pageAddress < (address + length);
       pageAddress += pageSize) {
    flash_erasePage(pageAddress);
  }

  BTL_DEBUG_PRINT("F ");
  BTL_DEBUG_PRINT_WORD_HEX(length);
  BTL_DEBUG_PRINT(" to ");
  BTL_DEBUG_PRINT_WORD_HEX(address);
  BTL_DEBUG_PRINT_LF();

  flash_writeBuffer_dma(address, data, length, SL_GBL_MSC_LDMA_CHANNEL);
}

static bool getSignatureX(ApplicationProperties_t *appProperties, uint32_t *appSignatureX)
{
  // Check if app properties struct or legacy direct signature pointer
  if (bootload_checkApplicationPropertiesMagic(appProperties)) {
    if (appProperties->signatureType != APPLICATION_SIGNATURE_ECDSA_P256) {
      // Application signature isn't ECDSA, fail early
      BTL_DEBUG_PRINTLN("Wrong s type");
      return false;
    }
    // Compatibility check of the application properties struct.
    if (!bootload_checkApplicationPropertiesVersion(appProperties)) {
      return false;
    }
    *appSignatureX = appProperties->signatureLocation;
  } else {
    *appSignatureX = (uint32_t)appProperties;
  }
  return true;
}

static bool bootload_verifySecureBoot(uint32_t startAddress)
{
  volatile int32_t retVal = BOOTLOADER_ERROR_SECURITY_REJECTED;
  Sha256Context_t shaState;

  BareBootTable_t *appStart = (BareBootTable_t *)startAddress;
  uint32_t appProps = (uint32_t)appStart->signature;
  uint32_t appSignatureX, appSignatureY;
  ApplicationProperties_t *appProperties =
    (ApplicationProperties_t *)(appProps);

  if (!bootload_checkApplicationPropertiesMagic(appProperties)) {
    return false;
  }
  if (!bootload_checkApplicationPropertiesVersion(appProperties)) {
    return false;
  }

#if !defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80)
  if (PARSER_REQUIRE_ANTI_ROLLBACK_PROTECTION) {
    if (!bootload_verifyApplicationVersion(appProperties->app.version, true)) {
      return false;
    }
  }
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
  // Access word 13 to read sl_app_properties of the bootloader.
  ApplicationProperties_t *blProperties =
    (ApplicationProperties_t *)(*(uint32_t *)(BTL_MAIN_STAGE_BASE + 52UL));
  if (!bootload_checkApplicationPropertiesMagic(blProperties)) {
    return false;
  }
#if !defined(MAIN_BOOTLOADER_TEST)
  if ((uint32_t)blProperties > ((uint32_t)mainBootloaderTable->startOfAppSpace - sizeof(ApplicationProperties_t))) {
    // Make sure that this pointer is within the bootloader space.
    return false;
  }
#endif

  bool gotCert = false;
  if (!bootload_verifyApplicationCertificate(appProperties, &gotCert)) {
    return false;
  }
#endif

  if (!getSignatureX(appProperties, &appSignatureX)) {
    return false;
  }

  // Check that signature is in application flash
  if ((appSignatureX < (uint32_t)(mainBootloaderTable->startOfAppSpace))
      || (appSignatureX < startAddress)
      || (appSignatureX > (FLASH_BASE + FLASH_SIZE))) {
    BTL_DEBUG_PRINTLN("No sign");
    return false;
  }

  // SHA-256 of the entire application (startAddress until signature)
  btl_initSha256(&shaState);
  btl_updateSha256(&shaState,
                   (const uint8_t*)startAddress,
                   appSignatureX - startAddress);
  btl_finalizeSha256(&shaState);

  appSignatureY = appSignatureX + 32UL;
#if defined(_SILICON_LABS_32B_SERIES_2)
  if (PARSER_REQUIRE_CERTIFICATE_AUTHENTICITY) {
    if (gotCert) {
      // Application certificate is already verified to be valid at this point.
      // Authenticate the application using the certificate of the application.
      retVal = btl_verifyEcdsaP256r1(shaState.sha,
                                     (uint8_t*)appSignatureX,
                                     (uint8_t*)appSignatureY,
                                     &(appProperties->cert->key[0]),
                                     &(appProperties->cert->key[32]));
    } else {
      // Application is directly signed + cert on app is not required.
      // Authenticate the application using the certificate of the bootloader.
      retVal = btl_verifyEcdsaP256r1(shaState.sha,
                                     (uint8_t*)appSignatureX,
                                     (uint8_t*)appSignatureY,
                                     &(blProperties->cert->key[0]),
                                     &(blProperties->cert->key[32]));
    }
  } else {
    // Use "lock bits" key or platform key to authenticate the application.
    retVal = btl_verifyEcdsaP256r1(shaState.sha,
                                   (uint8_t*)appSignatureX,
                                   (uint8_t*)appSignatureY,
                                   btl_getSignedBootloaderKeyXPtr(),
                                   btl_getSignedBootloaderKeyYPtr());
  }
#else
  retVal = btl_verifyEcdsaP256r1(shaState.sha,
                                 (uint8_t*)appSignatureX,
                                 (uint8_t*)appSignatureY,
                                 btl_getSignedBootloaderKeyXPtr(),
                                 btl_getSignedBootloaderKeyYPtr());
#endif
  if (retVal == BOOTLOADER_OK) {
    return true;
  } else {
    BTL_DEBUG_PRINTLN("Inv sign");
    return false;
  }
}

// --------------------------------
// Global functions

// Callbacks
SL_WEAK void bootload_applicationCallback(uint32_t address,
                                          uint8_t  data[],
                                          size_t   length,
                                          void     *context)
{
  (void) context;
  // Check if addresses to write to are within writeable space
  if ((address < (uint32_t)(mainBootloaderTable->startOfAppSpace))
      || ((address + length)
          > (uint32_t)(mainBootloaderTable->endOfAppSpace))) {
    BTL_DEBUG_PRINT("OOB 0x");
    BTL_DEBUG_PRINT_WORD_HEX(address);
    BTL_DEBUG_PRINT_LF();
    return;
  }

  flashData(address, data, length);
}

SL_WEAK void bootload_bootloaderCallback(uint32_t offset,
                                         uint8_t  data[],
                                         size_t   length,
                                         void     *context)
{
  (void) context;

#if defined(BOOTLOADER_HAS_FIRST_STAGE)
  if (firstBootloaderTable->header.type != BOOTLOADER_MAGIC_FIRST_STAGE) {
    // No first stage present
    return;
  }
#endif

  // Do not allow overwriting the last page of main flash if it coincides with
  // the "lock bits" page.
#if defined(LOCKBITS_BASE) \
  && (LOCKBITS_BASE != (FLASH_BASE + FLASH_SIZE - FLASH_PAGE_SIZE))
  const uint32_t max_address = FLASH_BASE + FLASH_SIZE;
#else
  const uint32_t max_address = FLASH_BASE + FLASH_SIZE - FLASH_PAGE_SIZE;
#endif
  volatile uint32_t address = BTL_UPGRADE_LOCATION + offset;

  // OOB checks
  // i) if NOT (BTL_UPGRADE_LOCATION <= address < max_address),
  //    with integer overflow check for address
  //    Skip offset > (uint32_t) (UINT32_MAX - BTL_UPGRADE_LOCATION)
  //     if BTL_UPGRADE_LOCATION is zero
  #if (BTL_UPGRADE_LOCATION != 0UL)
  if ((offset > (uint32_t) (UINT32_MAX - BTL_UPGRADE_LOCATION))
      || (address >= max_address))
  #else
  if (address >= max_address)
  #endif
  {
    BTL_DEBUG_PRINT("OOB, address not in allowed range; (address) 0x");
    BTL_DEBUG_PRINT_WORD_HEX(address);
    BTL_DEBUG_PRINT_LF();
    return;
  }

  // ii) Semantically equivalent to (address + length > max_address),
  //     but without the risk of integer overflow (or underflow, because of (i))
  if (length > (uint32_t) (max_address - address)) {
    BTL_DEBUG_PRINT("OOB, length too large; (address) 0x");
    BTL_DEBUG_PRINT_WORD_HEX(address);
    BTL_DEBUG_PRINT(", (length) 0x");
    BTL_DEBUG_PRINT_WORD_HEX(length);
    BTL_DEBUG_PRINT_LF();
    return;
  }

  // Erase first page of app if this is the first write and
  // if the bootloader upgrade location overlaps with the application.
  // This ensures that application is not misinterpreted as valid when
  // bootloader upgrade has started
  if (offset == 0UL) {
    if (BTL_UPGRADE_LOCATION < (uint32_t)(mainBootloaderTable->endOfAppSpace)) {
      flash_erasePage((uint32_t)(mainBootloaderTable->startOfAppSpace));
    }
  }

  flashData(address, data, length);
}

bool bootload_checkApplicationPropertiesMagic(void *appProperties)
{
  if ((appProperties == NULL) || ((uint32_t) appProperties == 0xFFFFFFFFUL)) {
    return false;
  }

#if (FLASH_BASE > 0x0UL)
  if ((uint32_t)appProperties < FLASH_BASE) {
    return false;
  }
#endif

  uint8_t magicRev[16U] = APPLICATION_PROPERTIES_REVERSED;
  uint8_t *magic = (uint8_t *)appProperties;

  for (size_t i = 0U; i < 16U; i++) {
    if (magicRev[15U - i] != magic[i]) {
      return false;
    }
  }

  return true;
}

bool bootload_checkApplicationPropertiesVersion(void *appProperties)
{
  ApplicationProperties_t *appProp = (ApplicationProperties_t *)appProperties;
  // Compatibility check of the application properties struct.
  if (((appProp->structVersion & APPLICATION_PROPERTIES_VERSION_MAJOR_MASK)
       >> APPLICATION_PROPERTIES_VERSION_MAJOR_SHIFT)
      > (uint32_t)APPLICATION_PROPERTIES_VERSION_MAJOR) {
    return false;
  }
  return true;
}

bool bootload_verifyApplication(uint32_t startAddress)
{
  BareBootTable_t *appStart = (BareBootTable_t *)startAddress;
  uint32_t appSp = (uint32_t)appStart->stackTop;
  uint32_t appPc = (uint32_t)appStart->resetVector;
  uint32_t appProps = (uint32_t)appStart->signature;

  // Check that SP points to RAM
  if ((appSp < SRAM_BASE) || (appSp > (SRAM_BASE + SRAM_SIZE))) {
    BTL_DEBUG_PRINTLN("SP n/i RAM");
    return false;
  }

  // Check that PC points to application flash
  if ((appPc < (uint32_t)mainBootloaderTable->startOfAppSpace)
      || (appPc > (FLASH_BASE + FLASH_SIZE))) {
    BTL_DEBUG_PRINTLN("PC n/i flash");
    return false;
  }

  ApplicationProperties_t *appProperties =
    (ApplicationProperties_t *)(appProps);

  // Application properties pointer can take on many possible values:
  //
  // 0xFFFFFFFF - Likely unused part of AAT
  // [FLASH_BASE, FLASH_SIZE] - Likely one of three things:
  //   - Pointer to Reset_Handler (app without app properties)
  //   - Pointer to ApplicationProperties_t
  //   - Pointer to ECDSA signature (legacy direct solution)

  if ((appProps < ((uint32_t)mainBootloaderTable->startOfAppSpace + 64UL))
      || (appProps > (FLASH_BASE + FLASH_SIZE))) {
    // Application properties pointer does not point inside flash (is not valid)
    if (BOOTLOADER_ENFORCE_SECURE_BOOT) {
      // Secure boot is enforced. Either an app properties struct or a direct
      // pointer to the signature is required. An address outside main flash
      // is not valid for either case.
      BTL_DEBUG_PRINTLN("AP n/i flash");
      return false;
    } else {
      // Secure boot is not enforced, we have to assume app is valid
      BTL_DEBUG_PRINTLN("No SB, assume valid");
      return true;
    }
  } else if (BOOTLOADER_ENFORCE_SECURE_BOOT) {
    // Secure boot is enforced, attempt to verify secure boot signature
    BTL_DEBUG_PRINTLN("Secure boot 1");
    return bootload_verifySecureBoot(startAddress);
  } else if (bootload_checkApplicationPropertiesMagic(appProperties)) {
    if (!bootload_checkApplicationPropertiesVersion(appProperties)) {
      return false;
    }
    // Application properties pointer is valid, decide what action to take
    // based on signature type
    if (appProperties->signatureType == APPLICATION_SIGNATURE_NONE) {
      // No signature, app has to be assumed valid
      BTL_DEBUG_PRINTLN("No signature, assume valid");
      return true;
    } else if (appProperties->signatureType == APPLICATION_SIGNATURE_CRC32) {
#ifdef BTL_LIB_NO_SUPPORT_CRC32_SIGNATURE
      // Don't support CRC32, app has to be assumed valid
      BTL_DEBUG_PRINTLN("CRC not supported, assume valid");
      return true;
#else
      uint32_t crc = btl_crc32Stream(
        (void *)startAddress,
        appProperties->signatureLocation + 4UL - startAddress,
        BTL_CRC32_START);
      if (crc == BTL_CRC32_END) {
        BTL_DEBUG_PRINTLN("CRC success");
        return true;
      } else {
        return false;
      }
#endif
    } else {
      // Default to secure boot check
      BTL_DEBUG_PRINTLN("Secure boot 2");
      return bootload_verifySecureBoot(startAddress);
    }
  } else {
    // Application properties pointer points into flash, but doesn't point to
    // an application properties struct.
    // Secure boot is not enforced (checked above), assume that this is a
    // pointer to the Reset_Handler and that the app is valid
    BTL_DEBUG_PRINTLN("No AP, assume valid");
    return true;
  }
}

uint32_t bootload_getApplicationVersionStorageCapacity(void)
{
#if defined(BOOTLOADER_ROLLBACK_PROTECTION) && (BOOTLOADER_ROLLBACK_PROTECTION == 1)
  return SL_GBL_APPLICATION_VERSION_STORAGE_CAPACITY;
#else
  return 0UL;
#endif
}

uint32_t* bootload_getApplicationVersionStoragePtr(uint32_t index)
{
#if defined(BOOTLOADER_ROLLBACK_PROTECTION) && (BOOTLOADER_ROLLBACK_PROTECTION == 1)
  uint32_t endOfBLpage = BTL_FIRST_STAGE_BASE + BTL_FIRST_STAGE_SIZE + BTL_MAIN_STAGE_MAX_SIZE;
  uint32_t *appVersionStoragePtr = (uint32_t*)endOfBLpage - (index + 1UL);
  return appVersionStoragePtr;
#else
  (void)index;
  return NULL;
#endif
}

bool bootload_storeApplicationVersion(uint32_t startAddress)
{
#if defined(BOOTLOADER_ROLLBACK_PROTECTION) && (BOOTLOADER_ROLLBACK_PROTECTION == 1)
  BareBootTable_t *appStart = (BareBootTable_t *)startAddress;
  ApplicationProperties_t *appProperties = (ApplicationProperties_t *)(appStart->signature);
  uint32_t appVersion = appProperties->app.version;
  uint32_t emptySlots = bootload_remainingApplicationUpgrades();
  uint32_t highestVersionSeen = getHighestApplicationVersionSeen();
  uint32_t *appVersionStoragePtr = bootload_getApplicationVersionStoragePtr(SL_GBL_APPLICATION_VERSION_STORAGE_CAPACITY);

  if (!bootload_checkApplicationPropertiesMagic(appProperties)) {
    return false;
  }
  if (!bootload_checkApplicationPropertiesVersion(appProperties)) {
    return false;
  }

  if (checkMaxVersionMagic()) {
    // The highest allowed version is seen, which is the maximum version allowed
    // so we do not need to remember any new application versions.
    return true;
  }
  if (*appVersionStoragePtr != SL_GBL_UINT32_MAX_NUMBER) {
    return false;
  }
  if (highestVersionSeen == appVersion) {
    // Do not need to store a new version.
    return true;
  }

  if (appVersion == SL_GBL_UINT32_MAX_NUMBER) {
    appVersion = SL_GBL_APPLICATION_VERSION_MAX_MAGIC;
    // Return true eventhough the flash pages are locked to avoid bricking devices.
    (void)flash_writeBuffer_dma((uint32_t)appVersionStoragePtr, &appVersion, 4UL, SL_GBL_MSC_LDMA_CHANNEL);
    return true;
  }

  // The application that is about to boot has a higher version than the highest seen version.
  // However, this version can't be stored, so do not allow the application to boot to prevent
  // downgrade later. This will "never" happen as the number of empty slots is checked before getting to this point.
  // Unless the slots are already filled for some unexpected reason.
  if (emptySlots == 0UL) {
    return false;
  }

  appVersionStoragePtr = bootload_getApplicationVersionStoragePtr(SL_GBL_APPLICATION_VERSION_STORAGE_CAPACITY - emptySlots);
  (void)flash_writeBuffer_dma((uint32_t)appVersionStoragePtr, &appVersion, 4UL, SL_GBL_MSC_LDMA_CHANNEL);
  return true;
#else
  (void)startAddress;
  return false;
#endif
}

bool bootload_verifyApplicationVersion(uint32_t appVersion, bool checkRemainingAppUpgrades)
{
#if defined(BOOTLOADER_ROLLBACK_PROTECTION) && (BOOTLOADER_ROLLBACK_PROTECTION == 1)
  uint32_t highestVersionSeen = getHighestApplicationVersionSeen();

  // Check for the minimum application version that should be allowed.
  if (PARSER_APPLICATION_MINIMUM_VERSION_VALID > appVersion) {
    return false;
  }
  if (highestVersionSeen > appVersion) {
    return false;
  }

  // Application version is higher/equal than the saved application versions.
  // Check if we have empty spaces left for storing new application versions.
  if ((appVersion > highestVersionSeen) && checkRemainingAppUpgrades) {
    // The new application version needs to be stored, check we have empty spaces left.
    if (bootload_remainingApplicationUpgrades() == 0UL) {
      return false;
    }
  }

  return true;
#else
  (void)appVersion;
  (void)checkRemainingAppUpgrades;
  return false;
#endif
}

uint32_t bootload_remainingApplicationUpgrades(void)
{
#if defined(BOOTLOADER_ROLLBACK_PROTECTION) && (BOOTLOADER_ROLLBACK_PROTECTION == 1)
  uint32_t *appVersionStoragePtr = bootload_getApplicationVersionStoragePtr(SL_GBL_APPLICATION_VERSION_STORAGE_CAPACITY);
  if (checkMaxVersionMagic()) {
    return 0UL;
  }

  for (uint32_t i = 0UL; i < SL_GBL_APPLICATION_VERSION_STORAGE_CAPACITY; i++) {
    appVersionStoragePtr = bootload_getApplicationVersionStoragePtr(i);
    if (*appVersionStoragePtr == SL_GBL_UINT32_MAX_NUMBER) {
      return (SL_GBL_APPLICATION_VERSION_STORAGE_CAPACITY - i);
    }
  }

  return 0UL;
#else
  return 0UL;
#endif
}

void bootload_storeApplicationVersionResetMagic(void)
{
#if defined(BOOTLOADER_ROLLBACK_PROTECTION) && (BOOTLOADER_ROLLBACK_PROTECTION == 1)
  uint32_t *appVersionResetPtr = bootload_getApplicationVersionStoragePtr(bootload_getApplicationVersionStorageCapacity() + 1UL);
  uint32_t appVersionResetMagic = SL_GBL_APPLICATION_VERSION_RESET_MAGIC;
  (void)flash_writeBuffer_dma((uint32_t)appVersionResetPtr, &appVersionResetMagic, 4UL, SL_GBL_MSC_LDMA_CHANNEL);
#else
  return;
#endif
}

void bootload_removeStoredApplicationVersions(void)
{
#if defined(BOOTLOADER_ROLLBACK_PROTECTION) && (BOOTLOADER_ROLLBACK_PROTECTION == 1)
  uint32_t *appVersionResetPtr = bootload_getApplicationVersionStoragePtr(SL_GBL_APPLICATION_VERSION_STORAGE_CAPACITY + 1UL);
  if ((bootload_remainingApplicationUpgrades() < SL_GBL_APPLICATION_VERSION_STORAGE_CAPACITY)
      && checkResetMagic()) {
    // Not empty and reset requested.
    uint32_t versionStorageAddr = ((uint32_t)appVersionResetPtr / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE;
    (void)flash_erasePage(versionStorageAddr);
  }
#else
  return;
#endif
}

bool bootload_gotCertificate(void *appProp)
{
#if defined(BOOTLOADER_SUPPORT_CERTIFICATES) && (BOOTLOADER_SUPPORT_CERTIFICATES == 1)
  if (appProp == NULL) {
    return false;
  }

  ApplicationProperties_t *appProperties = (ApplicationProperties_t *)(appProp);
  // Compatibility check of the application properties struct.
  // The application properties struct with the major version 0
  // does not contain the certificate struct.
  if (((appProperties->structVersion & APPLICATION_PROPERTIES_VERSION_MAJOR_MASK)
       >> APPLICATION_PROPERTIES_VERSION_MAJOR_SHIFT) < 1UL) {
    return false;
  }

  if (((appProperties->structVersion & APPLICATION_PROPERTIES_VERSION_MINOR_MASK)
       >> APPLICATION_PROPERTIES_VERSION_MINOR_SHIFT) < 1UL) {
    return false;
  }

  if (appProperties->cert == NULL) {
    return false;
  }

  return true;
#else
  (void)appProp;
  return false;
#endif
}

bool bootload_verifyCertificate(void *cert)
{
#if defined(BOOTLOADER_SUPPORT_CERTIFICATES) && (BOOTLOADER_SUPPORT_CERTIFICATES == 1)
  if (cert == NULL) {
    return false;
  }
  ApplicationCertificate_t *appCertificate = (ApplicationCertificate_t *)(cert);

  volatile int32_t retVal = BOOTLOADER_ERROR_SECURITY_REJECTED;
  Sha256Context_t shaState;

  // Access word 13 to read sl_app_properties of the bootloader.
  ApplicationProperties_t *blProperties =
    (ApplicationProperties_t *)(*(uint32_t *)(BTL_MAIN_STAGE_BASE + 52UL));
  if (!bootload_checkApplicationPropertiesMagic(blProperties)) {
    return false;
  }
#if !defined(MAIN_BOOTLOADER_TEST)
  if ((uint32_t)blProperties > (BTL_APPLICATION_BASE - sizeof(ApplicationProperties_t))) {
    // Make sure that this pointer is within the bootloader space.
    return false;
  }
#endif

  // Application cert version need to be higher or equal than
  // the running bootloader version.
  if (blProperties->cert->version > appCertificate->version) {
    return false;
  } else {
    // Check ECDSA signing.
    btl_initSha256(&shaState);
    btl_updateSha256(&shaState,
                     (const uint8_t*)appCertificate,
                     72U);
    btl_finalizeSha256(&shaState);

    // Use the public key stored in bootloader certificate
    // to verify the certificate. The bootloader cerfiticate
    // has been validated by SE.
    retVal = btl_verifyEcdsaP256r1(shaState.sha,
                                   &(appCertificate->signature[0]),
                                   &(appCertificate->signature[32]),
                                   &(blProperties->cert->key[0]),
                                   &(blProperties->cert->key[32]));
    if (retVal != BOOTLOADER_OK) {
      return false;
    }
    return true;
  }
#else
  (void)cert;
  return false;
#endif
}

bool bootload_verifyApplicationCertificate(void *appProp, void *gotCert)
{
#if defined(BOOTLOADER_SUPPORT_CERTIFICATES) && (BOOTLOADER_SUPPORT_CERTIFICATES == 1)
  ApplicationProperties_t *appProperties = (ApplicationProperties_t *)(appProp);
  bool *gotCertificate = (bool *)gotCert;
  *gotCertificate = bootload_gotCertificate(appProperties);
  if (*gotCertificate) {
    // Validate Cert
    if (!bootload_verifyCertificate(appProperties->cert)) {
      // Cert found, but it contains an invalid signature.
      return false;
    }
  }
#if defined(BOOTLOADER_REJECT_DIRECT_SIGNED_IMG) && (BOOTLOADER_REJECT_DIRECT_SIGNED_IMG == 1)
  else {
    return false;
  }
#endif
  return true;
#else
  (void)appProp;
  (void)gotCert;
  return true;
#endif
}

// --------------------------------
// Secure Element functions

SL_WEAK bool bootload_commitBootloaderUpgrade(uint32_t upgradeAddress, uint32_t size)
{
  // Check CRC32 checksum on the bootloader image.
  uint32_t crc = btl_crc32Stream((void *)upgradeAddress, (size_t)size, BTL_CRC32_START);
  if (crc != BTL_CRC32_END) {
    // CRC32 check failed. Return early.
    return false;
  }

#if defined(_SILICON_LABS_32B_SERIES_2)
  // The CRC32 checksum has been appended to the image and it has already been checked above
  // so just disregard it. This give us the correct image size.
  size = size - 4u;
#endif

#if defined(SEMAILBOX_PRESENT)
#if defined(_CMU_CLKEN1_SEMAILBOXHOST_MASK)
  CMU->CLKEN1_SET = CMU_CLKEN1_SEMAILBOXHOST;
#endif

  // Init with != SE_RESPONSE_OK response.
  SE_Response_t response = 0x12345678U;

  // Verify upgrade image
  SE_Command_t checkImage = SE_COMMAND_DEFAULT(SE_COMMAND_CHECK_HOST_IMAGE);
  SE_addParameter(&checkImage, upgradeAddress);
  SE_addParameter(&checkImage, size);

  SE_executeCommand(&checkImage);
  response = SE_readCommandResponse();

  if (response != SE_RESPONSE_OK) {
    return false;
  }
#endif

#if !defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80)
  // Set Reset Magic to signal that the application versions should be cleaned.
  // Doing this to make sure that those application versions gets cleaned with a bootloader upgrade.
  // (Those versions will not get cleaned if the bootloader image does not require the last bootloader page).
  bootload_storeApplicationVersionResetMagic();
 #endif

#if defined(SEMAILBOX_PRESENT)
  // Set reset code for when we get back
  reset_setResetReason(BOOTLOADER_RESET_REASON_BOOTLOAD);

  // Apply upgrade image
  SE_Command_t applyImage = SE_COMMAND_DEFAULT(SE_COMMAND_APPLY_HOST_IMAGE);
  SE_addParameter(&applyImage, upgradeAddress);
  SE_addParameter(&applyImage, size);

  SE_executeCommand(&applyImage);

  // Should never get here
  response = SE_readCommandResponse();
  return false;
#elif defined(CRYPTOACC_PRESENT)
  // Set reset code for when we get back
  reset_setResetReason(BOOTLOADER_RESET_REASON_BOOTLOAD);

  // Apply upgrade image
  SE_Command_t applyImage = SE_COMMAND_DEFAULT(SE_COMMAND_APPLY_HOST_IMAGE);
  SE_addParameter(&applyImage, upgradeAddress);
  SE_addParameter(&applyImage, size);

  SE_executeCommand(&applyImage);

  // Should never get here
  return false;
#else
  (void) upgradeAddress;
  (void) size;
  // Reboot and apply upgrade
  reset_resetWithReason(BOOTLOADER_RESET_REASON_UPGRADE);

  // Should never get here
  return false;
#endif
}

#if defined(_MSC_PAGELOCK0_MASK)
bool bootload_lockApplicationArea(uint32_t startAddress, uint32_t endAddress)
{
  if (endAddress == 0U) {
    // It is assumed that a valid start address of application is used.
    BareBootTable_t *appStart = (BareBootTable_t *)startAddress;
    ApplicationProperties_t *appProperties = (ApplicationProperties_t *)(appStart->signature);
    bool retVal = getSignatureX(appProperties, &endAddress);
    if (!retVal) {
      BTL_DEBUG_PRINTLN("Wrong s type");
      return false;
    }
  }

  if (startAddress > endAddress) {
    return false;
  }

  uint32_t volatile * pageLockAddr;
  const uint32_t pageSize = (uint32_t)FLASH_PAGE_SIZE;
  uint32_t pageNo = ((startAddress - (startAddress % pageSize)) - FLASH_BASE) / pageSize;
  uint32_t endPageNo = ((endAddress - (endAddress % pageSize) + pageSize) - FLASH_BASE) / pageSize;

#if defined(CMU_CLKEN1_MSC)
  CMU->CLKEN1_SET = CMU_CLKEN1_MSC;
#endif
  while (pageNo < endPageNo) {
    pageLockAddr = (uint32_t volatile *)(&(MSC->PAGELOCK0_SET));
    // Find the page lock register that includes current page number.
    pageLockAddr = &pageLockAddr[pageNo / 32U]; // 32 pages per page lock word.
    *pageLockAddr = (1UL << (pageNo % 32U));
    pageNo += 1U;
  }
#if defined(CRYPTOACC_PRESENT)
  CMU->CLKEN1_CLR = CMU_CLKEN1_MSC;
#endif
  return true;
}
#endif

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
SL_WEAK bool bootload_checkSeUpgradeVersion(uint32_t upgradeVersion)
{
  uint32_t runningVersion = 0xFFFFFFFFUL;

  if (!bootload_getSeVersion(&runningVersion)) {
    return false; // Could not retrieve SE version
  }

  // Only allow upgrade if it is higher than the running version
  if (runningVersion < upgradeVersion) {
    return true;
  } else {
    return false;
  }
}

#if defined(SEMAILBOX_PRESENT)
SL_WEAK bool bootload_commitSeUpgrade(uint32_t upgradeAddress)
{
#if defined(_CMU_CLKEN1_SEMAILBOXHOST_MASK)
  CMU->CLKEN1_SET = CMU_CLKEN1_SEMAILBOXHOST;
#endif

  // Init with != SE_RESPONSE_OK response.
  SE_Response_t response = 0x12345678U;

  // Verify upgrade image
  SE_Command_t checkImage = SE_COMMAND_DEFAULT(SE_COMMAND_CHECK_SE_IMAGE);
  SE_addParameter(&checkImage, upgradeAddress);

  SE_executeCommand(&checkImage);
  response = SE_readCommandResponse();

  if (response != SE_RESPONSE_OK) {
    return false;
  }

  // Set reset code for when we get back
  reset_setResetReason(BOOTLOADER_RESET_REASON_BOOTLOAD);

  // Apply upgrade image
  SE_Command_t applyImage = SE_COMMAND_DEFAULT(SE_COMMAND_APPLY_SE_IMAGE);
  SE_addParameter(&applyImage, upgradeAddress);

  SE_executeCommand(&applyImage);

  // Should never get here
  response = SE_readCommandResponse();
  return false;
}

#elif defined(CRYPTOACC_PRESENT)
SL_WEAK bool bootload_commitSeUpgrade(uint32_t upgradeAddress)
{
  // Set reset code for when we get back
  reset_setResetReason(BOOTLOADER_RESET_REASON_BOOTLOAD);

  // Apply upgrade image
  SE_Command_t applyImage = SE_COMMAND_DEFAULT(SE_COMMAND_APPLY_SE_IMAGE);
  SE_addParameter(&applyImage, upgradeAddress);

  SE_executeCommand(&applyImage);

  // Should never get here
  return false;
}
#endif // defined(CRYPTOACC_PRESENT)
#endif // defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)

uint32_t bootload_getBootloaderVersion(void)
{
  return mainBootloaderTable->header.version;
}

bool bootload_getApplicationVersion(uint32_t *version)
{
  ApplicationProperties_t *appProperties = (ApplicationProperties_t *)
                                           mainBootloaderTable->startOfAppSpace->signature;

  if (!bootload_checkApplicationPropertiesMagic(appProperties)) {
    return false; // invalid application properties struct, return error
  }

  *version = appProperties->app.version;
  return true;
}

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
bool bootload_getSeVersion(uint32_t *version)
{
  // Init with != SE_RESPONSE_OK response.
  SE_Response_t response = 0x12345678UL;

#if defined(SEMAILBOX_PRESENT)
#if defined(_CMU_CLKEN1_SEMAILBOXHOST_MASK)
  CMU->CLKEN1_SET = CMU_CLKEN1_SEMAILBOXHOST;
#endif

  SE_Command_t getVersion = SE_COMMAND_DEFAULT(SE_COMMAND_STATUS_SE_VERSION);
  SE_DataTransfer_t dataOut = SE_DATATRANSFER_DEFAULT(version, 4UL);
  SE_addDataOutput(&getVersion, &dataOut);

  SE_executeCommand(&getVersion);
  response = SE_readCommandResponse();

#elif defined(CRYPTOACC_PRESENT)
  response = SE_getVersion(version);
#endif

  if (response != SE_RESPONSE_OK) {
    return false; // error getting SE version
  }

  return true;
}
#endif // SEMAILBOX_PRESENT || CRYPTOACC_PRESENT

SL_WEAK uint32_t bootload_getUpgradeLocation(void)
{
  return BTL_UPGRADE_LOCATION;
}
