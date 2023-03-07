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
#ifndef SL_SE_MANAGER_UTIL_H
#define SL_SE_MANAGER_UTIL_H

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT) || defined(DOXYGEN)

/// @addtogroup sl_se_manager
/// @{

/***************************************************************************//**
 * @addtogroup sl_se_manager_util Utilities
 *
 * @brief
 *   Device initialisation, debug lock, upgrade functionality, user data...
 *
 * @details
 *   API for managing the Secure Engine or Root code on a device. Upload and
 *   read device configuration.
 *
 * @{
 ******************************************************************************/

#if !defined(SL_TRUSTZONE_NONSECURE)
#include "sl_se_manager_key_handling.h"
#endif
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
// Prototypes

/***************************************************************************//**
 * @brief
 *   Validate SE firmware image.
 *
 * @details
 *   Validate SE firmware image located at given address. This function is
 *   typically used before calling sl_se_apply_se_image.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] image_addr
 *   Pointer to SE image to validate.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t sl_se_check_se_image(sl_se_command_context_t *cmd_ctx,
                                 void *image_addr);

/***************************************************************************//**
 * @brief
 *   Apply SE firmware image.
 *
 * @details
 *   Apply SE firmware image located at given address.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] image_addr
 *   Pointer to SE image to apply.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t sl_se_apply_se_image(sl_se_command_context_t *cmd_ctx,
                                 void *image_addr);

/***************************************************************************//**
 * @brief
 *   Get upgrade status of SE firmware image.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] status
 *   Pointer to 32-bit word where to return upgrade status.
 *
 * @param[in] prev_version
 *   Pointer to 32-bit word where to return previous version.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t sl_se_get_upgrade_status_se_image(sl_se_command_context_t *cmd_ctx,
                                              uint32_t *status,
                                              uint32_t *prev_version);

/***************************************************************************//**
 * @brief
 *   Validate Host firmware image.
 *
 * @details
 *   Validate Host firmware image located at given address. This function is
 *   typically used before calling @ref sl_se_apply_host_image.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] image_addr
 *   Pointer to Host image to validate.
 *
 * @param[in] size
 *   Size of Host image to validate.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t sl_se_check_host_image(sl_se_command_context_t *cmd_ctx,
                                   void *image_addr,
                                   uint32_t size);

/***************************************************************************//**
 * @brief
 *   Apply Host firmware image.
 *
 * @details
 *   Apply Host firmware image located at given address.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] image_addr
 *   Pointer to Host image to apply.
 *
 * @param[in] size
 *   Size of Host image to apply.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t sl_se_apply_host_image(sl_se_command_context_t *cmd_ctx,
                                   void *image_addr,
                                   uint32_t size);

/***************************************************************************//**
 * @brief
 *   Get upgrade status of Host firmware image.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] status
 *   Pointer to 32-bit word where to return upgrade status.
 *
 * @param[in] prev_version
 *   Pointer to 32-bit word where to return previous version.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t
sl_se_get_upgrade_status_host_image(sl_se_command_context_t *cmd_ctx,
                                    uint32_t *status,
                                    uint32_t *prev_version);

/***************************************************************************//**
 * @brief
 *   Initialize key to be stored in the SE OTP flash.
 *
 * @details
 *   Initialize key stored in the SE. The command can be used to write (@ref sl_se_device_key_type_t):
 *   * SL_SE_KEY_TYPE_IMMUTABLE_BOOT
 *   * SL_SE_KEY_TYPE_IMMUTABLE_AUTH
 *   * SL_SE_KEY_TYPE_IMMUTABLE_AES_128
 *
 * @note
 *   These keys can not be overwritten, so this command can only be issued once
 *   per key per part.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key_type
 *   ID of key type to initialize.
 *
 * @param[in] key
 *   Pointer to a buffer that contains the key.
 *   Public keys must be word aligned and have a length of 64 bytes.
 *   AES-128 keys must be word aligned and have length of 16 bytes.
 *
 * @param[in] num_bytes
 *   Length of key buffer in bytes (16 or 64 bytes).
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t sl_se_init_otp_key(sl_se_command_context_t *cmd_ctx,
                               sl_se_device_key_type_t key_type,
                               void *key,
                               uint32_t num_bytes);

/***************************************************************************//**
 * @brief
 *   Read a public key stored in the SE.
 *
 * @details
 *   Read out a public key stored in the SE. The command can be used to read (@ref sl_se_device_key_type_t):
 *   * SL_SE_KEY_TYPE_IMMUTABLE_BOOT
 *   * SL_SE_KEY_TYPE_IMMUTABLE_AUTH
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key_type
 *   ID of key type to read.
 *
 * @param[out] key
 *   Pointer to a buffer to contain the returned public key.
 *   Must be word aligned and have a length of 64 bytes.
 *
 * @param[in] num_bytes
 *   Length of pubkey buffer (64 bytes).
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t sl_se_read_pubkey(sl_se_command_context_t *cmd_ctx,
                              sl_se_device_key_type_t key_type,
                              void *key,
                              uint32_t num_bytes);

/***************************************************************************//**
 * @brief
 *   Initialize SE OTP configuration.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] otp_init
 *   Pointer to OTP initialization structure.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 * @retval SL_STATUS_ABORT when the operation is not attempted.
 ******************************************************************************/
