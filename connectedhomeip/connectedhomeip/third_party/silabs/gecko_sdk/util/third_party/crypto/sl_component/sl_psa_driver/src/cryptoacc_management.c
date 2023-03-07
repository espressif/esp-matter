/***************************************************************************//**
 * @file
 * @brief Silicon Labs CRYPTOACC device management interface.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "cryptoacc_management.h"

#if defined(CRYPTOACC_PRESENT)

#include "sx_trng.h"
#include "ba431_config.h"
#include "sx_errors.h"
#include "cryptolib_def.h"
#include "sx_aes.h"
#include "sx_trng.h"
#include "sx_rng.h"
#include "ba414ep_config.h"
#include "sli_se_manager_internal.h"
#include "mbedtls/threading.h"

/// Perform the TRNG conditioning test on startup
#define DO_TRNG_COND_TEST  (1)

/**
 * @brief Check if the TRNG needs to be initialized
 * @returns true if initialization is needed
 */
static bool trng_needs_init(void)
{
  // If TRNG is not enabled, it most difinitely needs to be inited
  if ((ba431_read_controlreg() & BA431_CTRL_NDRNG_ENABLE) == 0u) {
    return true;
  }

  // If confitioning key is all zero, it needs to be reinitialized
  uint32_t cond_key[4] = { 0 };
  ba431_read_conditioning_key(cond_key);
  if ((cond_key[0] == 0) && (cond_key[1] == 0)
      && (cond_key[2] == 0) && (cond_key[3] == 0)) {
    return true;
  }

  // No conditions were met -> it is running
  return false;
}

/**
 * @brief Initialize the TRNG if it is not already enabled
 * @returns PSA_SUCCESS on successful init
 */
static psa_status_t initialize_trng(void)
{
  psa_status_t status = PSA_SUCCESS;
  if (trng_needs_init()) {
    status = sx_trng_init(DO_TRNG_COND_TEST);
    if (status != CRYPTOLIB_SUCCESS) {
      status = PSA_ERROR_HARDWARE_FAILURE;
    }
  }
  return status;
}

/* Get ownership of an available CRYPTOACC device */
psa_status_t cryptoacc_management_acquire(void)
{
#if defined(MBEDTLS_THREADING_C)
  if ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0U) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  // Take SE lock - wait/block if taken by another thread.
  sl_status_t ret = sli_se_lock_acquire();

  if (ret != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }
#endif

  CMU->CLKEN1_SET = CMU_CLKEN1_CRYPTOACC;
  CMU->CRYPTOACCCLKCTRL_SET = (CMU_CRYPTOACCCLKCTRL_PKEN
                               | CMU_CRYPTOACCCLKCTRL_AESEN);

  return PSA_SUCCESS;
}

/* Release ownership of an available CRYPTOACC device */
psa_status_t cryptoacc_management_release(void)
{
  CMU->CLKEN1_CLR = CMU_CLKEN1_CRYPTOACC;
  CMU->CRYPTOACCCLKCTRL_CLR = (CMU_CRYPTOACCCLKCTRL_PKEN
                               | CMU_CRYPTOACCCLKCTRL_AESEN);

#if defined(MBEDTLS_THREADING_C)
  if (sli_se_lock_release() != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }
#endif

  return PSA_SUCCESS;
}

psa_status_t cryptoacc_trng_initialize(void)
{
  psa_status_t status = cryptoacc_management_acquire();
  if (status != 0) {
    return status;
  }
  status = initialize_trng();
  if (cryptoacc_management_release() != PSA_SUCCESS) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }
  return status;
}

#if (_SILICON_LABS_32B_SERIES_2_CONFIG > 2)

// Set to true when CM has been initialized
static bool cm_inited = false;

psa_status_t cryptoacc_initialize_countermeasures(void)
{
  // Set up SCA countermeasures in hardware
  psa_status_t cm_status = PSA_SUCCESS;
  if (!cm_inited) {
    // Note on the error handling:
    // We want to try and set up the countermeasures even if some of the
    // steps fail. Hence, the first error code is stored and returned in
    // the end if something goes wrong.

    // Set up TRNG for PK engine
    struct sx_rng trng = {
      .param = NULL,
      .get_rand_blk = sx_trng_fill_blk,
    };
    ba414ep_set_rng(trng);

    // Set mask in AES engine
    psa_status_t hw_status = cryptoacc_management_acquire();
    if (hw_status != PSA_SUCCESS) {
      cm_status = hw_status;
    }
    hw_status = initialize_trng();
    if ((hw_status != PSA_SUCCESS) && (cm_status == PSA_SUCCESS)) {
      cm_status = hw_status;
    }
    uint32_t mask = sx_trng_get_word() | (1U << 31);
    sx_aes_load_mask(mask);

    hw_status = cryptoacc_management_release();
    if ((hw_status != PSA_SUCCESS) && (cm_status == PSA_SUCCESS)) {
      cm_status = hw_status;
    }

    // Only track that init was successful if no error codes popped up
    if (cm_status == PSA_SUCCESS) {
      cm_inited = true;
    }
  }
  return cm_status;
}
#endif // _SILICON_LABS_32B_SERIES_2_CONFIG > 2

#endif /* CRYPTOACC_PRESENT */
