/***************************************************************************//**
 * @file
 * @brief Secure main for Gecko Bootloader.
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

#include "core/btl_core.h"
#include "core/btl_reset.h"
#include "core/btl_parse.h"
#include "core/btl_bootload.h"
#include "core/btl_upgrade.h"

#include "debug/btl_debug.h"

#if defined(BOOTLOADER_APPLOADER)
#include "sl_device_init_clocks.h"
#endif

#ifdef BTL_GPIO_ACTIVATION
#include "gpio/gpio-activation/btl_gpio_activation.h"
#endif

#ifdef BTL_EZSP_GPIO_ACTIVATION
#include "gpio/ezsp-gpio-activation/btl_ezsp_gpio_activation.h"
#endif

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_chip.h"

// -----------------------------------------------------------------------------
// Defines

#define INFINITE_LOOP() while (1) {}

#if defined(__GNUC__)
#define ROM_END_SIZE 0
extern const size_t __rom_end__;
#elif defined(__ICCARM__)
#define ROM_END_SIZE 4
const size_t __rom_end__ @ "ROM_SIZE";
#endif

// Define Peripherals NS address range for the platform
#define PERIPHERALS_BASE_NS_START (0x50000000)
#define PERIPHERALS_BASE_NS_END   (0xBFFFFFFF)

// -----------------------------------------------------------------------------
// Test Functions

#if defined(TEST_BOOTLOADER_MEMORY_BOUNDARY)
extern void memory_boundary_test(void);
#endif

// -----------------------------------------------------------------------------
// Local function declarations

__STATIC_INLINE bool enterBootloader(void);
SL_NORETURN static void bootToApp(uint32_t);
#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
static void btl_getPeripheralList(uint32_t *ppusatd0, uint32_t *ppusatd1);
#endif
__STATIC_INLINE void lockBootloaderArea(void);

// -----------------------------------------------------------------------------
// TrustZone Utilities

#if defined(__GNUC__)
extern const uint32_t linker_sg_begin;
#elif defined(__ICCARM__)
const uint32_t linker_sg_begin @ "Veneer$$CMSE";
#endif
typedef void (*tz_nonsecure_function_void)(void) __attribute__((cmse_nonsecure_call));

// -----------------------------------------------------------------------------
// Bootloader main stage table

const MainBootloaderTable_t mainStageTable = {
  {
    .type = BOOTLOADER_MAGIC_MAIN,
    .layout = BOOTLOADER_HEADER_VERSION_MAIN,
    .version = BOOTLOADER_VERSION_MAIN
  },
  // Bootloader size is the relative address of the end variable plus 4 for the
  // CRC
  .size = ((uint32_t)&__rom_end__) - BTL_MAIN_STAGE_BASE + ROM_END_SIZE + 4,
  .startOfAppSpace = (BareBootTable_t *)(BTL_APPLICATION_BASE),
  .endOfAppSpace = (void *)(BTL_APPLICATION_BASE + BTL_APP_SPACE_SIZE),
  .capabilities = (0
#if defined(BOOTLOADER_ENFORCE_SIGNED_UPGRADE) && (BOOTLOADER_ENFORCE_SIGNED_UPGRADE == 1)
                   | BOOTLOADER_CAPABILITY_ENFORCE_UPGRADE_SIGNATURE
#endif
#if defined(BOOTLOADER_ENFORCE_ENCRYPTED_UPGRADE) && (BOOTLOADER_ENFORCE_ENCRYPTED_UPGRADE == 1)
                   | BOOTLOADER_CAPABILITY_ENFORCE_UPGRADE_ENCRYPTION
#endif
#if defined(BOOTLOADER_ENFORCE_SECURE_BOOT) && (BOOTLOADER_ENFORCE_SECURE_BOOT == 1)
                   | BOOTLOADER_CAPABILITY_ENFORCE_SECURE_BOOT
#endif
#if defined(BOOTLOADER_SUPPORT_CERTIFICATES) && (BOOTLOADER_SUPPORT_CERTIFICATES == 1)
                   | BOOTLOADER_CAPABILITY_ENFORCE_CERTIFICATE_SECURE_BOOT
#endif
#if defined(BOOTLOADER_ROLLBACK_PROTECTION) && (BOOTLOADER_ROLLBACK_PROTECTION == 1)
                   | BOOTLOADER_CAPABILITY_ROLLBACK_PROTECTION
#endif
                   | BOOTLOADER_CAPABILITY_BOOTLOADER_UPGRADE
                   | BOOTLOADER_CAPABILITY_GBL
                   | BOOTLOADER_CAPABILITY_GBL_SIGNATURE
#if !defined(BTL_PARSER_NO_SUPPORT_ENCRYPTION)
                   | BOOTLOADER_CAPABILITY_GBL_ENCRYPTION
#endif
#if defined(BOOTLOADER_SUPPORT_STORAGE) && (BOOTLOADER_SUPPORT_STORAGE == 1)
                   | BOOTLOADER_CAPABILITY_STORAGE
#endif
#if defined(BOOTLOADER_SUPPORT_COMMUNICATION) && (BOOTLOADER_SUPPORT_COMMUNICATION == 1)
                   | BOOTLOADER_CAPABILITY_COMMUNICATION
#endif
#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
                   | BOOTLOADER_CAPABILITY_PERIPHERAL_LIST
#endif
                   ),
  .init = &btl_init,
  .deinit = &btl_deinit,
  .verifyApplication = &bootload_verifyApplication,
  .initParser = &core_initParser,
  .parseBuffer = &core_parseBuffer,
#if defined(BOOTLOADER_SUPPORT_STORAGE) && (BOOTLOADER_SUPPORT_STORAGE == 1)
  .storage = &storageFunctions,
#else
  .storage = NULL,
#endif
  .parseImageInfo = core_parseImageInfo,
  .parserContextSize = core_parserContextSize,
#if defined(BOOTLOADER_ROLLBACK_PROTECTION) && (BOOTLOADER_ROLLBACK_PROTECTION == 1)
  .remainingApplicationUpgrades = &bootload_remainingApplicationUpgrades,
#else
  .remainingApplicationUpgrades = NULL,
#endif
#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
  .getPeripheralList = &btl_getPeripheralList
#else
  .getPeripheralList = NULL
#endif
};

#if defined(BOOTLOADER_SUPPORT_CERTIFICATES) && (BOOTLOADER_SUPPORT_CERTIFICATES == 1)
const ApplicationCertificate_t sl_app_certificate = {
  .structVersion = APPLICATION_CERTIFICATE_VERSION,
  .flags = { 0U },
  .key = { 0U },
  .version = 0,
  .signature = { 0U },
};
#endif

const ApplicationProperties_t sl_app_properties = {
  .magic = APPLICATION_PROPERTIES_MAGIC,
  .structVersion = APPLICATION_PROPERTIES_VERSION,
  .signatureType = APPLICATION_SIGNATURE_NONE,
  .signatureLocation = ((uint32_t)&__rom_end__) - BTL_MAIN_STAGE_BASE + ROM_END_SIZE,
  .app = {
    .type = APPLICATION_TYPE_BOOTLOADER,
    .version = BOOTLOADER_VERSION_MAIN,
    .capabilities = 0UL,
    .productId = { 0U },
  },
#if defined(BOOTLOADER_SUPPORT_CERTIFICATES) && (BOOTLOADER_SUPPORT_CERTIFICATES == 1)
  // If certificate based boot chain is enabled, the bootloader binary will be provided with
  // a certificate that does not contain any key.
  // A valid certificate needs to be injected to the bootloader images using Simplicity Commander.
  // Simplicity Commander will replace this certificate.
  .cert = (ApplicationCertificate_t *)&sl_app_certificate,
#else
  .cert = NULL,
#endif
  .longTokenSectionAddress = NULL,
  .decryptKey = { 0u },
};

// -----------------------------------------------------------------------------
// Faults

void MemManage_Handler(void)
{
  #if defined(DEBUG_EFM)
  INFINITE_LOOP();
  #else
  reset_resetWithReason(BOOTLOADER_RESET_REASON_FATAL);
  #endif
}

void HardFault_Handler(void)
{
  #if defined(DEBUG_EFM)
  INFINITE_LOOP();
  #else
  reset_resetWithReason(BOOTLOADER_RESET_REASON_FATAL);
  #endif
}

#if defined(SCB_SHCSR_USGFAULTENA_Msk)
void UsageFault_Handler(void)
{
  #if defined(DEBUG_EFM)
  INFINITE_LOOP();
  #else
  reset_resetWithReason(BOOTLOADER_RESET_REASON_FATAL);
  #endif
}
#endif

#if defined(SCB_SHCSR_BUSFAULTENA_Msk)
void BusFault_Handler(void)
{
  #if defined(DEBUG_EFM)
  INFINITE_LOOP();
  #else
  reset_resetWithReason(BOOTLOADER_RESET_REASON_FATAL);
  #endif
}
#endif

void SMU_SECURE_IRQHandler(void)
{
  #if defined(DEBUG_EFM)
  INFINITE_LOOP();
  #else
  reset_resetWithReason(BOOTLOADER_RESET_REASON_TZ_FAULT);
  #endif
}

void SecureFault_Handler(void)
{
  #if defined(DEBUG_EFM)
  INFINITE_LOOP();
  #else
  reset_resetWithReason(BOOTLOADER_RESET_REASON_TZ_FAULT);
  #endif
}

#if defined(BOOTLOADER_APPLOADER)
/**************************************************************************//**
 * @brief Configures secure state of bus masters using the SMU.
 *
 * This function is device dependent. Assumes that the SMU is clocked.
 *****************************************************************************/
