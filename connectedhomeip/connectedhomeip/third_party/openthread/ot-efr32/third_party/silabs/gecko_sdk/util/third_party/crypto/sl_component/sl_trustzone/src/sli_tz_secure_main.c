/***************************************************************************//**
 * @file
 * @brief Silicon Labs TrustZone secure main implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#if defined (SL_COMPONENT_CATALOG_PRESENT)
  #include "sl_component_catalog.h"
#endif

#include "arm_cmse.h"
#include "em_device.h"
#include "psa/crypto.h"

#if defined(SL_CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT)
  #include "sl_mpu.h"
#endif

#if defined(SEMAILBOX_PRESENT)
  #include "sl_se_manager_util.h"
#else
  #include "psa/internal_trusted_storage.h"
  #include "sli_psa_driver_common.h"
#endif

#include "sli_se_version_dependencies.h"

// -----------------------------------------------------------------------------
// External symbols

#if defined(__GNUC__)
extern const uint32_t linker_sg_begin;
extern const uint32_t linker_vectors_begin;
#elif defined(__ICCARM__)
const uint32_t linker_sg_begin @ "Veneer$$CMSE";
const uint32_t linker_vectors_begin @ ".intvec";
#endif

#if defined(MBEDTLS_PSA_CRYPTO_STORAGE_C) && defined(SLI_PSA_ITS_ENCRYPTED)
// Extern declaration is required due to the duplicate header used when
// the ITS service is compiled for the SKL secure appication.
extern psa_status_t sli_psa_its_set_root_key(uint8_t *root_key, size_t root_key_size);
#endif

// -----------------------------------------------------------------------------
// Typedefs

typedef void (*nsfunc)(void) __attribute__((cmse_nonsecure_call));

// -----------------------------------------------------------------------------
// Macros

#define INFINITE_LOOP() while (1) {}

#define AIRCR_UNLOCK_VECTKEY 0x5FAUL

// Mark the full range below as NS in SAU,
// and let the PPU generate access faults instead.
#define PERIPHERALS_BASE_NS_START (0x50000000)
#define PERIPHERALS_BASE_NS_END   (0xBFFFFFFF)

#define EPPB_REGION_START (0xE0044000)
#define EPPB_REGION_END   (0xE00FDFFF)

// Lock every page up until the NS app.
#define NUM_PAGES_TO_LOCK ((TZ_NS_FLASH_OFFSET + FLASH_PAGE_SIZE - 1) \
                           / FLASH_PAGE_SIZE)

// Each page is represented as a bit in the registers.
#define NUM_PAGELOCK_REGISTERS (FLASH_SIZE / FLASH_PAGE_SIZE / 32)

#if defined (__ICCARM__)
  #define BUILTIN_UNREACHABLE()
#elif defined (__GNUC__)
  #define BUILTIN_UNREACHABLE() __builtin_unreachable();
#endif

// -----------------------------------------------------------------------------
// Static function declarations

static inline void configure_sau(void);
static inline void configure_smu(void);
static inline void smu_configure_peripherals(void);
static inline void smu_configure_bus_masters(void);
static inline void smu_enable_security_fault_interrupts(void);
static inline void configure_interrupt_target_states(void);
static inline void prioritise_secure_exceptions(void);
static inline void enable_fault_exceptions(void);
static inline void enable_ns_fpu(void);
static inline void enable_page_locks(void);
static inline void start_ns_app(void);
#if defined(CRYPTOACC_PRESENT)
static inline void clear_rootdata(void);
#endif
static inline void fatal_error(void);

#if defined(SL_CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT)
static inline void configure_mpu(void);
#endif

#if defined(MBEDTLS_PSA_CRYPTO_STORAGE_C) && defined(SLI_PSA_ITS_ENCRYPTED)
#if defined(SEMAILBOX_PRESENT)
static inline void verify_hse_version(void);
#else
static inline void setup_storage_root_key(void);
#endif
#endif // defined(MBEDTLS_PSA_CRYPTO_STORAGE_C) && defined(SLI_PSA_ITS_ENCRYPTED)

// -----------------------------------------------------------------------------
// Static constants

// We will use an array of pagelock registers in order to keep
// the lock setting logic from becoming unreadable. This also
// means that the code will handle differences in flash size.
static volatile uint32_t *const pagelock_registers[] = {
  #if defined(_MSC_PAGELOCKWORD0_RESETVALUE)
  &MSC->PAGELOCKWORD0_SET,
  #elif defined(_MSC_PAGELOCK0_RESETVALUE)
  &MSC->PAGELOCK0_SET,
  #else
  // All devices must have at least one pagelock register.
  #error "Unable to lock pages for this device"
  #endif
  #if defined(_MSC_PAGELOCKWORD1_RESETVALUE)
  &MSC->PAGELOCKWORD1_SET,
  #elif defined(_MSC_PAGELOCK1_RESETVALUE)
  &MSC->PAGELOCK1_SET,
  #endif
  #if defined(_MSC_PAGELOCKWORD2_RESETVALUE)
  &MSC->PAGELOCKWORD2_SET,
  #elif defined(_MSC_PAGELOCK2_RESETVALUE)
  &MSC->PAGELOCK2_SET,
  #endif
  #if defined(_MSC_PAGELOCKWORD3_RESETVALUE)
  &MSC->PAGELOCKWORD3_SET,
  #elif defined(_MSC_PAGELOCK3_RESETVALUE)
  &MSC->PAGELOCK3_SET,
  #endif
  #if defined(_MSC_PAGELOCKWORD4_RESETVALUE)
  &MSC->PAGELOCKWORD4_SET,
  #endif
  #if defined(_MSC_PAGELOCKWORD5_RESETVALUE)
  &MSC->PAGELOCKWORD5_SET,
  #endif
  #if defined(_MSC_PAGELOCKWORD6_RESETVALUE)
  &MSC->PAGELOCKWORD6_SET,
  #endif
  #if defined(_MSC_PAGELOCKWORD7_RESETVALUE)
  &MSC->PAGELOCKWORD7_SET,
  #endif
};

// -----------------------------------------------------------------------------
// Test functions

#if defined(SLI_TZ_TEST_MEMORY_BOUNDARY)
extern void tz_test_memory_boundary(void);
#if defined(CRYPTOACC_PRESENT)
extern void tz_test_memory_prepare(void);
#endif // CRYPTOACC_PRESENT
#endif

// -----------------------------------------------------------------------------
// Main function

int main(void)
{
  enable_fault_exceptions();

  // Prioritise secure exceptions to avoid NS being able to pre-empt
  // secure SVC or SecureFault.
  prioritise_secure_exceptions();

  // Configure interrupt target states before any secure interrupts are enabled.
  configure_interrupt_target_states();

  #if defined(MBEDTLS_PSA_CRYPTO_STORAGE_C) && defined(SLI_PSA_ITS_ENCRYPTED)
  #if defined(SEMAILBOX_PRESENT)
  // HSE devices don't need any SRK initialization, since it will be available as an internal
  // key. However, we'll still check the SE version to ensure that SRK is supported.
  verify_hse_version();
  #else // SEMAILBOX_PRESENT
  // Setup SRK for devices without an HSE. (The SE version is checked as part of this
  // procedure.) Note that this is done before setting up the SAU/SMU since the root key is
  // stored in FRCRAM which is not guaranteed to stay Secure after TZ initialization. We
  // therefore need to initialize and clear the SRK passed from the VSE before setting
  // anything to Non-Secure.
  setup_storage_root_key();
  #endif // SEMAILBOX_PRESENT
  #endif // defined(MBEDTLS_PSA_CRYPTO_STORAGE_C) && defined(SLI_PSA_ITS_ENCRYPTED)

  #if defined(CRYPTOACC_PRESENT)
  #if defined(SLI_TZ_TEST_MEMORY_BOUNDARY)
  tz_test_memory_prepare();
  #endif
  // At the startup of the VSE devices, we need to invalidate the
  // ROOTDATA0 register which is only accessible by S to ensure that NS apps
  // can't craft a malicious mailbox command. This is a possible scenario since
  // FRCRAM, which is used as the (virtual) mailbox interface has to be configured as NS.
  clear_rootdata();
  #endif // CRYPTOACC_PRESENT

  // We should configure the SAU as early as possible in order to avoid odd
  // PPU security faults when accessing peripherals (e.g. CMU) after they have
  // been reconfigured by the SMU.
  configure_sau();
  configure_smu();
  #if defined(SL_CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT)
  configure_mpu();
  #endif // SL_CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT

  // The NS app should not be able to write to secure flash (including the
  // bootloader). As an extra layer of protection, we will enable flash page
  // locks. This also stops the NS app from being able to erase the S code
  // using the MSC service, which is another benefit.
  enable_page_locks();

  #if defined(SLI_TZ_TEST_MEMORY_BOUNDARY)
  tz_test_memory_boundary();
  #endif // SLI_TZ_TEST_MEMORY_BOUNDARY

  psa_crypto_init();

  enable_ns_fpu();

  start_ns_app();

  // Will never get here, as non-secure software doesn't return.
  INFINITE_LOOP();
}

// -----------------------------------------------------------------------------
// Static functions

/**************************************************************************//**
 * @brief Handle a fault by performing a soft reset.
 *****************************************************************************/
