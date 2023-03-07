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
#include "sl_se_manager_attestation.h"
#include "em_se.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @addtogroup sl_se_manager
/// @{

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
// -----------------------------------------------------------------------------
// Static Functions

/***************************************************************************//**
 * @brief
 *   Validate the command word, challenge size and update command word with
 *   option flags
 *
 * @param[in, out] command_word
 *   The command word to send to the SE. Will be modified to reflect challenge
 *   size.
 *
 * @param challenge_size
 *   Size of the challenge to be used.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
static sl_status_t validate_and_update_command_word(uint32_t *command_word,
                                                    size_t challenge_size)
{
  // Check supported challenge sizes
  uint32_t command_id = *command_word & 0xFFFF0000UL;
  if (command_id == SLI_SE_COMMAND_ATTEST_PSA_IAT) {
    switch (challenge_size) {
      case SL_SE_ATTESTATION_CHALLENGE_SIZE_32:
        *command_word |= 0x01 << 8;
        break;
      case SL_SE_ATTESTATION_CHALLENGE_SIZE_48:
        *command_word |= 0x02 << 8;
        break;
      case SL_SE_ATTESTATION_CHALLENGE_SIZE_64:
        *command_word |= 0x03 << 8;
        break;
      default:
        return SL_STATUS_INVALID_PARAMETER;
    }
  } else if (command_id == SLI_SE_COMMAND_ATTEST_CONFIG) {
    if (challenge_size != SL_SE_ATTESTATION_CHALLENGE_SIZE_32) {
      return SL_STATUS_INVALID_PARAMETER;
    }
    // No need to do anything with the command word
  } else {
    // Unknown command ID
    return SL_STATUS_INVALID_PARAMETER;
  }
  // All the checks passed
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Get the PSA initial attest token from the SE
 *
 * @param[in] cmd_ctx
 *   SE command context struct.
 *
 * @param[in] challenge_size
 *   Size of the challenge object in bytes. Must be either 32, 48 or 64.
 *
 * @param[out] token_size
 *   Number of bytes actually used in token_buf.
 *
 * @param[in] command_word
 *   The command word to send to the SE, to differentiat between token types
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
static sl_status_t get_attestation_token_size(sl_se_command_context_t *cmd_ctx,
                                              size_t challenge_size,
                                              size_t *token_size,
                                              uint32_t command_word)
{
  // Parameter check
  if (cmd_ctx == NULL || token_size == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check command word and challenge size
  sl_status_t status = validate_and_update_command_word(&command_word,
                                                        challenge_size);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Use a dummy nonce since the SE requires nonce input even if we just
  // want to find the token length
  uint8_t dummy_nonce[SL_SE_ATTESTATION_CHALLENGE_SIZE_64] = { 0 };

  // Build and execute the command
  SE_Command_t *se_cmd = &cmd_ctx->command;
  // Or comman word with 0x01 to enable length output only
  sli_se_command_init(cmd_ctx, command_word | 0x01UL);
  SE_DataTransfer_t noncedata =
    SE_DATATRANSFER_DEFAULT(dummy_nonce, challenge_size);
  SE_addDataInput(se_cmd, &noncedata);
  SE_DataTransfer_t sizedata =
    SE_DATATRANSFER_DEFAULT(token_size, sizeof(*token_size));
  SE_addDataOutput(se_cmd, &sizedata);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * @brief
 *   Get an attestation token from the SE
 *
 * @param[in] cmd_ctx
 *   SE command context struct.
 *
 * @param[in] auth_challenge
 *   Buffer with a challenge object selected by the caller.
 *
 * @param[in] challenge_size
 *   Size of the challenge object in bytes. Must be either 32, 48 or 64.
 *
 * @param[out] token_buf
 *   Buffer where the output token will be stored.
 *
 * @param[in] token_buf_size
 *   Size of token_buf in bytes.
 *
 * @param[out] token_size
 *   Number of bytes actually used in token_buf.
 *
 * @param[in] command_word
 *   The command word to send to the SE, to differentiat between token types
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
static sl_status_t get_attestation_token(sl_se_command_context_t *cmd_ctx,
                                         const uint8_t *auth_challenge,
                                         size_t challenge_size,
                                         uint8_t *token_buf,
                                         size_t token_buf_size,
                                         size_t *token_size,
                                         uint32_t command_word)
{
  // Parameter check
  if (cmd_ctx == NULL
      || auth_challenge == NULL
      || token_buf == NULL
      || token_size == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check supported challenge sizes
  sl_status_t status = validate_and_update_command_word(&command_word,
                                                        challenge_size);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Check that buffer is sufficiently large
  status = get_attestation_token_size(cmd_ctx,
                                      challenge_size,
                                      token_size,
                                      command_word);
  if (status != SL_STATUS_OK) {
    return status;
  }
  if (((*token_size + 0x3) & ~0x3) > token_buf_size) {
    return SL_STATUS_WOULD_OVERFLOW;
  }

  // Build and execute the command
  SE_Command_t *se_cmd = &cmd_ctx->command;
  sli_se_command_init(cmd_ctx, command_word);
  SE_DataTransfer_t noncedata =
    SE_DATATRANSFER_DEFAULT(auth_challenge, challenge_size);
  SE_addDataInput(se_cmd, &noncedata);
  SE_DataTransfer_t sizedata =
    SE_DATATRANSFER_DEFAULT(token_size, sizeof(*token_size));
  SE_addDataOutput(se_cmd, &sizedata);
  SE_DataTransfer_t tokendata =
    SE_DATATRANSFER_DEFAULT(token_buf, ((*token_size + 0x3) & ~0x3));
  SE_addDataOutput(se_cmd, &tokendata);

  return sli_se_execute_and_wait(cmd_ctx);
}

// -----------------------------------------------------------------------------
// Global Functions

/***************************************************************************//**
 * Get the PSA initial attest token from the SE
 ******************************************************************************/