static inline void smu_configure_bus_masters(void)
{
  // Configure all bus-masters as secure except for the RADIOSUBSYSTEM.
  SMU->BMPUSATD0_CLR = SMU_BMPUSATD0_RADIOSUBSYSTEM;
}
#endif // BOOTLOADER_APPLOADER

#if defined(BOOTLOADER_APPLOADER)
/**************************************************************************//**
 * @brief Configures secure attributes of peripherals using the SMU.
 *
 * This function is device dependent. Assumes that the SMU is clocked.
 *****************************************************************************/
static inline void smu_configure_peripherals(void)
{
  #if defined(SEMAILBOX_PRESENT)
  SMU->PPUSATD0_CLR = _SMU_PPUSATD0_MASK
                      & ~(SMU_PPUSATD0_SYSCFG
                          | SMU_PPUSATD0_MSC
                          | SMU_PPUSATD0_GPCRC
                          | SMU_PPUSATD0_LDMA
                          | SMU_PPUSATD0_LDMAXBAR);
  SMU->PPUSATD1_CLR = _SMU_PPUSATD1_MASK
                      & ~(SMU_PPUSATD1_SMU
                          | SMU_PPUSATD1_SEMAILBOX);
  #elif defined(CRYPTOACC_PRESENT)
  SMU->PPUSATD0_CLR = _SMU_PPUSATD0_MASK
                      & ~(SMU_PPUSATD0_SYSCFG
                          | SMU_PPUSATD0_MSC
                          | SMU_PPUSATD0_GPCRC
                          | SMU_PPUSATD0_LDMA
                          | SMU_PPUSATD0_LDMAXBAR);
  SMU->PPUSATD1_CLR = _SMU_PPUSATD1_MASK
                      & ~(SMU_PPUSATD1_SMU
                          | SMU_PPUSATD1_CRYPTOACC);
  #endif
}
#endif // BOOTLOADER_APPLOADER