static inline void fatal_error(void)
{
  #if defined(DEBUG_EFM)
  INFINITE_LOOP();
  #else
  NVIC_SystemReset();
  BUILTIN_UNREACHABLE();
  #endif
}

#if defined(CRYPTOACC_PRESENT)
/**************************************************************************//**
 * @brief Clear the ROOTDATA0 register
 *****************************************************************************/
static inline void clear_rootdata(void)
{
  #if defined(CMU_CLKEN0_SYSCFG)
  // Saving the state is not necessary.
  CMU_S->CLKEN0_SET = CMU_CLKEN0_SYSCFG;
  #endif

  // We only clear ROOTDATA0, which is used as the input channel.
  SYSCFG_S->ROOTDATA0 = _SYSCFG_ROOTDATA0_RESETVALUE;

  #if defined(CMU_CLKEN0_SYSCFG)
  CMU_S->CLKEN0_CLR = CMU_CLKEN0_SYSCFG;
  #endif
}
#endif

#if defined(SL_CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT)
/**************************************************************************//**
 * @brief Configure the memory region below the secure main to XN
 *****************************************************************************/
static inline void configure_mpu(void)
{
  sl_status_t status = sl_mpu_disable_execute(FLASH_BASE,
                                              (uint32_t)&linker_vectors_begin,
                                              (uint32_t)&linker_vectors_begin - FLASH_BASE - 1u);
  if (status != SL_STATUS_OK) {
    INFINITE_LOOP();
  }
}
#endif // SL_CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT

