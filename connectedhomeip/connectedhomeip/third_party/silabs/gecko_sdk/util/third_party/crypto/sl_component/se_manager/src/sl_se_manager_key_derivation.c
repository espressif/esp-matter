/***************************************************************************//**
 * @file
 * @brief Silicon Labs Secure Engine Manager API.
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

#include "sl_se_manager.h"
#include "sli_se_manager_internal.h"
#include "em_se.h"
#include "em_system.h"
#include <string.h>

/***************************************************************************//**
 * \addtogroup sl_se Secure Engine Manager API
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Defines

#define KEYSPEC_DH_RESERVED                 0x00000780U

#if defined(SEMAILBOX_PRESENT)

// Constant from RFC 4492.
#define SL_SE_ECP_TLS_NAMED_CURVE   3   ///< ECCurveType's named_curve
#define SL_SE_TLSID_ECC_P256        23  ///< Named curve value for SECP256R1

#endif // defined(SEMAILBOX_PRESENT)

// -----------------------------------------------------------------------------
// Locals

#if defined(SEMAILBOX_PRESENT)

// ECJPAKE role
static const char * const ecjpake_id[] = {
  "client",
  "server"
};

#endif // defined(SEMAILBOX_PRESENT)

// -----------------------------------------------------------------------------
// Global Functions

// -------------------------------
// Elliptic-curve Diffie–Hellman

/***************************************************************************//**
 * ECDH shared secret computation.
 ******************************************************************************/
sl_status_t sl_se_ecdh_compute_shared_secret(sl_se_command_context_t *cmd_ctx,
                                             const sl_se_key_descriptor_t *key_in_priv,
                                             const sl_se_key_descriptor_t *key_in_pub,
                                             const sl_se_key_descriptor_t *key_out)
{
  sl_status_t status;
  uint32_t keyspec_out;
  uint32_t keyspec_in;
  uint32_t key_pubkey_size;
  SE_DataTransfer_t pubkey_input_buffer;
  SE_DataTransfer_t auth_buffer_out;

  if (cmd_ctx == NULL
      || key_in_priv == NULL || key_in_pub == NULL || key_out == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  status = sli_key_check_equivalent(key_in_priv, key_in_pub, false, false);
  if (status != SL_STATUS_OK) {
    return status;
  }
  status = sli_key_get_size(key_in_pub, &key_pubkey_size);
  if (status != SL_STATUS_OK) {
    return status;
  }

  #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  // The key out buffer size needs be large enough for non-volatile keys
  if (key_out->storage.method != SL_SE_KEY_STORAGE_INTERNAL_VOLATILE) {
    if (key_in_priv->type == SL_SE_KEY_TYPE_ECC_X25519
        || key_in_priv->type == SL_SE_KEY_TYPE_ECC_X448) {
      if (key_pubkey_size > key_out->storage.location.buffer.size) {
        return SL_STATUS_INVALID_PARAMETER;
      }
    } else {
      if ((key_pubkey_size * 2) > key_out->storage.location.buffer.size) {
        return SL_STATUS_INVALID_PARAMETER;
      }
    }
  }
  #else
  if (key_in_priv->type == SL_SE_KEY_TYPE_ECC_X25519) {
    if (key_pubkey_size > key_out->storage.location.buffer.size) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  } else {
    if ((key_pubkey_size * 2) > key_out->storage.location.buffer.size) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  #endif

  // Output of DH must be in RAW format
  if (key_out->type != SL_SE_KEY_TYPE_SYMMETRIC) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Initialize command
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_DH);

  // Add key parameters to command
  status = sli_se_key_to_keyspec(key_in_priv, &keyspec_in);
  if (status != SL_STATUS_OK) {
    return status;
  }
  keyspec_in = keyspec_in & ~KEYSPEC_DH_RESERVED;
  SE_addParameter(&cmd_ctx->command, keyspec_in);

  status = sli_se_key_to_keyspec(key_out, &keyspec_out);
  if (status != SL_STATUS_OK) {
    return status;
  }
  SE_addParameter(&cmd_ctx->command, keyspec_out);

  // Add key input metadata block to command
  sli_add_key_metadata(cmd_ctx, key_in_priv, status);
  // Add key input block to command
  sli_add_key_input(cmd_ctx, key_in_priv, status);

  if (key_out->storage.method != SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT) {
    // Add key output metadata block to command
    status = sli_se_get_auth_buffer(key_out, &auth_buffer_out);
    if (status != SL_STATUS_OK) {
      return status;
    }
    SE_addDataInput(&cmd_ctx->command, &auth_buffer_out);
  }
  // Add key of other party
  status = sli_se_get_key_input_output(key_in_pub, &pubkey_input_buffer);
  if (status != SL_STATUS_OK) {
    return status;
  }

  #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  if (key_in_pub->flags & SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN) {
    if (key_in_pub->flags & SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY) {
      // If using custom domain, the key buffer stores domain parameters,
      // however when computing the shared secret only the key is of interest.
      uint32_t domain_size = ((sl_se_custom_weierstrass_prime_domain_t*)key_in_pub->domain)->size;
      pubkey_input_buffer.data = &key_in_pub->storage.location.buffer.pointer[domain_size * 6];
      pubkey_input_buffer.length = (domain_size * 2) | SE_DATATRANSFER_REALIGN;
    } else {
      // Does not contain a public key.
      return SL_STATUS_INVALID_KEY;
    }
  }
  #endif

  SE_addDataInput(&cmd_ctx->command, &pubkey_input_buffer);

  // Add key output block to command
  sli_add_key_output(cmd_ctx, key_out, status);

  // Execute command.
  // The retries are necessary in order to reduce the risk of random failures
  // in the accelerated point multiplication. This mainly affects very small or
  // large scalars, which in this case would be the private key.
  for (size_t i = 0; i < SLI_SE_MAX_POINT_MULT_RETRIES; ++i) {
    status = sli_se_execute_and_wait(cmd_ctx);
    if (status != SL_STATUS_FAIL) {
      break;
    }
  }
  return status;
}