#if defined(BOOTLOADER_APPLOADER)
/**************************************************************************//**
 * @brief Configure interrupt target states.
 *
 * Interrupts must either point at the secure or non-secure world. After reset
 * everything points to the secure world, and this function redirects all
 * intterupts to non-secure to better support existing applications.
 *****************************************************************************/
static inline void configure_interrupt_target_states(void)
{
  // Start by setting all Interrupt Non-Secure State (ITNS) bits. This results
  // in all IRQs being targeted at the NS world.
  for (size_t i = 0; i < sizeof(NVIC->ITNS) / sizeof(NVIC->ITNS[0]); i++) {
    NVIC->ITNS[i] = 0xFFFFFFFF;
  }

  // Clear the ITNS bits corresponding to all IRQs belonging to S peripherals.
  #if defined(SEMAILBOX_PRESENT)
  NVIC_ClearTargetState(SEMBRX_IRQn);
  NVIC_ClearTargetState(SEMBTX_IRQn);
  NVIC_ClearTargetState(SMU_SECURE_IRQn);
  #if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
  NVIC_ClearTargetState(SMU_PRIVILEGED_IRQn);
  #else
  NVIC_ClearTargetState(SMU_S_PRIVILEGED_IRQn);
  #endif
  NVIC_ClearTargetState(LDMA_IRQn);
  NVIC_ClearTargetState(SYSCFG_IRQn);
  NVIC_ClearTargetState(MSC_IRQn);
  #elif defined(CRYPTOACC_PRESENT)
  NVIC_ClearTargetState(CRYPTOACC_IRQn);
  NVIC_ClearTargetState(TRNG_IRQn);
  NVIC_ClearTargetState(PKE_IRQn);
  NVIC_ClearTargetState(SMU_SECURE_IRQn);
  NVIC_ClearTargetState(SMU_S_PRIVILEGED_IRQn);
  NVIC_ClearTargetState(LDMA_IRQn);
  NVIC_ClearTargetState(SYSCFG_IRQn);
  NVIC_ClearTargetState(MSC_IRQn);
  #endif
}
#endif // BOOTLOADER_APPLOADER

