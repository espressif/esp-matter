/***************************************************************************//**
 * @file
 * @brief PSA Driver initialization interface.
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

// -------------------------------------
// Includes

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)  \
  || defined(CRYPTOACC_PRESENT) \
  || defined(CRYPTO_PRESENT)

#include "psa/crypto.h"

#if defined(SEMAILBOX_PRESENT)
  #include "sli_se_transparent_functions.h"
  #include "sl_se_manager.h"
  #include "sli_se_opaque_functions.h"
#endif // SEMAILBOX_PRESENT

#if defined(CRYPTOACC_PRESENT)
  #include "sli_cryptoacc_transparent_functions.h"
  #include "cryptoacc_management.h"
#endif // CRYPTOACC_PRESENT

#if defined(CRYPTO_PRESENT)
  #include "sli_crypto_transparent_functions.h"
#endif // CRYPTO_PRESENT

// -------------------------------------
// Global function definitions

#if defined(SEMAILBOX_PRESENT)
psa_status_t sli_se_transparent_driver_init(void)
{
  sl_status_t sl_status = sl_se_init();
  if (sl_status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  return PSA_SUCCESS;
}

psa_status_t sli_se_transparent_driver_deinit(void)
{
  sl_status_t sl_status = sl_se_deinit();
  if (sl_status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  return PSA_SUCCESS;
}

#if defined(SEMAILBOX_PRESENT)                                                  \
  && ( (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) \
  || defined(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS) )
psa_status_t sli_se_opaque_driver_init(void)
{
  sl_status_t sl_status = sl_se_init();
  if (sl_status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  return PSA_SUCCESS;
}

psa_status_t sli_se_opaque_driver_deinit(void)
{
  sl_status_t sl_status = sl_se_deinit();
  if (sl_status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  return PSA_SUCCESS;
}
#endif // _SILICON_LABS_SECURITY_FEATURE_VAULT || BUILTIN_KEYS

#endif // SEMAILBOX_PRESENT

#if defined(CRYPTOACC_PRESENT)
psa_status_t sli_cryptoacc_transparent_driver_init(void)
{
  // Consider moving the clock init and etc. here, which is performed by the
  // management functions.

#if _SILICON_LABS_32B_SERIES_2_CONFIG > 2
  return cryptoacc_initialize_countermeasures();
#else // SILICON_LABS_32B_SERIES_2_CONFIG > 2
  return PSA_SUCCESS;
#endif // SILICON_LABS_32B_SERIES_2_CONFIG > 2
}

psa_status_t sli_cryptoacc_transparent_driver_deinit(void)
{
  return PSA_SUCCESS;
}
#endif // CRYPTOACC_PRESENT

#if defined(CRYPTO_PRESENT)
psa_status_t sli_crypto_transparent_driver_init(void)
{
  // Leave this function empty for now. Consider moving the clock init and etc. here,
  // which is performed by the management functions.
  return PSA_SUCCESS;
}

psa_status_t sli_crypto_transparent_driver_deinit(void)
{
  return PSA_SUCCESS;
}
#endif // CRYPTO_PRESENT

#endif // SEMAILBOX_PRESENT || CRYPTOACC_PRESENT || CRYPTO_PRESENT
