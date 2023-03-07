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

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)

#include "sl_se_manager_util.h"
#include "sli_se_manager_internal.h"
#include "em_se.h"
#include "sl_assert.h"
#include "em_system.h"

/// @addtogroup sl_se_manager
/// @{

// -----------------------------------------------------------------------------
// Defines

// OTP initialization structure defines.
#define SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_ENABLE (1 << 16)
#define SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_VERIFY_CERTIFICATE (1 << 17)
#define SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_ANTI_ROLLBACK (1 << 18)
#define SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_PAGE_LOCK_NARROW (1 << 19)
#define SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_PAGE_LOCK_FULL (1 << 20)

// -----------------------------------------------------------------------------
// Local Functions

/***************************************************************************//**
 * @brief
 *   Decode debug status word (as received from the SE).
 *
 * @return N/A
 ******************************************************************************/
#if defined(SEMAILBOX_PRESENT)
static void decode_debug_status(sl_se_debug_status_t *debug_status,
                                uint32_t status_word)
{
  debug_status->debug_port_lock_applied = status_word & (1 << 0);
  debug_status->device_erase_enabled = status_word & (1 << 1);
  debug_status->secure_debug_enabled = status_word & (1 << 2);
  debug_status->debug_port_lock_state = status_word & (1 << 5);
  debug_status->options_state.non_secure_invasive_debug =
    (status_word & (1 << 6)) == 0;
  debug_status->options_state.non_secure_non_invasive_debug =
    (status_word & (1 << 7)) == 0;
  debug_status->options_state.secure_invasive_debug =
    (status_word & (1 << 8)) == 0;
  debug_status->options_state.secure_non_invasive_debug =
    (status_word & (1 << 9)) == 0;
  debug_status->options_config.non_secure_invasive_debug =
    (status_word & (1 << 10)) == 0;
  debug_status->options_config.non_secure_non_invasive_debug =
    (status_word & (1 << 11)) == 0;
  debug_status->options_config.secure_invasive_debug =
    (status_word & (1 << 12)) == 0;
  debug_status->options_config.secure_non_invasive_debug =
    (status_word & (1 << 13)) == 0;
}
#elif defined(CRYPTOACC_PRESENT)
static void decode_debug_status(sl_se_debug_status_t *debug_status,
                                uint32_t status_word)
{
  debug_status->debug_port_lock_applied = status_word & (1 << 10);
  debug_status->device_erase_enabled = status_word & (1 << 11);
  debug_status->secure_debug_enabled = status_word & (1 << 12);
  debug_status->debug_port_lock_state = status_word & (1 << 15);
}
#endif // defined(SEMAILBOX_PRESENT)

// -----------------------------------------------------------------------------
// Global Functions

/***************************************************************************//**
 * Validate SE firmware image.
 ******************************************************************************/
