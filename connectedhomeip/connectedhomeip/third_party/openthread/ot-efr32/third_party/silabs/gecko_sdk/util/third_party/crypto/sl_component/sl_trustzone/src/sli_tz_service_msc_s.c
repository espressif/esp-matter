/***************************************************************************//**
 * @file
 * @brief Silicon Labs TrustZone secure MSC service (secure side).
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

#include "sli_tz_service_msc.h"
#include "sli_tz_iovec_check.h"

#include "em_device.h"
#include "em_msc.h"
#include "em_core.h"

//------------------------------------------------------------------------------
// Macros

// Note that we explicitly use the NS alias of the LDMA, since this macro is
// used before the LDMA is configured as a secure peripheral.
#define SLI_LDMA_NOT_READY()                      \
  (((LDMA_NS->STATUS & LDMA_STATUS_ANYBUSY) != 0) \
   || ((LDMA_NS->STATUS & LDMA_STATUS_ANYREQ) != 0))

//------------------------------------------------------------------------------
// Function definitions

//-------------------------------------
// Simple services (no IOVEC usage)

uint32_t sli_tz_msc_get_locked(uint32_t unused)
{
  (void)unused;
  return MSC_LockGetLocked();
}

uint32_t sli_tz_msc_set_locked(uint32_t unused)
{
  (void)unused;
  MSC_LockSetLocked();
  return SLI_TZ_DISPATCH_UNUSED_RETVAL;
}

uint32_t sli_tz_msc_set_unlocked(uint32_t unused)
{
  (void)unused;
  MSC_LockSetUnlocked();
  return SLI_TZ_DISPATCH_UNUSED_RETVAL;
}

uint32_t sli_tz_msc_get_readctrl(uint32_t unused)
{
  (void)unused;
  return MSC_ReadCTRLGet();
}

uint32_t sli_tz_msc_set_readctrl(uint32_t value)
{
  MSC_ReadCTRLSet(value);
  return SLI_TZ_DISPATCH_UNUSED_RETVAL;
}

uint32_t sli_tz_msc_set_pagelock(uint32_t page_number)
{
  MSC_PageLockSetLocked(page_number);
  return SLI_TZ_DISPATCH_UNUSED_RETVAL;
}

uint32_t sli_tz_msc_get_pagelock(uint32_t page_number)
{
  return MSC_PageLockGetLocked(page_number);
}

uint32_t sli_tz_msc_get_userdata_size(uint32_t unused)
{
  (void)unused;
  return MSC_UserDataGetSize();
}

uint32_t sli_tz_msc_get_misclockword(uint32_t unused)
{
  (void)unused;
  return MSC_MiscLockWordGet();
}

uint32_t sli_tz_msc_set_misclockword(uint32_t value)
{
  MSC_MiscLockWordSet(value);
  return SLI_TZ_DISPATCH_UNUSED_RETVAL;
}

uint32_t sli_tz_msc_init(uint32_t unused)
{
  (void)unused;
  MSC_Init();
  return SLI_TZ_DISPATCH_UNUSED_RETVAL;
}

uint32_t sli_tz_msc_deinit(uint32_t unused)
{
  (void)unused;
  MSC_Deinit();
  return SLI_TZ_DISPATCH_UNUSED_RETVAL;
}

uint32_t sli_tz_msc_erase_page(uint32_t startAddress)
{
  return (uint32_t)MSC_ErasePage((uint32_t *)startAddress);
}

//-------------------------------------
// Full services (IOVEC usage)

int32_t sli_tz_msc_write_word(psa_invec in_vec[],
                              size_t in_len,
                              psa_outvec out_vec[],
                              size_t out_len)
{
  SLI_TZ_IOVEC_ASSERT_N_IOVECS(2, 1);

  const void *data = in_vec[1].base;
  uint32_t numBytes = in_vec[1].len;
  uint32_t *address = out_vec[0].base;

  return (uint32_t)MSC_WriteWord(address, data, numBytes);
}

int32_t sli_tz_msc_write_word_dma(psa_invec in_vec[],
                                  size_t in_len,
                                  psa_outvec out_vec[],
                                  size_t out_len)
{
  SLI_TZ_IOVEC_ASSERT_N_IOVECS(3, 1);

  int ch = *((int *)in_vec[1].base);
  const void *data = in_vec[2].base;
  uint32_t numBytes = in_vec[2].len;
  uint32_t *address = out_vec[0].base;

  // Enter a critical section in order to avoid being interrupted while the LDMA
  // is configured as a secure bus master.
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  // Wait for LDMA to finish any ongoing operation before configuring it as a
  // secure bus master.
  uint32_t timeOut = MSC_PROGRAM_TIMEOUT;
  while (SLI_LDMA_NOT_READY() && (timeOut != 0)) {
    timeOut--;
  }
  if (timeOut == 0) {
    CORE_EXIT_CRITICAL();
    return mscReturnTimeOut;
  }

  // Make sure that the LDMA is configured as a secure bus master before
  // staring the operation. If we did not do this, and the LDMA is configured as
  // non-secure, the LDMA would not be able to write to the MSC registers.
  #if (_SILICON_LABS_32B_SERIES_2_CONFIG > 1)
  CMU->CLKEN1_SET = CMU_CLKEN1_SMU;
  #endif
  SMU->LOCK = SMU_LOCK_SMULOCKKEY_UNLOCK;
  uint32_t prev_bus_master_state = SMU->BMPUSATD0 & SMU_BMPUSATD0_LDMA;
  SMU->BMPUSATD0_SET = SMU_BMPUSATD0_LDMA;

  // We also configure the LDMA and LDMAXBAR peripherals as secure during the
  // MSC operation. This should technically not be needed since we're in a
  // critical region, however, it is done as an additional security measure.
  uint32_t prev_peripherals_state = SMU->PPUSATD0
                                    & (SMU_PPUSATD0_LDMA
                                       | SMU_PPUSATD0_LDMAXBAR);
  SMU->PPUSATD0_SET = SMU_PPUSATD0_LDMA | SMU_PPUSATD0_LDMAXBAR;

  uint32_t status = (uint32_t)MSC_WriteWordDma(ch, address, data, numBytes);

  // Restore security attributes and wrap up SMU usage.
  SMU->BMPUSATD0_TGL = SMU_BMPUSATD0_LDMA ^ prev_bus_master_state;
  SMU->PPUSATD0_TGL = (SMU_PPUSATD0_LDMA
                       | SMU_PPUSATD0_LDMAXBAR)
                      ^ prev_peripherals_state;
  SMU->LOCK = 0;
  #if (_SILICON_LABS_32B_SERIES_2_CONFIG > 1)
  CMU->CLKEN1_CLR = CMU_CLKEN1_SMU;
  #endif

  CORE_EXIT_CRITICAL();

  return status;
}