/**************************************************************************//**
 * @brief Configure the memory ranges' security attributes using the SAU.
 *****************************************************************************/
static inline void configure_sau(void)
{
  #define CONFIGURED_SAU_REGIONS 7

  // Disable all the regions
  for (uint32_t i = CONFIGURED_SAU_REGIONS;
       i < ((SAU->TYPE & SAU_TYPE_SREGION_Msk) >> SAU_TYPE_SREGION_Pos);
       i++) {
    SAU->RNR = i;
    SAU->RLAR = 0u;
  }

  // Configure SAU for flash
  SAU->RNR = 0; // Flash
  SAU->RBAR = ((uint32_t)&linker_sg_begin) & SAU_RBAR_BADDR_Msk;
  SAU->RLAR = ((FLASH_BASE + FLASH_SIZE - 1u) & SAU_RLAR_LADDR_Msk)
              | (0 << SAU_RLAR_NSC_Pos)
              | SAU_RLAR_ENABLE_Msk;
  SAU->RNR = 1; // NS Peripherals
  SAU->RBAR = PERIPHERALS_BASE_NS_START & SAU_RBAR_BADDR_Msk;
  SAU->RLAR = ((PERIPHERALS_BASE_NS_END) &SAU_RLAR_LADDR_Msk)
              | (0 << SAU_RLAR_NSC_Pos)
              | SAU_RLAR_ENABLE_Msk;
  SAU->RNR = 2; // SRAM
  SAU->RBAR = (SRAM_BASE + TZ_NS_RAM_OFFSET) & SAU_RBAR_BADDR_Msk;
  SAU->RLAR = ((SRAM_BASE + SRAM_SIZE - 1u) & SAU_RLAR_LADDR_Msk)
              | (0 << SAU_RLAR_NSC_Pos)
              | SAU_RLAR_ENABLE_Msk;
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
  SAU->RNR = 6; // EPPB
  SAU->RBAR = EPPB_REGION_START & SAU_RBAR_BADDR_Msk;
  SAU->RLAR = (EPPB_REGION_END & SAU_RLAR_LADDR_Msk)
              | (0 << SAU_RLAR_NSC_Pos)
              | SAU_RLAR_ENABLE_Msk;
  TZ_SAU_Enable();

  // Ensure that the new SAU configuration is used by subsequent instructions.
  __DSB();
  __ISB();
}

