/***************************************************************************//**
 * @file
 * @brief Application interface to the bootloader.
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

#include "btl_interface.h"
#include "em_core.h"
#include "btl_interface_cfg.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)

// -----------------------------------------------------------------------------
// Configurations

#if ((!defined(BOOTLOADER_DISABLE_NVM3_FAULT_HANDLING) \
  || (defined(BOOTLOADER_DISABLE_NVM3_FAULT_HANDLING)  \
  && (BOOTLOADER_DISABLE_NVM3_FAULT_HANDLING == 0))) && defined(SL_CATALOG_NVM3_PRESENT))
#define BOOTLOADER_ENABLE_NVM3_FAULT_HANDLING
#endif // BOOTLOADER_DISABLE_NVM3_FAULT_HANDLING && SL_CATALOG_NVM3_PRESENT

#if !defined(BOOTLOADER_MANUAL_OVERRIDE_SECURITY_STATE)  \
  || (defined(BOOTLOADER_MANUAL_OVERRIDE_SECURITY_STATE) \
  && (BOOTLOADER_MANUAL_OVERRIDE_SECURITY_STATE == 0))
#define BOOTLOADER_ENABLE_USART_AUTO_DETECTION
#endif // BOOTLOADER_MANUAL_OVERRIDE_SECURITY_STATE

#if (defined(BOOTLOADER_DISABLE_OLD_BOOTLOADER_MITIGATION) \
  && (BOOTLOADER_DISABLE_OLD_BOOTLOADER_MITIGATION == 1))
#define BOOTLOADER_DISABLE_MULTI_TIERED_FALLBACK
#undef BOOTLOADER_ENABLE_NVM3_FAULT_HANDLING
#undef BOOTLOADER_ENABLE_USART_AUTO_DETECTION
#endif // BOOTLOADER_DISABLE_OLD_BOOTLOADER_MITIGATION

#if defined(SL_TRUSTZONE_SECURE)
#undef BOOTLOADER_ENABLE_NVM3_FAULT_HANDLING
#endif // SL_TRUSTZONE_SECURE

// -----------------------------------------------------------------------------
// Configuration spesifics

#if defined(BOOTLOADER_ENABLE_NVM3_FAULT_HANDLING)
// Allocated range of NVM3 IDs for bootloader usage */
#define BL_NVM3_RESERVED_ID  (0x87100UL)

#include <string.h>
#include "nvm3_default.h"

static bool blPPUSATDnStateCacheSet = false;
static uint32_t blPPUSATDnStateCache[2] = { 0 };
#endif // BOOTLOADER_ENABLE_NVM3_FAULT_HANDLING

#if defined(BOOTLOADER_ENABLE_USART_AUTO_DETECTION)
static void preConfigureUsartPPUSATD(void);
static void storeUsartInUse(void);
static int32_t usartNumberSpi = -1;
#endif // BOOTLOADER_ENABLE_USART_AUTO_DETECTION

#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

// -----------------------------------------------------------------------------
// Static variables

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
#if !defined(BOOTLOADER_TEST_UNPRIVILEGED_ACCESS)
static CORE_DECLARE_IRQ_STATE;
#endif
static Bootloader_PPUSATDnCLKENnState_t blPPUSATDnCLKENnState = { 0 };

typedef enum {
  IDLE, SAVE, TIERED
} ppusatdConfigurationState_t;

static ppusatdConfigurationState_t bootloader_ppusatdConfigstate = IDLE;
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

// -----------------------------------------------------------------------------
// Enums

typedef enum {
  RESET, INITIALIZED, DEINITIALIZED
} initState_t;

static initState_t bootloader_InitState = RESET;

// -----------------------------------------------------------------------------
// Test helpers

#if defined(BOOTLOADER_TEST_UNPRIVILEGED_ACCESS)
extern bool enabled;
extern void enterUnprivilegedMode(bool);
extern void exitUnprivilegedMode(void);
#endif // BOOTLOADER_TEST_UNPRIVILEGED_ACCESS

// -----------------------------------------------------------------------------
// Functions