sl_status_t sl_se_init_otp(sl_se_command_context_t *cmd_ctx,
                           sl_se_otp_init_t *otp_init);

/***************************************************************************//**
 * @brief
 *   Read SE OTP configuration.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[out] otp_settings
 *   Pointer to OTP initialization structure.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_COMMAND if OTP configuration isn't initialized
 * @retval SL_STATUS_ABORT when the operation is not attempted.
 ******************************************************************************/
sl_status_t sl_se_read_otp(sl_se_command_context_t *cmd_ctx,
                           sl_se_otp_init_t *otp_settings);

/***************************************************************************//**
 * @brief
 *   Read the SE firmware version.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[out] version
 *   Pointer to uint32_t word where version shall be returned.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_OWNERSHIP when the ownership is already taken
 * @retval SL_STATUS_INVALID_OPERATION when the SE command ID is not recognized
 * @retval SL_STATUS_INVALID_CREDENTIALS when the command is not authorized
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t sl_se_get_se_version(sl_se_command_context_t *cmd_ctx,
                                 uint32_t *version);

/***************************************************************************//**
 * @brief
 *   Returns the current debug lock configuration.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[out] status
 *   Pointer to sl_se_debug_status_t structure to be filled out with the
 *   current status of the debug configuration.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t sl_se_get_debug_lock_status(sl_se_command_context_t *cmd_ctx,
                                        sl_se_debug_status_t *status);

/***************************************************************************//**
 * @brief
 *   Enables the debug lock for the part.
 *
 * @details
 *   The debug port will be closed and the only way to open it is through
 *   device erase (if enabled) or through secure debug unlock (if enabled).
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 ******************************************************************************/
sl_status_t sl_se_apply_debug_lock(sl_se_command_context_t *cmd_ctx);