// -------------------------------
// ECJPAKE

/***************************************************************************//**
 * Parse a ECJPAKE TLS point.
 ******************************************************************************/
static sl_status_t ecjpake_parse_tls_point(const uint8_t **ibuf,
                                           size_t *ilen,
                                           size_t *rlen,
                                           uint8_t **obuf, size_t *olen)
{
  if (ilen == NULL || ibuf == NULL || obuf == NULL || olen == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (*ilen == 0 || *ibuf == NULL || *obuf == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Consume first byte, length of what follows.
  size_t field_length = **ibuf;
  *ibuf += 1;
  *ilen -= 1;
  if (rlen != NULL) {
    *rlen += 1;
  }

  if (field_length > *ilen) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Consume second byte, point type.
  uint8_t point_type = **ibuf;
  size_t point_length = field_length - 1;
  *ibuf += 1;
  *ilen -= 1;

  switch (point_type) {
    case 0x0:
      // Why would we ever get a zero-point?
      return SL_STATUS_INVALID_KEY;
    case 0x04:
      break;
    case 0x05:
      // We don't support compressed points...
      return SL_STATUS_NOT_SUPPORTED;
    default:
      return SL_STATUS_INVALID_PARAMETER;
  }

  // Copy out binary point.
  if (point_length > *olen) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  memcpy(*obuf, *ibuf, point_length);
  *ibuf += point_length;
  *ilen -= point_length;
  *obuf += point_length;
  *olen -= point_length;
  if (rlen != NULL) {
    *rlen += field_length;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Parse a ECJPAKE TLS zkp.
 ******************************************************************************/
static sl_status_t ecjpake_parse_tls_zkp(const uint8_t **ibuf,
                                         size_t *ilen,
                                         size_t *rlen,
                                         uint8_t **obuf,
                                         size_t *olen)
{
  if (ilen == NULL || ibuf == NULL || obuf == NULL || olen == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (*ilen == 0 || *ibuf == NULL || *obuf == NULL || *olen < 96) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  int ret = ecjpake_parse_tls_point(ibuf, ilen, rlen, obuf, olen);
  if (ret != SL_STATUS_OK) {
    return ret;
  }

  if (*ilen < 1) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Consume first byte, length of what follows.
  size_t field_length = **ibuf;
  *ibuf += 1;
  *ilen -= 1;
  if (rlen != NULL) {
    *rlen += 1;
  }

  if (field_length > *ilen || field_length > *olen) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (field_length == 0) {
    // Scalar cannot be zero.
    return SL_STATUS_INVALID_KEY;
  }

  // Right-adjust.
  size_t adjust_length = 32 - field_length;
  memset(*obuf, 0, adjust_length);
  *obuf += adjust_length;
  *olen -= adjust_length;

  // Consume field.
  memcpy(*obuf, *ibuf, field_length);
  *obuf += field_length;
  *olen -= field_length;
  *ibuf += field_length;
  *ilen -= field_length;

  if (rlen != NULL) {
    *rlen += field_length;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Write a ECJPAKE TLS point.
 ******************************************************************************/
static sl_status_t ecjpake_write_tls_point(uint8_t **obuf,
                                           size_t *olen,
                                           size_t *wlen,
                                           const uint8_t **ibuf,
                                           size_t *ilen,
                                           size_t point_length)
{
  if (ibuf == NULL || obuf == NULL || olen == NULL || ilen == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (*obuf == NULL || *ibuf == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // We can only output uncompressed points here.
  if (*olen < point_length + 2) {
    return SL_STATUS_WOULD_OVERFLOW;
  }

  if (*ilen < point_length) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  **obuf = point_length + 1;
  *obuf += 1;
  *olen -= 1;

  **obuf = 0x04;
  *obuf += 1;
  *olen -= 1;

  memcpy(*obuf, *ibuf, point_length);

  *obuf += point_length;
  *olen -= point_length;
  *ibuf += point_length;
  *ilen -= point_length;

  if (wlen != NULL) {
    *wlen += point_length + 2;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Write a ECJPAKE TLS zkp.
 ******************************************************************************/
static sl_status_t ecjpake_write_tls_zkp(uint8_t **obuf,
                                         size_t *olen,
                                         size_t *wlen,
                                         const uint8_t **ibuf,
                                         size_t *ilen,
                                         size_t point_length)
{
  sl_status_t ret = SL_STATUS_OK;

  if (ibuf == NULL || obuf == NULL || olen == NULL || ilen == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (*obuf == NULL || *ibuf == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  ret = ecjpake_write_tls_point(obuf, olen, wlen, ibuf, ilen, point_length);

  if (ret != SL_STATUS_OK) {
    return ret;
  }

  size_t zkp_length = 32;

  if (*olen < zkp_length + 1 || *ilen < zkp_length) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  **obuf = zkp_length;
  *obuf += 1;
  *olen -= 1;

  memcpy(*obuf, *ibuf, zkp_length);

  *obuf += zkp_length;
  *olen -= zkp_length;
  *ibuf += zkp_length;
  *ilen -= zkp_length;

  if (wlen != NULL) {
    *wlen += zkp_length + 1;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Check if an ECJPAKE context is ready for use.
 ******************************************************************************/
sl_status_t sl_se_ecjpake_check(const sl_se_ecjpake_context_t *ctx)
{
  if (ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (ctx->curve_flags == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Derive the shared secret.
 ******************************************************************************/
sl_status_t sl_se_ecjpake_derive_secret(sl_se_ecjpake_context_t *ctx,
                                        unsigned char *buf,
                                        size_t len,
                                        size_t *olen)
{
  sl_se_command_context_t *cmd_ctx;
  SE_Command_t *se_cmd;

  if (ctx == NULL || buf == NULL || olen == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (len < 32) {
    return SL_STATUS_WOULD_OVERFLOW;
  }

  cmd_ctx = ctx->cmd_ctx;
  se_cmd = &ctx->cmd_ctx->command;

  // Generated session key needs to come out unprotected.
  uint32_t gen_key_flags = 32;

  // SE command structures.
  sli_se_command_init(cmd_ctx,
                      SLI_SE_COMMAND_JPAKE_GEN_SESSIONKEY
                      | SLI_SE_COMMAND_OPTION_HASH_SHA256);
  SE_DataTransfer_t domain_in = SE_DATATRANSFER_DEFAULT(NULL, 0);
  SE_DataTransfer_t pwd_in = SE_DATATRANSFER_DEFAULT(ctx->pwd, ctx->pwd_len);
  SE_DataTransfer_t r_in = SE_DATATRANSFER_DEFAULT(ctx->r, 32);
  SE_DataTransfer_t Xp2_in = SE_DATATRANSFER_DEFAULT(ctx->Xp2, 64);
  SE_DataTransfer_t Xp_in = SE_DATATRANSFER_DEFAULT(ctx->Xp, 64);
  SE_DataTransfer_t key_out = SE_DATATRANSFER_DEFAULT(buf, 32);

  SE_addDataInput(se_cmd, &domain_in);
  SE_addDataInput(se_cmd, &pwd_in);
  SE_addDataInput(se_cmd, &r_in);
  SE_addDataInput(se_cmd, &Xp2_in);
  SE_addDataInput(se_cmd, &Xp_in);

  SE_addDataOutput(se_cmd, &key_out);

  SE_addParameter(se_cmd, ctx->curve_flags);
  SE_addParameter(se_cmd, ctx->pwd_len);
  SE_addParameter(se_cmd, gen_key_flags);

  sl_status_t ret = sli_se_execute_and_wait(cmd_ctx);

  if (ret == SL_STATUS_OK) {
    *olen = 32;
  } else {
    *olen = 0;
  }
  return ret;
}

/***************************************************************************//**
 * This clears an ECJPAKE context.
 ******************************************************************************/
sl_status_t sl_se_ecjpake_free(sl_se_ecjpake_context_t *ctx)
{
  if (ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  memset(ctx, 0, sizeof(*ctx));

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Initialize an ECJPAKE context.
 ******************************************************************************/
sl_status_t sl_se_ecjpake_init(sl_se_ecjpake_context_t *ctx,
                               sl_se_command_context_t *cmd_ctx)
{
  if (ctx == NULL || cmd_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  memset(ctx, 0, sizeof(*ctx));

  // store pointer to command context object
  ctx->cmd_ctx = cmd_ctx;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Read and process the first round message.
 ******************************************************************************/
sl_status_t sl_se_ecjpake_read_round_one(sl_se_ecjpake_context_t *ctx,
                                         const unsigned char *buf,
                                         size_t len)
{
  sl_status_t ret = SL_STATUS_OK;

  if (ctx == NULL || buf == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_se_command_context_t *cmd_ctx = ctx->cmd_ctx;
  SE_Command_t *se_cmd = &ctx->cmd_ctx->command;

  // We should receive 2 binary points and 2 ZKP's.

  // Local storage for ZKP's.
  uint8_t zkp1[32 + 64] = { 0 };
  uint8_t zkp2[32 + 64] = { 0 };

  uint8_t *obuf = ctx->Xp1;
  size_t olen = 64;

  // Parse structures.
  ret = ecjpake_parse_tls_point(&buf, &len, NULL, &obuf, &olen);
  if (ret != SL_STATUS_OK) {
    return ret;
  }

  obuf = zkp1;
  olen = 96;
  ret = ecjpake_parse_tls_zkp(&buf, &len, NULL, &obuf, &olen);
  if (ret != SL_STATUS_OK) {
    return ret;
  }

  obuf = ctx->Xp2;
  olen = 64;
  ret = ecjpake_parse_tls_point(&buf, &len, NULL, &obuf, &olen);
  if (ret != SL_STATUS_OK) {
    return ret;
  }

  obuf = zkp2;
  olen = 96;
  ret = ecjpake_parse_tls_zkp(&buf, &len, NULL, &obuf, &olen);
  if (ret != SL_STATUS_OK) {
    return ret;
  }

  if (len > 0) {
    // Too much input.
    return SL_STATUS_WOULD_OVERFLOW;
  }

  // SE command structures.
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_JPAKE_R1_VERIFY);
  SE_DataTransfer_t domain_in = SE_DATATRANSFER_DEFAULT(NULL, 0);
  SE_DataTransfer_t userid_mine = SE_DATATRANSFER_DEFAULT(
    (void*)ecjpake_id[ctx->role], strlen(ecjpake_id[ctx->role]));
  SE_DataTransfer_t userid_peer = SE_DATATRANSFER_DEFAULT(
    (void*)ecjpake_id[1 - ctx->role], strlen(ecjpake_id[1 - ctx->role]));
  SE_DataTransfer_t Xp1_in = SE_DATATRANSFER_DEFAULT(ctx->Xp1, 64);
  SE_DataTransfer_t zkp1_in = SE_DATATRANSFER_DEFAULT(zkp1, sizeof(zkp1));
  SE_DataTransfer_t Xp2_in = SE_DATATRANSFER_DEFAULT(ctx->Xp2, 64);
  SE_DataTransfer_t zkp2_in = SE_DATATRANSFER_DEFAULT(zkp2, sizeof(zkp2));

  SE_addDataInput(se_cmd, &domain_in);
  SE_addDataInput(se_cmd, &userid_mine);
  SE_addDataInput(se_cmd, &userid_peer);
  SE_addDataInput(se_cmd, &Xp1_in);
  SE_addDataInput(se_cmd, &zkp1_in);
  SE_addDataInput(se_cmd, &Xp2_in);
  SE_addDataInput(se_cmd, &zkp2_in);

  SE_addParameter(se_cmd, ctx->curve_flags);
  SE_addParameter(se_cmd, strlen(ecjpake_id[ctx->role]));
  SE_addParameter(se_cmd, strlen(ecjpake_id[1 - ctx->role]));

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Read and process the second round message.
 ******************************************************************************/
sl_status_t sl_se_ecjpake_read_round_two(sl_se_ecjpake_context_t *ctx,
                                         const unsigned char *buf,
                                         size_t len)
{
  if (ctx == NULL || buf == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_se_command_context_t *cmd_ctx = ctx->cmd_ctx;
  SE_Command_t *se_cmd = &ctx->cmd_ctx->command;
  sl_status_t ret = SL_STATUS_OK;

  // Local storage for ZKP.
  uint8_t zkpB[32 + 64];

  if (ctx->role == SL_SE_ECJPAKE_CLIENT) {
    uint16_t tls_id;

    if (len < 3) {
      return SL_STATUS_INVALID_PARAMETER;
    }

    // First byte is curve_type; only named_curve is handled.
    if (*(buf++) != SL_SE_ECP_TLS_NAMED_CURVE) {
      return SL_STATUS_INVALID_PARAMETER;
    }

    // Next two bytes are the namedcurve value.
    tls_id = *(buf++);
    tls_id <<= 8;
    tls_id |= *(buf++);

    // We support only one curve.
    if (tls_id != SL_SE_TLSID_ECC_P256) {
      return SL_STATUS_NOT_SUPPORTED;
    }

    len -= 3;
  }

  // We should receive 1 binary point and 1 ZKP.
  uint8_t *obuf = ctx->Xp;
  size_t olen = 64;

  // Parse structures.
  ret = ecjpake_parse_tls_point(&buf, &len, NULL, &obuf, &olen);
  if (ret != SL_STATUS_OK) {
    return ret;
  }

  obuf = zkpB;
  olen = sizeof(zkpB);
  ret = ecjpake_parse_tls_zkp(&buf, &len, NULL, &obuf, &olen);
  if (ret != SL_STATUS_OK) {
    return ret;
  }

  if (len > 0) {
    // Too much input.
    return SL_STATUS_WOULD_OVERFLOW;
  }

  // SE command structures.
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_JPAKE_R2_VERIFY);
  SE_DataTransfer_t domain_in = SE_DATATRANSFER_DEFAULT(NULL, 0);
  SE_DataTransfer_t userid_peer = SE_DATATRANSFER_DEFAULT(
    (void*)ecjpake_id[1 - ctx->role], strlen(ecjpake_id[1 - ctx->role]));
  SE_DataTransfer_t Xm1_in = SE_DATATRANSFER_DEFAULT(ctx->Xm1, 64);
  SE_DataTransfer_t Xm2_in = SE_DATATRANSFER_DEFAULT(ctx->Xm2, 64);
  SE_DataTransfer_t Xp1_in = SE_DATATRANSFER_DEFAULT(ctx->Xp1, 64);
  SE_DataTransfer_t Xp_in = SE_DATATRANSFER_DEFAULT(ctx->Xp, 64);
  SE_DataTransfer_t zkpB_in = SE_DATATRANSFER_DEFAULT(zkpB, sizeof(zkpB));

  SE_addDataInput(se_cmd, &domain_in);
  SE_addDataInput(se_cmd, &userid_peer);
  SE_addDataInput(se_cmd, &Xm1_in);
  SE_addDataInput(se_cmd, &Xm2_in);
  SE_addDataInput(se_cmd, &Xp1_in);
  SE_addDataInput(se_cmd, &Xp_in);
  SE_addDataInput(se_cmd, &zkpB_in);

  SE_addParameter(se_cmd, ctx->curve_flags);
  SE_addParameter(se_cmd, strlen(ecjpake_id[1 - ctx->role]));

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Set up an ECJPAKE context for use.
 ******************************************************************************/
sl_status_t sl_se_ecjpake_setup(sl_se_ecjpake_context_t *ctx,
                                sl_se_ecjpake_role_t role,
                                sl_se_hash_type_t hash,
                                uint32_t curve,
                                const unsigned char *secret,
                                size_t len)
{
  if (ctx == NULL || (len > 0 && secret == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // SE only supports passphrases of maximum 32 bytes.
  if (len > sizeof(ctx->pwd)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // SE currently only supports SHA256 as JPAKE hashing mechanism.
  if (hash != SL_SE_HASH_SHA256) {
    return SL_STATUS_NOT_SUPPORTED;
  }

  // SE currently only supports ECDSA secp256r1 as curve.
  if (curve != SL_SE_KEY_TYPE_ECC_P256) {
    return SL_STATUS_NOT_SUPPORTED;
  }

  ctx->curve_flags = 0x8000001FUL;
  ctx->role = role;
  ctx->pwd_len = len;
  memcpy(ctx->pwd, secret, len);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Generate and write the first round message.
 ******************************************************************************/
sl_status_t sl_se_ecjpake_write_round_one(sl_se_ecjpake_context_t *ctx,
                                          unsigned char *buf,
                                          size_t len,
                                          size_t *olen)
{
  sl_status_t ret = SL_STATUS_OK;

  if (ctx == NULL || buf == NULL || olen == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_se_command_context_t *cmd_ctx = ctx->cmd_ctx;
  SE_Command_t *se_cmd = &ctx->cmd_ctx->command;
  *olen = 0;

  // Local storage for ZKP's.
  uint8_t zkp1[32 + 64];
  uint8_t zkp2[32 + 64];

  // SE command structures.
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_JPAKE_R1_GENERATE);
  SE_DataTransfer_t domain_in = SE_DATATRANSFER_DEFAULT(NULL, 0);
  SE_DataTransfer_t userid = SE_DATATRANSFER_DEFAULT(
    (void*)ecjpake_id[ctx->role], strlen(ecjpake_id[ctx->role]));
  SE_DataTransfer_t r_out = SE_DATATRANSFER_DEFAULT(ctx->r, 32);
  SE_DataTransfer_t Xm1_out = SE_DATATRANSFER_DEFAULT(ctx->Xm1, 64);
  SE_DataTransfer_t zkp1_out = SE_DATATRANSFER_DEFAULT(zkp1, sizeof(zkp1));
  SE_DataTransfer_t Xm2_out = SE_DATATRANSFER_DEFAULT(ctx->Xm2, 64);
  SE_DataTransfer_t zkp2_out = SE_DATATRANSFER_DEFAULT(zkp2, sizeof(zkp2));

  SE_addDataInput(se_cmd, &domain_in);
  SE_addDataInput(se_cmd, &userid);

  SE_addDataOutput(se_cmd, &r_out);
  SE_addDataOutput(se_cmd, &Xm1_out);
  SE_addDataOutput(se_cmd, &zkp1_out);
  SE_addDataOutput(se_cmd, &Xm2_out);
  SE_addDataOutput(se_cmd, &zkp2_out);

  SE_addParameter(se_cmd, ctx->curve_flags);
  SE_addParameter(se_cmd, strlen(ecjpake_id[ctx->role]));

  ret = sli_se_execute_and_wait(cmd_ctx);

  if (ret == SL_STATUS_OK) {
    // To write TLS structures of ECJ-PAKE, we need to write:
    // Xm1, zkp1, Xm2 and zkp2.
    uint8_t *obuf = buf;
    const uint8_t *ibuf = ctx->Xm1;
    size_t ilen = 64;

    ret = ecjpake_write_tls_point(&obuf, &len, olen, &ibuf, &ilen, 64);
    if (ret != SL_STATUS_OK) {
      return ret;
    }

    ibuf = zkp1;
    ilen = 96;
    ret = ecjpake_write_tls_zkp(&obuf, &len, olen, &ibuf, &ilen, 64);
    if (ret != SL_STATUS_OK) {
      return ret;
    }

    ibuf = ctx->Xm2;
    ilen = 64;
    ret = ecjpake_write_tls_point(&obuf, &len, olen, &ibuf, &ilen, 64);
    if (ret != SL_STATUS_OK) {
      return ret;
    }

    ibuf = zkp2;
    ilen = 96;
    ret = ecjpake_write_tls_zkp(&obuf, &len, olen, &ibuf, &ilen, 64);
  }
  return ret;
}

/***************************************************************************//**
 * Generate and write the second round message.
 ******************************************************************************/
sl_status_t sl_se_ecjpake_write_round_two(sl_se_ecjpake_context_t *ctx,
                                          unsigned char *buf,
                                          size_t len,
                                          size_t *olen)
{
  sl_status_t ret = SL_STATUS_OK;

  if (ctx == NULL || buf == NULL || olen == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_se_command_context_t *cmd_ctx = ctx->cmd_ctx;
  SE_Command_t *se_cmd = &ctx->cmd_ctx->command;

  *olen = 0;

  uint8_t zkpA[32 + 64];
  uint8_t xA[64];

  // SE command structures.
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_JPAKE_R2_GENERATE);
  SE_DataTransfer_t domain_in = SE_DATATRANSFER_DEFAULT(NULL, 0);
  SE_DataTransfer_t pwd_in = SE_DATATRANSFER_DEFAULT(ctx->pwd, ctx->pwd_len);
  SE_DataTransfer_t userid = SE_DATATRANSFER_DEFAULT(
    (void*)ecjpake_id[ctx->role], strlen(ecjpake_id[ctx->role]));
  SE_DataTransfer_t r_in = SE_DATATRANSFER_DEFAULT(ctx->r, 32);
  SE_DataTransfer_t Xm1_in = SE_DATATRANSFER_DEFAULT(ctx->Xm1, 64);
  SE_DataTransfer_t Xp1_in = SE_DATATRANSFER_DEFAULT(ctx->Xp1, 64);
  SE_DataTransfer_t Xp2_in = SE_DATATRANSFER_DEFAULT(ctx->Xp2, 64);
  SE_DataTransfer_t xA_out = SE_DATATRANSFER_DEFAULT(xA, sizeof(xA));
  SE_DataTransfer_t zkpA_out = SE_DATATRANSFER_DEFAULT(zkpA, sizeof(zkpA));

  SE_addDataInput(se_cmd, &domain_in);
  SE_addDataInput(se_cmd, &pwd_in);
  SE_addDataInput(se_cmd, &userid);
  SE_addDataInput(se_cmd, &r_in);
  SE_addDataInput(se_cmd, &Xm1_in);
  SE_addDataInput(se_cmd, &Xp1_in);
  SE_addDataInput(se_cmd, &Xp2_in);

  SE_addDataOutput(se_cmd, &xA_out);
  SE_addDataOutput(se_cmd, &zkpA_out);

  SE_addParameter(se_cmd, ctx->curve_flags);
  SE_addParameter(se_cmd, ctx->pwd_len);
  SE_addParameter(se_cmd, strlen(ecjpake_id[ctx->role]));

  ret = sli_se_execute_and_wait(cmd_ctx);

  if (ret == SLI_SE_RESPONSE_OK) {
    // If we are the server, we need to write out the ECParams.
    if (ctx->role == SL_SE_ECJPAKE_SERVER) {
      if (len < 3 + 66 + 66 + 33) {
        return SL_STATUS_WOULD_OVERFLOW;
      }
      // First byte is curve_type, always named_curve.
      *(buf++) = SL_SE_ECP_TLS_NAMED_CURVE;

      // Next two bytes are the namedcurve value, we only support one.
      *(buf++) = SL_SE_TLSID_ECC_P256 >> 8;
      *(buf++) = SL_SE_TLSID_ECC_P256 & 0xFF;

      *olen += 3;
      len -= 3;
    }

    // To write TLS structures of ECJ-PAKE, we need to write:
    // xA in uncompressed form and zkpA in uncompressed form
    uint8_t *obuf = buf;
    const uint8_t *ibuf = xA;
    size_t ilen = 64;

    ret = ecjpake_write_tls_point(&obuf, &len, olen, &ibuf, &ilen, 64);
    if (ret != SL_STATUS_OK) {
      return ret;
    }

    ibuf = zkpA;
    ilen = 96;
    ret = ecjpake_write_tls_zkp(&obuf, &len, olen, &ibuf, &ilen, 64);
  }

  return ret;
}

// -------------------------------
// Key derivation functions

#if (defined(_SILICON_LABS_SECURITY_FEATURE) \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT))
/***************************************************************************//**
 * HKDF key derivation.
 ******************************************************************************/
sl_status_t sl_se_derive_key_hkdf(sl_se_command_context_t *cmd_ctx,
                                  const sl_se_key_descriptor_t *in_key,
                                  sl_se_hash_type_t hash,
                                  const unsigned char *salt,
                                  size_t salt_len,
                                  const unsigned char *info,
                                  size_t info_len,
                                  sl_se_key_descriptor_t *out_key)
{
  uint32_t hash_mask;
  sl_status_t status = SL_STATUS_OK;

  if ((cmd_ctx == NULL) || (in_key == NULL) || (out_key == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((salt == NULL) && (salt_len != 0U)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((info == NULL) && (info_len != 0U)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  switch (hash) {
    case SL_SE_HASH_SHA1:
      hash_mask = SLI_SE_COMMAND_OPTION_HASH_SHA1;
      break;

    case SL_SE_HASH_SHA224:
      hash_mask = SLI_SE_COMMAND_OPTION_HASH_SHA224;
      break;

    case SL_SE_HASH_SHA256:
      hash_mask = SLI_SE_COMMAND_OPTION_HASH_SHA256;
      break;

    case SL_SE_HASH_SHA384:
      hash_mask = SLI_SE_COMMAND_OPTION_HASH_SHA384;
      break;

    case SL_SE_HASH_SHA512:
      hash_mask = SLI_SE_COMMAND_OPTION_HASH_SHA512;
      break;

    default:
      return SL_STATUS_INVALID_PARAMETER;
      break;
  }

  // SE command structures.
  SE_Command_t *se_cmd = &cmd_ctx->command;
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_DERIVE_KEY_HKDF | hash_mask);

  sli_add_key_parameters(cmd_ctx, in_key, status);
  SE_addParameter(se_cmd, salt_len);
  SE_addParameter(se_cmd, info_len);
  sli_add_key_parameters(cmd_ctx, out_key, status);

  sli_add_key_metadata(cmd_ctx, in_key, status);
  sli_add_key_input(cmd_ctx, in_key, status);

  sli_add_key_metadata_custom(cmd_ctx, auth_data, out_key, status);

  SE_DataTransfer_t salt_in = SE_DATATRANSFER_DEFAULT(salt, salt_len);
  SE_addDataInput(se_cmd, &salt_in);

  SE_DataTransfer_t info_in = SE_DATATRANSFER_DEFAULT(info, info_len);
  SE_addDataInput(se_cmd, &info_in);

  sli_add_key_output(cmd_ctx, out_key, status);

  status = sli_se_execute_and_wait(cmd_ctx);

  return status;
}
#endif // _SILICON_LABS_SECURITY_FEATURE_VAULT

#if (defined(_SILICON_LABS_SECURITY_FEATURE) \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT))
/***************************************************************************//**
 * PBKDF2 key derivation.
 ******************************************************************************/
sl_status_t sl_se_derive_key_pbkdf2(sl_se_command_context_t *cmd_ctx,
                                    const sl_se_key_descriptor_t *in_key,
                                    sl_se_pbkdf2_prf_type_t prf,
                                    const unsigned char *salt,
                                    size_t salt_len,
                                    uint32_t iterations,
                                    sl_se_key_descriptor_t *out_key)
{
  sl_status_t status = SL_STATUS_OK;
  uint32_t command_word = 0U;

  if ((cmd_ctx == NULL) || (in_key == NULL) || (out_key == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((salt == NULL) && (salt_len != 0U)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  // Too many iterations will trigger SE watchdog.
  if ((iterations == 0U) || (iterations > 16384U)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  switch (prf) {
    case SL_SE_PRF_HMAC_SHA1:
      command_word = SLI_SE_COMMAND_DERIVE_KEY_PBKDF2_HMAC
                     | SLI_SE_COMMAND_OPTION_HASH_SHA1;
      break;

    case SL_SE_PRF_HMAC_SHA224:
      command_word = SLI_SE_COMMAND_DERIVE_KEY_PBKDF2_HMAC
                     | SLI_SE_COMMAND_OPTION_HASH_SHA224;
      break;

    case SL_SE_PRF_HMAC_SHA256:
      command_word = SLI_SE_COMMAND_DERIVE_KEY_PBKDF2_HMAC
                     | SLI_SE_COMMAND_OPTION_HASH_SHA256;
      break;

    case SL_SE_PRF_HMAC_SHA384:
      command_word = SLI_SE_COMMAND_DERIVE_KEY_PBKDF2_HMAC
                     | SLI_SE_COMMAND_OPTION_HASH_SHA384;
      break;

    case SL_SE_PRF_HMAC_SHA512:
      command_word = SLI_SE_COMMAND_DERIVE_KEY_PBKDF2_HMAC
                     | SLI_SE_COMMAND_OPTION_HASH_SHA512;
      break;

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG) \
      && (_SILICON_LABS_32B_SERIES_2_CONFIG >= 3)
    // PBKDF2 with CMAC as the PRF was first supported on EFR32xG23.
    case SL_SE_PRF_AES_CMAC_128:
      command_word = SLI_SE_COMMAND_DERIVE_KEY_PBKDF2_CMAC;
      break;
#endif

    default:
      return SL_STATUS_INVALID_PARAMETER;
      break;
  }

  // SE command structures.
  SE_Command_t *se_cmd = &cmd_ctx->command;
  sli_se_command_init(cmd_ctx, command_word);

  sli_add_key_parameters(cmd_ctx, in_key, status);
  SE_addParameter(se_cmd, salt_len);
  SE_addParameter(se_cmd, iterations);
  sli_add_key_parameters(cmd_ctx, out_key, status);

  sli_add_key_metadata(cmd_ctx, in_key, status);
  sli_add_key_input(cmd_ctx, in_key, status);

  sli_add_key_metadata_custom(cmd_ctx, auth_data, out_key, status);

  SE_DataTransfer_t salt_in = SE_DATATRANSFER_DEFAULT(salt, salt_len);
  SE_addDataInput(se_cmd, &salt_in);

  sli_add_key_output(cmd_ctx, out_key, status);

  status = sli_se_execute_and_wait(cmd_ctx);

  return status;
}
#endif // _SILICON_LABS_SECURITY_FEATURE_VAULT

/** @} (end addtogroup sl_se) */

#endif // defined(SEMAILBOX_PRESENT)
