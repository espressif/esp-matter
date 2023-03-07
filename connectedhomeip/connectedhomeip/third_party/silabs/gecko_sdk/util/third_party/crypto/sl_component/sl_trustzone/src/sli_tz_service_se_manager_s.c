/***************************************************************************//**
 * @file
 * @brief TrustZone secure SE Manager service (secure side).
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

#include <stddef.h>
#include <stdint.h>

#include "psa/crypto_types.h"
#include "psa/crypto_values.h"

#include "sli_tz_service_se_manager.h"
#include "sl_se_manager.h"
#include "sl_se_manager_util.h"

#include "em_device.h"

//------------------------------------------------------------------------------
// Macros

#define SUPPRESS_UNUSED_IOVEC_PARAM_WARNING() \
  (void)in_vec;                               \
  (void)in_len;                               \
  (void)out_vec;                              \
  (void)out_len;

//------------------------------------------------------------------------------
// Function definitions

psa_status_t sli_tz_se_check_se_image(psa_invec in_vec[],
                                      size_t in_len,
                                      psa_outvec out_vec[],
                                      size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;
  void *image_addr = (void *)in_vec[2].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_check_se_image(cmd_ctx, image_addr);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_apply_se_image(psa_invec in_vec[],
                                      size_t in_len,
                                      psa_outvec out_vec[],
                                      size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;
  void *image_addr = (void *)in_vec[2].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_apply_se_image(cmd_ctx, image_addr);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_get_upgrade_status_se_image(psa_invec in_vec[],
                                                   size_t in_len,
                                                   psa_outvec out_vec[],
                                                   size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;
  uint32_t *status = (uint32_t *)in_vec[2].base;
  uint32_t *prev_version = (uint32_t *)in_vec[3].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_get_upgrade_status_se_image(cmd_ctx, status, prev_version);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_check_host_image(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;
  void *image_addr = (void *)in_vec[2].base;
  uint32_t size = in_vec[2].len;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_check_host_image(cmd_ctx, image_addr, size);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_apply_host_image(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;
  void *image_addr = (void *)in_vec[2].base;
  uint32_t size = in_vec[2].len;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_apply_host_image(cmd_ctx, image_addr, size);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_get_upgrade_status_host_image(psa_invec in_vec[],
                                                     size_t in_len,
                                                     psa_outvec out_vec[],
                                                     size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;
  uint32_t *status = (uint32_t *)in_vec[2].base;
  uint32_t *prev_version = (uint32_t *)in_vec[3].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_get_upgrade_status_host_image(cmd_ctx,
                                                   status,
                                                   prev_version);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_init_otp_key(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;
  sl_se_device_key_type_t key_type =
    *((sl_se_device_key_type_t *)in_vec[2].base);
  void *key = (void *)in_vec[3].base;
  uint32_t num_bytes = in_vec[3].len;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_init_otp_key(cmd_ctx, key_type, key, num_bytes);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_read_pubkey(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;
  sl_se_device_key_type_t key_type =
    *((sl_se_device_key_type_t *)in_vec[2].base);

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;
  void *key = out_vec[1].base;
  uint32_t num_bytes = out_vec[1].len;

  *sl_status = sl_se_read_pubkey(cmd_ctx, key_type, key, num_bytes);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_init_otp(psa_invec in_vec[],
                                size_t in_len,
                                psa_outvec out_vec[],
                                size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;
  sl_se_otp_init_t *otp_init = (sl_se_otp_init_t *)in_vec[2].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_init_otp(cmd_ctx, otp_init);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_read_otp(psa_invec in_vec[],
                                size_t in_len,
                                psa_outvec out_vec[],
                                size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;
  sl_se_otp_init_t *otp_settings = (sl_se_otp_init_t *)out_vec[1].base;

  *sl_status = sl_se_read_otp(cmd_ctx, otp_settings);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_get_se_version(psa_invec in_vec[],
                                      size_t in_len,
                                      psa_outvec out_vec[],
                                      size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;
  uint32_t *version = (uint32_t *)out_vec[1].base;

  *sl_status = sl_se_get_se_version(cmd_ctx, version);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_get_debug_lock_status(psa_invec in_vec[],
                                             size_t in_len,
                                             psa_outvec out_vec[],
                                             size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;
  sl_se_debug_status_t *debug_status = (sl_se_debug_status_t *)out_vec[1].base;

  *sl_status = sl_se_get_debug_lock_status(cmd_ctx, debug_status);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_apply_debug_lock(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_apply_debug_lock(cmd_ctx);

  return PSA_SUCCESS;
}

#if defined(CRYPTOACC_PRESENT)

psa_status_t sli_tz_se_read_executed_command(psa_invec in_vec[],
                                             size_t in_len,
                                             psa_outvec out_vec[],
                                             size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_read_executed_command(cmd_ctx);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_ack_command(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_ack_command(cmd_ctx);

  return PSA_SUCCESS;
}

#endif // CRYPTOACC_PRESENT

#if defined(SEMAILBOX_PRESENT)

psa_status_t sli_tz_se_get_status(psa_invec in_vec[],
                                  size_t in_len,
                                  psa_outvec out_vec[],
                                  size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;
  sl_se_status_t *status = (sl_se_status_t *)out_vec[1].base;

  *sl_status = sl_se_get_status(cmd_ctx, status);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_get_serialnumber(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;
  void *serial = out_vec[1].base;

  *sl_status = sl_se_get_serialnumber(cmd_ctx, serial);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_get_otp_version(psa_invec in_vec[],
                                       size_t in_len,
                                       psa_outvec out_vec[],
                                       size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;
  uint32_t *version = (uint32_t *)out_vec[1].base;

  *sl_status = sl_se_get_otp_version(cmd_ctx, version);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_enable_secure_debug(psa_invec in_vec[],
                                           size_t in_len,
                                           psa_outvec out_vec[],
                                           size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_enable_secure_debug(cmd_ctx);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_disable_secure_debug(psa_invec in_vec[],
                                            size_t in_len,
                                            psa_outvec out_vec[],
                                            size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_disable_secure_debug(cmd_ctx);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_set_debug_options(psa_invec in_vec[],
                                         size_t in_len,
                                         psa_outvec out_vec[],
                                         size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;
  const sl_se_debug_options_t *debug_options =
    (sl_se_debug_options_t *)in_vec[2].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_set_debug_options(cmd_ctx, debug_options);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_erase_device(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_erase_device(cmd_ctx);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_disable_device_erase(psa_invec in_vec[],
                                            size_t in_len,
                                            psa_outvec out_vec[],
                                            size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_disable_device_erase(cmd_ctx);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_get_challenge(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;
  sl_se_challenge_t *challenge = out_vec[1].base;

  *sl_status = sl_se_get_challenge(cmd_ctx, *challenge);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_roll_challenge(psa_invec in_vec[],
                                      size_t in_len,
                                      psa_outvec out_vec[],
                                      size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_roll_challenge(cmd_ctx);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_open_debug(psa_invec in_vec[],
                                  size_t in_len,
                                  psa_outvec out_vec[],
                                  size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;
  void *cert = (void *)in_vec[2].base;
  uint32_t len = in_vec[2].len;
  sl_se_debug_options_t *debug_options =
    (sl_se_debug_options_t *)in_vec[3].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_open_debug(cmd_ctx, cert, len, debug_options);

  return PSA_SUCCESS;
}

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)

psa_status_t sli_tz_se_write_user_data(psa_invec in_vec[],
                                       size_t in_len,
                                       psa_outvec out_vec[],
                                       size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;
  uint32_t offset = *((uint32_t *)in_vec[2].base);
  void *data = (void *)in_vec[3].base;
  uint32_t num_bytes = in_vec[3].len;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_write_user_data(cmd_ctx, offset, data, num_bytes);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_erase_user_data(psa_invec in_vec[],
                                       size_t in_len,
                                       psa_outvec out_vec[],
                                       size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_erase_user_data(cmd_ctx);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_get_reset_cause(psa_invec in_vec[],
                                       size_t in_len,
                                       psa_outvec out_vec[],
                                       size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;
  uint32_t *reset_cause = (uint32_t *)out_vec[1].base;

  *sl_status = sl_se_get_reset_cause(cmd_ctx, reset_cause);

  return PSA_SUCCESS;
}

#endif // _SILICON_LABS_32B_SERIES_2_CONFIG_1

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)

psa_status_t sli_tz_se_disable_tamper(psa_invec in_vec[],
                                      size_t in_len,
                                      psa_outvec out_vec[],
                                      size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;
  void *cert = (void *)in_vec[2].base;
  uint32_t len = in_vec[2].len;
  sl_se_tamper_signals_t tamper_signals =
    *((sl_se_tamper_signals_t*)in_vec[3].base);

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;

  *sl_status = sl_se_disable_tamper(cmd_ctx, cert, len, tamper_signals);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_read_cert_size(psa_invec in_vec[],
                                      size_t in_len,
                                      psa_outvec out_vec[],
                                      size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;
  sl_se_cert_size_type_t *cert_size = (sl_se_cert_size_type_t *)out_vec[1].base;

  *sl_status = sl_se_read_cert_size(cmd_ctx, cert_size);

  return PSA_SUCCESS;
}

psa_status_t sli_tz_se_read_cert(psa_invec in_vec[],
                                 size_t in_len,
                                 psa_outvec out_vec[],
                                 size_t out_len)
{
  SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();

  // Input arguments
  sl_se_command_context_t *cmd_ctx = (sl_se_command_context_t *)in_vec[1].base;
  sl_se_cert_type_t cert_type = *((sl_se_cert_type_t *)in_vec[2].base);

  // Output arguments
  sl_status_t *sl_status = out_vec[0].base;
  void *cert = out_vec[1].base;
  uint32_t num_bytes = out_vec[1].len;

  *sl_status = sl_se_read_cert(cmd_ctx, cert_type, cert, num_bytes);

  return PSA_SUCCESS;
}

#endif // VAULT
#endif // SEMAILBOX_PRESENT