/**************************************************************************//**
 * @brief Enable SMU security fault interrupts.
 *
 * Assumes that the SMU is clocked.
 *****************************************************************************/
static inline void smu_enable_security_fault_interrupts(void)
{
  NVIC_ClearPendingIRQ(SMU_SECURE_IRQn);
  SMU->IF_CLR = SMU_IF_PPUSEC | SMU_IF_BMPUSEC;
  NVIC_EnableIRQ(SMU_SECURE_IRQn);
  SMU->IEN = SMU_IEN_PPUSEC | SMU_IEN_BMPUSEC;
}

/**************************************************************************//**
 * @brief Enables SecureFault, BusFault, UsageFault, and MemFault system
 *        exceptions.
 *****************************************************************************/
static inline void enable_fault_exceptions(void)
{
  // Enable SecureFault, BusFault, UsageFault and MemFault.
  SCB->SHCSR |= SCB_SHCSR_SECUREFAULTENA_Msk
                | SCB_SHCSR_BUSFAULTENA_Msk
                | SCB_SHCSR_USGFAULTENA_Msk
                | SCB_SHCSR_MEMFAULTENA_Msk;
}

/**************************************************************************//**
 * @brief Enable the FPU for the non-secure app.
 *****************************************************************************/
static inline void enable_ns_fpu(void)
{
  SCB->NSACR |= (SCB_NSACR_CP10_Msk)    // enable CP10 Full Access
                | (SCB_NSACR_CP11_Msk); // enable CP11 Full Access
}

/**************************************************************************//**
 * @brief Prioritize the secure exceptions
 *
 * Modify the relative priorities of Secure and Non-secure interrupts,
 * so that the priority range for Secure interrupts extends to higher
 * priorities than the range for Non-secure interrupts.
 *****************************************************************************/
static inline void prioritise_secure_exceptions(void)
{
  #define AIRCR_UNLOCK_VECTKEY 0x5FAUL

  uint32_t scb_AIRCR = SCB->AIRCR;
  uint32_t scb_vectkey = AIRCR_UNLOCK_VECTKEY;
  SCB->AIRCR = SCB_AIRCR_PRIS_Msk
               | (scb_vectkey << SCB_AIRCR_VECTKEY_Pos)
               | (scb_AIRCR & ~SCB_AIRCR_VECTKEY_Msk);
}

/**************************************************************************//**
 * @brief Jump to the NS bootloader
 *****************************************************************************/
static void jump_to_ns(void)
{
  // Set stack pointer
  __TZ_set_MSP_NS(*((uint32_t *)(FLASH_BASE + NS_FLASH_OFFSET)));
  __TZ_set_PSP_NS(*((uint32_t *)(FLASH_BASE + NS_FLASH_OFFSET)));
  SCB_NS->VTOR = (FLASH_BASE + NS_FLASH_OFFSET);

  // Call non-secure reset handler
  ((tz_nonsecure_function_void)(*((uint32_t *)((FLASH_BASE + NS_FLASH_OFFSET) + 4UL))))();

  // Should never reach this point
  reset_resetWithReason(BOOTLOADER_RESET_REASON_FATAL);
}

/**************************************************************************//**
 * @brief Setup MPU
 *
 * Configures the application space as non-executable utilizing MPU
 *****************************************************************************/
