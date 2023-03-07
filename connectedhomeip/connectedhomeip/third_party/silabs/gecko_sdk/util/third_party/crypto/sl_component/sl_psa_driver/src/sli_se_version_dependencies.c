/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Driver SE Version Dependencies.
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

#if defined(SEMAILBOX_PRESENT)

#include "psa/crypto.h"
#include "sli_se_version_dependencies.h"
#include "sli_se_driver_key_management.h"

#include "sl_se_manager_util.h"

// -----------------------------------------------------------------------------
// Global Functions

#if defined(SLI_SE_VERSION_ED25519_ERRATA_CHECK_REQUIRED)

// Check for an errata causing the SE to emit a faulty EdDSA public key for
// operations where only a private key is provided. Assumes that an already
// initalized SE command context is passed as input.
psa_status_t sli_se_check_eddsa_errata(const psa_key_attributes_t* attributes,
                                       sl_se_command_context_t* cmd_ctx)
{
  if (PSA_KEY_TYPE_ECC_GET_FAMILY(psa_get_key_type(attributes))
      == PSA_ECC_FAMILY_TWISTED_EDWARDS) {
    uint32_t se_version = 0;
    sl_status_t status = sl_se_get_se_version(cmd_ctx, &se_version);
    if (status != SL_STATUS_OK) {
      return PSA_ERROR_HARDWARE_FAILURE;
    }
    se_version = SLI_VERSION_REMOVE_DIE_ID(se_version);

    if (SLI_SE_VERSION_ED25519_BROKEN(se_version)) {
      return PSA_ERROR_NOT_SUPPORTED;
    }
  }

  return PSA_SUCCESS;
}

#endif // SLI_SE_VERSION_ED25519_ERRATA_CHECK_REQUIRED

#endif // defined(SEMAILBOX_PRESENT)