/**************************************************************************//**
 * @brief Configures the TZ memory map using the SMU.
 *
 * Configures the memory map with the SMU using the defines TZ_NS_FLASH_OFFSET
 * and TZ_NS_RAM_OFFSET as start locations for the non-secure sections in flash
 * and RAM. All of the peripherals are mapped and available for the non-secure
 * application, except for the a few secure-only peripherals.
 *****************************************************************************/
static inline void configure_smu(void)
{
  // Enable clocking (where necessary).
  #if (_SILICON_LABS_32B_SERIES_2_CONFIG > 1)
  CMU_S->CLKEN1_SET = CMU_CLKEN1_SMU;
  #endif
  SMU->LOCK = SMU_LOCK_SMULOCKKEY_UNLOCK;

  // Configure flash into S/NSC/NS regions.

  // S - NSC boundary
  SMU->ESAUMRB01 = FLASH_BASE & _SMU_ESAUMRB01_MASK;
  // NSC - NS boundary
  SMU->ESAUMRB12 = (FLASH_BASE + TZ_NS_FLASH_OFFSET) & _SMU_ESAUMRB12_MASK;

  // Configure RAM into S/NS regions.

  // S - NSC boundary
  SMU->ESAUMRB45 = (SRAM_BASE + TZ_NS_RAM_OFFSET) & _SMU_ESAUMRB45_MASK;
  // NSC - NS boundary
  SMU->ESAUMRB56 = (SRAM_BASE + TZ_NS_RAM_OFFSET) & _SMU_ESAUMRB56_MASK;

  // Make the info page NS.
  SMU->ESAURTYPES0 = SMU_ESAURTYPES0_ESAUR3NS;

  // Make the EPPB area NS.
  SMU->ESAURTYPES1 = SMU_ESAURTYPES1_ESAUR11NS;

  smu_configure_peripherals();

  smu_configure_bus_masters();

  smu_enable_security_fault_interrupts();

  // Lock SMU config.
  SMU->LOCK = 0;

  // Disable clocking (where necessary).
  #if (_SILICON_LABS_32B_SERIES_2_CONFIG > 1)
  CMU_NS->CLKEN1_CLR = CMU_CLKEN1_SMU;
  #endif
}

/**************************************************************************//**
 * @brief Configures secure attributes of peripherals using the SMU.
 *
 * This function is device dependent. Assumes that the SMU is clocked.
 *****************************************************************************/
static inline void smu_configure_peripherals(void)
{
  #if defined(SEMAILBOX_PRESENT)
  // All peripherals are non-secure except SMU, SEMAILBOX, MSC and SYSCFG.
  SMU->PPUSATD0_CLR = _SMU_PPUSATD0_MASK
                      & ~(SMU_PPUSATD0_SYSCFG
                          | SMU_PPUSATD0_MSC);
  SMU->PPUSATD1_CLR = _SMU_PPUSATD1_MASK
                      & ~(SMU_PPUSATD1_SMU
                          | SMU_PPUSATD1_SEMAILBOX);
  #elif defined(CRYPTOACC_PRESENT)
  // All peripherals are non-secure except SMU, CRYPTOACC, MSC and SYSCFG.
  SMU->PPUSATD0_CLR = _SMU_PPUSATD0_MASK
                      & ~(SMU_PPUSATD0_SYSCFG
                          | SMU_PPUSATD0_MSC);
  SMU->PPUSATD1_CLR = _SMU_PPUSATD1_MASK
                      & ~(SMU_PPUSATD1_SMU
                          | SMU_PPUSATD1_CRYPTOACC);
  #endif
}

/**************************************************************************//**
 * @brief Configures secure state of bus masters using the SMU.
 *
 * This function is device dependent. Assumes that the SMU is clocked.
 *****************************************************************************/