static void setup_mpu(void)
{
  ARM_MPU_SetMemAttr(0UL, ARM_MPU_ATTR(                          // Normal memory
                       ARM_MPU_ATTR_MEMORY_(1UL, 1UL, 1UL, 1UL), // Outer non-transient write-back with read and write cache allocate
                       ARM_MPU_ATTR_MEMORY_(1UL, 1UL, 1UL, 1UL)  // Inner non-transient write-back with read and write cache allocate
                       ));
  ARM_MPU_SetMemAttr(1UL, ARM_MPU_ATTR(
                       ARM_MPU_ATTR_MEMORY_(0UL, 1UL, 0UL, 0UL),
                       ARM_MPU_ATTR_MEMORY_(0UL, 1UL, 0UL, 0UL)
                       ));
  ARM_MPU_SetMemAttr(2UL, ARM_MPU_ATTR_DEVICE_nGnRnE);

  // MPU S region
  ARM_MPU_Disable();
  // Application region is XN
  uint32_t rbarVal = (BTL_APPLICATION_BASE & MPU_RBAR_BASE_Msk)
                     | ((ARM_MPU_SH_NON << MPU_RBAR_SH_Pos) & MPU_RBAR_SH_Msk)
                     | ((ARM_MPU_AP_(0U, 1U) << MPU_RBAR_AP_Pos) & MPU_RBAR_AP_Msk)
                     | ((1U << MPU_RBAR_XN_Pos) & MPU_RBAR_XN_Msk); // Non-executable
  uint32_t rlarVal = ARM_MPU_RLAR((FLASH_BASE + FLASH_SIZE - 1), 0UL);
  ARM_MPU_SetRegion(0, rbarVal, rlarVal);
  ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

  // MPU NS region
  ARM_MPU_Disable_NS();
  // Application space is XN
  rbarVal = (BTL_APPLICATION_BASE & MPU_RBAR_BASE_Msk)
            | ((ARM_MPU_SH_NON << MPU_RBAR_SH_Pos) & MPU_RBAR_SH_Msk)
            | ((ARM_MPU_AP_(0U, 1U) << MPU_RBAR_AP_Pos) & MPU_RBAR_AP_Msk)
            | ((1U << MPU_RBAR_XN_Pos) & MPU_RBAR_XN_Msk); // Non-executable
  rlarVal = ARM_MPU_RLAR((FLASH_BASE + FLASH_SIZE - 1), 0UL);
  ARM_MPU_SetRegion_NS(0, rbarVal, rlarVal);
  ARM_MPU_Enable_NS(MPU_CTRL_PRIVDEFENA_Msk);

  __DSB();
  __ISB();
}

/**************************************************************************//**
 * @brief Setup TrustZone boundaries
 *
 * Sets up the static isolation boundaries which are constant throughout
 * the runtime of the system. Assumes that the SMU is clocked.
 *****************************************************************************/
