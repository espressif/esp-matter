/***************************************************************************//**
 * @file app_se_manager_key_provisioning.c
 * @brief SE manager key provisioning functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_key_provisioning.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Command context
static sl_se_command_context_t cmd_ctx;

/// SE firmware version
static uint32_t version;

/// SE OTP configuration buffer
static sl_se_otp_init_t se_otp_conf_buf;

/// Pointer to AES or public key buffer
static uint8_t *key_buf_ptr;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get version data.
 ******************************************************************************/
uint32_t get_version(void)
{
  return(version);
}

/***************************************************************************//**
 * Get SE OTP configuration buffer pointer.
 ******************************************************************************/
sl_se_otp_init_t * get_se_otp_conf_buf_ptr(void)
{
  return(&se_otp_conf_buf);
}

/***************************************************************************//**
 * Set pointer to AES or public key buffer.
 ******************************************************************************/
void set_key_buf_ptr(uint8_t *ptr)
{
  key_buf_ptr = ptr;
}

/***************************************************************************//**
 * Initialize the SE Manager.
 ******************************************************************************/
sl_status_t init_se_manager(void)
{
  print_error_cycle(sl_se_init(), NULL);
}

/***************************************************************************//**
 * Deinitialize the SE Manager.
 ******************************************************************************/
sl_status_t deinit_se_manager(void)
{
  print_error_cycle(sl_se_deinit(), NULL);
}

/***************************************************************************//**
 * Get the SE firmware version.
 ******************************************************************************/
sl_status_t get_se_version(void)
{
  print_error_cycle(sl_se_get_se_version(&cmd_ctx, &version), &cmd_ctx);
}

/***************************************************************************//**
 * Get SE OTP configuration.
 ******************************************************************************/
sl_status_t get_se_otp_conf(void)
{
  print_error_cycle(sl_se_read_otp(&cmd_ctx, &se_otp_conf_buf), &cmd_ctx);
}

/***************************************************************************//**
 * Read public key from SE OTP.
 ******************************************************************************/
sl_status_t read_public_key(sl_se_device_key_type_t key_id)
{
  print_error_cycle(sl_se_read_pubkey(&cmd_ctx,
                                      key_id,
                                      key_buf_ptr,
                                      PUB_KEY_SIZE),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Program public key to the SE OTP.
 ******************************************************************************/
sl_status_t program_public_key(sl_se_device_key_type_t key_id)
{
  // Warning: This is a ONE-TIME command
  // The public key buffer must be word aligned
  print_error_cycle(sl_se_init_otp_key(&cmd_ctx,
                                       key_id,
                                       key_buf_ptr,
                                       PUB_KEY_SIZE),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Initialize SE OTP configuration.
 ******************************************************************************/
sl_status_t init_se_otp_conf(void)
{
  sl_se_otp_init_t otp_init = SL_SE_OTP_INIT_DEFAULT;

  // Overwrite secure boot options in SL_SE_OTP_INIT_DEFAULT if necessary.
  otp_init.enable_secure_boot = false;
  otp_init.verify_secure_boot_certificate = false;
  otp_init.enable_anti_rollback = false;
  otp_init.secure_boot_page_lock_narrow = false;
  otp_init.secure_boot_page_lock_full = false;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  // Overwrite tamper signal levels in SL_SE_OTP_INIT_DEFAULT if necessary.
  // It is not possible to degrade the default response level of a tamper
  // signal, so if a response is set to a lower level than the default response
  // level, this won't have any effect.
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_FILTER_COUNTER] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_WATCHDOG] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_RAM_CRC] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_HARDFAULT] =
    SL_SE_TAMPER_LEVEL_RESET;

  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_SOFTWARE_ASSERTION] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_SECURE_BOOT_FAILED] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_USER_SECURE_BOOT_FAILED] =
    SL_SE_TAMPER_LEVEL_IGNORE;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_MAILBOX_AUTHORIZATION_ERROR] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_DCI_AUTHORIZATION_ERROR] =
    SL_SE_TAMPER_LEVEL_IGNORE;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_FLASH_INTEGRITY_ERROR] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SELFTEST_FAILED] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_TRNG_MONITOR] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;

  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS0] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS1] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS2] = SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS3] = SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS4] = SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS5] = SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS6] = SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS7] = SL_SE_TAMPER_LEVEL_RESET;

  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_DECOUPLE_BOD] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_TEMPERATURE_SENSOR] =
    SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_VOLTAGE_GLITCH_FALLING] =
    SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_VOLTAGE_GLITCH_RISING] =
    SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SECURE_LOCK_ERROR] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_DEBUG_GRANTED] =
    SL_SE_TAMPER_LEVEL_IGNORE;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_DIGITAL_GLITCH] =
    SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_ICACHE_ERROR] =
    SL_SE_TAMPER_LEVEL_RESET;