static inline void smu_configure_bus_masters(void)
{
  #if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_32B_SERIES_2_CONFIG == 1)
  // Configure all bus-masters execpt for SEDMA as non-secure.
  SMU->BMPUSATD0_CLR = _SMU_BMPUSATD0_MASK
                       & ~(SMU_BMPUSATD0_SEDMA);
  #elif defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_32B_SERIES_2_CONFIG > 1)
  // Configure all bus-masters execpt for SEEXTDMA as non-secure.
  SMU->BMPUSATD0_CLR = _SMU_BMPUSATD0_MASK
                       & ~(SMU_BMPUSATD0_SEEXTDMA);
  #elif defined(CRYPTOACC_PRESENT)
  // Configure all bus-masters execpt for CRYPTOACC as non-secure.
  SMU->BMPUSATD0_CLR = _SMU_BMPUSATD0_MASK
                       & ~(SMU_BMPUSATD0_CRYPTOACC);
  #endif
}

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
  NVIC_ClearTargetState(SYSCFG_IRQn);
  NVIC_ClearTargetState(MSC_IRQn);
  #elif defined(CRYPTOACC_PRESENT)
  NVIC_ClearTargetState(CRYPTOACC_IRQn);
  NVIC_ClearTargetState(TRNG_IRQn);
  NVIC_ClearTargetState(PKE_IRQn);
  NVIC_ClearTargetState(SMU_SECURE_IRQn);
  NVIC_ClearTargetState(SYSCFG_IRQn);
  NVIC_ClearTargetState(MSC_IRQn);
  #endif
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
  uint32_t scb_AIRCR = SCB->AIRCR;
  uint32_t scb_vectkey = AIRCR_UNLOCK_VECTKEY;
  SCB->AIRCR = SCB_AIRCR_PRIS_Msk
               | (scb_vectkey << SCB_AIRCR_VECTKEY_Pos)
               | (scb_AIRCR & ~SCB_AIRCR_VECTKEY_Msk);
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
 * @brief Enable the flash page locks corresponding to the bootloader and
 *        secure library code.
 *****************************************************************************/
static inline void enable_page_locks(void)
{
  #if defined(_CMU_CLKEN1_MASK)
  CMU->CLKEN1_SET = CMU_CLKEN1_MSC;
  #endif

  size_t pages_to_lock = NUM_PAGES_TO_LOCK;

  // Iterate over pagelock registers and set as many ones needed.
  for (size_t i = 0; i < NUM_PAGELOCK_REGISTERS; ++i) {
    if (pages_to_lock < 32u) {
      // It is possible that pages_to_lock is equal to zero at this point
      // (for example when the total amount of pages to lock is zero mod 32).
      // This should not be a problem though, as pushing 0 into _SET is a no-op
      *pagelock_registers[i] = (1u << pages_to_lock) - 1;
      pages_to_lock = 0u;
      break;
    } else {
      *pagelock_registers[i] = 0xffffffffu;
      pages_to_lock -= 32u;
    }
  }

  #if defined(_CMU_CLKEN1_MASK)
  CMU->CLKEN1_CLR = CMU_CLKEN1_MSC;
  #endif
}

/**************************************************************************//**
 * @brief Start non-secure app.
 *
 * This function should not return.
 *****************************************************************************/
static inline void start_ns_app(void)
{
  // Set stack pointer.
  __TZ_set_MSP_NS(*((uint32_t *)(FLASH_BASE + TZ_NS_FLASH_OFFSET)));
  __TZ_set_PSP_NS(*((uint32_t *)(FLASH_BASE + TZ_NS_FLASH_OFFSET)));

  // Set vector table (may be done by SystemInit() on NS side, but do it here to
  // be sure).
  SCB_NS->VTOR = (FLASH_BASE + TZ_NS_FLASH_OFFSET);

  // Call non-secure reset handler.
  nsfunc fp = (nsfunc) * ((uint32_t *)((FLASH_BASE + TZ_NS_FLASH_OFFSET) + 4UL));
  fp();
}

#if defined(MBEDTLS_PSA_CRYPTO_STORAGE_C) && defined(SLI_PSA_ITS_ENCRYPTED)

#if defined(SEMAILBOX_PRESENT)
/**************************************************************************//**
 * @brief Verify that the HSE has a supported firmware version.
 *
 * The Storage Root Key must be present in order for the TrustZone-enabled
 * secure key storage to work. Support for this was added in SE FW v1.2.12,
 * which is the minimum version supported.
 *****************************************************************************/
