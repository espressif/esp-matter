/***************************************************************************//**
 * @file
 * @brief Silicon Labs CRYPTOACC device management interface.
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

#ifndef CRYPTOACC_MANAGEMENT_H
#define CRYPTOACC_MANAGEMENT_H

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

/***************************************************************************//**
 * \addtogroup sl_crypto_plugins
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_cryptoacc_management CRYPTOACC device instance management
 * \brief Management functions for the CRYPTOACC. These functions take care
 *        of not having two 'owners' simultaneously for the same CRYPTOACC
 *        device, which could potentially be causing conflicts and system
 *        lock-up.
 * \{
 ******************************************************************************/

#if !defined(MBEDTLS_CONFIG_FILE)
    #include "mbedtls/config.h"
#else
    #include MBEDTLS_CONFIG_FILE
#endif

#include <stdlib.h>

#include "em_device.h"

#if defined(CRYPTOACC_PRESENT)

#include "psa/crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Get ownership of the crypto device
 *
 * \return PSA_SUCCESS if successful, PSA_ERROR_HARDWARE_FAILURE on error
 */
psa_status_t cryptoacc_management_acquire(void);

/**
 * \brief Release ownership of the crypto device
 *
 * \return PSA_SUCCESS if successful, PSA_ERROR_HARDWARE_FAILURE on error
 */
psa_status_t cryptoacc_management_release(void);

/**
 * \brief TRNG initialization
 *
 * \return PSA_SUCCESS if successful, PSA_ERROR_HARDWARE_FAILURE on error
 */
psa_status_t cryptoacc_trng_initialize(void);

#if (_SILICON_LABS_32B_SERIES_2_CONFIG > 2)
/**
 * \brief Set up hardware SCA countermeasures
 *
 * \return PSA_SUCCESS if successful, PSA_ERROR_HARDWARE_FAILURE on error
 *
 * \note Will try to set up CM even if errors are returned early on.
 *       In that case, the function will return the first error code that is
 *       encountered, but only after CM has been set up.
 */
psa_status_t cryptoacc_initialize_countermeasures(void);
#endif // _SILICON_LABS_32B_SERIES_2_CONFIG > 2

#ifdef __cplusplus
}
#endif

#endif /* CRYPTOACC_PRESENT */

/** \} (end addtogroup sl_cryptoacc_management) */
/** \} (end addtogroup sl_crypto_plugins) */

/// @endcond

#endif /* CRYPTOACC_MANAGEMENT_H */