#if defined(SEMAILBOX_PRESENT) || defined(DOXYGEN)
/***************************************************************************//**
 * @brief
 *   Writes data to User Data section in MTP. Write data must be aligned to
 *    word size and contain a number of bytes that is divisable by four.
 * @note
 *   It is recommended to erase the flash page before performing a write.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 * @param[in] offset
 *   Offset to the flash word to write to. Must be aligned to words.
 * @param[in] data
 *   Data to write to flash.
 * @param[in] num_bytes
 *   Number of bytes to write to flash. NB: Must be divisable by four.
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_OPERATION when the SE command ID is not recognized
 * @retval SL_STATUS_INVALID_CREDENTIALS when the command is not authorized
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t sl_se_write_user_data(sl_se_command_context_t *cmd_ctx,
                                  uint32_t offset,
                                  void *data,
                                  uint32_t num_bytes);

/***************************************************************************//**
 * @brief
 *   Erases User Data section in MTP.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_OPERATION when the SE command ID is not recognized
 * @retval SL_STATUS_INVALID_CREDENTIALS when the command is not authorized
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t sl_se_erase_user_data(sl_se_command_context_t *cmd_ctx);

/***************************************************************************//**
 * @brief
 *   Returns the current boot status, versions and system configuration.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[out] status
 *   SE_Status_t containing current SE status.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK upon command completion. Errors are encoded in the
 *                        different parts of the returned status object.
 * @retval SL_STATUS_INVALID_OPERATION when the SE command ID is not recognized
 * @retval SL_STATUS_INVALID_CREDENTIALS when the command is not authorized
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t sl_se_get_status(sl_se_command_context_t *cmd_ctx,
                             sl_se_status_t *status);

/***************************************************************************//**
 * @brief
 *   Read the serial number of the SE module.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[out] serial
 *   Pointer to array of size 16 bytes.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_OPERATION when the SE command ID is not recognized
 * @retval SL_STATUS_INVALID_CREDENTIALS when the command is not authorized
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t sl_se_get_serialnumber(sl_se_command_context_t *cmd_ctx,
                                   void *serial);

/***************************************************************************//**
 * @brief
 *   Read the OTP firmware version of the SE module.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[out] version
 *   Pointer to uint32_t word where version shall be returned.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_OPERATION when the SE command ID is not recognized
 * @retval SL_STATUS_INVALID_CREDENTIALS when the command is not authorized
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t sl_se_get_otp_version(sl_se_command_context_t *cmd_ctx,
                                  uint32_t *version);

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1) || defined(DOXYGEN)
/***************************************************************************//**
 * @brief
 *   Read the EMU->RSTCAUSE after a tamper reset. This function should be called
 *   if EMU->RSTCAUSE has been cleared upon boot.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[out] reset_cause
 *   Pointer to uint32_t word where reset cause shall be returned.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_OPERATION when the SE command ID is not recognized
 ******************************************************************************/
sl_status_t sl_se_get_reset_cause(sl_se_command_context_t *cmd_ctx,
                                  uint32_t* reset_cause);
#endif

/***************************************************************************//**
 * @brief
 *   Enables the secure debug functionality.
 *
 * @details
 *   Enables the secure debug functionality that can be used to open a locked
 *   debug port through the Get challenge and Open debug commands. This command
 *   can only be executed before the debug port is locked, and after a secure
 *   debug public key has been installed in the SE.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 ******************************************************************************/
sl_status_t sl_se_enable_secure_debug(sl_se_command_context_t *cmd_ctx);

/***************************************************************************//**
 * @brief
 *   Disables the secure debug functionality.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @details
 *   Disables the secure debug functionality that can be used to open a
 *   locked debug port.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 ******************************************************************************/
sl_status_t sl_se_disable_secure_debug(sl_se_command_context_t *cmd_ctx);

/***************************************************************************/ /**
 * @brief
 *   Set debug options.
 *
 * @details
 *   This function makes it possible to configure the Trust-Zone access
 *   permissions of the debug interface. For details please refer to
 *   @ref sl_se_debug_options_t.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] debug_options
 *   Pointer to debug options structure.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 ******************************************************************************/
sl_status_t sl_se_set_debug_options(sl_se_command_context_t *cmd_ctx,
                                    const sl_se_debug_options_t *debug_options);

/***************************************************************************//**
 * @brief
 *   Performs a device mass erase and debug unlock.
 *
 * @details
 *   Performs a device mass erase and resets the debug configuration to its
 *   initial unlocked state. Only available before DEVICE_ERASE_DISABLE has
 *   been executed.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @note
 *   This command clears and verifies the complete flash and ram of the
 *   system, excluding the user data pages and one-time programmable
 *   commissioning information in the secure engine.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_COMMAND if device erase is disabled.
 ******************************************************************************/
sl_status_t sl_se_erase_device(sl_se_command_context_t *cmd_ctx);