#else
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_FILTER_COUNTER] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_WATCHDOG] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_RAM_ECC_2] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_HARDFAULT] =
    SL_SE_TAMPER_LEVEL_RESET;

  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_SOFTWARE_ASSERTION] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_SECURE_BOOT_FAILED] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_USER_SECURE_BOOT_FAILED] =
    SL_SE_TAMPER_LEVEL_IGNORE;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_MAILBOX_AUTHORIZATION_ERROR] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_DCI_AUTHORIZATION_ERROR] =
    SL_SE_TAMPER_LEVEL_IGNORE;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_FLASH_INTEGRITY_ERROR] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SELFTEST_FAILED] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_TRNG_MONITOR] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;

  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SECURE_LOCK_ERROR] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_ATAMPDET_EMPGD] =
    SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_ATAMPDET_SUPGD] =
    SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SE_ICACHE_ERROR] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_SE_RAM_ECC_1] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_BOD] =
    SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_TEMPERATURE_SENSOR] =
    SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_DPLL_LOCK_FAIL_LOW] =
    SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_DPLL_LOCK_FAIL_HIGH] =
    SL_SE_TAMPER_LEVEL_FILTER;

  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS0] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS1] =
    SL_SE_TAMPER_LEVEL_INTERRUPT;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS2] = SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS3] = SL_SE_TAMPER_LEVEL_FILTER;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS4] = SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS5] = SL_SE_TAMPER_LEVEL_RESET;
  otp_init.tamper_levels[SL_SE_TAMPER_SIGNAL_PRS6] = SL_SE_TAMPER_LEVEL_RESET;
#endif

  // Overwrite tamper filter options in SL_SE_OTP_INIT_DEFAULT if necessary.
  otp_init.tamper_filter_period = SL_SE_TAMPER_FILTER_PERIOD_33S;
  otp_init.tamper_filter_threshold = SL_SE_TAMPER_FILTER_THRESHOLD_4;

  // Overwrite tamper flags in SL_SE_OTP_INIT_DEFAULT if necessary.
  otp_init.tamper_flags = 0;

  // Overwrite reset threshold in SL_SE_OTP_INIT_DEFAULT if necessary.
  otp_init.tamper_reset_threshold = 5;
#endif

  // Commit OTP settings. This command is only available once!
  print_error_cycle(sl_se_init_otp(&cmd_ctx, &otp_init), &cmd_ctx);
}

#if !defined(CRYPTOACC_PRESENT)
/***************************************************************************//**
 * Program 128-bit AES key to the SE OTP.
 ******************************************************************************/
sl_status_t program_aes_key(void)
{
  // Warning: This is a ONE-TIME command
  // The AES key buffer must be word aligned
  print_error_cycle(sl_se_init_otp_key(&cmd_ctx,
                                       SL_SE_KEY_TYPE_IMMUTABLE_AES_128,
                                       key_buf_ptr,
                                       16),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Encrypt message with AES ECB.
 ******************************************************************************/
sl_status_t encrypt_aes_ecb(void)
{
  uint8_t plain_msg[16] = { 0 };

  // Set up a key descriptor for AES OTP key
  sl_se_key_descriptor_t aes_otp_key = SL_SE_APPLICATION_AES_128_KEY;

  print_error_cycle(sl_se_aes_crypt_ecb(&cmd_ctx,
                                        &aes_otp_key,
                                        SL_SE_ENCRYPT,
                                        sizeof(plain_msg),
                                        plain_msg,
                                        key_buf_ptr),
                    &cmd_ctx);
}
#endif

#if defined(CRYPTOACC_PRESENT)
/***************************************************************************//**
 * Get executed command from command context.
 ******************************************************************************/
uint32_t get_executed_command(void)
{
  return(cmd_ctx.command.command);
}

/***************************************************************************//**
 * Check if any command was executed.
 ******************************************************************************/
sl_status_t check_executed_command(void)
{
  // Keep command context to get the response
  sl_se_init_command_context(&cmd_ctx);

  // Read executed command if any
  print_error_cycle(sl_se_read_executed_command(&cmd_ctx), NULL);
}

/***************************************************************************//**
 * Get the executed command response.
 ******************************************************************************/
sl_status_t get_executed_command_response(uint8_t *buf, uint32_t buf_len)
{
  // Allocate buffer for reading the executed command response
  SE_DataTransfer_t out_buf = SE_DATATRANSFER_DEFAULT(buf, buf_len);

  // Set executed command and buffer to run sl_se_ack_command()
  SE_addDataOutput(&cmd_ctx.command, &out_buf);

  // Acknowledge and get status and output data of an executed command
  print_error_cycle(sl_se_ack_command(&cmd_ctx), NULL);
}
#endif

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
