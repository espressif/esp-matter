/***************************************************************************//**
 * @file
 * @brief Silicon Labs TrustZone Secure Library Interface.
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

#if defined(SL_COMPONENT_CATALOG_PRESENT)
  #include "sl_component_catalog.h"
#endif

#include "sli_tz_iovec_check.h"
#include "sli_tz_s_interface.h"

#include "sl_assert.h"

#include "tfm_crypto_defs.h"
#include "psa/client.h"

#include "sli_tz_service_psa_crypto.h"
#include "sli_tz_service_nvm3.h"
#if defined(SL_CATALOG_TZ_SERVICE_MSC_PRESENT)
  #include "sli_tz_service_msc.h"
  #include "em_msc.h"
#endif // SL_CATALOG_TZ_SERVICE_MSC_PRESENT
#if defined(SL_CATALOG_TZ_SERVICE_PSA_ITS_PRESENT)
  #include "sli_tz_service_its.h"
#endif // SL_CATALOG_TZ_SERVICE_PSA_ITS_PRESENT
#if defined(SL_CATALOG_TZ_SERVICE_SE_MANAGER_PRESENT)
  #include "sli_tz_service_se_manager.h"
#endif // SL_CATALOG_TZ_SERVICE_SE_MANAGER_PRESENT

//------------------------------------------------------------------------------
// Local typedefs

// Function pointer conforming to the PSA Crypto secure function signature.
typedef int32_t (*iovec_fn) (psa_invec *in_vec,
                             size_t in_len,
                             psa_outvec *out_vec,
                             size_t out_len);

//------------------------------------------------------------------------------
// Secure service function tables

static const iovec_fn crypto_function_table[] = {
  tfm_crypto_get_key_attributes,
  tfm_crypto_reset_key_attributes,
  tfm_crypto_open_key,
  tfm_crypto_close_key,
  tfm_crypto_import_key,
  tfm_crypto_destroy_key,
  tfm_crypto_export_key,
  tfm_crypto_export_public_key,
  tfm_crypto_purge_key,
  tfm_crypto_copy_key,
  tfm_crypto_hash_compute,
  tfm_crypto_hash_compare,
  tfm_crypto_hash_setup,
  tfm_crypto_hash_update,
  tfm_crypto_hash_finish,
  tfm_crypto_hash_verify,
  tfm_crypto_hash_abort,
  tfm_crypto_hash_clone,
  tfm_crypto_mac_compute,
  tfm_crypto_mac_verify,
  tfm_crypto_mac_sign_setup,
  tfm_crypto_mac_verify_setup,
  tfm_crypto_mac_update,
  tfm_crypto_mac_sign_finish,
  tfm_crypto_mac_verify_finish,
  tfm_crypto_mac_abort,
  tfm_crypto_cipher_encrypt,
  tfm_crypto_cipher_decrypt,
  tfm_crypto_cipher_encrypt_setup,
  tfm_crypto_cipher_decrypt_setup,
  tfm_crypto_cipher_generate_iv,
  tfm_crypto_cipher_set_iv,
  tfm_crypto_cipher_update,
  tfm_crypto_cipher_finish,
  tfm_crypto_cipher_abort,
  tfm_crypto_aead_encrypt,
  tfm_crypto_aead_decrypt,
  tfm_crypto_aead_encrypt_setup,
  tfm_crypto_aead_decrypt_setup,
  tfm_crypto_aead_generate_nonce,
  tfm_crypto_aead_set_nonce,
  tfm_crypto_aead_set_lengths,
  tfm_crypto_aead_update_ad,
  tfm_crypto_aead_update,
  tfm_crypto_aead_finish,
  tfm_crypto_aead_verify,
  tfm_crypto_aead_abort,
  tfm_crypto_sign_message,
  tfm_crypto_verify_message,
  tfm_crypto_sign_hash,
  tfm_crypto_verify_hash,
  tfm_crypto_asymmetric_encrypt,
  tfm_crypto_asymmetric_decrypt,
  tfm_crypto_key_derivation_setup,
  tfm_crypto_key_derivation_get_capacity,
  tfm_crypto_key_derivation_set_capacity,
  tfm_crypto_key_derivation_input_bytes,
  tfm_crypto_key_derivation_input_key,
  tfm_crypto_key_derivation_key_agreement,
  tfm_crypto_key_derivation_output_bytes,
  tfm_crypto_key_derivation_output_key,
  tfm_crypto_key_derivation_abort,
  tfm_sl_psa_key_derivation_single_shot,
  tfm_crypto_raw_key_agreement,
  tfm_crypto_generate_random,
  tfm_crypto_generate_key,
};

#if defined(SL_CATALOG_TZ_SERVICE_MSC_PRESENT)
static const iovec_fn msc_function_table[] = {
  (iovec_fn)sli_tz_msc_write_word,
  (iovec_fn)sli_tz_msc_write_word_dma,
};
#endif // SL_CATALOG_TZ_SERVICE_MSC_PRESENT

static const iovec_fn nvm3_function_table[] = {
  (iovec_fn)tfm_nvm3_init_default,
  (iovec_fn)tfm_nvm3_deinit_default,
#if defined(SL_CATALOG_TZ_SERVICE_NVM3_PRESENT)
  (iovec_fn)tfm_nvm3_read_partial_data,
  (iovec_fn)tfm_nvm3_read_data,
  (iovec_fn)tfm_nvm3_write_data,
  (iovec_fn)tfm_nvm3_delete_object,
  (iovec_fn)tfm_nvm3_open,
  (iovec_fn)tfm_nvm3_close,
  (iovec_fn)tfm_nvm3_get_object_info,
  (iovec_fn)tfm_nvm3_write_counter,
  (iovec_fn)tfm_nvm3_read_counter,
  (iovec_fn)tfm_nvm3_increment_counter,
  (iovec_fn)tfm_nvm3_erase_all,
  (iovec_fn)tfm_nvm3_get_erase_count,
  (iovec_fn)tfm_nvm3_set_erase_count,
  (iovec_fn)tfm_nvm3_repack,
  (iovec_fn)tfm_nvm3_repack_needed,
  (iovec_fn)tfm_nvm3_resize,
  (iovec_fn)tfm_nvm3_enum_objects,
  (iovec_fn)tfm_nvm3_enum_deleted_objects,
#endif // SL_CATALOG_TZ_SERVICE_NVM3_PRESENT
};

#if defined(SL_CATALOG_TZ_SERVICE_PSA_ITS_PRESENT)
static const iovec_fn its_function_table[] = {
  (iovec_fn)tfm_its_set_req,
  (iovec_fn)tfm_its_get_req,
  (iovec_fn)tfm_its_get_info_req,
  (iovec_fn)tfm_its_remove_req,
  (iovec_fn)sli_psa_its_encrypted_req,
};
#endif // SL_CATALOG_TZ_SERVICE_PSA_ITS_PRESENT

#if defined(SL_CATALOG_TZ_SERVICE_SE_MANAGER_PRESENT)
static const iovec_fn se_manager_function_table[] = {
  (iovec_fn)sli_tz_se_check_se_image,
  (iovec_fn)sli_tz_se_apply_se_image,
  (iovec_fn)sli_tz_se_get_upgrade_status_se_image,
  (iovec_fn)sli_tz_se_check_host_image,
  (iovec_fn)sli_tz_se_apply_host_image,
  (iovec_fn)sli_tz_se_get_upgrade_status_host_image,
  (iovec_fn)sli_tz_se_init_otp_key,
  (iovec_fn)sli_tz_se_read_pubkey,
  (iovec_fn)sli_tz_se_init_otp,
  (iovec_fn)sli_tz_se_read_otp,
  (iovec_fn)sli_tz_se_get_se_version,
  (iovec_fn)sli_tz_se_get_debug_lock_status,
  (iovec_fn)sli_tz_se_apply_debug_lock,

#if defined(CRYPTOACC_PRESENT)
  (iovec_fn)sli_tz_se_read_executed_command,
  (iovec_fn)sli_tz_se_ack_command,
#endif // CRYPTOACC_PRESENT

#if defined(SEMAILBOX_PRESENT)
  (iovec_fn)sli_tz_se_get_status,
  (iovec_fn)sli_tz_se_get_serialnumber,
  (iovec_fn)sli_tz_se_get_otp_version,
  (iovec_fn)sli_tz_se_enable_secure_debug,
  (iovec_fn)sli_tz_se_disable_secure_debug,
  (iovec_fn)sli_tz_se_set_debug_options,
  (iovec_fn)sli_tz_se_erase_device,
  (iovec_fn)sli_tz_se_disable_device_erase,
  (iovec_fn)sli_tz_se_get_challenge,
  (iovec_fn)sli_tz_se_roll_challenge,
  (iovec_fn)sli_tz_se_open_debug,
  (iovec_fn)sli_tz_se_write_user_data,
  (iovec_fn)sli_tz_se_erase_user_data,
  (iovec_fn)sli_tz_se_get_reset_cause,
  (iovec_fn)sli_tz_se_disable_tamper,
  (iovec_fn)sli_tz_se_read_cert_size,
  (iovec_fn)sli_tz_se_read_cert,
#endif // SEMAILBOX_PRESENT
};
#endif // SL_CATALOG_TZ_SERVICE_SE_MANAGER_PRESENT

//------------------------------------------------------------------------------
// Global secure dispatch functions

int32_t sli_tz_s_interface_dispatch_crypto(psa_invec in_vec[],
                                           size_t in_len,
                                           psa_outvec out_vec[],
                                           size_t out_len)
{
  EFM_ASSERT(sizeof(crypto_function_table) / sizeof(crypto_function_table[0])
             == TFM_CRYPTO_SID_MAX);
  sli_tz_iovec_params_t iovec_copy = { 0 };

  uint32_t status = sli_tz_iovecs_live_in_ns(in_vec,
                                             in_len,
                                             out_vec,
                                             out_len,
                                             &iovec_copy);
  if (status != SLI_TZ_IOVEC_OK) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  const struct tfm_crypto_pack_iovec *iov = iovec_copy.in_vec[0].base;
  uint32_t function_id = iov->sfn_id;
  if (function_id >= TFM_CRYPTO_SID_MAX) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
  iovec_fn fn = crypto_function_table[function_id];

  status = fn(iovec_copy.in_vec, in_len, iovec_copy.out_vec, out_len);

  // For certain functions, the .len field of one of the outvecs is used in
  // order to communicate one of the output values. We therefore have to copy
  // back any potential updates from the outvec copy to the origin living in NS.
  for (size_t i = 0; i < out_len; ++i) {
    out_vec[i].len = iovec_copy.out_vec[i].len;
  }

  return status;
}

#if defined(SL_CATALOG_TZ_SERVICE_MSC_PRESENT)
int32_t sli_tz_s_interface_dispatch_msc(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len)
{
  EFM_ASSERT(sizeof(msc_function_table) / sizeof(msc_function_table[0])
             == SLI_TZ_SERVICE_MSC_MAX_SID);
  sli_tz_iovec_params_t iovec_copy = { 0 };

  uint32_t status = sli_tz_iovecs_live_in_ns(in_vec,
                                             in_len,
                                             out_vec,
                                             out_len,
                                             &iovec_copy);
  if (status != SLI_TZ_IOVEC_OK) {
    return mscReturnInvalidAddr;
  }

  sli_tz_fn_id function_id = *((sli_tz_fn_id *)iovec_copy.in_vec[0].base);
  if (function_id >= SLI_TZ_SERVICE_MSC_MAX_SID) {
    return mscReturnInvalidAddr;
  }
  iovec_fn fn = msc_function_table[function_id];

  return fn(iovec_copy.in_vec, in_len, iovec_copy.out_vec, out_len);
}
#endif // SL_CATALOG_TZ_SERVICE_MSC_PRESENT

int32_t sli_tz_s_interface_dispatch_nvm3(psa_invec in_vec[],
                                         size_t in_len,
                                         psa_outvec out_vec[],
                                         size_t out_len)
{
  EFM_ASSERT(sizeof(nvm3_function_table) / sizeof(nvm3_function_table[0])
             == SLI_TZ_SERVICE_NVM3_MAX_SID);
  sli_tz_iovec_params_t iovec_copy = { 0 };

  uint32_t status = sli_tz_iovecs_live_in_ns(in_vec,
                                             in_len,
                                             out_vec,
                                             out_len,
                                             &iovec_copy);
  if (status != SLI_TZ_IOVEC_OK) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  sli_tz_fn_id function_id = *((sli_tz_fn_id *)iovec_copy.in_vec[0].base);
  if (function_id >= SLI_TZ_SERVICE_NVM3_MAX_SID) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
  iovec_fn fn = nvm3_function_table[function_id];

  return fn(iovec_copy.in_vec, in_len, iovec_copy.out_vec, out_len);
}

#if defined(SL_CATALOG_TZ_SERVICE_PSA_ITS_PRESENT)
int32_t sli_tz_s_interface_dispatch_its(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len)
{
  EFM_ASSERT(sizeof(its_function_table) / sizeof(its_function_table[0])
             == SLI_TZ_SERVICE_ITS_MAX_SID);
  sli_tz_iovec_params_t iovec_copy = { 0 };

  uint32_t status = sli_tz_iovecs_live_in_ns(in_vec,
                                             in_len,
                                             out_vec,
                                             out_len,
                                             &iovec_copy);
  if (status != SLI_TZ_IOVEC_OK) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  sli_tz_fn_id function_id = *((sli_tz_fn_id *)iovec_copy.in_vec[0].base);
  if (function_id >= SLI_TZ_SERVICE_ITS_MAX_SID) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
  iovec_fn fn = its_function_table[function_id];

  return fn(iovec_copy.in_vec, in_len, iovec_copy.out_vec, out_len);
}
#endif // SL_CATALOG_TZ_SERVICE_PSA_ITS_PRESENT

#if defined(SL_CATALOG_TZ_SERVICE_SE_MANAGER_PRESENT)
int32_t sli_tz_s_interface_dispatch_se_manager(psa_invec in_vec[],
                                               size_t in_len,
                                               psa_outvec out_vec[],
                                               size_t out_len)
{
  EFM_ASSERT(sizeof(se_manager_function_table) / sizeof(se_manager_function_table[0])
             == SLI_TZ_SERVICE_SE_MANAGER_MAX_SID);
  sli_tz_iovec_params_t iovec_copy = { 0 };

  uint32_t status = sli_tz_iovecs_live_in_ns(in_vec,
                                             in_len,
                                             out_vec,
                                             out_len,
                                             &iovec_copy);
  if (status != SLI_TZ_IOVEC_OK) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  sli_tz_fn_id function_id = *((sli_tz_fn_id *)iovec_copy.in_vec[0].base);
  if (function_id >= SLI_TZ_SERVICE_SE_MANAGER_MAX_SID) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
  iovec_fn fn = se_manager_function_table[function_id];

  return fn(iovec_copy.in_vec, in_len, iovec_copy.out_vec, out_len);
}
#endif // SL_CATALOG_TZ_SERVICE_SE_MANAGER_PRESENT