/****************************************************************q***********//**
 * @brief
 *   Disabled device erase functionality.
 *
 * @details
 *   This command disables the device erase command. It does not lock the
 *   debug interface to the part, but it is a permanent action for the part.
 *   If device erase is disabled and the device is debug locked, there is no
 *   way to permanently unlock the part. If secure debug unlock is enabled,
 *   secure debug unlock can still be used to temporarily open the debug port.
 *
 * @warning
 *   This command permanently disables the device erase functionality!
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 ******************************************************************************/
sl_status_t sl_se_disable_device_erase(sl_se_command_context_t *cmd_ctx);

/***************************************************************************//**
 * @brief
 *   Request challenge from SE which can be used to open debug access.
 *
 * @details
 *   This command requests a challenge (16 bytes) which can be used to generate
 *   a certificate in order to open debug access, @ref sl_se_open_debug.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[out] challenge
 *   SE challenge storage.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t sl_se_get_challenge(sl_se_command_context_t *cmd_ctx,
                                sl_se_challenge_t challenge);

/***************************************************************************//**
 * @brief
 *   Invalidate current challenge and make a new challenge.
 *
 * @details
 *   This command requests the SE to invalidate it's current challenge (16bytes)
 *   and generate a new challenge.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 ******************************************************************************/
sl_status_t sl_se_roll_challenge(sl_se_command_context_t *cmd_ctx);

/***************************************************************************//**
 * @brief
 *   Unlock debug access using certificate and signed challenge.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] cert
 *   Certificate for debug unlock and signed challenge.
 *
 * @param[in] len
 *   Length of certificate in number of bytes.
 *
 * @param[in] debug_options
 *   Debug options to open/unlock.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_OPERATION when the SE command ID is not recognized
 * @retval SL_STATUS_INVALID_CREDENTIALS when the command is not authorized
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t sl_se_open_debug(sl_se_command_context_t *cmd_ctx,
                             void *cert,
                             uint32_t len,
                             const sl_se_debug_options_t *debug_options);

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
/***************************************************************************//**
 * @brief
 *   Temporarily disable tamper configuration using certificate and signed
 *   challenge.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] cert
 *   Certificate for disabling tamper and signed challenge.
 *
 * @param[in] len
 *   Length of certificate in number of bytes.
 *
 * @param[in] tamper_signals
 *   Tamper signals to disable. Each signal represented by a bit.
 *
 * @return
 *   One of the following sl_status_t codes:
 * @retval SL_STATUS_OK when the command was executed successfully
 * @retval SL_STATUS_INVALID_OPERATION when the SE command ID is not recognized
 * @retval SL_STATUS_INVALID_CREDENTIALS when the command is not authorized
 * @retval SL_STATUS_INVALID_PARAMETER when an invalid parameter was passed
 ******************************************************************************/
sl_status_t sl_se_disable_tamper(sl_se_command_context_t *cmd_ctx,
                                 void *cert,
                                 uint32_t len,
                                 sl_se_tamper_signals_t tamper_signals);

#endif // (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)

/***************************************************************************//**
 * @brief
 *   Read size of stored certificates in SE.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in,out] cert_size
 *   Size of the certificates stored in SE.

 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_read_cert_size(sl_se_command_context_t *cmd_ctx,
                                 sl_se_cert_size_type_t *cert_size);

/***************************************************************************//**
 * @brief
 *   Read stored certificates in SE.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] cert_type
 *   Type of the certificate stored in SE.
 *
 * @param[in,out] cert
 *   Buffer to read certificate.
 *
 * @param[in] num_bytes
 *   Length of certificate in number of bytes.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_read_cert(sl_se_command_context_t *cmd_ctx,
                            sl_se_cert_type_t cert_type,
                            void *cert,
                            uint32_t num_bytes);

#endif // defined(SEMAILBOX_PRESENT)

#ifdef __cplusplus
}
#endif

/// @} (end addtogroup sl_se_manager_util)
/// @} (end addtogroup sl_se_manager)

#endif // defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)

#endif // SL_SE_MANAGER_UTIL_H
