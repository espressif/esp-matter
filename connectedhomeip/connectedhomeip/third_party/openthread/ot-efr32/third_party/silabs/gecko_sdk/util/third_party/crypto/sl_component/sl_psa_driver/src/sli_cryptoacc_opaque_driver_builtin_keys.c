/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Driver Builtin key functions.
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

#include "em_device.h"

#if defined(CRYPTOACC_PRESENT) && defined(SEPUF_PRESENT)

#include <psa/crypto.h>
#include "mbedtls/platform.h"
#include "sli_cryptoacc_opaque_types.h"

#include <string.h>

#if defined(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS)

psa_status_t sli_cryptoacc_opaque_get_builtin_key(psa_drv_slot_number_t slot_number,
                                                  psa_key_attributes_t *attributes,
                                                  uint8_t *key_buffer,
                                                  size_t key_buffer_size,
                                                  size_t *key_buffer_length)
{
  sli_cryptoacc_opaque_key_context_t header = { 0 };

  // Set key type and permissions according to key ID
  switch (slot_number) {
    case SLI_CRYPTOACC_BUILTIN_KEY_PUF_SLOT:
      psa_set_key_bits(attributes, 256);
      psa_set_key_type(attributes, PSA_KEY_TYPE_AES);
      psa_set_key_usage_flags(attributes, PSA_KEY_USAGE_SIGN_MESSAGE);
      psa_set_key_algorithm(attributes, PSA_ALG_CMAC);
      break;
    default:
      return PSA_ERROR_DOES_NOT_EXIST;
  }

  psa_set_key_lifetime(attributes,
                       PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(
                         PSA_KEY_PERSISTENCE_READ_ONLY,
                         PSA_KEY_LOCATION_SL_CRYPTOACC_OPAQUE) );

  // Check the key buffer size after populating the key attributes:
  // From mbedTLS, psa-driver-interface.md (snippet):
  //
  //  This entry point may return the following status values:
  //    (...)
  //    * PSA_ERROR_BUFFER_TOO_SMALL: key_buffer_size is insufficient.
  //      In this case, the driver must pass the key's attributes in
  //      *attributes. In particular, get_builtin_key(slot_number,
  //      &attributes, NULL, 0) is a way for the core to obtain the
  //      key's attributes.
  if (key_buffer_size < sizeof(sli_cryptoacc_opaque_key_context_t)) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  header.struct_version = SLI_CRYPTOACC_OPAQUE_KEY_CONTEXT_VERSION;
  header.builtin_key_id = (uint8_t)slot_number;

  memcpy(key_buffer, &header, sizeof(sli_cryptoacc_opaque_key_context_t));
  *key_buffer_length = sizeof(sli_cryptoacc_opaque_key_context_t);
  return PSA_SUCCESS;
}

#if !defined(PSA_CRYPTO_DRIVER_TEST)
psa_status_t mbedtls_psa_platform_get_builtin_key(
  mbedtls_svc_key_id_t key_id,
  psa_key_lifetime_t *lifetime,
  psa_drv_slot_number_t *slot_number)
{
  switch (MBEDTLS_SVC_KEY_ID_GET_KEY_ID(key_id)) {
    case SL_CRYPTOACC_BUILTIN_KEY_PUF_ID:
      // Slot number is just the same as the key ID
      *slot_number = SLI_CRYPTOACC_BUILTIN_KEY_PUF_SLOT;
      break;
    default:
      return PSA_ERROR_DOES_NOT_EXIST;
  }
  *lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(
    PSA_KEY_PERSISTENCE_READ_ONLY,
    PSA_KEY_LOCATION_SL_CRYPTOACC_OPAQUE);
  return PSA_SUCCESS;
}

#endif // !PSA_CRYPTO_DRIVER_TEST

#endif // MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS

#endif // defined(CRYPTOACC_PRESENT) && defined(SEPUF_PRESENT)