sl_status_t sl_se_attestation_get_psa_iat_token(sl_se_command_context_t *cmd_ctx,
                                                const uint8_t *auth_challenge,
                                                size_t challenge_size,
                                                uint8_t *token_buf,
                                                size_t token_buf_size,
                                                size_t *token_size)
{
  return get_attestation_token(cmd_ctx,
                               auth_challenge,
                               challenge_size,
                               token_buf,
                               token_buf_size,
                               token_size,
                               SLI_SE_COMMAND_ATTEST_PSA_IAT);
}

/***************************************************************************//**
 * Get the size of a PSA initial attest token with the given nonce
 ******************************************************************************/
sl_status_t sl_se_attestation_get_psa_iat_token_size(sl_se_command_context_t *cmd_ctx,
                                                     size_t challenge_size,
                                                     size_t *token_size)
{
  return get_attestation_token_size(cmd_ctx,
                                    challenge_size,
                                    token_size,
                                    SLI_SE_COMMAND_ATTEST_PSA_IAT);
}

/***************************************************************************//**
 * Get an attested (signed) security configuration token from the SE
 ******************************************************************************/
sl_status_t sl_se_attestation_get_config_token(sl_se_command_context_t *cmd_ctx,
                                               const uint8_t *auth_challenge,
                                               size_t challenge_size,
                                               uint8_t *token_buf,
                                               size_t token_buf_size,
                                               size_t *token_size)
{
  return get_attestation_token(cmd_ctx,
                               auth_challenge,
                               challenge_size,
                               token_buf,
                               token_buf_size,
                               token_size,
                               SLI_SE_COMMAND_ATTEST_CONFIG);
}

/***************************************************************************//**
 * Get the size of a security configuration token
 ******************************************************************************/
sl_status_t sl_se_attestation_get_config_token_size(sl_se_command_context_t *cmd_ctx,
                                                    size_t challenge_size,
                                                    size_t *token_size)
{
  return get_attestation_token_size(cmd_ctx,
                                    challenge_size,
                                    token_size,
                                    SLI_SE_COMMAND_ATTEST_CONFIG);
}

#endif // (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)

#ifdef __cplusplus
}
#endif

/** @} (end addtogroup sl_se_manager) */

#endif // defined(SEMAILBOX_PRESENT)