static void setup_static_boundaries(void)
{
  // Memory map configuration
  // Uses the SMU to split flash into S/NSC/NS, and RAM into S/NS.
  SMU->LOCK = SMU_LOCK_SMULOCKKEY_UNLOCK;

  // Flash configuration
  // S - NSC boundary
  SMU->ESAUMRB01 = FLASH_BASE & _SMU_ESAUMRB01_MASK;
  // NSC - NS boundary
  SMU->ESAUMRB12 = (FLASH_BASE + NS_FLASH_OFFSET) & _SMU_ESAUMRB12_MASK;

  // RAM configuration
  // S - NSC boundary
  SMU->ESAUMRB45 = (SRAM_BASE + NS_RAM_OFFSET) & _SMU_ESAUMRB45_MASK;
  // NSC - NS boundary
  SMU->ESAUMRB56 = (SRAM_BASE + NS_RAM_OFFSET) & _SMU_ESAUMRB56_MASK;

#if defined(BOOTLOADER_APPLOADER)
  // Make the info page NS.
  SMU->ESAURTYPES0 = SMU_ESAURTYPES0_ESAUR3NS;
#endif

  // Lock SMU config
  SMU->LOCK = 0;

  SAU->RNR = 0; // Flash
  SAU->RBAR = ((uint32_t)&linker_sg_begin) & SAU_RBAR_BADDR_Msk;
  SAU->RLAR = ((BTL_APPLICATION_BASE - 1u) & SAU_RLAR_LADDR_Msk)
              | (0 << SAU_RLAR_NSC_Pos)
              | SAU_RLAR_ENABLE_Msk;
  SAU->RNR = 1; // NS Peripherals
  SAU->RBAR = PERIPHERALS_BASE_NS_START & SAU_RBAR_BADDR_Msk;
  SAU->RLAR = ((PERIPHERALS_BASE_NS_END) &SAU_RLAR_LADDR_Msk)
              | (0 << SAU_RLAR_NSC_Pos)
              | SAU_RLAR_ENABLE_Msk;
  SAU->RNR = 2; // SRAM
  SAU->RBAR = (SRAM_BASE + NS_RAM_OFFSET) & SAU_RBAR_BADDR_Msk;
  SAU->RLAR = ((SRAM_BASE + SRAM_SIZE - 1u) & SAU_RLAR_LADDR_Msk)
              | (0 << SAU_RLAR_NSC_Pos)
              | SAU_RLAR_ENABLE_Msk;
#if defined(BOOTLOADER_APPLOADER)
  SAU->RNR = 3; // User Data
  SAU->RBAR = MSC_FLASH_USERDATA_MEM_BASE & SAU_RBAR_BADDR_Msk;
  SAU->RLAR = (MSC_FLASH_USERDATA_MEM_END & SAU_RLAR_LADDR_Msk)
              | (0 << SAU_RLAR_NSC_Pos)
              | SAU_RLAR_ENABLE_Msk;
  SAU->RNR = 4; // Device Info
  SAU->RBAR = MSC_FLASH_DEVINFO_MEM_BASE & SAU_RBAR_BADDR_Msk;
  SAU->RLAR = (MSC_FLASH_DEVINFO_MEM_END & SAU_RLAR_LADDR_Msk)
              | (0 << SAU_RLAR_NSC_Pos)
              | SAU_RLAR_ENABLE_Msk;
  SAU->RNR = 5; // Chip Config
  SAU->RBAR = MSC_FLASH_CHIPCONFIG_MEM_BASE & SAU_RBAR_BADDR_Msk;
  SAU->RLAR = (MSC_FLASH_CHIPCONFIG_MEM_END & SAU_RLAR_LADDR_Msk)
              | (0 << SAU_RLAR_NSC_Pos)
              | SAU_RLAR_ENABLE_Msk;
#endif // BOOTLOADER_APPLOADER
  TZ_SAU_Enable();
  __DSB();
  __ISB();

  enable_ns_fpu();
  setup_mpu();
#if defined(TEST_BOOTLOADER_MEMORY_BOUNDARY)
  memory_boundary_test();
#endif
}

__STATIC_INLINE void lockBootloaderArea(void)
{
  // Disable write access to bootloader.
  // Prevents application from touching the bootloader.
#if defined(_CMU_CLKEN1_MASK)
  CMU->CLKEN1_SET = CMU_CLKEN1_MSC;
#endif

  uint32_t num_pages = (BTL_MAIN_STAGE_MAX_SIZE + BTL_FIRST_STAGE_SIZE) / FLASH_PAGE_SIZE;
  uint32_t pagelock = ~0;
  pagelock = pagelock >> (32u - num_pages);
  MSC->PAGELOCK0_SET = pagelock;

#if defined(_CMU_CLKEN1_MASK)
  CMU->CLKEN1_CLR = CMU_CLKEN1_MSC;
#endif
}

/**
 * Jump to app
 */
SL_NORETURN static void bootToApp(uint32_t startOfAppSpace)
{
  (void)startOfAppSpace;

  // Load SP and PC of application
  __ASM("mov r0, %0       \n" // Load address of SP into R0
        "ldr r1, [r0]     \n" // Load SP into R1
        "msr msp, r1      \n" // Set MSP
        "msr psp, r1      \n" // Set PSP
        "ldr r0, [r0, #4] \n" // Load PC into R0
        "mov pc, r0       \n" // Set PC
        :: "r" (startOfAppSpace) : "r0", "r1");

  while (1) {
    // Do nothing
  }
}

/**
 * Check whether we should enter the bootloader
 *
 * @return True if the bootloader should be entered
 */
