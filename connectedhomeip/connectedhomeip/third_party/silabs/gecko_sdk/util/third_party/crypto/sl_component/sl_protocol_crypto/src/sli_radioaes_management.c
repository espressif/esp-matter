/***************************************************************************//**
 * @file
 * @brief Threadsafe utilities for RADIOAES peripheral.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_device.h"

#if defined(RADIOAES_PRESENT)
/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

#include "sli_radioaes_management.h"
#include "sli_se_manager_osal.h"
#include "em_core.h"

#if defined(SL_SE_MANAGER_THREADING)
static se_manager_osal_mutex_t      radioaes_lock = { 0 };
static volatile bool                radioaes_lock_initialized = false;
#endif

#if defined(SLI_RADIOAES_REQUIRES_MASKING)

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_PSA_CRYPTO_PRESENT)
// If the PSA Crypto core is present, use its randomness abstraction to get
// the initial mask seed.
#include "psa/crypto.h"
#else
// If the PSA Crypto core is not present, we need to target the TRNG driver
// directly to get the initial mask seed. We'll always have an external randomness
// provider function on devices containing a RADIOAES instance.
#include "sli_psa_driver_common.h"
#endif
#include "sl_assert.h"

uint32_t sli_radioaes_mask = 0;

static void sli_radioaes_update_mask(void)
{
  if (sli_radioaes_mask == 0) {
    // Mask has not been initialized yet, get a random value to start
#if defined(SL_CATALOG_PSA_CRYPTO_PRESENT)
    psa_status_t status = psa_generate_random((uint8_t*)&sli_radioaes_mask, sizeof(sli_radioaes_mask));
    EFM_ASSERT(status == PSA_SUCCESS);
#else
    size_t out_len = 0;
    psa_status_t status = mbedtls_psa_external_get_random(NULL, (uint8_t*)&sli_radioaes_mask, sizeof(sli_radioaes_mask), &out_len);
    EFM_ASSERT(status == PSA_SUCCESS);
    EFM_ASSERT(out_len == sizeof(sli_radioaes_mask));
#endif
  }

  // Use a different mask for each new operation
  // The masking logic requires the upper mask bit to be set
  sli_radioaes_mask = (sli_radioaes_mask + 1) | (1UL << 31);
}
#endif // SLI_RADIOAES_REQUIRES_MASKING

sl_status_t sli_radioaes_acquire(void)
{
#if defined(_CMU_CLKEN0_MASK)
  CMU->CLKEN0 |= CMU_CLKEN0_RADIOAES;
#endif
  CMU->RADIOCLKCTRL |= CMU_RADIOCLKCTRL_EN;
  if ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0U) {
    // IRQ: need to store & restore RADIOAES registers
    while (RADIOAES->STATUS & (AES_STATUS_FETCHERBSY | AES_STATUS_PUSHERBSY | AES_STATUS_SOFTRSTBSY)) {
      // Wait for completion of the previous operation, since the RADIOAES
      // peripheral does not support preemption of an operation in progress.
    }
    #if defined(SLI_RADIOAES_REQUIRES_MASKING)
    // The mask should have been initialized from non-ISR context by calling
    // sl_mbedtls_init, before using the radioaes.
    EFM_ASSERT(sli_radioaes_mask != 0);
    #endif
    return SL_STATUS_ISR;
  } else {
#if defined(SL_SE_MANAGER_THREADING)
    sl_status_t ret = SL_STATUS_OK;

    // Non-IRQ, RTOS available: take mutex
    // Initialize mutex if that hasn't happened yet

    // Check flag first before going into a critical section, to avoid going into
    // a critical section on every single acquire() call. Since the _initialized
    // flag only transitions false -> true, we can in 99% of the calls avoid the
    // critical section.
    if (!radioaes_lock_initialized) {
      int32_t kernel_lock_state = 0;
      osKernelState_t kernel_state = se_manager_osal_kernel_get_state();
      if (kernel_state != osKernelInactive && kernel_state != osKernelReady) {
        kernel_lock_state = se_manager_osal_kernel_lock();
        if (kernel_lock_state < 0) {
          return SL_STATUS_SUSPENDED;
        }
      }

      // Check the flag again after entering the critical section. Now that we're
      // in the critical section, we can be sure that we are the only ones looking
      // at the flag and no-one is interrupting us during its manipulation.
      if (!radioaes_lock_initialized) {
        ret = se_manager_osal_init_mutex(&radioaes_lock);
        if (ret == SL_STATUS_OK) {
          radioaes_lock_initialized = true;
        }
      }

      if (kernel_state != osKernelInactive && kernel_state != osKernelReady) {
        if (se_manager_osal_kernel_restore_lock(kernel_lock_state) < 0) {
          return SL_STATUS_INVALID_STATE;
        }
      }
    }

    if (ret == SL_STATUS_OK) {
      ret = se_manager_osal_take_mutex(&radioaes_lock);
    }

    #if defined(SLI_RADIOAES_REQUIRES_MASKING)
    if (ret == SL_STATUS_OK) {
      sli_radioaes_update_mask();
    }
    #endif

    return ret;
#else
    // Non-IRQ, no RTOS: busywait
    while (RADIOAES->STATUS & (AES_STATUS_FETCHERBSY | AES_STATUS_PUSHERBSY | AES_STATUS_SOFTRSTBSY)) {
      // Wait for completion
    }
    #if defined(SLI_RADIOAES_REQUIRES_MASKING)
    sli_radioaes_update_mask();
    #endif
    return SL_STATUS_OK;
#endif
  }
}

sl_status_t sli_radioaes_release(void)
{
  // IRQ: nothing to do
  if ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0U) {
    return SL_STATUS_OK;
  }
#if defined(SL_SE_MANAGER_THREADING)
  // Non-IRQ, RTOS available: free mutex
  return se_manager_osal_give_mutex(&radioaes_lock);
#else
  // Non-IRQ, no RTOS: nothing to do.
  return SL_STATUS_OK;
#endif
}

sl_status_t sli_radioaes_save_state(sli_radioaes_state_t *ctx)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  ctx->FETCHADDR = RADIOAES->FETCHADDR;
  ctx->PUSHADDR = RADIOAES->PUSHADDR;

  CORE_EXIT_CRITICAL();
  return SL_STATUS_OK;
}

sl_status_t sli_radioaes_restore_state(sli_radioaes_state_t *ctx)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  RADIOAES->FETCHADDR = ctx->FETCHADDR;
  RADIOAES->PUSHADDR = ctx->PUSHADDR;

  CORE_EXIT_CRITICAL();
  return SL_STATUS_OK;
}

/// @endcond
#endif //defined(RADIOAES_PRESENT)
