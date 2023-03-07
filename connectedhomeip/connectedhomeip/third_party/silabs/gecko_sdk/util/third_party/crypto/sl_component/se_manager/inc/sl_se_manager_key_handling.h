/***************************************************************************//**
 * @file
 * @brief Silicon Labs Secure Engine Manager key handling.
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
#ifndef SL_SE_MANAGER_KEY_HANDLING_H
#define SL_SE_MANAGER_KEY_HANDLING_H

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT) || defined(DOXYGEN)

/// @addtogroup sl_se_manager
/// @{

#include "sl_se_manager_types.h"
#include "sl_status.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * \addtogroup sl_se_manager_key_handling Key handling
 *
 * \brief Secure Engine key handling API.
 *
 * \details
 *   API for using cryptographic keys with the SE. Contains functionality to
 *   generate, import and export keys to and from protected types like wrapped
 *   or volatile keys.
 *
 * \{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * @brief
 *   Validate key descriptor.
 *
 * @details
 *   Takes a key descriptor and checks if all required properties have been set
 *   for the specific key type.
 *
 * @param[in] key
 *   The key to validate.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_validate_key(const sl_se_key_descriptor_t *key);

/***************************************************************************//**
 * @brief
 *   Generate a random key adhering to the given key descriptor
 *
 * @details
 *   The SE takes a key descriptor and generates a key with the given properties
 *   in the location specified by the descriptor.
 *
 *   If the key size is not aligned to a multiple of words the key
 *   representation is padded in front of the key.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key_out
 *   Description of the key to generate. Sets key parameters and describes the
 *   storage location for the key.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_generate_key(sl_se_command_context_t *cmd_ctx,
                               const sl_se_key_descriptor_t *key_out);

/***************************************************************************//**
 * @brief
 *   Import a key using the SE
 *
 * @details
 *   Protect a key using the SE. Import a plaintext key and store it either in a
 *   volatile slot or as a wrapped key.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key_in
 *   A plaintext key.
 *
 * @param[in] key_out
 *   Either a volatile or a wrapped key with similar properties as key_in.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_import_key(sl_se_command_context_t *cmd_ctx,
                             const sl_se_key_descriptor_t *key_in,
                             const sl_se_key_descriptor_t *key_out);

/***************************************************************************//**
 * @brief
 *   Export a volatile or wrapped key back to plaintext
 *
 * @details
 *   Attempt to have the SE export a volatile or wrapped key back to plaintext
 *   if allowed.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key_in
 *   Either a volatile or a wrapped key with similar properties as key_out.
 *
 * @param[in] key_out
 *   The exported key in plaintext.
 *
 * @return
 *   Status code, @ref sl_status.h.
 *   SL_STATUS_INVALID_PARAMETER if key does not exist.
 ******************************************************************************/
sl_status_t sl_se_export_key(sl_se_command_context_t *cmd_ctx,
                             const sl_se_key_descriptor_t *key_in,
                             const sl_se_key_descriptor_t *key_out);

/***************************************************************************//**
 * @brief
 *   Transfer a volatile or wrapped key to another protected storage.
 *
 * @details
 *   Attempt to have the SE transfer a volatile or wrapped key if allowed.
 *
 * @note
 *   The key stored in the source protected storage will not be deleted.
 *
 *   Transferring a key between the same volatile slot is not allowed.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key_in
 *   Either a volatile or a wrapped key with similar properties as key_out.
 *
 * @param[in] key_out
 *   Either a volatile or a wrapped key with similar properties as key_in.
 *
 * @return
 *   Status code, @ref sl_status.h.
 *   SL_STATUS_INVALID_PARAMETER if key does not exist.
 ******************************************************************************/
sl_status_t sl_se_transfer_key(sl_se_command_context_t *cmd_ctx,
                               const sl_se_key_descriptor_t *key_in,
                               const sl_se_key_descriptor_t *key_out);

/***************************************************************************//**
 * @brief
 *   Export the public part of an ECC keypair
 *
 * @details
 *   The output key must be specified to only contain a public key.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key_in
 *   An asymmetric key with either a private or public part.
 *
 * @param[out] key_out
 *   Describes output key parameters. Should only be set to contain the public
 *   part of the key.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_export_public_key(sl_se_command_context_t *cmd_ctx,
                                    const sl_se_key_descriptor_t *key_in,
                                    const sl_se_key_descriptor_t *key_out);

/***************************************************************************//**
 * @brief
 *   Delete a key from a volatile SE storage slot
 *
 * @details
 *   The given key will be removed from the SE. The key descriptor is not
 *   modified and can be used to generate a new key without any updates.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Key to delete.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_delete_key(sl_se_command_context_t *cmd_ctx,
                             const sl_se_key_descriptor_t *key);

/***************************************************************************//**
 * @brief
 *   Returns the required storage size for the given key
 *
 * @details
 *   Finds the total storage size required for a given key. This includes
 *   storage for the public and private part of asymmetric keys, as well as
 *   overhead for wrapping keys.
 *
 * @param[in] key
 *   The sl_se_key_descriptor_t to find the required storage size for.
 *
 * @param[out] storage_size
 *   The required storage size in bytes.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_get_storage_size(const sl_se_key_descriptor_t *key,
                                   uint32_t *storage_size);

#ifdef __cplusplus
}
#endif

/** @} (end addtogroup sl_se_key) */
/** @} (end addtogroup sl_se) */

#endif // defined(SEMAILBOX_PRESENT)

#endif // SL_SE_MANAGER_KEY_HANDLING_H