void bootloader_getInfo(BootloaderInformation_t *info)
{
#if defined(BOOTLOADER_HAS_FIRST_STAGE)
  if (!bootloader_pointerToFirstStageValid(firstBootloaderTable)
      || !bootloader_pointerValid(mainBootloaderTable)) {
    // No bootloader is present (first stage or main stage invalid)
    info->type = NO_BOOTLOADER;
    info->capabilities = 0;
  } else if ((firstBootloaderTable->header.type == BOOTLOADER_MAGIC_FIRST_STAGE)
             && (mainBootloaderTable->header.type == BOOTLOADER_MAGIC_MAIN)) {
    info->type = SL_BOOTLOADER;
    info->version = mainBootloaderTable->header.version;
    info->capabilities = mainBootloaderTable->capabilities;
  } else {
    info->type = NO_BOOTLOADER;
    info->capabilities = 0;
  }
#else
  if (!bootloader_pointerValid(mainBootloaderTable)) {
    // No bootloader is present (first stage or main stage invalid)
    info->type = NO_BOOTLOADER;
    info->capabilities = 0;
  } else if (mainBootloaderTable->header.type == BOOTLOADER_MAGIC_MAIN) {
    info->type = SL_BOOTLOADER;
    info->version = mainBootloaderTable->header.version;
    info->capabilities = mainBootloaderTable->capabilities;
  } else {
    info->type = NO_BOOTLOADER;
    info->capabilities = 0;
  }
#endif
}