__STATIC_INLINE bool enterBootloader(void)
{
// *INDENT-OFF*
#if defined(EMU_RSTCAUSE_SYSREQ)
  if (EMU->RSTCAUSE & EMU_RSTCAUSE_SYSREQ) {
#else
  if (RMU->RSTCAUSE & RMU_RSTCAUSE_SYSREQRST) {
#endif
    // Check if we were asked to run the bootloader...
    switch (reset_classifyReset()) {
      case BOOTLOADER_RESET_REASON_BOOTLOAD:
      case BOOTLOADER_RESET_REASON_FORCE:
      case BOOTLOADER_RESET_REASON_UPGRADE:
      case BOOTLOADER_RESET_REASON_BADAPP:
        // Asked to go into bootload mode
        return true;
      default:
        break;
    }
  }
// *INDENT-ON*

#ifdef BTL_GPIO_ACTIVATION
  if (gpio_enterBootloader()) {
    // GPIO pin state signals bootloader entry
    return true;
  }
#endif

#ifdef BTL_EZSP_GPIO_ACTIVATION
  if (ezsp_gpio_enterBootloader()) {
    // GPIO pin state signals bootloader entry
    return true;
  }
#endif

  return false;
}

#if defined(BOOTLOADER_INTERFACE_TRUSTZONE_AWARE)
static void btl_getPeripheralList(uint32_t *ppusatd0, uint32_t *ppusatd1)
{
  if (ppusatd0 == NULL || ppusatd1 == NULL) {
    return;
  }
  *ppusatd0 = 0u;
  *ppusatd1 = 0u;

  *ppusatd0 |= SMU_PPUSATD0_MSC;
  *ppusatd0 |= SMU_PPUSATD0_CMU;
  *ppusatd0 |= SMU_PPUSATD0_HFRCO0;
  *ppusatd0 |= SMU_PPUSATD0_GPIO;
  *ppusatd0 |= SMU_PPUSATD0_GPCRC;

#ifdef BOOTLOADER_SUPPORT_STORAGE
  if (storage_getDMAchannel() != -1) {
    *ppusatd0 |= SMU_PPUPATD0_LDMA;
    *ppusatd0 |= SMU_PPUSATD0_LDMAXBAR;
  }

#if defined(BTL_SPI_USART_ENABLE) || defined(BTL_SPI_EUSART_ENABLE)
  uint32_t ppusatdNr = 0xFFFFFFFFUL;
  uint32_t ppusatdUsartMask = storage_getSpiUsartPPUSATD(&ppusatdNr);
  if (ppusatdNr == 0u) {
    *ppusatd0 |= ppusatdUsartMask;
  } else if (ppusatdNr == 1u) {
    *ppusatd1 |= ppusatdUsartMask;
  }
#endif
#endif // BOOTLOADER_SUPPORT_STORAGE

#if defined(SMU_PPUSATD1_CRYPTOACC)
  *ppusatd1 |= SMU_PPUSATD1_CRYPTOACC;
#endif // SMU_PPUSATD1_CRYPTOACC
#if defined(SMU_PPUPATD1_SEMAILBOX)
  *ppusatd1 |= SMU_PPUPATD1_SEMAILBOX;
#endif // SMU_PPUPATD1_SEMAILBOX
}
#endif // BOOTLOADER_INTERFACE_TRUSTZONE_AWARE

/**
 * This function gets executed before anything got initialized.
 * So, no using global variables here!
 */
void SystemInit2(void)
{
  // Initialize debug before first debug print
  BTL_DEBUG_INIT();

  // Assumption: We should enter the app
  volatile bool enterApp = true;
  // Assumption: The app should be verified
  volatile bool verifyApp = true;

  if (enterBootloader()) {
    // We want to enter the bootloader, app doesn't need to be verified
    enterApp = false;
    verifyApp = false;
  }
  uint32_t startOfAppSpace = (uint32_t)mainStageTable.startOfAppSpace;

  // Sanity check application program counter
  uint32_t pc = *(uint32_t *)(startOfAppSpace + 4);
  if (pc == 0xFFFFFFFF) {
    // Sanity check failed; enter the bootloader
    reset_setResetReason(BOOTLOADER_RESET_REASON_BADAPP);
    enterApp = false;
    verifyApp = false;
  }

  // App should be verified
  if (verifyApp) {
    // If app verification fails, enter bootloader instead
    enterApp = bootload_verifyApplication(startOfAppSpace);
    if (!enterApp) {
      BTL_DEBUG_PRINTLN("App verify fail");
      reset_setResetReason(BOOTLOADER_RESET_REASON_BADAPP);
    }
  }

#if defined(BOOTLOADER_ROLLBACK_PROTECTION) && (BOOTLOADER_ROLLBACK_PROTECTION == 1)
  // Clean the stored application versions if requested with a magic.
  // The magic is only written when a bootloader upgrade is triggered.
  bootload_removeStoredApplicationVersions();

  if (enterApp) {
    enterApp = bootload_storeApplicationVersion(startOfAppSpace);
  }
#endif

  if (enterApp) {
    BTL_DEBUG_PRINTLN("Enter app");
    BTL_DEBUG_PRINT_LF();

#if defined(BOOTLOADER_SUPPORT_STORAGE)
    // Disable the reset counter if we're booting (back) into the application
    if (reset_resetCounterEnabled()) {
      reset_disableResetCounter();
    }
#endif

#if defined(BOOTLOADER_WRITE_DISABLE) && (BOOTLOADER_WRITE_DISABLE == 1)
    lockBootloaderArea();
#endif

#if defined(BOOTLOADER_ENFORCE_SECURE_BOOT) && (BOOTLOADER_ENFORCE_SECURE_BOOT == 1) \
    && defined(APPLICATION_WRITE_DISABLE) && (APPLICATION_WRITE_DISABLE == 1)
    // The neccessary check of valid signature pointer for application at startOfAppSpace
    // is already done in bootload_verifyApplication.
    bootload_lockApplicationArea(startOfAppSpace, 0);
#endif

    // Set vector table to application's table
    SCB->VTOR = startOfAppSpace;

    // The secure state configurations does not need to be reset since
    // we have been through a reset to get to this point.
    bootToApp(startOfAppSpace);
  }
  // Enter bootloader
}

// Main Bootloader implementation
int main(void)
{
  CHIP_Init();
#if defined(BOOTLOADER_APPLOADER)
  sl_device_init_clocks();
#endif
  BTL_DEBUG_PRINTLN("BTL entry");

#if defined(CMU_CLKEN1_SMU)
  CMU->CLKEN1_SET = CMU_CLKEN1_SMU;
#endif

  enable_fault_exceptions();
  prioritise_secure_exceptions();
  smu_enable_security_fault_interrupts();

#if defined(EMU_CMD_EM01VSCALE2) && defined(EMU_STATUS_VSCALEBUSY)
  // Device supports voltage scaling, and the bootloader may have been entered
  // with a downscaled voltage. Scale voltage up to allow flash programming.
  if ((EMU->STATUS & EMU_STATUS_VSCALE_VSCALE2) != EMU_STATUS_VSCALE_VSCALE2) {
    EMU->CMD = EMU_CMD_EM01VSCALE2;
    while (EMU->STATUS & EMU_STATUS_VSCALEBUSY) {
      // Do nothing
    }
  }
#endif
#if defined(BOOTLOADER_APPLOADER)
  // Only redirect the interrupts to non-secure for the AppLoader usecase.
  // All the interrupts can be owned by the secure code for all the other bootloader
  // sample apps.
  configure_interrupt_target_states();
  // All the peripherals as well as the busmasters are considered secure for all the
  // bootloader sample apps except for the AppLoader.
  smu_configure_peripherals();
  smu_configure_bus_masters();
#endif // BOOTLOADER_APPLOADER
  setup_static_boundaries();

  // Lock the whole bootloader flash unconditionally
  lockBootloaderArea();

#if defined(CMU_CLKEN1_SMU)
#if defined(BOOTLOADER_APPLOADER)
  CMU_NS->CLKEN1_CLR = CMU_CLKEN1_SMU;
#else
  CMU->CLKEN1_CLR = CMU_CLKEN1_SMU;
#endif // BOOTLOADER_APPLOADER
#endif // CMU_CLKEN1_SMU

  btl_init();
  jump_to_ns();

  // Should never reach this point
  reset_resetWithReason(BOOTLOADER_RESET_REASON_FATAL);
  return 0;
}