sl_status_t sl_se_check_se_image(sl_se_command_context_t *cmd_ctx,
                                 void *image_addr)
{
  if (cmd_ctx == NULL || image_addr == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  // SE command structures
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_CHECK_SE_IMAGE);

  SE_addParameter(se_cmd, (uint32_t)image_addr);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Apply SE firmware image.
 ******************************************************************************/
sl_status_t sl_se_apply_se_image(sl_se_command_context_t *cmd_ctx,
                                 void *image_addr)
{
  if (cmd_ctx == NULL || image_addr == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  // SE command structures
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_APPLY_SE_IMAGE);

  SE_addParameter(se_cmd, (uint32_t)image_addr);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Get upgrade status of SE firmware image.
 ******************************************************************************/
sl_status_t sl_se_get_upgrade_status_se_image(sl_se_command_context_t *cmd_ctx,
                                              uint32_t *status,
                                              uint32_t *prev_version)
{
  if (cmd_ctx == NULL || status == NULL || prev_version == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  // SE command structures
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_STATUS_SE_IMAGE);

  volatile uint32_t out_buf[2];
  SE_DataTransfer_t out_data = SE_DATATRANSFER_DEFAULT(out_buf,
                                                       sizeof(out_buf));
  SE_addDataOutput(se_cmd, &out_data);

  sl_status_t ret = sli_se_execute_and_wait(cmd_ctx);

  if (ret == SL_STATUS_OK) {
    *status = out_buf[0];
    *prev_version = out_buf[1];
  }

  return ret;
}

/***************************************************************************//**
 * Validate Host firmware image.
 ******************************************************************************/
sl_status_t sl_se_check_host_image(sl_se_command_context_t *cmd_ctx,
                                   void *image_addr,
                                   uint32_t size)
{
  if (cmd_ctx == NULL || image_addr == NULL || size == 0UL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  // SE command structures
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_CHECK_HOST_IMAGE);

  SE_addParameter(se_cmd, (uint32_t)image_addr);
  SE_addParameter(se_cmd, size);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Apply Host firmware image.
 ******************************************************************************/
sl_status_t sl_se_apply_host_image(sl_se_command_context_t *cmd_ctx,
                                   void *image_addr,
                                   uint32_t size)
{
  if (cmd_ctx == NULL || image_addr == NULL || size == 0UL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  // SE command structures
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_APPLY_HOST_IMAGE);

  SE_addParameter(se_cmd, (uint32_t)image_addr);
  SE_addParameter(se_cmd, size);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Get upgrade status of Host firmware image.
 ******************************************************************************/
sl_status_t
sl_se_get_upgrade_status_host_image(sl_se_command_context_t *cmd_ctx,
                                    uint32_t *status,
                                    uint32_t *prev_version)
{
  if (cmd_ctx == NULL || status == NULL || prev_version == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  // SE command structures
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_STATUS_HOST_IMAGE);

  volatile uint32_t out_buf[2];
  SE_DataTransfer_t out_data = SE_DATATRANSFER_DEFAULT(out_buf,
                                                       sizeof(out_buf));
  SE_addDataOutput(se_cmd, &out_data);

  sl_status_t ret = sli_se_execute_and_wait(cmd_ctx);

  if (ret == SL_STATUS_OK) {
    *status = out_buf[0];
    *prev_version = out_buf[1];
  }

  return ret;
}

/***************************************************************************//**
 * Initialize key to be stored in the SE OTP flash.
 ******************************************************************************/
sl_status_t sl_se_init_otp_key(sl_se_command_context_t *cmd_ctx,
                               sl_se_device_key_type_t key_type,
                               void *key,
                               uint32_t num_bytes)
{
  if (cmd_ctx == NULL || key == NULL || num_bytes == 0UL || (size_t)key & 3U) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  #if defined(SEMAILBOX_PRESENT)
  if (key_type == SL_SE_KEY_TYPE_IMMUTABLE_AES_128) {
    if (num_bytes != 16UL) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  } else {
    if (num_bytes != 64UL) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  #elif defined(CRYPTOACC_PRESENT)
  if (num_bytes != 64UL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  #endif

  uint32_t command_word;
  SE_Command_t *se_cmd = &cmd_ctx->command;

  uint32_t se_key_type;
  switch (key_type) {
    case SL_SE_KEY_TYPE_IMMUTABLE_BOOT:
      se_key_type = SLI_SE_KEY_TYPE_BOOT;
      break;

    case SL_SE_KEY_TYPE_IMMUTABLE_AUTH:
      se_key_type = SLI_SE_KEY_TYPE_AUTH;
      break;

    #if defined(SEMAILBOX_PRESENT)
    case SL_SE_KEY_TYPE_IMMUTABLE_AES_128:
      se_key_type = SLI_SE_IMMUTABLE_KEY_TYPE_AES_128;
      break;
    #endif // SEMAILBOX_PRESENT

    default:
      return SL_STATUS_INVALID_PARAMETER;
      break;
  }

  // Find parity word
  volatile uint32_t parity = 0;
  for (size_t i = 0; i < num_bytes / 4; i++) {
    parity = parity ^ ((uint32_t *)key)[i];
  }

  // SE command structures
  #if defined(SEMAILBOX_PRESENT)
  command_word = key_type == SL_SE_KEY_TYPE_IMMUTABLE_AES_128
                 ? SLI_SE_COMMAND_INIT_AES_128_KEY : SLI_SE_COMMAND_INIT_PUBKEY;
  #elif defined(CRYPTOACC_PRESENT)
  command_word = SLI_SE_COMMAND_INIT_PUBKEY;
  #endif

  sli_se_command_init(cmd_ctx, (command_word | se_key_type));

  SE_DataTransfer_t parity_data = SE_DATATRANSFER_DEFAULT(&parity, 4);
  SE_addDataInput(se_cmd, &parity_data);

  SE_DataTransfer_t key_data = SE_DATATRANSFER_DEFAULT(key, num_bytes);
  SE_addDataInput(se_cmd, &key_data);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Read a public key stored in the SE.
 ******************************************************************************/
sl_status_t sl_se_read_pubkey(sl_se_command_context_t *cmd_ctx,
                              sl_se_device_key_type_t key_type,
                              void *key,
                              uint32_t num_bytes)
{
  if (cmd_ctx == NULL || key == NULL || num_bytes != 64UL || (size_t)key & 3U) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  uint32_t se_key_type;
  uint32_t command_word = SLI_SE_COMMAND_READ_PUBKEY;
  switch (key_type) {
    case SL_SE_KEY_TYPE_IMMUTABLE_BOOT:
      se_key_type = SLI_SE_KEY_TYPE_BOOT;
      break;

    case SL_SE_KEY_TYPE_IMMUTABLE_AUTH:
      se_key_type = SLI_SE_KEY_TYPE_AUTH;
      break;
    #if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case SL_SE_KEY_TYPE_IMMUTABLE_SE_ATTESTATION:
      command_word = command_word & ~0x1;
    // Intentional fallthrough
    case SL_SE_KEY_TYPE_IMMUTABLE_ATTESTATION:
      se_key_type = SLI_SE_KEY_TYPE_ATTEST;
      break;
    #endif // _SILICON_LABS_SECURITY_FEATURE_VAULT

    default:
      return SL_STATUS_INVALID_PARAMETER;
      break;
  }

  // SE command structures
  sli_se_command_init(cmd_ctx, command_word | se_key_type);

  SE_DataTransfer_t out_data = SE_DATATRANSFER_DEFAULT(key, num_bytes);
  SE_addDataOutput(se_cmd, &out_data);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Read the SE firmware version.
 ******************************************************************************/
sl_status_t sl_se_get_se_version(sl_se_command_context_t *cmd_ctx,
                                 uint32_t *version)
{
  if (cmd_ctx == NULL || version == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  #if defined(SEMAILBOX_PRESENT)

  // SE command structures
  SE_Command_t *se_cmd = &cmd_ctx->command;
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_STATUS_SE_VERSION);
  SE_DataTransfer_t out_data = SE_DATATRANSFER_DEFAULT(version, sizeof(uint32_t));

  SE_addDataOutput(se_cmd, &out_data);

  return sli_se_execute_and_wait(cmd_ctx);

  #elif defined(CRYPTOACC_PRESENT)

  sl_status_t status = SL_STATUS_OK;
  SE_Response_t command_response;

  // Try to acquire SE lock.
  // Need to protect VSE mailbox from being written by e.g. SE_ackCommand()
  status = sli_se_lock_acquire();
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Read SE version from VSE mailbox.
  command_response = SE_getVersion(version);

  // Release SE lock
  status = sli_se_lock_release();

  // Return sl_status_t code.
  if (command_response == SLI_SE_RESPONSE_OK) {
    return status;
  } else {
    // Convert from SE_Response_t to sl_status_t code and return.
    return sli_se_to_sl_status(command_response);
  }

  #endif
}

/***************************************************************************//**
 * Enables the debug lock for the part.
 ******************************************************************************/
sl_status_t sl_se_apply_debug_lock(sl_se_command_context_t *cmd_ctx)
{
  if (cmd_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_DBG_LOCK_APPLY);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Returns the current debug lock configuration.
 ******************************************************************************/
sl_status_t sl_se_get_debug_lock_status(sl_se_command_context_t *cmd_ctx,
                                        sl_se_debug_status_t *debug_status)
{
  if (cmd_ctx == NULL || debug_status == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  #if defined(SEMAILBOX_PRESENT)
  SE_Command_t *se_cmd = &cmd_ctx->command;
  volatile uint32_t status_word = 0;
  SE_DataTransfer_t out_data = SE_DATATRANSFER_DEFAULT(&status_word, 4);

  // Initialize SE command structures
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_DBG_LOCK_STATUS);
  SE_addDataOutput(se_cmd, &out_data);

  sl_status_t ret = sli_se_execute_and_wait(cmd_ctx);

  if (ret == SL_STATUS_OK) {
    decode_debug_status(debug_status, status_word);
  }

  return ret;
  #elif defined(CRYPTOACC_PRESENT)
  uint32_t vse_version = 0;
  uint32_t debug_lock_flags = 0;

  // Try to acquire SE lock
  sl_status_t status = sli_se_lock_acquire();
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Read SE version from VSE mailbox.
  SE_Response_t vse_mbx_status = SE_getVersion(&vse_version);

  // Reading debug lock status is not supported on VSE with versions <= 1.2.2.
  if ((vse_version <= 0x1010202UL) || (vse_mbx_status != SE_RESPONSE_OK)) {
    sli_se_lock_release();
    return SL_STATUS_COMMAND_IS_INVALID;
  }

  vse_mbx_status = SE_getConfigStatusBits(&debug_lock_flags);
  // Release SE lock
  status = sli_se_lock_release();

  if (vse_mbx_status != SE_RESPONSE_OK) {
    return sli_se_to_sl_status(vse_mbx_status);
  } else if (status != SL_STATUS_OK) {
    return status;
  }

  decode_debug_status(debug_status, debug_lock_flags);

  return SL_STATUS_OK;
  #endif
}

#if defined(SEMAILBOX_PRESENT)

/***************************************************************************//**
 * Initialize SE OTP configuration.
 ******************************************************************************/
sl_status_t sl_se_init_otp(sl_se_command_context_t *cmd_ctx,
                           sl_se_otp_init_t *otp_init)
{
  if (cmd_ctx == NULL || otp_init == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  uint32_t mcu_settings_flags = 0;

  sl_status_t status;

  if (otp_init->enable_secure_boot) {
    mcu_settings_flags |= SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_ENABLE;

    // Check for installed boot pubkey before OTP initialization
    uint8_t pubkey[64];
    status =
      sl_se_read_pubkey(cmd_ctx, SL_SE_KEY_TYPE_IMMUTABLE_BOOT, &pubkey, 64);
    if (status != SL_STATUS_OK) {
      return SL_STATUS_ABORT;
    }
  }
  if (otp_init->verify_secure_boot_certificate) {
    mcu_settings_flags |=
      SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_VERIFY_CERTIFICATE;
  }
  if (otp_init->enable_anti_rollback) {
    // Verify firmware compatibility before enabling anti-rollback
    #if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
    uint16_t part_number = SYSTEM_GetPartNumber();
    if (part_number == 1010 || part_number == 1020) {
      if (SYSTEM_GetProdRev() < 16) {
        sl_se_status_t se_status;
        status = sl_se_get_status(cmd_ctx, &se_status);
        if ((status != SL_STATUS_OK)
            || (se_status.se_fw_version < 0x00010201)) {
          // If the following error is returned, the SE firmware version
          // needs to be upgraded to v1.2.1 or higher before enabling
          // anti-rollback.
          EFM_ASSERT(false);
          return SL_STATUS_ABORT;
        }
      }
    }
    #endif

    mcu_settings_flags |= SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_ANTI_ROLLBACK;
  }
  if (otp_init->secure_boot_page_lock_narrow) {
    mcu_settings_flags |= SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_PAGE_LOCK_NARROW;
  }
  if (otp_init->secure_boot_page_lock_full) {
    mcu_settings_flags |= SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_PAGE_LOCK_FULL;
  }

  #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  static struct {
    uint8_t levels[SL_SE_TAMPER_SIGNAL_NUM_SIGNALS / 2];
    uint8_t period;
    uint8_t threshold;
    uint8_t flags;
    uint8_t reset_threshold;
  } otp_tamper_settings;

  // Check for reserved sources
  if ((otp_init->tamper_levels[SL_SE_TAMPER_SIGNAL_RESERVED_1] != SL_SE_TAMPER_LEVEL_IGNORE)
      || (otp_init->tamper_levels[SL_SE_TAMPER_SIGNAL_RESERVED_2] != SL_SE_TAMPER_LEVEL_IGNORE)
      || (otp_init->tamper_levels[SL_SE_TAMPER_SIGNAL_RESERVED_3] != SL_SE_TAMPER_LEVEL_IGNORE)
      || (otp_init->tamper_levels[SL_SE_TAMPER_SIGNAL_RESERVED_4] != SL_SE_TAMPER_LEVEL_IGNORE)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Combine tamper levels, two per byte
  for (size_t i = 0; i < SL_SE_TAMPER_SIGNAL_NUM_SIGNALS; i += 2) {
    // Check for reserved levels
    for (size_t offset = 0; offset < 2; ++offset) {
      switch (otp_init->tamper_levels[i + offset]) {
        case SL_SE_TAMPER_LEVEL_IGNORE:
        case SL_SE_TAMPER_LEVEL_INTERRUPT:
        case SL_SE_TAMPER_LEVEL_FILTER:
        case SL_SE_TAMPER_LEVEL_RESET:
        case SL_SE_TAMPER_LEVEL_PERMANENTLY_ERASE_OTP:
          break;
        default:
          return SL_STATUS_INVALID_PARAMETER;
      }
    }

    otp_tamper_settings.levels[i / 2] = (otp_init->tamper_levels[i] & 0x7)
                                        | ((otp_init->tamper_levels[i + 1] & 0x7) << 4);
  }
  // Limit period and threshold input
  otp_tamper_settings.period = otp_init->tamper_filter_period & 0x1f;
  otp_tamper_settings.threshold = otp_init->tamper_filter_threshold & 0x7;

  otp_tamper_settings.flags = otp_init->tamper_flags;
  otp_tamper_settings.reset_threshold = otp_init->tamper_reset_threshold;
  #else
  static struct otp_tamper_settings {
    uint8_t reserved1[16];
    uint8_t reserved2[2];
    uint8_t reserved3[2];
  } otp_tamper_settings = {
    { 0x00 },
    { 0xFF, 0xFF },
    { 0x00 }
  };
  #endif

  // Find parity word
  volatile uint32_t parity = 0;
  parity = parity ^ mcu_settings_flags;
  for (size_t i = 0; i < 5; i++) {
    parity = parity ^ ((uint32_t*)(&otp_tamper_settings))[i];
  }

  // SE command structures
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_INIT_OTP);

  volatile uint32_t parameters[2] = {
    parity,
    sizeof(mcu_settings_flags)
    + sizeof(otp_tamper_settings)
  };
  SE_DataTransfer_t parameters_data = SE_DATATRANSFER_DEFAULT(&parameters, 8);
  SE_addDataInput(se_cmd, &parameters_data);

  SE_DataTransfer_t mcu_settings_flags_data =
    SE_DATATRANSFER_DEFAULT((volatile void *)&mcu_settings_flags, sizeof(mcu_settings_flags));
  SE_addDataInput(se_cmd, &mcu_settings_flags_data);

  SE_DataTransfer_t tamper_settings_data =
    SE_DATATRANSFER_DEFAULT((volatile void *)&otp_tamper_settings, sizeof(otp_tamper_settings));
  SE_addDataInput(se_cmd, &tamper_settings_data);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Read SE OTP configuration.
 ******************************************************************************/
sl_status_t sl_se_read_otp(sl_se_command_context_t *cmd_ctx,
                           sl_se_otp_init_t *otp_settings)
{
  if (cmd_ctx == NULL || otp_settings == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  sl_status_t status;

  #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  volatile struct {
    uint32_t mcu_settings_flags;
    uint8_t levels[SL_SE_TAMPER_SIGNAL_NUM_SIGNALS / 2];
    uint8_t period;
    uint8_t threshold;
    uint8_t flags;
    uint8_t reset_threshold;
  } otp_raw;
  #else
  volatile struct {
    uint32_t mcu_settings_flags;
    uint8_t reserved1[16];
    uint8_t reserved2[2];
    uint8_t reserved3[2];
  } otp_raw;
  #endif

  // SE command structures
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_READ_OTP);

  SE_DataTransfer_t otp_raw_data =
    SE_DATATRANSFER_DEFAULT(&otp_raw, sizeof(otp_raw));
  SE_addDataOutput(se_cmd, &otp_raw_data);

  status = sli_se_execute_and_wait(cmd_ctx);

  if (status != SL_STATUS_OK) {
    return status;
  }

  otp_settings->enable_secure_boot =
    (otp_raw.mcu_settings_flags
     & SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_ENABLE);
  otp_settings->verify_secure_boot_certificate =
    (otp_raw.mcu_settings_flags
     & SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_VERIFY_CERTIFICATE);
  otp_settings->enable_anti_rollback =
    (otp_raw.mcu_settings_flags
     & SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_ANTI_ROLLBACK);
  otp_settings->secure_boot_page_lock_narrow =
    (otp_raw.mcu_settings_flags
     & SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_PAGE_LOCK_NARROW);
  otp_settings->secure_boot_page_lock_full =
    (otp_raw.mcu_settings_flags
     & SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_PAGE_LOCK_FULL);

  #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  // Split levels
  for (size_t i = 0; i < sizeof(otp_raw.levels); i++) {
    otp_settings->tamper_levels[2 * i] = (otp_raw.levels[i]) & 0x7;
    otp_settings->tamper_levels[2 * i + 1] = (otp_raw.levels[i] >> 4) & 0x7;
  }

  otp_settings->tamper_filter_period = otp_raw.period;
  otp_settings->tamper_filter_threshold = otp_raw.threshold;

  otp_settings->tamper_flags = otp_raw.flags;
  otp_settings->tamper_reset_threshold = otp_raw.reset_threshold;
  #endif

  return SL_STATUS_OK;
}

#elif defined(CRYPTOACC_PRESENT)

sl_status_t sl_se_init_otp(sl_se_command_context_t *cmd_ctx,
                           sl_se_otp_init_t *otp_init)
{
  if (cmd_ctx == NULL || otp_init == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  volatile uint32_t mcu_settings_flags = 0;

  if (otp_init->enable_secure_boot) {
    mcu_settings_flags |= SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_ENABLE;
  }
  if (otp_init->verify_secure_boot_certificate) {
    mcu_settings_flags |=
      SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_VERIFY_CERTIFICATE;
  }
  if (otp_init->enable_anti_rollback) {
    mcu_settings_flags |= SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_ANTI_ROLLBACK;
  }
  if (otp_init->secure_boot_page_lock_narrow) {
    mcu_settings_flags |= SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_PAGE_LOCK_NARROW;
  }
  if (otp_init->secure_boot_page_lock_full) {
    mcu_settings_flags |= SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_PAGE_LOCK_FULL;
  }

  // Find parity word
  uint32_t parity = 0;
  parity = parity ^ mcu_settings_flags;

  // SE command structures
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_INIT_OTP);

  volatile uint32_t parameters[2] = {
    parity,
    sizeof(mcu_settings_flags)
  };
  SE_DataTransfer_t parameters_data = SE_DATATRANSFER_DEFAULT(&parameters, 8);
  SE_addDataInput(se_cmd, &parameters_data);

  SE_DataTransfer_t mcu_settings_flags_data =
    SE_DATATRANSFER_DEFAULT(&mcu_settings_flags, sizeof(mcu_settings_flags));
  SE_addDataInput(se_cmd, &mcu_settings_flags_data);

  SE_executeCommand(se_cmd);
  return SL_STATUS_FAIL; // Should never get to this point
}

sl_status_t sl_se_read_otp(sl_se_command_context_t *cmd_ctx,
                           sl_se_otp_init_t *otp_settings)
{
  if (cmd_ctx == NULL || otp_settings == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Try to acquire SE lock
  sl_status_t status = sli_se_lock_acquire();
  if (status != SL_STATUS_OK) {
    return status;
  }

  uint32_t mcu_settings_flags = 0;
  SE_Response_t vse_mbx_status = SE_getConfigStatusBits(&mcu_settings_flags);

  // Release SE lock
  status = sli_se_lock_release();

  if (vse_mbx_status != SE_RESPONSE_OK) {
    return sli_se_to_sl_status(vse_mbx_status);
  } else if (status != SL_STATUS_OK) {
    return status;
  }

  otp_settings->enable_secure_boot =
    (mcu_settings_flags
     & (SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_ENABLE >> SL_SE_ROOT_CONFIG_MCU_SETTINGS_SHIFT));
  otp_settings->verify_secure_boot_certificate =
    (mcu_settings_flags
     & (SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_VERIFY_CERTIFICATE >> SL_SE_ROOT_CONFIG_MCU_SETTINGS_SHIFT));
  otp_settings->enable_anti_rollback =
    (mcu_settings_flags
     & (SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_ANTI_ROLLBACK >> SL_SE_ROOT_CONFIG_MCU_SETTINGS_SHIFT));
  otp_settings->secure_boot_page_lock_narrow =
    (mcu_settings_flags
     & (SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_PAGE_LOCK_NARROW >> SL_SE_ROOT_CONFIG_MCU_SETTINGS_SHIFT));
  otp_settings->secure_boot_page_lock_full =
    (mcu_settings_flags
     & (SE_OTP_MCU_SETTINGS_FLAG_SECURE_BOOT_PAGE_LOCK_FULL >> SL_SE_ROOT_CONFIG_MCU_SETTINGS_SHIFT));

  return SL_STATUS_OK;
}
#endif

#if defined(SEMAILBOX_PRESENT)

/***************************************************************************//**
 * Writes data to User Data section in MTP. Write data must be aligned to
 * word size and contain a number of bytes that is divisable by four.
 ******************************************************************************/
sl_status_t sl_se_write_user_data(sl_se_command_context_t *cmd_ctx,
                                  uint32_t offset,
                                  void *data,
                                  uint32_t num_bytes)
{
  if (cmd_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (data == NULL && num_bytes > 0UL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Setup SE command structures
  SE_Command_t *se_cmd = &cmd_ctx->command;
  SE_DataTransfer_t in_data = SE_DATATRANSFER_DEFAULT(data, num_bytes);

  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_WRITE_USER_DATA);
  SE_addDataInput(se_cmd, &in_data);

  SE_addParameter(se_cmd, offset);
  SE_addParameter(se_cmd, num_bytes);

  // Execute and wait
  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Erases User Data section in MTP.
 ******************************************************************************/
sl_status_t sl_se_erase_user_data(sl_se_command_context_t *cmd_ctx)
{
  if (cmd_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // SE command structures
  SE_Command_t *se_cmd = &cmd_ctx->command;
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_ERASE_USER_DATA);

  SE_addParameter(se_cmd, SLI_SE_COMMAND_OPTION_ERASE_UD);

  // Execute and wait.
  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Returns the current boot status, versions and system configuration.
 ******************************************************************************/
sl_status_t sl_se_get_status(sl_se_command_context_t *cmd_ctx,
                             sl_se_status_t *status)
{
  if (cmd_ctx == NULL || status == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  volatile uint32_t output[9] = { 0 };
  SE_Command_t *se_cmd = &cmd_ctx->command;

  // SE command structures
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_GET_STATUS);
  SE_DataTransfer_t out_data = SE_DATATRANSFER_DEFAULT(output, 4 * 9);

  SE_addDataOutput(se_cmd, &out_data);

  sl_status_t ret = sli_se_execute_and_wait(cmd_ctx);

  if (ret == SL_STATUS_OK) {
    // Tamper status
    status->tamper_status = output[0];
    status->tamper_status_raw = output[2];

    // Update status object
    status->boot_status = output[4];
    status->se_fw_version = output[5];
    status->host_fw_version = output[6];

    // Decode debug status
    decode_debug_status(&status->debug_status, output[7]);

    // Decode secure boot mode
    status->secure_boot_enabled =
      ((output[8] & 0x1U) && ((output[8] & ~0x1U) == 0));
  }

  return ret;
}

/***************************************************************************//**
 * Read the serial number of the SE module.
 ******************************************************************************/
sl_status_t sl_se_get_serialnumber(sl_se_command_context_t *cmd_ctx,
                                   void *serial)
{
  if (cmd_ctx == NULL || serial == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // SE command structures
  SE_Command_t *se_cmd = &cmd_ctx->command;
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_READ_SERIAL);
  SE_DataTransfer_t out_data = SE_DATATRANSFER_DEFAULT(serial, 16);

  SE_addDataOutput(se_cmd, &out_data);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Read the OTP firmware version of the SE module.
 ******************************************************************************/
sl_status_t sl_se_get_otp_version(sl_se_command_context_t *cmd_ctx,
                                  uint32_t *version)
{
  if (cmd_ctx == NULL || version == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // SE command structures
  SE_Command_t *se_cmd = &cmd_ctx->command;
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_STATUS_OTP_VERSION);
  SE_DataTransfer_t out_data = SE_DATATRANSFER_DEFAULT(version, sizeof(uint32_t));

  SE_addDataOutput(se_cmd, &out_data);

  return sli_se_execute_and_wait(cmd_ctx);
}

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
/***************************************************************************//**
 * Read the EMU->RSTCAUSE after a tamper reset. This function should be called
 * if EMU->RSTCAUSE has been cleared upon boot.
 ******************************************************************************/
sl_status_t sl_se_get_reset_cause(sl_se_command_context_t *cmd_ctx,
                                  uint32_t* reset_cause)
{
  if (cmd_ctx == NULL || reset_cause == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // SE command structures
  SE_Command_t *se_cmd = &cmd_ctx->command;
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_STATUS_READ_RSTCAUSE);
  SE_DataTransfer_t out_data =
    SE_DATATRANSFER_DEFAULT(reset_cause, sizeof(uint32_t));
  SE_addDataOutput(se_cmd, &out_data);
  return sli_se_execute_and_wait(cmd_ctx);
}
#endif // _SILICON_LABS_32B_SERIES_2_CONFIG_1

/***************************************************************************//**
 * Enables the secure debug functionality.
 ******************************************************************************/
sl_status_t sl_se_enable_secure_debug(sl_se_command_context_t *cmd_ctx)
{
  if (cmd_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_DBG_LOCK_ENABLE_SECURE);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Disables the secure debug functionality.
 ******************************************************************************/
sl_status_t sl_se_disable_secure_debug(sl_se_command_context_t *cmd_ctx)
{
  if (cmd_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_DBG_LOCK_DISABLE_SECURE);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Set options on the debug interface.
 ******************************************************************************/
sl_status_t sl_se_set_debug_options(sl_se_command_context_t *cmd_ctx,
                                    const sl_se_debug_options_t *debug_options)
{
  if (cmd_ctx == NULL || debug_options == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  uint32_t restriction_bits = 0x0;

  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_DBG_SET_RESTRICTIONS);

  /// Encode restricted debug options parameter.
  restriction_bits |= debug_options->non_secure_invasive_debug ? 0 : 1UL << 0;
  restriction_bits |= debug_options->non_secure_non_invasive_debug ? 0 : 1UL << 1;
  restriction_bits |= debug_options->secure_invasive_debug ? 0 : 1UL << 2;
  restriction_bits |= debug_options->secure_non_invasive_debug ? 0 : 1UL << 3;

  SE_addParameter(se_cmd, restriction_bits);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Performs a device mass erase and debug unlock.
 ******************************************************************************/
sl_status_t sl_se_erase_device(sl_se_command_context_t *cmd_ctx)
{
  if (cmd_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_DEVICE_ERASE);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Disabled device erase functionality.
 ******************************************************************************/
sl_status_t sl_se_disable_device_erase(sl_se_command_context_t *cmd_ctx)
{
  if (cmd_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_DEVICE_ERASE_DISABLE);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Request challenge from SE which can be used to open debug access.
 ******************************************************************************/
sl_status_t sl_se_get_challenge(sl_se_command_context_t *cmd_ctx,
                                sl_se_challenge_t challenge)
{
  if (cmd_ctx == NULL || challenge == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  SE_DataTransfer_t out_data =
    SE_DATATRANSFER_DEFAULT(challenge, sizeof(sl_se_challenge_t));

  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_GET_CHALLENGE);

  SE_addDataOutput(se_cmd, &out_data);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Invalidate current challenge and make a new challenge.
 ******************************************************************************/
sl_status_t sl_se_roll_challenge(sl_se_command_context_t *cmd_ctx)
{
  sl_se_challenge_t new_challenge;
  if (cmd_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_DataTransfer_t out_data =
    SE_DATATRANSFER_DEFAULT(new_challenge, sizeof(sl_se_challenge_t));

  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_ROLL_CHALLENGE);
  SE_addDataOutput(&cmd_ctx->command, &out_data);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Unlock debug access using certificate signed with challenge.
 ******************************************************************************/
sl_status_t sl_se_open_debug(sl_se_command_context_t *cmd_ctx,
                             void *cert, uint32_t len,
                             const sl_se_debug_options_t *debug_options)
{
  if (cmd_ctx == NULL || cert == NULL || debug_options == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  SE_DataTransfer_t in_data = SE_DATATRANSFER_DEFAULT(cert, len);
  uint32_t unlock_bits = 1UL << 1;  // Always request to unlock debug access port

  // SE command structures
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_OPEN_DEBUG);

  SE_addDataInput(se_cmd, &in_data);

  /** Encode parameter that holds debug options to unlock. */
  unlock_bits |= debug_options->non_secure_invasive_debug     ? 1UL << 2 : 0;
  unlock_bits |= debug_options->non_secure_non_invasive_debug ? 1UL << 3 : 0;
  unlock_bits |= debug_options->secure_invasive_debug         ? 1UL << 4 : 0;
  unlock_bits |= debug_options->secure_non_invasive_debug     ? 1UL << 5 : 0;

  SE_addParameter(se_cmd, unlock_bits);

  return sli_se_execute_and_wait(cmd_ctx);
}

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Temporarily disable tamper configuration using certificate signed with
 * challenge.
 ******************************************************************************/
sl_status_t sl_se_disable_tamper(sl_se_command_context_t *cmd_ctx,
                                 void *cert,
                                 uint32_t len,
                                 sl_se_tamper_signals_t tamper_signals)
{
  if (cmd_ctx == NULL || cert == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  SE_DataTransfer_t in_data = SE_DATATRANSFER_DEFAULT(cert, len);

  // SE command structures
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_DISABLE_TAMPER);

  SE_addDataInput(se_cmd, &in_data);

  SE_addParameter(se_cmd, tamper_signals);

  return sli_se_execute_and_wait(cmd_ctx);
}

#endif

/***************************************************************************//**
 * Read size of stored certificates in SE.
 ******************************************************************************/
sl_status_t sl_se_read_cert_size(sl_se_command_context_t *cmd_ctx,
                                 sl_se_cert_size_type_t *cert_size)
{
  if (cmd_ctx == NULL || cert_size == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  SE_Command_t *se_cmd = &cmd_ctx->command;

  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_READ_USER_CERT_SIZE);

  SE_DataTransfer_t out_data = SE_DATATRANSFER_DEFAULT(cert_size, 12UL);
  SE_addDataOutput(se_cmd, &out_data);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Read stored certificates in SE.
 ******************************************************************************/
sl_status_t sl_se_read_cert(sl_se_command_context_t *cmd_ctx,
                            sl_se_cert_type_t cert_type,
                            void *cert,
                            uint32_t num_bytes)
{
  if (cmd_ctx == NULL || cert == NULL || num_bytes == 0UL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  SE_Command_t *se_cmd = &cmd_ctx->command;
  uint32_t se_cert_type;

  switch (cert_type) {
    case SL_SE_CERT_BATCH:
      se_cert_type = SLI_SE_COMMAND_CERT_BATCH;
      break;

    case SL_SE_CERT_DEVICE_SE:
      se_cert_type = SLI_SE_COMMAND_CERT_SE;
      break;

    case SL_SE_CERT_DEVICE_HOST:
      se_cert_type = SLI_SE_COMMAND_CERT_HOST;
      break;

    default:
      return SL_STATUS_INVALID_PARAMETER;
      break;
  }

  // SE command structures
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_READ_USER_CERT | se_cert_type);

#if  _SILICON_LABS_32B_SERIES_2_CONFIG > 2
  // One parameter is required, but has no effect
  SE_addParameter(se_cmd, 0);
#endif //

  SE_DataTransfer_t out_data = SE_DATATRANSFER_DEFAULT(cert, num_bytes);
  SE_addDataOutput(se_cmd, &out_data);

  return sli_se_execute_and_wait(cmd_ctx);
}

#endif // defined(SEMAILBOX_PRESENT)

/// @} (end addtogroup sl_se)

#endif // defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