int32_t bootloader_init(void)
{
  int32_t retVal;
  if (!bootloader_pointerValid(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }
  if (bootloader_InitState == RESET || bootloader_InitState == DEINITIALIZED) {
#if defined(SL_TRUSTZONE_SECURE)
    if (bootloader_InitState == RESET) {
      // Enable SMU bus clock at the start-up of the TZ secure application
      // to make it possible to configure the SMU peripheral. Since the CMU address
      // is known. Otherwise, delegate the SMU bus clock enablement to the NS application.
#if defined(_CMU_CLKEN1_SMU_MASK)
      CMU->CLKEN1_SET = CMU_CLKEN1_SMU;
#endif // _CMU_CLKEN1_SMU_MASK
    }
#endif // SL_TRUSTZONE_SECURE

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
    bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif

#if defined(BOOTLOADER_ENABLE_NVM3_FAULT_HANDLING)
    NVIC_ClearPendingIRQ(SMU_SECURE_IRQn);
    SMU->IF_CLR = SMU_IEN_PPUSEC;
    NVIC_EnableIRQ(SMU_SECURE_IRQn);
    SMU->IEN_SET = SMU_IEN_PPUSEC;
#endif
    retVal = mainBootloaderTable->init();

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
    bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif

    if (retVal == BOOTLOADER_OK) {
      bootloader_InitState = INITIALIZED;
    }
  } else {
    retVal = BOOTLOADER_OK;
  }
  return retVal;
}

int32_t bootloader_deinit(void)
{
  int32_t retVal;
  if (!bootloader_pointerValid(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }

  if (bootloader_InitState == INITIALIZED) {
#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
    bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

#if defined(BOOTLOADER_ENABLE_NVM3_FAULT_HANDLING)
    SMU->IF_CLR = SMU_IEN_PPUSEC;
    SMU->IEN_CLR = SMU_IEN_PPUSEC;
    NVIC_ClearPendingIRQ(SMU_SECURE_IRQn);
    NVIC_DisableIRQ(SMU_SECURE_IRQn);
#endif

    retVal = mainBootloaderTable->deinit();
    if (retVal == BOOTLOADER_OK) {
      bootloader_InitState = DEINITIALIZED;
    }
#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
    bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE
  } else {
    retVal = BOOTLOADER_OK;
  }
  return retVal;
}

BootloaderResetCause_t bootloader_getResetReason(void)
{
  volatile BootloaderResetCause_t* resetCause = (BootloaderResetCause_t*) (SRAM_BASE);
  return *resetCause;
}

void bootloader_rebootAndInstall(void)
{
  // Set reset reason to bootloader entry
  BootloaderResetCause_t* resetCause = (BootloaderResetCause_t*) (SRAM_BASE);
  resetCause->reason = BOOTLOADER_RESET_REASON_BOOTLOAD;
  resetCause->signature = BOOTLOADER_RESET_SIGNATURE_VALID;
#if defined(RMU_PRESENT)
  // Clear resetcause
  RMU->CMD = RMU_CMD_RCCLR;
  // Trigger a software system reset
  RMU->CTRL = (RMU->CTRL & ~_RMU_CTRL_SYSRMODE_MASK) | RMU_CTRL_SYSRMODE_FULL;
#endif
  NVIC_SystemReset();
}

int32_t bootloader_initParser(BootloaderParserContext_t *context,
                              size_t                    contextSize)
{
  if (!bootloader_pointerValid(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_PARSE_FAILED;
  }

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  int32_t retVal = mainBootloaderTable->initParser(context, contextSize);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  return retVal;
}

int32_t bootloader_parseBuffer(BootloaderParserContext_t   *context,
                               BootloaderParserCallbacks_t *callbacks,
                               uint8_t                     data[],
                               size_t                      numBytes)
{
  if (!bootloader_pointerValid(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_PARSE_FAILED;
  }

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  int32_t retVal = mainBootloaderTable->parseBuffer(context, callbacks, data, numBytes);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  return retVal;
}

int32_t bootloader_parseImageInfo(BootloaderParserContext_t *context,
                                  uint8_t                   data[],
                                  size_t                    numBytes,
                                  ApplicationData_t         *appInfo,
                                  uint32_t                  *bootloaderVersion)
{
  if (!bootloader_pointerValid(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_PARSE_FAILED;
  }

  BootloaderInformation_t info = { .type = SL_BOOTLOADER, .version = 0U, .capabilities = 0U };
  bootloader_getInfo(&info);

  uint32_t blMajorVersion = ((info.version & BOOTLOADER_VERSION_MAJOR_MASK)
                             >> BOOTLOADER_VERSION_MAJOR_SHIFT);
  uint32_t blMinorVersion = ((info.version & BOOTLOADER_VERSION_MINOR_MASK)
                             >> BOOTLOADER_VERSION_MINOR_SHIFT);

  if ((blMajorVersion < 1UL) || (blMajorVersion == 1UL && blMinorVersion < 11UL)) {
    return BOOTLOADER_ERROR_PARSE_FAILED;
  }

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  int32_t retVal = mainBootloaderTable->parseImageInfo(context, data, numBytes, appInfo, bootloaderVersion);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  return retVal;
}

uint32_t bootloader_parserContextSize(void)
{
  if (!bootloader_pointerValid(mainBootloaderTable)) {
    return 0UL;
  }

  BootloaderInformation_t info = { .type = SL_BOOTLOADER, .version = 0U, .capabilities = 0U };
  bootloader_getInfo(&info);

  uint32_t blMajorVersion = ((info.version & BOOTLOADER_VERSION_MAJOR_MASK)
                             >> BOOTLOADER_VERSION_MAJOR_SHIFT);
  uint32_t blMinorVersion = ((info.version & BOOTLOADER_VERSION_MINOR_MASK)
                             >> BOOTLOADER_VERSION_MINOR_SHIFT);

  if (blMajorVersion < 1UL) {
    return 384UL;
  }

  if (blMajorVersion == 1UL && blMinorVersion < 11UL) {
#if defined(_SILICON_LABS_32B_SERIES_2)
    if (blMinorVersion == 10UL) {
      return 524UL;
    } else {
      return 384UL;
    }
#else
    return 384UL;
#endif
  }

  return mainBootloaderTable->parserContextSize();
}

bool bootloader_verifyApplication(uint32_t startAddress)
{
  if (!bootloader_pointerValid(mainBootloaderTable)) {
    return false;
  }

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnSaveReconfigureState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  bool retVal = mainBootloaderTable->verifyApplication(startAddress);

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  bootloader_ppusatdnRestoreState(&blPPUSATDnCLKENnState);
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

  return retVal;
}

bool bootloader_secureBootEnforced(void)
{
  BootloaderInformation_t info = { .type = SL_BOOTLOADER, .version = 0U, .capabilities = 0U };
  bootloader_getInfo(&info);

  if (info.capabilities & BOOTLOADER_CAPABILITY_ENFORCE_SECURE_BOOT) {
    return true;
  }
  return false;
}

bool bootloader_getUpgradeLocation(uint32_t *location)
{
  if (!bootloader_pointerValid(mainBootloaderTable)) {
    return false;
  }

  BootloaderInformation_t info = { .type = SL_BOOTLOADER, .version = 0U, .capabilities = 0U };
  bootloader_getInfo(&info);

  uint32_t blMajorVersion = ((info.version & BOOTLOADER_VERSION_MAJOR_MASK)
                             >> BOOTLOADER_VERSION_MAJOR_SHIFT);
  uint32_t blMinorVersion = ((info.version & BOOTLOADER_VERSION_MINOR_MASK)
                             >> BOOTLOADER_VERSION_MINOR_SHIFT);

  if (blMajorVersion == 2UL && blMinorVersion >= 1UL) {
    *location = mainBootloaderTable->getUpgradeLocation();
    return true;
  }

  return false;
}

#if !defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80)
uint32_t bootloader_remainingApplicationUpgrades(void)
{
  if (!bootloader_pointerValid(mainBootloaderTable)) {
    return 0UL;
  }

  BootloaderInformation_t info = { .type = SL_BOOTLOADER, .version = 0U, .capabilities = 0U };
  bootloader_getInfo(&info);

  uint32_t blMajorVersion = ((info.version & BOOTLOADER_VERSION_MAJOR_MASK)
                             >> BOOTLOADER_VERSION_MAJOR_SHIFT);
  uint32_t blMinorVersion = ((info.version & BOOTLOADER_VERSION_MINOR_MASK)
                             >> BOOTLOADER_VERSION_MINOR_SHIFT);

  if ((blMajorVersion < 1UL) || (blMajorVersion == 1UL && blMinorVersion < 11UL)) {
    return 0UL;
  }

  return mainBootloaderTable->remainingApplicationUpgrades();
}
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
bool bootloader_getCertificateVersion(uint32_t *version)
{
  // Access word 13 to read sl_app_properties of the bootloader.
  ApplicationProperties_t *blProperties =
    (ApplicationProperties_t *)(*(uint32_t *)(BTL_MAIN_STAGE_BASE + 52UL));

  if (!bootloader_pointerValid(blProperties)) {
    return false;
  }

  // Compatibility check of the application properties struct.
  if (((blProperties->structVersion & APPLICATION_PROPERTIES_VERSION_MAJOR_MASK)
       >> APPLICATION_PROPERTIES_VERSION_MAJOR_SHIFT) < 1UL) {
    return false;
  }
  if (((blProperties->structVersion & APPLICATION_PROPERTIES_VERSION_MINOR_MASK)
       >> APPLICATION_PROPERTIES_VERSION_MINOR_SHIFT) < 1UL) {
    return false;
  }

  if (blProperties->cert == NULL) {
    return false;
  }

  *version = blProperties->cert->version;
  return true;
}
#endif // _SILICON_LABS_32B_SERIES_2

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
void bootloader_getPeripheralList(uint32_t *ppusatd0, uint32_t *ppusatd1)
{
  if (!bootloader_pointerValid(mainBootloaderTable)) {
    return;
  }

  BootloaderInformation_t info = { .type = SL_BOOTLOADER, .version = 0U, .capabilities = 0U };
  bootloader_getInfo(&info);

  if (info.capabilities & BOOTLOADER_CAPABILITY_PERIPHERAL_LIST) {
    mainBootloaderTable->getPeripheralList(ppusatd0, ppusatd1);
  }
}

void bootloader_ppusatdnSaveReconfigureState(Bootloader_PPUSATDnCLKENnState_t *ctx)
{
  if (bootloader_ppusatdConfigstate != IDLE) {
    // This function is called from a bootloader callback function
    bootloader_ppusatdConfigstate = TIERED;
    return;
  }
  uint32_t ppusatd0 = 0u;
  uint32_t ppusatd1 = 0u;

  if (ctx == NULL) {
    return;
  }

  bootloader_ppusatdConfigstate = SAVE;

  // Enter ATOMIC section. The ATOMIC section is exited when
  // the restore function is called.
#if !defined(BOOTLOADER_TEST_UNPRIVILEGED_ACCESS)
  CORE_ENTER_ATOMIC();
#endif

  sli_bootloader_preHook();

#if defined(BOOTLOADER_ENABLE_NVM3_FAULT_HANDLING)
  // Read the stored PPUSATD state from NVM3
  if (!blPPUSATDnStateCacheSet) {
    Ecode_t status;
    status = nvm3_initDefault();
    if (status == ECODE_NVM3_OK) {
      nvm3_ObjectKey_t object_id = BL_NVM3_RESERVED_ID;
      status = nvm3_readData(nvm3_defaultHandle,
                             object_id,
                             blPPUSATDnStateCache,
                             sizeof(blPPUSATDnStateCache));
      if (status == ECODE_NVM3_OK) {
        blPPUSATDnStateCacheSet = true;
      }
    }
  }
#endif // BOOTLOADER_ENABLE_NVM3_FAULT_HANDLING

  // Store the CLKENn states
#if defined(_CMU_CLKEN0_MASK)
#if defined(SL_TRUSTZONE_SECURE)
  ctx->CLKEN0 = CMU_NS->CLKEN0;
  ctx->CLKEN1 = CMU_NS->CLKEN1;
#else
  ctx->CLKEN0 = CMU->CLKEN0;
  ctx->CLKEN1 = CMU->CLKEN1;
#endif // SL_TRUSTZONE_SECURE
#endif

#if defined(_CMU_CLKEN1_SMU_MASK)
#if defined(SL_TRUSTZONE_SECURE)
  CMU_NS->CLKEN1_SET = CMU_CLKEN1_SMU;
#else
  CMU->CLKEN1_SET = CMU_CLKEN1_SMU;
#endif // SL_TRUSTZONE_SECURE
#endif // _CMU_CLKEN1_SMU_MASK
  ctx->SMU_STATUS = SMU->STATUS;

  // Unlock SMU before re-configuration
  SMU->LOCK = SMU_LOCK_SMULOCKKEY_UNLOCK;

  // Store the PPUSATDn states
  ctx->PPUSATD0 = SMU->PPUSATD0;
  ctx->PPUSATD1 = SMU->PPUSATD1;

#if defined(SMU_NS_CFGNS_BASE)
  // Store the PPUPATDn states
  ctx->PPUPATD0 = SMU->PPUPATD0;
  ctx->PPUPATD1 = SMU->PPUPATD1;

  SMU->PPUPATD0 = SMU_NS_CFGNS->PPUNSPATD0;
  SMU->PPUPATD1 = SMU_NS_CFGNS->PPUNSPATD1;
#endif // SMU_NS_CFGNS_BASE

  // Move the CMU address to secure before enabling the bus clocks
  // if the call is triggered by an NS app.
#if defined(SL_TRUSTZONE_SECURE)
  SMU->PPUSATD0_SET = SMU_PPUSATD0_CMU;
#endif // SL_TRUSTZONE_SECURE

#if defined(CMU_CLKEN0_LDMA)
  CMU->CLKEN0_SET = CMU_CLKEN0_LDMA;
#endif // CMU_CLKEN0_LDMA
#if defined(CRYPTOACC_PRESENT)
  CMU->CLKEN1_SET = CMU_CLKEN1_CRYPTOACC;
#endif

  // Wait for any active transition of other busmasters to finish
  if (SMU->PPUSATD0 & SMU_PPUSATD0_LDMA) {
    while (LDMA_S->STATUS & LDMA_STATUS_ANYBUSY) ;
  } else {
    while (LDMA_NS->STATUS & LDMA_STATUS_ANYBUSY) ;
  }
#if defined(CRYPTOACC_PRESENT)
  if (SMU->PPUSATD1 & SMU_PPUSATD1_CRYPTOACC) {
    #if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_230)
    while (CRYPTOACC_S->DMACTRL_STATUS & (CRYPTOACC_DMACTRL_STATUS_FETCH_BUSY | CRYPTOACC_DMACTRL_STATUS_PUSH_BUSY | CRYPTOACC_DMACTRL_STATUS_SOFT_RST_BUSY)) ;
    #else
    while (CRYPTOACC_S->STATUS & (CRYPTOACC_STATUS_FETCHERBSY | CRYPTOACC_STATUS_PUSHERBSY | CRYPTOACC_STATUS_SOFTRSTBSY)) ;
    #endif
  } else {
    #if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_230)
    while (CRYPTOACC_NS->DMACTRL_STATUS & (CRYPTOACC_DMACTRL_STATUS_FETCH_BUSY | CRYPTOACC_DMACTRL_STATUS_PUSH_BUSY | CRYPTOACC_DMACTRL_STATUS_SOFT_RST_BUSY)) ;
    #else
    while (CRYPTOACC_NS->STATUS & (CRYPTOACC_STATUS_FETCHERBSY | CRYPTOACC_STATUS_PUSHERBSY | CRYPTOACC_STATUS_SOFTRSTBSY)) ;
    #endif
  }
#endif // CRYPTOACC_PRESENT

  // Configure the peripheral secure access state before calling into the bootloader.
#if !defined(BOOTLOADER_DISABLE_MULTI_TIERED_FALLBACK)
#if defined(BOOTLOADER_ENABLE_USART_AUTO_DETECTION)
  // If we do not know which USART is used by the bootloader,
  // and the bootloader is not initialized,
  // Configure PPUSATDn bits for all the USART not in use.
  preConfigureUsartPPUSATD();
#else
  SMU->PPUSATD0_SET = BOOTLOADER_PPUSATD0_MASK;
  SMU->PPUSATD1_SET = BOOTLOADER_PPUSATD1_MASK;
#endif // BOOTLOADER_ENABLE_USART_AUTO_DETECTION

  SMU->PPUSATD0_SET = SMU_PPUSATD0_CMU;
  SMU->PPUSATD0_SET = SMU_PPUSATD0_MSC;
  if (bootloader_getAllocatedDMAChannel() != -1
      && bootloader_getAllocatedDMAChannel() != BOOTLOADER_ERROR_INIT_STORAGE) {
    SMU->PPUSATD0_SET = SMU_PPUSATD0_LDMA;
    SMU->PPUSATD0_SET = SMU_PPUSATD0_LDMAXBAR;
  }

  SMU->PPUSATD0_SET = SMU_PPUSATD0_HFRCO0;
  SMU->PPUSATD0_SET = SMU_PPUSATD0_GPIO;
  SMU->PPUSATD0_SET = SMU_PPUSATD0_GPCRC;
#if defined(SMU_PPUSATD1_CRYPTOACC)
  SMU->PPUSATD1_SET = SMU_PPUSATD1_CRYPTOACC;
#endif // SMU_PPUSATD1_CRYPTOACC
#if defined(SMU_PPUSATD1_SEMAILBOX)
  SMU->PPUSATD1_SET = SMU_PPUSATD1_SEMAILBOX;
#endif // SMU_PPUSATD1_SEMAILBOX
#endif // BOOTLOADER_DISABLE_MULTI_TIERED_FALLBACK

  bootloader_getPeripheralList(&ppusatd0, &ppusatd1);
  SMU->PPUSATD0_SET = ppusatd0;
  SMU->PPUSATD1_SET = ppusatd1;

#if defined(BOOTLOADER_ENABLE_NVM3_FAULT_HANDLING)
  // Update the peripheral secure access state of
  // the "unknown" peripheral that triggered a fault earlier
  if (blPPUSATDnStateCacheSet == true) {
    SMU->PPUSATD0_SET = blPPUSATDnStateCache[0];
    SMU->PPUSATD1_SET = blPPUSATDnStateCache[1];
  }
#endif // BOOTLOADER_ENABLE_NVM3_FAULT_HANDLING
#if defined(BOOTLOADER_TEST_UNPRIVILEGED_ACCESS)
  enterUnprivilegedMode(enabled);
#endif
}

void bootloader_ppusatdnRestoreState(Bootloader_PPUSATDnCLKENnState_t *ctx)
{
#if defined(BOOTLOADER_TEST_UNPRIVILEGED_ACCESS)
  exitUnprivilegedMode();
#endif

  if (bootloader_ppusatdConfigstate != SAVE) {
    // This function is called from a bootloader callback function
    bootloader_ppusatdConfigstate = SAVE;
    return;
  }

  if (ctx == NULL) {
    return;
  }

  // Wait for any active transition of other busmasters to finish
  if (bootloader_getAllocatedDMAChannel() != -1
      && bootloader_getAllocatedDMAChannel() != BOOTLOADER_ERROR_INIT_STORAGE) {
#if defined(CMU_CLKEN0_LDMA)
    CMU_S->CLKEN0_SET = CMU_CLKEN0_LDMA;
#endif // CMU_CLKEN0_LDMA
    while (LDMA_S->STATUS & LDMA_STATUS_ANYBUSY) ;
  }
#if defined(CRYPTOACC_PRESENT)
  CMU_S->CLKEN1_SET = CMU_CLKEN1_CRYPTOACC;
  #if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_230)
  while (CRYPTOACC_S->DMACTRL_STATUS & (CRYPTOACC_DMACTRL_STATUS_FETCH_BUSY | CRYPTOACC_DMACTRL_STATUS_PUSH_BUSY | CRYPTOACC_DMACTRL_STATUS_SOFT_RST_BUSY)) ;
    #else
  while (CRYPTOACC_S->STATUS & (CRYPTOACC_STATUS_FETCHERBSY | CRYPTOACC_STATUS_PUSHERBSY | CRYPTOACC_STATUS_SOFTRSTBSY)) ;
    #endif
#endif // CRYPTOACC_PRESENT

  SMU->PPUSATD0 = ctx->PPUSATD0;
  SMU->PPUSATD1 = ctx->PPUSATD1;
#if defined(SMU_NS_CFGNS_BASE)
  SMU->PPUPATD0 = ctx->PPUPATD0;
  SMU->PPUPATD1 = ctx->PPUPATD1;
#endif // SMU_NS_CFGNS_BASE
  if (ctx->SMU_STATUS & SMU_STATUS_SMULOCK) {
    SMU->LOCK = 0u;
  }

#if defined(BOOTLOADER_ENABLE_USART_AUTO_DETECTION)
  storeUsartInUse();
#endif // BOOTLOADER_ENABLE_USART_AUTO_DETECTION

  // Restore the CLKENn states
#if defined(_CMU_CLKEN0_MASK)
#if defined(SL_TRUSTZONE_SECURE)
  CMU_NS->CLKEN0 = ctx->CLKEN0;
  CMU_NS->CLKEN1 = ctx->CLKEN1;
#else
  CMU->CLKEN0 = ctx->CLKEN0;
  CMU->CLKEN1 = ctx->CLKEN1;
#endif // SL_TRUSTZONE_SECURE
#endif

  sli_bootloader_postHook();

#if !defined(BOOTLOADER_TEST_UNPRIVILEGED_ACCESS)
  CORE_EXIT_ATOMIC();
#endif

  // Update the state after the critical section has been
  // exited to ensure that the SMU interrupt gets fired,
  // before the state change happens.
  bootloader_ppusatdConfigstate = IDLE;
}

__attribute__ ((weak)) void sli_bootloader_preHook(void)
{
}

__attribute__ ((weak)) void sli_bootloader_postHook(void)
{
}

#if defined(BOOTLOADER_ENABLE_USART_AUTO_DETECTION)
static void preConfigureUsartPPUSATD(void)
{
  if (usartNumberSpi != -1) {
    SMU->PPUSATD0_SET = (SMU_PPUSATD0_USART0 << usartNumberSpi);
    return;
  }

  if (bootloader_InitState != RESET) {
    // The USART taken by the bootloader
    // is still unknown, do nothing.
    return;
  }
#if defined(USART0_BASE)
#if defined(CMU_CLKEN0_USART0)
  CMU->CLKEN0_SET = CMU_CLKEN0_USART0;
#endif // CMU_CLKEN0_USART0

  if (USART0->EN == _USART_EN_RESETVALUE) {
    SMU->PPUSATD0_SET = SMU_PPUSATD0_USART0;
  }
#endif // USART0_BASE

#if defined(USART1_BASE)
#if defined(CMU_CLKEN0_USART1)
  CMU->CLKEN0_SET = CMU_CLKEN0_USART1;
#endif // CMU_CLKEN0_USART1

  if (USART1->EN == _USART_EN_RESETVALUE) {
    SMU->PPUSATD0_SET = SMU_PPUSATD0_USART1;
  }
#endif // USART1_BASE

#if defined(USART2_BASE)
  if (USART2->EN == _USART_EN_RESETVALUE) {
    SMU->PPUSATD0_SET = SMU_PPUSATD0_USART2;
  }
#endif // USART2_BASE
}

static void storeUsartInUse(void)
{
  if ((usartNumberSpi != -1) || (bootloader_InitState != RESET)) {
    return;
  }
#if defined(USART0_BASE)
  if (USART0->EN & USART_EN_EN) {
    usartNumberSpi = 0;
  }
#endif // USART0_BASE

#if defined(USART1_BASE)
  if (USART1->EN & USART_EN_EN) {
    usartNumberSpi = 1;
  }
#endif // USART1_BASE

#if defined(USART2_BASE)
  if (USART2->EN & USART_EN_EN) {
    usartNumberSpi = 2;
  }
#endif // USART2_BASE
}
#endif // BOOTLOADER_ENABLE_USART_AUTO_DETECTION

#if defined(BOOTLOADER_ENABLE_NVM3_FAULT_HANDLING)
void SMU_SECURE_IRQHandler(void)
{
#if !defined(BOOTLOADER_TEST_FAULT_HANDLING)
  if (bootloader_ppusatdConfigstate == IDLE) {
    // If none of the bootloader interface operations are active,
    // the fault is not caused by the bootloader. Just park in the while loop.
    while (true) ;
  }
#endif // BOOTLOADER_TEST_FAULT_HANDLING

  Ecode_t status;
  uint32_t PPUSATDn_state[2] = { 0 };
  nvm3_ObjectKey_t object_id = BL_NVM3_RESERVED_ID;

  // First read the pre-existing configuration
  status = nvm3_readData(nvm3_defaultHandle, object_id, PPUSATDn_state, sizeof(PPUSATDn_state));
  if (status == ECODE_NVM3_OK) {
    if (SMU->PPUFS > 31) {
      PPUSATDn_state[1] |= 1 << (SMU->PPUFS - 32);
    } else {
      PPUSATDn_state[0] |= 1 << SMU->PPUFS;
    }
    // Nothing to do to recover if this fails, just continue and perform a reset.
    (void)nvm3_writeData(nvm3_defaultHandle, object_id, PPUSATDn_state, sizeof(PPUSATDn_state));
  } else if (status == ECODE_NVM3_ERR_KEY_NOT_FOUND) {
    if (SMU->PPUFS > 31) {
      PPUSATDn_state[1] = 1 << (SMU->PPUFS - 32);
    } else {
      PPUSATDn_state[0] = 1 << SMU->PPUFS;
    }
    (void)nvm3_writeData(nvm3_defaultHandle, object_id, PPUSATDn_state, sizeof(PPUSATDn_state));
  } else {
    // Do nothing. If NVM3 read fails for an unknown reason,
    // perform a reset to recover from the failure state.
  }

  BootloaderResetCause_t* resetCause = (BootloaderResetCause_t*) (SRAM_BASE);
  resetCause->reason = BOOTLOADER_RESET_REASON_FAULT;
  resetCause->signature = BOOTLOADER_RESET_SIGNATURE_VALID;
  NVIC_SystemReset();
}
#endif // BOOTLOADER_ENABLE_NVM3_FAULT_HANDLING

#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE
