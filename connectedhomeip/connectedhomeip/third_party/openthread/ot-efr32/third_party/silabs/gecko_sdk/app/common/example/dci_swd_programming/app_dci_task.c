/***************************************************************************//**
 * @file app_dci_task.c
 * @brief DCI task functions.
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
#include "app_dci_task.h"

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
/// 128-bit AES key
static const uint32_t aes_key[AES_KEY_SIZE / 4] = {
  0x1fe2a581, 0xf18652a1, 0x2c5c44df, 0x3ffa20c1
};

/// Public sign key
static const uint32_t public_sign_key[PUB_KEY_SIZE / 4] = {
  0xC64AAFC4, 0x1295AB9A, 0xA2F750DB, 0x80B4E56A,
  0x54D88311, 0xA529E717, 0xF474A96D, 0x2C568AE0,
  0xDE1960DE, 0x321341A9, 0x33741ADC, 0xB470D172,
  0x348A2336, 0x0E417C59, 0x4D0277A1, 0x19C80FE2
};

/// Public command key
static const uint32_t public_command_key[PUB_KEY_SIZE / 4] = {
  0x6F6FBCB1, 0xED4066A5, 0xE02E2B52, 0x7ECFB3F5,
  0x0BF6485D, 0x0D8F14E8, 0xF04084C0, 0xA4DCE1A4,
  0x9E11047C, 0x31BEA1D6, 0x5F7E70B7, 0x651A009D,
  0x0310059A, 0x931B5EE9, 0x7EC3056F, 0x63AD93A7
};

#if defined(DEVICE_XG21) || defined(DEVICE_XG23)
/// HSE-SVM user configuration
static const uint32_t hse_svm_conf[HSE_USER_CONF_SIZE] = {
  0x00000018,                                   // 24 bytes data below
  SECURE_BOOT_ENABLE_MASK + ANTI_ROLLBACK_MASK, // MCU settings
  0x00000000,                                   // 20 bytes reserved data
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000
};
#endif

#if defined(DEVICE_XG21)
/// HSE-SVH (xG21B) user configuration
static const uint32_t hse_svh_xg21b_conf[HSE_USER_CONF_SIZE] = {
  0x00000018,                                   // 24 bytes data below
  SECURE_BOOT_ENABLE_MASK + ANTI_ROLLBACK_MASK, // MCU settings
  0x40440410,                                   // Tamper settings
  0x14040104,
  0x77442211,
  0x42042224,
  0x0500060A
};
#endif

#if defined(DEVICE_XG23)
/// Other HSE-SVH user configuration
static const uint32_t hse_svh_other_conf[HSE_USER_CONF_SIZE] = {
  0x00000018,                                   // 24 bytes data below
  SECURE_BOOT_ENABLE_MASK + ANTI_ROLLBACK_MASK, // MCU settings
  0x40440410,                                   // Tamper settings
  0x14040104,
  0x22414224,
  0x74422112,
  0x0500060A
};
#endif

#if defined(DEVICE_XG22)
/// VSE-SVM user configuration
static const uint32_t vse_svm_conf[VSE_USER_CONF_SIZE] = {
  0x00000004,                                   // 4 bytes data below
  SECURE_BOOT_ENABLE_MASK + ANTI_ROLLBACK_MASK  // MCU settings
};
#endif

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Set up buffer to implement DCI command.
 ******************************************************************************/
void set_dci_command(uint32_t *cmd_buf)
{
  uint32_t i;

  // DCI task in cmd_resp_buf[0]
  switch (*cmd_buf) {
    case GET_SE_STATUS:
      *cmd_buf = NO_ARGUMENT_LENGTH;
      *(++cmd_buf) = COMMAND_GET_SE_STATUS;
      break;

    case READ_USER_CONFIG:
      *cmd_buf = NO_ARGUMENT_LENGTH;
      *(++cmd_buf) = COMMAND_READ_OTP;
      break;

    case READ_SERIAL_NUMBER:
      *cmd_buf = NO_ARGUMENT_LENGTH;
      *(++cmd_buf) = COMMAND_READ_SERIAL;
      break;

    case READ_PUB_SIGN_KEY:
      *cmd_buf = NO_ARGUMENT_LENGTH;
      *(++cmd_buf) = COMMAND_READ_PUB_SIGN_KEY;
      break;

    case READ_PUB_CMD_KEY:
      *cmd_buf = NO_ARGUMENT_LENGTH;
      *(++cmd_buf) = COMMAND_READ_PUB_CMD_KEY;
      break;

    case ENABLE_SECURE_DEBUG:
      *cmd_buf = NO_ARGUMENT_LENGTH;
      *(++cmd_buf) = COMMAND_DBG_LOCK_ENABLE_SECURE;
      break;

    case DISABLE_SECURE_DEBUG:
      *cmd_buf = NO_ARGUMENT_LENGTH;
      *(++cmd_buf) = COMMAND_DBG_LOCK_DISABLE_SECURE;
      break;

    case LOCK_DEVICE:
      *cmd_buf = NO_ARGUMENT_LENGTH;
      *(++cmd_buf) = COMMAND_DBG_LOCK_APPLY;
      break;

    case ERASE_DEVICE:
    case RECOVER_DEVICE:
      *cmd_buf = NO_ARGUMENT_LENGTH;
      *(++cmd_buf) = COMMAND_DEVICE_ERASE;
      break;

    case CHECK_SE_FIRMWARE:
      *cmd_buf = UPGRADE_SE_FIRMWARE_LENGTH;
      *(++cmd_buf) = COMMAND_CHECK_SE_IMAGE;
      *(++cmd_buf) = SE_START_ADDR + get_flash_start_addr();
      break;

    case APPLY_SE_FIRMWARE:
      *cmd_buf = UPGRADE_SE_FIRMWARE_LENGTH;
      *(++cmd_buf) = COMMAND_APPLY_SE_IMAGE;
      *(++cmd_buf) = SE_START_ADDR + get_flash_start_addr();
      break;

    case INIT_AES_KEY_HSE:
      *cmd_buf = INIT_AES_KEY_LENGTH;
      *(++cmd_buf) = COMMAND_INIT_AES_128_KEY;
      // Calculate parity of AES-128 key
      *(++cmd_buf) = 0;
      for (i = 0; i < AES_KEY_SIZE / 4; i++) {
        *cmd_buf ^= aes_key[i];
      }
      // Copy AES-128 key to buffer
      memcpy((uint32_t *)(++cmd_buf), (uint32_t *)aes_key,
             AES_KEY_SIZE);
      break;

    case INIT_PUB_SIGN_KEY:
      *cmd_buf = INIT_PUB_KEY_LENGTH;
      *(++cmd_buf) = COMMAND_INIT_PUB_SIGN_KEY;
      // Calculate parity of public sign key
      *(++cmd_buf) = 0;
      for (i = 0; i < PUB_KEY_SIZE / 4; i++) {
        *cmd_buf ^= public_sign_key[i];
      }
      // Copy public sign key to buffer
      memcpy((uint32_t *)(++cmd_buf), (uint32_t *)public_sign_key,
             PUB_KEY_SIZE);
      break;

    case INIT_PUB_CMD_KEY:
      *cmd_buf = INIT_PUB_KEY_LENGTH;
      *(++cmd_buf) = COMMAND_INIT_PUB_CMD_KEY;
      // Calculate parity of public command key
      *(++cmd_buf) = 0;
      for (i = 0; i < PUB_KEY_SIZE / 4; i++) {
        *cmd_buf ^= public_command_key[i];
      }
      // Copy public command key to buffer
      memcpy((uint32_t *)(++cmd_buf), (uint32_t *)public_command_key,
             PUB_KEY_SIZE);
      break;

    case INIT_SE_OTP:
      // Check device family
      switch (*(cmd_buf + 1)) {
#if defined(DEVICE_XG21)
        case DEVICE_XG21:
          // Check SVM or SVH
          if (*(cmd_buf + 2) == 'A') {
            *cmd_buf = INIT_HSE_OTP_LENGTH;
            *(++cmd_buf) = COMMAND_INIT_OTP;
            // Calculate parity of OTP configuration
            *(++cmd_buf) = 0;
            for (i = 1; i < HSE_USER_CONF_SIZE; i++) {
              *cmd_buf ^= hse_svm_conf[i];
            }
            // Copy OTP configuration to buffer
            memcpy((uint32_t *)(++cmd_buf), (uint32_t *)hse_svm_conf,
                   INIT_HSE_OTP_LENGTH);
          } else if (*(cmd_buf + 2) == 'B') {
            *cmd_buf = INIT_HSE_OTP_LENGTH;
            *(++cmd_buf) = COMMAND_INIT_OTP;
            // Calculate parity of OTP configuration
            *(++cmd_buf) = 0;
            for (i = 1; i < HSE_USER_CONF_SIZE; i++) {
              *cmd_buf ^= hse_svh_xg21b_conf[i];
            }
            // Copy OTP configuration to buffer
            memcpy((uint32_t *)(++cmd_buf), (uint32_t *)hse_svh_xg21b_conf,
                   INIT_HSE_OTP_LENGTH);
          } else {
            RAISE(SWD_ERROR_UNKNOWN_DEVICE);
          }
          break;
#endif

#if defined(DEVICE_XG22)
        case DEVICE_XG22:
          *cmd_buf = INIT_VSE_OTP_LENGTH;
          *(++cmd_buf) = COMMAND_INIT_OTP;
          // Parity of OTP configuration
          *(++cmd_buf) = vse_svm_conf[1];
          // Copy OTP configuration to buffer
          memcpy((uint32_t *)(++cmd_buf), (uint32_t *)vse_svm_conf,
                 INIT_VSE_OTP_LENGTH);
          break;
#endif

#if defined(DEVICE_XG23)
        case DEVICE_XG23:
          // Check SVM or SVH
          if (*(cmd_buf + 2) == 'A') {
            *cmd_buf = INIT_HSE_OTP_LENGTH;
            *(++cmd_buf) = COMMAND_INIT_OTP;
            // Calculate parity of OTP configuration
            *(++cmd_buf) = 0;
            for (i = 1; i < HSE_USER_CONF_SIZE; i++) {
              *cmd_buf ^= hse_svm_conf[i];
            }
            // Copy OTP configuration to buffer
            memcpy((uint32_t *)(++cmd_buf), (uint32_t *)hse_svm_conf,
                   INIT_HSE_OTP_LENGTH);
          } else if (*(cmd_buf + 2) == 'B') {
            *cmd_buf = INIT_HSE_OTP_LENGTH;
            *(++cmd_buf) = COMMAND_INIT_OTP;
            // Calculate parity of OTP configuration
            *(++cmd_buf) = 0;
            for (i = 1; i < HSE_USER_CONF_SIZE; i++) {
              *cmd_buf ^= hse_svh_other_conf[i];
            }
            // Copy OTP configuration to buffer
            memcpy((uint32_t *)(++cmd_buf), (uint32_t *)hse_svh_other_conf,
                   INIT_HSE_OTP_LENGTH);
          } else {
            RAISE(SWD_ERROR_UNKNOWN_DEVICE);
          }
          break;
#endif

        default:
          RAISE(SWD_ERROR_UNKNOWN_DEVICE);
          break;
      }
      break;

    case DISABLE_DEVICE_ERASE:
      *cmd_buf = NO_ARGUMENT_LENGTH;
      *(++cmd_buf) = COMMAND_DEVICE_ERASE_DISABLE;
      break;

    default:
      break;
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