static inline void verify_hse_version(void)
{
  sl_se_command_context_t cmd_ctx;
  uint32_t se_version = 0;
  sl_status_t status = sl_se_get_se_version(&cmd_ctx, &se_version);
  if (status != SL_STATUS_OK) {
    INFINITE_LOOP();
  }
  if (!SLI_SE_VERSION_SUPPORTS_SRK(se_version)) {
    // SE firmware version must be at least v1.2.12 for xG21/xG22 and at least
    // v2.2.0 for xG23+.
    INFINITE_LOOP();
  }
}
#endif

#if !defined(SEMAILBOX_PRESENT)
/**************************************************************************//**
 * @brief Setup Storage Root Key (SRK)
 *
 * Reads the TrustZone Root Key / Storage Root Key passed from the VSE and
 * configures ITS to use it for NVM3 encryption.
 *****************************************************************************/
static inline void setup_storage_root_key(void)
{
  #if defined(CRYPTOACC_PRESENT)

  #define VSE_REPLY_SIZE_WORDS                  5
  #define VSE_REPLY_STATUS_TRUSTZONE_ROOT_KEY   (1 << 22)
  #define TZ_SRK_SIZE_WORDS                     8

  typedef struct {
    uint32_t data[TZ_SRK_SIZE_WORDS];
    uint32_t checksum;
  } vse_srk_t;

  // Store current CLKEN0 state
  uint32_t clken0_state = CMU_S->CLKEN0;

  // Read the SRK next to the VSE mailbox
  CMU_S->CLKEN0_SET = CMU_CLKEN0_SYSCFG;
  uint32_t *vse_reply = (uint32_t*)SYSCFG_S->ROOTDATA1;

  // Restore prior CLKEN0 state
  CMU_S->CLKEN0 = clken0_state;

  // Verify that the SE version supports SRK
  uint32_t se_version = vse_reply[1];
  if (!SLI_SE_VERSION_SUPPORTS_SRK(se_version)) {
    // SE firmware version must be at least v1.2.12 for xG21/xG22 and at least
    // v2.2.0 for xG23+.
    INFINITE_LOOP();
  }

  // Verify that the TrustZone Root Key flag is present in the reply status
  uint32_t vse_reply_status = vse_reply[2];
  if ((vse_reply_status & VSE_REPLY_STATUS_TRUSTZONE_ROOT_KEY) == 0) {
    INFINITE_LOOP();
  }

  // Read the SRK next to the mailbox, i.e. after the VSE reply header, data length and
  // checksum (+1).
  uint32_t vse_data_length = vse_reply[4];
  vse_srk_t *srk = (vse_srk_t*)(vse_reply + VSE_REPLY_SIZE_WORDS + vse_data_length + 1);

  // Verify checksum
  uint32_t checksum = 0;
  for (uint32_t i = 0; i < sizeof(srk->data) / sizeof(uint32_t); ++i) {
    checksum ^= srk->data[i];
  }
  if (checksum != srk->checksum) {
    INFINITE_LOOP();
  }

  // Set the ITS root key. Note that this function is not dependent on PSA Crypto
  // being initialized.
  psa_status_t status = sli_psa_its_set_root_key((uint8_t*)srk->data,
                                                 sizeof(srk->data));
  if (status != PSA_SUCCESS) {
    INFINITE_LOOP();
  }

  // Clear the SRK from the mailbox location
  sli_psa_zeroize(srk, sizeof(vse_srk_t));
  #else // defined(CRYPTOACC_PRESENT)
    #error "Unknown device without SEMAILBOX (HSE) or CRYPTOACC (VSE)"
  #endif // defined(CRYPTOACC_PRESENT)
}
#endif // !SEMAILBOX_PRESENT

#endif // defined(MBEDTLS_PSA_CRYPTO_STORAGE_C) && defined(SLI_PSA_ITS_ENCRYPTED)

// -----------------------------------------------------------------------------
// Fault handlers

void SMU_SECURE_IRQHandler(void)
{
  fatal_error();
}

void HardFault_Handler(void)
{
  fatal_error();
}

void SecureFault_Handler(void)
{
  fatal_error();
}

void UsageFault_Handler(void)
{
  fatal_error();
}

void BusFault_Handler(void)
{
  fatal_error();
}

void mpu_fault_handler(void)
{
  fatal_error();
}
