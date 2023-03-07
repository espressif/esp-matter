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
#ifndef SL_SE_MANAGER_ATTESTATION_H
#define SL_SE_MANAGER_ATTESTATION_H

#include "em_device.h"

#if (defined(SEMAILBOX_PRESENT)                                                \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)) \
  || defined(DOXYGEN)

/// @addtogroup sl_se_manager
/// @{

/***************************************************************************//**
 * @addtogroup sl_se_manager_attestation Attestation
 *
 * @brief
 *   System and configuration attestation
 *
 * @details
 *   API for retrieveing attestation tokens from the SE.
 *
 * @{
 ******************************************************************************/

#include "sl_se_manager_key_handling.h"
#include "sl_se_manager_types.h"
#include "em_se.h"
#include "sl_status.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Defines

/// 32 byte challenge size
#define SL_SE_ATTESTATION_CHALLENGE_SIZE_32  (32U)
/// 48 byte challenge size
#define SL_SE_ATTESTATION_CHALLENGE_SIZE_48  (48U)
/// 64 byte challenge size
#define SL_SE_ATTESTATION_CHALLENGE_SIZE_64  (64U)

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * @brief
 *   Get the PSA initial attest token from the SE
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
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
 *   Size of token_buf in bytes. Must be at least the size found by calling
 *   \ref sl_se_attestation_get_psa_iat_token_size with equivalent arguments,
 *   and padded to word alignment.
 *
 * @param[out] token_size
 *   Number of bytes actually used in token_buf.
 *
 * @warning
 *   Once a nonce/challenge has been used, the same challenge should not be used
 *   ever again, to prevent replay attacks.
 *
 * @warning
 *   The output will be length-extended to the next word-multiple.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_attestation_get_psa_iat_token(sl_se_command_context_t *cmd_ctx,
                                                const uint8_t *auth_challenge,
                                                size_t challenge_size,
                                                uint8_t *token_buf,
                                                size_t token_buf_size,
                                                size_t *token_size);

/***************************************************************************//**
 * @brief
 *   Get the size of a PSA initial attest token with the given nonce
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] challenge_size
 *   Size of the challenge object in bytes. Must be either 32, 48 or 64.
 *
 * @param[out] token_size
 *   Pointer to output word. Result is stored here.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_attestation_get_psa_iat_token_size(sl_se_command_context_t *cmd_ctx,
                                                     size_t challenge_size,
                                                     size_t *token_size);

/***************************************************************************//**
 * @brief
 *    Get an attested (signed) security configuration token from the SE
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] auth_challenge
 *   Buffer with a challenge object selected by the caller.
 *
 * @param[in] challenge_size
 *   Size of the challenge object in bytes. Must be 32.
 *
 * @param[out] token_buf
 *   Buffer where the output token will be stored.
 *
 * @param[in] token_buf_size
 *   Size of token_buf in bytes. Must be at least the size found by calling
 *   \ref sl_se_attestation_get_config_token_size with equivalent arguments,
 *   and padded to word alignment.
 *
 * @param[out] token_size
 *   Number of bytes actually used in token_buf.
 *
 * @warning
 *   Once a nonce/challenge has been used, the same challenge should not be used
 *   ever again, to prevent replay attacks.
 *
 * @warning
 *   The output will be length-extended to the next word-multiple.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_attestation_get_config_token(sl_se_command_context_t *cmd_ctx,
                                               const uint8_t *auth_challenge,
                                               size_t challenge_size,
                                               uint8_t *token_buf,
                                               size_t token_buf_size,
                                               size_t *token_size);

/***************************************************************************//**
 * @brief
 *    Get the size of a security configuration token
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] challenge_size
 *   Size of the challenge object in bytes. Must be 32.
 *
 * @param[out] token_size
 *   Pointer to output word. Result is stored here.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_attestation_get_config_token_size(sl_se_command_context_t *cmd_ctx,
                                                    size_t challenge_size,
                                                    size_t *token_size);

#ifdef __cplusplus
}
#endif

/// @} (end addtogroup sl_se_manager_attestation)
/// @} (end addtogroup sl_se_manager)

#endif // SEMAILBOX_PRESENT && VAULT

#endif // SL_SE_MANAGER_ATTESTATION_H
